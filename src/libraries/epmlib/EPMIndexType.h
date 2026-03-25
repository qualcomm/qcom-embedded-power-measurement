#ifndef EPMINDEXTYPE_H
#define EPMINDEXTYPE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPMLib
#include "EPMGlobalLib.h"

// Qt
#include <QString>

enum EPMIndexType
{
	eUnsetIndex,
	eMarkerIndex,
	eRCMIndex,
	eSPMIndex
};

const QString kUnset("UNSET");
const QString kMarker("MARKER");
const QString kRCM("RCM");
const QString kSPM("SPM");

QString EPMLIB_EXPORT EPMIndexTypeToString(EPMIndexType indexType);
EPMIndexType EPMLIB_EXPORT StringToEPMIndexType(const QString& epmString);

#endif // EPMINDEXTYPE_H
