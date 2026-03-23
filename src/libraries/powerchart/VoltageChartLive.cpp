// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "VoltageChartLive.h"

#include "ui_QEPMChartLive.h"

VoltageChartLive::VoltageChartLive
(
	QWidget* parent
) :
	QEPMChartLive(parent)
{
	_ui->_yLabel->setRotation(90.0);
	setYTitle("Voltage (V)");
}

QString VoltageChartLive::generateHoveredText(const QPointF &logicalValues)
{
	return QString("Time: %1 s, Voltage: %2 mV").arg(logicalValues.x(), 3).arg(logicalValues.y(), 3);
}
