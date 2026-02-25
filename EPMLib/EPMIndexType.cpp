// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMIndexType.h"

QString EPMIndexTypeToString(EPMIndexType indexType)
{
	QString result;

	switch (indexType)
	{
    case eUnsetIndex: result = kUnset; break;
	case eMarkerIndex: result = kMarker; break;
	case eRCMIndex: result = kRCM; break;
	case eSPMIndex: result = kSPM; break;
	}

	return result;
}


EPMIndexType StringToEPMIndexType
(
	const QString& epmString
)
{
	EPMIndexType result(eUnsetIndex);
    if (epmString == kUnset)
        result = eUnsetIndex;
    else if (epmString == kMarker)
		result = eMarkerIndex;
	else if (epmString == kRCM)
		result = eRCMIndex;
	else if (epmString == kSPM)
		result = eSPMIndex;

	return result;
}
