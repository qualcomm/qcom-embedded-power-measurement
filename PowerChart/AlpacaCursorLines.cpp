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

#include "AlpacaCursorLines.h"

#include "AlpacaChart.h"

// qwt
#include <qwt_plot.h>

// Qt
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>

AlpacaCursorLines::AlpacaCursorLines
(
	AlpacaChart* alpacaChart
) :
	QwtPlotItem(),
	_alpacaChart(alpacaChart)
{
	setZ(9999.0);
}

bool AlpacaCursorLines::onBoundary
(
	int xpos
)
{
	bool result(false);

	if (xpos > _leftBounds - 3 && xpos < _leftBounds + 3)
		result = true;
	else if (xpos > _rightBounds - 3 && xpos < _rightBounds + 3)
		result = true;

	return result;
}

AlpacaCursorLines::BandingLine AlpacaCursorLines::bandingLine()
{
	return _bandingLine;
}

qreal AlpacaCursorLines::left()
{
	return plot()->invTransform(QwtPlot::xBottom, _leftBounds);
}

qreal AlpacaCursorLines::right()
{
	return plot()->invTransform(QwtPlot::xBottom, _rightBounds);
}

void AlpacaCursorLines::setLeftBoundary
(
	qreal boundary
)
{
	_leftBounds = plot()->transform(QwtPlot::xBottom, boundary);
	plot()->replot();
}

void AlpacaCursorLines::setRightBoundary
(
	qreal boundary
)
{
	_rightBounds = plot()->transform(QwtPlot::xBottom, boundary);
	plot()->replot();
}

void AlpacaCursorLines::draw
(
	QPainter* painter,
	const QwtScaleMap& xMap,
	const QwtScaleMap& yMap,
	const QRectF& canvasRect
) const
{
	Q_UNUSED(xMap)
	Q_UNUSED(yMap)

	QPen pen;

	pen.setColor(Qt::yellow);
	pen.setWidthF(0.75);

	painter->save();
	painter->setPen(pen);
	painter->drawLine(_leftBounds, static_cast<int>(canvasRect.top()), _leftBounds, static_cast<int>(canvasRect.bottom()));

	pen.setColor(Qt::red);
	painter->setPen(pen);
	painter->drawLine(_rightBounds, static_cast<int>(canvasRect.top()), _rightBounds, static_cast<int>(canvasRect.bottom()));

	painter->restore();
}

void AlpacaCursorLines::showRubberBand
(
	QMouseEvent* mouseEvent,
	QwtPlot* parent
)
{
	if (_rubberBand == Q_NULLPTR)
	{
		_parent = parent;
		_rubberBand = new QRubberBand(QRubberBand::Line, parent);
	}

	QPoint origin = mouseEvent->pos();

	setBandingLine(origin.x());

	QRect bounds(origin.x() + canvasBounds().left(), 0, 3, parent->size().height());
	_rubberBand->setGeometry(bounds);
	_rubberBand->show();
}

void AlpacaCursorLines::moveRubberBand
(
	QMouseEvent* mouseEvent
)
{
	QPoint origin = mouseEvent->pos();
	origin.setY(0);

	QRect canvasRect = canvasBounds();

	switch (_bandingLine)
	{
	case eLeft:
		if (origin.x() < canvasRect.left())
			origin.setX(canvasRect.left());
		if (origin.x() > _rightBounds - 10)
			origin.setX(_rightBounds - 10);
		break;

	case eRight:
		if (origin.x() < _leftBounds + 10)
			origin.setX( _leftBounds + 10);
		if (origin.x() > canvasRect.width())
			origin.setX(canvasRect.width());
		break;

	case eNone:
		if (origin.x() < canvasRect.left())
			origin.setX(canvasRect.left());
		if (origin.x() > canvasRect.width())
			origin.setX(canvasRect.width());
		break;
	}

	origin.setX(origin.x() + canvasRect.left());

	QPoint endPoint = _rubberBand->pos();

	QPointF logicalCoordinates = _alpacaChart->invTransform(endPoint);

	if (_bandingLine == eLeft)
		setLeftBoundary(logicalCoordinates.x());
	else
		setRightBoundary(logicalCoordinates.x());

	_rubberBand->move(origin);
}

void AlpacaCursorLines::hideRubberBand
(
	QMouseEvent* mouseEvent
)
{
	Q_UNUSED(mouseEvent)

	_rubberBand->hide();

	_bandingLine = eNone;
}

QWidget* AlpacaCursorLines::canvas()
{
	return _parent->canvas();
}

QRect AlpacaCursorLines::canvasBounds()
{
	QRect result;

	result = canvas()->geometry();

	return result;
}

void AlpacaCursorLines::setBandingLine
(
	int xPos
)
{
	_bandingLine = eNone;

	if (xPos > _leftBounds - 3 && xPos < _leftBounds + 3)
		_bandingLine = eLeft;
	else if (xPos > _rightBounds - 3 && xPos < _rightBounds + 3)
		_bandingLine = eRight;
}
