#ifndef DATASERIES_H
#define DATASERIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"

// QCommonConsole
#include "BasicColor.h"
#include "DataPoints.h"
#include "Statistics.h"
#include "UDASBlock.h"
#include "WaveForms.h"

// Qt
#include <QtGlobal>
#include <QMap>
#include <QSharedPointer>

class LineSeriesModel;

class EPMLIB_EXPORT _DataSeries
{
friend class LineSeriesModel;

public:
	_DataSeries() = default;

	_DataSeries(UDASBlock udasBlock)
	{
		_waveFormType = udasBlock->_waveFormType;
		_channelName = udasBlock->channelName();
		_channelHash = udasBlock->channelHash();
		_channelNumber = udasBlock->_channelNumber;

		_seriesName = udasBlock->seriesName();
		_seriesHash = udasBlock->seriesHash();
		_seriesColor = udasBlock->channelColor();

		_timeBetweenSamples = udasBlock->_timeBetweenSamples;
		_minSample = udasBlock->_minSample;
		_maxSample = udasBlock->_maxSample;
		_ySeries = udasBlock->_ySeries;

		_statistics.setSpan(_ySeries);
	}

	Q_DISABLE_COPY(_DataSeries)

	WaveFormType seriesType()
	{
		return _waveFormType;
	}

	QString channelName()
	{
		return _channelName;
	}

	uint channelNumber()
	{
		return _channelNumber;
	}

	QString seriesName()
	{
		return _seriesName;
	}

	BasicColor seriesColor()
	{
		return _seriesColor;
	}

	void setSeriesColor(const BasicColor& seriesColor)
	{
		_seriesColor = seriesColor;
	}

	HashType seriesHash() const
	{
		return _seriesHash;
	}

	qreal duration() const
	{
		return _timeBetweenSamples;
	}

	uint count() const
	{
		return static_cast<uint>(_ySeries.count());
	}

	virtual QPointF value(uint i) const;

	qreal min()
	{
		return _statistics.min();
	}

	qreal max()
	{
		return _statistics.max();
	}

	qreal minSample()
	{
		return _minSample;
	}

	qreal maxSample()
	{
		return _maxSample;
	}

protected:
    GraphDataVector				_ySeries;
    Statistics					_statistics;

	WaveFormType				_waveFormType{eWaveFormUnset};
	QString						_channelName;
	quint32						_channelNumber;
	HashType					_channelHash{0};

	QString						_seriesName;
	HashType					_seriesHash{0};

	qreal						_timeBetweenSamples{0.};
	qreal						_minSample{999999.};
	qreal						_maxSample{0.};

	BasicColor					_seriesColor;
};

typedef QSharedPointer<_DataSeries> DataSeries;
typedef QMap<HashType, DataSeries> DataSeriesMap;
typedef DataSeriesMap::iterator DataSeriesMapIter;

#endif // DATASERIES_H
