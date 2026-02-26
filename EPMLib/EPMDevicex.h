#ifndef EPMDEVICE_H
#define EPMDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
