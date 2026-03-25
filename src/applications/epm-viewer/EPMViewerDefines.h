#ifndef ALPCAVIEWERDEFINES_H
#define ALPCAVIEWERDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "version.h"

// Qt
#include <QString>

const QString kAppName(QStringLiteral("EPMViewer"));
const QString kAppVersion(EPM_VIEWER_VERSION);
const QString kInitialString(kAppName + ", version " + kAppVersion);

#endif // ALPCAVIEWERDEFINES_H
