#ifndef EPMDEVICEMODEL_H
#define EPMDEVICEMODEL_H

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
