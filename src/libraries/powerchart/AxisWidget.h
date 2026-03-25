#ifndef AXISWIDGET_H
#define AXISWIDGET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QWidget>

class QwtPlot;

class AxisWidget :
	public QWidget
{
	Q_OBJECT
public:
	enum TickType
	{
		Minor = 0,
		Medium = 1,
		Major = 2
	};

	explicit AxisWidget(QWidget* parent = Q_NULLPTR);

	void setPlot(QwtPlot* plot);
	void setOrientation(Qt::Orientation orientation);

protected:
	virtual void paintEvent(QPaintEvent* paintEvt);

private:
	void drawHorizontalAxisTicks(const QList<double>& logicalTicks, const QList<int>& pixelTicks, QPainter& painter, TickType tickType);
	void drawVerticalAxisTicks(const QList<double>& logicalTicks, const QList<int>& pixelTicks, QPainter& painter, TickType tickType);

	QwtPlot*					_plot{Q_NULLPTR};
	Qt::Orientation				_orientation{Qt::Horizontal};
};

#endif // AXISWIDGET_H
