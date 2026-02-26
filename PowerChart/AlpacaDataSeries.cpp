// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AlpacaDataSeries.h"

_AlpacaDataSeries::_AlpacaDataSeries
(
	UDASBlock udasBlock
) :
	_DataSeries(udasBlock)
{
}

_AlpacaDataSeries::~_AlpacaDataSeries()
{

}

size_t _AlpacaDataSeries::size() const
{
	return static_cast<size_t>(count());
}

QPointF _AlpacaDataSeries::sample
(
	size_t i
) const
{
	return value(static_cast<uint>(i));
}

QRectF _AlpacaDataSeries::boundingRect() const
{
	QRectF result;

	result.setLeft(0.0);
	result.setRight(duration());
	result.setTop(const_cast<_AlpacaDataSeries*>(this)->max());
	result.setBottom(const_cast<_AlpacaDataSeries*>(this)->min());

	return result;
}

void _AlpacaDataSeries::getSampleRange
(
	qreal& min,
	qreal& max
)
{
	min = minSample();
	max = maxSample();
}
