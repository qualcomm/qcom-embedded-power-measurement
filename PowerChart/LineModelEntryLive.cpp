// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

