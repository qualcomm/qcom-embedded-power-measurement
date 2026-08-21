// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef ISSUEWRITER_H
#define ISSUEWRITER_H

// Qt
#include <QObject>
#include <QTimer>

class QNetworkAccessManager;

class IssueWriter : public QObject
{
    Q_OBJECT

public:
    explicit IssueWriter(QObject* parent = Q_NULLPTR);

    bool isAuthenticated() const;
    static bool isAuthorValid(const QString& author);

    QString username() const;

    void authenticate();
    void submitIssue(const QString& title, const QString& body);
    void logout();

signals:
    void deviceFlowStarted(const QString& userCode, const QString& verificationUri);
    void authenticated(const QString& username);
    void issueSubmitted(int issueNumber, const QString& issueUrl);
    void errorOccurred(const QString& message);

private:
    void requestDeviceCode();
    void pollForToken();
    void fetchUsername();
    void postIssue();

    void    saveToken(const QString& token);
    QString loadToken() const;
    void    clearToken();

    QNetworkAccessManager*  _nam{Q_NULLPTR};
    QTimer                  _pollTimer;

    QString _accessToken;
    QString _username;
    QString _deviceCode;
    int     _pollInterval{5};

    QString _pendingTitle;
    QString _pendingBody;
    bool    _hasPendingIssue{false};
};

#endif // ISSUEWRITER_H
