// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMCursorLines.h"

#include "QEPMChart.h"

// qwt
#include <qwt_plot.h>

// Qt
#include <QMouseEvent>
#include <QPainter>
#include <QRubberBand>

QEPMCursorLines::QEPMCursorLines
(
	QEPMChart* QEPMChart
) :
	QwtPlotItem(),
	_QEPMChart(QEPMChart)
{
	setZ(9999.0);
}

bool QEPMCursorLines::onBoundary
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

QEPMCursorLines::BandingLine QEPMCursorLines::bandingLine()
{
	return _bandingLine;
}

qreal QEPMCursorLines::left()
{
	return plot()->invTransform(QwtPlot::xBottom, _leftBounds);
}

qreal QEPMCursorLines::right()
{
	return plot()->invTransform(QwtPlot::xBottom, _rightBounds);
}

void QEPMCursorLines::setLeftBoundary
(
	qreal boundary
)
{
	_leftBounds = plot()->transform(QwtPlot::xBottom, boundary);
	plot()->replot();
}

void QEPMCursorLines::setRightBoundary
(
	qreal boundary
)
{
	_rightBounds = plot()->transform(QwtPlot::xBottom, boundary);
	plot()->replot();
}

void QEPMCursorLines::draw
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

void QEPMCursorLines::showRubberBand
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

void QEPMCursorLines::moveRubberBand
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

	QPointF logicalCoordinates = _QEPMChart->invTransform(endPoint);

	if (_bandingLine == eLeft)
		setLeftBoundary(logicalCoordinates.x());
	else
		setRightBoundary(logicalCoordinates.x());

	_rubberBand->move(origin);
}

void QEPMCursorLines::hideRubberBand
(
	QMouseEvent* mouseEvent
)
{
	Q_UNUSED(mouseEvent)

	_rubberBand->hide();

	_bandingLine = eNone;
}

QWidget* QEPMCursorLines::canvas()
{
	return _parent->canvas();
}

QRect QEPMCursorLines::canvasBounds()
{
	QRect result;

	result = canvas()->geometry();

	return result;
}

void QEPMCursorLines::setBandingLine
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
