// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "HorizontalPlotZoomer.h"

#include <QPen>
#include <QRectF>

HorizontalPlotZoomer::HorizontalPlotZoomer
(
	QWidget* canvas,
	bool replot
):
	QwtPlotZoomer(canvas, replot)
{
	setRubberBandPen(QPen(Qt::white));
}

void HorizontalPlotZoomer::zoom
(
	const QRectF& zoomRect
)
{
	QRectF zoomRectClone = zoomRect;
	QRectF baseRect = zoomBase();
	zoomRectClone.setTop(baseRect.top());
	zoomRectClone.setBottom(baseRect.bottom());

	if (zoomRectClone.left() < baseRect.left()) zoomRectClone.setLeft(baseRect.left());
	if (zoomRectClone.right() > baseRect.right()) zoomRectClone.setRight(baseRect.right());

	QwtPlotZoomer::zoom(zoomRectClone);
}
