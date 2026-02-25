#ifndef LINESERIESMODEL_H
#define LINESERIESMODEL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "AlpacaDataSeries.h"

// Qt
#include <QtGlobal>

#include "PowerChart_global.h"

class POWERCHART_EXPORT LineSeriesModel
{
public:
	LineSeriesModel() = default;

	void clear();

	AlpacaDataSeriesMapIter begin()
	{
		return _dataSeriesMap.begin();
	}

	AlpacaDataSeriesMapIter end()
	{
		return _dataSeriesMap.end();
	}

	void addDataSeries(AlpacaDataSeries saveMe);
	uint findDataSeries(const QString& seriesName);
	AlpacaDataSeries getDataSeries(uint seriesHash);

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

		auto seriesMapEntry = _dataSeriesMap.find(seriesHash);
		if (seriesMapEntry != _dataSeriesMap.end())
		{
			result.setX((index * _timeIncrement) + _timeStart);
			result.setY((*seriesMapEntry)->_ySeries.at(index));
		}

		return result;
	}

	GraphDataVector getSpan(uint seriesHash, int index, int count)
	{
		GraphDataVector result;

		auto seriesMapEntry = _dataSeriesMap.find(seriesHash);
		if (seriesMapEntry != _dataSeriesMap.end())
		{
			result = (*seriesMapEntry)->_ySeries.mid(index, count);
		}

		return result;
	}

private:
	int							_sampleCount{0};
	qreal						_timeStart{0.0};
	qreal						_timeIncrement{0.0};
	AlpacaDataSeriesMap			_dataSeriesMap;
};

#endif // LINESERIESMODEL_H
