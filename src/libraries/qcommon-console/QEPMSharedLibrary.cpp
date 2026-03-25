// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause


#include "QEPMSharedLibrary.h"

// QCommon
#include "QCommonConsole.h"

// Qt
#include <QDateTime>
#include <QDir>

#include <QTextStream>

AppCore* QEPMSharedLibrary::_appCore{Q_NULLPTR};

QEPMSharedLibrary::QEPMSharedLibrary()
{
}

QEPMSharedLibrary::~QEPMSharedLibrary()
{
}

AppCore* QEPMSharedLibrary::getAppCore()
{
	if (QEPMSharedLibrary::_appCore == Q_NULLPTR)
	{
		QEPMSharedLibrary::_appCore = AppCore::getAppCore();
	}

	return QEPMSharedLibrary::_appCore;
}

bool QEPMSharedLibrary::initialize
(
	const QByteArray &appName,
	const QByteArray &appVersion,
	PreferencesBase* preferencesBase
)
{
	bool result{false};

	_appName = appName;
	_appVersion = appVersion;

	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		appCore->setPreferences(preferencesBase);

		InitializeQCommonConsole();
	}

	return result;
}

bool QEPMSharedLibrary::licenseIsValid()
{
	/* let me know when LIME works
		if (_validLicense == false)
			_validLicense = _appCore.checkLicense(kProductID, kCoreFeature);

		*/

	return _validLicense;
}

void QEPMSharedLibrary::setLoggingState(bool state)
{
	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		appCore->setAppLogging(state);
	}
}

bool QEPMSharedLibrary::getLoggingState()
{
	bool result{false};

	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		result = appCore->appLoggingActive();
	}

	return result;
}
