#ifndef EPMDEVICEDIALOG_H
#define EPMDEVICEDIALOG_H
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
// Copyright 2020-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// libEpm
#include "EPMDevice.h"

// Qt
#include <QDialog>
#include <QTimer>
#include <QWidget>

namespace Ui {
	class DeviceDialogClass;
}

class EPMLIB_EXPORT EPMDeviceDialog :
	public QDialog
{
	Q_OBJECT

public:
	explicit EPMDeviceDialog(QWidget* parent = Q_NULLPTR);
	~EPMDeviceDialog();
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

protected:
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void on__deviceList_currentRowChanged(int currentRow);
	void on__probeDevicesButton_clicked();
	void on__timer_timeout();

	void on_DeviceDialogClass_accepted();

private:
	Ui::DeviceDialogClass*		_ui{Q_NULLPTR};
	bool						_resizeEvent{false};
	QTimer						_timer;
	QString						_lastError;

	void setupDeviceList();
};

#endif // EPMDEVICEDIALOG_H
