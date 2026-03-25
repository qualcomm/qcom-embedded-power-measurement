// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
