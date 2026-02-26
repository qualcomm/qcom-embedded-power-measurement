#ifndef EPMCHANNEL_H
#define EPMCHANNEL_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"
#include "EPMLibDefines.h"
#include "KratosMapping.h"

// QCommonConsole
#include "BasicColor.h"
#include "EPMIndexType.h"
#include "StringUtilities.h"
#include "WaveForms.h"

// Qt
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>

const quint32 kNotSetInt(0xFFFFFFFF);

class _EPMChannel;

typedef QSharedPointer<_EPMChannel> EPMChannel;

typedef QList<EPMChannel>			EPMChannels;
typedef QMap<HashType, EPMChannel>	EPMChannelMap;

class EPMLIB_EXPORT _EPMChannel
{
public:
	enum FieldPosition
	{
		eEnabled = 0,
		eType,
		eIndex,
		eName,
		eGroup,
		eDescription,
		eResistorValue,
		eLast
	};

	enum ChannelType
	{
		eNotSet,
		eCurrent,
		eVoltage,
		ePower,
		eMarker
	};

	_EPMChannel();

	int fieldCount()
	{
		return 7; // increment if any fields are added
	}

	static EPMChannel copyToNewEPMChannel(EPMChannel copyMe);
	static EPMChannel findChannelByHash(HashType channelHash, EPMChannels& epmChannels);
	static EPMChannel findSeriesByHash(HashType seriesHash, EPMChannels& epmChannels);

	void setKratosMapping(KratosMapping kratosMapping);

	// The rail name, i.e. Battery
	QString channelName() const;
	void setChannelName(const QString& seriesName);

	HashType channelHash() const;

	BasicColor channelColor() const;
	void setChannelColor(const BasicColor& channelColor);

	// The series name, i.e. Battery_I, Battery_V
	QString seriesName() const;
	void setSeriesName(const QString& seriesName);
	HashType seriesHash() const;

	WaveFormType waveForm() const;
	void setWaveForm(const WaveFormType& waveForm);

	bool active()
	{
		return _active;
	}
	void setActive(bool active)
	{
		_active = active;
	}

	quint32 key() const;

	quint32 index() const
	{
		return _index;
	}
	void setIndex(quint32 index);

	EPMIndexType indexType(void) const
	{
		return _indexType;
	}
	void setIndexType(EPMIndexType epmIndexType);

	quint32 channel() const;
	void setChannel(quint32 channel);

	ChannelType channelType(void) const
	{
		return _channelType;
	}
	void setChannelType(ChannelType epmChannelType);

	quint32 rcmChannel() const;
	void setRcmChannel(quint32 _rcmChannel);

	QString category()
	{
		return _category;
	}
	void setCategory(const QString& category)
	{
		_category = category;
	}

	bool dumpRawValues()
	{
		return _dumpRawValues;
	}
	void setDumpRawValues(bool state = false)
	{
		_dumpRawValues = state;
	}

	bool valid()
	{
		return _valid;
	}

	QString						_description;
	qreal						_resistorValue{MICRO_EPM_DEFAULT_RSENSE_MOHM};

private:	
	_EPMChannel(const _EPMChannel& copyMe);

	void setupChannel();
	void compileAMarkerChannel();
	void compileARCMChannel();
	void compileASPMChannel();

	bool						_active{true};
	KratosMapping				_kratosMapping;
	bool						_valid{false};
	quint32						_index{kNotSetInt};

	EPMIndexType				_indexType{eUnsetIndex};
	ChannelType					_channelType{eNotSet};
	WaveFormType				_waveFormType{eWaveFormUnset};

	QString						_seriesName;	
	HashType					_seriesHash{0};

	QString						_channelName;
	HashType					_channelHash{0};

	quint32						_channel{kNotSetInt};
	quint32						_rcmChannel{kNotSetInt};
	BasicColor					_channelColor{0, 0, 0};

	QString						_category;
	bool						_dumpRawValues{false};
};


#endif // EPMCHANNEL_H
