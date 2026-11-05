// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMDeviceList.h"

#include "ui_EPMDeviceList.h"

// libEPM
#include "EPMDevice.h"

// QCommon
#include "Range.h"

const int kHashIndexRole(Qt::UserRole);
const int kProgrammedRole(Qt::UserRole + 1);

EPMDeviceList::EPMDeviceList
(
	QWidget* parent
) :
	QWidget(parent),
	_ui(new Ui::EPMDeviceListClass)
{
	_ui->setupUi(this);
	_ui->_setupDeviceMappingsButton->hide();

	connect(&_timer, &QTimer::timeout, this, &EPMDeviceList::on__timer_timeout);
}

EPMDeviceList::~EPMDeviceList()
{
}

void EPMDeviceList::addToLog
(
	const QString &logMe
)
{
	AppCore::writeToApplicationLog(logMe + "\n");
}

void EPMDeviceList::refresh()
{
	setupDeviceList();
}

void EPMDeviceList::showProbeButton(bool show)
{
	if (show)
	{
		_ui->_probeDevicesButton->show();
		_timer.stop();
	}
	else
	{
		_ui->_probeDevicesButton->hide();
		_timer.start(5000);
	}
}

EPMDevice EPMDeviceList::currentDevice()
{
	EPMDevice result;

	QListWidgetItem* lwi{Q_NULLPTR};

	auto selectedDevices = _ui->_deviceList->selectedItems();
	if (selectedDevices.isEmpty() == false)
	{
		lwi = selectedDevices.at(0);
	}
	else if (_ui->_deviceList->count() > 0)
	{
		lwi = _ui->_deviceList->item(0);
	}

	if (lwi != Q_NULLPTR)
	{
		HashType deviceHash = lwi->data(kHashIndexRole).toULongLong();
		result = _EPMDevice::deviceByHash(deviceHash);
	}

	return result;
}

bool EPMDeviceList::selectFirstDevice()
{
	bool result{false};

	// Row 0 can be the "in use by another application" info row,
	// which is not a device and must never be selected.
	if (_ui->_deviceList->count() && (_ui->_deviceList->item(0)->flags() & Qt::ItemIsSelectable))
	{
		_ui->_deviceList->setCurrentRow(0, QItemSelectionModel::Select);
		emit deviceSelected(true);
		result = true;
	}
	else
	{
		emit deviceSelected(false);
	}

	return result;
}

bool EPMDeviceList::selectDeviceByName
(
	const QString &deviceName
)
{
	bool result(false);

	QListWidgetItem* lwi;

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

	return result;
}

void EPMDeviceList::setupDeviceList()
{
	emit deviceScanStarted();
	QString logMessage;

	QListWidgetItem* lwi;
	QListWidgetItem* lwiSelected{Q_NULLPTR};
	HashType selectedHash{0};

	auto selectedItems = _ui->_deviceList->selectedItems();
	if (selectedItems.isEmpty() == false)
	{
		auto selectedItem = selectedItems.at(0);
		selectedHash = selectedItem->data(kHashIndexRole).toULongLong();

		QString logMessage = QString("EPMDeviceList::setupDeviceList: Selected Device is: %1").arg(selectedHash);

		addToLog(logMessage);
	}

	_ui->_deviceList->clear();

	_EPMDevice::updateDeviceList();

	uint deviceCount = _EPMDevice::deviceCount();
	for (auto deviceIndex: basic_range<uint>(deviceCount))
	{
		auto epmDevice = _EPMDevice::device(deviceIndex);

		logMessage = QString("EPMDeviceList::setupDeviceList: Adding Device: %1").arg(epmDevice->_targetInfo._serialNumber);
		addToLog(logMessage);

		if (epmDevice.isNull() == false)
		{
			bool programmed{epmDevice->programmed()};

			QString name;
			HashType deviceHash = epmDevice->hash();

			if (programmed)
			{
				logMessage = QString("   EPMDeviceList::setupDeviceList: Device is programmed");
				addToLog(logMessage);

				name = epmDevice->getTargetName();
				if (name.isEmpty())
					name = epmDevice->getSerialNumber();

				if (name.isEmpty())
					name = epmDevice->serialNumber();

				if (name.isEmpty())
					name = "Unnamed - Use EEPROM Utility";

				logMessage = QString("   EPMDeviceList::setupDeviceList: Name: %1").arg(name);
				addToLog(logMessage);
			}
			else
			{
				logMessage = QString("   EPMDeviceList::setupDeviceList: Device is unprogrammed");
				addToLog(logMessage);

				name = "Unprogrammed";
			}

			lwi = new QListWidgetItem(name);
			lwi->setData(kHashIndexRole, deviceHash);
			lwi->setData(kProgrammedRole, programmed);
			lwi->setForeground(programmed ? Qt::black : Qt::red);
			lwi->setToolTip(programmed ? "" : "Try and set this up in the EEPROM Utility");

			if (deviceHash == selectedHash)
			{
				if (programmed)
					lwiSelected = lwi;
			}

			_ui->_deviceList->addItem(lwi);

			logMessage = QString("   EPMDeviceList::setupDeviceList: Device added");
			addToLog(logMessage);
		}
	}

	// Boards seen on USB but held open by another application are not
	// listed as devices. Show one info row (not selectable) so the user knows why.
	const QStringList unavailable = _EPMDevice::unavailableDevices();
	if (unavailable.isEmpty() == false)
	{
		lwi = new QListWidgetItem(QString("%1 EPM device(s) in use by another application - "
			"close it, then click Probe for Devices").arg(unavailable.count()));
		lwi->setData(kHashIndexRole, QVariant::fromValue<HashType>(0));
		lwi->setData(kProgrammedRole, false);
		lwi->setFlags(Qt::ItemIsEnabled);
		lwi->setForeground(Qt::darkGray);
		lwi->setToolTip(QString("Found but could not be opened. Close the other application "
			"(for example EPM Scope), then click Probe for Devices.\n%1").arg(unavailable.join("\n")));
		_ui->_deviceList->addItem(lwi);

		addToLog(QString("   EPMDeviceList::setupDeviceList: Unavailable: %1").arg(unavailable.join("; ")));
	}

	if (lwiSelected != Q_NULLPTR)
		_ui->_deviceList->setCurrentItem(lwiSelected, QItemSelectionModel::Select);

	logMessage = QString("   EPMDeviceList::setupDeviceList: Devices Added: %1\n").arg(_ui->_deviceList->count());
	addToLog(logMessage);

	emit deviceCountUpdated(deviceCount);
}

void EPMDeviceList::on__deviceList_currentRowChanged
(
	int currentRow
)
{
	emit deviceSelected(currentRow != -1 ? true : false);
}

void EPMDeviceList::on__probeDevicesButton_clicked()
{
	setupDeviceList();
}

void EPMDeviceList::on__timer_timeout()
{
	setupDeviceList();
}
