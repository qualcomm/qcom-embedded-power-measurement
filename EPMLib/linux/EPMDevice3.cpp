// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMDevice3.h"

#include "MicroEpmCommands.h"
#include "MicroEpmErrors.h"
#include "RecordingInferface.h"

// QCommon
#include "AppCore.h"

// C++
#include <algorithm>

const quint16 VENDORID			{0x05C6};
const quint16 PRODUCTID			{0x9301};
const quint8 IN_EP				{0x81};
const quint8 IN2_EP				{0x83};
const quint8 OUT_EP				{0x02};

_EPMDevice3::_EPMDevice3()
{
	_vendorID = VENDORID;
	_productID = PRODUCTID;
	_in = IN_EP;
	_in2 = IN2_EP;
	_out = OUT_EP;
}

void _EPMDevice3::close()
{
	closeInternal();
    _LinuxBaseDevice::close();
}

void _EPMDevice3::updateDeviceList
(
	struct usb_device* usbDevice
)
{
	try
    {
        if (usbDevice->descriptor.idProduct == PRODUCTID)
        {
            _EPMDevice3* epmDevice3 = new _EPMDevice3;
            epmDevice3->_device = usbDevice;

			EPMDevice epmDevice = EPMDevice(epmDevice3);
            epmDevice->_devicePath = QByteArray(usbDevice->filename);

            AppCore::writeToApplicationLogLine(QString("Device found %1").arg(epmDevice->_devicePath.data()));

            _EPMDevice::addDevice(epmDevice);
        }
	}
	catch (const EPMException& error)
	{
        AppCore::writeToApplicationLogLine(QString("_EPMDevice3::updateDeviceList EPM Error %1").arg(error.what()));
	}
}

/*****************************************************
*  Packet format
*
* Index(Byte)  Description
*     0     Command (0x0C)
*     1     Device index (0)
*     2     GPIO Mask (reading from GPIO_In_Reg)
*     3     Channel Index (first channel reading in packet)
*     4     Channel Bitmask
*     8     Timestamp Start
*     12    Timestamp End
*     16    Reading 0
*     18    Reading 1
*      .
*      .
*     63    Reading 23
*
****************************************************/
void _EPMDevice3::getBulkData
(
	MicroEpmChannelData* channelData,
	quint32 channelDataSize,
	quint32& sampleCount,
	quint32 packetCount,
	quint32& overflowCount,
	quint32& emptyCount
)
{
	double timestamp_start{0.0};
	double timestamp_start_1{0.0};
	double timestamp_end{0.0};
	double timestamp_end_1{0.0};
	double timestamp_inc = {0.0};
	double cur_timestamp ={0.0};
	quint32 gpio_status = {0};
	quint32 channel_mask = {0};
	quint32 channel_status = {0};
	quint32 i = {0};
	quint32 channelIdx = {0};
	quint32 enabledChannelsList[32] = {0};
	quint32 numEnabledChannels = {0};
	int32_t nRaw = {0};
	quint32 uChannel = {0};
	quint32 uMaxArraySampleSize = {0};
	quint32 uOutBufferIdx = {0};
	quint32 uPacket = {0};
	uint8_t* sampleBuffer;
	quint32 uNumEmpty = {0};
	quint32 uNumOverflow = {0};
	quint32 uNumSamplesInPacket = {0};

	if (packetCount > MAX_PACKETS)
		throw MICRO_EPM_INVALID_PARAMETER;

	sampleCount = 0;

	try
	{
		MakeGetBufferedDataCommand();
		
		readBulkData(BUF_SIZE * packetCount);

		for (uPacket = 0; uPacket < packetCount; uPacket++)
		{
            uMaxArraySampleSize = std::min<quint16>(_version._maxSamplesPerPacket, channelDataSize);

			if (uMaxArraySampleSize == 0)
				break;

			numEnabledChannels = 0;
			uNumSamplesInPacket = 0;
			sampleBuffer = &_bigBuffer[uPacket * BUF_SIZE];

			if (sampleBuffer[1] & 0x20)
			{
				// empty packet
				uNumEmpty++;
				continue;
			}

			if (sampleBuffer[1] & 0x10)
			{
				// buffer overflowed.
				uNumOverflow++;
				_overflowCount++;
				continue;
			}

			if (sampleBuffer[0] != GET_BUFFERED_DATA_RSP)
				_errorCount++;

			gpio_status = sampleBuffer[2];
			channel_status = sampleBuffer[3];

			channel_mask = (((quint32) sampleBuffer[7] << 24)
					| ((quint32) sampleBuffer[6] << 16)
					| ((quint32) sampleBuffer[5] << 8)
					| (quint32) sampleBuffer[4]);

			timestamp_start_1 = ((quint32) sampleBuffer[11] << 24)
					| ((quint32) sampleBuffer[10] << 16)
					| ((quint32) sampleBuffer[9] << 8)
					| (quint32) sampleBuffer[8];

			timestamp_end_1 = ((quint32)sampleBuffer[15] << 24)
					| ((quint32) sampleBuffer[14] << 16)
					| ((quint32) sampleBuffer[13] << 8)
					| (quint32) sampleBuffer[12];


			if ((timestamp_start_1 == 0) || (timestamp_end_1 == 0))
				_timeErrorCount++;

			timestamp_start = TIMER_MASK - timestamp_start_1;
			timestamp_end = TIMER_MASK - timestamp_end_1;

			cur_timestamp = timestamp_start;
			timestamp_inc = (timestamp_end - timestamp_start) / (_version._maxSamplesPerPacket - 2);

			for (i = 0; i < 32; i++)
			{
				if (channel_mask & (1 << i))
				{
					if ((channel_status & 0x1f) == i)
					{
						channelIdx = numEnabledChannels;
					}

					enabledChannelsList[numEnabledChannels] = i;
					numEnabledChannels++;
				}
			}

			//channelData[uOutBufferIdx].uTarget = uTarget;
			//channelData[uOutBufferIdx].uModule = uModule;
			channelData[uOutBufferIdx]._channelType = MICRO_EPM_CHANNEL_TYPE_GPIO;
			channelData[uOutBufferIdx]._rawCode = gpio_status;
			channelData[uOutBufferIdx]._timeStamp = (quint32) timestamp_start;
			uOutBufferIdx++;
			uNumSamplesInPacket++;

			for (i = 0; i < uMaxArraySampleSize - 1; i++)
			{
				uChannel = enabledChannelsList[channelIdx];

				if (_enabledChannelBitmask[0] & (1 << uChannel))
				{
					if (uChannel == 31)
					{
						channelData[uOutBufferIdx]._channelType = MICRO_EPM_CHANNEL_TYPE_GPIO;
					}
					else if (uVoltageChannelBitmask & (1 << uChannel))
					{
						channelData[uOutBufferIdx]._channelType = MICRO_EPM_CHANNEL_TYPE_VOLTAGE;
					} else
					{
						channelData[uOutBufferIdx]._channelType = MICRO_EPM_CHANNEL_TYPE_CURRENT;
					}

					//channelData[uOutBufferIdx].uTarget = uTarget;
					//channelData[uOutBufferIdx].uModule = uModule;
					channelData[uOutBufferIdx]._channel = uChannel;
					nRaw = sampleBuffer[16 + i * 2] + (sampleBuffer[16 + i * 2 + 1] << 8);

					if (channelData[uOutBufferIdx]._channelType == MICRO_EPM_CHANNEL_TYPE_GPIO)
					{
						nRaw = nRaw >> 8;
					}
					else if (channelData[uOutBufferIdx]._channelType == MICRO_EPM_CHANNEL_TYPE_VOLTAGE)
					{
						/* Voltage channels use the 12-bit ADC */
						nRaw = nRaw << 3;
						if (nRaw > MAX_ADC_CODE)
						{
							if (nRaw > MAX_ADC_OVERFLOW_CODE)
							{
								nRaw = 0;
							}
							else
							{
								nRaw = MAX_ADC_CODE;
							}
						}
					}
					else
					{
						if (nRaw > MAX_ADC_CODE) {
							if (nRaw > MAX_ADC_OVERFLOW_CODE) {
								nRaw = (int16_t)nRaw;
							} else {
								nRaw = MAX_ADC_CODE;
							}
						}
					}
					channelData[uOutBufferIdx]._rawCode = nRaw;
					channelData[uOutBufferIdx]._timeStamp = (quint32) cur_timestamp;
					uOutBufferIdx++;
					uNumSamplesInPacket++;
				}  /* end if */

				channelIdx++;

				cur_timestamp += timestamp_inc;

				if (channelIdx >= numEnabledChannels)
				{
					channelIdx = 0;
				}
			}  /* end for */

			sampleCount += uNumSamplesInPacket;
			if (channelDataSize <=  uNumSamplesInPacket)
				channelDataSize = 0 ;
			else
				channelDataSize -= uNumSamplesInPacket;
		}

		emptyCount = uNumEmpty;
		overflowCount = uNumOverflow;
	}
    catch (const EPMException& error)
	{
        AppCore::writeToApplicationLogLine(error.what());

		sampleCount = 0;
		overflowCount = 0;
		emptyCount = 0;
	}
}

