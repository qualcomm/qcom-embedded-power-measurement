#ifndef UDASCHANNELRECORDING_H
#define UDASCHANNELRECORDING_H
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

// libEPM
#include "UDASFile.h"

// Qt
#include <QFile>
#include <QDataStream>
#include <QTextStream>

struct MicroEpmChannelData;
struct MicroEpmChannelInfo;

class UDASChannelRecording
{
public:
	UDASChannelRecording() = default;
	~UDASChannelRecording();

	bool open(const QString& filePath, UDASFile* parent);
	void close();

	void logRaw(const MicroEpmChannelData& channelData);
	void logSample(double dbPhysical, quint32 uRawTimestamp);

	void saveWaveFormBlock(MicroEpmChannelInfo* pChanInfo);

	UDASFile*					_parent{Q_NULLPTR};

	QFile						_recordingFile;
	QDataStream					_recordingDataStream;

	QFile*						_rawLogFile{Q_NULLPTR};
	QTextStream*				_rawDataStream{Q_NULLPTR};

	uint64_t					u64TimestampStart{0};
	uint64_t					u64TimestampStop{0};
	quint32						uNumSamples{0};
	quint32						uFileIdx{0};
	quint32						uRcmChannel{0};
	quint32						uChanIdx{0};
	double						_min{100000.0};
	double						_max{-1000000.0};
	bool						_logRaw{false};
};

#endif // UDASCHANNELRECORDING_H
