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
