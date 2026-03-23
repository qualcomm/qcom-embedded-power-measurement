#ifndef EPMDEV_H
#define EPMDEV_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#if defined(EPMDEV_LIBRARY)
#ifdef __linux__
	#define EPMDEV_EXPORT __attribute__((visibility("default")))
#else
	#define EPMDEV_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __linux__
#   define EPMDEV_EXPORT __attribute__((visibility("default")))
#else
#  define EPMDEV_EXPORT  __declspec(dllimport)
#endif
#endif

typedef unsigned long EPM_HANDLE;
typedef unsigned long EPM_ERROR;
typedef unsigned long EPM_RESULT;

const EPM_HANDLE kBadHandle{0};

const unsigned long NO_EPM_ERROR{0};
const unsigned long EPM_BUFFER_TOO_SMALL{1};
const unsigned long EPM_BAD_EPM_HANDLE{2};
const unsigned long EPM_BAD_EPM_INDEX{3};
const unsigned long EPM_BAD_PATH{4};
const unsigned long EPM_ACQUIRE_FAILED{5};
const unsigned long EPM_BAD_CHANNEL_HASH{6};
const unsigned long EPM_INIT_FAILED{7};

#ifdef __cplusplus
extern "C" {
#endif
	EPMDEV_EXPORT EPM_RESULT InitializeEPMDev();

	EPMDEV_EXPORT EPM_RESULT GetQEPMVersion(char* QEPMVersion, int bufferSize);
	EPMDEV_EXPORT EPM_RESULT GetEPMVersion(char* epmVersion, int bufferSize);
	EPMDEV_EXPORT EPM_RESULT GetLastEPMError(char* lastError, int bufferSize);

	EPMDEV_EXPORT EPM_RESULT GetLoggingState(bool* loggingState);
	EPMDEV_EXPORT EPM_RESULT SetLoggingState(bool loggingState);

	EPMDEV_EXPORT EPM_RESULT WriteToAppLog(const char* logText);

	EPMDEV_EXPORT EPM_RESULT GetDeviceCount(int* deviceCount);
	EPMDEV_EXPORT unsigned long GetPortData(int deviceIndex, char* portData, int bufferSize);

	EPMDEV_EXPORT EPM_HANDLE OpenHandleByDescription(const char* targetName);
	EPMDEV_EXPORT EPM_RESULT CloseEPMHandle(EPM_HANDLE epmHandle);

	EPMDEV_EXPORT EPM_RESULT GetUUID(EPM_HANDLE epmHandle, char* uuid, int bufferSize);

	EPMDEV_EXPORT EPM_RESULT GetPlatformPath(EPM_HANDLE epmHandle, char* platformPath, int bufferSize);
	EPMDEV_EXPORT EPM_RESULT SetPlatformPath(EPM_HANDLE epmHandle, const char* newPath);

	EPMDEV_EXPORT EPM_RESULT GetPlatformCount(EPM_HANDLE epmHandle, int* count);
	EPMDEV_EXPORT EPM_RESULT GetPlatform(EPM_HANDLE epmHandle, int platformIndex, char* platform, int bufferSize);
	EPMDEV_EXPORT EPM_RESULT SetPlatform(EPM_HANDLE epmHandle, const char* platform);

	EPMDEV_EXPORT EPM_RESULT GetChannelCount(EPM_HANDLE epmHandle, int* channelCount);
	EPMDEV_EXPORT EPM_RESULT GetChannel(EPM_HANDLE epmHandle, int channelIndex, char* channelDesc, int bufferSize, unsigned long long *channelHash);

	EPMDEV_EXPORT EPM_RESULT SetRuntimeConfig(EPM_HANDLE epmHandle, const char* runtimeConfigfile);

	EPMDEV_EXPORT EPM_RESULT GetCurrentChannelState(EPM_HANDLE epmHandle, unsigned long long channelHash, bool* active, bool* saveRaw);
	EPMDEV_EXPORT EPM_RESULT SetCurrentChannelState(EPM_HANDLE epmHandle, unsigned long long channelHash, bool active, bool saveRaw);

	EPMDEV_EXPORT EPM_RESULT GetVoltageChannelState(EPM_HANDLE epmHandle, unsigned long long channelHash, bool* active, bool* saveRaw);
	EPMDEV_EXPORT EPM_RESULT SetVoltageChannelState(EPM_HANDLE epmHandle, unsigned long long channelHash, bool active, bool saveRaw);

	// Set a temporary, one time path for EPM results
	EPMDEV_EXPORT EPM_RESULT SetTemporaryResultsFolder(EPM_HANDLE epmHandle, const char* temporaryPath);

	// static snapshot
	EPMDEV_EXPORT EPM_RESULT Acquire(EPM_HANDLE epmHandle, double seconds);

	// open ended acquistion, subject to memory exhaustion
	EPMDEV_EXPORT EPM_RESULT StartAcquisition(EPM_HANDLE epmHandle);
	EPMDEV_EXPORT EPM_RESULT StopAcquisition(EPM_HANDLE epmHandle);

	EPMDEV_EXPORT EPM_RESULT GetResultsPath(EPM_HANDLE epmHandle, char* resultsPath, int bufferSize);
#ifdef __cplusplus
}
#endif

#endif // EPMDEV_H
