// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMDev.h"
#include "DevEPMCore.h"

#include "mymemcpy.h"

// libEPM
#include "EPMDefines.h"
#include "EPMDevice.h"
#include "EPMProject.h"

// QCommon
#include "version.h"

DevEPMCore gDevEPMCore;

EPMDEV_EXPORT EPM_RESULT InitializeEPMDev()
{
	EPM_RESULT result{NO_EPM_ERROR};
	if (gDevEPMCore.initialize(kAppName.toLatin1(), kAppVersion.toLatin1()) == false)
		result = EPM_INIT_FAILED;

	return result;
}

EPM_RESULT GetAlpacaVersion
(
	char* alpacaVersion,
	int bufferSize
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	QByteArray version{ALPACA_VERSION};

	if (version.size() < bufferSize)
	{
		my_memcpy(alpacaVersion, bufferSize, version.data(), version.size());
	}
	else
	{
		result = version.size();
	}

	return result;
}

EPM_RESULT GetEPMVersion(char* epmVersion, int bufferSize)
{
	EPM_RESULT result{NO_EPM_ERROR};

	QByteArray version{EPM_VERSION};

	if (version.size() < bufferSize)
	{
		my_memcpy(epmVersion, bufferSize, version.data(), version.size());
	}
	else
	{
		result = version.size();
	}

	return result;
}

EPM_RESULT GetLastEPMError
(
	char* lastError,
	int bufferSize
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	QByteArray lastErrorBA = gDevEPMCore.lastError();
	if (lastErrorBA.size() < bufferSize)
	{
		my_memcpy(lastError, bufferSize, lastErrorBA.data(), lastErrorBA.size());
	}
	else
	{
		result = EPM_BUFFER_TOO_SMALL;
	}

	return result;
}

EPM_RESULT GetDeviceCount(int *deviceCount)
{
	*deviceCount = gDevEPMCore.GetDeviceCount();

	return NO_EPM_ERROR;
}

unsigned long GetPortData
(
	int deviceIndex,
	char* portData,
	int bufferSize
)
{
	int result{0};

	const EPMDevices& epmDevices = gDevEPMCore.GetEPMDevices();

	if (deviceIndex < epmDevices.count())
	{
		EPMDevice epmDevice =  epmDevices.at(deviceIndex);
		QByteArray portData2;

		portData2 = epmDevice->getTargetName().toLatin1();
		portData2 += ";";
		portData2 += epmDevice->getSerialNumber().toLatin1();
		portData2 += ";";
		portData2 += epmDevice->getUUID().toLatin1();

		if (portData2.size() < bufferSize)
		{
			my_memcpy(portData, bufferSize, portData2.constData(), portData2.length());
			result = portData2.size();
		}
	}

	return result;
}

EPM_HANDLE OpenHandleByDescription
(
	const char* targetName
)
{
	return gDevEPMCore.OpenHandleByDescription(targetName);
}

EPM_RESULT CloseEPMHandle(EPM_HANDLE epmHandle)
{
	return gDevEPMCore.CloseEPMHandle(epmHandle);
}

EPM_RESULT GetUUID
(
	EPM_HANDLE epmHandle,
	char* uuid,
	int bufferSize
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMDevice epmDevice = gDevEPMCore.getEPMDevice(epmHandle);
	if (epmDevice.isNull() == false)
	{
		QByteArray uuidStr = epmDevice->getUUID().toLatin1();

		if (uuidStr.size() < bufferSize)
		{
			my_memcpy(uuid, bufferSize, uuidStr.data(), uuidStr.size());
		}
		else
		{
			result = EPM_BUFFER_TOO_SMALL;
		}
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT GetPlatformPath
(
	EPM_HANDLE epmHandle,
	char* platformPath,
	int bufferSize
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		QByteArray path =  epmProject->platformPath().toLatin1();

		if (path.size() < bufferSize)
		{
			my_memcpy(platformPath, bufferSize, path.data(), path.size());
		}
		else
		{
			result = EPM_BUFFER_TOO_SMALL;
		}
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT SetPlatformPath
(
	EPM_HANDLE epmHandle,
	const char* newPath
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject = gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		if (epmProject->setPlatformPath(QString(newPath)) == false)
			result = EPM_BAD_PATH;
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT GetPlatformCount
(
	EPM_HANDLE epmHandle,
	int* count
)
{
	EPM_RESULT result{NO_EPM_ERROR};
	unsigned long longCount;

	result = gDevEPMCore.GetPlatformCount(epmHandle, &longCount);
	*count = longCount;

	return result;
}

EPM_RESULT GetPlatform
(
	EPM_HANDLE epmHandle,
	int platformIndex,
	char* platform,
	int bufferSize
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	Q_UNUSED(epmHandle);

	Platforms platforms = gDevEPMCore.GetPlatforms();
	if (platformIndex < platforms.count())
	{
		QPair<QByteArray, QByteArray> platformPair = platforms.at(platformIndex);
		QByteArray merged = platformPair.first + ";" + platformPair.second;
		if (merged.size() < bufferSize)
		{
			my_memcpy(platform, bufferSize, merged.data(), merged.size());
		}
		else
		{
			result = EPM_BUFFER_TOO_SMALL;
		}
	}
	else
	{
		result =  EPM_BAD_EPM_INDEX;
	}

	return result;
}

EPM_RESULT SetPlatform
(
	EPM_HANDLE epmHandle,
	const char* platform
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject = gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		QString platformPath = gDevEPMCore.getPlatformConfigPath(platform);
		if (epmProject->loadPlatformConfigFile(platformPath) == false)
			result = EPM_BAD_PATH;
	}
	else
		result = EPM_BAD_EPM_HANDLE;

	return result;
}

EPM_RESULT GetChannelCount(EPM_HANDLE epmHandle, int* channelCount)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		*channelCount = epmProject->channelCount();
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT GetChannel
(
	EPM_HANDLE epmHandle,
	int channelIndex,
	char* channelDesc,
	int bufferSize,
	unsigned long long* hash
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		QString channelName;
		HashType channelHash;

		if (epmProject->getChannelData(channelIndex, channelName, channelHash) == true)
		{
			my_memcpy(channelDesc, bufferSize, channelName.toLatin1().data(), channelName.size());
			*hash = static_cast<unsigned long long>(channelHash);
		}
		else
			result = EPM_BAD_EPM_INDEX;
	}
	else
		result = EPM_BAD_EPM_HANDLE;

	return result;
}

EPM_RESULT SetRuntimeConfig
(
	EPM_HANDLE epmHandle,
	const char* runtimeConfigfile
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		if (epmProject->loadRuntimeConfigFile(runtimeConfigfile) == false)
			result = EPM_BAD_PATH;
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT GetCurrentChannelState
(
	EPM_HANDLE epmHandle,
	unsigned long long channelHash,
	bool* active,
	bool* saveRaw
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		if (epmProject->getCurrentChannelState(channelHash, *active, *saveRaw) == false)
			result = EPM_BAD_CHANNEL_HASH;
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT SetCurrentChannelState
(
	EPM_HANDLE epmHandle,
	unsigned long long channelHash,
	bool active,
	bool saveRaw
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		if (epmProject->setCurrentChannelState(channelHash, active, saveRaw) == false)
			result = EPM_BAD_CHANNEL_HASH;
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT GetVoltageChannelState
(
	EPM_HANDLE epmHandle,
	unsigned long long channelHash,
	bool* active,
	bool* saveRaw
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		if (epmProject->getVoltageChannelState(channelHash, *active, *saveRaw) == false)
			result = EPM_BAD_CHANNEL_HASH;
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT SetVoltageChannelState
(
	EPM_HANDLE epmHandle,
	unsigned long long channelHash,
	bool active,
	bool saveRaw
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		if (epmProject->setVoltageChannelState(channelHash, active, saveRaw) == false)
			result = EPM_BAD_CHANNEL_HASH;
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT Acquire(EPM_HANDLE epmHandle, double seconds)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMDevice epmDevice = gDevEPMCore.getEPMDevice(epmHandle);
	if (epmDevice.isNull() == false)
	{
		EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
		if (epmProject.isNull() == false)
		{
			epmProject->setEPMDevice(epmDevice);
			epmProject->acquire(seconds);
		}
		else
		{
			result = EPM_BAD_EPM_HANDLE;
		}
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT StartAcquisition
(
	EPM_HANDLE epmHandle
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMDevice epmDevice = gDevEPMCore.getEPMDevice(epmHandle);
	if (epmDevice.isNull() == false)
	{
		EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
		if (epmProject.isNull() == false)
		{
			epmProject->setEPMDevice(epmDevice);
			if (epmProject->startAcquire() == false)
				result = EPM_ACQUIRE_FAILED;

		}
		else
		{
			result = EPM_BAD_EPM_HANDLE;
		}
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT StopAcquisition(EPM_HANDLE epmHandle)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		epmProject->stopAcquire();
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT GetResultsPath
(
	EPM_HANDLE epmHandle,
	char* resultsPath,
	int bufferSize
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		QByteArray results = epmProject->getResultsPath().toLatin1();

		if (results.size() < bufferSize)
		{
			my_memcpy(resultsPath, bufferSize, results.data(), results.size());
		}
		else
		{
			result = EPM_BUFFER_TOO_SMALL;
		}

		return result;
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT SetTemporaryResultsFolder
(
	EPM_HANDLE epmHandle,
	const char* temporaryPath
)
{
	EPM_RESULT result{NO_EPM_ERROR};

	EPMProject epmProject =gDevEPMCore.getEPMProject(epmHandle);
	if (epmProject.isNull() == false)
	{
		epmProject->setTemporaryResultsDirectory(temporaryPath);
	}
	else
	{
		result = EPM_BAD_EPM_HANDLE;
	}

	return result;
}

EPM_RESULT GetLoggingState
(
	bool* loggingState
)
{
	*loggingState = gDevEPMCore.getLoggingState();

	return NO_EPM_ERROR;
}

EPM_RESULT SetLoggingState(bool loggingState)
{
	gDevEPMCore.setLoggingState(loggingState);

	return NO_EPM_ERROR;
}

EPM_RESULT WriteToAppLog(const char *logText)
{
	gDevEPMCore.writeToAppLog(logText);

	return NO_EPM_ERROR;
}


