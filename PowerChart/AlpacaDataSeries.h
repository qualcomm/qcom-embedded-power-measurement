#ifndef ALPACADATASERIES_H
#define ALPACADATASERIES_H
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
