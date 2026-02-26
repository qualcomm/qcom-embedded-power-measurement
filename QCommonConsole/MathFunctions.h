#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

bool inRange(int testMe, int lowBounds, int highbounds, bool inclusive = true)
{
	bool result(false);

	if (inclusive)
		result = testMe >= lowBounds && testMe <= highbounds;
	else
		result = testMe > lowBounds && testMe < highbounds;

	return result;
}

#endif // MATHFUNCTIONS_H
