#ifndef VOLTAGECHART_H
#define VOLTAGECHART_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMChart.h"

#include "PowerChart_global.h"

class POWERCHART_EXPORT VoltageChart :
	public QEPMChart
{
Q_OBJECT

public:
	VoltageChart(QWidget* parent = Q_NULLPTR);

protected:
	virtual QString generateHoveredText(const QPointF& logicalValues);
};

#endif // VOLTAGECHART_H
