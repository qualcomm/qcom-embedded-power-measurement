// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMChannel.h"

// QCommon
#include "KratosMapping.h"
#include "StringUtilities.h"

// libEPM
#include "EpmLibPrivate.h"

_EPMChannel::_EPMChannel()
{
}

EPMChannel _EPMChannel::copyToNewEPMChannel
(
	EPMChannel copyMe
)
{
	EPMChannel result = EPMChannel(new _EPMChannel);

	result->setActive(copyMe->active());
	result->_kratosMapping = copyMe->_kratosMapping;
	result->_valid = copyMe->_valid;
	result->_index = copyMe->_index;
	result->_indexType = copyMe->_indexType;
	result->_channelType = copyMe->_channelType;
	result->_waveFormType = copyMe->_waveFormType;
	result->_seriesName = copyMe->_seriesName;
	result->_seriesHash = copyMe->_seriesHash;
	result->_channelName = copyMe->_channelName;
	result->_channelHash = copyMe->_channelHash;
	result->_channel = copyMe->_channel;
	result->_rcmChannel = copyMe->_rcmChannel;
	result->_channelColor = copyMe->_channelColor;
	result->_category = copyMe->_category;
	result->_resistorValue = copyMe->_resistorValue;
	result->_dumpRawValues = copyMe->_dumpRawValues;

	return result;
}

EPMChannel _EPMChannel::findChannelByHash
(
	HashType channelHash,
	EPMChannels& epmChannels
)
{
	for (auto& channel: epmChannels)
	{
		if (channel->channelHash() == channelHash)
			return channel;
	}

	return EPMChannel();
}

EPMChannel _EPMChannel::findSeriesByHash
(
	HashType seriesHash,
	EPMChannels &epmChannels
)
{
	for (auto& channel: epmChannels)
	{
		if (channel->seriesHash() == seriesHash)
			return channel;
	}

	return EPMChannel();
}

void _EPMChannel::setKratosMapping
(
	KratosMapping kratosMapping
)
{
	_kratosMapping = kratosMapping;
	_valid = false;

	setupChannel();
}

BasicColor _EPMChannel::channelColor() const
{
	return _channelColor;
}

void _EPMChannel::setChannelColor
(
	const BasicColor& channelColor
)
{
	_channelColor = channelColor;
}

QString _EPMChannel::channelName() const
{
	return _channelName;
}

void _EPMChannel::setChannelName
(
	const QString& channelName
)
{
	_channelName = channelName;

	_channelHash = strHash(channelName);
}

QString _EPMChannel::seriesName() const
{
	return _seriesName;
}

void _EPMChannel::setSeriesName
(
	const QString &seriesName
)
{
	_seriesName = seriesName;

	createChannelName(_seriesName, _seriesHash, _channelName, _channelHash, _waveFormType);

	switch (_waveFormType)
	{
	case eWaveFormCurrent: _channelType = eCurrent; break;
	case eWaveFormVoltage: _channelType = eVoltage; break;
	case eWaveFormMarker: _channelType = eMarker; break;
	default:
		break;
	}
}

void _EPMChannel::setWaveForm
(
	const WaveFormType& waveForm
)
{
	_waveFormType = waveForm;
	switch (_waveFormType)
	{
	case eWaveFormCurrent:
		_channelType = eCurrent;
		break;

	case eWaveFormVoltage:
		_channelType = eVoltage;
		break;

	case eWaveFormMarker:
		_channelType = eMarker;
		break;

	default:
		_channelType = eNotSet;
	}
}

WaveFormType _EPMChannel::waveForm() const
{
	return _waveFormType;
}

HashType _EPMChannel::channelHash() const
{
	return _channelHash;
}

HashType _EPMChannel::seriesHash() const
{
	return _seriesHash;
}

quint32 _EPMChannel::key() const
{
	quint32 result;

	if (_indexType == eMarkerIndex)
		result = 0x00010000;
	else if (_indexType == eRCMIndex)
		result = 0x00100000;
	else
		result = 0x01000000;

	result = result | (0x0000FFFF & _index);

	return result;
}

void _EPMChannel::setIndex
(
	quint32 index
)
{
	if (_index != index)
	{
		_valid = false;
		_index = index;
		setupChannel();
	}
}

void _EPMChannel::setIndexType
(
	EPMIndexType epmIndexType
)
{
	if (_indexType != epmIndexType)
	{
		_valid = false;
		_indexType = epmIndexType;
		setupChannel();
	}
}

void _EPMChannel::setChannelType
(
	ChannelType epmChannelType
)
{
	if (_channelType != epmChannelType)
	{
		_valid = false;
		_channelType = epmChannelType;
		setupChannel();
	}
}

quint32 _EPMChannel::channel() const
{
	return _channel;
}

void _EPMChannel::setChannel(quint32 channel)
{
	if (_channel != channel)
	{
		_channel = channel;
	}
}

quint32 _EPMChannel::rcmChannel() const
{
	return _rcmChannel;
}

void _EPMChannel::setRcmChannel(quint32 rcmChannel)
{
	_rcmChannel = rcmChannel;
}

_EPMChannel::_EPMChannel(const _EPMChannel &copyMe)
{
	_active = copyMe._active;
	_indexType = copyMe._indexType;
	_index = copyMe._index;
	_category = copyMe._category;
	_description = copyMe._description;
	_resistorValue = copyMe._resistorValue;
	_active = copyMe._active;
	_channelType = copyMe._channelType;
	_valid = copyMe._valid;
	_waveFormType = copyMe._waveFormType;
	_seriesName = copyMe._seriesName;
	_seriesHash = copyMe._seriesHash;
	_channelName = copyMe._channelName;
	_channelHash = copyMe._channelHash;
	_channel = copyMe._channel;
	_rcmChannel = copyMe._rcmChannel;
	_channelColor = copyMe._channelColor;
	_kratosMapping = copyMe._kratosMapping;
}

void _EPMChannel::setupChannel()
{
	Q_ASSERT(_kratosMapping.isNull() == false);

	if (_valid == false)
	{
		if (_index != kNotSetInt)
		{
			if (_channelType != eNotSet)
			{
				switch (_indexType)
				{
				case eMarkerIndex:
					compileAMarkerChannel();
					break;

				case eRCMIndex:
					compileARCMChannel();
					break;

				case eSPMIndex:
					compileASPMChannel();
					break;

				default:
					break;
				}
			}
		}
	}
}

void _EPMChannel::compileAMarkerChannel()
{
	setChannel(static_cast<quint32>(_kratosMapping->markerToMarkerChannel(_index)));
	_valid = true;
}

void _EPMChannel::compileARCMChannel()
{
	quint32 rcmChannel = _index;
	quint32 spmChannel = _kratosMapping->rcmChannelToSpmChannel(rcmChannel);
	quint32 rcmCfgChan;

	if (_kratosMapping->kratosMapping())
		rcmCfgChan = _kratosMapping->rcmChannelToKratosChannel(rcmChannel);
	else
		rcmCfgChan = rcmChannel;

	switch (_channelType)
	{
	case _EPMChannel::eCurrent:
		setChannel(_kratosMapping->spmChannelToCurrentChannel(spmChannel));
		setRcmChannel(rcmCfgChan);
		break;

	case _EPMChannel::eVoltage:
		setChannel(_kratosMapping->spmChannelToVoltageChannel(spmChannel));
		setRcmChannel(rcmCfgChan);
		break;

	default:
		break;
	}

	_valid = true;
}

void _EPMChannel::compileASPMChannel()
{
	quint32 rcmCfgChan;
	quint32 rcmChannel;
	quint32 spmChannel = _index;
	if (_kratosMapping->rcmMapping())
	{
		rcmChannel = _kratosMapping->spmChannelToRcmChannel(spmChannel);
		if (_kratosMapping->kratosMapping())
			rcmCfgChan = _kratosMapping->rcmChannelToKratosChannel(rcmChannel);
		else
			rcmCfgChan = rcmChannel;
	}
	else
	{
		rcmCfgChan = spmChannel;
	}

	setRcmChannel(rcmCfgChan);

	switch (_channelType)
	{
	case eCurrent:
		setChannel(_kratosMapping->spmChannelToCurrentChannel(spmChannel));
		break;

	case eVoltage:
		setChannel(_kratosMapping->spmChannelToVoltageChannel(spmChannel));
		break;

	default:
		break;
	}
	_valid = true;
}
