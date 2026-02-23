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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

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
