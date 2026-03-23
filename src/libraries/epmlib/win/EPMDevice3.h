#ifndef EPMDEVICE3_H
#define EPMDEVICE3_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "WinBaseDevice.h"

class _EPMDevice3 :
	public _WinBaseDevice
{
	Q_OBJECT
public:
	_EPMDevice3();

	virtual void close();

	static void updateDeviceList();

protected:
	virtual void getBulkData(MicroEpmChannelData* channelData, quint32 channelDataSize, quint32& sampleCount, quint32 packetCount,
		quint32& overflowCount, quint32& emptyCount);
};

#endif // EPMDEVICE3_H
