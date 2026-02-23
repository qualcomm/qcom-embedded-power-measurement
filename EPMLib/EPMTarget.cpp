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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMTarget.h"

// QT
#include <QDir>

QString EPMTarget::platformPath()
{
	return _path;
}

bool EPMTarget::setPlatformPath
(
	const QString& platformPath
)
{
	bool result{false};

	if (QDir(platformPath).exists())
	{
		_path = platformPath;

		result = true;

		_platformEntryList.clear();

		QDir enumerateMe(platformPath);

		QFileInfoList entryInfoList = enumerateMe.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::Files, QDir::Name);
		auto entryInfo = entryInfoList.begin();
		while (entryInfo != entryInfoList.end())
		{
			if (entryInfo->suffix().compare("ccnf", Qt::CaseInsensitive) == 0)
			{
				QString fileName(entryInfo->fileName());

				fileName.remove(".ccnf", Qt::CaseInsensitive);
				fileName.remove("_config", Qt::CaseInsensitive);
				fileName.remove("_alpaca", Qt::CaseInsensitive);

				EPMTargetEntry epmTargetEntry;

				epmTargetEntry._name = fileName;
				epmTargetEntry._path = entryInfo->absoluteFilePath();

				_platformEntryList.push_back(epmTargetEntry);
			}

			entryInfo++;
		}
	}

	return result;
}

quint32 EPMTarget::getPlatformCount()
{
	return _platformEntryList.count();
}

bool EPMTarget::getPlatform
(
	quint32 platformIndex,
	QString& name,
	QString& path
)
{
	bool result{false};

	if (platformIndex < static_cast<quint32>(_platformEntryList.count()))
	{
		auto platformEntry = _platformEntryList.at(platformIndex);
		name = platformEntry._name;
		path = platformEntry._path;

		result = true;
	}

	return result;
}

