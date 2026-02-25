// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// PowerChart
#include "AlpacaChart.h"
#include "AlpacaDataSeries.h"
#include "AlpacaCursorLines.h"
#include "HorizontalPlotZoomer.h"
#include "ui_AlpacaChart.h"

// UILib
#include "ColorConversion.h"

// QCommon
#include "QualcommColors.h"

// qwt
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_scale_widget.h>
#include <qwt_series_store.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>

// Qt
#include <QApplication>
#include <QMouseEvent>
#include <QStack>

// C++
#include <limits>

AlpacaChart::AlpacaChart
(
	QWidget* parent
) :
	QWidget(parent),
	_ui(new Ui_AlpacaChartClass),
	_plotGrid(new QwtPlotGrid),
	_cursorLines(new AlpacaCursorLines(this))
{
	_ui->setupUi(this);

	_ui->_xAxis->setPlot(_ui->_plot);

	_ui->_yAxis->setPlot(_ui->_plot);
	_ui->_yAxis->setOrientation(Qt::Vertical);

	setGraphBackground(Qt::black);
	BasicColor titleGray = kTitleGray;
	setTitleColor(QColor(titleGray.red(), titleGray.green(), titleGray.blue()));

	setXTitle("Time");

	_ui->_plot->enableAxis(QwtPlot::yLeft, false);
	_ui->_plot->enableAxis(QwtPlot::xBottom, false);

	setXAxis(10, 4);
	setYAxis(5, 4);

	setAxisFont(QFont("Segoe UI", 7));
	setTitleFont(QFont("Segoe UI", 10));

	_plotGrid->setMajorPen(Qt::white, .50);
	_plotGrid->enableXMin(true);
	_plotGrid->enableYMin(true);
	_plotGrid->setMinorPen(Qt::white, .25, Qt::DashLine);
	_plotGrid->attach(_ui->_plot);

	_cursorLines->attach(_ui->_plot);

	_okayZoomer = new HorizontalPlotZoomer(_ui->_plot->canvas(), true);

	//setMouseTracking(true);
	_ui->_plot->canvas()->setMouseTracking(true);
	_ui->_plot->canvas()->installEventFilter(this);

//	connect(_okayZoomer, &QwtPlotZoomer::zoomed, [this](const QRectF& zoomRect){
//		_ui->_xAxis->repaint();
//		_ui->_yAxis->repaint();
//		emit zoomRectChanged(zoomRect);
//	});
}

AlpacaChart::~AlpacaChart()
{
	if (_okayZoomer != Q_NULLPTR)
	{
		delete _okayZoomer;
		_okayZoomer = Q_NULLPTR;
	}

	if (_plotGrid != Q_NULLPTR)
	{
		_plotGrid->detach();
		delete _plotGrid;
		_plotGrid = Q_NULLPTR;
	}

	if (_cursorLines != Q_NULLPTR)
	{
		_cursorLines->detach();
		delete _cursorLines;
		_cursorLines = Q_NULLPTR;
	}

	delete _ui;
}

void AlpacaChart::setSeriesColor
(
	UDASBlock udasBlock
)
{
	uint seriesHash = udasBlock->seriesHash();

	AlpacaDataSeries alpacaDataSeries = _lineSeriesModel.getDataSeries(seriesHash);

	if (alpacaDataSeries.isNull() == false)
	{
		BasicColor newColor = udasBlock->channelColor();
		alpacaDataSeries->setPen(QColor(newColor.red(), newColor.green(), newColor.blue()), .5);

		replot();
	}
}

uint AlpacaChart::addLineSeries
(
	UDASBlock udasBlock
)
{
	AlpacaDataSeries alpacaSeries = AlpacaDataSeries(new _AlpacaDataSeries(udasBlock));

	alpacaSeries->setData(alpacaSeries.data());
	BasicColor seriesColor = alpacaSeries->seriesColor();
	alpacaSeries->setPen(QColor(seriesColor.red(), seriesColor.green(), seriesColor.blue()), .5);
	alpacaSeries->setRenderHint( QwtPlotItem::RenderAntialiased, true );
	alpacaSeries->setVisible(false);
	alpacaSeries->attach(_ui->_plot);

	_lineSeriesModel.addDataSeries(alpacaSeries);

	_active = true;

	return alpacaSeries->seriesHash();
}

void AlpacaChart::setGraphBackground(const QColor &backgroundColor)
{
	_ui->_plot->setCanvasBackground(backgroundColor);
}

void AlpacaChart::setXTitle(const QString &title)
{
	_ui->_xLabel->setText(title);
}

void AlpacaChart::setYTitle(const QString &title)
{
	_ui->_yLabel->setText(title);
}

void AlpacaChart::setXAxis(int major, int minor)
{
	_ui->_plot->setAxisMaxMajor(QwtPlot::xBottom, major);
	_ui->_plot->setAxisMaxMinor(QwtPlot::xBottom, minor);
}

void AlpacaChart::setXRange(qreal min, qreal max)
{
	_ui->_plot->setAxisScale(QwtPlot::xBottom, min, max);
}

void AlpacaChart::setYAxis(int major, int minor)
{
	_ui->_plot->setAxisMaxMajor(QwtPlot::yLeft, major);
	_ui->_plot->setAxisMaxMinor(QwtPlot::yLeft, minor);
}

void AlpacaChart::setYRange(qreal min, qreal max)
{
	_ui->_plot->setAxisScale(QwtPlot::yLeft, min, max);

	if (_active)
	{
		QRectF zoomBase(0, min, _lineSeriesModel.duration(), max - min);
		_okayZoomer->setZoomBase(zoomBase);
	}
}

void AlpacaChart::setAxisFont(const QFont &font)
{
	_ui->_plot->QwtPlot::setAxisFont(QwtPlot::yLeft, font);
	_ui->_plot->QwtPlot::setAxisFont(QwtPlot::xBottom, font);
}

void AlpacaChart::setTitleFont(const QFont &font)
{
	QwtText axisTitleText;

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::xBottom);
	axisTitleText.setFont(font);
	_ui->_plot->setAxisTitle(QwtPlot::xBottom, axisTitleText);

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::yLeft);
	axisTitleText.setFont(font);
	_ui->_plot->setAxisTitle(QwtPlot::yLeft, axisTitleText);
}

void AlpacaChart::setTitleColor(const QColor &color)
{
	QwtText axisTitleText;

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::xBottom);
	axisTitleText.setColor(color);
	_ui->_plot->setAxisTitle(QwtPlot::xBottom, axisTitleText);

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::yLeft);
	axisTitleText.setColor(color);
	_ui->_plot->setAxisTitle(QwtPlot::yLeft, axisTitleText);
}

void AlpacaChart::setSeriesState
(
	uint seriesHash,
	bool state
)
{
	auto plotCurveIter = _lineSeriesModel.getDataSeries(seriesHash);
	if (plotCurveIter.isNull() == false)
	{
		plotCurveIter->setVisible(state);
	}
}

void AlpacaChart::calculateRange
(
	qreal& min,
	qreal& max
)
{
	bool sampleSeen(false);

	min = std::numeric_limits<qreal>::max();
	max = std::numeric_limits<qreal>::min();

	for (const auto& plotCurve: _lineSeriesModel)
	{
		if (plotCurve->isVisible() == true)
		{
			qreal dataSeriesMin;
			qreal dataSeriesMax;

			plotCurve->getSampleRange(dataSeriesMin, dataSeriesMax);

			if (dataSeriesMin < min)
				min = dataSeriesMin;

			if (dataSeriesMax > max)
				max = dataSeriesMax;

			sampleSeen = true;
		}
	}

	if (sampleSeen == false)
	{
		min = 0.0;
		max = 10.0;
	}
	else
	{
		qreal span = max - min;

		if (span <= .05)
			span = .025;
		else if (span <= .1)
			span = .05;
		else if (span <= .25)
			span = .15;
		else if (span <= .5)
			span = .25;
		else if (span <= 1.0)
			span = .5;

		if (span <= 1)
		{
			min = floor(min - span);
			max = ceil(max + span);
		}
		else
		{
			if (span < 2.0)
			{
				min = floor(static_cast<qreal>(static_cast<int>(min - .5)) - .5);
				max = ceil(static_cast<qreal>(static_cast<int>(max + .5)) + .5);
			}
			else if (span <= 10.0)
			{
				min = floor(static_cast<qreal>(static_cast<int>(min - 1)));
				max = ceil(static_cast<qreal>(static_cast<int>(max + 1)));
			}
			else if (span <= 50.0)
			{
				min = floor(static_cast<qreal>(static_cast<int>(min - 2)));
				max = ceil(static_cast<qreal>(static_cast<int>(max + 2)));
			}
			else
			{
				min = floor(static_cast<qreal>(static_cast<int>(min - 10)));
				max = ceil(static_cast<qreal>(static_cast<int>(max + 10)));
			}
		}
	}
}

void AlpacaChart::setCursorLines(qreal min, qreal max)
{
	_cursorLines->setLeftBoundary(min);
	_cursorLines->setRightBoundary(max);
}

void AlpacaChart::setZoomBase(const QRectF &zoomBase)
{
	_okayZoomer->setZoomBase(zoomBase);
}

void AlpacaChart::setXZoom(qreal left, qreal right)
{
	QRectF current = _okayZoomer->zoomRect();
	current.setLeft(left);
	current.setRight(right);
	_okayZoomer->zoom(current);
}

void AlpacaChart::zoomToCursor()
{
	QRectF current = _okayZoomer->zoomRect();
	current.setLeft(_cursorLines->left());
	current.setRight(_cursorLines->right());
	_okayZoomer->zoom(current);
}

void AlpacaChart::zoomIn()
{
	uint stackSize = static_cast<uint>(_okayZoomer->zoomStack().count());
	uint newRectIndex = _okayZoomer->zoomRectIndex() + 1;
	_okayZoomer->setZoomStack(_okayZoomer->zoomStack(), newRectIndex >= stackSize ? stackSize - 1 : newRectIndex);
	_ui->_xAxis->update();
	_ui->_yAxis->update();
}

void AlpacaChart::zoomOut()
{
	int newRectIndex = static_cast<int>(_okayZoomer->zoomRectIndex()) - 1;
	_okayZoomer->setZoomStack(_okayZoomer->zoomStack(), newRectIndex < 0 ? 0 : newRectIndex);
	_ui->_xAxis->update();
	_ui->_yAxis->update();
}

void AlpacaChart::resetZoom()
{
	_okayZoomer->zoom(_okayZoomer->zoomBase());
}

qreal AlpacaChart::cursorT0()
{
	return _cursorLines == Q_NULLPTR ? 0. : _cursorLines->left();
}

void AlpacaChart::setCursorT0(qreal logicalValue)
{
	if (_cursorLines) _cursorLines->setLeftBoundary(logicalValue);
}

qreal AlpacaChart::cursorT1()
{
	return _cursorLines == Q_NULLPTR ? 0. : _cursorLines->right();
}

void AlpacaChart::setCursorT1(qreal logicalValue)
{
	if (_cursorLines) _cursorLines->setRightBoundary(logicalValue);
}

void AlpacaChart::replot()
{
	_ui->_yAxis->update();
	_ui->_plot->replot();
}

bool AlpacaChart::eventFilter
(
	QObject* obj,
	QEvent* event
)
{
	bool result{false};

	if (_active && obj == _ui->_plot->canvas())
	{
		QPoint pos;
		QMouseEvent* mouseEvent(Q_NULLPTR);

		QPointF logicalCoordinates;

		switch (event->type())
		{
		case QEvent::MouseMove:
			mouseEvent = reinterpret_cast<QMouseEvent*>(event);

			pos = mouseEvent->pos();

			logicalCoordinates = invTransform(pos);
			emit hoveredCoordChange(generateHoveredText(logicalCoordinates));

			if (_tracking)
			{
				if (mouseEvent->buttons() == Qt::NoButton)
				{
					_tracking = false;
					_cursorLines->hideRubberBand(mouseEvent);
				}
				else
				{
					_cursorLines->moveRubberBand(mouseEvent);

					emitBandingPosition();
				}
				result = true;
			}
			else
			{
				if (_cursorLines->onBoundary(pos.x()))
				{
					_ui->_plot->canvas()->setCursor(Qt::SizeHorCursor);
					result = true;
				}
				else
				{
					_ui->_plot->canvas()->setCursor(Qt::ArrowCursor);
				}
			}

			break;

		case QEvent::Leave:
			emit hoveredCoordChange("");
			break;

		case QEvent::MouseButtonPress:

			mouseEvent = static_cast<QMouseEvent*>(event);

			pos = mouseEvent->pos();

			if (_cursorLines->onBoundary(pos.x()))
			{
				_tracking = true;
				_cursorLines->showRubberBand(mouseEvent, _ui->_plot);

				result = true;
			}

			break;

		case QEvent::MouseButtonRelease:
			if (_tracking)
			{
				mouseEvent = static_cast<QMouseEvent*>(event);

				emitBandingComplete();

				_cursorLines->hideRubberBand(mouseEvent);

				_tracking = false;
				result = true;
			}

			break;

		default:
			result = _ui->_plot->QwtPlot::eventFilter(obj, event);
			break;
		}
	}
	else
	{
		result = _ui->_plot->QwtPlot::eventFilter(obj, event);
	}

	return result;
}

void AlpacaChart::emitBandingPosition()
{
	switch (_cursorLines->bandingLine())
	{
	case AlpacaCursorLines::eLeft:
		emit t0Changed(_cursorLines->left());
		break;

	case AlpacaCursorLines::eRight:
		emit t1Changed(_cursorLines->right());
		break;

	case AlpacaCursorLines::eNone:
		break;
	}
}

void AlpacaChart::emitBandingComplete()
{
	switch (_cursorLines->bandingLine())
	{
	case AlpacaCursorLines::eLeft:
		emit t0Complete(_cursorLines->left());
		break;

	case AlpacaCursorLines::eRight:
		emit t1Complete(_cursorLines->right());
		break;

	case AlpacaCursorLines::eNone:
		break;
	}
}

QPointF AlpacaChart::invTransform(const QPoint &invMe)
{
	return QPointF(_ui->_plot->invTransform(QwtPlot::xBottom, invMe.x()), _ui->_plot->invTransform(QwtPlot::yLeft, invMe.y()));
}

QPoint AlpacaChart::transform(const QPointF& transformMe)
{
	return QPoint(_ui->_plot->transform(QwtPlot::xBottom, transformMe.x()), _ui->_plot->transform(QwtPlot::yLeft, transformMe.y()));
}


