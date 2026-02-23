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
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
