#ifndef LINUXBASEDEVICE_H
#define LINUXBASEDEVICE_H
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
// Copyright 2021-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

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

