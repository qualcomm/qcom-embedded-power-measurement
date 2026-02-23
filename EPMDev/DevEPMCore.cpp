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

#include "DevEPMCore.h"


// libEPM
#include "EPMDevice.h"
#include "EPMProject.h"
#include "EPMPreferences.h"

// QCommon
#include "AppCore.h"
#include "Range.h"
#include "RangedContainer.h"

// QT
#include <QMap>
#include <QPair>
#include <QVector>

bool DevEPMCore::initialize
(
	const QByteArray& appName,
	const QByteArray& appVersion
)
{
	if (_initialized == false)
	{
		AppCore* appCore = AppCore::getAppCore();
		_preferences.setAppName(appName, appVersion);
		appCore->setPreferences(&_preferences);
		appCore->postAutomationEvent();
		_initialized = true;
	}

	return _initialized;
}

void DevEPMCore::writeToAppLog(const char *logText)
{
	AppCore::writeToApplicationLog(logText);
}

QString DevEPMCore::getPlatformConfigPath
(
		const QString &platform
		)
{
	QString result{platform};

	for (auto& platformItem: _platforms)
	{
		if (platform == platformItem.first)
		{
			result = platformItem.second;
			break;
		}
	}

	return result;
}

EPMDevice DevEPMCore::getEPMDevice(EPM_HANDLE epmHandle)
{
	EPMDevice result;

	if (_openDevices.find(epmHandle) != _openDevices.end())
	{
		result = _openDevices[epmHandle];
	}
	else
	{
		setLastError("Bad EPM Handle");
	}

	return result;
}

EPMProject DevEPMCore::getEPMProject(EPM_HANDLE epmHandle)
{
	EPMProject result;

	if (_EPMProjects.find(epmHandle) != _EPMProjects.end())
	{
		result = _EPMProjects[epmHandle];
	}
	else
	{
		setLastError("Bad EPM Handle");
	}

	return result;
}

unsigned long DevEPMCore::GetDeviceCount()
{
	_EPMDevice::updateDeviceList();
	_EPMDevice::getEPMDevices(_EPMDevices);

	return _EPMDevices.count();
}

const EPMDevices &DevEPMCore::GetEPMDevices()
{
	return _EPMDevices;
}

EPM_HANDLE DevEPMCore::OpenHandleByDescription(const char *targetName)
{
	EPM_HANDLE result{kBadHandle};

	for (auto [epmHandle, openDevice]: RangedContainer(_openDevices))
	{
		if (openDevice->getTargetName() == targetName)
			return epmHandle;

		if (openDevice->getSerialNumber().contains(targetName))
			return epmHandle;
	}

	EPMDevice epmDevice;

	epmDevice = _EPMDevice::deviceByName(targetName, _EPMDevices);
	if (epmDevice.isNull() == false)
	{
		result = epmDevice->hash();
		_openDevices[result] = epmDevice;
		_EPMProjects[result] = EPMProject(new _EPMProject(&_preferences));
	}
	else
	{
		setLastError(QByteArray(targetName) + " can't be opened.");
	}

	return result;
}

EPM_RESULT DevEPMCore::CloseEPMHandle(EPM_HANDLE epmHandle)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMDevice epmDevice = getEPMDevice(epmHandle);
	if (epmDevice.isNull() == false)
	{
		epmDevice->close();
		_openDevices.remove(epmHandle);
		_EPMProjects.remove(epmHandle);
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT DevEPMCore::GetPlatformCount
(
	EPM_HANDLE epmHandle,
	unsigned long* count
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	*count = 0;

	EPMProject epmProject = getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		_platforms.clear();

		unsigned long platformCount = epmProject->getPlatformCount();

		for (const auto& platformIndex: range(platformCount))
		{
			QString platformPath;
			QString platformName;

			epmProject->getPlatform(platformIndex, platformName, platformPath);

			_platforms.append(qMakePair(platformName.toLatin1(), platformPath.toLatin1()));
			*count = _platforms.count();
		}
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

Platforms DevEPMCore::GetPlatforms()
{
	Platforms result;

	int count = _platforms.count();
	for (auto index: range(count))
	{
		result.push_back(_platforms.at(index));
	}
	return result;
}

