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
const QString kQuitExcel(QStringLiteral("quitExcelOnExportEnd"));
const QString kUseCSV(QStringLiteral("useCSV"));
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
	_quitExcelOnFinish = settings.value(kQuitExcel, defaultQuitExcelOnFinish()).toBool();
	_useCSV = settings.value(kUseCSV, defaultUseCSV()).toBool();

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

bool UDASPreferences::defaultQuitExcelOnFinish()
{
	return false;
}

bool UDASPreferences::quitExcelOnFinish()
{
	return _quitExcelOnFinish;
}

void UDASPreferences::setQuitExcelOnFinish(bool quitState)
{
	_quitExcelOnFinish = quitState;
}

void UDASPreferences::saveQuitExcelOnFinish
(
	bool quitState
)
{
	setQuitExcelOnFinish(quitState);

	QByteArray applicationName = EPMPreferences::appName();
	AlpacaSettings settings(applicationName);

	settings.beginGroup(kPreferences);
	settings.setValue(kQuitExcel, quitState);
	settings.endGroup();
}

bool UDASPreferences::defaultUseCSV()
{
	return false;
}

bool UDASPreferences::useCSV()
{
	return _useCSV;
}

void UDASPreferences::setUseCSV(bool saveCSVState)
{
	_useCSV = saveCSVState;
}

void UDASPreferences::saveUseCSV(bool saveCSVState)
{
	setUseCSV(saveCSVState);

	QByteArray applicationName = EPMPreferences::appName();
	AlpacaSettings settings(applicationName);

	settings.beginGroup(kPreferences);
	settings.setValue(kUseCSV, _useCSV);
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
