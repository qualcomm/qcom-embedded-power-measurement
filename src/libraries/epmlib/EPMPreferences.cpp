// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMPreferences.h"

// TAC
#include "EPMDefines.h"

// QCommon
#include "ConsoleApplicationEnhancements.h"
#include "AlpacaSettings.h"
#include "AppCore.h"

// Qt
#include <QDir>

const QString kDefaultPlatformPath(epmConfigRoot());
const QString kDefaultRuntimeConfigPath(QStringLiteral("/Runtime Configurations"));
const QString kDefaultOutputPath(QStringLiteral("/Results"));
const QString kDefaultLogPath(QStringLiteral("/EPM/Logs"));
const QString kRunDuration(QStringLiteral("runDuration"));
const QString kLastPlatform(QStringLiteral("lastPlatform"));
const QString kDefaultExportConfigPath(QStringLiteral("/Export Configurations"));

void EPMPreferences::setAppName
(
	const QByteArray& appName,
	const QByteArray& appVersion
)
{
	PreferencesBase::setAppName(appName, appVersion);

	AlpacaSettings settings(appName);

	settings.beginGroup(kPreferences);

	_platformPath = killOneDrive(settings.value(kPlatformPath, defaultPlatformPath()).toString(), defaultPlatformPath());
	_runtimeConfigurationsPath = killOneDrive(settings.value(kRuntimeConfigurationsPath, defaultRuntimeConfigurationsPath()).toString(), defaultRuntimeConfigurationsPath());
	_outputPath = killOneDrive(settings.value(kOutputPath, defaultOutputPath()).toString(), defaultOutputPath());
	_lastPlatform = settings.value(kLastPlatform).toString();
	_runDuration  = settings.value(kRunDuration, defaultRunDuration()).toReal();

	settings.endGroup();
}

QString EPMPreferences::defaultPlatformPath()
{
	return QDir::cleanPath(kDefaultPlatformPath);
}

QString EPMPreferences::platformPath()
{
	//return _platformPath;

	return defaultPlatformPath();
}

void EPMPreferences::setPlatformPath
(
	const QString& platformPath
)
{
	_platformPath = killOneDrive(platformPath, defaultPlatformPath());
}

void EPMPreferences::savePlatformPath
(
	const QString& platformPath
)
{
	setPlatformPath(platformPath);

//	AlpacaSettings settings(kAppName);
//	settings.beginGroup(kPreferences);
//	settings.setValue(kPlatformPath, _platformPath);
//	settings.endGroup();
}

QString EPMPreferences::defaultRuntimeConfigurationsPath()
{
	return documentsDataPath(kDefaultRuntimeConfigPath);
}

QString EPMPreferences::runtimeConfigurationsPath()
{
	return _runtimeConfigurationsPath;
}

void EPMPreferences::setRuntimeConfigurationsPath
(
	const QString& runtimeConfigurationPath
)
{
	_runtimeConfigurationsPath = killOneDrive(runtimeConfigurationPath, defaultRuntimeConfigurationsPath());
}

void EPMPreferences::saveRuntimeConfigurationsPath
(
	const QString& runtimeConfigurationPath
)
{
	setRuntimeConfigurationsPath(runtimeConfigurationPath);

	AlpacaSettings settings(kAppName);
	settings.beginGroup(kPreferences);
	settings.setValue(kRuntimeConfigurationsPath, _runtimeConfigurationsPath);
	settings.endGroup();
}

QString EPMPreferences::defaultOutputPath()
{
	return documentsDataPath(kDefaultOutputPath);
}

QString EPMPreferences::outputPath()
{
	return _outputPath;
}

void EPMPreferences::setOutputPath
(
	const QString& outputPath
)
{
	_outputPath = killOneDrive(outputPath, defaultOutputPath());
}

void EPMPreferences::saveOutputPath
(
	const QString& outputPath
)
{
	setOutputPath(outputPath);

	AlpacaSettings settings(kAppName);
	settings.beginGroup(kPreferences);
	settings.setValue(kOutputPath, _outputPath);
	settings.endGroup();
}

QString EPMPreferences::lastPlatform()
{
	return _lastPlatform;
}

void EPMPreferences::setLastPlatform
(
	const QString& lastPlatform
)
{
	_lastPlatform = lastPlatform;
}

void EPMPreferences::saveLastPlatform
(
	const QString& lastPlatform
)
{
	setLastPlatform(lastPlatform);

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kLastPlatform, lastPlatform);
	settings.endGroup();
}

qreal EPMPreferences::defaultRunDuration()
{
	return 4.0;
}

qreal EPMPreferences::runDuration()
{
	return _runDuration;
}

void EPMPreferences::setRunDuration(qreal runDuration)
{
	_runDuration = runDuration;
}

void EPMPreferences::saveRunDuration(qreal runDuration)
{
	setRunDuration(runDuration);

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kRunDuration, runDuration);
	settings.endGroup();
}

QString EPMPreferences::defaultExportConfigurationsPath()
{
	return documentsDataPath(kDefaultExportConfigPath);
}
