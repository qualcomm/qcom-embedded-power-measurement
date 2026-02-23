#ifndef DEVICECATALOG_H
#define DEVICECATALOG_H
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
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
			Arkojit Sen (arkosen@qti.qualcomm.com)
*/
#include "ui_DeviceCatalog.h"

// QCommon
#include "AlpacaDevice.h"
#include "MediaPlaybackFrame.h"


class DeviceCatalog :
	public QDialog,
	private Ui::DeviceCatalog
{
	Q_OBJECT

public:
	DeviceCatalog(QWidget* parent = Q_NULLPTR);
	~DeviceCatalog();

private slots:
	void onConfigurationLinkClicked(QTableWidgetItem* twi);
	void onCustomContextMenuRequested(const QPoint& pos);
	void on__deviceTable_itemClicked(QTableWidgetItem* item);
	void on__programBtn_clicked();
	void on__firmwareUpdateBtn_clicked();
	void on__docsBtn_clicked();
	void on__firmwareSelect_currentTextChanged(const QString &firmwareVersion);

	void onInfoGroupCloseBtnClicked();
	void onInfoGroupLinkClicked(const QString& link);

private:
	AlpacaDevices enumerateDevices(const DebugBoardType type);
	void deviceSelectionDialog(const DebugBoardType type);
	void processProgramming(const DebugBoardType& type, const QPoint& pos, int row);
	void invokeProgrammer(const DebugBoardType type);
	void invokeLiteProgrammer(const QString& serialNumber, const PlatformID platformId);
	void invokePSOCProgrammer(const QString& serialNumber, const PlatformID platformId);

	QByteArray					_firmwareDir;
	QString						_currentSerialNumber;
	PlatformID					_currentPlatformId;

	MediaPlaybackFrame			_player;
};

#endif // DEVICECATALOG_H
