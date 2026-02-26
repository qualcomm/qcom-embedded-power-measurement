#ifndef EPMRESULT_H
#define EPMRESULT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
