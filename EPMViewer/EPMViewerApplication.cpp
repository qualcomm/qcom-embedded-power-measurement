// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMViewerApplication.h"

// EPMViewer
#include "EPMViewerDefines.h"

// QCommon
#include "AboutDialog.h"
#include "QuitAppEvent.h"

EPMViewerApplication::EPMViewerApplication
(
	int& argc,
	char** argv
) :
	AlpacaApplication (argc, argv, kAppName, kAppVersion)
{
	_preferences.setAppName(kAppName.toLatin1(), kAppVersion.toLatin1());

	initialize(&_preferences);
}

EPMViewerApplication* EPMViewerApplication::appInstance()
{
	return qobject_cast<EPMViewerApplication*>(QCoreApplication::instance());
}

EPMViewerWindow* EPMViewerApplication::createViewerWindow()
{
	EPMViewerWindow* result;

	result = new EPMViewerWindow(EPMViewerApplication::appInstance()->getPreferences());
	result->show();

	return result;
}

void EPMViewerApplication::showAboutDialog()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.exec();
}

void EPMViewerApplication::quitViewer()
{
	postEvent(this, new QuitAppEvent);
}

void EPMViewerApplication::on_preferencesChanged()
{
}

void EPMViewerApplication::shutDown()
{
	quit();
}
