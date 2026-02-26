#ifndef ALPACADEFINES_H
#define ALPACADEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"
#include "StringProof.h"
#include "version.h"

#include <QString>

// Lime / Telematics
#ifdef INTERNAL_BUILD
const QString kProductName(QStringLiteral("Alpaca"));
const char kProductID[] =	"6cc4491c-f28b-11e8-be2b-06e1158024a8"; // Alpaca Internal
const char kCoreFeature[] = "31eb9adf-f28c-11e8-be2b-06e1158024a8";
#else
const QString kProductName(QStringLiteral("QEPM"));
const char kProductID[] =	"1e48f695-c109-11ec-aebb-063166a9270b"; // Alpaca External
const char kCoreFeature[] = "1e71efc7-c109-11ec-aebb-063166a9270b";
#endif

const QString kOrganizationName(QStringLiteral("Qualcomm, Inc."));
const QString kProductVersion(QStringLiteral(ALPACA_VERSION));
const QString kCompileDate(QStringLiteral(__DATE__));
const QString kCompileTime(QStringLiteral(__TIME__));
const QString kBuildTime(kCompileDate + QStringLiteral(" ") + kCompileTime);
const QString kVersionGUID(QStringLiteral("{6F01E0AB-1962-4054-C061-3CA7CA4397E0}")); // produced by GUID Maker

quint32 QCOMMONCONSOLE_EXPORT makeFirmwareVersion(quint32 hw, quint32 major, quint32 minor);

#endif // ALPACADEFINES_H
