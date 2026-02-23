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
// Copyright 2020-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
