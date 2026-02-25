// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "BugWriterPreferences.h"

// QCommon
#include "AlpacaSettings.h"
#include "ConsoleApplicationEnhancements.h"

const QString kLogPath(QStringLiteral("BugWriterLogPath"));
const QString kUserPhoneMap(QStringLiteral("UserPhoneMap"));
const QString kLastAuthor(QStringLiteral("kLastAuthor"));

void BugWriterPreferences::setAppName
	(
		const QByteArray& appName,
		const QByteArray& appVersion
	)
{
	PreferencesBase::setAppName(appName, appVersion);
	PreferencesBase::setLoggingActive(true);

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);

	_logLocation = settings.value(kLogPath, defaultLogPath()).toString();
	_lastAuthor = settings.value(kLastAuthor, "").toString();
	_userNamePhoneMap = settings.value(kUserPhoneMap, QVariant()).toMap();

	settings.endGroup();
}

void BugWriterPreferences::setUserNamePhone(const QString &userName, const QString &phoneNumString)
{
	_userNamePhoneMap.insert(userName, phoneNumString);
	setLastAuthor(userName);

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	settings.setValue(kUserPhoneMap, _userNamePhoneMap);

	settings.endGroup();
}

QString BugWriterPreferences::getUserNamePhone(const QString &userName)
{
	QString result;

	auto it = _userNamePhoneMap.find(userName);
	if (it != _userNamePhoneMap.end())
		result = it.value().toString();
	else
		result = "";

	return result;
}

void BugWriterPreferences::setLastAuthor(const QString& author)
{
	_lastAuthor = author;

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	settings.setValue(kLastAuthor, _lastAuthor);

	settings.endGroup();
}

QString BugWriterPreferences::lastAuthor()
{
	return _lastAuthor;
}

QString BugWriterPreferences::defaultLogPath()
{
	return documentsDataPath("BugWriter");
}
