// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AlpacaDataSeriesLive.h"

AlpacaDataSeriesLive::AlpacaDataSeriesLive
(
	qreal duration,
	const EPMChannel& epmChannel
) :
	DynamicDataSeries(duration, epmChannel)
{
}

AlpacaDataSeriesLive::~AlpacaDataSeriesLive()
{

}

size_t AlpacaDataSeriesLive::size() const
{
	return static_cast<size_t>(count());
}

QPointF AlpacaDataSeriesLive::sample
(
	size_t i
) const
{
	return value(static_cast<uint>(i));
}

QRectF AlpacaDataSeriesLive::boundingRect() const
{
	QRectF rect;

	rect.setLeft(value(0).x());
	rect.setRight(value(kLastEntry).x());
	rect.setTop(yMax());
	rect.setBottom(yMin());

	return rect;
}

void AlpacaDataSeriesLive::getXSampleRange
(
	qreal& min,
	qreal& max
)
{
	min = xMin();
	max = xMax();
}

void AlpacaDataSeriesLive::getYSampleRange
(
	qreal& min,
	qreal& max
)
{
	min = yMin();
	max = yMax();
}
