#ifndef QEPMDATASERIESLIVE_H
#define QEPMDATASERIESLIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPM
#include "DynamicDataSeries.h"

// QWT
#include <qwt_series_data.h>

class QEPMDataSeriesLive :
	public DynamicDataSeries,
	public QwtSeriesData<QPointF>
{
public:
	QEPMDataSeriesLive(qreal duration, const EPMChannel& epmChannel);
	virtual ~QEPMDataSeriesLive();

	Q_DISABLE_COPY(QEPMDataSeriesLive)

	virtual size_t size() const;
	virtual QPointF sample(size_t i) const;
	virtual QRectF boundingRect() const;

	void getXSampleRange(qreal& min, qreal& max);
	void getYSampleRange(qreal& min, qreal& max);
};

#endif // QEPMDATASERIESLIVE_H
