#ifndef BUGWRITERWINDOW_H
#define BUGWRITERWINDOW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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

