// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause


#include "Statistics.h"

// C++

// C++
#include <algorithm>
#include <cmath>
#include <numeric>


void Statistics::clear()
{
	reset();
}

void Statistics::setSpan
(
	std::span<qreal> dataPoints
)
{
	_dataPoints = dataPoints;
}

qreal Statistics::min()
{
	if (qIsNaN(_min))
	{
		_min  = std::numeric_limits<qreal>::infinity();
		if (!_dataPoints.empty())
			_min = *std::min_element(_dataPoints.begin(), _dataPoints.end());
	}
	return _min;
}

qreal Statistics::max()
{
	if (qIsNaN(_max))
	{
		_max = -std::numeric_limits<qreal>::infinity();
		if (!_dataPoints.empty())
			_max = *std::max_element(_dataPoints.begin(), _dataPoints.end());
	}

	return _max;
}

qreal Statistics::avg()
{
	if (qIsNaN(_avg))
	{
		_avg = 0.0;

		for (auto start : _dataPoints)
		{
			_avg += start;
		}

		_avg = _avg / count();
	}

	return _avg;
}

qreal Statistics::sumOfSquares()
{
	if (qIsNaN(_sumOfSquares))
	{
		_sumOfSquares = 0.0;

		for (auto start : _dataPoints)
		{
			_sumOfSquares += pow(start - avg(), 2.0);
		}

	}

	return _sumOfSquares;
}

qreal Statistics::sampleStdDeviation()
{
	if (qIsNaN(_sampleStdDeviation))
	{
		if (count() >= 2.0)
			_sampleStdDeviation = sqrt(sumOfSquares() / count() - 1);
	}

	return _sampleStdDeviation;
}

qreal Statistics::populationStdDeviation()
{
	if (qIsNaN(_populationStdDeviation))
	{
		if (count() >= 1.0)
			_populationStdDeviation = sqrt(sumOfSquares() / count()) / 1000.0;

	}

	return _populationStdDeviation;
}

void Statistics::reset()
{
	_min = qQNaN();
	_max = qQNaN();
	_avg = qQNaN();
	_sumOfSquares = qQNaN();
	_sampleStdDeviation = qQNaN();
	_populationStdDeviation = qQNaN();
}

qreal Statistics::count()
{
	return static_cast<qreal>(_dataPoints.size());
}
