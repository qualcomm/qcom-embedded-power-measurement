#ifndef EPMRUNTIMECONFIGFILE_H
#define EPMRUNTIMECONFIGFILE_H
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
// Copyright 2019-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
