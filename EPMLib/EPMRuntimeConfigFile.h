#ifndef EPMRUNTIMECONFIGFILE_H
#define EPMRUNTIMECONFIGFILE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"
#include "EPMChannel.h"
#include "EPMRuntimeChannel.h"

// Qt
#include <QByteArray>
#include <QJsonDocument>
#include <QList>
#include <QString>

const QByteArray kEPMRuntimeConfigPath(QByteArrayLiteral("lastEPMRuntimePath"));
const QByteArray kEPMRuntimeSettingsGroup(QByteArrayLiteral("EPM Runtime Config"));

class EPMLIB_EXPORT EPMRuntimeConfigFile
{
public:
	EPMRuntimeConfigFile();

	bool open(const QString& filePath);
	void close();
	QString errorText();

	bool isDirty();

	void save(const QString& filePath);

	QString platformFile()
	{
		return _platformFile;
	}

	void setPlatformFile(const QString& platformFile)
	{
		_platformFile = platformFile;
	}

	QList<HashType> getKeys();
	EPMRuntimeChannel get(HashType seriesHash);

	void resetChannels();
	void addChannel(const EPMChannel& addMe);

private:
	bool read();
	void readAChannel(QJsonObject& channelObject);

	void write();
	void writeAChannel(QJsonObject& channelObject, const EPMRuntimeChannel& channel);

	bool						_dirty{false};
	QString						_errorText;
	QJsonDocument				_document;

	QString						_creationDate;
	QString						_modifyDate;
	QString						_platformFile;
	EPMRuntimeChannels			_channels;
};

#endif // EPMRUNTIMECONFIGFILE_H
