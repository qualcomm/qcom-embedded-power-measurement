#ifndef STATISTICS_H
#define STATISTICS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
