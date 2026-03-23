#ifndef TARGETINFO_H
#define TARGETINFO_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMLibDefines.h"
#include "EpmLibPrivate.h"

struct EpmVersionInfo
{
	MicroEpmBoardId EpmID;
	quint16 uMaxSamplesPerPacket;
	quint16 uMaxSamplesPerAveragePacket;
	quint16 uMaxPackets;
	quint8 uFirmwareVersion[4];
	quint8 uFirmwareProtocolVersion;
	quint8 uFirmwareLowestCompatibleProtocolVersion;
	quint8 uHostProtocolVersion;
	quint8 uHostLowestCompatibleProtocolVersion;
} ;

#endif // TARGETINFO_H
