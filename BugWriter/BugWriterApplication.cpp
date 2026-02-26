// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "BugWriterApplication.h"

// BugWriter
#include "BugWriterDefines.h"


BugWriterApplication::BugWriterApplication
(
	int& argc,
	char **argv
) :
	AlpacaApplication(argc, argv, kAppName, kAppVersion)
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
