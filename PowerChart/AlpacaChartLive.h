#ifndef ALPACACHARTLIVE_H
#define ALPACACHARTLIVE_H
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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
