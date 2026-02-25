#ifndef CURRENTCHARTLIVE_H
#define CURRENTCHARTLIVE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AlpacaChartLive.h"

#include "PowerChart_global.h"

class POWERCHART_EXPORT CurrentChartLive :
	public AlpacaChartLive
{
Q_OBJECT

public:
	CurrentChartLive(QWidget* parent = Q_NULLPTR);

protected:
	virtual QString generateHoveredText(const QPointF& logicalValues);
};

#endif // CURRENTCHARTLIVE_H
