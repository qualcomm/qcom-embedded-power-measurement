#ifndef BUGWRITERWINDOW_H
#define BUGWRITERWINDOW_H
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
// Copyright 2018-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "ui_BugWriterWindow.h"
#include "EmailWriter.h"

// Qt
#include <QMainWindow>
#include <QTimer>


class BugWriterWindow :
	public QMainWindow,
	public Ui::BugWriterWindow
{
Q_OBJECT

public:
	BugWriterWindow(QWidget* parent = Q_NULLPTR);
	~BugWriterWindow();

private slots:
	void on__quitButton_clicked();
	void on__saveAsTextFileButton_clicked();
	void on__submitButton_clicked();

	void on__authorName_textChanged(const QString &arg1);
	void on__authorPhone_editingFinished();

	void on_actionAbout_triggered();

	void on__attachmentBtn_clicked();

	void on_actionQuit_triggered();

	void onTimerTimeout();

private:
	bool checkFields();
	QString buildBody();

	bool						_listPorts{false};

	QString						_driverVersion;
	QString						_portData;
	QByteArrayList              _attachments;

	EmailWriter*                _emailWriter{Q_NULLPTR};

	QTimer						_timer;
};

#endif // BUGWRITERWINDOW_H

