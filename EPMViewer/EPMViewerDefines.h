#ifndef ALPCAVIEWERDEFINES_H
#define ALPCAVIEWERDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "version.h"

// Qt
#include <QString>

const QString kAppName(QStringLiteral("EPMViewer"));
const QString kAppVersion(QStringLiteral(ALPACA_VIEWER_VERSION));
const QString kInitialString(kAppName + ", version " + kAppVersion);

#endif // ALPCAVIEWERDEFINES_H
