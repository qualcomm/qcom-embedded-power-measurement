// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Reference
// 80-NL175-1_H Kratos UDAS Programming Manual

// QCommonConsole
#include "CountingStream.h"

// libEPM
#include "UDASBlock.h"
#include "UDASDefines.h"
#include "EpmLibPrivate.h"
#include "UDASFile.h"

// QT
#include <QDir>

QString _UDASBlock::channelName() const
{
	return _channelName;
}

HashType _UDASBlock::channelHash() const
{
	return _channelHash;
}

void _UDASBlock::setSeriesName
(
	const QString &seriesName
)
{
	_seriesName = seriesName;
	createChannelName(_seriesName, _seriesHash, _channelName, _channelHash, _waveFormType);
}

HashType _UDASBlock::seriesHash() const
{
	return _seriesHash;
}

qreal _UDASBlock::seriesValue(quint32 index)
{
	qreal result{0.};

	if (index < static_cast<quint32>(_ySeries.count()))
		result = _ySeries.at(index);

	return result;
}

quint32 _UDASBlock::seriesCount()
{
	return _ySeries.count();
}

qreal _UDASBlock::seriesTime(quint32 index)
{
	qreal result(0.0);

	result = _timeBetweenSamples * index;

	return result;
}

void _UDASBlock::setChannelColor(const BasicColor &channelColor)
{
	_channelColor = channelColor;
}

QString _UDASBlock::seriesName() const
{
	return _seriesName;
}

BasicColor _UDASBlock::channelColor() const
{
	return _channelColor;
}

qreal _UDASBlock::duration()
{
	qreal result(0.0);

	result = _timeBetweenSamples * static_cast<qreal>(_sampleCount);

	return result;
}

void _UDASBlock::setRange
(
	qreal timeStart,
	qreal timeEnd
)
{
	Q_ASSERT(timeStart < timeEnd);

	_rangedStatistics.clear();

	_rangeStart = timeStart;
	_rangeEnd = timeEnd;

	quint32 startElement = static_cast<quint32>(_rangeStart / _timeBetweenSamples);
	if (startElement > _sampleCount)
		startElement = _sampleCount - 1;

	quint32 endElement = static_cast<quint32>(_rangeEnd / _timeBetweenSamples);
	if (endElement > _sampleCount)
		endElement = _sampleCount;

	std::span seriesSpan(_ySeries);

	_rangedStatistics.setSpan(seriesSpan.subspan(startElement, endElement - startElement));
}

qreal _UDASBlock::rangeDuration()
{
	return _rangeEnd - _rangeStart;
}

bool _UDASBlock::read
(
	QDataStream &dataStream
)
{
	quint8 temp[1024];
	quint32 waveFormType;

	char* tempPtr(reinterpret_cast<char*>(&temp[0]));

	memset(temp, 0, sizeof(temp));

	dataStream >> _channelNumber;
	dataStream >> waveFormType;

	_waveFormType = static_cast<WaveFormType>(waveFormType);
	dataStream >> _dataFileNumber;

	dataStream.readRawData(tempPtr, kSeriesNameLength);
	setSeriesName(QString(tempPtr));

	dataStream >> _timeBetweenSamples;
	dataStream >> _timeDeltaFromZero;
	dataStream >> _dataRange;

	dataStream >> _sampleCount;
	dataStream >> _unitMultiplier;
	dataStream >> _clockPeriod;

	dataStream >> _5PercentAccuracy;
	dataStream >> _1PercentAccuracy;
	dataStream >> _dcr;
	dataStream >> _saturationCounter;

	readDataSeries(_parent->_paramsPath);

	auto cmp = [](auto a, auto b) { return a < b ; };
	_minSample = *std::min_element(_ySeries.begin(), _ySeries.end(), cmp);
	_maxSample = *std::max_element(_ySeries.begin(), _ySeries.end(), cmp);

	return dataStream.status() == QDataStream::Ok;
}

void _UDASBlock::readDataSeries
(
	const QString& path
)
{
	QString dataPath = QDir::cleanPath(QString("%1").arg(path) + QDir::separator() + QString("Data_%1.scl").arg(_dataFileNumber));
	QFile seriesFile(dataPath);

	if (seriesFile.open(QIODevice::ReadOnly | QIODevice::ExistingOnly) == true)
	{
		QDataStream seriesDataStream(&seriesFile);

		seriesDataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
		seriesDataStream.setByteOrder(QDataStream::BigEndian);

		int sampleIndex(0);

		_ySeries.resize(static_cast<int>(_sampleCount));

		while (sampleIndex < static_cast<int>(_sampleCount) && seriesDataStream.atEnd() == false)
		{
			float y;
			seriesDataStream >> y;

			if (y < _minSample)
				_minSample = y;
			else if (y > _maxSample)
				_maxSample = y;

			_ySeries[sampleIndex] = y;

			sampleIndex++;
		}

		seriesFile.close();
	}

	_statistics.setSpan(_ySeries);
}

void _UDASBlock::write
(
	QDataStream &dataStream
)
{
	CountingStream countingStream;

	countingStream << _channelNumber;
	countingStream << static_cast<quint32>(_waveFormType);
	countingStream << _dataFileNumber;
	countingStream.writeRawData(Q_NULLPTR, kSeriesNameLength);
	countingStream << _timeBetweenSamples;
	countingStream << _timeDeltaFromZero;
	countingStream << _dataRange;
	countingStream << _sampleCount;
	countingStream << _unitMultiplier;
	countingStream << _clockPeriod;
	countingStream << _5PercentAccuracy; //Accuracy Limit 5%
	countingStream << _1PercentAccuracy; //Accuracy Limit 1%
	countingStream << _dcr; //DCR	DOUBLE
	countingStream << _saturationCounter; //Saturation Counter

	// Table 3-2 UDAS dataset block header
	dataStream << kUDASBlockTypeStandardWaveform;
	dataStream << static_cast<quint32>(countingStream.count());

	dataStream << _channelNumber;
	dataStream << _waveFormType;
	dataStream << _dataFileNumber;

	char temp[64];
	char* tempPtr = &temp[0];
	memset(temp, 0, sizeof(temp));
	memcpy(tempPtr, _seriesName.toLatin1().data(), _seriesName.size());
	dataStream.writeRawData(tempPtr, kSeriesNameLength);

	dataStream << _timeBetweenSamples;
	dataStream << _timeDeltaFromZero;
	dataStream << _dataRange;
	dataStream << _sampleCount;
	dataStream << _unitMultiplier;
	dataStream << _clockPeriod;
	dataStream << _5PercentAccuracy; //Accuracy Limit 5%
	dataStream << _1PercentAccuracy; //Accuracy Limit 1%
	dataStream << _dcr; //DCR	DOUBLE
	dataStream << _saturationCounter; //Saturation Counter
}

qreal _UDASBlock::deltaTime()
{
	return _rangeEnd - _rangeStart;
}

qreal _UDASBlock::statAvg(qreal startTime, qreal endTime)
{
	if(startTime != kTimeUnset && endTime != kTimeUnset)
		return _rangedStatistics.avg();

	return _statistics.avg();;
}

qreal _UDASBlock::statDuration(qreal startTime, qreal endTime)
{
	if(startTime != kTimeUnset || endTime != kTimeUnset)
		return rangeDuration();

	return duration();
}

qreal _UDASBlock::statArea(qreal startTime, qreal endTime)
{
	qreal area = duration() * _statistics.avg();

	if(startTime != kTimeUnset && endTime != kTimeUnset)
		area = rangeDuration() * _rangedStatistics.avg();

	return area;
}

qreal _UDASBlock::statPopulationStdDev(qreal startTime, qreal endTime)
{
	if(startTime != kTimeUnset && endTime != kTimeUnset)
		return _rangedStatistics.populationStdDeviation();

	return _statistics.populationStdDeviation();
}

qreal _UDASBlock::statMax(qreal startTime, qreal endTime)
{
	if(startTime != kTimeUnset && endTime != kTimeUnset)
		return _rangedStatistics.max();

	return _statistics.max();
}

qreal _UDASBlock::statRange(qreal startTime, qreal endTime)
{
	qreal range = _statistics.max() - _statistics.min();

	if(startTime != kTimeUnset && endTime != kTimeUnset)
		range = _rangedStatistics.max() - _rangedStatistics.min();

	return range;
}

qreal _UDASBlock::statMin(qreal startTime, qreal endTime)
{
	if(startTime != kTimeUnset && endTime != kTimeUnset)
		return _rangedStatistics.min();

	return _statistics.min();
}
