#ifndef CURRENTCHART_H
#define CURRENTCHART_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMChart.h"

#include "PowerChart_global.h"

class POWERCHART_EXPORT CurrentChart :
	public QEPMChart
{
Q_OBJECT

public:
	CurrentChart(QWidget* parent = Q_NULLPTR);

protected:
	virtual QString generateHoveredText(const QPointF& logicalValues);
};

#endif // CURRENTCHART_H
