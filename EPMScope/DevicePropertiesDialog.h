#ifndef DEVICEPROPERTIESDIALOG_H
#define DEVICEPROPERTIESDIALOG_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_DevicePropertiesDialog.h"
#include "EPMDevice.h"

// Qt
#include <QDialog>


class DevicePropertiesDialog :
	public QDialog, private Ui::DevicePropertiesDialog
{
	Q_OBJECT

public:
	DevicePropertiesDialog(QWidget *parent = nullptr);
	~DevicePropertiesDialog();

	void setDevice(EPMDevice epmDevice);

private slots:
	void on__okayBtn_clicked();
};

#endif // DEVICEPROPERTIESDIALOG_H
