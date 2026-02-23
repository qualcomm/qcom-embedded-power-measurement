// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
