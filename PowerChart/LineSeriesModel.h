#ifndef LINESERIESMODEL_H
#define LINESERIESMODEL_H
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
