#ifndef UDASBLOCK_H
#define UDASBLOCK_H
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
// Copyright 2013-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)	
			Biswajit Roy (biswroy@qti.qualcomm.com)	
*/

// EPMLib
#include "EPMGlobalLib.h"
#include "WaveForms.h"

// QCommonConsole
#include "BasicColor.h"
#include "DataPoints.h"
#include "Statistics.h"
#include "StringUtilities.h"

// Qt
#include <QDataStream>
#include <QFile>
#include <QPointF>
#include <QList>
#include <QSharedPointer>
#include <QString>

const qreal kTimeUnset(0.0);

class UDASFile;

struct EPMLIB_EXPORT _UDASBlock
{
	_UDASBlock(UDASFile* parent)
	{
		_parent = parent;
	}

	~_UDASBlock()
	{
		_ySeries.clear();
	}

	UDASFile* udasFile()
	{
		return _parent;
	}

	// Channel Data
	QString channelName() const;
	HashType channelHash() const;
	BasicColor channelColor() const;
	void setChannelColor(const BasicColor& seriesColor);

	// SeriesData
	QString seriesName() const;
	void setSeriesName(const QString& seriesName);
	HashType seriesHash() const;
	qreal seriesValue(quint32 index);
	quint32 seriesCount();
	qreal seriesTime(quint32 index);

	// attributes
	qreal duration();

	void setRange(qreal timeStart, qreal timeEnd);
	qreal rangeDuration();

	bool read(QDataStream& dataStream);
	void readDataSeries(const QString& path);

	void write(QDataStream& dataStream);

	qreal deltaTime();

	// Statistics API
	qreal statAvg(qreal startTime = kTimeUnset, qreal endTime = kTimeUnset);
	qreal statDuration(qreal startTime = kTimeUnset, qreal endTime = kTimeUnset);
	qreal statArea(qreal startTime = kTimeUnset, qreal endTime = kTimeUnset);
	qreal statPopulationStdDev(qreal startTime = kTimeUnset, qreal endTime = kTimeUnset);
	qreal statMax(qreal startTime = kTimeUnset, qreal endTime = kTimeUnset);
	qreal statRange(qreal startTime = kTimeUnset, qreal endTime = kTimeUnset);
	qreal statMin(qreal startTime = kTimeUnset, qreal endTime = kTimeUnset);

	UDASFile*					_parent{Q_NULLPTR};
	quint32						_channelNumber{0};
	WaveFormType				_waveFormType{eWaveFormUnset};
	quint32						_dataFileNumber{0xFFFFFFFF};
	qreal						_timeBetweenSamples{0.};
	qreal						_timeDeltaFromZero{0.};
	qreal						_timeDuration{0.};
	qreal						_dataRange{0.};
	qreal						_minSample{999999.};
	qreal						_maxSample{0.};
	quint32						_sampleCount{0};
	qreal						_unitMultiplier{0.};
	qreal						_clockPeriod{0.};
	GraphDataVector				_ySeries;

	// unused
	qreal						_5PercentAccuracy{0.};
	qreal						_1PercentAccuracy{0.};
	qreal						_dcr{0.};
	quint32						_saturationCounter{0};

	// calculated
	Statistics					_statistics;

	Statistics					_rangedStatistics;
	qreal						_rangeStart{0.};
	qreal						_rangeEnd{0.};

private:
	Q_DISABLE_COPY(_UDASBlock)

	QString						_channelName;
	HashType					_channelHash{0};
	BasicColor					_channelColor;

	QString						_seriesName;
	HashType					_seriesHash{0};
};

typedef QSharedPointer<_UDASBlock> UDASBlock;
typedef QList<UDASBlock> UDASBlocks;
typedef QVector<UDASBlock> UDASBlockVector;

#endif // UDASBlock
