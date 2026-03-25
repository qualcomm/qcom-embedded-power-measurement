// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
				fileName.remove("_QEPM", Qt::CaseInsensitive);

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

