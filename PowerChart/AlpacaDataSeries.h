#ifndef ALPACADATASERIES_H
#define ALPACADATASERIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "DataSeries.h"

// QWT
#include <qwt_series_data.h>
#include <qwt_plot_curve.h>

class _AlpacaDataSeries :
	public _DataSeries,
	public QwtSeriesData<QPointF>,
	public QwtPlotCurve
{
public:
	_AlpacaDataSeries(UDASBlock udasBlock);
	virtual ~_AlpacaDataSeries();

	Q_DISABLE_COPY(_AlpacaDataSeries)

	virtual size_t size() const;
	virtual QPointF sample(size_t i) const;
	virtual QRectF boundingRect() const;

	void getSampleRange(qreal& min, qreal& max);
};

typedef QSharedPointer<_AlpacaDataSeries> AlpacaDataSeries;
typedef QMap<HashType, AlpacaDataSeries> AlpacaDataSeriesMap;
typedef AlpacaDataSeriesMap::iterator AlpacaDataSeriesMapIter;

#endif // ALPACADATASERIES_H
