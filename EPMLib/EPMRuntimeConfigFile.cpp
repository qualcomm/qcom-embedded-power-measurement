// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMRuntimeConfigFile.h"

// QCommon
#include "Range.h"

// Qt
#include <QDateTime>
#include <QFile>
//#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

const QString kCreationDate(QStringLiteral("creation_date"));
const QString kModifyDate(QStringLiteral("modify_date"));
const QString kPlatformkFile(QStringLiteral("platformFile"));

const QString kChannels(QStringLiteral("channels"));
const QString kHash(QStringLiteral("hash"));
const QString kActive(QStringLiteral("active"));
const QString kSeriesName(QStringLiteral("series_name"));

EPMRuntimeConfigFile::EPMRuntimeConfigFile() :
	_dirty(false)
{
	_creationDate = _modifyDate = QDateTime::currentDateTime().toString();
}

bool EPMRuntimeConfigFile::open
(
	const QString& filePath
)
{
	bool result(false);

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

void EPMRuntimeConfigFile::close()
{

}

QString EPMRuntimeConfigFile::errorText()
{
	QString result(_errorText);

	_errorText.clear();

	return result;
}

bool EPMRuntimeConfigFile::isDirty()
{
	return _dirty;
}

void EPMRuntimeConfigFile::save
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

void EPMRuntimeConfigFile::resetChannels()
{
	_channels.clear();
}

QList<HashType> EPMRuntimeConfigFile::getKeys()
{
	return _channels.keys();
}

EPMRuntimeChannel EPMRuntimeConfigFile::get
(
	HashType seriesHash
)
{
	EPMRuntimeChannel result;

	if (_channels.find(seriesHash) != _channels.end())
		result = _channels[seriesHash];

	return result;
}

void EPMRuntimeConfigFile::addChannel
(
	const EPMChannel& addMe
)
{
	EPMRuntimeChannel runtimeChannel;

	runtimeChannel._seriesHash = addMe->seriesHash();
	runtimeChannel._active = addMe->active();
	runtimeChannel._seriesName = addMe->seriesName();

	_channels[runtimeChannel._seriesHash] = runtimeChannel;
	_dirty = true;
}

bool EPMRuntimeConfigFile::read()
{
	bool result(false);

	if (_document.isNull() == false)
	{
		result = true;

		QJsonObject rootLevel = _document.object();
		QJsonValue jsonValue;

		jsonValue = rootLevel.value(kCreationDate);
		if (jsonValue.isNull() == false)
			_creationDate = jsonValue.toString();

		jsonValue = rootLevel.value(kModifyDate);
		if (jsonValue.isNull() == false)
			_modifyDate = jsonValue.toString();

		jsonValue = rootLevel.value(kPlatformkFile);
		if (jsonValue.isNull() == false)
			_platformFile = jsonValue.toString();

		if (rootLevel.contains(kChannels) && rootLevel[kChannels].isArray())
		{
			QJsonArray channelArray = rootLevel[kChannels].toArray();

			_channels.clear();

			for (auto levelIndex: range(channelArray.size()))
			{
				QJsonObject channelObject = channelArray[levelIndex].toObject();

				readAChannel(channelObject);
			}
		}
	}

	return result;
}

void EPMRuntimeConfigFile::readAChannel
(
	QJsonObject& channelObject
)
{
	HashType seriesHash{0};
	bool active{false};
	QString seriesName;

	if (channelObject.contains(kHash) && channelObject[kHash].isString())
		seriesHash = channelObject[kHash].toString().toULongLong();

	if (seriesHash != 0)
	{
		if (channelObject.contains(kActive) == true)
			active = channelObject[kActive].toBool();

		if (channelObject.contains(kSeriesName) && channelObject[kSeriesName].isString())
			seriesName = channelObject[kSeriesName].toString();

		EPMRuntimeChannel epmRuntimeChannel;

		epmRuntimeChannel._seriesHash = seriesHash;
		epmRuntimeChannel._active = active;
		epmRuntimeChannel._seriesName = seriesName;

		_channels[seriesHash] = epmRuntimeChannel;
	}
}

void EPMRuntimeConfigFile::write()
{
	QJsonObject rootLevel;

	rootLevel[kCreationDate] = _creationDate;
	rootLevel[kModifyDate] = _modifyDate;
	rootLevel[kPlatformkFile] = _platformFile;
	QJsonArray channelsArray;
	for (const auto& channel: _channels)
	{
		QJsonObject channelObject;
		writeAChannel(channelObject, channel);
		channelsArray.append(channelObject);
	}
	rootLevel[kChannels] = channelsArray;

	_document = QJsonDocument(rootLevel);
}

void EPMRuntimeConfigFile::writeAChannel
(
	QJsonObject& channelObject,
	const EPMRuntimeChannel& channel
)
{
	channelObject[kHash] = QString::number(channel._seriesHash);
	channelObject[kActive] = channel._active;
	channelObject[kSeriesName] = channel._seriesName;
}

