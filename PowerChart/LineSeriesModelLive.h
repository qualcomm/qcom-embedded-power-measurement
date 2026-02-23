#ifndef LINESERIESMODELLIVE_H
#define LINESERIESMODELLIVE_H
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
