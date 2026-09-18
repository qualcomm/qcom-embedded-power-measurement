// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMApplication.h"
#include "AlpacaDefines.h"
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"
#include "QuitAppEvent.h"

// Qt
#include <QtConcurrentRun>
#include <QDateTime>
#include <QDir>
#include <QFont>
#include <QMessageBox>
#include <QTextStream>

QEPMApplication::QEPMApplication(int &argc, char **argv, const QString& appName, const QString& appVersion) :
	QApplication(argc, argv),
	_appName(appName),
	_appVersion(appVersion)
{
	_appCore = AppCore::getAppCore();

	setApplicationName(kProductName);
	setOrganizationName("Qualcomm, Inc.");
	setOrganizationDomain("www.qualcomm.com");

#ifdef Q_OS_LINUX
	QFont font("Tahoma", 8);
	QApplication::setFont(font);
#endif

	setupApplicationStyle();
}

QEPMApplication::~QEPMApplication()
{
	AppCore::writeToApplicationLogLine("QEPMApplication::~QEPMApplication()");

	kickIt();
}

bool QEPMApplication::initialize(PreferencesBase *preferencesBase)
{
	bool result{false};

	_appCore->setPreferences(preferencesBase);

	_appCore->postStartEvent();

	cleanupLogs();

	result = true;

	return result;
}

QEPMApplication* QEPMApplication::QEPMAppinstance()
{
	return qobject_cast<QEPMApplication*>(QCoreApplication::instance());
}

void QEPMApplication::cleanupLogs()
{
	PreferencesBase* preferences = AppCore::getAppCore()->getPreferences();
	if (preferences != Q_NULLPTR)
	{
		cleanIt(preferences->appLogPath());
		cleanIt(preferences->runLogPath());
	}
}

bool QEPMApplication::event(QEvent *e)
{
	QSettings AlpacaSettings;

	quint32 eventType = e->type();
	switch (eventType)
	{
	case kQuitAppEvent:
		AppCore::writeToApplicationLogLine(_appName + " received an application quit event.");
		shutDown();
		break;

	default:
		return QApplication::event(e);
	}

	return true;
}
