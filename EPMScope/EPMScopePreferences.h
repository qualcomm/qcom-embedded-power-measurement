#ifndef  EPMScopePREFERENCES_H
#define  EPMScopePREFERENCES_H
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

// Author: msimpson

#include <QString>

#include "EPMPreferences.h"

class EPMScopePreferences :
	public EPMPreferences
{
public:
	EPMScopePreferences();

	virtual void setAppName(const QByteArray& appName, const QByteArray& appVersion);

	qreal defaultWindowDuration();
	qreal windowDuration();
	void setWindowDuration(qreal newWindowDuration);
	void saveWindowDuration(qreal newWindowDuration);

	bool defaultSaveData();
	bool saveRunData();
	void setSaveRunData(bool saveRunData);
	void saveSaveRunData(bool saveRunData);

	QString defaultLastPlatform();
	QString lastPlatform();
	void setLastPlatform(const QString& lastPlatform);
	void saveLastPlatform(const QString& lastPlatform);

	QString lastDevice();
	void saveLastDevice(const QString& lastDevice);

private:
	qreal						_windowDuration;
	bool						_saveRunData;
	QString						_lastPlatform;
	QString						_lastDevice;
};

#endif // PREFERENCE_H
