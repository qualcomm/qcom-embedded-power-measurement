// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "EPMDeviceModel.h"

// QCommonConsole
#include "ApplicationEnhancements.h"
#include "DevicePropertiesDialog.h"
#include "Range.h"

// PowerChart
#include "AlpacaChartLive.h"

// Qt
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>


const QByteArray kDefaultPropsTip(QByteArrayLiteral("Scan & select device to view properties"));
const QByteArray kDefaultDeviceSelection(QByteArrayLiteral("<scan to update list>"));
const QByteArray kDefaultPlatformSelection(QByteArrayLiteral("<choose configuration>"));
const QByteArray kModelId(QByteArrayLiteral("Id"));
const int kDefaultDeviceIndex(0);


EPMDeviceModel::EPMDeviceModel(EPMScopePreferences *preferences)
{
	_propsIcon.addFile(QString::fromUtf8(":/Properties.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);

	_preferences = preferences;

	_epmProject = EPMProject(new _EPMProject(_preferences));
	_epmProject->setPlatformPath(_preferences->platformPath());
}

void EPMDeviceModel::setCurrentChart(CurrentChartLive *currChart)
{
	_currentChart = currChart;
}

void EPMDeviceModel::setVoltageChart(VoltageChartLive *voltChart)
{
	_voltageChart = voltChart;
}

void EPMDeviceModel::updateDeviceModel()
{
	removeAllDevices();

	_EPMDevice::updateDeviceList();
	quint32 devicecount = _EPMDevice::deviceCount();

	for (auto deviceIndex: range(devicecount))
	{
		EPMDevice epmDevice = _EPMDevice::device(deviceIndex);
		addDevice(epmDevice);
	}

	for (auto& hbl : _layoutMap.keys())
	{
		int cnt = hbl->count();

		if (cnt > 2)
		{
			QLayoutItem* li = hbl->itemAt(1);

			if (li != Q_NULLPTR)
			{
				QComboBox* cb = qobject_cast<QComboBox*>(li->widget());

				cb->blockSignals(true);

				cb->clear();
				cb->addItem(kDefaultDeviceSelection);
				cb->setCurrentIndex(kDefaultDeviceIndex);

				for (auto& epmDevice: _epmDevices)
					cb->addItem(epmDevice->getSerialNumber());

				cb->blockSignals(false);

				li = hbl->itemAt(0);

				if (li != Q_NULLPTR)
				{
					QPushButton* pb = qobject_cast<QPushButton*>(li->widget());

					if (pb != Q_NULLPTR)
						pb->setEnabled(false);
				}
			}
		}
	}
}

quint8 EPMDeviceModel::deviceCount()
{
	return _epmDevices.count();
}

EPMDevices EPMDeviceModel::availableDevices()
{
	EPMDevices result;

	for (auto& epmDevice : _epmDevices)
	{
		if (_openDevices.contains(epmDevice) == false)
			result.append(epmDevice);
	}

	return result;
}

EPMDevices EPMDeviceModel::openedDevices()
{
	return _openDevices;
}

void EPMDeviceModel::addDevice(EPMDevice epmDevice)
{
	if (epmDevice.isNull() == false)
	{
		if (_epmDevices.contains(epmDevice) == false)
			_epmDevices.append(epmDevice);
	}
}

void EPMDeviceModel::removeDevice(EPMDevice epmDevice)
{
	if (epmDevice.isNull() == false)
	{
		if (_epmDevices.contains(epmDevice) == true)
		{
			if (epmDevice->_connected == true)
				epmDevice->close();

			_epmDevices.removeAll(epmDevice);
		}
	}
}

void EPMDeviceModel::closeDevice(EPMDevice epmDevice)
{
	if (epmDevice.isNull() == false)
	{
		if (_openDevices.contains(epmDevice) == true)
		{
			if (epmDevice->_connected == true)
				epmDevice->close();

			_openDevices.removeAll(epmDevice);
		}
	}
}

void EPMDeviceModel::removeAllDevices()
{
	for (auto& epmDevice : _epmDevices)
	{
		if (epmDevice->_connected == true)
			epmDevice->close();
	}

	_epmDevices.clear();
	_openDevices.clear();
}

QHBoxLayout* EPMDeviceModel::createLayout(QGroupBox* parent)
{
	QHBoxLayout* deviceLayout = new QHBoxLayout();

	QPushButton* propsBtn = new QPushButton(parent);
	QComboBox* deviceComboBox = new QComboBox(parent);
	QComboBox* platformComboBox = new QComboBox(parent);

	QSizePolicy propBtnSize(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

	propsBtn->setSizePolicy(propBtnSize);
	propsBtn->setIcon(_propsIcon);

	propsBtn->setToolTip(kDefaultPropsTip);
	propsBtn->setEnabled(false);

	_counter++;

	// Add platforms
	platformComboBox->addItem(kDefaultPlatformSelection);
	quint32 platformCount =	_epmProject->getPlatformCount();
	for (const auto& platformIndex: range(platformCount))
	{
		QString platformName, platformPath;

		bool status = _epmProject->getPlatform(platformIndex, platformName, platformPath);

		if (status == true)
			platformComboBox->addItem(platformName, platformPath);
	}

	// Add devices
	deviceComboBox->addItem(kDefaultDeviceSelection);
	for (auto& epmDevice: _epmDevices)
	{
		if (_openDevices.contains(epmDevice) == false)
			deviceComboBox->addItem(epmDevice->getSerialNumber());
	}

	deviceLayout->addWidget(propsBtn);
	deviceLayout->addWidget(deviceComboBox);
	deviceLayout->addWidget(platformComboBox);

	deviceLayout->setProperty(kModelId, _counter);
	propsBtn->setProperty(kModelId, _counter);
	deviceComboBox->setProperty(kModelId, _counter);
	platformComboBox->setProperty(kModelId, _counter);

	connect(propsBtn, &QPushButton::clicked, this, &EPMDeviceModel::onPropsBtnClicked);
	connect(deviceComboBox, &QComboBox::currentIndexChanged, this, &EPMDeviceModel::onDeviceIndexChanged);
	connect(platformComboBox, &QComboBox::currentIndexChanged, this, &EPMDeviceModel::onPlatformIndexChanged);

	return deviceLayout;
}

QWidget* EPMDeviceModel::createTable(QTabWidget *parent)
{
	QWidget* wgt = new QWidget();

	if (wgt != Q_NULLPTR)
	{
		wgt->setProperty(kModelId, _counter);

		QHBoxLayout* hl = new QHBoxLayout(wgt);

		EPMChannelTable* ct = new EPMChannelTable(wgt);
		ct->setColumns(EPMChannelTable::TypeColumnVisible | EPMChannelTable::CurrentColumnVisible | EPMChannelTable::VoltageColumnVisible);

		EPMProject epmProject = EPMProject(new _EPMProject(_preferences));
		epmProject->setPlatformPath(_preferences->platformPath());

		epmProject->getPlatformCount();

		ct->setEPMProject(epmProject);
		setTableWidget(wgt, epmProject);

		hl->addWidget(ct);
	}

	return wgt;
}

void EPMDeviceModel::openDevice(EPMDevice epmDevice)
{
	// Re-check if device has been opened else-where
	_EPMDevice::updateDeviceList();

	/*
	 * 1. Open an un-opened device
	 * 2. Open a device that's already open
	 *		a. Opened device has layout -- block open
	 *		b. Opened device does not have layout -- pass thru
	 */

	QString serialNumber = epmDevice->getSerialNumber();
	EPMDevice device = _EPMDevice::deviceByName(serialNumber);

	QString connStatus = device->connectionStatus();

	AppCore::writeToApplicationLogLine("Open status for device with serial number " + serialNumber + ": " + connStatus);

	if (connStatus.isEmpty() == true)
	{
		if (_openDevices.contains(epmDevice) == true)
		{
			EPMDevices devices = _layoutMap.values();

			if (devices.contains(epmDevice) == true)
			{
				QMessageBox::critical(_currentChart->window(), "EPM device already selected", "You're trying to add a device that is already added. "
										"Please review your selection and try again.");

				emit scanDevices();
			}
		}
		else if (_epmDevices.contains(epmDevice) == true)
		{
			try
			{
				epmDevice->open();

				if (epmDevice->_connected == true)
					_openDevices.append(epmDevice);
			}
			catch(EPMException& e)
			{
				QMessageBox::critical(_currentChart->window(), "EPM device is not available", "The EPM device you're trying to connect to is "
																		   "disconnected or in use. Please reconnect the EPM device to the host and scan for devices. Error: " + QString(e.what()));

				emit scanDevices();
			}
		}
	}
	else
	{
		QMessageBox::critical(_currentChart->window(), "EPM device is not available", "The EPM device you're trying to connect to is "
								"disconnected or in use. Please reconnect the EPM device to the host and scan for devices. Error: " + connStatus);

		emit scanDevices();
	}
}

int EPMDeviceModel::count()
{
	int lsize = _layoutMap.size();
	int tsize = _tableMap.size();

	if (lsize != tsize)
		AppCore::writeToApplicationLogLine("Layout and table counts are out of sync. Please file bug report using the Bug Writer");

	return _layoutMap.size();
}

QHBoxLayout* EPMDeviceModel::layout(QGroupBox* parent, EPMDevice epmDevice)
{
	Q_ASSERT(parent != Q_NULLPTR);

	QHBoxLayout* hbl = createLayout(parent);
	_layoutMap.insert(hbl, Q_NULLPTR);

	return hbl;
}

void EPMDeviceModel::setLayout(QHBoxLayout *hbl, EPMDevice epmDevice)
{
	Q_ASSERT(hbl != Q_NULLPTR);

	if (epmDevice != Q_NULLPTR)
	{
		_layoutMap[hbl] = epmDevice;

		for (int idx : range(hbl->count()))
		{
			QLayoutItem* li = hbl->itemAt(idx);
			if (li != Q_NULLPTR)
			{
				QWidget* w = li->widget();
				if (w != Q_NULLPTR)
				{
					QPushButton* btn = qobject_cast<QPushButton*>(w);
					if (btn != Q_NULLPTR)
					{
						btn->setEnabled(true);
						break;
					}
				}
			}
		}
	}
}

void EPMDeviceModel::removeLayout(QHBoxLayout *hbl)
{
	if (_layoutMap.contains(hbl) == true)
		_layoutMap.remove(hbl);

	_counter--;
}

EPMDevice EPMDeviceModel::getDeviceForLayout(QHBoxLayout *hbl)
{
	if (_layoutMap.contains(hbl))
		return _layoutMap.value(hbl);
	
	return Q_NULLPTR;
}

QWidget* EPMDeviceModel::tableWidget(QTabWidget *parent, EPMDevice epmDevice)
{
	Q_ASSERT(parent != Q_NULLPTR);

	return createTable(parent);
}

void EPMDeviceModel::setTableWidget(QWidget *wgt, EPMProject epmProject)
{
	Q_ASSERT(wgt != Q_NULLPTR);
	_tableMap[wgt] = epmProject;
}

void EPMDeviceModel::removeTableWidget(QWidget *wgt)
{
	if (_tableMap.contains(wgt) == true)
		_tableMap.remove(wgt);
}

void EPMDeviceModel::record(MicroEpmChannelData* channelData, quint32 sampleCount)
{
	if (sampleCount > 0)
	{
		MicroEpmChannelData* pSample;

		for (auto uSampleIdx: range(sampleCount))
		{
			pSample = &channelData[uSampleIdx];

			if (std::isinf(pSample->_physicalValue) == false)
			{
				double value = pSample->_physicalValue;

				if (pSample->_channelType == MICRO_EPM_CHANNEL_TYPE_CURRENT ||
					pSample->_channelType == MICRO_EPM_CHANNEL_TYPE_VOLTAGE)
				{
					AlpacaChartLive* alpacaChartLive{Q_NULLPTR};

					switch (pSample->_channelType)
					{
					case MICRO_EPM_CHANNEL_TYPE_CURRENT:
						alpacaChartLive = _currentChart;
						break;

					case MICRO_EPM_CHANNEL_TYPE_VOLTAGE:
						alpacaChartLive = _voltageChart;
						value /= 1000.0;
						break;

					default:
						break;
					}

					if (alpacaChartLive != Q_NULLPTR)
					{
						quint32 channel = pSample->_channel;

						if (channel < MAX_NUM_ADC_CHANNELS)
							alpacaChartLive->logSample(channel, value, pSample->_timeStamp);
					}
				}
			}
		}
	}
}

void EPMDeviceModel::startRecord()
{
	/*
	 * 1. Check if valid request
	 * 2. Set device in epmproject
	 * 3. Set recording interface in device
	 */

	bool validRequest{false};

	_recording = !_recording;

	for (auto& wgt : _tableMap.keys())
	{
		int wgtId = wgt->property(kModelId).toInt();
		EPMProject epmProject = _tableMap.value(wgt);
		EPMChannelTable* cht = wgt->findChild<EPMChannelTable*>();


		AppCore::writeToApplicationLog(QString("Starting Acquisition: %1\n").arg(epmProject->target()));
		AppCore::writeToApplicationLog(QString("   ") + QDateTime::currentDateTime().toString() + "\n");
		AppCore::writeToApplicationLog(QString("                       Label: %1\n").arg(epmProject->label()));
		AppCore::writeToApplicationLog(QString("                 Description: %1\n").arg(epmProject->description()));
		AppCore::writeToApplicationLog(QString("   Config File Channel Count: %1\n").arg(epmProject->channelCount()));

		QString targetFile = epmProject->target() + createFilenameTimeStamp();

		epmProject->setTemporaryResultsDirectory(_preferences->defaultOutputPath() + QDir::separator() + targetFile);

		if (_recording)
		{
			if (cht->checked(EPMChannelTable::eCurrentColumn) == true)
				validRequest = true;

			else if (cht->checked(EPMChannelTable::eVoltageColumn) == true)
				validRequest = true;

			emit recordState(_recording);

			for (auto& hbl : _layoutMap.keys())
			{
				int hblId = hbl->property(kModelId).toInt();
				if (hblId == wgtId)
				{
					EPMDevice device = _layoutMap.value(hbl);
					if (device.isNull() == false)
					{
						for (auto& wgt : _tableMap.keys())
						{
							EPMProject epmProject = _tableMap.value(wgt);
							if (epmProject.isNull() == false)
							{
								quint32 channelCount = epmProject->channelCount();
								for (auto channelIndex : range(channelCount))
								{
									EPMChannel currentChannel;
									EPMChannel voltageChannel;
									epmProject->getChannelPair(channelIndex, currentChannel, voltageChannel);

									if (currentChannel.isNull() == false && currentChannel->active())
										_currentChart->addLineSeries(currentChannel);

									if (voltageChannel.isNull() == false && voltageChannel->active())
										_voltageChart->addLineSeries(voltageChannel);
								}
							}
						}

						try
						{
							if (_tableMap.count() > 0)
							{
								epmProject->setEPMDevice(device);
								epmProject->startAcquire();
							}
							else
							{
								QMessageBox::information(_currentChart->window(), "Invalid Selection", "At least one rail must be enabled.  Check a current or voltage checkbox and then click record.");
								_recording = false;

								emit recordState(_recording);
							}
						}
						catch (EPMException& epmException)
						{
							AppCore::writeToApplicationLog(QString("EPMException epmException: %1\n").arg(epmException.what()));
						}

						break;
					}
				}
			}
		}
	}

	if (validRequest == false)
	{
		QMessageBox::information(_currentChart->window(), "Invalid Selection", "At least one rail must be enabled.  Check a current or voltage checkbox and then click record.");
		_recording = false;

		emit recordState(_recording);
	}
}

void EPMDeviceModel::stopRecord()
{
	if (_recording)
	{
		_recording = false;
		emit recordState(_recording);

		for (auto& wgt : _tableMap.keys())
		{
			int wgtId = wgt->property(kModelId).toInt();
			EPMProject epmProject = _tableMap.value(wgt);
			EPMChannelTable* cht = wgt->findChild<EPMChannelTable*>();

			for (auto& hbl : _layoutMap.keys())
			{
				int hblId = hbl->property(kModelId).toInt();
				if (hblId == wgtId)
				{
					EPMDevice device = _layoutMap.value(hbl);
					if (device.isNull() == false)
					{
						epmProject->stopAcquire();
						AppCore::writeToApplicationLog(QString("   Acquisition Finished: %1\n").arg(epmProject->target()));
					}
				}
			}

			if (cht != Q_NULLPTR)
				cht->setEnabled(true);
		}

		AppCore::writeToApplicationLog(QString("               End Time: %1\n") + QDateTime::currentDateTime().toString());

		AppCore::getAppCore()->setRunLogging(false);
	}
}

void EPMDeviceModel::onPropsBtnClicked()
{
	QPushButton* btn = qobject_cast<QPushButton*>(sender());
	if (btn != Q_NULLPTR)
	{
		int btnId = btn->property(kModelId).toInt();

		for (auto& hbl : _layoutMap.keys())
		{
			int hblId = hbl->property(kModelId).toInt();
			if (btnId == hblId)
			{
				EPMDevice device = _layoutMap.value(hbl);

				if (device != Q_NULLPTR)
				{
					DevicePropertiesDialog dpd(btn->window());
					dpd.setDevice(device);

					dpd.exec();
				}
				break;
			}
		}
	}
}

void EPMDeviceModel::onDeviceIndexChanged(int index)
{	
	QComboBox* cb = qobject_cast<QComboBox*>(sender());

	if (cb != Q_NULLPTR)
	{
		int cbId = cb->property(kModelId).toInt();
		QString serialNum = cb->currentText();

		for (auto& hbl : _layoutMap.keys())
		{
			int hblId = hbl->property(kModelId).toInt();

			if (cbId == hblId)
			{
				int cnt = hbl->count();
				if (cnt > 0)
				{
					QLayoutItem* li = hbl->itemAt(0);

					QPushButton* btn = qobject_cast<QPushButton*>(li->widget());

					if (btn != Q_NULLPTR)
					{
						if (index > kDefaultDeviceIndex)
						{
							btn->setEnabled(true);

							EPMDevice device = _EPMDevice::deviceByName(serialNum);
							openDevice(device);

							setLayout(hbl, device);
						}
						else
						{
							btn->setEnabled(false);

							closeDevice(_layoutMap.value(hbl));
							setLayout(hbl, NULL);
						}
					}
				}
				break;
			}
		}			
	}
}

void EPMDeviceModel::onPlatformIndexChanged(int index)
{
	QComboBox* cb = qobject_cast<QComboBox*>(sender());

	if (cb != Q_NULLPTR)
	{
		if (index > 0)
		{
			QString absoluteFilePath = cb->currentData().toString();

			int cbId = cb->property(kModelId).toInt();

			for (auto& wgt : _tableMap.keys())
			{
				int wgtId = wgt->property(kModelId).toInt();

				if (wgtId == cbId)
				{
					EPMProject epmProject = _tableMap.value(wgt);
					bool status = loadPlatformFile(epmProject, absoluteFilePath);

					if (status == true)
					{
						EPMChannelTable* ct = wgt->findChild<EPMChannelTable*>();
						if (ct != Q_NULLPTR)
						{
							ct->clearContents();
							ct->populateChannels();
						}
					}

					break;
				}
			}
		}
		else
		{
			int cbId = cb->property(kModelId).toInt();

			for (auto& wgt : _tableMap.keys())
			{
				int wgtId = wgt->property(kModelId).toInt();

				if (wgtId == cbId)
				{

					EPMChannelTable* ct = wgt->findChild<EPMChannelTable*>();
					if (ct != Q_NULLPTR)
						ct->clearContents();

					break;
				}
			}
		}
	}
}

bool  EPMDeviceModel::loadPlatformFile(EPMProject epmProject, const QString& platformFile)
{
	bool result{false};

	if (epmProject->loadPlatformConfigFile(platformFile) == true)
	{
		quint32 channelCount = epmProject->channelCount();

		// for scope, set the check boxes to an initial value of off
		for (auto channelIndex: range(channelCount))
		{
			EPMChannel currentChannel;
			EPMChannel voltageChannel;

			epmProject->getChannelPair(channelIndex, currentChannel, voltageChannel);

			if (currentChannel.isNull() == false)
				currentChannel->setActive(false);

			if (voltageChannel.isNull() == false)
				voltageChannel->setActive(false);
		}

		result = true;
	}

	return result;
}

