// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMDataSeries.h"

_QEPMDataSeries::_QEPMDataSeries
(
	UDASBlock udasBlock
) :
	_DataSeries(udasBlock)
{
}

_QEPMDataSeries::~_QEPMDataSeries()
{

}

size_t _QEPMDataSeries::size() const
{
	return static_cast<size_t>(count());
}

QPointF _QEPMDataSeries::sample
(
	size_t i
) const
{
	return value(static_cast<uint>(i));
}

QRectF _QEPMDataSeries::boundingRect() const
{
	QRectF result;

	result.setLeft(0.0);
	result.setRight(duration());
	result.setTop(const_cast<_QEPMDataSeries*>(this)->max());
	result.setBottom(const_cast<_QEPMDataSeries*>(this)->min());

	return result;
}

void _QEPMDataSeries::getSampleRange
(
	qreal& min,
	qreal& max
)
{
	min = minSample();
	max = maxSample();
}
