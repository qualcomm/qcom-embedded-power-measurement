#ifndef STATISTICS_H
#define STATISTICS_H
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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QCommon

// Qt
#include<QtNumeric>
#include <QVector>

// C++
#include <span>

class QCOMMONCONSOLE_EXPORT Statistics
{
public:
	Statistics() = default;

	void clear();

	void setSpan(std::span<qreal> dataPoints);

	qreal min();
	qreal max();
	qreal avg();
	qreal sampleStdDeviation();
	qreal populationStdDeviation();

private:
	void reset();

	qreal count();
	qreal sumOfSquares();

	std::span<qreal>			_dataPoints;

	qreal						_min{qQNaN()};
	qreal						_max{qQNaN()};
	qreal						_avg{qQNaN()};
	qreal						_sumOfSquares{qQNaN()};
	qreal						_sampleStdDeviation{qQNaN()};
	qreal						_populationStdDeviation{qQNaN()};
};

#endif // STATISTICS_H
