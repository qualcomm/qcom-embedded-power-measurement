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
            Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "LinuxBaseDevice.h"

// libEPM
#include "EPMDevice3.h"
#include "EPMDevice4.h"
#include "LibUsbInitializer.h"
#include "MicroEpmErrors.h"


const QString kNoUSBError(QStringLiteral("No error"));

_LinuxBaseDevice::~_LinuxBaseDevice()
{
}

void _LinuxBaseDevice::open()
{
    if (_connected == false)
    {
        try
        {
            _deviceHandle = usb_open(_device);
            if (_deviceHandle != Q_NULLPTR)
            {
                _EPMDevice::open();
            }
            else
            {
				_connectionStatus = QString("USB open failed");
				throw EPMException(MICRO_EPM_OS_ERROR, kUSBOpenError, "USB open failed");
            }
        }
        catch (EPMException& error)
        {
            close();
			throw EPMException(MICRO_EPM_OS_ERROR, kUSBOpenError, QString("usb_open() crashed: %1").arg(error.what()).toLatin1());
        }
    }

    _connected = true;
}

void _LinuxBaseDevice::close()
{
    if (_deviceHandle != Q_NULLPTR)
    {
        usb_close(_deviceHandle);
        _deviceHandle = Q_NULLPTR;
    }
}

void _LinuxBaseDevice::closeInternal()
{
    _connected = false;
}

void _LinuxBaseDevice::updateDeviceList()
{
    LibUsbInitialize();

    usb_find_busses();
    usb_find_devices();

    usb_bus* bus{Q_NULLPTR};
    usb_bus* busses{Q_NULLPTR};

    busses = usb_get_busses();
    for (bus = busses; bus; bus = bus->next)
    {
        struct usb_device* dev;

        for (dev = bus->devices; dev; dev = dev->next)
        {
            if (dev->descriptor.idVendor == 0x05C6)
            {
                _EPMDevice3::updateDeviceList(dev);
                _EPMDevice4::updateDeviceList(dev);
            }
        }
    }
}

void _LinuxBaseDevice::send
(
    quint32 uLength
)
{
    const char* outBuffer = reinterpret_cast<const char*>(&_outputBuffer[0]);

    int bytesWritten = usb_bulk_write(_deviceHandle, _out, outBuffer, uLength, 30);
    if (bytesWritten >= 0)
    {

    }

    QString usbError = usb_strerror();

    if (usbError.compare(kNoUSBError, Qt::CaseInsensitive) != 0)
        throw EPMException(MICRO_EPM_COMM_ERROR, kUSBSendError, usbError.toLatin1());
}

void _LinuxBaseDevice::receive
(
    quint32 uRequestedLength,
    quint8 commandCode
)
{
    int bytesToRead{static_cast<int>(uRequestedLength)};
    int bytesRead{0};
    quint32 tries{0};
    bool good{false};

    clearInputBuffer();

    char* inBuffer = reinterpret_cast<char*>(&_inputBuffer[0]);

    while (bytesToRead > 0)
    {
        bytesRead = usb_bulk_read(_deviceHandle, _in, inBuffer, bytesToRead, 100);
        if (bytesRead > 0)
        {
            bytesToRead -= bytesRead;
            inBuffer += bytesRead;

            if (bytesToRead <= 0)
                good = true;
        }
        else
        {
            tries++;

            if (tries > 2)
                break;
        }
    }

    if (good == false)
        throw EPMException(MICRO_EPM_COMM_ERROR, commandCode, QString("Insufficient Bytes Received, Expected %1 got %2").
            arg(uRequestedLength).arg(uRequestedLength - bytesToRead).toLatin1());

    if (checkReturnCode(commandCode) == false)
        throw EPMException(MICRO_EPM_COMM_ERROR, commandCode, QString("Bad Command Code, Expected %1 got %2").arg(commandCode).arg(_inputBuffer[0]).toLatin1());

    QString usbError = usb_strerror();

    if (usbError.compare(kNoUSBError, Qt::CaseInsensitive) != 0)
        throw EPMException(MICRO_EPM_COMM_ERROR, kUSBReceiveError, usbError.toLatin1());
}

void _LinuxBaseDevice::readBulkData
(
    quint32 uRequestedLength
)
{
    int bytesRead;

    clearBigBuffer();

    char* inBuffer = reinterpret_cast<char*>(&_bigBuffer[0]);

    bytesRead = usb_bulk_read(_deviceHandle, _in2, inBuffer, uRequestedLength, 30);
    if (bytesRead >= 0)
    {

    }
}


