#ifndef QEPMAPPLICATION_H
#define QEPMAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonGlobal.h"

// QCommonConsole
#include "AppCore.h"

// QT
#include <QApplication>

class QCOMMON_EXPORT QEPMApplication :
	public QApplication
{
Q_OBJECT

public:
	explicit QEPMApplication(int& argc, char** argv, const QString& appName, const QString& appVersion);
	~QEPMApplication();

	bool initialize(PreferencesBase* preferencesBase);

	static QEPMApplication* QEPMAppinstance();
	static AppCore* appCore()
	{
		return QEPMApplication::QEPMAppinstance()->_appCore;
	}

	bool readyToRate();
	void showRateDialog();

protected:
	void cleanupLogs();

	virtual void shutDown() = 0;
	virtual bool event(QEvent* e);

protected:
	QString						_appName;
	QString						_appVersion;
	AppCore*					_appCore{Q_NULLPTR};
	bool						_dateStale{false};
};

#endif // QEPMAPPLICATION_H
