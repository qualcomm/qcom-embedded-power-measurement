#ifndef ALPACACHARTLIVE_H
#define ALPACACHARTLIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// Powerchart
#include "LineSeriesModelLive.h"

// libEPM
#include "EPMChannel.h"

// Qt
#include <QMap>
#include <QWidget>

class QwtPlot;
class QwtPlotCurve;
class QwtPlotGrid;

#include "PowerChart_global.h"

class Ui_AlpacaChartLiveClass;

class POWERCHART_EXPORT AlpacaChartLive :
	public QWidget
{
Q_OBJECT

public:
	AlpacaChartLive(QWidget* parent = Q_NULLPTR);
	virtual ~AlpacaChartLive();

	void clearLineSeries();

	void setDuration(qreal seconds);
	void addLineSeries(EPMChannel epmChannel);

	void setGraphBackground(const QColor& backgroundColor);

	void setXTitle(const QString& title);
	void setYTitle(const QString& title);

	void setXAxis(int major, int minor);
	void setXRange(qreal min, qreal max);

	void setYAxis(int major, int minor);
	void setYRange(qreal min, qreal max);

	void setAxisFont(const QFont& font);

	void setTitleFont(const QFont& font);

	void setTitleColor(const QColor& color);

	void setSeriesState(uint seriesHash, bool state);

	void calculateRange(qreal& min, qreal& max);

	void setCursorLines(qreal min, qreal max);

	QPointF invTransform(const QPoint& invMe);
	QPoint transform(const QPointF& transformMe);

	void replot();

	void logSample(uint seriesHash, double dbPhysical, quint32 uRawTimestamp);

signals:
	void hoveredCoordChange(const QString& coordText);

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

	virtual QString generateHoveredText(const QPointF& logicalValues) = 0;

	void recalulateAxis();

	Ui_AlpacaChartLiveClass*	_ui{Q_NULLPTR};
	LineSeriesModelLive			_lineSeriesModel;
	qreal						_duration{0.};
	QwtPlotGrid*				_plotGrid{Q_NULLPTR};
};

#endif // ALPACACHART_H
