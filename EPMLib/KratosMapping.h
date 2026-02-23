#ifndef KRATOSMAPPING_H
#define KRATOSMAPPING_H
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
// Copyright 2018-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"

// QCommon
#include "SPMVersion.h"

// QT
#include <QMap>
#include <QSharedPointer>

// C++
#include <tuple>

const int kLegacyTupleIndex(0);
const int kAlpacaTupleIndex(1);
const int kSPMTupleIndex(2);
const int kMarkerTupleIndex(3);

const quint32 kInvalidChannel(0xFFFFFFFF);

// index, <legacy, alpaca, spm>
typedef QMap<quint32, std::tuple<quint32 /*kLegacyTupleIndex*/, quint32 /*kAlpacaTupleIndex*/, quint32 /*kSPMTupleIndex*/, quint32 /*kMarkerTupleIndex*/>> ChannelMap;

class EPMLIB_EXPORT _KratosMapping
{
public:
	_KratosMapping();

	quint32 spmVersion()
	{
		return _spmVersion;
	}

	void setSPMVersion(SPMVersion spmVersion);

	bool rcmMapping()
	{
		return _rcmMapping;
	}

	void setRCMMapping(bool state)
	{
		_rcmMapping = state;
	}

	bool kratosMapping()
	{
		return _useKratosMapping;
	}

	void useKratosMapping(bool state)
	{
		_useKratosMapping = state;
	}

	quint32 markerToMarkerChannel(quint32 marker) const;
	quint32 markerChannelToSPMChannel(quint32 markerChannel) const;
	quint32 rcmChannelToSpmChannel(quint32 rcmChannel) const;
	quint32 rcmChannelToKratosChannel(quint32 rcmChannel) const;
	quint32 spmChannelToBus(quint32 spmChannel) const;
	quint32 spmChannelToAdc(quint32 spmChannel) const;
	quint32 spmChannelToCurrentChannel(quint32 spmChannel) const;
	quint32 spmChannelToVoltageChannel(quint32 spmChannel) const;
	quint32 spmChannelToRcmChannel(quint32 spmChannel) const;

private:
	SPMVersion					_spmVersion;
	bool						_useKratosMapping;
	bool						_rcmMapping;
};

typedef QSharedPointer<_KratosMapping> KratosMapping;

#endif // KRATOSMAPPING_H
