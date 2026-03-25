#ifndef MICRO_EPM_ERRORS_H
#define MICRO_EPM_ERRORS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Qt
#include <QString>

//C++
#include <stdexcept>

// USB error codes
const uint kUSBSendError{0x2203};
const uint kUSBReceiveError{0x4869};
const uint kUSBOpenError{0x1503};

enum EpmErrorCode
{
    MICRO_EPM_SUCCESS = 0,
    MICRO_EPM_ERROR,
	MICRO_EPM_OS_ERROR,
    MICRO_EPM_VERSION_MISMATCH_OLD_HOST,
    MICRO_EPM_VERSION_MISMATCH_OLD_FIRMWARE,
    MICRO_EPM_COMM_ERROR,
    MICRO_EPM_OUT_OF_MEMORY,
    MICRO_EPM_NOT_CONNECTED,
    MICRO_EPM_INVALID_PARAMETER,
    MICRO_EPM_OVERFLOW,
    MICRO_EPM_NOT_PROGRAMMED,
    MICRO_EPM_INVALID_CHECKSUM,
    MICRO_EPM_TOO_MANY_DEVICES,
    MICRO_EPM_NOT_IMPLEMENTED,
    MICRO_EPM_POWER_ON_TEST_FAILED,
    MICRO_EPM_INVALID_PARSED_DATA,
    MICRO_EPM_NOT_ERASED,
    MICRO_EPM_FLASH_IMAGE_FAILED,
    MICRO_EPM_INVALID_TRIGGERED_MODE_SETTINGS,
    MICRO_EPM_COMM_ERROR_NOT_CONNECTED,
    MICRO_EPM_TAC_CMD_NOT_SUPPORTED,
    MICRO_EPM_FILE_ERROR,
    MICRO_EPM_INVALID_FILE_PATH,
    MICRO_EPM_ERROR_NO_CHAN_EN,
    MICRO_EPM_ERROR_ALREADY_ACQUIRING,
	MICRO_EPM_ERROR_INVALID_RESPONSE_CMD,
    MICRO_EPM_ERROR_MAX           = 0x7FFFFFFF
} ;

inline QString EPMErrorToString
(
	EpmErrorCode status
)
{
	QString result = QString("Error: %1 ").arg(status);

	switch (status)
	{
	case MICRO_EPM_SUCCESS: result += "Success"; break;
	case MICRO_EPM_ERROR: result += "Error"; break;
	case MICRO_EPM_OS_ERROR: result += "OS Error"; break;
	case MICRO_EPM_VERSION_MISMATCH_OLD_HOST: result += "Version Mismatch Old Host"; break;
	case MICRO_EPM_VERSION_MISMATCH_OLD_FIRMWARE: result += "Version Mismatch Old Firmware"; break;
	case MICRO_EPM_COMM_ERROR: result += "COMM Error"; break;
	case MICRO_EPM_OUT_OF_MEMORY: result += "Out of Memory"; break;
	case MICRO_EPM_NOT_CONNECTED: result += "Not Connected"; break;
	case MICRO_EPM_INVALID_PARAMETER: result += "Invalid Parameter"; break;
	case MICRO_EPM_OVERFLOW: result += "Overflow"; break;
	case MICRO_EPM_NOT_PROGRAMMED: result += "Not Programmed"; break;
	case MICRO_EPM_INVALID_CHECKSUM: result += "Invalid Checksum"; break;
	case MICRO_EPM_TOO_MANY_DEVICES: result += "Too Many Devices"; break;
	case MICRO_EPM_NOT_IMPLEMENTED: result += "Not Implemented"; break;
	case MICRO_EPM_POWER_ON_TEST_FAILED: result += "Power On Test Failed"; break;
	case MICRO_EPM_INVALID_PARSED_DATA: result += "Invalid Parsed Data"; break;
	case MICRO_EPM_NOT_ERASED: result += "Not Erased"; break;
	case MICRO_EPM_FLASH_IMAGE_FAILED: result += "Flash Image Failed"; break;
	case MICRO_EPM_INVALID_TRIGGERED_MODE_SETTINGS: result += "Invalid Triggered Mode Settings"; break;
	case MICRO_EPM_COMM_ERROR_NOT_CONNECTED: result += "COMM Error Not Connected"; break;
	case MICRO_EPM_TAC_CMD_NOT_SUPPORTED: result += "TAC Command Not Supported"; break;
	case MICRO_EPM_FILE_ERROR: result += "File Error"; break;
	case MICRO_EPM_INVALID_FILE_PATH: result += "Invalid File Path"; break;
	case MICRO_EPM_ERROR_NO_CHAN_EN: result += "Error No Channels Enabled"; break;
	case MICRO_EPM_ERROR_ALREADY_ACQUIRING: result += "Error Already Acquiring"; break;
	case MICRO_EPM_ERROR_INVALID_RESPONSE_CMD: result += "Response Command didn't match Request Command"; break;
	default:
		break;
	}

	return result;
}

inline QByteArray EPMErrorToByteArray(EpmErrorCode status)
{
	return EPMErrorToString(status).toLatin1();
}

class EPMException:
	public std::runtime_error
{
public:
	EPMException(EpmErrorCode epmCode, uint nativeCode, const QByteArray& what) :
		std::runtime_error(what.data()),
		_epmError(epmCode),
		_nativeErrorcode(nativeCode)
	{

	}

	EPMException(const EPMException& copyMe) :
		std::runtime_error(copyMe.what())
	{
		_epmError = copyMe._epmError;
		_nativeErrorcode = copyMe._nativeErrorcode;
	}

	EpmErrorCode epmError() const
	{
		return _epmError;
	}

	uint nativeError() const
	{
		return _nativeErrorcode;
	}

private:
	EpmErrorCode				_epmError{MICRO_EPM_SUCCESS};
	uint						_nativeErrorcode{0};
};

#endif

