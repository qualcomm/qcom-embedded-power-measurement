#ifndef BUGWRITERPREFERENCES_H
#define BUGWRITERPREFERENCES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include <PreferencesBase.h>

class BugWriterPreferences : public PreferencesBase
{
public:
	BugWriterPreferences() = default;

	virtual void setAppName(const QByteArray& appName, const QByteArray& appVersion);

	void setUserNamePhone(const QString& userName, const QString& phoneNumString);
	QString getUserNamePhone(const QString& userName);

	void setLastAuthor(const QString &author);
	QString lastAuthor();

	QString defaultLogPath();
	void setLogPath(const QString& bugWriterLogPath);

private:
	QString                     _logLocation;
	QString                     _lastAuthor;
	QMap<QString, QVariant>     _userNamePhoneMap;
};

#endif // BUGWRITERPREFERENCES_H
