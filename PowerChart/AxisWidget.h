#ifndef AXISWIDGET_H
#define AXISWIDGET_H
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
