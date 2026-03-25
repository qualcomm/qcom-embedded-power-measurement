#ifndef VOLTAGECHARTLIVE_H
#define VOLTAGECHARTLIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QEPMChartLive.h"

#include "PowerChart_global.h"

class POWERCHART_EXPORT VoltageChartLive :
	public QEPMChartLive
{
Q_OBJECT

public:
	VoltageChartLive(QWidget* parent = Q_NULLPTR);

protected:
	virtual QString generateHoveredText(const QPointF& logicalValues);
};

#endif // VOLTAGECHART_H
