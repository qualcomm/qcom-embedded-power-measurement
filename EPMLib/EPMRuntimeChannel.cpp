// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMRuntimeChannel.h"

EPMRuntimeChannel::EPMRuntimeChannel
(
	HashType seriesHash,
	bool active
)
{
	_seriesHash = seriesHash;
	_active = active;
}

EPMRuntimeChannel::EPMRuntimeChannel(const EPMRuntimeChannel &copyMe)
{
	_seriesHash = copyMe._seriesHash;
	_active = copyMe._active;
	_seriesName = copyMe._seriesName;
}

EPMRuntimeChannel &EPMRuntimeChannel::operator =
(
	const EPMRuntimeChannel& assignMe
)
{
	_seriesHash = assignMe._seriesHash;
	_active = assignMe._active;
	_seriesName = assignMe._seriesName;

	return *this;
}
