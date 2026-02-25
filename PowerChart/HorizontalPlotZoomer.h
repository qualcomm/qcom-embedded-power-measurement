#ifndef OKZOOMER_H
#define OKZOOMER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <qwt_plot_zoomer.h>

class HorizontalPlotZoomer : public QwtPlotZoomer
{
public:
	HorizontalPlotZoomer(QWidget* canvas, bool replot = true);

	void zoom(const QRectF& ) override;
};

#endif // OKZOOMER_H
