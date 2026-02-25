#ifndef WINBASEDEVICE_H
#define WINBASEDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "../EPMDevice.h"

#include <QUuid>

// Windows
#include <windows.h>
#include <winusb.h>

typedef _EPMDevice* (*DeviceAllocator)();

class _WinBaseDevice :
	public _EPMDevice
{
	Q_OBJECT
public:
	_WinBaseDevice() {};
	~_WinBaseDevice();

	virtual QString serialNumber()
	{
		return _serialNumber;
	}

	virtual void open();
	virtual void close() = 0;

	static void updateDeviceList(QUuid guid, DeviceAllocator deviceAllocator);

protected:
	void closeInternal();

	virtual void send(quint32 uLength);
	virtual void receive(quint32 uRequestedLength, quint8 commandCode);
	virtual void readBulkData(quint32 uRequestedLength);

	HANDLE						_deviceHandle = INVALID_HANDLE_VALUE;
	WINUSB_INTERFACE_HANDLE		_winUsbHandle = INVALID_HANDLE_VALUE;
	QString						_serialNumber;
	quint16						_vendorID = 0;
	quint16						_productID = 0;
	quint8						_in = 0;
	quint8						_in2 = 0;
	quint8						_out = 0;
};

#endif // WINBASEDEVICE_H

