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
// Copyright 2018-2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
