// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// libEPM
#include "MicroEpmErrors.h"


QString EPMException::toString()
{
	QString result;

	if (_epmError != MICRO_EPM_SUCCESS)
	{
		result = EPMErrorToString(_epmError);

		if (_nativeErrorcode != 0)
		{
			result += QString("OS Code %1").arg(_nativeErrorcode);
		}
	}

	return result;
}



