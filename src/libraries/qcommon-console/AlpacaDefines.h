#ifndef ALPACADEFINES_H
#define ALPACADEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause


#include "QCommonConsoleGlobal.h"
#include "version.h"

#include <QString>

const QString kProductName(QStringLiteral("QEPM"));

const QString kOrganizationName(QStringLiteral("Qualcomm, Inc."));
const QString kProductVersion(QStringLiteral(QEPM_VERSION));
const QString kCompileDate(QStringLiteral(__DATE__));
const QString kCompileTime(QStringLiteral(__TIME__));
const QString kBuildTime(kCompileDate + QStringLiteral(" ") + kCompileTime);
const QString kVersionGUID(QStringLiteral("{6F01E0AB-1962-4054-C061-3CA7CA4397E0}")); // produced by GUID Maker

quint32 QCOMMONCONSOLE_EXPORT makeFirmwareVersion(quint32 hw, quint32 major, quint32 minor);

#endif // ALPACADEFINES_H
