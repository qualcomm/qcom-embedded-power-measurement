#ifndef TARGETINFO_H
#define TARGETINFO_H
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
// Copyright 2013-2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "BoardUuid.h"
#include "EPMLibDefines.h"
#include "PlatformID.h"

enum TargetIdentityInfo
{
	_MICRO_EPM_MSM_MAX        = 0x7FFFFFFF
};

class TargetInfo
{
public:

	TargetInfo()
	{
		memset(_serialNumber, 0, MICRO_EPM_STRING_MAX_SIZE);
		memset(_modelName, 0, MICRO_EPM_STRING_MAX_SIZE);
	}

	TargetInfo(const TargetInfo& copyMe)
	{
		memcpy(_serialNumber, copyMe._serialNumber, MICRO_EPM_STRING_MAX_SIZE);
		memcpy(_modelName, copyMe._modelName, MICRO_EPM_STRING_MAX_SIZE);
		_targetIdentifier = copyMe._targetIdentifier;
		_platformIdentifier = copyMe._platformIdentifier;
		_boardOptionIdentifier = copyMe._boardOptionIdentifier;
		_eTargetIdentifier = copyMe._eTargetIdentifier;
		_targetRevisionIdentifier = copyMe._targetRevisionIdentifier;
		_configurationIdentifier = copyMe._configurationIdentifier;
		_eepromProgrammedTime = copyMe._eepromProgrammedTime;
		_eepromWriteCount = copyMe._eepromWriteCount;
		_epmBoardUuid = copyMe._epmBoardUuid;
	}

	TargetInfo& operator=(const TargetInfo& assignMe)
	{
		memcpy(_serialNumber, assignMe._serialNumber, MICRO_EPM_STRING_MAX_SIZE);
		memcpy(_modelName, assignMe._modelName, MICRO_EPM_STRING_MAX_SIZE);
		_targetIdentifier = assignMe._targetIdentifier;
		_platformIdentifier = assignMe._platformIdentifier;
		_boardOptionIdentifier = assignMe._boardOptionIdentifier;
		_eTargetIdentifier = assignMe._eTargetIdentifier;
		_targetRevisionIdentifier = assignMe._targetRevisionIdentifier;
		_configurationIdentifier = assignMe._configurationIdentifier;
		_eepromProgrammedTime = assignMe._eepromProgrammedTime;
		_eepromWriteCount = assignMe._eepromWriteCount;
		_epmBoardUuid = assignMe._epmBoardUuid;

		return *this;
	}

	QByteArray serialNumber()
	{
		return QByteArray(_serialNumber);
	}
	void setSerialNumber(const QByteArray& serialNumber)
	{
		int bytesToCopy = MICRO_EPM_STRING_MAX_SIZE;
		if (serialNumber.size() < static_cast<int>(MICRO_EPM_STRING_MAX_SIZE))
			bytesToCopy = serialNumber.size();

		memcpy(_serialNumber, serialNumber.data(), bytesToCopy);
	}

	QByteArray modelName()
	{
		return QByteArray(_modelName);
	}
	void setModelName(const QByteArray& modelName)
	{
		int bytesToCopy = MICRO_EPM_STRING_MAX_SIZE;
		if (modelName.size() < static_cast<int>(MICRO_EPM_STRING_MAX_SIZE))
			bytesToCopy = modelName.size();

		memcpy(_modelName, modelName.data(), bytesToCopy);
	}

	char						_serialNumber[MICRO_EPM_STRING_MAX_SIZE];                 // Serial number of this board
	char						_modelName[MICRO_EPM_STRING_MAX_SIZE];                  // Model number of this board
	quint16						_targetIdentifier{0};                                     // Target identifier
	PlatformID					_platformIdentifier{MICRO_EPM_BOARD_ID_UNKNOWN};		  // EPM platform type
	MicroEpmBoardInfo			_boardOptionIdentifier{_MICRO_EPM_BOARD_MAX};
	TargetIdentityInfo			_eTargetIdentifier{_MICRO_EPM_MSM_MAX};
	quint16						_targetRevisionIdentifier{0};                              // EPM board revision ID
	quint16						_configurationIdentifier{0};
	uint64_t					_eepromProgrammedTime{0};
	quint32						_eepromWriteCount{0};
	BoardUuid					_epmBoardUuid;
};

#endif // TARGETINFO_H
