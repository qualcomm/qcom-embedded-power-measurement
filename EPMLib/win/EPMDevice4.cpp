// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMDevice4.h"
#include "EpmLibPrivate.h"
#include "MicroEpmCommands.h"
#include "MicroEpmErrors.h"
#include "RecordingInferface.h"

// QCommon
#include "Range.h"
#include "TickCount.h"

// C++
#include <algorithm>

#pragma pack(push, 1)

struct Device4Packet
{
	quint8			_command;

	quint8			_bitField;
//	quint8			_empty : 1;
//	quint8			_overflowed : 1;
//	quint8			_unused : 1;
//	quint8			_gpioData : 1;
//	quint8			_bus : 4;

	quint8			_sampleCount;
	quint8			_channelIndex;
	quint32			_channelBitmask;
	quint32			_firstSetTS;
	quint32			_lastSetTS;
	quint16			_readings[12];

	bool empty()
	{
		return (_bitField & 0x80) != 0;
	}

	bool overFlowed()
	{
		return (_bitField & 0x40) != 0;
	}

	bool gpioData()
	{
		return (_bitField & 0x20) != 0;
	}

	quint8 i2cBus()
	{
		return _bitField & 0x0F;
	}
};

#pragma pack(pop)

const quint16 VENDORID			{0x05C6};
const quint16 PRODUCTID			{0x9302};
const quint8 IN_EP				{0x82};
const quint8 IN2_EP				{0x83};
const quint8 OUT_EP				{0x01};

// Constant for {DFB21FFD-031F-4E30-B858-BE5B6FD65E0E}
static const GUID MICRO_EPM_4_DEVICE_INTERFACE =
{ 0xDFB21FFD, 0x031F, 0x4E30, { 0xB8, 0x58, 0xBE, 0x5B, 0x6F, 0xD6, 0x5E, 0x0E } };


/*****************************************************
*  Packet format
*
* Index(Byte)  Description
*     0     Command (0x0C)
*     1     Epmty [7], Overflowed [6], Unused [5], GPIOData [4], Bus index [3:0]
*     2     Number of samples in packet
*     3     Channel Index (first channel reading in packet)
*     4     Channel Bitmask
*     8     Timestamp of first set
*     12    Timestamp of last set
*     16    Reading 0 LSB
*     17    Reading 1 MSB
*      .
*      .
*      .
*     62    Reading 23 LSB
*     63    Reading 23 MSB
*
****************************************************/
static int MicroEpmGetDataSortCompareFunction
(
	const void *p1,
	const void *p2
)
{
	MicroEpmChannelData* a1 = (MicroEpmChannelData*) p1;
	MicroEpmChannelData* a2 = (MicroEpmChannelData*) p2;

	if (a1->_timeStamp == a2->_timeStamp)
	{
		return 0;
	}
	else if (a1->_timeStamp < a2->_timeStamp)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

_EPMDevice* device4Alloctor()
{
	return new _EPMDevice4;
}

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

_EPMDevice4::_EPMDevice4()
{
	_vendorID = VENDORID;
	_productID = PRODUCTID;
	_in = IN_EP;
	_in2 = IN2_EP;
	_out = OUT_EP;
}

void _EPMDevice4::close()
{
	closeInternal();
}

void _EPMDevice4::updateDeviceList()
{
	try
	{
		_WinBaseDevice::updateDeviceList(MICRO_EPM_4_DEVICE_INTERFACE, device4Alloctor);
	}
	catch (EPMException error)
	{
		AppCore::writeToApplicationLog(QString("_EPMDevice4::updateDeviceList EPM Error %1\n").arg(error.what()));
	}
}

/*****************************************************
*  Packet format
*
* Index(Byte)  Description
*     0     Command (0x0C)
*     1     Epmty [7], Overflowed [6], Unused [5], GPIOData [4], Bus index [3:0]
*     2     Number of samples in packet
*     3     Channel Index (first channel reading in packet)
*     4     Channel Bitmask
*     8     Timestamp of first set
*     12    Timestamp of last set
*     16    Reading 0 LSB
*     17    Reading 1 MSB
*      .
*      .
*      .
*     62    Reading 23 LSB
*     63    Reading 23 MSB
*
****************************************************/
void _EPMDevice4::getBulkData
(
	MicroEpmChannelData* channelData,
	quint32 channelDataSize,
	quint32& sampleCount,
	quint32 packetCount,
	quint32& overflowCount,
	quint32& emptyCount
)
{
	quint32 uStartTime;
	quint32 uEndTime;
	quint32 uChannelMask;
	quint32 uSample;
	quint32 uChannelIdx = 0;
	quint32 uNumSamplesInPacket;
	quint32 auEnabledChannelsList[32];
	quint32 uNumEnabledChannels = 0;
	int32_t nRaw = 0;
	quint32 uChannel;
	quint32 uPacket;
	uint8_t* sampleBuffer;
	quint32 uFirstChannel;
	quint32 uNumEmpty = 0;
	quint32 uNumOverflow = 0;
	quint32 uNumSamples = 0;
	quint32 uNumChannelsInFirstSet;
	quint32 uNumSets;
	quint32 uSetPeriod;
	quint32 uCurrentTime;
	uint8_t uBus;
	uint8_t bIsAdcDataPacket;

	if (packetCount > _version._maxPackets)
		throw MICRO_EPM_INVALID_PARAMETER;

	if (channelDataSize < (packetCount * _version._maxSamplesPerPacket))
		throw MICRO_EPM_INVALID_PARAMETER;

	MakeGetBufferedDataCommand();

	try
	{
		readBulkData(BUF_SIZE * packetCount);

		// Extract the samples from each packet of buffered data
		for (uPacket = 0; uPacket < packetCount; uPacket++)
		{
			sampleBuffer = &_bigBuffer[uPacket * BUF_SIZE];

			if (sampleBuffer[0] != GET_BUFFERED_DATA_RSP)
			{
				// bad packet format - discard packet
				_errorCount++;
				continue;
			}
			else if (sampleBuffer[1] & 0x80)
			{
				// empty packet - discard packet
				emptyCount++;
				continue;
			}

			uNumSamplesInPacket = sampleBuffer[2];
			if (uNumSamplesInPacket == 0)
			{
				// invalid packet - discard
				_errorCount++;
				continue;
			}

			if (sampleBuffer[1] & 0x40)
			{
				// buffer overflowed - do not discard
				uNumOverflow++;
				_overflowCount++;
			}

			uBus = sampleBuffer[1] & 0xF;

			if (sampleBuffer[1] & 0x10)
			{
				// GPIO data
				if (_gpioEnMask == 0)
				{
					// client hasn't enabled GPIOs - ignore this packet
					continue;
				}

				bIsAdcDataPacket = FALSE;
			}
			else
			{
				// ADC data
				bIsAdcDataPacket = TRUE;

				if (uBus >= _numBuses)
				{
					// invalid packet - discard
					_errorCount++;
					continue;
				}
			}

			uFirstChannel = sampleBuffer[3];

			uChannelMask = (((quint32) sampleBuffer[7] << 24)
				| ((quint32) sampleBuffer[6] << 16)
				| ((quint32) sampleBuffer[5] << 8)
				| (quint32) sampleBuffer[4]);

			if (uChannelMask == 0)
			{
				// no channels are enabled - discard packet
				_errorCount++;
				continue;
			}

			uStartTime = ((quint32) sampleBuffer[11] << 24) |
						 ((quint32) sampleBuffer[10] << 16) |
						 ((quint32) sampleBuffer[9] << 8) |
						  (quint32) sampleBuffer[8];

			uEndTime = ((quint32) sampleBuffer[15] << 24) |
					   ((quint32) sampleBuffer[14] << 16) |
					   ((quint32) sampleBuffer[13] << 8) |
						(quint32) sampleBuffer[12];


			uStartTime = TIMER_MASK - uStartTime;
			uEndTime = TIMER_MASK - uEndTime;

			if (bIsAdcDataPacket == TRUE)
			{
				// Get the array of enabled channels
				uNumEnabledChannels = 0;

				for (uChannel = 0; uChannel < 32; uChannel++)
				{
					if (uChannelMask & (1 << uChannel))
					{
						auEnabledChannelsList[uNumEnabledChannels] = uChannel;
						uNumEnabledChannels++;
					}
				}

				// Get the index to the first channel
				uChannelIdx = uNumEnabledChannels;

				for (uChannel = 0; uChannel < uNumEnabledChannels; uChannel++)
				{
					if (auEnabledChannelsList[uChannel] == uFirstChannel)
					{
						// Index to first channel enabled is found
						uChannelIdx = uChannel;
						break;
					}
				}

				if (uChannelIdx == uNumEnabledChannels)
				{
					// The first channel was not found in the list of enabled channels
					_errorCount++;
					continue;
				}

				// Get the number of sets
				// First, get the number of channels in the first set
				uNumChannelsInFirstSet = uNumEnabledChannels - uChannelIdx;
				if (uNumChannelsInFirstSet >= uNumSamplesInPacket)
				{
					// All the samples belong to the same set
					uNumSets = 1;
				} else
				{
					// Partial 1st set + N full sets
					uNumSets = 1 + ((uNumSamplesInPacket - uNumChannelsInFirstSet) / uNumEnabledChannels);

					if (((uNumSamplesInPacket - uNumChannelsInFirstSet) % uNumEnabledChannels) > 0) {
						// Add in a partial final set
						uNumSets++;
					}
				}
			}
			else
			{
				// GPIO case
				uNumSets = uNumSamplesInPacket;
			}

			// Get the set period
			if (uNumSets == 1)
			{
				uSetPeriod = 0;
			}
			else
			{
				if (uEndTime > uStartTime)
				{
					// The typical case
					// --------------s=============e----------
					// 0                                    MASK
					uSetPeriod = (uEndTime - uStartTime) / (uNumSets - 1);
				} else
				{
					// The time has rolled over
					// ====e-----------------------s==========
					// 0                                    MASK
					uSetPeriod = (TIMER_MASK - uStartTime + uEndTime) / (uNumSets - 1);
				}
			}

			uCurrentTime = uStartTime;

			for (uSample = 0; uSample < uNumSamplesInPacket; uSample++)
			{
				if (bIsAdcDataPacket == TRUE)
				{
					quint32 uMicroEpmChanIdx;

					// ADC data
					uChannel = auEnabledChannelsList[uChannelIdx];
					uMicroEpmChanIdx = uChannel + uBus * MAX_CHANNELS_PER_BUS;

					if (_enabledChannelBitmask[uBus] & (1 << uChannel))
					{
						nRaw = (int16_t)(sampleBuffer[16 + uSample * 2] + (sampleBuffer[16 + uSample * 2 + 1] << 8));
						if (uChannel & 1)
						{
							channelData[uNumSamples]._channelType = MICRO_EPM_CHANNEL_TYPE_VOLTAGE;
							channelData[uNumSamples]._physicalValue = (double)nRaw * SPMV4_V_MV_PER_CODE;
						}
						else
						{
							channelData[uNumSamples]._channelType = MICRO_EPM_CHANNEL_TYPE_CURRENT;
							channelData[uNumSamples]._physicalValue = (double)nRaw * SPMV4_I_UV_PER_CODE / _channelInfo[uMicroEpmChanIdx].dbRsenseMilliOhms;
						}


						//channelData[uNumSamples].uTarget = uTarget;
						//channelData[uNumSamples].uModule = uModule;
						channelData[uNumSamples]._channel = uMicroEpmChanIdx;
						channelData[uNumSamples]._rawCode = nRaw;
						channelData[uNumSamples]._timeStamp = uCurrentTime;
						uNumSamples++;
					}  /* end if */

					// Increment to the next channel
					uChannelIdx++;
					if (uChannelIdx >= uNumEnabledChannels)
					{
						// Increment to the next set
						uChannelIdx = 0;

						// Increment the current time by the set period
						if ((TIMER_MASK - uCurrentTime) >= uSetPeriod)
						{
							uCurrentTime += uSetPeriod;
						}
						else
						{
							// uCurrentTime needs to roll over
							uCurrentTime = uSetPeriod - (TIMER_MASK - uCurrentTime);
						}
					}
				}
				else
				{
					// GPIO data
					//channelData[uNumSamples].uTarget = uTarget;
					//channelData[uNumSamples].uModule = uModule;
					channelData[uNumSamples]._channel = 0;
					channelData[uNumSamples]._channelType = MICRO_EPM_CHANNEL_TYPE_GPIO;
					channelData[uNumSamples]._physicalValue = 0;
					channelData[uNumSamples]._rawCode = (int32_t)(sampleBuffer[16 + uSample * 2]);
					channelData[uNumSamples]._timeStamp = uCurrentTime;
					uNumSamples++;

					// Increment the current time by the set period
					if ((TIMER_MASK - uCurrentTime) >= uSetPeriod)
					{
						uCurrentTime += uSetPeriod;
					} else {
						// uCurrentTime needs to roll over
						uCurrentTime = uSetPeriod - (TIMER_MASK - uCurrentTime);
					}
				}
			}  /* end for samples */
		} /* end for packets */

		sampleCount = uNumSamples;
		emptyCount = uNumEmpty;
		overflowCount = uNumOverflow;

		if (sampleCount > 1)
		{
			qsort(channelData, sampleCount, sizeof(MicroEpmChannelData), MicroEpmGetDataSortCompareFunction);
		}
	}
	catch (EPMException error)
	{
		if (_appCore != Q_NULLPTR)
			_appCore->writeToRunLog(QByteArray(error.what()) + "\n");
		sampleCount = 0;
		emptyCount = 0;
		overflowCount = 0;
	}
}

