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

#include "UDASChannelRecording.h"

#include "EPMLibDefines.h"
#include "UDASDefines.h"
#include "UDASFile.h"

// QT
#include <QDir>
#include <QFileInfo>

// C++
#include <math.h>

UDASChannelRecording::~UDASChannelRecording()
{
}

bool UDASChannelRecording::open
(
	const QString& filePath,
	UDASFile* parent
)
{
	bool result{false};

	_parent = parent;

	_recordingFile.setFileName(filePath);
	if (_recordingFile.open(QIODevice::WriteOnly))
	{
		_recordingDataStream.setByteOrder(QDataStream::BigEndian);
		_recordingDataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);

		_recordingDataStream.setDevice(&_recordingFile);
		result = true;
	}

	if (_logRaw)
	{
		QFileInfo rawFileInfo(filePath);
		QString rawPath = rawFileInfo.absolutePath() + QDir::separator() + rawFileInfo.baseName() + "_raw.txt";

		_rawLogFile = new QFile(rawPath);
		if (_rawLogFile)
		{
			if (_rawLogFile->open(QIODevice::WriteOnly))
			{
				_rawDataStream = new QTextStream;
				_rawDataStream->setDevice(_rawLogFile);
			}
			else
			{
				delete _rawLogFile;
				_rawLogFile = Q_NULLPTR;
			}

		}
	}

	return result;
}

void UDASChannelRecording::close()
{
	if (_recordingFile.isOpen())
		_recordingFile.close();

	if (_rawDataStream)
	{
		delete _rawDataStream;
		_rawDataStream = Q_NULLPTR;
	}

	if (_rawLogFile)
	{
		_rawLogFile->close();
		_rawLogFile->deleteLater();
		_rawLogFile = Q_NULLPTR;
	}

	_parent = Q_NULLPTR;
}

void UDASChannelRecording::logRaw
(
	const MicroEpmChannelData& channelData
)
{
	if (_rawDataStream)
		*_rawDataStream << QByteArray("raw:") << channelData._rawCode << QByteArray(" physical:") << channelData._physicalValue << Qt::endl;
}

void UDASChannelRecording::logSample
(
	double dbPhysical, quint32 uRawTimestamp
)
{
    double physical = fabs(dbPhysical);

	if (uNumSamples == 0)
	{
		u64TimestampStart = uRawTimestamp;
		u64TimestampStop = uRawTimestamp;
		_min = physical;
		_max = physical;
	}
	else
	{
		u64TimestampStop = uRawTimestamp;
		if (dbPhysical < _min)
		{
			_min = physical;
		}
		if (dbPhysical > _max)
		{
			_max = physical;
		}

		u64TimestampStop = uRawTimestamp;
	}

	uNumSamples++;

	_recordingDataStream << static_cast<float>(physical);
}

void UDASChannelRecording::saveWaveFormBlock
(
	MicroEpmChannelInfo* pChanInfo
)
{
	if (uNumSamples > 0 && _parent != Q_NULLPTR)
	{
		double duration = (double)(u64TimestampStop - u64TimestampStart) / US_PER_MS / MS_PER_S;
		double dbPeriod = (double)(u64TimestampStop - u64TimestampStart) / uNumSamples / US_PER_MS / MS_PER_S;

		UDASBlock udasBlock(new _UDASBlock(_parent));

		udasBlock->_channelNumber = uRcmChannel;
		udasBlock->_dataFileNumber = uFileIdx;

		if (uChanIdx < MAX_NUM_ADC_CHANNELS)
		{
			if (uChanIdx & 0x1)
			{
				// Odd: voltage channel
				udasBlock->_waveFormType = eWaveFormVoltage;
				udasBlock->_unitMultiplier = UDAS_WAVEFORM_UNIT_V;
			}
			else
			{
				// Even: current channel
				udasBlock->_waveFormType = eWaveFormCurrent;
				udasBlock->_unitMultiplier = UDAS_WAVEFORM_UNIT_MA;
			}
		}
		else
		{
			// Treat GPIOs as voltages
			udasBlock->_waveFormType = eWaveFormVoltage;
			udasBlock->_unitMultiplier = UDAS_WAVEFORM_UNIT_V;
		}

		udasBlock->setSeriesName(pChanInfo->szName);
		udasBlock->_timeBetweenSamples = dbPeriod;
		udasBlock->_timeDeltaFromZero = UDAS_NO_OFFSET;
		udasBlock->_timeDuration = duration;

		double range = _max - _min;

		if (range < .25)
			range = .25;

		udasBlock->_dataRange = range;
		udasBlock->_sampleCount = uNumSamples;
		udasBlock->_clockPeriod = UDAS_DUMMY_CONVERT_CLOCK_PERIOD;

		_parent->addBlock(udasBlock);
	}
}
