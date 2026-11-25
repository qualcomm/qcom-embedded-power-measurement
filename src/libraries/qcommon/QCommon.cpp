// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommon.h"

#include "QCommonConsole.h"

// QCommon
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"

// QT
#include <QDebug>
#include <QtGlobal>

void InitializeQCommon()
{
	InitializeQCommonConsole();

	// GUI applications call this right after creating their application
	// object, so this is the one place that records where the install was
	// found. That makes a wrong path visible in the log of any install layout.
	logInstallLayout();
	qInfo().noquote() << "Install layout: documentation     =" << docsRoot();
}
