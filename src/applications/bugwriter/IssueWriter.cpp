// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "IssueWriter.h"

// Qt
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QSettings>
#include <QUrlQuery>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <wincred.h>
#   pragma comment(lib, "Advapi32")
#endif

// OAuth configuration
static const QString kClientId       = QStringLiteral("Ov23liFIS5RVUdz4qrUG");

// Repository
static const QString kOwner          = QStringLiteral("qualcomm");
static const QString kRepo           = QStringLiteral("qcom-embedded-power-measurement");

// API endpoints
static const QString kUrlDeviceCode  = QStringLiteral("https://github.com/login/device/code");
static const QString kUrlAccessToken = QStringLiteral("https://github.com/login/oauth/access_token");
static const QString kUrlUser        = QStringLiteral("https://api.github.com/user");
static const QString kUrlIssues      = QStringLiteral("https://api.github.com/repos/%1/%2/issues");


IssueWriter::IssueWriter(QObject* parent)
    : QObject(parent)
    , _nam(new QNetworkAccessManager(this))
{
    connect(&_pollTimer, &QTimer::timeout, this, &IssueWriter::pollForToken);

    _accessToken = loadToken();
    if (!_accessToken.isEmpty())
        fetchUsername();
}

bool IssueWriter::isAuthenticated() const
{
    return !_accessToken.isEmpty() && !_username.isEmpty();
}

bool IssueWriter::isAuthorValid(const QString& author)
{
    static QRegularExpression re(QStringLiteral("(slua_*)|(default-user)|\\d|(^[a-zA-Z]{0,2}$)"));
    return !re.match(author).hasMatch();
}

QString IssueWriter::username() const
{
    return _username;
}

void IssueWriter::authenticate()
{
    if (isAuthenticated())
    {
        emit authenticated(_username);
        return;
    }
    requestDeviceCode();
}

void IssueWriter::submitIssue(const QString& title, const QString& body)
{
    _pendingTitle    = title;
    _pendingBody     = body;
    _hasPendingIssue = true;

    if (isAuthenticated())
        postIssue();
    else
        authenticate();
}

void IssueWriter::logout()
{
    clearToken();
    _accessToken.clear();
    _username.clear();
    _pollTimer.stop();
    _hasPendingIssue = false;
}

void IssueWriter::requestDeviceCode()
{
    const QUrl url(kUrlDeviceCode);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Accept", "application/json");

    QUrlQuery params;
    params.addQueryItem(QStringLiteral("client_id"), kClientId);
    params.addQueryItem(QStringLiteral("scope"),     QStringLiteral("public_repo"));

    QNetworkReply* reply = _nam->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError)
        {
            emit errorOccurred(tr("Device flow request failed: %1").arg(reply->errorString()));
            return;
        }

        const QJsonObject json  = QJsonDocument::fromJson(reply->readAll()).object();
        _deviceCode             = json[QStringLiteral("device_code")].toString();
        const QString userCode  = json[QStringLiteral("user_code")].toString();
        const QString verifyUri = json[QStringLiteral("verification_uri")].toString();
        _pollInterval           = json[QStringLiteral("interval")].toInt(5);

        if (_deviceCode.isEmpty() || userCode.isEmpty())
        {
            emit errorOccurred(tr("Invalid device flow response."));
            return;
        }

        _pollTimer.setInterval(_pollInterval * 1000);
        _pollTimer.start();

        emit deviceFlowStarted(userCode, verifyUri);
    });
}

void IssueWriter::pollForToken()
{
    const QUrl url(kUrlAccessToken);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Accept", "application/json");

    QUrlQuery params;
    params.addQueryItem(QStringLiteral("client_id"),   kClientId);
    params.addQueryItem(QStringLiteral("device_code"), _deviceCode);
    params.addQueryItem(QStringLiteral("grant_type"),  QStringLiteral("urn:ietf:params:oauth:grant-type:device_code"));

    QNetworkReply* reply = _nam->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError)
        {
            _pollTimer.stop();
            emit errorOccurred(tr("Token poll failed: %1").arg(reply->errorString()));
            return;
        }

        const QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        const QString error    = json[QStringLiteral("error")].toString();

        if (error == QLatin1String("authorization_pending"))
            return;

        if (error == QLatin1String("slow_down"))
        {
            _pollInterval += 5;
            _pollTimer.setInterval(_pollInterval * 1000);
            return;
        }

        _pollTimer.stop();

        if (!error.isEmpty())
        {
            emit errorOccurred(tr("Authorization failed: %1").arg(error));
            return;
        }

        _accessToken = json[QStringLiteral("access_token")].toString();
        if (_accessToken.isEmpty())
        {
            emit errorOccurred(tr("No access token in response."));
            return;
        }

        saveToken(_accessToken);
        fetchUsername();
    });
}

void IssueWriter::fetchUsername()
{
    const QUrl url(kUrlUser);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QStringLiteral("Bearer %1").arg(_accessToken).toUtf8());
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");

    QNetworkReply* reply = _nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError)
        {
            // Token may be stale; clear it so the next submit re-authenticates
            clearToken();
            _accessToken.clear();
            return;
        }

        const QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        _username              = json[QStringLiteral("login")].toString();

        emit authenticated(_username);

        if (_hasPendingIssue)
            postIssue();
    });
}

void IssueWriter::postIssue()
{
    _hasPendingIssue = false;

    const QUrl url(kUrlIssues.arg(kOwner, kRepo));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QStringLiteral("Bearer %1").arg(_accessToken).toUtf8());
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    payload[QStringLiteral("title")] = _pendingTitle;
    payload[QStringLiteral("body")]  = _pendingBody;

    QNetworkReply* reply = _nam->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError)
        {
            emit errorOccurred(tr("Failed to submit issue: %1").arg(reply->errorString()));
            return;
        }

        const QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        const int issueNumber  = json[QStringLiteral("number")].toInt();
        const QString issueUrl = json[QStringLiteral("html_url")].toString();

        emit issueSubmitted(issueNumber, issueUrl);
    });
}

// ── Token storage ──────────────────────────────────────────────────────────────

void IssueWriter::saveToken(const QString& token)
{
#ifdef Q_OS_WIN
    const QByteArray tokenBytes = token.toUtf8();

    CREDENTIALW cred        = {};
    cred.Type               = CRED_TYPE_GENERIC;
    cred.TargetName         = const_cast<LPWSTR>(L"QEPM_Token");
    cred.CredentialBlobSize = static_cast<DWORD>(tokenBytes.size());
    cred.CredentialBlob     = reinterpret_cast<LPBYTE>(const_cast<char*>(tokenBytes.constData()));
    cred.Persist            = CRED_PERSIST_LOCAL_MACHINE;

    CredWriteW(&cred, 0);
#else
    QSettings settings(QStringLiteral("Qualcomm"), QStringLiteral("QEPM"));
    settings.setValue(QStringLiteral("IssueToken"), token);
#endif
}

QString IssueWriter::loadToken() const
{
#ifdef Q_OS_WIN
    PCREDENTIALW pCred = Q_NULLPTR;
    if (CredReadW(L"QEPM_Token", CRED_TYPE_GENERIC, 0, &pCred))
    {
        const QString token = QString::fromUtf8(
            reinterpret_cast<const char*>(pCred->CredentialBlob),
            static_cast<int>(pCred->CredentialBlobSize)
        );
        CredFree(pCred);
        return token;
    }
    return {};
#else
    QSettings settings(QStringLiteral("Qualcomm"), QStringLiteral("QEPM"));
    return settings.value(QStringLiteral("IssueToken")).toString();
#endif
}

void IssueWriter::clearToken()
{
#ifdef Q_OS_WIN
    CredDeleteW(L"QEPM_Token", CRED_TYPE_GENERIC, 0);
#else
    QSettings settings(QStringLiteral("Qualcomm"), QStringLiteral("QEPM"));
    settings.remove(QStringLiteral("IssueToken"));
#endif
}
