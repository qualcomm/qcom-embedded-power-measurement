#ifndef EPMPREFERENCES_H
#define EPMPREFERENCES_H
// Confidential and Proprietary â€“ Qualcomm Technologies, Inc.

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
// Copyright 2021-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
