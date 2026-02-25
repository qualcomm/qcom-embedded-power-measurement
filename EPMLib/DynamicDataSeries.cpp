// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
