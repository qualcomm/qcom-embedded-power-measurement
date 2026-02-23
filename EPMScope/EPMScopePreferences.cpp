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
// Copyright 2019-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMScopePreferences.h"

//  EPMScope
#include "EPMScopeDefines.h"

// QCommon
#include "AlpacaSettings.h"

// Qt
#include <QDir>
#include <QStringLiteral>

const QLatin1String kWindowDuration("WindowDuration");
const QLatin1String kSaveRunData("SaveRunData");
const QString kLastPlatform(QStringLiteral("lastPlatform"));
const QString kLastDevice(QStringLiteral("lastDevice"));

const QString kDefaultLogPath(QStringLiteral("/EPMScope/Logs"));

EPMScopePreferences::EPMScopePreferences()
{

}

void EPMScopePreferences::setAppName
(
	const QByteArray& appName,
	const QByteArray& appVersion
)
{
	EPMPreferences::setAppName(appName, appVersion);

	AlpacaSettings settings(appName);
	settings.beginGroup(kPreferences);

	_windowDuration = settings.value(kWindowDuration, defaultWindowDuration()).toReal();
	_saveRunData = settings.value(kSaveRunData, defaultSaveData()).toBool();;
	_lastPlatform = settings.value(kLastPlatform, defaultLastPlatform()).toString();
	_lastDevice = settings.value(kLastDevice, "").toString();

	settings.endGroup();
}

qreal EPMScopePreferences::defaultWindowDuration()
{
	return 10.;
}

qreal EPMScopePreferences::windowDuration()
{
	return _windowDuration;
}

void EPMScopePreferences::setWindowDuration(qreal newWindowDuration)
{
	_windowDuration = newWindowDuration;
}

void EPMScopePreferences::saveWindowDuration(qreal newWindowDuration)
{
	setWindowDuration(newWindowDuration);

	AlpacaSettings settings(kAppName);
	settings.beginGroup(kPreferences);
	settings.setValue(kWindowDuration, _windowDuration);
	settings.endGroup();
}

bool EPMScopePreferences::defaultSaveData()
{
	return true;
}

bool EPMScopePreferences::saveRunData()
{
	return _saveRunData;
}

void EPMScopePreferences::setSaveRunData
(
	bool saveRunData
)
{
	_saveRunData = saveRunData;
}

void EPMScopePreferences::saveSaveRunData
(
	bool saveRunData
)
{
	setSaveRunData(saveRunData);

	AlpacaSettings settings(kAppName);
	settings.beginGroup(kPreferences);
	settings.setValue(kSaveRunData, _saveRunData);
	settings.endGroup();
}

QString EPMScopePreferences::defaultLastPlatform()
{
	return "-";
}

QString EPMScopePreferences::lastPlatform()
{
	return _lastPlatform;
}

void EPMScopePreferences::setLastPlatform
(
	const QString& lastPlatform
)
{
	_lastPlatform = lastPlatform;
}

void EPMScopePreferences::saveLastPlatform
(
	const QString& lastPlatform
)
{
	setLastPlatform(lastPlatform);

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kLastPlatform, _lastPlatform);
	settings.endGroup();
}

QString EPMScopePreferences::lastDevice()
{
	return _lastDevice;
}

void EPMScopePreferences::saveLastDevice
(
	const QString& lastDevice
)
{
	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kLastDevice, lastDevice);
	settings.endGroup();
}

