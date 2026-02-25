// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AxisWidget.h"

// QCommon
#include "PainterEffects.h"
#include "Range.h"

// qwt
#include <qwt_plot.h>
#include <qwt_scale_div.h>

// Qt
#include <QPainter>

//standard libraries
#include <array>

constexpr std::array kTickLengths = { 3, 4, 5 };

AxisWidget::AxisWidget
(
	QWidget* parent
) :
	QWidget(parent)
{

}

void AxisWidget::setPlot
(
	QwtPlot* plot
)
{
	_plot = plot;
}

void AxisWidget::setOrientation
(
	Qt::Orientation orientation
)
{
	_orientation = orientation;
}

void AxisWidget::paintEvent
(
	QPaintEvent *paintEvt
)
{
	Q_UNUSED(paintEvt);

	int axisID = _orientation == Qt::Horizontal ?  QwtPlot::xBottom : QwtPlot::yLeft;

	QPainter painter(this);

	QFont painterFont = painter.font();
	painterFont.setPointSize(6);
	painter.setFont(painterFont);

	auto drawTicksFunction = [this, &painter, axisID](QwtScaleDiv::TickType tickType) -> void
	{
		QList<double> logicalTicks = _plot->axisScaleDiv(axisID).ticks(tickType);
		if (logicalTicks.size() > 0)
		{
			QList<int> pixelTicks;

			pixelTicks.reserve(logicalTicks.size());
			std::transform(logicalTicks.begin(), logicalTicks.end(), std::back_inserter(pixelTicks), [this, axisID](const double tickValue){
				return static_cast<int>(_plot->transform(axisID, tickValue)) + 1;
			});

			if (_orientation == Qt::Horizontal)
			{
				drawHorizontalAxisTicks(logicalTicks, pixelTicks, painter, static_cast<TickType>(tickType));
			}
			else
			{
				drawVerticalAxisTicks(logicalTicks, pixelTicks, painter, static_cast<TickType>(tickType));
			}
		}
	};

	drawTicksFunction(QwtScaleDiv::MinorTick);
	drawTicksFunction(QwtScaleDiv::MediumTick);
	drawTicksFunction(QwtScaleDiv::MajorTick);
}

void AxisWidget::drawHorizontalAxisTicks
(
	const QList<double> &logicalTicks,
	const QList<int> &pixelTicks,
	QPainter& painter,
	TickType tickType
)
{
	const int fontHalfHeight = QFontMetrics(painter.font()).height() / 2;
	for (const int& tickIndex : range(pixelTicks.count()))
	{
		const auto& pixelTick = pixelTicks[tickIndex];

		auto top = QPoint{ pixelTick, 0 };
		auto bottom = QPoint{ pixelTick, kTickLengths[tickType] };
		painter.drawLine(top, bottom);

		if (tickType == TickType::Major)
		{
			const QString logicalTickText = QString::number(logicalTicks[tickIndex], 'f', 2);

			if (tickIndex == 0)
			{
				paintLeftRotatedText(painter, QPoint{pixelTick, bottom.y() + fontHalfHeight + 4}, logicalTickText, 0.);
			}
			else if (tickIndex == pixelTicks.count() - 1)
			{
				paintRightRotatedText(painter, QPoint{pixelTick, bottom.y() + fontHalfHeight + 4}, logicalTickText, 0.);
			}
			else
			{
				paintCenterRotatedText(painter, QPoint{pixelTick, bottom.y() + fontHalfHeight + 4}, logicalTickText, 0.);
			}
		}
	}
}

void AxisWidget::drawVerticalAxisTicks
(
	const QList<double> &logicalTicks,
	const QList<int> &pixelTicks,
	QPainter& painter,
	TickType tickType
)
{
	for (const int& tickIndex : range(pixelTicks.count()))
	{
		const auto& pixelTick = pixelTicks[tickIndex];

		auto left = QPoint{ width() - kTickLengths[tickType], pixelTick };
		auto right = QPoint{ width(), pixelTick };
		painter.drawLine(left, right);

		if (tickType == TickType::Major)
		{
			const QString logicalTickText = QString::number(logicalTicks[tickIndex], 'f', 2);
			paintRightRotatedText(painter, QPoint{ left.x() - 4, pixelTick }, logicalTickText, 0.);
		}
	}
}
