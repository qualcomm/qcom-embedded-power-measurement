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
// Copyright 2020-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMDeviceDialog.h"
#include "ui_EPMDeviceDialog.h"

// QCommon
#include "Range.h"

const int kHashIndexRole(Qt::UserRole + 4);

EPMDeviceDialog::EPMDeviceDialog
(
	QWidget* parent
) :
	QDialog(parent),
	_ui(new Ui::DeviceDialogClass)
{
	_ui->setupUi(this);

	connect(_ui->_probeDevicesButton, &QPushButton::clicked, this, &EPMDeviceDialog::on__probeDevicesButton_clicked);
	connect(&_timer, &QTimer::timeout, this, &EPMDeviceDialog::on__timer_timeout);
	refresh();
	showProbeButton(false);
}

EPMDeviceDialog::~EPMDeviceDialog()
{
	delete _ui;
}

void EPMDeviceDialog::refresh()
{
	setupDeviceList();
}

void EPMDeviceDialog::showProbeButton(bool show)
{
	if (show)
	{
		_ui->_probeDevicesButton->show();
		_timer.stop();
	}
	else
	{
		_ui->_probeDevicesButton->hide();
		_timer.start(2000);
	}
}

EPMDevice EPMDeviceDialog::currentDevice()
{
	EPMDevice result;

	QTableWidgetItem* lwi{Q_NULLPTR};

	auto selectedDevices = _ui->_deviceList->selectedItems();
	if (selectedDevices.isEmpty() == false)
	{
		lwi = selectedDevices.at(0);
	}
	else if (_ui->_deviceList->rowCount() > 0)
	{
		lwi = _ui->_deviceList->item(0, 0);
	}

	if (lwi != Q_NULLPTR)
	{
		HashType deviceHash = lwi->data(kHashIndexRole).toULongLong();
		result = _EPMDevice::deviceByHash(deviceHash);
	}

	return result;
}

bool EPMDeviceDialog::selectFirstDevice()
{
	bool result{false};

/*	if (_ui->_deviceList->count())
	{
		_ui->_deviceList->setCurrentRow(0, QItemSelectionModel::Select);
		emit deviceSelected(true);
		result = true;
	}
	else
	{
		emit deviceSelected(false);
	}
*/
	return result;
}

bool EPMDeviceDialog::selectDeviceByName
(
	const QString &deviceName
)
{
	Q_UNUSED(deviceName)

	bool result(false);

/*	QTableWidgetItem* lwi;

	for (auto row: range(_ui->_deviceList->count()))
	{
		lwi = _ui->_deviceList->item(row);

		QString portName = lwi->text().toLower();
		if (portName == deviceName.toLower())
		{
			_ui->_deviceList->setCurrentRow(row, QItemSelectionModel::Select);
			result = true;
			emit deviceSelected(true);
			break;
		}
	}

	if (result == false)
	{
		_lastError = QString("Device %1 not found.").arg(deviceName);
		emit deviceSelected(false);
	}
*/
	return result;
}

void EPMDeviceDialog::resizeEvent
(
	QResizeEvent* event
)
{
	_resizeEvent = true;

	QDialog::resizeEvent(event);
}

void EPMDeviceDialog::setupDeviceList()
{
	const HashType kNoCurrentRow(0xFFFFFFFFFFFFFFFF);
	QTableWidgetItem* lwi;

	HashType currentHash{kNoCurrentRow};
	QTableWidgetItem* currentItem = _ui->_deviceList->currentItem();
	if (currentItem != Q_NULLPTR)
		currentHash = currentItem->data(kHashIndexRole).toULongLong();

	uint deviceCount = _EPMDevice::deviceCount();
	if (deviceCount == 0)
		_resizeEvent = true;

	_EPMDevice::updateDeviceList();

	deviceCount = _EPMDevice::deviceCount();
	if (deviceCount == 0 && _resizeEvent == true)
		_resizeEvent = false; // Don't reshrink columns if all devices have left.

	_ui->_deviceList->clearContents();
	_ui->_deviceList->setRowCount(deviceCount);

	for (auto row: basic_range<uint>(deviceCount))
	{
		auto epmDevice = _EPMDevice::device(row);

		if (epmDevice->programmed())
		{
			lwi = new QTableWidgetItem(epmDevice->getTargetName());
			lwi->setData(kHashIndexRole, epmDevice->hash());
			_ui->_deviceList->setItem(row, 0, lwi);

			lwi = new QTableWidgetItem(epmDevice->getSerialNumber());
			lwi->setData(kHashIndexRole, epmDevice->hash());
			_ui->_deviceList->setItem(row, 1, lwi);

			lwi = new QTableWidgetItem(epmDevice->getPlatformIDString());
			lwi->setData(kHashIndexRole, epmDevice->hash());
			_ui->_deviceList->setItem(row, 2, lwi);

			lwi = new QTableWidgetItem(epmDevice->getUUID());
			lwi->setData(kHashIndexRole, epmDevice->hash());
			_ui->_deviceList->setItem(row, 3, lwi);

			if (epmDevice->hash() == currentHash)
				_ui->_deviceList->selectRow(row);
		}
		else
		{
			QBrush brush{Qt::red};

			lwi = new QTableWidgetItem("Unprogrammed");
			lwi->setData(kHashIndexRole, epmDevice->hash());
			lwi->setForeground(brush);

			_ui->_deviceList->setItem(row, 0, lwi);

			lwi = new QTableWidgetItem("Unprogrammed");
			lwi->setData(kHashIndexRole, epmDevice->hash());
			lwi->setForeground(brush);
			_ui->_deviceList->setItem(row, 1, lwi);

			lwi = new QTableWidgetItem(epmDevice->getPlatformIDString());
			lwi->setData(kHashIndexRole, epmDevice->hash());
			lwi->setForeground(brush);
			_ui->_deviceList->setItem(row, 2, lwi);

			lwi = new QTableWidgetItem(epmDevice->getUUID());
			lwi->setData(kHashIndexRole, epmDevice->hash());
			lwi->setForeground(brush);
			_ui->_deviceList->setItem(row, 3, lwi);

			if (epmDevice->hash() == currentHash)
				_ui->_deviceList->selectRow(row);
		}
	}

	if (_resizeEvent == true)
	{
		auto columnWidth = _ui->_deviceList->width() - 2;

		_ui->_deviceList->resizeColumnToContents(0);
		columnWidth -= _ui->_deviceList->columnWidth(0);

		_ui->_deviceList->resizeColumnToContents(1);
		columnWidth -= _ui->_deviceList->columnWidth(1);

		_ui->_deviceList->resizeColumnToContents(2);
		columnWidth -= _ui->_deviceList->columnWidth(2);

		_ui->_deviceList->setColumnWidth(3, columnWidth);
		_resizeEvent = false;
	}
}

void EPMDeviceDialog::on__deviceList_currentRowChanged
(
	int currentRow
)
{
	emit deviceSelected(currentRow != -1 ? true : false);
}

void EPMDeviceDialog::on__probeDevicesButton_clicked()
{
	setupDeviceList();
}

void EPMDeviceDialog::on__timer_timeout()
{
	setupDeviceList();
}

void EPMDeviceDialog::on_DeviceDialogClass_accepted()
{

}
