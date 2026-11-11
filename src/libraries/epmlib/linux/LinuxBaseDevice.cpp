// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "LinuxBaseDevice.h"

// libEPM
#include "EPMDevice3.h"
#include "EPMDevice4.h"
#include "LibUsbInitializer.h"
#include "MicroEpmErrors.h"

// Qt
#include <QDir>
#include <QFile>

const QString kNoUSBError(QStringLiteral("No error"));

static QString readSysfsValue(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
		return QString();

	return QString::fromLatin1(file.readAll()).trimmed();
}

QString _LinuxBaseDevice::readUsbSerialNumber
(
	struct usb_device* usbDevice
)
{
	if (usbDevice == Q_NULLPTR)
		return QString();

	// libusb-0.1 names the bus and device by number ("001", "058"); sysfs has the same numbers
	// in busnum/devnum, plus the serial string, so no device open (or extra permission) is needed.
	bool busOk{false};
	bool devOk{false};
	const int busNumber = QString::fromLatin1(usbDevice->bus != Q_NULLPTR ? usbDevice->bus->dirname : "").toInt(&busOk);
	const int deviceNumber = QString::fromLatin1(usbDevice->filename).toInt(&devOk);

	if (busOk && devOk)
	{
		const QString sysfsRoot(QStringLiteral("/sys/bus/usb/devices"));
		const QStringList entries = QDir(sysfsRoot).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

		for (const auto& entry : entries)
		{
			const QString base = sysfsRoot + "/" + entry + "/";
			if (QFile::exists(base + "serial") == false)
				continue;

			if (readSysfsValue(base + "busnum").toInt() == busNumber &&
				readSysfsValue(base + "devnum").toInt() == deviceNumber)
			{
				const QString serial = readSysfsValue(base + "serial");
				if (serial.isEmpty() == false)
					return serial;
			}
		}
	}

	// Fallback: read the iSerialNumber string descriptor directly.
	if (usbDevice->descriptor.iSerialNumber != 0)
	{
		usb_dev_handle* handle = usb_open(usbDevice);
		if (handle != Q_NULLPTR)
		{
			char buffer[256] = {0};
			const int length = usb_get_string_simple(handle, usbDevice->descriptor.iSerialNumber, buffer, sizeof(buffer) - 1);
			usb_close(handle);

			if (length > 0)
				return QString::fromLatin1(buffer, length).trimmed();
		}
	}

	return QString();
}

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
