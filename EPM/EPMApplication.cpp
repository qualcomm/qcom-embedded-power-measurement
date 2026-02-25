// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
