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

#include "EPMApplication.h"

// EPM
#include "EPMDefines.h"
#include "EPMWindow.h"
#include "EPMPreferences.h"

// QCommon
#include "QuitAppEvent.h"

// QT
#include <QDir>

QList<EPMWindow*> EPMApplication::_epmWindows;

EPMApplication::EPMApplication
(
	int& argc,
	char** argv
) :
	AlpacaApplication (argc, argv, kAppName, kAppVersion)
{
	_preferences.setAppName(kAppName.toLatin1(), kAppVersion.toLatin1());
	initialize(&_preferences);
}

EPMApplication* EPMApplication::epmAppInstance()
{
	return qobject_cast<EPMApplication*>(QCoreApplication::instance());
}

EPMWindow* EPMApplication::createEPMWindow()
{
	EPMWindow* result;

	result = new EPMWindow();
	result->show();

	_epmWindows.append(result);

	return result;
}

void EPMApplication::removeEPMWindow
(
	EPMWindow* empWindow
)
{
	_epmWindows.removeAll(empWindow);

	if (_epmWindows.count() == 0)
		EPMApplication::epmAppInstance()->quitEPM();
}

void EPMApplication::quitEPM()
{
	postEvent(this, new QuitAppEvent);
}

void EPMApplication::shutDown()
{
	auto epmWindow = _epmWindows.begin();
	while (epmWindow != _epmWindows.end())
	{
		(*epmWindow)->shutDown();

		delete (*epmWindow);
		epmWindow++;
	}

	_epmWindows.clear();

	quit();
}
