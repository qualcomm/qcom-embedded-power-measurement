// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "DevUDASCore.h"

DevUDASCore::DevUDASCore()
{
}

DevUDASCore::~DevUDASCore()
{
}

bool DevUDASCore::initialize(const QByteArray &appName, const QByteArray &appVersion)
{
	if (_initialized == false)
	{
		AppCore* appCore = AppCore::getAppCore();

		_preferences.setAppName(appName, appVersion);
		appCore->setPreferences(&_preferences);

		_initialized = true;
	}

	return _initialized;

}

void DevUDASCore::setLoggingState(bool state)
{
	AppCore* appCore = DevUDASCore::appCore();
	if (appCore != Q_NULLPTR)
	{
		appCore->setAppLogging(state);
	}
}

bool DevUDASCore::getLoggingState()
{
	bool result{false};

	AppCore* appCore = DevUDASCore::appCore();
	if (appCore != Q_NULLPTR)
	{
		result = appCore->appLoggingActive();
	}

	return result;
}

void DevUDASCore::setExportLocation(const QString& exportLocation)
{
	if(exportLocation.isNull() == false)
		_exportLocation = exportLocation;
}

QString DevUDASCore::exportLocation()
{
	if(_exportLocation.isNull())
		_exportLocation = _preferences.defaultExportLocation();

	return _exportLocation;
}

AppCore *DevUDASCore::appCore()
{
	return AppCore::getAppCore();
}

