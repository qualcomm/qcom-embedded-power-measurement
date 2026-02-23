#ifndef EPMRESULT_H
#define EPMRESULT_H
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
// Copyright 2018-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// Qt
#include <QList>
#include <QString>

struct EPMResult
{
	EPMResult()
	{
		_channel = static_cast<quint32>(-1);
		_average = 0.0;
		_min = 0.0;
		_max = 0.0;
		_sampleCount = 0;
	}

	EPMResult(const EPMResult& copyMe)
	{
		_channel = copyMe._channel;
		_channelName = copyMe._channelName;
		_units = copyMe._units;
		_average = copyMe._average;
		_min = copyMe._min;
		_max = copyMe._max;
		_sampleCount = copyMe._sampleCount;
	}

	quint32						_channel;
	QString						_channelName;
	QString						_units;
	double						_average;
	double						_min;
	double						_max;
	quint32						_sampleCount;
};

typedef QList<EPMResult> EPMResults;

#endif // EPMRESULT_H
