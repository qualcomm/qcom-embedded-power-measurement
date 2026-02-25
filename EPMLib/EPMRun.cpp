// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// libEPM
#include "EPMRun.h"
#include "EPMPreferences.h"

// QCommonConsole
#include "KratosMapping.h"
#include "MathFunctions.h"
#include "Range.h"
#include "TickCount.h"

//Qt
#include <QCoreApplication>
#include <QDir>

EPMRun::EPMRun(EPMPreferences* preferences) :
	_preferences(preferences)
{

}

EPMRun::~EPMRun()
{
	_epmDevice = EPMDevice(Q_NULLPTR);
	_activeChannels.clear();
}

qreal EPMRun::runDuration()
{
	return _preferences->runDuration();
}


void EPMRun::setResultsDirectory
(
	const QString& resultsPath
)
{
	_resultsPath = resultsPath;

	QDir().mkpath(_resultsPath);
}

QString EPMRun::temporaryResultsDirectory()
{
	QString result{_temporaryResultsPath};

	_temporaryResultsPath.clear();

	return result;
}

void EPMRun::addChannel
(
	EPMChannel addMe
)
{
	if (addMe->valid())
	{
		_activeChannels.append(addMe);

		log(QString("EPM Run Channel %1 added\n").arg(addMe->seriesName()));
	}
}

void EPMRun::removeChannel
(
	EPMChannel removeMe
)
{
	EPMDevice epmDevice = getEPMDevice(removeMe->index(), removeMe->indexType());
	if (epmDevice.isNull() == false)
	{
		if (removeMe->valid())
		{
			auto runChannelIter = _activeChannels.begin();
			while (runChannelIter != _activeChannels.end())
			{
				if ((*runChannelIter)->channel() == removeMe->channel())
				{
					_removedChannels.append(*runChannelIter);
					
					_activeChannels.erase(runChannelIter);	

					log(QString("EPM Run Channel %1 removed\n").arg(removeMe->seriesName()));

					break;
				}

				runChannelIter++;
			}
		}
	}
	else
	{
		log(QString("getEPMDevice %1 == null\n").arg(removeMe->index()));
	}
}

void EPMRun::acquire()
{
	EpmErrorCode threadExitError{MICRO_EPM_SUCCESS};

	try
	{
		_epmDevice->open();
	}
    catch (EPMException& error)
	{
		throw error;
	}

	for (const auto& runChannel: std::as_const(_removedChannels))
	{
		_epmDevice->removeChannel(runChannel);
	}

	if (_activeChannels.count() > 0)
	{
		for (const auto& runChannel: std::as_const(_activeChannels))
		{
			_epmDevice->addChannel(runChannel);
		}

		bool process{true};

		try 
		{
			_epmDevice->acquire();
		} 
        catch (EPMException& error)
		{
			log(QString("epm->acquire failed: %1\n").arg(error.what()));
			process = false;
		}
		
		QCoreApplication* instance = QCoreApplication::instance();

		quint64 end = tickCount() + static_cast<quint64>(_preferences->runDuration() * 1000.0);

		while (process == true)
		{
			if (tickCount() <= end)
			{
				instance->processEvents();

				if (_epmDevice->isRunning() == false)
				{
					process = false;
					threadExitError = _epmDevice->threadExitCode();
				}
			}
			else
			{
				process = false;
			}
		}

		_epmDevice->stop();
	}


	if (threadExitError != MICRO_EPM_SUCCESS && threadExitError != MICRO_EPM_OVERFLOW)
	{
        EPMException error(threadExitError, 0, EPMErrorToByteArray(threadExitError));

		log(error.what());
	}
}

void EPMRun::startAcquire()
{
	try
	{
		_epmDevice->open();
	}
    catch (EPMException& error)
	{
		throw error;
	}

	for (const auto& runChannel: std::as_const(_removedChannels))
	{
		_epmDevice->removeChannel(runChannel);
	}

	for (const auto& runChannel: std::as_const(_activeChannels))
	{
		_epmDevice->addChannel(runChannel);
	}

	_epmDevice->acquire();
}

void EPMRun::stopAcquire()
{
	EpmErrorCode threadExitError{MICRO_EPM_SUCCESS};

	if (_epmDevice->isRunning() == false)
	{
		threadExitError = _epmDevice->threadExitCode();
	}

	_epmDevice->stop();
	_epmDevice->close();

	if (threadExitError != MICRO_EPM_SUCCESS && threadExitError != MICRO_EPM_OVERFLOW)
	{
        EPMException error(threadExitError, 0, EPMErrorToByteArray(threadExitError));

		log(error.what());
	}
}

void EPMRun::log
(
	const QString& logMe
)
{
	AppCore::writeToRuntimeLog(logMe);
}

EPMDevice EPMRun::getEPMDevice
(
	const quint32 index,
	const EPMIndexType indexType
)
{
	EPMDevice epmDevice = _epmDevice;

	Q_UNUSED(index)
	Q_UNUSED(indexType);
	
	return epmDevice;
}



