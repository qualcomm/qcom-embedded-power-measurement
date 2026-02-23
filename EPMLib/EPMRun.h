#ifndef EPMRUN_H
#define EPMRUN_H
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
// Copyright 2018-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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

	// snapshot
    void acquire();

	// dynamic
	void startAcquire();
	void stopAcquire();

private:
	EPMPreferences*				_preferences{Q_NULLPTR};

	EPMDevice					_epmDevice;
	EPMChannels					_activeChannels;
	EPMChannels					_removedChannels;
	QList<EPMResult>			_result;
	QString						_resultsPath;

	QString						_temporaryResultsPath;

	void log(const QString& logMe);

	EPMDevice getEPMDevice(const quint32 index, const EPMIndexType indexType);
};

#endif // EPMRUN_H
