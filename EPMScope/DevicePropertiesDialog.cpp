// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "DevicePropertiesDialog.h"

// Qt
#include <QDateTime>

const QString formattedText("<html><head/><body><p><span style=\" font-weight:700; color:#0000ff;\">%1</span></p></body></html>");


DevicePropertiesDialog::DevicePropertiesDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
}

DevicePropertiesDialog::~DevicePropertiesDialog()
{
}

void DevicePropertiesDialog::setDevice(EPMDevice epmDevice)
{
	TargetInfo info = epmDevice->getTargetInfo();
	QString dt = QDateTime::fromSecsSinceEpoch(info._eepromProgrammedTime).toString();

	_name->setText(formattedText.arg(epmDevice->getTargetName()));
	_guid->setText(formattedText.arg(epmDevice->getUUID()));
	_platformId->setText(formattedText.arg(epmDevice->getPlatformIDString()));
	_progTime->setText(formattedText.arg(dt));
	_serial->setText(formattedText.arg(epmDevice->getSerialNumber()));

	quint8* firmwareVersion = epmDevice->_version._firmwareVersion;
	QString formattedVersion = QString::number(firmwareVersion[0]) + "." + QString::number(firmwareVersion[1]) + "." + QString::number(firmwareVersion[2]) + "." + QString::number(firmwareVersion[3]);
	_version->setText(formattedText.arg(formattedVersion));
}

void DevicePropertiesDialog::on__okayBtn_clicked()
{
	accept();
}
