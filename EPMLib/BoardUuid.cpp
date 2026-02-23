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

#include "BoardUuid.h"

#include "Range.h"

BoardUuid::BoardUuid()
{
	QUuid randomGuid = QUuid::createUuid();
	*this = randomGuid;
}

BoardUuid::BoardUuid
(
	const BoardUuid& copyMe
)
{
	data1 = copyMe.data1;
	data2 = copyMe.data2;
	data3 = copyMe.data3;

	for (const auto& index : range(8))
		data4[index] = copyMe.data4[index];
}

BoardUuid::BoardUuid
(
	QUuid& quuid
)
{
	data1 = quuid.data1;
	data2 = quuid.data2;
	data3 = quuid.data3;

	for (const auto& index : range(8))
		data4[index] = quuid.data4[index];
}

BoardUuid::BoardUuid
(
	MicroEpmEepromUuid& boardUuid
)
{
	data1 = boardUuid.data1;
	data2 = boardUuid.data2;
	data3 = boardUuid.data3;

	for (const auto& index : range(8))
		data4[index] = boardUuid.data4[index];
}

MicroEpmEepromUuid BoardUuid::toMicroEPMUuid()
{
	MicroEpmEepromUuid result;

	result.data1 = data1;
	result.data2 = data2;
	result.data3 = data3;

	for (const auto& index : range(8))
		 result.data4[index] = data4[index];

	return result;
}

QUuid BoardUuid::toQUuid()
{
	QUuid result;

	result.data1 = data1;
	result.data2 = data2;
	result.data3 = data3;

	for (const auto& index : range(8))
		 result.data4[index] = data4[index];

	return result;
}

MicroEpmEepromUuid::MicroEpmEepromUuid
(
	const MicroEpmEepromUuid& copyMe
)
{
	data1 = copyMe.data1;
	data2 = copyMe.data2;
	data3 = copyMe.data3;

	for (const auto& index : range(8))
		 data4[index] = copyMe.data4[index];
}
