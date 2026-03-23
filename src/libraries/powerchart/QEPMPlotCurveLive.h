#ifndef QEPMPLOTCURVELIVE_H
#define QEPMPLOTCURVELIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPM
#include "DynamicDataSeries.h"

// QWT
#include <qwt_plot_curve.h>

class QEPMPlotCurveLive :
	public QwtPlotCurve
{
public:
	QEPMPlotCurveLive(const quint64 reservedPoints, const EPMChannel& epmChannel);
	virtual ~QEPMPlotCurveLive();

	Q_DISABLE_COPY(QEPMPlotCurveLive)
};

#endif // QEPMPLOTCURVELIVE_H
