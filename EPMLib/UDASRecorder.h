#ifndef UDASRECORDER_H
#define UDASRECORDER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"
#include "RecordingInferface.h"
#include "UDASFile.h"
#include "UDASChannelRecording.h"

class EPMLIB_EXPORT UDASRecorder :
	public RecordingInterface
{
public:
	UDASRecorder()
	{

	}

	virtual ~UDASRecorder()
	{
	}

	void setResultsFolder(const QString resultsFolder)
	{
		_resultsFolder = resultsFolder;
	}

	void logRaw(UDASChannelRecording& channelRecording, const MicroEpmChannelData& channelData);

	virtual void startRecording();
	virtual void stopRecording();
	virtual void recordData(MicroEpmChannelData* channelData, quint32 sampleCount);

private:

	QString						_resultsFolder;
	UDASChannelRecording		_chanRecordings[MICRO_EPM_MAX_NUMBER_CHANNELS];
	UDASFile					_udasFile;
	uint64_t					u64TimestampStart;
	uint64_t					u64TimestampStop;

	bool						_record = false;
};

#endif // UDASRECORDER_H
