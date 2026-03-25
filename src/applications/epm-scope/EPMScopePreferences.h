#ifndef  EPMScopePREFERENCES_H
#define  EPMScopePREFERENCES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QString>

#include "EPMPreferences.h"

class EPMScopePreferences :
	public EPMPreferences
{
public:
	EPMScopePreferences();

	virtual void setAppName(const QByteArray& appName, const QByteArray& appVersion);

	qreal defaultWindowDuration();
	qreal windowDuration();
	void setWindowDuration(qreal newWindowDuration);
	void saveWindowDuration(qreal newWindowDuration);

	bool defaultSaveData();
	bool saveRunData();
	void setSaveRunData(bool saveRunData);
	void saveSaveRunData(bool saveRunData);

	QString defaultLastPlatform();
	QString lastPlatform();
	void setLastPlatform(const QString& lastPlatform);
	void saveLastPlatform(const QString& lastPlatform);

	QString lastDevice();
	void saveLastDevice(const QString& lastDevice);

private:
	qreal						_windowDuration;
	bool						_saveRunData;
	QString						_lastPlatform;
	QString						_lastDevice;
};

#endif // PREFERENCE_H
