#ifndef ALPACACHART_H
#define ALPACACHART_H
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
// Copyright 2018-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "LineSeriesModel.h"

// Qt
#include <QMap>
#include <QWidget>

class AlpacaCursorLines;
class QwtPlot;
class QwtPlotGrid;
class HorizontalPlotZoomer;

#include "PowerChart_global.h"

class Ui_AlpacaChartClass;

class POWERCHART_EXPORT AlpacaChart :
	public QWidget
{
Q_OBJECT

public:
	AlpacaChart(QWidget* parent = Q_NULLPTR);
	virtual ~AlpacaChart();

	LineSeriesModel& lineSeriesModel()
	{
		return _lineSeriesModel;
	}

	void setSeriesColor(UDASBlock udasBlock);

	uint addLineSeries(UDASBlock udasBlock);

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

	void setZoomBase(const QRectF& zoomBase);
	void setXZoom(qreal left, qreal right);
	void zoomToCursor();
	void zoomIn();
	void zoomOut();
	void resetZoom();

	qreal cursorT0();
	void setCursorT0(qreal logicalValue);

	qreal cursorT1();
	void setCursorT1(qreal logicalValue);

	QPointF invTransform(const QPoint& invMe);
	QPoint transform(const QPointF& transformMe);

	void replot();

signals:
	void hoveredCoordChange(const QString& coordText);
	void zoomRectChanged(const QRectF& zoomRect);

	void t0Changed(qreal logicalValue);
	void t1Changed(qreal logicalValue);

	void t0Complete(qreal logicalValue);
	void t1Complete(qreal logicalValue);

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

	void emitBandingPosition();
	void emitBandingComplete();

	virtual QString generateHoveredText(const QPointF& logicalValues) = 0;

	Ui_AlpacaChartClass*		_ui{Q_NULLPTR};
	LineSeriesModel				_lineSeriesModel;
	QwtPlotGrid*				_plotGrid{Q_NULLPTR};
	AlpacaCursorLines*			_cursorLines{Q_NULLPTR};
	HorizontalPlotZoomer*		_okayZoomer{Q_NULLPTR};
	bool						_tracking{false};
	bool						_active{false};
};

#endif // ALPACACHART_H
