// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "CurrentChart.h"

#include "ui_QEPMChart.h"

CurrentChart::CurrentChart
(
	QWidget* parent
) :
	QEPMChart(parent)
{
	_ui->_yLabel->setRotation(90.0);
	setYTitle("Current (mA)");
}

QString CurrentChart::generateHoveredText(const QPointF &logicalValues)
{
	return QString("Time: %1 s, Current: %2 mA").arg(static_cast<double>(logicalValues.x()), 0, 'f', 2).arg(static_cast<double>(logicalValues.y()), 0, 'f', 2);
}
