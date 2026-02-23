#ifndef BOARDUUID_H
#define BOARDUUID_H
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

#include "EPMGlobalLib.h"

#include <qglobal.h>
#include <QUuid>

#pragma pack(1)
struct EPMLIB_EXPORT MicroEpmEepromUuid
{
	MicroEpmEepromUuid(){};
	MicroEpmEepromUuid(const MicroEpmEepromUuid& copyMe);

    MicroEpmEepromUuid& operator=(const MicroEpmEepromUuid& assignMe)
    {
        data1 = assignMe.data1;
        data2 = assignMe.data2;
        data3 = assignMe.data3;

        data4[0] = assignMe.data4[0];
        data4[1] = assignMe.data4[1];
        data4[2] = assignMe.data4[2];
        data4[3] = assignMe.data4[3];
        data4[4] = assignMe.data4[4];
        data4[5] = assignMe.data4[5];
        data4[6] = assignMe.data4[6];
        data4[7] = assignMe.data4[7];

        return *this;
    }

	quint32 data1 {0};
	quint16 data2 {0};
	quint16 data3 {0};
	quint8 data4[8] {0, 0, 0, 0, 0, 0, 0, 0};
};
#pragma pack()

class EPMLIB_EXPORT BoardUuid
{
public:
	BoardUuid();
	BoardUuid(const BoardUuid& copyMe);
	BoardUuid(QUuid& quuid);
	BoardUuid(MicroEpmEepromUuid& boardUuid);

	BoardUuid& operator=(const BoardUuid& assignMe)
	{
		data1 = assignMe.data1;
		data2 = assignMe.data2;
		data3 = assignMe.data3;

		data4[0] = assignMe.data4[0];
		data4[1] = assignMe.data4[1];
		data4[2] = assignMe.data4[2];
		data4[3] = assignMe.data4[3];
		data4[4] = assignMe.data4[4];
		data4[5] = assignMe.data4[5];
		data4[6] = assignMe.data4[6];
		data4[7] = assignMe.data4[7];

		return *this;
	}

	MicroEpmEepromUuid toMicroEPMUuid();
	QUuid toQUuid();

	quint32 data1{0};
	quint16 data2{0};
	quint16 data3{0};
	quint8 data4[8]{0, 0, 0, 0, 0, 0, 0, 0};
};

inline QString MicroEpmConvertUuidToString
(
	BoardUuid uuid
)
{
	QString result = QString("%1-%2-%3-%5%6%7%8%9%10%11%12").
			arg(uuid.data1, 8, 16, QChar('0')).
			arg(uuid.data2, 4, 16, QChar('0')).
			arg(uuid.data3, 4, 16, QChar('0')).
			arg(uuid.data4[0], 2, 16, QChar('0')).
			arg(uuid.data4[1], 2, 16, QChar('0')).
			arg(uuid.data4[2], 2, 16, QChar('0')).
			arg(uuid.data4[3], 2, 16, QChar('0')).
			arg(uuid.data4[4], 2, 16, QChar('0')).
			arg(uuid.data4[5], 2, 16, QChar('0')).
			arg(uuid.data4[6], 2, 16, QChar('0')).
			arg(uuid.data4[7], 2, 16, QChar('0'));

	return result;
}

#endif // BOARDUUID_H
