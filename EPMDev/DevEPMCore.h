// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

