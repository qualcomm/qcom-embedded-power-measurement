#ifndef LINEMODELENTRYLIVE_H
#define LINEMODELENTRYLIVE_H
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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
