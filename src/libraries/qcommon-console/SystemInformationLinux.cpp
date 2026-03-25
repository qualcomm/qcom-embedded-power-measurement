// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SystemInformation.h"
#include "SystemUsername.h"

#include <QFile>
#include <QProcess>
#include <QSysInfo>
#include <QTextStream>

const QString kDriverNotFound("Driver not found");

SystemInformation::SystemInformation()
{
}

QString SystemInformation::computerName()
{
    if (_computerName.isEmpty())
        _computerName = QSysInfo::machineHostName();
    return _computerName;
}

QString SystemInformation::userName()
{
    if (_userName.isEmpty()) {
        SystemUsername userInfo;
        _userName = userInfo.userName();
    }
    return _userName;
}

QString SystemInformation::osName()
{
    if (_osName.isEmpty()) {
        _osName    = QSysInfo::productType();
        _osVersion = QSysInfo::productVersion();
    }
    return _osName;
}

QString SystemInformation::osVersion()
{
    if (_osVersion.isEmpty()) {
        _osName    = QSysInfo::productType();
        _osVersion = QSysInfo::productVersion();
    }
    return _osVersion;
}

QString SystemInformation::driverVersion()
{
    QProcess process;
    process.start("modinfo", QStringList() << "ftdi_sio");
    process.waitForFinished();

    const QString output = process.readAllStandardOutput();
    const QRegularExpression re(QStringLiteral("^version:\\s+(\\S+)"),
                                QRegularExpression::MultilineOption);
    const QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch())
        return match.captured(1).trimmed();

    return kDriverNotFound;
}

static QString memInfoValue(const QString &key)
{
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QStringLiteral("Unknown");

    QTextStream in(&file);
    const QString prefix = key + QStringLiteral(":");
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.startsWith(prefix)) {
            const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                bool ok = false;
                const qreal kb = static_cast<qreal>(parts[1].toLongLong(&ok));
                if (ok)
                    return QString::number(kb / 1024.0 / 1024.0);
            }
        }
    }
    return QStringLiteral("Unknown");
}

QString SystemInformation::totalPhysicalMemory()
{
    if (_totalPhysicalMemory.isEmpty())
        _totalPhysicalMemory = memInfoValue(QStringLiteral("MemTotal"));
    return _totalPhysicalMemory;
}

QString SystemInformation::totalVirtualMemory()
{
    return memInfoValue(QStringLiteral("SwapTotal"));
}
