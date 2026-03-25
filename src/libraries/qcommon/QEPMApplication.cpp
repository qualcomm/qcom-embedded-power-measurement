// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMApplication.h"
#include "AlpacaDefines.h"
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"
#include "HappinessDialog.h"
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

bool QEPMApplication::readyToRate()
{
	bool result{false};

	QSettings AlpacaSettings;

	bool rated = AlpacaSettings.value("rated", false).toBool();
	if (rated)
	{
		QDate lastRate = AlpacaSettings.value("lastRating", QDate()).toDate();
		if (lastRate.isValid())
		{
			lastRate = lastRate.addDays(30);

			if (lastRate <= QDate::currentDate())
				result = true;
		}
		else
		{
			AlpacaSettings.setValue("lastRating", QDate::currentDate());
		}
	}
	else
	{
		AlpacaSettings.setValue("rated", true);
		AlpacaSettings.setValue("lastRating", QDate::currentDate());
	}

	return result;
}

void QEPMApplication::showRateDialog()
{
	HappinessDialog happinessDialog(Q_NULLPTR);

	if (happinessDialog.exec() == QDialog::Accepted)
	{
		HappinessRating rating = happinessDialog.getRating();

		QByteArray ratingString = "Rating";

		double value{.0};

		switch (rating)
		{
		case eNotSet: break;
		case eDissatisfied: value = 1.0; break;
		case eUnhappy: value = 2.0; break;
		case eSatisfied: value = 3.0; break;
		case eHappy: value = 4.0; break;
		case eLove: value = 5.0; break;
		}

		_appCore->postMetric(ratingString, value);
	}

	QSettings AlpacaSettings;

	AlpacaSettings.setValue("lastRating", QDate::currentDate());
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
