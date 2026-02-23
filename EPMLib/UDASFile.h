#ifndef UDASFILE_H
#define UDASFILE_H
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
// Copyright 2013-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"
#include "PRNFile.h"
#include "UDASBlock.h"

// Qt
#include <QDataStream>
#include <QFile>
#include <QPointF>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>

// C++
#include <tuple>

typedef QList<HashType> HashList;
typedef QSet<HashType> HashSet;

typedef std::tuple<HashType, HashType, HashType, HashType> HashTuple;
typedef QList<HashTuple> HashTuples;

const HashType kNoHash{0};

struct EPMLIB_EXPORT ExportEntry
{
	ExportEntry() = default;
    ExportEntry(const ExportEntry& copyMe) = default;
    ExportEntry& operator = (const ExportEntry& copyMe) = default;

	HashType _channelHash{kNoHash};
	HashType _currentHash{kNoHash};
	HashType _voltageHash{kNoHash};
	HashType _powerHash{kNoHash};
};

typedef QList<ExportEntry> ExportEntries;

class EPMLIB_EXPORT UDASFile
{
public:
	UDASFile();

	void reset();

	QString lastErrorMessage();

	// for an existing UDAS file
	bool open(const QString& sclPath);

	void updateWithPRN(const PRNFile& prnFile);

	// for UDAS file creation
	bool create(const QString& sclPath);
	void addBlock(UDASBlock& addMe);

	bool save();
	void close();

	void setExportTimeSpan(qreal start, qreal end);
	bool exportAsCVS(const QString& destinationFolder, HashTuples& exportedSeries);
	bool exportAsExcel(const QString& destinationFolder, HashTuples& exportedSeries, bool quitOnFinish);

	qreal timeIncrement(ExportEntries& exportEntries);
	qreal timeIncrement();

	bool isValid()
	{
		bool result(true);

		if (_currentBlocks.isEmpty())
			if (_voltageBlocks.isEmpty())
				if (_markerBlocks.isEmpty())
					if(_powerBlocks.isEmpty())
						result = false;

		return result;
	}

	int blockCount()
	{
		return _allBlocks.count();
	}

	int currentBlockCount()
	{
		return _currentBlocks.count();
	}

	int voltageBlockCount()
	{
		return _voltageBlocks.count();
	}

	int powerBlockCount()
	{
		return _powerBlocks.count();
	}

	int markerBlockCount()
	{
		return _markerBlocks.count();
	}

	UDASBlock getBlock(int index);
	UDASBlock getBlockBySeriesName(const QString& seriesName);

	UDASBlock getCurrentBlock(int index);
	UDASBlock getCurrentBlockByChannelHash(HashType hash);
	UDASBlock getCurrentBlockBySeriesHash(HashType hash);

	UDASBlock getVoltageBlock(int index);
	UDASBlock getVoltageBlockByChannelHash(HashType hash);
	UDASBlock getVoltageBlockBySeriesHash(HashType hash);

	UDASBlock getPowerBlock(int index);
	UDASBlock getPowerBlockByChannelHash(HashType hash);
	UDASBlock getPowerBlockBySeriesHash(HashType hash);

	UDASBlock getMarkerBlock(int index);
	UDASBlock getMarkerBlockByChannelHash(HashType hash);
	UDASBlock getMarkerBlockBySeriesHash(HashType hash);

	HashList buildSortedIVHashList();

	BasicColor getChannelColor(HashType channelHash);

	QString						_target;
	QString						_paramsFile;
	QString						_paramsPath;

	qreal						_duration = 0.0;
	qreal						_maxCurrentRange = 0.0;
	qreal						_maxVoltageRange = 0.0;
	qreal						_maxPowerRange = 0.0;
	qreal						_exportStart = 0.0;
	qreal						_exportEnd = 0.0;
	QString						_date;
	QString						_time;
	QString						_label;
	QString						_comments;

private:
	void setCurrentTime();

	bool readDataSetHeader();
	bool readABlock(QDataStream& dataStream);
	void buildVirtualBlocks();

	// save
	void writeDataSetHeader();
	void writeEOFBlock();

	QFile						_file;
	QString						_lastErrorMessage;
	QDataStream					_dataStream;
	UDASBlocks					_allBlocks;
	UDASBlocks					_currentBlocks;
	UDASBlocks					_voltageBlocks;
	UDASBlocks					_powerBlocks;
	UDASBlocks					_markerBlocks;
};

#endif // UDASFILE_H
