// Confidential and Proprietary Qualcomm Technologies, Inc.

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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "UDASRecorder.h"
#include "EpmLibPrivate.h"

// QCommon
#include "Range.h"

// Qt
#include <QDir>

static int UdasSortCompareFunction
(
	const void* p1,
	const void* p2
)
{
	UDASChannelRecording* a1 = (UDASChannelRecording*) p1;
	UDASChannelRecording* a2 = (UDASChannelRecording*) p2;

	if (a1->_recordingFile.isOpen() == false && a2->_recordingFile.isOpen() == false)
		return 0;
	else if (a1->_recordingFile.isOpen() == false && a2->_recordingFile.isOpen() == true)
		return 1;
	else if (a1->_recordingFile.isOpen() == true && a2->_recordingFile.isOpen() == false)
		return -1;
	else
	{
		if (a1->uRcmChannel == a2->uRcmChannel)
			return 0;
		else if (a1->uRcmChannel < a2->uRcmChannel)
			return -1;
		else
			return 1;
	}
}

void UDASRecorder::startRecording()
{
	quint32 uNumChanEn = 0;
	quint32 uChanIdx;

	QString paramsFile = _resultsFolder + QDir::separator() + "Params.scl";

	// Open summary file
	if (_udasFile.create(paramsFile) == true)
	{
		// For each enabled channel, open a data file and initialize
		for (auto uBus: range(MAX_BUSES))
		{
			const quint32 uAdcChannelMask = _epmDevice->_enabledChannelBitmask[uBus];

			for (auto uChannel: range(MAX_CHANNELS_PER_BUS))
			{
				uChanIdx = (uBus * MAX_CHANNELS_PER_BUS) + uChannel;
				UDASChannelRecording& channelRecord = _chanRecordings[uChanIdx];
				if (uAdcChannelMask & (1 << uChannel))
				{
					channelRecord._logRaw = _epmDevice->_channelInfo[uChanIdx]._dumpRawValues;
					channelRecord.uNumSamples = 0;
					channelRecord.uFileIdx = uNumChanEn;
					channelRecord.uRcmChannel = _epmDevice->_channelInfo[uChanIdx].uRcmChannel;
					channelRecord.uChanIdx = uChanIdx;
					uNumChanEn++;

					QString dataFile = _resultsFolder + QDir::separator() + QString("Data_%1.scl").arg(channelRecord.uFileIdx);

					channelRecord.open(dataFile, &_udasFile);
				}
			}
		}

		// GPIO channels
		for (auto uGpio: range(MAX_NUM_GPIO_CHANNELS))
		{
			uChanIdx = MAX_NUM_ADC_CHANNELS + uGpio;
			UDASChannelRecording& channelRecord = _chanRecordings[uChanIdx];

			if (_epmDevice->_gpioEnMask & (1 << uGpio))
			{
				channelRecord.uNumSamples = 0;
				channelRecord.uFileIdx = uNumChanEn;
				channelRecord.uRcmChannel = _epmDevice->_channelInfo[uChanIdx].uRcmChannel;
				channelRecord.uChanIdx = uChanIdx;
				uNumChanEn++;

				QString dataFile = _resultsFolder + QDir::separator() + QString("Data_%1.scl").arg(channelRecord.uFileIdx);

				channelRecord.open(dataFile, &_udasFile);
			}
		}

		_record = true;
	}
}

void UDASRecorder::stopRecording()
{
	if (_record == true)
	{
		// First, sort the recordings so Kratos Data Viewer displays them in sorted order
		qsort(_chanRecordings, MICRO_EPM_MAX_NUMBER_CHANNELS, sizeof(UDASChannelRecording), UdasSortCompareFunction);
		
		for (auto uRecIdx: range(MICRO_EPM_MAX_NUMBER_CHANNELS))
		{
			UDASChannelRecording& channelRecording = _chanRecordings[uRecIdx];
		
			if (channelRecording._recordingFile.isOpen())
			{
				MicroEpmChannelInfo* pChanInfo;
		
				pChanInfo = &_epmDevice->_channelInfo[channelRecording.uChanIdx];
		
				channelRecording.saveWaveFormBlock(pChanInfo);

				channelRecording.close();
			}
		}

		_udasFile.save();

		_record = false;
	}
}

void UDASRecorder::recordData
(
	MicroEpmChannelData* channelData,
	quint32 sampleCount
)
{
	double dbPhysical;

	if (_record)
	{
		MicroEpmChannelData* pSample;

		for (auto uSampleIdx: range(sampleCount))
		{
			pSample = &channelData[uSampleIdx];

			if (pSample->_channelType == MICRO_EPM_CHANNEL_TYPE_GPIO)
			{
				// Result is a bitmask of GPIO readings

				for (auto uGpio: range(MAX_NUM_GPIO_CHANNELS))
				{
					if (_epmDevice->_gpioEnMask & (1 << uGpio))
					{
						UDASChannelRecording& channelRecording = _chanRecordings[MAX_NUM_ADC_CHANNELS + uGpio];

						if (pSample->_rawCode & (1 << uGpio))
						{
							dbPhysical = GPIO_HIGH_V;
						} else
						{
							dbPhysical = GPIO_LOW_V;
						}

						channelRecording.logSample(dbPhysical, pSample->_timeStamp);
					}
				}
			}
			else if (pSample->_channel < MAX_NUM_ADC_CHANNELS)
			{
				// Result is a current or voltage
				UDASChannelRecording& channelRecording = _chanRecordings[pSample->_channel];

				if (pSample->_channelType == MICRO_EPM_CHANNEL_TYPE_VOLTAGE)
					dbPhysical = pSample->_physicalValue / 1000.0;
				else
					dbPhysical = pSample->_physicalValue;

				logRaw(channelRecording, *pSample);

				channelRecording.logSample(dbPhysical, pSample->_timeStamp);
			}
		}
	}
}

void UDASRecorder::logRaw
(
	UDASChannelRecording& channelRecording,
	const MicroEpmChannelData& channelData
)
{
	channelRecording.logRaw(channelData);
}
