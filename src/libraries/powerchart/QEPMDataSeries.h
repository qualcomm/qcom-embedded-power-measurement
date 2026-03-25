#ifndef QEPMDATASERIES_H
#define QEPMDATASERIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "DataSeries.h"

// QWT
#include <qwt_series_data.h>
#include <qwt_plot_curve.h>

class _QEPMDataSeries :
	public _DataSeries,
	public QwtSeriesData<QPointF>,
	public QwtPlotCurve
{
public:
	_QEPMDataSeries(UDASBlock udasBlock);
	virtual ~_QEPMDataSeries();

	Q_DISABLE_COPY(_QEPMDataSeries)

	virtual size_t size() const;
	virtual QPointF sample(size_t i) const;
	virtual QRectF boundingRect() const;

	void getSampleRange(qreal& min, qreal& max);
};

typedef QSharedPointer<_QEPMDataSeries> QEPMDataSeries;
typedef QMap<HashType, QEPMDataSeries> QEPMDataSeriesMap;
typedef QEPMDataSeriesMap::iterator QEPMDataSeriesMapIter;

#endif // QEPMDATASERIES_H
