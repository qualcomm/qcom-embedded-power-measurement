// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMScopeApplication.h"

//  EPMScope
#include "AboutDialog.h"
#include "EPMScopeDefines.h"

// QCommon
#include "QuitAppEvent.h"

QList<EPMScopeWindow*> EPMScopeApplication::_scopeWindows;

EPMScopeApplication:: EPMScopeApplication(int& argc, char** argv) :
	QEPMApplication (argc, argv, kAppName, kAppVersion)
{
	_preferences.setAppName(kAppName.toLatin1(), kAppVersion.toLatin1());
	initialize(&_preferences);
}

EPMScopeApplication* EPMScopeApplication::appInstance()
{
	return qobject_cast<EPMScopeApplication*>(QCoreApplication::instance());
}

EPMScopeWindow* EPMScopeApplication::createScopeWindow()
{
	EPMScopeWindow* result;

	result = new EPMScopeWindow(EPMScopeApplication::appInstance()->getPreferences());
	result->show();

	return result;
}

void  EPMScopeApplication::removeScopeWindow(EPMScopeWindow* scopeWindow)
{
	_scopeWindows.removeAll(scopeWindow);

	if (_scopeWindows.count() == 0)
		EPMScopeApplication::appInstance()->quitViewer();
}

void EPMScopeApplication::showAboutDialog()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.exec();
}

void EPMScopeApplication::quitViewer()
{
	postEvent(this, new QuitAppEvent);
}

void EPMScopeApplication::on__applicationTimer_timeout()
{
}

void EPMScopeApplication::shutDown()
{
	for (auto &scopeWindow: _scopeWindows)
	{
		scopeWindow->shutDown();

		delete (scopeWindow);
	}

	_scopeWindows.clear();

	quit();
}
