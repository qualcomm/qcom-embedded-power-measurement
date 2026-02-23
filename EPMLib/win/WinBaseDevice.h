#ifndef WINBASEDEVICE_H
#define WINBASEDEVICE_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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

