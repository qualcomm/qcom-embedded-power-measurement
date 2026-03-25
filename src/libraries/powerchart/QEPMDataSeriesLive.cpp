// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMDataSeriesLive.h"

QEPMDataSeriesLive::QEPMDataSeriesLive
(
	qreal duration,
	const EPMChannel& epmChannel
) :
	DynamicDataSeries(duration, epmChannel)
{
}

QEPMDataSeriesLive::~QEPMDataSeriesLive()
{

}

size_t QEPMDataSeriesLive::size() const
{
	return static_cast<size_t>(count());
}

QPointF QEPMDataSeriesLive::sample
(
	size_t i
) const
{
	return value(static_cast<uint>(i));
}

QRectF QEPMDataSeriesLive::boundingRect() const
{
	QRectF rect;

	rect.setLeft(value(0).x());
	rect.setRight(value(kLastEntry).x());
	rect.setTop(yMax());
	rect.setBottom(yMin());

	return rect;
}

void QEPMDataSeriesLive::getXSampleRange
(
	qreal& min,
	qreal& max
)
{
	min = xMin();
	max = xMax();
}

void QEPMDataSeriesLive::getYSampleRange
(
	qreal& min,
	qreal& max
)
{
	min = yMin();
	max = yMax();
}
