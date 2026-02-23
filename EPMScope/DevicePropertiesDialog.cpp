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
// Copyright 2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
