#ifndef EPMDEVICESETUP_H
#define EPMDEVICESETUP_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Author: msimpson

// QtUSB

#include "ui_EPMDeviceSetup.h"

// ligEPM
#include "EPMDefines.h"

// QT
#include <QDialog>
#include <QString>

const QString kDeviceSetupGroup("DeviceSetup");
const QString kUsageState("UsageState");
const QString kNamedDevice("NamedDevice");

class EPMDeviceSetup :
	public QDialog,
	public Ui::EPMDeviceSetupClass
{
Q_OBJECT

public:
	EPMDeviceSetup(QWidget* parent);
	~EPMDeviceSetup();

	DeviceUsageState getDeviceUsageState(void);
	QString namedDevice(void);

public slots:
	void on__epmDevices_clicked(const QModelIndex& index);
	void on__epmDevices_doubleClicked(const QModelIndex& index);

	void on__allDeviceRadio_clicked(bool checked);
	void on__firstDeviceRadio_clicked(bool checked);
	void on__namedDeviceRadio_clicked(bool checked);
	void on__mappedDevicesRadio_clicked(bool checked);

private:
	void saveSettings(void);
	void restoreSettings(void);

	void enableOk(bool state = true);
	void setupUsageState(DeviceUsageState deviceUsageState = eFirst);

	void filterEPMDevices(void);
};

#endif // EPMDEVICESETUP_H
