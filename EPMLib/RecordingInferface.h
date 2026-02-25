#ifndef RECORDINGINFERFACE_H
#define RECORDINGINFERFACE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMDevice.h"

class RecordingInterface
{
public:
	
	virtual ~RecordingInterface()
	{
		
	}
	
	void setEPMDevice(_EPMDevice* epmDevice)
	{
		_epmDevice = epmDevice;
	}

	virtual void startRecording() = 0;
	virtual void stopRecording() = 0;
	virtual void recordData(MicroEpmChannelData* channelData, quint32 uNumSamples) = 0;

protected:
	_EPMDevice*					_epmDevice{Q_NULLPTR};
};

#endif // RECORDINGINFERFACE_H
