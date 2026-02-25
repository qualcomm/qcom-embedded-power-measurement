#ifndef EPMRUNTIMECHANNEL_H
#define EPMRUNTIMECHANNEL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// libEPM
#include "EPMIndexType.h"
#include "EPMGlobalLib.h"

// QCommon
#include "StringUtilities.h"

// Qt
#include <QMap>

struct EPMLIB_EXPORT EPMRuntimeChannel
{
	EPMRuntimeChannel() = default;
	EPMRuntimeChannel(HashType seriesHash, bool active);
    EPMRuntimeChannel(const EPMRuntimeChannel& copyMe);
	EPMRuntimeChannel& operator = (const EPMRuntimeChannel& assignMe);

	HashType					_seriesHash{0};
	bool						_active{false};
	QString						_seriesName;
};

typedef QMap<HashType, EPMRuntimeChannel>		EPMRuntimeChannels;

#endif // EPMRUNTIMECHANNEL_H
