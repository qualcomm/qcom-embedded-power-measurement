// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "BugWriterApplication.h"

// BugWriter
#include "BugWriterDefines.h"

BugWriterApplication::BugWriterApplication
(
	int& argc,
	char **argv
) :
	QEPMApplication(argc, argv, kAppName, kAppVersion)
{
	_bugWriterPreferences.setAppName(kAppName.toLatin1(), kAppVersion.toLatin1());
	initialize(&_bugWriterPreferences);
}

BugWriterApplication* BugWriterApplication::appInstance()
{
	return qobject_cast<BugWriterApplication*>(QCoreApplication::instance());
}

BugWriterPreferences* BugWriterApplication::getPreferences()
{
	BugWriterApplication* app = BugWriterApplication::appInstance();
	return &(app->_bugWriterPreferences);
}

void BugWriterApplication::shutDown()
{
	quit();
}
