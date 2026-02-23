#ifndef PRNFILE_H
#define PRNFILE_H
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
// Copyright 2013-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
