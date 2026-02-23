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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

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

