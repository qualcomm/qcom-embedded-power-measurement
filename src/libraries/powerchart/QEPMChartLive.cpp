// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// PowerChart
#include "QEPMChartLive.h"
#include "LineModelEntryLive.h"
#include "ui_QEPMChartLive.h"

// UI
#include "ColorConversion.h"

// libEPM
#include "EPMLibDefines.h"

// QCommon
#include "QualcommColors.h"

// qwt
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_scale_widget.h>
#include <qwt_series_store.h>

// Qt
#include <QApplication>
#include <QMouseEvent>

// C++
#include <limits>

QEPMChartLive::QEPMChartLive
(
	QWidget* parent
) :
	QWidget(parent),
	_ui(new Ui_QEPMChartLiveClass),
	_plotGrid(new QwtPlotGrid)
{
	_ui->setupUi(this);

	_ui->_xAxis->setPlot(_ui->_plot);

	_ui->_yAxis->setPlot(_ui->_plot);
	_ui->_yAxis->setOrientation(Qt::Vertical);

	setGraphBackground(Qt::black);
	setTitleColor(ColorConversion::BasicToColor(kTitleGray));

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

	_ui->_plot->canvas()->setMouseTracking(true);
	_ui->_plot->canvas()->installEventFilter(this);
}

QEPMChartLive::~QEPMChartLive()
{
	_lineSeriesModel.clear();

	if (_plotGrid != Q_NULLPTR)
	{
		_plotGrid->detach();
		delete _plotGrid;
		_plotGrid = Q_NULLPTR;
	}

	delete _ui;
}

void QEPMChartLive::clearLineSeries()
{
	_lineSeriesModel.clear();

	if (_plotGrid != Q_NULLPTR)
	{
		_plotGrid->detach();
		delete _plotGrid;
		_plotGrid = Q_NULLPTR;
	}
}

void QEPMChartLive::setDuration(qreal seconds)
{
	_duration = seconds;
}

void QEPMChartLive::addLineSeries
(
	EPMChannel epmChannel
)
{
	LineModelEntryLive lineModelEntry = LineModelEntryLive(new _LineModelEntryLive(_ui->_plot, _duration, epmChannel));

	_lineSeriesModel.addDataSeries(lineModelEntry);
}

void QEPMChartLive::setGraphBackground(const QColor &backgroundColor)
{
	_ui->_plot->setCanvasBackground(backgroundColor);
}

void QEPMChartLive::setXTitle(const QString &title)
{
	_ui->_xLabel->setText(title);
}

void QEPMChartLive::setYTitle(const QString &title)
{
	_ui->_yLabel->setText(title);
}

void QEPMChartLive::setXAxis(int major, int minor)
{
	_ui->_plot->setAxisMaxMajor(QwtPlot::xBottom, major);
	_ui->_plot->setAxisMaxMinor(QwtPlot::xBottom, minor);
}

void QEPMChartLive::setXRange(qreal min, qreal max)
{
	_ui->_plot->setAxisScale(QwtPlot::xBottom, min, max);
}

void QEPMChartLive::setYAxis(int major, int minor)
{
	_ui->_plot->setAxisMaxMajor(QwtPlot::yLeft, major);
	_ui->_plot->setAxisMaxMinor(QwtPlot::yLeft, minor);
}

void QEPMChartLive::setYRange(qreal min, qreal max)
{
	_ui->_plot->setAxisScale(QwtPlot::yLeft, min, max);
}

void QEPMChartLive::setAxisFont(const QFont &font)
{
	_ui->_plot->QwtPlot::setAxisFont(QwtPlot::yLeft, font);
	_ui->_plot->QwtPlot::setAxisFont(QwtPlot::xBottom, font);
}

void QEPMChartLive::setTitleFont(const QFont &font)
{
	QwtText axisTitleText;

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::xBottom);
	axisTitleText.setFont(font);
	_ui->_plot->setAxisTitle(QwtPlot::xBottom, axisTitleText);

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::yLeft);
	axisTitleText.setFont(font);
	_ui->_plot->setAxisTitle(QwtPlot::yLeft, axisTitleText);
}

void QEPMChartLive::setTitleColor(const QColor &color)
{
	QwtText axisTitleText;

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::xBottom);
	axisTitleText.setColor(color);
	_ui->_plot->setAxisTitle(QwtPlot::xBottom, axisTitleText);

	axisTitleText = _ui->_plot->axisTitle(QwtPlot::yLeft);
	axisTitleText.setColor(color);
	_ui->_plot->setAxisTitle(QwtPlot::yLeft, axisTitleText);
}

void QEPMChartLive::calculateRange
(
	qreal& min,
	qreal& max
)
{
	bool sampleSeen(false);

	min = std::numeric_limits<qreal>::max();
	max = std::numeric_limits<qreal>::min();

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

		if (min > 0.)
			min = 0.;
	}
}

void QEPMChartLive::replot()
{
	recalulateAxis();

	if (_plotGrid == Q_NULLPTR)
	{
		_plotGrid= new QwtPlotGrid;
		_plotGrid->setMajorPen(Qt::white, .50);
		_plotGrid->enableXMin(true);
		_plotGrid->enableYMin(true);
		_plotGrid->setMinorPen(Qt::white, .25, Qt::DashLine);
		_plotGrid->attach(_ui->_plot);
	}

	_ui->_plot->replot();
}

void QEPMChartLive::logSample
(
	uint channel,
	double dbPhysical,
	quint32 uRawTimestamp
)
{
	LineModelEntryLive dataSeries = _lineSeriesModel.getDataSeriesByChannel(channel);
	if (dataSeries.isNull() == false)
	{
		QPointF dataPoint(static_cast<double>(uRawTimestamp) / MICRO_PER_BASE, dbPhysical);

		dataSeries->addDataPoint(dataPoint);
	}
}

bool QEPMChartLive::eventFilter
(
		QObject* obj,
		QEvent* event
		)
{
	bool result{false};

	if (obj == _ui->_plot->canvas())
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

			break;

		case QEvent::Leave:
			emit hoveredCoordChange("");
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

void QEPMChartLive::recalulateAxis()
{
	qreal min, max;

	_lineSeriesModel.getXSampleRange(min, max);
	setXRange(min, max);

	_lineSeriesModel.getYSampleRange(min, max);
	setYRange(min, max);

	_ui->_xAxis->update();
	_ui->_yAxis->update();
}

QPointF QEPMChartLive::invTransform(const QPoint &invMe)
{
	return QPointF(_ui->_plot->invTransform(QwtPlot::xBottom, invMe.x()), _ui->_plot->invTransform(QwtPlot::yLeft, invMe.y()));
}

QPoint QEPMChartLive::transform(const QPointF& transformMe)
{
	return QPoint(_ui->_plot->transform(QwtPlot::xBottom, transformMe.x()), _ui->_plot->transform(QwtPlot::yLeft, transformMe.y()));
}

