#ifndef EPMDEVICE_H
#define EPMDEVICE_H
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
// Copyright 2018-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// QCommon
#include "EPMChannel.h"
#include "EPMResult.h"
#include "EPMRunChannel.h"
#include "ThreadedLog.h"

struct EpmImpl;

// Qt
class QFile;
#include <QList>
#include <QMap>
#include <QSharedPointer>
class QTextStream;
#include <QSet>

const uint kInvalidDevice(0xFFFFFFFF);

class _EPMDevice;

typedef QSharedPointer<_EPMDevice> EPMDevice;

typedef QMap<HashType, EPMDevice> EPMDeviceMap;

enum DeviceUsageState
{
	eAllDevices,
	eFirst,
	eNamed,
	eMapped
};

class _EPMDevice
{
public:
	_EPMDevice();
	~_EPMDevice();

	static EPMDeviceMap getEpmDevices(QString& deviceError);

	void setOutputDirectory(const QString& outputDirectory);
	void setLog(ThreadedLog threadedLog);

	void addChannel(const EPMRunChannel& epmRunChannel);
	void removeChannel(const EPMRunChannel& epmChannel);
	void acquire(void);
	void stop(void);

	quint32							_portIndex;
	QString							_port;
	QString							_name;
	QString							_serialNumber;
	QString							_uuid;

	EpmImpl*						_impl;
	QMap<quint32, EPMRunChannel>	_runChannels;
	EPMResults						_results;

private:
	void log(const QString& message);

	QString							_outputDirectory;
	ThreadedLog						_threadedLog;
};

#endif // EPMDEVICE_H
