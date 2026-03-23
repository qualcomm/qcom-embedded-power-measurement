#ifndef UDASCHANNELRECORDING_H
#define UDASCHANNELRECORDING_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
