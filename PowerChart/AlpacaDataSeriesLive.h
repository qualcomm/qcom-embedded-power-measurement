#ifndef ALPACADATASERIESLIVE_H
#define ALPACADATASERIESLIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPM
#include "DynamicDataSeries.h"

// QWT
#include <qwt_series_data.h>

class AlpacaDataSeriesLive :
	public DynamicDataSeries,
	public QwtSeriesData<QPointF>
{
public:
	AlpacaDataSeriesLive(qreal duration, const EPMChannel& epmChannel);
	virtual ~AlpacaDataSeriesLive();

	Q_DISABLE_COPY(AlpacaDataSeriesLive)

	virtual size_t size() const;
	virtual QPointF sample(size_t i) const;
	virtual QRectF boundingRect() const;

	void getXSampleRange(qreal& min, qreal& max);
	void getYSampleRange(qreal& min, qreal& max);
};

#endif // ALPACADATASERIESLIVE_H
