#ifndef  EPMScopeWINDOW_H
#define  EPMScopeWINDOW_H
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
// Copyright 2020-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_EPMScopeWindow.h"
#include "EPMDeviceModel.h"

// libEPM
#include "RecordingInferface.h"

// Qt
#include <QColor>
#include <QElapsedTimer>
#include <QMap>
#include <QTimer>


class EPMScopeWindow :
	public QMainWindow,
	public RecordingInterface,
	private Ui::EPMScopeWindowClass
{
	Q_OBJECT

public:
	EPMScopeWindow(EPMScopePreferences* preferences, QWidget* parent = Q_NULLPTR);
	~EPMScopeWindow();

	void shutDown();

	// Recording Interface
	void startRecording();
	void stopRecording();
	void recordData(MicroEpmChannelData* channelData, quint32 sampleCount);

protected:
	void closeEvent(QCloseEvent* event);
	void changeEvent(QEvent* e);

private slots:
	void on__recordBtn_clicked();
	void on_plotTimerTimeout();

	void on__actionAbout_triggered();
	void on__actionContents_triggered();
	void on__actionQuit_triggered();
	void on__actionRateMe_triggered();
	void on__actionSubmitBugReport_triggered();

	void on__scanDevices_clicked();
	void on__addDeviceBtn_clicked();
	void on__removeDeviceBtn_clicked();

	void on__settingsBtn_clicked();
	void on__actionPreferences_triggered();

private:
	EPMDeviceModel				_model;
	EPMScopePreferences*		_preferences;

	QString						_lastError;
	bool						_recording{false};

	QIcon						_recordIcon;
	QIcon						_stopIcon;

	QElapsedTimer				_elapsed;
	QTimer*						_plotUpdateTimer{Q_NULLPTR};

	void setupActiveCharts();
	void setupRecordState(bool status);
};

#endif //  EPMScopeWINDOW_H
