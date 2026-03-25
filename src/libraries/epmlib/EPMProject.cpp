// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMProject.h"

// libEPM
#include "UDASRecorder.h"

// QCommonConsole
#include "AppCore.h"
#include "ConsoleApplicationEnhancements.h"

// Qt
#include <QDir>
#include <QFileInfo>
#include <QStringBuilder>

_EPMProject::_EPMProject
(
	EPMPreferences* preferences
) :
	_preferences(preferences),
	_epmRun(preferences)
{
	_kratosMapping = KratosMapping(new _KratosMapping);
	_epmTarget.setPlatformPath(_preferences->platformPath());
}

_EPMProject::~_EPMProject()
{
	if (_epmDevice.isNull() == false)
	{
		_epmDevice->close();
		_epmDevice = EPMDevice(Q_NULLPTR);
	}
}

EPMProject _EPMProject::createEPMProject
(
	EPMPreferences* preferences
)
{
	EPMProject result = EPMProject(new _EPMProject(preferences));

	return result;
}

void _EPMProject::setEPMDevice(EPMDevice epmDevice)
{
	_epmDevice = epmDevice;
}

QString _EPMProject::recordingAuthor()
{
	QString result{"Unknown"};

	if (_epmConfigFile != Q_NULLPTR)
		result = _epmConfigFile->recordingAuthor();

	return result;
}

void _EPMProject::setRecordingAuthor(const QString &author)
{
	if (_epmConfigFile != Q_NULLPTR)
		_epmConfigFile->setRecordingAuthor(author);
}

QString _EPMProject::platformPath()
{
	return _epmTarget.platformPath();
}

bool _EPMProject::setPlatformPath(const QString& platformPath)
{
	bool result{false};

	if (platformPath.compare(_epmTarget.platformPath(), Qt::CaseInsensitive) != 0)
		result = _epmTarget.setPlatformPath(platformPath);

	return result;
}

quint32 _EPMProject::getPlatformCount()
{
	return _epmTarget.getPlatformCount();
}

bool _EPMProject::getPlatform
(
	quint32 platformIndex,
	QString& name,
	QString& path
)
{
	bool result{_epmTarget.getPlatform(platformIndex, name, path)};

	return result;
}

bool _EPMProject::loadPlatformConfigFile
(
	const QString& platformConfigFilePath
)
{
	bool result{false};

	if (_epmConfigFile != Q_NULLPTR)
	{
		delete _epmConfigFile;
		_epmConfigFile = Q_NULLPTR;
		_currentPlatformFile.clear();
	}

	_epmConfigFile = new EPMConfigFile;
	if (_epmConfigFile->open(platformConfigFilePath, _kratosMapping) == true)
	{
		result = true;
		_currentPlatformFile = platformConfigFilePath;

		setupChannels();
	}

	return result;
}

quint32 _EPMProject::buildChannelsFromConfiguration(EPMChannels& runtimeChannels)
{
	if (_epmConfigFile != Q_NULLPTR)
		return _epmConfigFile->buildChannelsFromConfiguration(runtimeChannels);

	return 0;
}

void _EPMProject::buildPRNFile(const QString &targetPath, const QString &targetFileName)
{
	if (_epmConfigFile != Q_NULLPTR)
		_epmConfigFile->buildPRNFile(targetPath, targetFileName);
}

quint32 _EPMProject::channelCount()
{
	quint32 result{0};

	result = _currentChannels.count();
	if (result < static_cast<quint32>(_voltageChannels.count()))
		result = _voltageChannels.count();

	return result;
}

bool _EPMProject::getChannelData
(
	quint32 index,
	QString& channelName,
	HashType& channelHash
)
{
	EPMChannel currentChannel;
	EPMChannel voltageChannel;

	bool result{getChannelPair(index, currentChannel, voltageChannel)};

	if (result)
	{
		result = false;

		if (currentChannel.isNull() == false)
		{
			channelName = currentChannel->channelName();
			channelHash = currentChannel->channelHash();

			result = true;
		}
		else if (voltageChannel.isNull() == false)
		{
			channelName = voltageChannel->channelName();
			channelHash = voltageChannel->channelHash();

			result = true;
		}
	}

	return result;
}

bool _EPMProject::getChannelPair
(
	quint32 index,
	EPMChannel& currentChannel,
	EPMChannel& voltageChannel
)
{
	bool result{false};

	voltageChannel = _voltageChannels.at(index);
	if (voltageChannel.isNull() == false)
	{
		HashType channelHash = voltageChannel->channelHash();

		currentChannel = _EPMChannel::findChannelByHash(channelHash, _currentChannels);

		result = true;
	}

	return result;
}

bool _EPMProject::getCurrentChannelState
(
	HashType channelHash,
	bool& active,
	bool& saveRaw
)
{
	bool result{false};

	EPMChannel epmChannel = _EPMChannel::findChannelByHash(channelHash, _currentChannels);
	if (epmChannel.isNull() == false)
	{
		active = epmChannel->active();
		saveRaw = epmChannel->dumpRawValues();

		result = true;
	}

	return result;
}

bool _EPMProject::setCurrentChannelState
(
	HashType channelHash,
	bool active,
	bool saveRaw
)
{
	bool result{false};

	EPMChannel epmChannel = _EPMChannel::findChannelByHash(channelHash, _currentChannels);
	if (epmChannel.isNull() == false)
	{
		epmChannel->setActive(active);
		epmChannel->setDumpRawValues(saveRaw);

		result = true;
	}

	return result;
}

bool _EPMProject::getVoltageChannelState
(
	HashType channelHash,
	bool& active,
	bool& saveRaw
)
{
	bool result{false};

	EPMChannel epmChannel = _EPMChannel::findChannelByHash(channelHash, _voltageChannels);
	if (epmChannel.isNull() == false)
	{
		active = epmChannel->active();
		saveRaw = epmChannel->dumpRawValues();

		result = true;
	}

	return result;
}

bool _EPMProject::setVoltageChannelState
(
	HashType channelHash,
	bool active,
	bool saveRaw
)
{
	bool result{false};

	EPMChannel epmChannel = _EPMChannel::findChannelByHash(channelHash, _voltageChannels);
	if (epmChannel.isNull() == false)
	{
		epmChannel->setActive(active);
		epmChannel->setDumpRawValues(saveRaw);

		result = true;
	}

	return result;
}

QString _EPMProject::runtimeConfigFilePath()
{
	return _runtimeConfigFilePath;
}

void _EPMProject::setupChannels()
{
	_currentChannels.clear();
	_voltageChannels.clear();

	EPMChannels runtimeChannels;

	buildChannelsFromConfiguration(runtimeChannels);

	auto sortIndexLambda = [] (const EPMChannel& ep, const EPMChannel& ep2) -> bool
	{
		return ep->index() < ep2->index();
	};

	auto sortTypeLambda = [] (const EPMChannel& ep, const EPMChannel& ep2) -> bool
	{
		return ep->indexType() < ep2->indexType();
	};

	for (const auto& epmChannel: std::as_const(runtimeChannels))
	{
		if (epmChannel->channelType() == _EPMChannel::eCurrent)
			_currentChannels.push_back(epmChannel);
		else if (epmChannel->channelType() == _EPMChannel::eVoltage || epmChannel->channelType() == _EPMChannel::eMarker)
			_voltageChannels.push_back(epmChannel);
	}

	std::stable_sort(_voltageChannels.begin(), _voltageChannels.end(), sortIndexLambda);
	std::stable_sort(_voltageChannels.begin(), _voltageChannels.end(), sortTypeLambda);
}

void _EPMProject::updateChannel(EPMRuntimeChannel epmRuntimeChannel)
{
	if (epmRuntimeChannel._seriesHash != 0)
	{
		EPMChannel epmChannel;

		epmChannel = _EPMChannel::findSeriesByHash(epmRuntimeChannel._seriesHash, _currentChannels);
		if (epmChannel.isNull())
			epmChannel = _EPMChannel::findSeriesByHash(epmRuntimeChannel._seriesHash, _voltageChannels);

		if (epmChannel.isNull() == false)
		{
			epmChannel->setActive(epmRuntimeChannel._active);
		}
	}
}

bool _EPMProject::loadRuntimeConfigFile
(
	const QString& runtimeConfigFilePath
)
{
	bool result{false};
	QString activePath{runtimeConfigFilePath};

	if (activePath.isEmpty())
	{
		AppCore::writeToApplicationLog("Runtime Config Path is empty");
		return false;
	}

	QFileInfo fileInfo(activePath);

	if (fileInfo.isRelative() == true)
	{
		activePath = _preferences->runtimeConfigurationsPath();

		activePath += QDir::separator() + fileInfo.fileName();
		activePath = QDir::cleanPath(activePath);
	}

	if (_epmRuntimeConfigFile != Q_NULLPTR)
	{
		_epmRuntimeConfigFile->close();
		delete _epmRuntimeConfigFile;
		_epmRuntimeConfigFile = Q_NULLPTR;
	}

	_epmRuntimeConfigFile = new EPMRuntimeConfigFile;
	if (_epmRuntimeConfigFile->open(activePath))
	{
		QFileInfo fileInfo(_currentPlatformFile);

		if (_epmRuntimeConfigFile->platformFile() == fileInfo.fileName())
		{
			QList<HashType> serialHashes = _epmRuntimeConfigFile->getKeys();
			for (const auto& serialHash: serialHashes)
			{
				EPMRuntimeChannel epmRuntimeChannel = _epmRuntimeConfigFile->get(serialHash);

				updateChannel(epmRuntimeChannel);
			}
			result = true;
		}
		else
		{
			QString message = QString("The runtime configfile platform ") % _epmRuntimeConfigFile->platformFile() % " doesn't match the selected platform, " %
				fileInfo.fileName();

			AppCore::writeToApplicationLog(message);
		}
	}
	else
	{
		AppCore::writeToApplicationLog(_epmRuntimeConfigFile->errorText());
	}

	return result;
}

QString _EPMProject::spmVersion()
{
	if (_epmConfigFile != Q_NULLPTR)
		return _epmConfigFile->spmVersionAsText();

	return QString();
}

QString _EPMProject::target()
{
	if (_epmConfigFile != Q_NULLPTR)
		return _epmConfigFile->target();

	return QString();
}

QString _EPMProject::label()
{
	if (_epmConfigFile != Q_NULLPTR)
		return _epmConfigFile->label();

	return QString();
}

QString _EPMProject::description()
{
	if (_epmConfigFile != Q_NULLPTR)
		return _epmConfigFile->description();

	return QString();
}

QStringList _EPMProject::categories()
{
	if (_epmConfigFile != Q_NULLPTR)
		return _epmConfigFile->getCategories();

	return QStringList();
}

bool _EPMProject::acquire(qreal duration)
{
	bool result{false};

	_preferences->setRunDuration(duration);

	UDASRecorder* udasRecorder = new UDASRecorder;

	AppCore::writeToApplicationLog(QString("Starting Acquisition: %1\n").arg(_epmConfigFile->target()));
	AppCore::writeToApplicationLog(QString("   ") + QDateTime::currentDateTime().toString());
	AppCore::writeToApplicationLog(QString("               Label: %1\n").arg(_epmConfigFile->label()));
	AppCore::writeToApplicationLog(QString("         Description: %1\n").arg(_epmConfigFile->description()));
	AppCore::writeToApplicationLog(QString("       Channel Count: %1\n").arg(_epmConfigFile->channelCount()));
	AppCore::writeToApplicationLog(QString("            Duration: %1\n\n").arg(duration));

	QString targetFile = _epmConfigFile->target() + createFilenameTimeStamp();
	QString resultsFolder;

	QString tempDirectory = _epmRun.temporaryResultsDirectory();
	if (tempDirectory.isEmpty() == false)
	{
		resultsFolder = QDir::cleanPath(tempDirectory);
	}
	else
	{
		resultsFolder = QDir::cleanPath(_preferences->outputPath() + QDir::separator() + targetFile);
	}

	_epmRun.setResultsDirectory(resultsFolder);
	_lastResultsPath = resultsFolder;
	udasRecorder->setResultsFolder(resultsFolder);

	_epmDevice->setUDASRecorder(udasRecorder);
	_epmRun.setDevice(_epmDevice);

	buildPRNFile(resultsFolder, targetFile);

	buildRuntimeChannels();

	try
	{
		_epmRun.setRunDuration(_preferences->runDuration());
		_epmRun.acquire();
	}
	catch (EPMException& epmException)
	{
		AppCore::writeToApplicationLog(QString("EPMException epmException: %1\n").arg(epmException.what()));
	}

	_epmDevice->removeUDASRecorder();
	if (udasRecorder)
		delete udasRecorder;

	AppCore::writeToApplicationLog(QString("Acquisition Finished: %1\n").arg(target()));
	AppCore::writeToApplicationLog(QString("            End Time: %1\n") + QDateTime::currentDateTime().toString());

	return result;
}

bool _EPMProject::startAcquire()
{
	bool result{false};

	if (_epmConfigFile != Q_NULLPTR)
	{
		_udasRecorder = new UDASRecorder;
		AppCore::writeToApplicationLog(QString("Starting Acquisition: %1\n").arg(_epmConfigFile->target()));
		AppCore::writeToApplicationLog(QString("   ") + QDateTime::currentDateTime().toString());
		AppCore::writeToApplicationLog(QString("               Label: %1\n").arg(_epmConfigFile->label()));
		AppCore::writeToApplicationLog(QString("         Description: %1\n").arg(_epmConfigFile->description()));
		AppCore::writeToApplicationLog(QString("       Channel Count: %1\n").arg(_epmConfigFile->channelCount()));

		QString targetFile = _epmConfigFile->target() + createFilenameTimeStamp();
		QString resultsFolder;

		QString tempDirectory = _epmRun.temporaryResultsDirectory();
		if (tempDirectory.isEmpty() == false)
		{
			resultsFolder = QDir::cleanPath(tempDirectory);
		}
		else
		{
			resultsFolder = QDir::cleanPath(_preferences->outputPath() + QDir::separator() + targetFile);
		}

		_epmRun.setResultsDirectory(resultsFolder);
		_lastResultsPath = resultsFolder;
		_udasRecorder->setResultsFolder(resultsFolder);

		_epmDevice->setUDASRecorder(_udasRecorder);
		_epmRun.setDevice(_epmDevice);

		buildPRNFile(resultsFolder, targetFile);

		buildRuntimeChannels();

		try
		{
			_epmRun.startAcquire();
			result = true;
		}
		catch (EPMException& epmException)
		{
			AppCore::writeToApplicationLog(QString("EPMException epmException: %1\n").arg(epmException.what()));
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("Please select a configuration to start data acquisition");
	}

	return result;
}

void _EPMProject::stopAcquire()
{
	_epmRun.stopAcquire();

	_epmDevice->removeUDASRecorder();
	if (_udasRecorder)
	{
		delete _udasRecorder;
		_udasRecorder = Q_NULLPTR;
	}

	AppCore::writeToApplicationLog(QString("Acquisition Finished: %1\n").arg(target()));
	AppCore::writeToApplicationLog(QString("            End Time: %1\n") + QDateTime::currentDateTime().toString());
}

QString _EPMProject::getResultsPath()
{
	return _epmRun.resultsPath();
}

void _EPMProject::setTemporaryResultsDirectory
(
	const QString& resultsFolder
)
{
	_epmRun.setTemporaryResultsDirectory(resultsFolder);
}

void _EPMProject::buildRuntimeChannels()
{
	for (auto& currentChannel: _currentChannels)
	{
		if (currentChannel->active())
			_epmRun.addChannel(currentChannel);
		else
			_epmRun.removeChannel(currentChannel);
	}

	for (auto& voltageChannel: _voltageChannels)
	{
		if (voltageChannel->active())
			_epmRun.addChannel(voltageChannel);
		else
			_epmRun.removeChannel(voltageChannel);
	}
}

bool _EPMProject::setRuntimeConfigurationFile
(
	const QString& runtimeConfigFilePath
)
{
	bool result{false};

	if (_epmRuntimeConfigFile != Q_NULLPTR)
	{
		_epmRuntimeConfigFile->close();
		delete _epmRuntimeConfigFile;
		_epmRuntimeConfigFile = Q_NULLPTR;
	}

	QString runtimeConfigDirectory = runtimeConfigFilePath.mid(0, runtimeConfigFilePath.lastIndexOf("/"));
	if (QDir(runtimeConfigDirectory).exists())
	{
		_epmRuntimeConfigFile = new EPMRuntimeConfigFile;
		if (_epmRuntimeConfigFile != Q_NULLPTR)
		{
			if (runtimeConfigFilePath.isEmpty() == false)
			{
				_runtimeConfigFilePath = runtimeConfigFilePath;
				AppCore::writeToApplicationLog("Runtime Path Set");
				result = true;
			}
			else
			{
				_runtimeConfigFilePath.clear();
				AppCore::writeToApplicationLog("Runtime Path Cleared");
				result = true;
			}
		}
		else
		{
			_runtimeConfigFilePath.clear();
			AppCore::writeToApplicationLog("Could not initialize EPMRuntimeConfigFile");
		}
	}
	else
	{
		_runtimeConfigFilePath.clear();
		AppCore::writeToApplicationLog(QString("%1 directory does not exist.").arg(runtimeConfigDirectory));
	}

	return result;
}

bool _EPMProject::saveRuntimeConfigurationFile()
{
	bool result{true};

	QFileInfo fileInfo(_currentPlatformFile);

	if (_epmRuntimeConfigFile != Q_NULLPTR)
	{
		_epmRuntimeConfigFile->setPlatformFile(fileInfo.fileName());
		_epmRuntimeConfigFile->resetChannels();

		for (const auto& currentChannel : std::as_const(_currentChannels))
		{
			_epmRuntimeConfigFile->addChannel(currentChannel);
		}

		for (const auto& voltageChannel : std::as_const(_voltageChannels))
		{
			_epmRuntimeConfigFile->addChannel(voltageChannel);
		}

		_epmRuntimeConfigFile->save(_runtimeConfigFilePath);
	}

	return result;
}

QString _EPMProject::exportConfigFilePath()
{
	return _exportConfigFilePath;
}

bool _EPMProject::loadExportConfigFile(const QString &exportConfigFilePath)
{
	bool result{false};
	QString activePath{exportConfigFilePath};

	if (activePath.isEmpty())
	{
		AppCore::writeToApplicationLog("Export Config Path is empty");
		return false;
	}

	QFileInfo fileInfo(activePath);

	if (fileInfo.isRelative() == true)
	{
		activePath = _preferences->defaultExportConfigurationsPath();

		activePath += QDir::separator() + fileInfo.fileName();
		activePath = QDir::cleanPath(activePath);
	}

	if (_epmExportConfigFile != Q_NULLPTR)
	{
		_epmExportConfigFile->close();
		delete _epmExportConfigFile;
		_epmExportConfigFile = Q_NULLPTR;
	}

	_epmExportConfigFile = new EPMExportConfigFile;
	if (_epmExportConfigFile->open(activePath))
	{
		QFileInfo fileInfo(_currentPlatformFile);

		if (_epmExportConfigFile->platformFile() == fileInfo.fileName())
		{
			QList<HashType> serialHashes = _epmExportConfigFile->getKeys();
			for (const auto& serialHash: serialHashes)
			{
				EPMRuntimeChannel epmRuntimeChannel = _epmExportConfigFile->get(serialHash);

				updateChannel(epmRuntimeChannel);
			}
			result = true;
		}
		else
		{
			QString message = QString("The export config file platform ") % _epmExportConfigFile->platformFile() % " doesn't match the selected platform, " %
							  fileInfo.fileName();

			AppCore::writeToApplicationLog(message);
		}
	}
	else
	{
		AppCore::writeToApplicationLog(_epmExportConfigFile->errorText());
	}

	return result;
}

bool _EPMProject::setExportConfigurationFile(const QString& exportConfigFilePath)
{
	bool result{false};

	if (_epmExportConfigFile != Q_NULLPTR)
	{
		_epmExportConfigFile->close();
		delete _epmExportConfigFile;
		_epmExportConfigFile = Q_NULLPTR;
	}

	QString exportConfigDirectory = exportConfigFilePath.mid(0, exportConfigFilePath.lastIndexOf("/"));
	if (QDir(exportConfigDirectory).exists())
	{
		_epmExportConfigFile = new EPMExportConfigFile;
		if (_epmExportConfigFile != Q_NULLPTR)
		{
			if (exportConfigFilePath.isEmpty() == false)
			{
				_exportConfigFilePath = exportConfigFilePath;
				AppCore::writeToApplicationLog("Export Path Set");
				result = true;
			}
			else
			{
				_exportConfigFilePath.clear();
				AppCore::writeToApplicationLog("Export Path Cleared");
				result = true;
			}
		}
		else
		{
			_exportConfigFilePath.clear();
			AppCore::writeToApplicationLog("Could not initialize EPMExportConfigFile");
		}
	}
	else
	{
		_exportConfigFilePath.clear();
		AppCore::writeToApplicationLog(QString("%1 directory does not exist.").arg(exportConfigDirectory));
	}

	return result;
}

bool _EPMProject::saveExportConfigurationFile()
{
	bool result{true};

	QFileInfo fileInfo(_currentPlatformFile);

	if (_epmExportConfigFile != Q_NULLPTR)
	{
		_epmExportConfigFile->setPlatformFile(fileInfo.fileName());
		_epmExportConfigFile->resetChannels();

		for (const auto& currentChannel : std::as_const(_currentChannels))
		{
			_epmExportConfigFile->addChannel(currentChannel);
		}

		for (const auto& voltageChannel : std::as_const(_voltageChannels))
		{
			_epmExportConfigFile->addChannel(voltageChannel);
		}

		_epmExportConfigFile->save(_exportConfigFilePath);
	}

	return result;
}

