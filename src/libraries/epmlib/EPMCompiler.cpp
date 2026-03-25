// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMCompiler.h"

// QCommon
#include "EPMConfigFile.h"
#include "Range.h"

// QT
#include <QFile>
#include <QJsonObject>

const int kMarkerChannelStart(128);
const int kMaxMarkerIndex(5);

EPMCompiler::EPMCompiler
(
	EPMConfigFile* compileMe
) :
	_epmConfigFile(compileMe)
{
	_kratosMapping = compileMe->getKratosMappping();
}

bool EPMCompiler::compile
(
	const QString& targetPath
)
{
	bool result(false);

	QFile targetFile(targetPath);
	if (targetFile.open(QIODevice::WriteOnly) == true)
	{
		QTextStream targetStream(&targetFile);

		QJsonObject rootLevel;

		rootLevel["comment"] = "Compiled by QEPM EPM Configuration Editor";
		rootLevel["author"] = _epmConfigFile->author();
		rootLevel["dab"] = "spmv4";
		rootLevel["description"] = _epmConfigFile->description();
		rootLevel["label"] = _epmConfigFile->label();
		rootLevel["platform"] = _epmConfigFile->label();
		rootLevel["target"] = _epmConfigFile->target();
		rootLevel["version"] = 1;

		_kratosMapping->setSPMVersion(_epmConfigFile->spmVersion());

		QJsonArray channelsArray;

		for (auto row: range(_epmConfigFile->channelCount()))
		{
			EPMChannel epmChannel = _epmConfigFile->at(row);

			compileChannel(epmChannel, channelsArray);
		}

		rootLevel["channels"] = channelsArray;

		QJsonDocument doc(channelsArray);

		targetStream << doc.toJson();

		targetFile.close();

		result = true;
	}

	return result;
}

void EPMCompiler::compileChannel
(
	EPMChannel channel,
	QJsonArray& channelArray
)
{
	if (channel->indexType() == eMarkerIndex)
	{
		compileAMarkerChannel(channel, channelArray);
	}
	else if (channel->indexType() == eRCMIndex)
	{
		compileARCMChannel(channel, channelArray);
	}
	else if (channel->indexType() == eSPMIndex)
	{
		compileASPMChannel(channel, channelArray);
	}
	else
	{
	}
}

void EPMCompiler::compileAMarkerChannel
(
	EPMChannel channel,
	QJsonArray& channelArray
)
{
	QJsonObject channelObject;

	if (channel->index() <= kMaxMarkerIndex)
	{
		quint32 markerChannel = kMarkerChannelStart - 1 + channel->index();
		quint32 spmChannel = _kratosMapping->markerChannelToSPMChannel(markerChannel);
		quint32 rcmCfgChan;

		if (_kratosMapping->rcmMapping())
		{
			quint32 rcmChan = _kratosMapping->spmChannelToRcmChannel(spmChannel);
			if (_kratosMapping->kratosMapping())
				rcmCfgChan = _kratosMapping->rcmChannelToKratosChannel(rcmChan);
			else
				rcmCfgChan = rcmChan;
		}
		else
		{
			rcmCfgChan = spmChannel;
		}

		channelObject["category"] = channel->category();
		channelObject["channel"] = static_cast<int>(markerChannel);
		channelObject["chipsel"] = 0;
		channelObject["description"] = channel->_description;
		channelObject["disabled"] = channel->active() ? 0 : 1;
		channelObject["name"] = channel->channelName();
		channelObject["type"] = "G";
		channelObject["rcm_channel"] = static_cast<int>(rcmCfgChan);

		channelArray.append(channelObject);
	}
	else
	{
	}
}

void EPMCompiler::compileARCMChannel
(
	EPMChannel channel,
	QJsonArray& channelArray
)
{
	QJsonObject iChannelObject;
	QJsonObject vChannelObject;

	quint32 rcmChannel = channel->index();
	quint32 spmChannel = _kratosMapping->rcmChannelToSpmChannel(rcmChannel);
	quint32 rcmCfgChan;

	quint32 rcmChan = _kratosMapping->spmChannelToRcmChannel(spmChannel);
	if (_kratosMapping->kratosMapping())
		rcmCfgChan = _kratosMapping->rcmChannelToKratosChannel(rcmChan);
	else
		rcmCfgChan = rcmChan;

	iChannelObject["category"] = channel->category();
	iChannelObject["chipsel"] = 0;
	iChannelObject["description"] = channel->_description;
	iChannelObject["disabled"] = channel->active() ? 0 : 1;
	iChannelObject["gain"] = 1;
	iChannelObject["sense"] = static_cast<int>(spmChannel);
	iChannelObject["rcm_channel"] = static_cast<int>(rcmCfgChan);
	iChannelObject["resistor"] = static_cast<int>(channel->_resistorValue);
	iChannelObject["channel"] = static_cast<int>(_kratosMapping->spmChannelToCurrentChannel(spmChannel));
	iChannelObject["coupling"] = channel->channelName() + "_V";
	iChannelObject["name"] = channel->channelName() + "_I";
	iChannelObject["type"] = "I";

	channelArray.append(iChannelObject);

	vChannelObject["category"] = channel->category();
	vChannelObject["chipsel"] = 0;
	vChannelObject["description"] = channel->_description;
	vChannelObject["disabled"] = channel->active() ? 0 : 1;
	vChannelObject["gain"] = 1;
	vChannelObject["sense"] = static_cast<int>(spmChannel);
	vChannelObject["rcm_channel"] = static_cast<int>(rcmCfgChan);
	vChannelObject["resistor"] = 1000;
	vChannelObject["channel"] = static_cast<int>(_kratosMapping->spmChannelToVoltageChannel(spmChannel));
	vChannelObject["name"] = channel->channelName() + "_V";
	vChannelObject["type"] = "V";

	channelArray.append(vChannelObject);
}

void EPMCompiler::compileASPMChannel
(
	EPMChannel channel,
	QJsonArray& channelArray
)
{
	QJsonObject iChannelObject;
	QJsonObject vChannelObject;

	quint32 spmChannel = channel->index();
	quint32 rcmCfgChan;

	if (_kratosMapping->rcmMapping() == true)
	{
		quint32 rcmChan = _kratosMapping->spmChannelToRcmChannel(spmChannel);
		if (_kratosMapping->kratosMapping())
			rcmCfgChan = _kratosMapping->rcmChannelToKratosChannel(rcmChan);
		else
			rcmCfgChan = rcmChan;
	}
	else
	{
		rcmCfgChan = spmChannel;
	}

	iChannelObject["category"] = channel->category();
	iChannelObject["chipsel"] = 0;
	iChannelObject["description"] = channel->_description;
	iChannelObject["disabled"] = channel->active() ? 0 : 1;
	iChannelObject["gain"] = 1;
	iChannelObject["sense"] = static_cast<int>(spmChannel);
	iChannelObject["rcm_channel"] = static_cast<int>(rcmCfgChan);
	iChannelObject["resistor"] = static_cast<int>(channel->_resistorValue);
	iChannelObject["channel"] = static_cast<int>(_kratosMapping->spmChannelToCurrentChannel(spmChannel));
	iChannelObject["coupling"] = channel->channelName() + "_V";
	iChannelObject["name"] = channel->channelName() + "_I";
	iChannelObject["type"] = "I";

	channelArray.append(iChannelObject);

	vChannelObject["category"] = channel->category();
	vChannelObject["chipsel"] = 0;
	vChannelObject["description"] = channel->_description;
	vChannelObject["disabled"] = channel->active() ? 0 : 1;
	vChannelObject["gain"] = 1;
	vChannelObject["sense"] = static_cast<int>(spmChannel);
	vChannelObject["rcm_channel"] = static_cast<int>(rcmCfgChan);
	vChannelObject["resistor"] = 1000;
	vChannelObject["channel"] = static_cast<int>(_kratosMapping->spmChannelToVoltageChannel(spmChannel));
	vChannelObject["name"] = channel->channelName() + "_V";
	vChannelObject["type"] = "V";

	channelArray.append(vChannelObject);
}
