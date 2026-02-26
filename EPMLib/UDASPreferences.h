#ifndef UDASPREFERENCES_H
#define UDASPREFERENCES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// libEPM
#include "EPMPreferences.h"

// Qt
#include <QString>

class UDASPreferences :
	public EPMPreferences
{
public:
	UDASPreferences()
	{

	}

	virtual void setAppName(const QByteArray& appName, const QByteArray& appVersion);

	bool defaultExportSelectedItems();
	bool exportSelectedItems();
	void setExportSelectedItems(bool exportSelectedItemState);
	void saveExportSelectedItems(bool exportSelectedItemState);

	QString defaultExportLocation();
	QString exportLocation();
	void setExportLocation(const QString& exportLocation);
	void saveExportLocation(const QString& exportLocation);
	
	bool defaultQuitExcelOnFinish();
	bool quitExcelOnFinish();
	void setQuitExcelOnFinish(bool quitState);
	void saveQuitExcelOnFinish(bool quitState);

	bool defaultUseCSV();
	bool useCSV();
	void setUseCSV(bool saveCSVState);
	void saveUseCSV(bool saveCSVState);

	bool defaultUseTimespan();
	bool useTimespan();
	void setUseTimespan(bool useTimespan);
	void saveUseTimespan(bool useTimespan);

private:
	bool						_exportSelectedItems;
	QString						_exportLocation;
	bool						_quitExcelOnFinish;
	bool						_useCSV{false};
	bool						_timespan{false};
};

#endif // UDASPREFERENCES_H
