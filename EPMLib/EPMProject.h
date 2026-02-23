#ifndef EPMPROJECT_H
#define EPMPROJECT_H
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
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "EPMGlobalLib.h"

#include "EPMChannel.h"
#include "EPMConfigFile.h"
#include "EPMDevice.h"
#include "EPMPreferences.h"
#include "EPMRun.h"
#include "EPMRuntimeConfigFile.h"
#include "EPMExportConfigFile.h"
#include "EPMTarget.h"
class UDASRecorder;

#include <QSharedPointer>

class _EPMProject;
typedef QSharedPointer<_EPMProject> EPMProject;

class EPMLIB_EXPORT _EPMProject
{
public:
	_EPMProject(EPMPreferences* preferences);
	~_EPMProject();

	static EPMProject createEPMProject(EPMPreferences* preferences);

	void setEPMDevice(EPMDevice epmDevice);

	QString recordingAuthor();
	void setRecordingAuthor(const QString& author);

	// Platform
	QString platformPath();
	bool setPlatformPath(const QString& platformPath);

	quint32 getPlatformCount();
	bool getPlatform(quint32 platformIndex, QString& name, QString& path);

	// Configuration Files
	bool loadPlatformConfigFile(const QString& platformPath);

	// Channels
	quint32 channelCount();
	bool getChannelData(quint32 index, QString& channelName, HashType& channelHash);
	bool getChannelPair(quint32 index, EPMChannel& currentChannel, EPMChannel& voltageChannel);

	bool getCurrentChannelState(HashType channelHash, bool& active, bool& saveRaw);
	bool setCurrentChannelState(HashType channelHash, bool active, bool saveRaw = false);

	bool getVoltageChannelState(HashType channelHash, bool& active, bool& saveRaw);
	bool setVoltageChannelState(HashType channelHash, bool active, bool saveRaw = false);

	// Runtime Configuration
	QString runtimeConfigFilePath();
	bool loadRuntimeConfigFile(const QString& runtimeConfigFilePath);
	bool setRuntimeConfigurationFile(const QString& runtimeConfigFilePath);
	bool saveRuntimeConfigurationFile();

	// Export Configuration
	QString exportConfigFilePath();
	bool loadExportConfigFile(const QString& exportConfigFilePath);
	bool setExportConfigurationFile(const QString& exportConfigFilePath);
	bool saveExportConfigurationFile();

	QString spmVersion();
	QString target();
	QString label();
	QString description();

	QStringList categories();

	bool acquire(qreal duration);

	bool startAcquire();
	void stopAcquire();

	QString getResultsPath();

	void setTemporaryResultsDirectory(const QString& resultsFolder);

protected:
	EPMPreferences*				_preferences{Q_NULLPTR};
	EPMTarget					_epmTarget;
	EPMDevice					_epmDevice;
	EPMRun						_epmRun;
	UDASRecorder*				_udasRecorder{Q_NULLPTR};
	QString						_lastResultsPath;

	KratosMapping				_kratosMapping;
	EPMConfigFile*				_epmConfigFile{Q_NULLPTR};
	QString						_currentPlatformFile;

	QString						_runtimeConfigFilePath;
	EPMRuntimeConfigFile*		_epmRuntimeConfigFile{Q_NULLPTR};

	QString						_exportConfigFilePath;
	EPMExportConfigFile*		_epmExportConfigFile{Q_NULLPTR};

	EPMChannels					_currentChannels;
	EPMChannels					_voltageChannels;

private:
	void setupChannels();
	void updateChannel(EPMRuntimeChannel epmRuntimeChannel);

	quint32 buildChannelsFromConfiguration(EPMChannels& runtimeChannels);
	void buildPRNFile(const QString &targetPath, const QString &targetFileName);
	void buildRuntimeChannels();
};

#endif // EPMPROJECT_H
