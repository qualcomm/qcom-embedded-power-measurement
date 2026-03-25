#ifndef LINESERIESMODELLIVE_H
#define LINESERIESMODELLIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "DataPoints.h"
#include "LineModelEntryLive.h"

// Qt
#include <QtGlobal>

#include "PowerChart_global.h"

class POWERCHART_EXPORT LineSeriesModelLive
{
public:
	LineSeriesModelLive();

	void clear();

	void getXSampleRange(qreal& min, qreal& max);
	void getYSampleRange(qreal& min, qreal& max);

	LineModelEntryLiveMapIter dataSeriesBeginIter()
	{
		return _lineModelEntries.begin();
	}

	LineModelEntryLiveMapIter dataSeriesEndIter()
	{
		return _lineModelEntries.end();
	}

	void addDataSeries(LineModelEntryLive saveMe);
	LineModelEntryLive getDataSeries(uint seriesHash);
	LineModelEntryLive getDataSeriesByChannel(quint32 channelNumber);

	void setXValues(int sampleCount, qreal increment, qreal timeStart = 0.0)
	{
		_sampleCount = sampleCount;
		_timeStart = timeStart;
		_timeIncrement = increment;
	}

	qreal duration()
	{
		return _sampleCount * _timeIncrement;
	}

	int count()
	{
		return _sampleCount;
	}

	QPointF getValue(uint seriesHash, int index)
	{
		QPointF result(qQNaN(), qQNaN());

		auto seriesMapEntry = _lineModelEntries.find(seriesHash);
		if (seriesMapEntry != _lineModelEntries.end())
		{
			result = (*seriesMapEntry)->getValue(index);
		}

		return result;
	}

	GraphDataPoints getSpan(uint seriesHash, int index, int count)
	{
		GraphDataPoints result;

		auto seriesMapEntry = _lineModelEntries.find(seriesHash);
		if (seriesMapEntry != _lineModelEntries.end())
		{
			result = (*seriesMapEntry)->getSpan(index, count);
		}

		return result;
	}

private:
	int							_sampleCount;
	qreal						_timeStart;
	qreal						_timeIncrement;
	LineModelEntryLiveMap		_lineModelEntries;
};

#endif // LINESERIESMODEL_H
