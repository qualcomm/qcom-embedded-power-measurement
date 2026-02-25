#ifndef ALPACACURSORLINES_H
#define ALPACACURSORLINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QWT
#include "qtypes.h"
#include <qwt_plot_curve.h>

class AlpacaChart;

// QT
class QMouseEvent;
class QwtPlot;
class QRubberBand;
class QWidget;

class AlpacaCursorLines :
	public QwtPlotItem
{
public:

	enum BandingLine
	{
		eNone,
		eLeft,
		eRight,
	};

	AlpacaCursorLines(AlpacaChart* alpacaChart);

	bool onBoundary(int xpos);

	BandingLine bandingLine();
	qreal left();
	qreal right();

	void setLeftBoundary(qreal boundary);
	void setRightBoundary(qreal boundary);

	virtual void draw(QPainter* painter, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& canvasRect) const;

	void showRubberBand(QMouseEvent* mouseEvent, QwtPlot* parent);
	void moveRubberBand(QMouseEvent* mouseEvent);
	void hideRubberBand(QMouseEvent* mouseEvent);

private:
	QWidget* canvas();
	QRect canvasBounds();

	void setBandingLine(int xPos);

	qreal						_leftBounds{-99999};
	qreal						_rightBounds{99999};
	QRubberBand*				_rubberBand{Q_NULLPTR};
	BandingLine					_bandingLine{eNone};
	AlpacaChart*				_alpacaChart{Q_NULLPTR};
	QwtPlot*					_parent{Q_NULLPTR};
};

#endif // ALPACAZOOMLINES_H
