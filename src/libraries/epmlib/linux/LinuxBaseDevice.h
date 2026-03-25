#ifndef LINUXBASEDEVICE_H
#define LINUXBASEDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "../EPMDevice.h"

// Linux
#include <usb.h>

class _LinuxBaseDevice :
	public _EPMDevice
{
	Q_OBJECT
public:
    _LinuxBaseDevice() {};
    ~_LinuxBaseDevice();

	virtual QString serialNumber()
	{
		return _serialNumber;
	}

	virtual void open();
    virtual void close();

	static void updateDeviceList();

protected:
	void closeInternal();

	virtual void send(quint32 uLength);
	virtual void receive(quint32 uRequestedLength, quint8 commandCode);
	virtual void readBulkData(quint32 uRequestedLength);

	QString						_serialNumber;
	quint16						_vendorID = 0;
	quint16						_productID = 0;
    struct usb_device*          _device{Q_NULLPTR};
    usb_dev_handle*             _deviceHandle{Q_NULLPTR};
	quint8						_in = 0;
	quint8						_in2 = 0;
	quint8						_out = 0;
};

#endif // LINUXBASEDEVICE_H

