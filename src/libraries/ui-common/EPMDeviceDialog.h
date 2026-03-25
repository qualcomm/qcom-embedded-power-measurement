#ifndef EPMDEVICEDIALOG_H
#define EPMDEVICEDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
