#ifndef POWERCHART_H
#define POWERCHART_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMChart.h"

#include "PowerChart_global.h"

class POWERCHART_EXPORT PowerChart :
	public QEPMChart
{
Q_OBJECT

public:
	PowerChart(QWidget* parent = Q_NULLPTR);

protected:
	virtual QString generateHoveredText(const QPointF& logicalValues);
};

#endif // POWERCHART_H
