#ifndef EPMDEFINES_H
#define EPMDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "version.h"

// QT
#include <QString>

const QString kAppName(QStringLiteral("EPM"));
const QString kAppVersion(QStringLiteral(EPM_VERSION));
const QString kInitialString(kAppName + ", version " + kAppVersion);

const QByteArray kPlatformPath(QByteArrayLiteral("EPMConfigPath"));
const QByteArray kRuntimeConfigurationsPath(QByteArrayLiteral("RuntimeConfigPath"));
const QByteArray kOutputPath(QByteArrayLiteral("OutputPath"));
const QByteArray kExportConfigurationsPath(QByteArrayLiteral("ExportConfigPath"));

#endif // EPMDEFINES_H
