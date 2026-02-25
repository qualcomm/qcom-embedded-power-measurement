#ifndef EPMDEVICELIST_H
#define EPMDEVICELIST_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Arkojit Sen (arkosen@qti.qualcomm.com)
*/

// EPMLib
#include "EPMDevice.h"
#include "EPMGlobalLib.h"

// Qt
#include <QTimer>
#include <QWidget>

namespace Ui
{
	class EPMDeviceListClass;
}

class EPMLIB_EXPORT EPMDeviceList :
	public QWidget
{
Q_OBJECT

public:
	explicit EPMDeviceList(QWidget* parent = Q_NULLPTR);
	virtual ~EPMDeviceList();

	void addToLog(const QString& logMe);

	QString lastError()
	{
		return _lastError;
	}

	void refresh();

	void showProbeButton(bool show = true);

	EPMDevice currentDevice();
	bool selectFirstDevice();
	bool selectDeviceByName(const QString& deviceName);

signals:
	void deviceSelected(bool deviceAvailable);
	void deviceCountUpdated(uint deviceCount);
	void deviceScanStarted();

private slots:
	void on__deviceList_currentRowChanged(int currentRow);
	void on__probeDevicesButton_clicked();
	void on__timer_timeout();

private:
	Ui::EPMDeviceListClass*		_ui = Q_NULLPTR;
	QTimer						_timer;
	QString						_lastError;

	void setupDeviceList();
};

#endif // EPMDEVICELIST_H
