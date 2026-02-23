#ifndef UDASPREFERENCES_H
#define UDASPREFERENCES_H
// Confidential and Proprietary â€“ Qualcomm Technologies, Inc.

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
// Copyright Â©2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
