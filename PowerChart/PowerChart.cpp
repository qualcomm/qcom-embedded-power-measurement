// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PowerChart.h"


#include "ui_AlpacaChart.h"

PowerChart::PowerChart
(
	QWidget* parent
) :
	AlpacaChart(parent)
{
	_ui->_yLabel->setRotation(90.0);
	setYTitle("Power (mW)");
}

QString PowerChart::generateHoveredText(const QPointF &logicalValues)
{
	return QString("Time: %1 s, Power: %2 mW").arg(logicalValues.x(), 3).arg(logicalValues.y(), 3);
}
