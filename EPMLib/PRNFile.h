#ifndef PRNFILE_H
#define PRNFILE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "EPMGlobalLib.h"

// QCommonConsole
#include "BasicColor.h"
#include "StringUtilities.h"

// QT
#include <QMap>
#include <QString>

typedef QMap<HashType, BasicColor> ChannelColorMap;
typedef QMap<HashType, QString> ChannelNameMap;
typedef QMap<HashType, QString> ChannelCategoryMap;

class EPMLIB_EXPORT PRNFile
{
public:
	PRNFile();

	bool read(const QString& prnPath);
	bool write(const QString& prnPath);

	QString target() const;
	void setTarget(const QString& target);

	QString author();
	void setAuthor(const QString& author);

	void addChannelColor(HashType channelHash, const BasicColor& channelColor);
	void setChannelColor(HashType channelHash, const BasicColor& channelColor);
	BasicColor channelColor(HashType channelHash) const;

	void addChannelName(HashType channelHash, const QString& channelName);
	QString channelName(HashType channelHash) const;

	void addChannelCategory(HashType channelHash, const QString& channelCategory);
	QString channelCategory(HashType channelHash) const;

	QString lastError();

private:
	QString						_author;
	QString						_description;
	QString						_target;
	QString						_date;
	QString						_time;
	ChannelColorMap				_channelColorMap;
	ChannelNameMap				_channelNameMap;
	ChannelCategoryMap			_channelCategoryMap;
	QString						_lastError;
};

#endif // PRNFILE_H
