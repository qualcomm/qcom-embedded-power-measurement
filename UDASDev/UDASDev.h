#ifndef UDASDEV_H
#define UDASDEV_H
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
// Copyright 2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
		   Biswajit Roy (biswroy@qti.qualcomm.com)
		   Biswajit Roy (biswroy@qti.qualcomm.com)
*/

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

	UDASDEV_EXPORT UDAS_RESULT GetAlpacaVersion(char* alpacaVersion, int bufferSize);
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
