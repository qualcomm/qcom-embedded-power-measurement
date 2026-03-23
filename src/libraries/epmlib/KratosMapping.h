#ifndef KRATOSMAPPING_H
#define KRATOSMAPPING_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
const int kQEPMTupleIndex(1);
const int kSPMTupleIndex(2);
const int kMarkerTupleIndex(3);

const quint32 kInvalidChannel(0xFFFFFFFF);

// index, <legacy, QEPM, spm>
typedef QMap<quint32, std::tuple<quint32 /*kLegacyTupleIndex*/, quint32 /*kQEPMTupleIndex*/, quint32 /*kSPMTupleIndex*/, quint32 /*kMarkerTupleIndex*/>> ChannelMap;

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
