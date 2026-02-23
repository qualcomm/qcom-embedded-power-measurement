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
// Copyright 2018-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMConfigFile.h"
#include "PRNFile.h"

// QCommon
#include "AppCore.h"
#include "AlpacaDefines.h"
#include "EPMConfigFile.h"
#include "QualcommColors.h"
#include "QualcommChartColor.h"
#include "Range.h"

// Qt
#include <QDateTime>
#include <QDir>
#include <QFile>
//#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

const QString kEPMVersion(QStringLiteral("epm_version"));
const QString kAuthor(QStringLiteral("author"));
const QString kCreationDate(QStringLiteral("creation_date"));
const QString kModifyDate(QStringLiteral("modify_date"));
const QString kTarget(QStringLiteral("target"));
const QString kLabel(QStringLiteral("label"));
const QString kDescription(QStringLiteral("description"));
const QString kSPMVersion(QStringLiteral("spm_version"));
const QString kColor(QStringLiteral("color"));

const QString kChannels(QStringLiteral("channels"));
const QString kDisabled(QStringLiteral("disabled"));
const QString kType(QStringLiteral("type"));
const QString kIndex(QStringLiteral("index"));
const QString kName(QStringLiteral("name"));
const QString kCategory(QStringLiteral("category"));
const QString kResistorValue(QStringLiteral("resistor_value"));

EPMConfigFile::EPMConfigFile()
{
	_creationDate = _modifyDate = QDateTime::currentDateTime().toString();
}

quint32 EPMConfigFile::buildChannelsFromConfiguration
(
	EPMChannels& runtimeChannels
)
{
	runtimeChannels.clear();

	for (const auto& channel: std::as_const(_channels))
	{
		EPMChannel epmChannel;

		switch (channel->indexType())
		{
		case eUnsetIndex:
			Q_ASSERT("channel->indexType() == eUnsetIndex");
			break;

		case eMarkerIndex:
			epmChannel = _EPMChannel::copyToNewEPMChannel(channel);
			epmChannel->setChannelType(_EPMChannel::eVoltage);
			epmChannel->setSeriesName(channel->channelName() + "_V");
			runtimeChannels.push_back(epmChannel);
			break;

		case eRCMIndex:
			epmChannel = _EPMChannel::copyToNewEPMChannel(channel);
			epmChannel->setChannelType(_EPMChannel::eCurrent);
			epmChannel->setSeriesName(channel->channelName() + "_I");
			runtimeChannels.push_back(epmChannel);

			epmChannel = _EPMChannel::copyToNewEPMChannel(channel);
			epmChannel->setChannelType(_EPMChannel::eVoltage);
			epmChannel->setSeriesName(channel->channelName() + "_V");
			runtimeChannels.push_back(epmChannel);
			break;

		case eSPMIndex:
			epmChannel = _EPMChannel::copyToNewEPMChannel(channel);
			epmChannel->setChannelType(_EPMChannel::eCurrent);
			epmChannel->setSeriesName(channel->channelName() + "_I");
			runtimeChannels.push_back(epmChannel);

			epmChannel = _EPMChannel::copyToNewEPMChannel(channel);
			epmChannel->setChannelType(_EPMChannel::eVoltage);
			epmChannel->setSeriesName(channel->channelName() + "_V");
			runtimeChannels.push_back(epmChannel);
			break;
		}
	}

	return runtimeChannels.count();
}

void EPMConfigFile::buildPRNFile
(
	const QString& targetPath,
	const QString& targetFileName
)
{
	PRNFile prnFile;

	prnFile.setTarget(target());
	prnFile.setAuthor(recordingAuthor());

	QList<HashType> colorHashes;
	getColorHashes(colorHashes);
	for (const auto hash: std::as_const(colorHashes))
	{
		BasicColor channelColor = getColor(hash);
		if (channelColor.isValid())
		{
			prnFile.addChannelColor(hash, channelColor);
		}

		EPMChannel voltageChannel = getVoltageChannelByChannelHash(hash);
		if (voltageChannel.isNull() == false)
		{
			prnFile.addChannelName(hash, voltageChannel->channelName());
			prnFile.addChannelCategory(hash, voltageChannel->category());
		}
	}

	if (prnFile.write(QDir::cleanPath(targetPath + QDir::separator() +  targetFileName + ".prn")) == false)
		AppCore::writeToApplicationLog(QString("PRN Write Error: ") + prnFile.lastError());
}

bool EPMConfigFile::open
(
	const QString& filePath,
	KratosMapping kratosMapping
)
{
	bool result(false);

	Q_ASSERT(kratosMapping.isNull() == false);

	_kratosMapping = kratosMapping;

	QFile jsonFile(filePath);

	if (jsonFile.open(QIODevice::ReadOnly))
	{
		QByteArray jsonFileContents = jsonFile.readAll();
		if (jsonFileContents.isEmpty() == false)
		{
			QJsonParseError parserError;

			_document = QJsonDocument::fromJson(jsonFileContents, &parserError);
			if (parserError.error == QJsonParseError::NoError)
			{
				if (read())
				{
					result = true;
				}
			}
			else
			{
				_errorText = QObject::tr("Error parsing configuration file\n") +
						parserError.errorString();
			}
		}
	}
	else
	{
		_errorText = QObject::tr("Unable to open configuration file ") + filePath;
	}

	return result;
}

void EPMConfigFile::close()
{

}

QString EPMConfigFile::errorText()
{
	QString result(_errorText);

	_errorText.clear();

	return result;
}

bool EPMConfigFile::isDirty()
{
	return _dirty;
}

void EPMConfigFile::save
(

	const QString& filePath
)
{
	_modifyDate = QDateTime::currentDateTime().toString();
	write();

	if (_document.isNull() == false)
	{
		QByteArray jsonDocument = _document.toJson(QJsonDocument::Indented);
		if (jsonDocument.isEmpty() == false)
		{
			QFile jsonFile(filePath);

			if (jsonFile.open(QIODevice::WriteOnly) == true)
			{
				jsonFile.write(jsonDocument);
				jsonFile.close();

				_dirty = false;
			}
		}
	}
}

KratosMapping EPMConfigFile::getKratosMappping()
{
	return _kratosMapping;
}

QString EPMConfigFile::author()
{
	return _author;
}

void EPMConfigFile::setAuthor
(
	const QString& author
)
{
	if (_author != author)
	{
		_author = author;
		_dirty = true;
	}
}

QString EPMConfigFile::recordingAuthor()
{
	return _recordingAuthor;
}

void EPMConfigFile::setRecordingAuthor(const QString &author)
{
	_recordingAuthor = author;
}

QString EPMConfigFile::target()
{
	return _target;
}

void EPMConfigFile::setTarget
(
	const QString& target
)
{
	if (_target != target)
	{
		_target = target;
		_dirty = true;
	}
}

QString EPMConfigFile::label()
{
	return _label;
}

void EPMConfigFile::setLabel
(
	const QString& label
)
{
	if (_label != label)
	{
		_label = label;
		_dirty = true;
	}
}

QString EPMConfigFile::description()
{
	return _description;
}

void EPMConfigFile::setDescription
(
	const QString& description
)
{
	if (_description != description)
	{
		_description = description;
		_dirty = true;
	}
}

SPMVersion EPMConfigFile::spmVersion()
{
	return _spmVersion;
}

void EPMConfigFile::setSpmVersion
(
	SPMVersion spmVersion
)
{
	if (_spmVersion != spmVersion)
	{
		_spmVersion = spmVersion;
		_dirty = true;
	}
}

QString EPMConfigFile::spmVersionAsText()
{
	QString result("SPM Version: ");
	switch (_spmVersion)
	{
	case eLegacyVersion: result += "Legacy Alpaca"; break;
	case eAlpacaVersion: result += "Alpaca"; break;
	case eSPMVersion: result += "SPM 4.1"; break;
	case eSPM_MTPVersion: result += "SPM + MTP"; break;
	case eSPM_CDPVersion: result += "SPM + CDP"; break;
	}

	return result;
}

quint32 EPMConfigFile::channelCount()
{
	return static_cast<quint32>(_channels.count());
}

int EPMConfigFile::getColorHashes
(
	QList<HashType>& hashes
)
{
	hashes.clear();

	for (auto [key, color]: keyValueRange(_seriesColorMap))
	{
		hashes.push_back(key);
	}

	return hashes.count();
}

BasicColor EPMConfigFile::getColor
(
	HashType hash
)
{
	BasicColor result;

	if (_seriesColorMap.find(hash) != _seriesColorMap.end())
		result = _seriesColorMap[hash];

	return result;
}

BasicColor EPMConfigFile::nextColor()
{
	return qualcommChartColor(_colorIndex++);
}

EPMChannel EPMConfigFile::at
(
	quint32 channelIndex
)
{
	EPMChannel result;

	if (channelIndex < static_cast<quint32>(_channels.count()))
		result = _channels.at(static_cast<int>(channelIndex));

	return result;
}

void EPMConfigFile::addChannel
(
	const EPMChannel& addMe
)
{
	addMe->setChannelColor(nextColor());

	_channels.push_back(addMe);
	_dirty = true;
}

void EPMConfigFile::updateChannel
(
	quint32 channelIndex,
	const EPMChannel& epmChannel
)
{
	if (channelIndex < channelCount())
	{
		_channels.replace(static_cast<int>(channelIndex), epmChannel);
		_dirty = true;
	}
}

void EPMConfigFile::removeChannel
(
	quint32 channelIndex
)
{
	if (channelIndex < channelCount())
	{
		_channels.erase(_channels.begin() + channelIndex);
		_dirty = true;
	}
}

void EPMConfigFile::sort
(
	_EPMChannel::FieldPosition column
)
{
	auto sortLambda = [column] (const EPMChannel& ep, const EPMChannel& ep2) -> bool
	{
		switch (column)
		{
		case _EPMChannel::eEnabled:
			return ep->active() < ep2->active();

		case _EPMChannel::eType:
			return ep->indexType() < ep2->indexType();

		case _EPMChannel::eIndex:
			return ep->index() < ep2->index();

		case _EPMChannel::eName:
			return ep->seriesName() < ep2->seriesName();

		case _EPMChannel::eGroup:
			return ep->eGroup < ep2->eGroup;

		case _EPMChannel::eDescription:
			return ep->_description < ep2->_description;

		case _EPMChannel::eResistorValue:
			return ep->_resistorValue < ep2->_resistorValue;

		case _EPMChannel::eLast:
		break;
		}

		return false;
	};

	std::stable_sort(_channels.begin(), _channels.end(), sortLambda);
}

QStringList EPMConfigFile::getCategories()
{
	return _categories;
}

EPMChannel EPMConfigFile::getCurrentChannelByChannelHash
(
	HashType hash
)
{
	EPMChannel result;

	for (auto& epmChannel: _currentChannels)
	{
		if (epmChannel->channelHash() == hash)
		{
			result = epmChannel;
			break;
		}
	}

	return result;
}

EPMChannel EPMConfigFile::getCurrentChannelBySeriesHash
(
	HashType hash
)
{
	EPMChannel result;

	for (auto& epmChannel: _currentChannels)
	{
		if (epmChannel->seriesHash() == hash)
		{
			result = epmChannel;
			break;
		}
	}

	return result;
}

EPMChannel EPMConfigFile::getVoltageChannelByChannelHash
(
	HashType hash
)
{
	EPMChannel result;

	for (auto& epmChannel: _voltageChannels)
	{
		if (epmChannel->channelHash() == hash)
		{
			result = epmChannel;
			break;
		}
	}

	return result;
}

EPMChannel EPMConfigFile::getVoltageChannelBySeriesHash
(
	HashType hash
)
{
	EPMChannel result;

	for (auto& epmChannel: _voltageChannels)
	{
		if (epmChannel->seriesHash() == hash)
		{
			if (epmChannel->channelType() == _EPMChannel::eVoltage)
			{
				result = epmChannel;
				break;
			}
		}
	}

	return result;
}

EPMChannel EPMConfigFile::getMarkerChannelByChannelHash
(
	HashType hash
)
{
	EPMChannel result;

	for (auto& epmChannel: _voltageChannels)
	{
		if (epmChannel->channelHash() == hash)
		{
			if (epmChannel->channelType() == _EPMChannel::eMarker)
			{
				result = epmChannel;
				break;
			}
		}
	}

	return result;
}

EPMChannel EPMConfigFile::getMarkerChannelBySeriesHash
(
	HashType hash
)
{
	EPMChannel result;

	for (auto& epmChannel: _voltageChannels)
	{
		if (epmChannel->seriesHash() == hash)
		{
			result = epmChannel;
			break;
		}
	}

	return result;
}

bool EPMConfigFile::read()
{
	bool result(false);

	_categories.clear();
	_currentChannels.clear();
	_voltageChannels.clear();

	if (_document.isNull() == false)
	{
		result = true;

		QJsonObject rootLevel = _document.object();
		QJsonValue jsonValue;

		jsonValue = rootLevel.value(kAuthor);
		if (jsonValue.isNull() == false)
			_author = jsonValue.toString();

		jsonValue = rootLevel.value(kCreationDate);
		if (jsonValue.isNull() == false)
			_creationDate = jsonValue.toString();

		jsonValue = rootLevel.value(kModifyDate);
		if (jsonValue.isNull() == false)
			_modifyDate = jsonValue.toString();

		jsonValue = rootLevel.value(kDescription);
		if (jsonValue.isNull() == false)
			_description = jsonValue.toString();

		jsonValue = rootLevel.value(kTarget);
		if (jsonValue.isNull() == false)
			_target = jsonValue.toString();

		jsonValue = rootLevel.value(kSPMVersion);
		if (jsonValue.isNull() == false)
			_spmVersion = static_cast<SPMVersion>(jsonValue.toInt());
		else
			_spmVersion = eAlpacaVersion;

		_kratosMapping->setSPMVersion(_spmVersion);

		jsonValue = rootLevel.value(kLabel);
		if (jsonValue.isNull() == false)
			_label = jsonValue.toString();

		if (rootLevel.contains(kChannels) && rootLevel[kChannels].isArray())
		{
			QJsonArray channelArray = rootLevel[kChannels].toArray();

			_channels.clear();

			for (auto levelIndex: range(channelArray.size()))
			{
				QJsonObject channelObject = channelArray[levelIndex].toObject();

				readAChannel(channelObject);
			}

			_categories.sort(Qt::CaseInsensitive);
		}
	}

	return result;
}

void EPMConfigFile::readAChannel
(
	QJsonObject& channelObject
)
{
	EPMChannel epmChannel = EPMChannel(new _EPMChannel);
	QString reservedTestString;

	epmChannel->setKratosMapping(_kratosMapping);

	if (channelObject.contains(kDisabled))
		epmChannel->setActive(channelObject[kDisabled].toString() == "true" ? false : true);

	if (channelObject.contains(kType) && channelObject[kType].isString())
		epmChannel->setIndexType(StringToEPMIndexType(channelObject[kType].toString()));

	if (channelObject.contains(kIndex))
		epmChannel->setIndex(static_cast<quint32>(channelObject[kIndex].toInt()));

	if (channelObject.contains(kName) && channelObject[kName].isString())
	{
		reservedTestString = channelObject[kName].toString().toUpper();

		epmChannel->setChannelName(channelObject[kName].toString());
	}

	if (channelObject.contains(kCategory) && channelObject[kCategory].isString())
	{
		QString category = channelObject[kCategory].toString();
		epmChannel->setCategory(category);

		if (_categories.contains(category) == false)
			_categories.append(category);
	}
	else if (channelObject.contains("group") && channelObject["group"].isString())
	{
		QString category = channelObject["group"].toString();
		epmChannel->setCategory(category);

		if (_categories.contains(category) == false)
			_categories.append(category);
	}

	if (channelObject.contains(kDescription) && channelObject[kDescription].isString())
		epmChannel->_description = channelObject[kDescription].toString();

	if (channelObject.contains(kResistorValue))
	{
		QJsonValue resistorVal = channelObject[kResistorValue];

		qreal tempDouble = resistorVal.toDouble();
		quint64 tempCeil = ceil(tempDouble);
		quint64 tempFloor = floor(tempDouble);

		if (tempCeil != tempFloor)
			epmChannel->_resistorValue = resistorVal.toDouble();
		else
		{
			int temp = resistorVal.toInt();
			if (temp < 0)
				epmChannel->_resistorValue = MICRO_EPM_DEFAULT_RSENSE_MOHM;
			else if (temp == 0)
				epmChannel->_resistorValue = MICRO_EPM_DEFAULT_RSENSE_MOHM;
			else
				epmChannel->_resistorValue = resistorVal.toDouble();
		}
	}

	BasicColor channelColor;

	if (channelObject.contains(kColor) && channelObject[kColor].isString())
	{
		channelColor.setNamedColor(channelObject[kColor].toString());
	}
	else
	{
		// We are adding a new color to an existing document, mark it dirty
		_dirty = true;

		if (reservedTestString.isEmpty() == false)
		{
			populateReservedColors();

			if (_reservedColors.contains(reservedTestString) == true)
			{
				channelColor = _reservedColors[reservedTestString];
			}
			else
			{
				channelColor = qualcommChartColor(_colorIndex++);
			}
		}
		else
		{
			channelColor = qualcommChartColor(_colorIndex++);
		}
	}

	epmChannel->setChannelColor(channelColor);

	_seriesColorMap[epmChannel->channelHash()] = channelColor;

	_channels.push_back(epmChannel);

	switch (epmChannel->indexType())
	{
	case eUnsetIndex:
		break;

	case eMarkerIndex:
		epmChannel = _EPMChannel::copyToNewEPMChannel(epmChannel);
		epmChannel->setChannelType(_EPMChannel::eVoltage);
		epmChannel->setSeriesName(epmChannel->channelName() + "_V");
		_voltageChannels.push_back(epmChannel);
		break;

	case eRCMIndex:
		epmChannel = _EPMChannel::copyToNewEPMChannel(epmChannel);
		epmChannel->setChannelType(_EPMChannel::eCurrent);
		epmChannel->setSeriesName(epmChannel->channelName() + "_I");
		_currentChannels.push_back(epmChannel);

		epmChannel = _EPMChannel::copyToNewEPMChannel(epmChannel);
		epmChannel->setChannelType(_EPMChannel::eVoltage);
		epmChannel->setSeriesName(epmChannel->channelName() + "_V");
		_voltageChannels.push_back(epmChannel);
		break;

	case eSPMIndex:
		epmChannel = _EPMChannel::copyToNewEPMChannel(epmChannel);
		epmChannel->setChannelType(_EPMChannel::eCurrent);
		epmChannel->setSeriesName(epmChannel->channelName() + "_I");
		_currentChannels.push_back(epmChannel);

		epmChannel = _EPMChannel::copyToNewEPMChannel(epmChannel);
		epmChannel->setChannelType(_EPMChannel::eVoltage);
		epmChannel->setSeriesName(epmChannel->channelName() + "_V");
		_voltageChannels.push_back(epmChannel);
		break;
	}
}

void EPMConfigFile::write()
{
	QJsonObject rootLevel;

	rootLevel[kEPMVersion] = kProductName + " " + kProductVersion;

	rootLevel[kAuthor] = _author;
	rootLevel[kCreationDate] = _creationDate;
	rootLevel[kModifyDate] = _modifyDate;
	rootLevel[kTarget] = _target;
	rootLevel[kLabel] = _label;
	rootLevel[kDescription] = _description;
	rootLevel[kSPMVersion] = _spmVersion;

	QJsonArray channelsArray;
	for (const auto& channel: std::as_const(_channels))
	{
		QJsonObject channelObject;
		writeAChannel(channelObject, channel);
		channelsArray.append(channelObject);
	}
	rootLevel[kChannels] = channelsArray;

	_document = QJsonDocument(rootLevel);
}

void EPMConfigFile::writeAChannel
(
	QJsonObject& channelObject,
	const EPMChannel& channel
)
{
	channelObject[kDisabled] = channel->active() ? "false" : "true";
	channelObject[kType] = EPMIndexTypeToString(channel->indexType());
	channelObject[kIndex] = static_cast<int>(channel->index());
	channelObject[kName] = channel->channelName();
	channelObject[kCategory] = channel->category();
	channelObject[kDescription] = channel->_description;
	channelObject[kResistorValue] = channel->_resistorValue;
	channelObject[kColor] = channel->channelColor().name();
}

void EPMConfigFile::populateReservedColors()
{
	if (_reservedColors.isEmpty())
	{
		_reservedColors["BATTERY"] = kGreen;
		_reservedColors["VBATT"] = _reservedColors["BATTERY"].lighter(110);
		_reservedColors["VBATT_CONN"] = _reservedColors["VBATT"].lighter(120);
		_reservedColors["BATTERY_LCM"] = _reservedColors["BATTERY"].lighter(120);
	}
}
