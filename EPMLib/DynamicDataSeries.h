#ifndef DYNAMICDATASERIES_H
#define DYNAMICDATASERIES_H
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
// Copyright 2021-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "DataPoints.h"

// libEPM
#include "EPMChannel.h"

// Qt
#include <QVector>

class LineSeriesModel;

const uint kLastEntry(0xFFFFFFFF);

class EPMLIB_EXPORT DynamicDataSeries
{
public:
	DynamicDataSeries(const qreal duration, const EPMChannel& epmChannel);

	int count() const
	{
		return _ySeries.size();
	}

	qreal duration() const
	{
		return _duration;
	}

	qreal xMin() const
	{
		if (_ySeries.count())
			return _ySeries.at(0).x();

		return 0.;
	}

	qreal xMax() const
	{
		if (_ySeries.count())
			return _ySeries.at(_ySeries.count() - 1).x();

		return 10.;
	}

	qreal yMax() const
	{
		return _max;
	}

	qreal yMin() const
	{
		return _min;
	}

	BasicColor seriesColor()
	{
		return _seriesColor;
	}
	void setSeriesColor(const BasicColor& seriesColor)
	{
		_seriesColor = seriesColor;
	}

	HashType seriesHash()
	{
		return _seriesHash;
	}

	quint32 channelNumber()
	{
		return _channelNumber;
	}

	virtual void addDataPoint(QPointF dataPoint);
	virtual QPointF value(uint i) const;

	GraphDataPoints				_ySeries;
	qreal						_duration{0.};
	qreal						_min{99999.0};
	qreal						_max{-99999.0};
	quint32						_seriesDurationCount{0};
	quint32						_currentSeriesIndex{0};

protected:
	WaveFormType				_waveFormType{eWaveFormUnset};
	QString						_channelName;
	quint32						_channelNumber{0xFFFF};
	HashType					_channelHash{0};

	QString						_seriesName;
	HashType					_seriesHash{0};

	BasicColor					_seriesColor;
};

#endif // DYNAMICDATASERIES_H
