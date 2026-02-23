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

