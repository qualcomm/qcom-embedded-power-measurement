// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "UDASPreferences.h"

// QCommon
#include "AppCore.h"
#include "AlpacaSettings.h"

// QCommonConsole
#include "ConsoleApplicationEnhancements.h"

// Qt
#include <QDir>

const QString kLogAppendEnabled(QStringLiteral("logAppend"));
const QString kExportSelectedItemsOnly(QStringLiteral("exportSelectedItems"));
const QString kLastExportDir(QStringLiteral("lastExportDir"));
const QString kUseTimeSpan(QStringLiteral("useTimeSpan"));

void UDASPreferences::setAppName
(
	const QByteArray& appName,
	const QByteArray& appVersion
)
{
	EPMPreferences::setAppName(appName, appVersion);

	AlpacaSettings settings(appName);

	settings.beginGroup(kPreferences);

	_exportSelectedItems = settings.value(kExportSelectedItemsOnly, defaultExportSelectedItems()).toBool();
	_exportLocation = settings.value(kLastExportDir, defaultExportLocation()).toString();

	settings.endGroup();
}

bool UDASPreferences::defaultExportSelectedItems()
{
	return true;
}

bool UDASPreferences::exportSelectedItems()
{
	return _exportSelectedItems;
}

void UDASPreferences::setExportSelectedItems
(
	bool exportSelectedItemState
)
{
	_exportSelectedItems = exportSelectedItemState;
}

void UDASPreferences::saveExportSelectedItems
(
	bool exportSelectedItemState
)
{
	setExportSelectedItems(exportSelectedItemState);

	QByteArray applicationName = EPMPreferences::appName();
	AlpacaSettings settings(applicationName);

	settings.beginGroup(kPreferences);
	settings.setValue(kExportSelectedItemsOnly, _exportSelectedItems);
	settings.endGroup();
}

QString UDASPreferences::defaultExportLocation()
{
	return documentsDataPath("EPMViewer/Exports");
}

QString UDASPreferences::exportLocation()
{
	return _exportLocation;
}

void UDASPreferences::setExportLocation
(
	const QString& exportLocation
)
{
	_exportLocation = killOneDrive(exportLocation, defaultExportLocation());
}

void UDASPreferences::saveExportLocation
(
	const QString& exportLocation
)
{
	setExportLocation(exportLocation);

	QByteArray applicationName = EPMPreferences::appName();
	AlpacaSettings settings(applicationName);

	settings.beginGroup(kPreferences);
	settings.setValue(kLastExportDir, _exportLocation);
	settings.endGroup();
}

bool UDASPreferences::defaultUseTimespan()
{
	return false;
}

bool UDASPreferences::useTimespan()
{
	return _timespan;
}

void UDASPreferences::setUseTimespan
(
	bool useTimespan
)
{
	_timespan = useTimespan;
}

void UDASPreferences::saveUseTimespan
(
	bool useTimespan
)
{
	setUseTimespan(useTimespan);

	QByteArray applicationName = EPMPreferences::appName();
	AlpacaSettings settings(applicationName);

	settings.beginGroup(kPreferences);
	settings.setValue(kUseTimeSpan, _timespan);
	settings.endGroup();
}
