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

#include "CurrentChart.h"

#include "ui_AlpacaChart.h"

CurrentChart::CurrentChart
(
	QWidget* parent
) :
	AlpacaChart(parent)
{
	_ui->_yLabel->setRotation(90.0);
	setYTitle("Current (mA)");
}

QString CurrentChart::generateHoveredText(const QPointF &logicalValues)
{
	return QString("Time: %1 s, Current: %2 mA").arg(static_cast<double>(logicalValues.x()), 0, 'f', 2).arg(static_cast<double>(logicalValues.y()), 0, 'f', 2);
}
