#ifndef EPMRUN_H
#define EPMRUN_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPMLib
#include "EPMPreferences.h"
#include "EPMDevice.h"
#include "EPMResult.h"
#include "EPMGlobalLib.h"

// Qt
#include <QList>
#include <QString>

class EPMLIB_EXPORT EPMRun
{
public:
	EPMRun(EPMPreferences* preferences);
	~EPMRun();
	
	void setRunDuration(qreal seconds)
	{
		_preferences->setRunDuration(seconds);
	}
	qreal runDuration();

	// Devices
	void setDevice(EPMDevice epmDevice)
	{
		_epmDevice = epmDevice;
	}
	EPMDevice getDevice()
	{
		return _epmDevice;
	}

	void setResultsDirectory(const QString& resultsPath);

	void setTemporaryResultsDirectory(const QString& temporaryResultsPath)
	{
		_temporaryResultsPath = temporaryResultsPath;
	}
	QString temporaryResultsDirectory();

	QString resultsPath()
	{
		return _resultsPath;
	}

	void addChannel(EPMChannel addMe);
	void removeChannel(EPMChannel removeMe);

	// Forget the channels of the previous recording. At the next acquisition
	// the device's own selection is cleared and rebuilt from the channels
	// added after this call, whichever device that acquisition uses.
	void resetChannels();

    bool setMarkerTrigger(bool state);

	// snapshot
    void acquire();

	// dynamic
	void startAcquire();
	void stopAcquire();

private:
	EPMPreferences*				_preferences{Q_NULLPTR};

	EPMDevice					_epmDevice;
	EPMChannels					_activeChannels;
	QList<EPMResult>			_result;
	QString						_resultsPath;

	QString						_temporaryResultsPath;

	void log(const QString& logMe);

	void applyChannelsToDevice();

	EPMDevice getEPMDevice(const quint32 index, const EPMIndexType indexType);
};

#endif // EPMRUN_H
