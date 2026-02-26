// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PRNFile.h"

// QCommon
#include "Range.h"

// QT
#include <QDate>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

const QString kAuthor(QStringLiteral("author"));
const QString kDescription(QStringLiteral("description"));
const QString kTarget(QStringLiteral("target"));
const QString kDate(QStringLiteral("date"));
const QString kTime(QStringLiteral("time"));
const QString kColors(QStringLiteral("colors"));
const QString kHash(QStringLiteral("hash"));
const QString kColor(QStringLiteral("color"));
const QString kChannelName(QStringLiteral("channel_name"));
const QString kChannelCategory(QStringLiteral("channel_category"));

PRNFile::PRNFile()
{

}

bool PRNFile::read
(
	const QString &prnPath
)
{
	bool result(false);

	QJsonDocument document;
	QFile jsonFile(prnPath);

	if (jsonFile.open(QIODevice::ReadOnly))
	{
		QByteArray jsonFileContents = jsonFile.readAll();
		if (jsonFileContents.isEmpty() == false)
		{
			QJsonParseError parserError;

			document = QJsonDocument::fromJson(jsonFileContents, &parserError);
			if (parserError.error == QJsonParseError::NoError)
			{
				if (document.isNull() == false)
				{
					result = true;

					QJsonObject rootLevel = document.object();
					QJsonValue jsonValue;

					jsonValue = rootLevel.value(kAuthor);
					if (jsonValue.isNull() == false)
						_author = jsonValue.toString();

					jsonValue = rootLevel.value(kDescription);
					if (jsonValue.isNull() == false)
						_description = jsonValue.toString();

					jsonValue = rootLevel.value(kTarget);
					if (jsonValue.isNull() == false)
						_target = jsonValue.toString();

					jsonValue = rootLevel.value(kDate);
					if (jsonValue.isNull() == false)
						_date = jsonValue.toString();

					jsonValue = rootLevel.value(kTime);
					if (jsonValue.isNull() == false)
						_time = jsonValue.toString();

					const QString kColors(QStringLiteral("colors"));
					const QString kHash(QStringLiteral("hash"));
					const QString kColor(QStringLiteral("color"));

					if (rootLevel.contains(kColors) && rootLevel[kColors].isArray())
					{
						QJsonArray colorsArray = rootLevel[kColors].toArray();

						_channelColorMap.clear();

						for (auto levelIndex: range(colorsArray.size()))
						{
							QJsonObject colorObject = colorsArray[levelIndex].toObject();

							if (colorObject.contains(kHash))
							{
								HashType hash = static_cast<HashType>(colorObject[kHash].toString().toULongLong());

								if (colorObject.contains(kColor) && colorObject[kColor].isString())
								{
									BasicColor colorEntry;
									QString colorString = colorObject[kColor].toString();
									colorEntry.setNamedColor(colorString);
									_channelColorMap[hash] = colorEntry;
								}

								if (colorObject.contains(kChannelName) && colorObject[kChannelName].isString())
								{
									_channelNameMap[hash] = colorObject[kChannelName].toString();
								}

								if (colorObject.contains(kChannelCategory) && colorObject[kChannelCategory].isString())
								{
									_channelCategoryMap[hash] = colorObject[kChannelCategory].toString();
								}
							}
						}
					}
				}
				else
				{
					_lastError = QObject::tr("JSON Document is NULL") ;
				}
			}
			else
			{
				_lastError = QObject::tr("Error parsing configuration file") + parserError.errorString();
			}
		}
	}
	else
	{
		_lastError = QObject::tr("Unable to open configuration file ") + prnPath;
	}

	return result;
}

bool PRNFile::write
(
	const QString &prnPath
)
{
	bool result{false};

	QFile targetFile(prnPath);
	if (targetFile.open(QIODevice::WriteOnly) == true)
	{
		QJsonObject rootLevel;

		rootLevel[kAuthor] = "Michael Simpson msimpson@qti.qualcomm.com";
		rootLevel[kDescription] = "Power Target Runtime File";
		rootLevel[kTarget] = _target;
		rootLevel[kDate] = QDate::currentDate().toString();
		rootLevel[kTime] = QTime::currentTime().toString();

		QJsonArray channelArray;

		for (auto [key, value]: keyValueRange(_channelColorMap))
		{
			QJsonObject channelObject;

			channelObject[kHash] = QString::number(static_cast<HashType>(key));
			channelObject[kColor] = value.name();
			channelObject[kChannelName] = _channelNameMap[key];
			channelObject[kChannelCategory] = _channelCategoryMap[key];

			channelArray.append(channelObject);
		}

		rootLevel[kColors] = channelArray;

		QJsonDocument compiledDocument(rootLevel);
		result = (targetFile.write(compiledDocument.toJson(QJsonDocument::Indented)) > 0);
		targetFile.close();
	}
	else
	{
		_lastError = QString("Can't open prn file %1").arg(prnPath);
	}

	return result;
}

QString PRNFile::target() const
{
	return _target;
}

void PRNFile::setTarget
(
	const QString& target
)
{
	_target = target;
}

QString PRNFile::author()
{
	return _author;
}

void PRNFile::setAuthor(const QString &author)
{
	_author = author;
}

void PRNFile::addChannelColor
(
	HashType channelHash,
	const BasicColor &channelColor
)
{
	_channelColorMap[channelHash] = channelColor;
}

void PRNFile::setChannelColor
(
	HashType channelHash,
	const BasicColor& channelColor
)
{
	_channelColorMap[channelHash] = channelColor;
}

BasicColor PRNFile::channelColor
(
	HashType channelHash
) const
{
	BasicColor result;

	if (_channelColorMap.find(channelHash) != _channelColorMap.end())
		result = _channelColorMap[channelHash];

	return result;
}

void PRNFile::addChannelName
(
	HashType channelHash,
	const QString &channelName
)
{
	_channelNameMap[channelHash] = channelName;
}

QString PRNFile::channelName
(
	HashType channelHash
) const
{
	QString result;

	if (_channelNameMap.find(channelHash) != _channelNameMap.end())
		result = _channelNameMap[channelHash];

	return result;
}

void PRNFile::addChannelCategory
(
	HashType channelHash,
	const QString& channelCategory
)
{
	_channelCategoryMap[channelHash] = channelCategory;
}

QString PRNFile::channelCategory
(
	HashType channelHash
) const
{

	QString result;

	if (_channelCategoryMap.find(channelHash) != _channelCategoryMap.end())
		result = _channelCategoryMap[channelHash];

	return result;
}

QString PRNFile::lastError()
{
	QString temp(_lastError);

	_lastError.clear();

	return temp;
}
