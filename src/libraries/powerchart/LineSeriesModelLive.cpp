// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "LineSeriesModelLive.h"

LineSeriesModelLive::LineSeriesModelLive()
{
	_sampleCount = 0;
	_timeStart = 0;
	_timeIncrement = 0;
}

void LineSeriesModelLive::clear()
{
	_sampleCount = 0;
	_timeStart = 0;
	_timeIncrement = 0;

	_lineModelEntries.clear();
}

void LineSeriesModelLive::getXSampleRange(qreal &min, qreal &max)
{

	for (const auto& lineModelEntry: std::as_const(_lineModelEntries))
	{
		lineModelEntry->getXSampleRange(min, max);
		continue; // we really on want the first
	}
}

void LineSeriesModelLive::getYSampleRange
(
	qreal& min, qreal& max
)
{
	min = 0.;
	max = 0.;

	for (const auto& lineModelEntry: std::as_const(_lineModelEntries))
	{
		qreal entryMin, entryMax;

		lineModelEntry->getYSampleRange(entryMin, entryMax);

		if (entryMin < min)
			min = entryMin;

		if (entryMax > max)
			max = entryMax;
	}
}

void LineSeriesModelLive::addDataSeries
(
	LineModelEntryLive saveMe
)
{
	if (saveMe->seriesHash() != 0)
	{
		_lineModelEntries[saveMe->seriesHash()] = saveMe;
	}
}

LineModelEntryLive LineSeriesModelLive::getDataSeries
(
	uint seriesHash
)
{
	LineModelEntryLive result;

	auto dataSeriesIter = _lineModelEntries.find(seriesHash);
	if (dataSeriesIter != _lineModelEntries.end())
		result = dataSeriesIter.value();

	return result;
}

LineModelEntryLive LineSeriesModelLive::getDataSeriesByChannel
(
	quint32 channelNumber
)
{
	LineModelEntryLive result;

	for (const auto& dataSeries: std::as_const(_lineModelEntries))
	{
		if (dataSeries->channelNumber() == channelNumber)
		{
			result = dataSeries;
			break;
		}
	}

	return result;
}
