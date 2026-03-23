#ifndef UDASDEV_H
#define UDASDEV_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#if defined(UDASDEV_LIBRARY)
#ifdef __linux__
	#define UDASDEV_EXPORT __attribute__((visibility("default")))
#else
	#define UDASDEV_EXPORT __declspec(dllexport)
#endif
#else
#ifdef __linux__
#   define UDASDEV_EXPORT __attribute__((visibility("default")))
#else
#  define UDASDEV_EXPORT  __declspec(dllimport)
#endif
#endif

typedef unsigned long UDAS_FILE_HANDLE;
typedef unsigned long UDAS_BLOCK_HANDLE;
typedef unsigned long UDAS_ERROR;
typedef unsigned long UDAS_RESULT;
typedef unsigned long DataCount;
typedef unsigned long ChannelCount;
typedef unsigned long ChannelIndex;
typedef unsigned long DataIndex;

const double kDefaultTime{0.0};

// UDAS Error codes
const unsigned long NO_UDAS_ERROR{0};
const unsigned long UDAS_BUFFER_TOO_SMALL{1};
const unsigned long UDAS_BAD_UDAS_HANDLE{2};
const unsigned long UDAS_BAD_UDAS_INDEX{3};
const unsigned long UDAS_BAD_WAVEFORM_TYPE{4};
const unsigned long UDAS_INIT_FAILED{5};

#ifdef __cplusplus
extern "C" {
#endif
	UDASDEV_EXPORT UDAS_RESULT InitializeUDASDev();

	UDASDEV_EXPORT UDAS_RESULT GetQEPMVersion(char* QEPMVersion, int bufferSize);
	UDASDEV_EXPORT UDAS_RESULT GetEPMVersion(char* epmVersion, int bufferSize);
	UDASDEV_EXPORT UDAS_RESULT GetLastUdasError(char* lastError, int bufferSize);

	UDASDEV_EXPORT UDAS_FILE_HANDLE OpenResults(const char* powerRunFileName);

	UDASDEV_EXPORT UDAS_RESULT GetChannelCount(UDAS_FILE_HANDLE fileHandle, ChannelCount* channelCount);
	UDASDEV_EXPORT UDAS_RESULT GetChannelName(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, char* channelName, int bufferSize);
	UDASDEV_EXPORT UDAS_RESULT GetChannelIndex(UDAS_FILE_HANDLE fileHandle, char* channelName, ChannelIndex* channelIndex);

	UDASDEV_EXPORT UDAS_RESULT GetCurrentChannelDataCount(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, DataCount* dataCount);
	UDASDEV_EXPORT UDAS_RESULT GetCurrentChannelData(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, DataIndex dataIndex, double* result);

	UDASDEV_EXPORT UDAS_RESULT GetVoltageChannelDataCount(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, DataCount* dataCount);
	UDASDEV_EXPORT UDAS_RESULT GetVoltageChannelData(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, DataIndex dataIndex, double* result);

	UDASDEV_EXPORT UDAS_RESULT GetPowerChannelDataCount(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, DataCount* dataCount);
	UDASDEV_EXPORT UDAS_RESULT GetPowerChannelData(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, DataIndex dataIndex, double* result);

	UDASDEV_EXPORT UDAS_RESULT GetTimeSeries(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, char* dataPoints, const int dataSize, long long* actualSize);

	UDASDEV_EXPORT UDAS_RESULT GetWaveformAvg(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *average, double startTime = kDefaultTime, double endTime = kDefaultTime);
	UDASDEV_EXPORT UDAS_RESULT GetWaveformDuration(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *duration, double startTime = kDefaultTime, double endTime = kDefaultTime);
	UDASDEV_EXPORT UDAS_RESULT GetWaveformArea(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *area, double startTime = kDefaultTime, double endTime = kDefaultTime);
	UDASDEV_EXPORT UDAS_RESULT GetWaveformPStdDev(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *populationStdDev, double startTime = kDefaultTime, double endTime = kDefaultTime);
	UDASDEV_EXPORT UDAS_RESULT GetWaveformMax(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *max, double startTime = kDefaultTime, double endTime = kDefaultTime);
	UDASDEV_EXPORT UDAS_RESULT GetWaveformRange(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *range, double startTime = kDefaultTime, double endTime = kDefaultTime);
	UDASDEV_EXPORT UDAS_RESULT GetWaveformMin(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *min, double startTime = kDefaultTime, double endTime = kDefaultTime);

	UDASDEV_EXPORT UDAS_RESULT SetExportPath(UDAS_FILE_HANDLE fileHandle, const char* exportDirectory);
	UDASDEV_EXPORT UDAS_RESULT GetExportPath(UDAS_FILE_HANDLE fileHandle, char* resultsPath, int bufferSize);
    UDASDEV_EXPORT UDAS_RESULT ExportAsCSV(UDAS_FILE_HANDLE fileHandle, const char* exportConfigfile = nullptr);

#ifdef __cplusplus
}
#endif

#endif // UDASDEV_H
