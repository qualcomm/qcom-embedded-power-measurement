// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "KratosMapping.h"

static ChannelMap kQEPMMapping
{
	{0, std::make_tuple(0, 0, 54, -1)},
	{1, std::make_tuple(1, 16, 2, 128)},
	{2, std::make_tuple(2, 17, 4, 129)},
	{3, std::make_tuple(3, 18, 5, 130)},
	{4, std::make_tuple(4, 19, 6, 131)},
	{5, std::make_tuple(5, 20, 7, 132)},
	{6, std::make_tuple(6, 21, 8, kInvalidChannel)},
	{7, std::make_tuple(7, 22, 9, kInvalidChannel)},
	{8, std::make_tuple(8, 23, 10, kInvalidChannel)},
	{9, std::make_tuple(9, 24, 11, kInvalidChannel)},
	{10, std::make_tuple(10, 25, 12, kInvalidChannel)},
	{11, std::make_tuple(11, 26, 13, kInvalidChannel)},
	{12, std::make_tuple(12, 27, 14, kInvalidChannel)},
	{13, std::make_tuple(13, 28, 15, kInvalidChannel)},
	{14, std::make_tuple(14, 29, 16, kInvalidChannel)},
	{15, std::make_tuple(15, 30, 17, kInvalidChannel)},
	{16, std::make_tuple(16, 70, 19, kInvalidChannel)},
	{17, std::make_tuple(17, 71, 21, kInvalidChannel)},
	{18, std::make_tuple(18, 72, 23, kInvalidChannel)},
	{19, std::make_tuple(19, 73, 25, kInvalidChannel)},
	{20, std::make_tuple(20, 74, 27, kInvalidChannel)},
	{21, std::make_tuple(21, 75, 41, kInvalidChannel)},
	{22, std::make_tuple(22, 76, 42, kInvalidChannel)},
	{23, std::make_tuple(23, 77, 43, kInvalidChannel)},
	{24, std::make_tuple(24, 78, 44, kInvalidChannel)},
	{25, std::make_tuple(25, 79, 45, kInvalidChannel)},
	{26, std::make_tuple(26, 80, 46, kInvalidChannel)},
	{27, std::make_tuple(27, 81, 47, kInvalidChannel)},
	{28, std::make_tuple(28, 52, 52, kInvalidChannel)}
};

_KratosMapping::_KratosMapping() :
	_useKratosMapping(false),
	_rcmMapping(true)
{
	setSPMVersion(eQEPMVersion);
}

void _KratosMapping::setSPMVersion
(
	SPMVersion spmVersion
)
{
	_spmVersion = spmVersion;
}

quint32 _KratosMapping::markerToMarkerChannel
(
	quint32 marker
) const
{
	return std::get<kMarkerTupleIndex>(kQEPMMapping[marker]);
}

quint32 _KratosMapping::markerChannelToSPMChannel
(
	quint32 markerChannel
) const
{
	return markerChannel;
}

quint32 _KratosMapping::rcmChannelToSpmChannel
(
	quint32 rcmChannel
) const
{
	quint32 result(rcmChannel);

	if (_spmVersion == eLegacyVersion)
	{
		auto kme = kQEPMMapping.find(rcmChannel);
		if (kme != kQEPMMapping.end())
		{
			result = std::get<kSPMTupleIndex>(kme.value());
		}
		else
		{
			result = kInvalidChannel;
		}
	}

	return result;
}

quint32 _KratosMapping::rcmChannelToKratosChannel
(
	quint32 rcmChannel
) const
{
	quint32 result(rcmChannel);

	if (_spmVersion == eLegacyVersion)
	{
		result = kInvalidChannel;

		auto kme = kQEPMMapping.find(rcmChannel);
		if (kme != kQEPMMapping.end())
		{
			result = std::get<kQEPMTupleIndex>(kme.value());
		}
	}

	return 	result;
}

quint32 _KratosMapping::spmChannelToBus
(
	quint32 spmChannel
)  const
{
	return spmChannel % 4;
}

quint32 _KratosMapping::spmChannelToAdc
(
	quint32 spmChannel
) const
{
	return static_cast<quint32>(static_cast<double>(spmChannel) / 4.0);
}

quint32 _KratosMapping::spmChannelToCurrentChannel(quint32 spmChannel) const
{
	quint32 bus = spmChannelToBus(spmChannel);
	quint32 adc = spmChannelToAdc(spmChannel);

	return bus * 32 + adc * 2;
}

quint32 _KratosMapping::spmChannelToVoltageChannel
(
	quint32 spmChannel
) const
{
	return spmChannelToCurrentChannel(spmChannel) + 1;
}

quint32 _KratosMapping::spmChannelToRcmChannel
(
	quint32 spmChannel
) const
{
	quint32 result(spmChannel);

	if (_spmVersion == eLegacyVersion)
	{
		result = kInvalidChannel;

		auto kme = kQEPMMapping.begin();
		while (kme != kQEPMMapping.end())
		{
			if (spmChannel == std::get<kSPMTupleIndex>(kme.value()))
			{
				result = kme.key();
				break;
			}

			kme++;
		}
	}

	return 	result;
}

