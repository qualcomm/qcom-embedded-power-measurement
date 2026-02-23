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
*/

#include "EPMDev.h"

// libEPM
#include "EPMDevice.h"
#include "EPMProject.h"
#include "EPMPreferences.h"

// QCommon
#include "AlpacaSharedLibrary.h"

// QT
#include <QPair>
#include <QThread>
#include <QVector>

typedef QVector<QPair<QByteArray, QByteArray>> Platforms;
typedef QMap<EPM_HANDLE, EPMDevice> EPMDeviceMap;
typedef QMap<EPM_HANDLE, EPMProject> EPMProjectMap;

class DevEPMCore :
	public AlpacaSharedLibrary
{
public:
	DevEPMCore()
	{
	}

	~DevEPMCore()
	{
	}

	bool initialize(const QByteArray& appName, const QByteArray& appVersion);

	void writeToAppLog(const char* logText);

	QString getPlatformConfigPath(const QString& platform);
	EPMDevice getEPMDevice(EPM_HANDLE epmHandle);
	EPMProject getEPMProject(EPM_HANDLE epmHandle);

	unsigned long GetDeviceCount();
	const EPMDevices& GetEPMDevices();

	EPM_HANDLE OpenHandleByDescription(const char* targetName);
	EPM_RESULT CloseEPMHandle(EPM_HANDLE epmHandle);

	EPM_RESULT GetPlatformCount(EPM_HANDLE epmHandle, unsigned long* count);
	Platforms GetPlatforms();

private:
	bool						_initialized{false};
	EPMPreferences				_preferences;

	Platforms					_platforms;
	EPMDevices					_EPMDevices;

	EPMDeviceMap				_openDevices;
	EPMProjectMap				_EPMProjects;
} ;

