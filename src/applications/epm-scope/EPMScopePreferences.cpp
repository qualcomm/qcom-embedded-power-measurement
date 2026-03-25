// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

