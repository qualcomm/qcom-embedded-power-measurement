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

// libEPM
#include "WinBaseDevice.h"
#include "MicroEpmErrors.h"
#include "WinUtils.h"

// QCommon
#include "Range.h"

// windows
#include <Setupapi.h>
#include <winerror.h>

_WinBaseDevice::~_WinBaseDevice()
{
}

void _WinBaseDevice::open()
{
	if (_connected == false)
	{
		DWORD lastError;

		/* We use the path to open a file to the device */
		_deviceHandle = CreateFileA(_devicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if (_deviceHandle == INVALID_HANDLE_VALUE)
		{
			DWORD lastError{ GetLastError()};

			EPMException epmException(MICRO_EPM_OS_ERROR, kUSBOpenError, "CreateFileA failed");

			_connectionStatus = QString("CreateFileA failed: %1").arg(lastError);

			throw epmException;
		}

		/* Now we intiailize the winUSB handle */
		BOOL result = WinUsb_Initialize(_deviceHandle, &_winUsbHandle);
		if (result == FALSE)
		{
			DWORD lastError{ GetLastError()};

			EPMException epmException(MICRO_EPM_OS_ERROR, kUSBOpenError, "WinUsb_Initialize failed");

			_connectionStatus = QString("WinUsb_Initialize failed: %1").arg(lastError);

			throw epmException;
		}

		USB_DEVICE_DESCRIPTOR deviceDescriptor;
		ULONG lengthTransfered;

		result = WinUsb_GetDescriptor(_winUsbHandle, USB_DEVICE_DESCRIPTOR_TYPE, 0, 0, reinterpret_cast<PUCHAR>(&deviceDescriptor), sizeof(USB_DEVICE_DESCRIPTOR), &lengthTransfered);
		if (result == TRUE)
		{
			ULONG bufferSize = sizeof(UCHAR) + sizeof(UCHAR) + sizeof(WCHAR) * MAXIMUM_USB_STRING_LENGTH;
			PUSB_STRING_DESCRIPTOR usbStringDescriptor = reinterpret_cast<PUSB_STRING_DESCRIPTOR>(new quint8[bufferSize]);

			if (usbStringDescriptor != Q_NULLPTR)
			{
				result = WinUsb_GetDescriptor(_winUsbHandle, USB_STRING_DESCRIPTOR_TYPE, deviceDescriptor.iSerialNumber, 0, reinterpret_cast<PUCHAR>(usbStringDescriptor), bufferSize, &lengthTransfered);
				if (result == TRUE)
				{
					UCHAR characterCount = (usbStringDescriptor->bLength - 2) / sizeof(WCHAR);

					for (auto charIndex: range(characterCount))
					{
						_serialNumber.push_back(usbStringDescriptor->bString[charIndex]);
					}
				}

				delete[] usbStringDescriptor;
			}
		}

		ULONG timeoutVal = 1000;

		if (WinUsb_SetPipePolicy(_winUsbHandle, _in, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal) == FALSE)
		{
			lastError = GetLastError();
			QString message = QString("WinUsb_SetPipePolicy(_winUsbHandle, _in, ...) == FALSE Error:%1\n").arg(lastError);

			if (_appCore != Q_NULLPTR)
				_appCore->writeToRunLog(message);
		}

		if (WinUsb_SetPipePolicy(_winUsbHandle, _in2, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal) == FALSE)
		{
			lastError = GetLastError();
			QString message = QString("WinUsb_SetPipePolicy(_winUsbHandle, _in2, ...) == FALSE Error:%1\n").arg(lastError);

			if (_appCore != Q_NULLPTR)
				_appCore->writeToRunLog(message);
		}

		if (WinUsb_SetPipePolicy(_winUsbHandle, _out, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal) == FALSE)
		{
			lastError = GetLastError();
			QString message = QString("WinUsb_SetPipePolicy(_winUsbHandle, _out, ...) == FALSE Error:%1\n").arg(lastError);

			if (_appCore != Q_NULLPTR)
				_appCore->writeToRunLog(message);
		}

		try
		{
			_EPMDevice::open();
		}
		catch (EPMException error)
		{
			close();
			throw error;
		}

		_connected = true;
	}
}

void _WinBaseDevice::closeInternal()
{
	if (_deviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_deviceHandle);
		_deviceHandle = INVALID_HANDLE_VALUE;
	}

	if (_winUsbHandle != INVALID_HANDLE_VALUE)
	{
		WinUsb_Free(_winUsbHandle);
		_winUsbHandle = INVALID_HANDLE_VALUE;
	}

	_connected = false;
}

void _WinBaseDevice::updateDeviceList
(
	QUuid guid,
	DeviceAllocator deviceAllocator
)
{
	HDEVINFO hDevInfo;
	DWORD lastError;
	BOOL bResult;

	GUID winGuid = guid;

	/* Search for info handle on specified GUID */
	hDevInfo = SetupDiGetClassDevs(&winGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
		throw EPMException(MICRO_EPM_ERROR, GetLastError(), EPMErrorToString(MICRO_EPM_COMM_ERROR).toLatin1());

	HDevMonitor hdevMonitor(hDevInfo);

	DWORD dwIndex{0};

	SP_DEVINFO_DATA devInfoData;
	memset(&devInfoData, 0, sizeof(SP_DEVINFO_DATA));
	devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	AppCore::writeToApplicationLog("Starting Device Iteration\n");

	while (SetupDiEnumDeviceInfo(hDevInfo, dwIndex, &devInfoData) == TRUE)
	{
		SP_DEVICE_INTERFACE_DATA devInterfaceData;

		memset(&devInterfaceData, 0, sizeof(SP_INTERFACE_DEVICE_DATA));
		devInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

		/* Get device interface information */
		bResult = SetupDiEnumDeviceInterfaces(hDevInfo, &devInfoData, &winGuid, 0, &devInterfaceData);
		if (bResult == FALSE)
		{
			lastError = GetLastError();
			if (lastError == ERROR_NO_MORE_ITEMS)
			{
				break;
			}

			throw EPMException(MICRO_EPM_ERROR, lastError, EPMErrorToString(MICRO_EPM_COMM_ERROR).toLatin1());
		}

		/* Two step process with this function. First we call it with
		* arguments NULL to determine how much space we need to allocate
		*/
		quint8 buffer[1024];
		PSP_DEVICE_INTERFACE_DETAIL_DATA_A pInterfaceDetailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA_A>(buffer);
		DWORD requiredLength(sizeof(buffer));

		pInterfaceDetailData->cbSize = sizeof(PSP_DEVICE_INTERFACE_DETAIL_DATA_A);

		bResult = SetupDiGetDeviceInterfaceDetailA(hDevInfo, &devInterfaceData, pInterfaceDetailData, requiredLength, &requiredLength, &devInfoData);
		if (bResult != TRUE)
		{
			AppCore::writeToApplicationLog("SetupDiGetDeviceInterfaceDetailA Failed\n");

			throw EPMException(MICRO_EPM_ERROR, GetLastError(), EPMErrorToString(MICRO_EPM_COMM_ERROR).toLatin1());
		}

		EPMDevice epmDevice = EPMDevice(deviceAllocator());
		epmDevice->_devicePath = QByteArray(pInterfaceDetailData->DevicePath);

		AppCore::writeToApplicationLog(QString("Device found %1\n").arg(epmDevice->_devicePath.data()));

		_EPMDevice::addDevice(epmDevice);

		dwIndex++;
	}
}

void _WinBaseDevice::send
(
	quint32 uLength
)
{
	bool operationOkay;
	ULONG cbSent = 0;

	for (qint32 i = 0; i < 2; i++)
	{
		operationOkay = WinUsb_WritePipe(_winUsbHandle, _out, _outputBuffer, uLength, &cbSent, NULL) == TRUE;

		if (!operationOkay)
			WinUsb_ResetPipe(_winUsbHandle, _out);
		else
			break;
	}

	if (!operationOkay)
		throw EPMException(MICRO_EPM_COMM_ERROR, GetLastError(), EPMErrorToString(MICRO_EPM_COMM_ERROR).toLatin1());
}

void _WinBaseDevice::receive
(
	quint32 uRequestedLength,
	quint8 commandCode
)
{
	bool operationOkay{false};
	ULONG readLength;
	qint32 i;

	clearInputBuffer();

	for (i = 0; i < 3; i++)
	{
		operationOkay = WinUsb_ReadPipe(_winUsbHandle, _in, _inputBuffer, uRequestedLength, &readLength, 0) == TRUE;
		if (!operationOkay)
		{
			DWORD lastError = ::GetLastError();
			switch (lastError)
			{
			case ERROR_BAD_COMMAND:
				if (i == 2)
					throw EPMException(MICRO_EPM_COMM_ERROR, lastError, "Device doesn't understand command.");

			default:
				WinUsb_ResetPipe(_winUsbHandle, _in);
			}

			continue;
		}

		if (readLength == 0 && _appCore != Q_NULLPTR)
			_appCore->writeToRunLog(QString("_EPMDevice3::Read Length == 0\n"));

		break;
	}

	if (!operationOkay)
		throw EPMException(MICRO_EPM_COMM_ERROR, 0, "!operationOkay");

	if (checkReturnCode(commandCode) == false)
		throw EPMException(MICRO_EPM_COMM_ERROR, commandCode, QString("Bad Command Code, Expected %1 got %2").arg(commandCode).arg(_inputBuffer[0]).toLatin1());
}

void _WinBaseDevice::readBulkData
(
	quint32 uRequestedLength
)
{
	BOOL bResult = TRUE;
	ULONG cbRead = 0;
	int32_t i;

	for (i = 0; i < 2; i++)
	{
		bResult = WinUsb_ReadPipe(_winUsbHandle, _in2, _bigBuffer, uRequestedLength, &cbRead, 0);
		if(!bResult)
		{
			WinUsb_ResetPipe(_winUsbHandle, _in2);
			continue;
		}

		if (cbRead != uRequestedLength)
		{
			if (_appCore != Q_NULLPTR)
				_appCore->writeToRunLog("_WinBaseDevice::cbRead != uRequestedLength\n");
		}

		break;
	}

	if (!bResult)
		throw EPMException(MICRO_EPM_COMM_ERROR, GetLastError(), "WinUsb_ReadPipe failed");
}


