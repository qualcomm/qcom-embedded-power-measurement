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

#include "LineModelEntryLive.h"

// UI
#include "ColorConversion.h"

// Powerchart
#include "AlpacaDataSeriesLive.h"

_LineModelEntryLive::_LineModelEntryLive
(
	QwtPlot* plot,
	const qreal duration,
	const EPMChannel& epmChannel
) :
	_plot(plot)
{
	_dataSeries = new AlpacaDataSeriesLive(duration, epmChannel);
	_plotCurve = new QwtPlotCurve;

	_plotCurve->setData(_dataSeries);

	_plotCurve->setPen(ColorConversion::BasicToColor(_dataSeries->seriesColor()), .5);
	_plotCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
	_plotCurve->setVisible(true);
	_plotCurve->attach(_plot);
}

_LineModelEntryLive::~_LineModelEntryLive()
{
	_plotCurve->detach();
	_dataSeries = Q_NULLPTR;  // Owned by the _plotCurve!!!!
}

void _LineModelEntryLive::addDataPoint(QPointF dataPoint)
{
	_dataSeries->addDataPoint(dataPoint);
}

uint _LineModelEntryLive::channelNumber()
{
	return _dataSeries->channelNumber();
}

uint _LineModelEntryLive::seriesHash()
{
	return _dataSeries->seriesHash();
}

QColor _LineModelEntryLive::seriesColor()
{
	return ColorConversion::BasicToColor(_dataSeries->seriesColor());
}

GraphDataPoints _LineModelEntryLive::getSpan
(
	int index,
	int count
)
{
	return _dataSeries->_ySeries.mid(index, count);
}

QPointF _LineModelEntryLive::getValue(uint index)
{
	QPointF result(_dataSeries->duration() + 1.0, 0.0);

	if (index < static_cast<uint>(_dataSeries->_ySeries.count()))
		result = _dataSeries->_ySeries[index];

	return result;
}

