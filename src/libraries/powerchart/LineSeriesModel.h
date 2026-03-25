#ifndef LINESERIESMODEL_H
#define LINESERIESMODEL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QEPMDataSeries.h"

// Qt
#include <QtGlobal>

#include "PowerChart_global.h"

class POWERCHART_EXPORT LineSeriesModel
{
public:
	LineSeriesModel() = default;

	void clear();

	QEPMDataSeriesMapIter begin()
	{
		return _dataSeriesMap.begin();
	}

	QEPMDataSeriesMapIter end()
	{
		return _dataSeriesMap.end();
	}

	void addDataSeries(QEPMDataSeries saveMe);
	uint findDataSeries(const QString& seriesName);
	QEPMDataSeries getDataSeries(uint seriesHash);

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
	QEPMDataSeriesMap			_dataSeriesMap;
};

#endif // LINESERIESMODEL_H
