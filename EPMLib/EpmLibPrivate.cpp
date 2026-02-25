// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EpmLibPrivate.h"

// libEPM
#include "WaveForms.h"

// QCommon
#include "StringUtilities.h"

void createChannelName
(
	const QString& seriesName,
	HashType& seriesHash,
	QString& channelName,
	HashType& channelHash,
	WaveFormType& waveFormType
)
{
	if (seriesName.endsWith("_I", Qt::CaseInsensitive))
	{
		channelName = seriesName.left(seriesName.length() - 2);
		waveFormType = eWaveFormCurrent;
	}
	else if (seriesName.endsWith("_V", Qt::CaseInsensitive))
	{
		channelName = seriesName.left(seriesName.length() - 2);
		waveFormType = eWaveFormVoltage;
	}
	else if (seriesName.endsWith("_P", Qt::CaseInsensitive))
	{
		channelName = seriesName.left(seriesName.length() - 2);
		waveFormType = eWaveFormPower;
	}
	else
	{
		channelName = seriesName;

		if (waveFormType == eWaveFormVoltage || waveFormType == eWaveFormUnset)
			waveFormType = eWaveFormMarker;
	}

    seriesHash = strHash(seriesName);
    channelHash = strHash(channelName);
}

