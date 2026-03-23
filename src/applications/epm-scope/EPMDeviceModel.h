#ifndef EPMDEVICEMODEL_H
#define EPMDEVICEMODEL_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMScopePreferences.h"

// EPMLib
#include "EPMDevice.h"
#include "EPMProject.h"

// PowerChart
#include "CurrentChartLive.h"
#include "VoltageChartLive.h"

// UILib
#include <EPMChannelTable.h>

// Qt
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QTabWidget>

const QByteArray kTabIndex("TabIndex");

class EPMDeviceModel : public QObject
{
	Q_OBJECT

public:
	EPMDeviceModel(EPMScopePreferences* preferences = Q_NULLPTR);

	void setCurrentChart(CurrentChartLive* currChart);
	void setVoltageChart(VoltageChartLive* voltChart);

	void updateDeviceModel();
	quint8 deviceCount();

	EPMDevices availableDevices();

	EPMDevices openedDevices();
	void openDevice(EPMDevice epmDevice);

	int count();

	QHBoxLayout* layout(QGroupBox* parent, EPMDevice epmDevice = NULL);
	void setLayout(QHBoxLayout* hbl, EPMDevice epmDevice);
	void removeLayout(QHBoxLayout* hbl);
	EPMDevice getDeviceForLayout(QHBoxLayout* hbl);

	QWidget* tableWidget(QTabWidget* parent, EPMDevice epmDevice = NULL);
	void setTableWidget(QWidget* wgt, EPMProject epmProject);
	void removeTableWidget(QWidget* wgt);

	void record(MicroEpmChannelData *channelData, quint32 sampleCount);

	void startRecord();
	void stopRecord();

signals:
	void scanDevices();
	void recordState(bool state);

private slots:
	void onPropsBtnClicked();
	void onDeviceIndexChanged(int index);
	void onPlatformIndexChanged(int index);

private:
	void addDevice(EPMDevice epmDevice);
	void removeDevice(EPMDevice epmDevice);
	void closeDevice(EPMDevice epmDevice);
	void removeAllDevices();

	bool loadPlatformFile(EPMProject epmProject, const QString& platformFile);

	QHBoxLayout* createLayout(QGroupBox *parent);
	QWidget* createTable(QTabWidget *parent);

	EPMDevices									_openDevices{Q_NULLPTR};
	EPMDevices									_epmDevices;

	EPMProject									_epmProject{Q_NULLPTR};
	EPMScopePreferences*						_preferences;

	QMap<QHBoxLayout*, EPMDevice>				_layoutMap;
	QMap<QWidget*, EPMProject>					_tableMap;

	QIcon										_propsIcon;

	int											_counter{0};
	bool										_recording{false};

	CurrentChartLive*							_currentChart{Q_NULLPTR};
	VoltageChartLive*							_voltageChart{Q_NULLPTR};
};

#endif // EPMDEVICEMODEL_H
