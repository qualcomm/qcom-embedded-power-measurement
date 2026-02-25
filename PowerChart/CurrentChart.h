#ifndef CURRENTCHART_H
#define CURRENTCHART_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AlpacaChart.h"

#include "PowerChart_global.h"

class POWERCHART_EXPORT CurrentChart :
	public AlpacaChart
{
Q_OBJECT

public:
	CurrentChart(QWidget* parent = Q_NULLPTR);

protected:
	virtual QString generateHoveredText(const QPointF& logicalValues);
};

#endif // CURRENTCHART_H
