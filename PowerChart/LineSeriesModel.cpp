// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "LineSeriesModel.h"


void LineSeriesModel::clear()
{
	_sampleCount = 0;
	_timeStart = 0.;
	_timeIncrement = 0.;

	_dataSeriesMap.clear();
}

void LineSeriesModel::addDataSeries
(
	AlpacaDataSeries saveMe
)
{
	if (saveMe->seriesHash() != 0)
	{
		_dataSeriesMap[saveMe->seriesHash()] = saveMe;
	}
}

uint LineSeriesModel::findDataSeries
(
	const QString& seriesName
)
{
	uint hash(0);

	auto dataSeriesIter = _dataSeriesMap.begin();
	while (dataSeriesIter != _dataSeriesMap.end())
	{
		if (dataSeriesIter.value()->seriesName() == seriesName)
		{
			hash = dataSeriesIter.key();
			break;
		}

		dataSeriesIter++;
	}

	return hash;
}

AlpacaDataSeries LineSeriesModel::getDataSeries
(
	uint seriesHash
)
{
	AlpacaDataSeries result;

	auto dataSeriesIter = _dataSeriesMap.find(seriesHash);
	if (dataSeriesIter != _dataSeriesMap.end())
		result = dataSeriesIter.value();

	return result;
}
