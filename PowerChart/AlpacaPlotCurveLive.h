#ifndef ALPACAPLOTCURVELIVE_H
#define ALPACAPLOTCURVELIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPM
#include "DynamicDataSeries.h"

// QWT
#include <qwt_plot_curve.h>

class AlpacaPlotCurveLive :
	public QwtPlotCurve
{
public:
	AlpacaPlotCurveLive(const quint64 reservedPoints, const EPMChannel& epmChannel);
	virtual ~AlpacaPlotCurveLive();

	Q_DISABLE_COPY(AlpacaPlotCurveLive)
};

#endif // ALPACAPLOTCURVELIVE_H
