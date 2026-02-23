#ifndef ALPACACURSORLINES_H
#define ALPACACURSORLINES_H
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
