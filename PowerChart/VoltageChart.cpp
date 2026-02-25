// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "VoltageChart.h"

#include "ui_AlpacaChart.h"

VoltageChart::VoltageChart
(
	QWidget* parent
) :
	AlpacaChart(parent)
{
	_ui->_yLabel->setRotation(90.0);
	setYTitle("Voltage (V)");
}

QString VoltageChart::generateHoveredText(const QPointF &logicalValues)
{
	return QString("Time: %1 s, Voltage: %2 mV").arg(logicalValues.x(), 3).arg(logicalValues.y(), 3);
}
