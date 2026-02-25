#ifndef LINEMODELENTRYLIVE_H
#define LINEMODELENTRYLIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PowerChart_global.h"

// PowerChart
#include "AlpacaDataSeriesLive.h"

// QWT
#include <qwt_series_data.h>
#include <qwt_plot_curve.h>

// QT
#include <QMap>

class AlpacaDataSeriesLive;

class POWERCHART_EXPORT _LineModelEntryLive
{
public:
	_LineModelEntryLive(QwtPlot* plot, const qreal duration, const EPMChannel& epmChannel);
	virtual	~_LineModelEntryLive();

	Q_DISABLE_COPY(_LineModelEntryLive)

	QwtPlotCurve* plotCurve()
	{
		return _plotCurve;
	}

	void addDataPoint(QPointF dataPoint);

	uint channelNumber();
	uint seriesHash();
	QColor seriesColor();

	GraphDataPoints getSpan(int index, int count);
	QPointF	getValue(uint index);

	void getXSampleRange(qreal& min, qreal& max)
	{
		return _dataSeries->getXSampleRange(min, max);
	}

	void getYSampleRange(qreal& min, qreal& max)
	{
		return _dataSeries->getYSampleRange(min, max);
	}

private:
	QwtPlot*					_plot{Q_NULLPTR};
	AlpacaDataSeriesLive*		_dataSeries{Q_NULLPTR};
	QwtPlotCurve*				_plotCurve{Q_NULLPTR};
};

typedef QSharedPointer<_LineModelEntryLive> LineModelEntryLive;
typedef QMap<uint, LineModelEntryLive> LineModelEntryLiveMap;
typedef LineModelEntryLiveMap::iterator LineModelEntryLiveMapIter;

#endif // LINEMODELENTRYLIVE_H
