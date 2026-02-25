#ifndef BUGWRITERDEFINES_H
#define BUGWRITERDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include <QString>

// QCommon
#include "version.h"

const QString kAppName(QStringLiteral("BugWriter"));
const QString kAppVersion(BUG_WRITER_VERSION);

const QString kInitialString(kAppName + ", version " + kAppVersion);

#endif // BUGWRITERDEFINES_H
