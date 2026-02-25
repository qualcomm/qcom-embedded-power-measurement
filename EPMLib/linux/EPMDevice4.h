#ifndef EPMDEVICE4_H
#define EPMDEVICE4_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "LinuxBaseDevice.h"

class _EPMDevice4 :
	public _LinuxBaseDevice
{
	Q_OBJECT
public:
	_EPMDevice4();

	virtual void close();

    static void updateDeviceList(struct usb_device* usbDevice);

protected:
	virtual void getBulkData(MicroEpmChannelData* channelData, quint32 channelDataSize, quint32& sampleCount, quint32 packetCount,
		quint32& overflowCount, quint32& emptyCount);
};

#endif // EPMDEVICE4_H
