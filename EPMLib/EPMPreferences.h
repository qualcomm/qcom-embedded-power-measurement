#ifndef EPMPREFERENCES_H
#define EPMPREFERENCES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"

// QCommon
#include "PreferencesBase.h"

// QT
#include <QString>

class EPMLIB_EXPORT EPMPreferences :
	public PreferencesBase
{
public:
	EPMPreferences() = default;
	virtual ~EPMPreferences() = default;

	virtual void setAppName(const QByteArray& appName, const QByteArray& appVersion);

	QString defaultPlatformPath();
	QString platformPath();
	void setPlatformPath(const QString& platformPath);
	void savePlatformPath(const QString& platformPath);

	QString defaultRuntimeConfigurationsPath();
	QString runtimeConfigurationsPath();
	void setRuntimeConfigurationsPath(const QString& runtimeConfigurationPath);
	void saveRuntimeConfigurationsPath(const QString& runtimeConfigurationPath);

	QString defaultOutputPath();
	QString outputPath();
	void setOutputPath(const QString& outputPath);
	void saveOutputPath(const QString& outputPath);

	QString lastPlatform();
	void setLastPlatform(const QString& lastPlatform);
	void saveLastPlatform(const QString& lastPlatform);

	qreal defaultRunDuration();
	qreal runDuration();
	void setRunDuration(qreal runDuration);
	void saveRunDuration(qreal runDuration);

	QString defaultExportConfigurationsPath();

private:
	QString						_platformPath;
	QString						_runtimeConfigurationsPath;
	QString						_outputPath;
	QString						_lastPlatform;
	qreal						_runDuration{0.};
};

#endif // EPMPREFERENCES_H
