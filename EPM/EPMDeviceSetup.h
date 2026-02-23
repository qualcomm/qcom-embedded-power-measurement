#ifndef EPMDEVICESETUP_H
#define EPMDEVICESETUP_H
// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2018-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
