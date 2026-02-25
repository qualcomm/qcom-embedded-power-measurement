// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: msimpson
*/

#include "ParserInterface.h"

#include "Hexify.h"

QByteArray ParserInterface::ParseToHex
(
	const SPDiagMessage& dataPacket
)
{
	return dataPacket->GetBuffer().toHex();
}
