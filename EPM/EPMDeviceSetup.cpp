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
// Copyright ©2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

#include "EPMDeviceSetup.h"

// EPM
#include "EPMApplication.h"
#include "EPMDefines.h"

// QCommon
#include "AlpacaSettings.h"
#include "Range.h"

// Qt
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>

const int kPortCol(0);
const int kNameCol(1);
const int kSerialNumberCol(2);
const int kKRAMCol(3);
const int kUUIDCol(4);

EPMDeviceSetup::EPMDeviceSetup
(
	QWidget* parent
) :
	QDialog(parent)
{
	setupUi(this);

	connect(this, &QDialog::accepted, this, [this]{ saveSettings();});
	restoreSettings();

	enableOk(false);

	QHeaderView* headerView = _epmDevices->horizontalHeader();
	if (headerView != Q_NULLPTR)
	{
		headerView->setFrameStyle(QFrame::Raised);
		headerView->setFrameShape(QFrame::StyledPanel);
	}

	filterEPMDevices();
}

EPMDeviceSetup::~EPMDeviceSetup()
{
}

void EPMDeviceSetup::on__epmDevices_clicked
(
	const QModelIndex& index
)
{
	Q_UNUSED(index)
}

void EPMDeviceSetup::on__epmDevices_doubleClicked
(
	const QModelIndex& index
)
{
	on__epmDevices_clicked(index);

	accept();
}

void EPMDeviceSetup::on__allDeviceRadio_clicked
(
	bool checked
)
{
	if (checked)
		setupUsageState(eAllDevices);
}

void EPMDeviceSetup::on__firstDeviceRadio_clicked
(
	bool checked
)
{
	if (checked)
		setupUsageState(eFirst);
}

void EPMDeviceSetup::on__namedDeviceRadio_clicked
(
	bool checked
)
{
	if (checked)
		setupUsageState(eNamed);
}

void EPMDeviceSetup::on__mappedDevicesRadio_clicked
(
	bool checked
)
{
	if (checked)
		setupUsageState(eMapped);
}

void EPMDeviceSetup::saveSettings()
{
	AlpacaSettings settings(kAppName);

	settings.beginGroup(kDeviceSetupGroup);

	DeviceUsageState deviceUsage = getDeviceUsageState();
	settings.setValue(kUsageState, static_cast<int>(deviceUsage));
	if (deviceUsage == eNamed)
		settings.setValue(kNamedDevice, _namedDeviceCombo->currentText());
	else
		settings.setValue(kNamedDevice, QString());

	settings.endGroup();
}

void EPMDeviceSetup::restoreSettings()
{
	AlpacaSettings settings(kAppName);

	settings.beginGroup(kDeviceSetupGroup);
	DeviceUsageState usageState = static_cast<DeviceUsageState>(settings.value(kUsageState, eFirst).toInt());

	setupUsageState(usageState);

	settings.endGroup();
}

void EPMDeviceSetup::enableOk
(
	bool state
)
{
	QPushButton* okayButton = _buttonBox->button(QDialogButtonBox::Ok);
	if (okayButton != Q_NULLPTR)
	{
		okayButton->setEnabled(state);
	}
}

void EPMDeviceSetup::setupUsageState
(
	DeviceUsageState deviceUsageState
)
{
	switch (deviceUsageState)
	{
	case eAllDevices:
		_epmDevices->setEnabled(false);
		_allDeviceRadio->setChecked(true);
		break;

	case eFirst:
		_epmDevices->setEnabled(false);
		_firstDeviceRadio->setChecked(true);
		break;

	case eNamed:
		_epmDevices->setEnabled(false);
		_namedDeviceRadio->setChecked(true);
		break;

	case eMapped:
		_epmDevices->setEnabled(true);
		_mappedDevicesRadio->setChecked(true);
		break;
	}

	_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

	saveSettings();
}

DeviceUsageState EPMDeviceSetup::getDeviceUsageState()
{
	DeviceUsageState deviceUsage(eAllDevices);

	if (_firstDeviceRadio->isChecked())
		deviceUsage = eFirst;
	else if (_namedDeviceRadio->isChecked())
		deviceUsage = eNamed;
	else if (_mappedDevicesRadio->isChecked())
		deviceUsage = eMapped;

	return deviceUsage;
}

QString EPMDeviceSetup::namedDevice()
{
	if (getDeviceUsageState() == eNamed)
		return _namedDeviceCombo->currentText();

	return QString();
}

void EPMDeviceSetup::filterEPMDevices()
{
	_namedDeviceCombo->clear();
	_epmDevices->clearContents();

	_EPMDevice::updateDeviceList();

	uint deviceCount = _EPMDevice::deviceCount();
	_epmDevices->setRowCount(static_cast<int>(deviceCount));

	for (auto index : basic_range<uint>(deviceCount))
	{
		EPMDevice epmDevice = _EPMDevice::device(index);
		if (epmDevice.isNull() == false)
		{
			_namedDeviceCombo->addItem(epmDevice->getSerialNumber());

			QTableWidgetItem* twi;

			twi = new QTableWidgetItem(epmDevice->getTargetName());
			twi->setData(Qt::UserRole, epmDevice->hash());
			_epmDevices->setItem(static_cast<int>(index), 0, twi);

			twi = new QTableWidgetItem(epmDevice->getSerialNumber());
			_epmDevices->setItem(static_cast<int>(index), 1, twi);

			twi = new QTableWidgetItem(epmDevice->getUUID());
			_epmDevices->setItem(static_cast<int>(index), 2, twi);
		}
	}
}
