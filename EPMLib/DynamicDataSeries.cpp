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

#include "DynamicDataSeries.h"

// libEPM
#include "EPMLibDefines.h"

DynamicDataSeries::DynamicDataSeries
(
	const qreal duration,
	const EPMChannel& epmChannel
) :
	_duration(duration),
	_seriesDurationCount(duration * MAX_SAMPLES_PER_SECOND)
{
	_ySeries.reserve(_seriesDurationCount);

	_waveFormType = epmChannel->waveForm();
	_channelName = epmChannel->channelName();
	_channelHash = epmChannel->channelHash();
	_channelNumber = epmChannel->channel();

	_seriesName = epmChannel->seriesName();
	_seriesHash = epmChannel->seriesHash();
	_seriesColor = epmChannel->channelColor();
}

void DynamicDataSeries::addDataPoint(QPointF dataPoint)
{
	double value = dataPoint.y();
	if (value > _max)
		_max = value;

	if (value < _min)
		_min = value;

	if (_ySeries.count() == _ySeries.capacity())
		_ySeries.pop_front();

	_ySeries.push_back(dataPoint);
}

QPointF DynamicDataSeries::value(uint i) const
{
	QPointF result(_duration + 1.0, 0.0);

	if (i == kLastEntry)
		i = _ySeries.count() - 1;

	if (i < static_cast<uint>(_ySeries.count()))
		result = _ySeries[i];

	return result;
}
