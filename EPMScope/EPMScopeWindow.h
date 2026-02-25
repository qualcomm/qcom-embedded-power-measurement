#ifndef  EPMScopeWINDOW_H
#define  EPMScopeWINDOW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
