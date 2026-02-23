// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMScopeApplication.h"

//  EPMScope
#include "AboutDialog.h"
#include "EPMScopeDefines.h"

// QCommon
#include "QuitAppEvent.h"

QList<EPMScopeWindow*> EPMScopeApplication::_scopeWindows;

EPMScopeApplication:: EPMScopeApplication(int& argc, char** argv) :
	AlpacaApplication (argc, argv, kAppName, kAppVersion)
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
