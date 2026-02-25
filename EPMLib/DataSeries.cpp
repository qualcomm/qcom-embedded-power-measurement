// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "DataSeries.h"


QPointF _DataSeries::value(uint i) const
{
	QPointF result;

	result = QPointF(_timeBetweenSamples * static_cast<qreal>(i), _ySeries.at(static_cast<int>(i)));

	return result;
}
