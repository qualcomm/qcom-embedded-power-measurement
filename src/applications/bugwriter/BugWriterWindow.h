// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef BUGWRITERWINDOW_H
#define BUGWRITERWINDOW_H

#include "ui_BugWriterWindow.h"
#include "IssueWriter.h"

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

	void onDeviceFlowStarted(const QString& userCode, const QString& verificationUri);
	void onAuthenticated(const QString& username);
	void onIssueSubmitted(int issueNumber, const QString& issueUrl);
	void onIssueError(const QString& message);

private:
	bool checkFields();
	QString buildMarkdownBody();

	bool					_listPorts{false};

	QString					_driverVersion;
	QString					_portData;
	QByteArrayList          _attachments;

	IssueWriter*            _issueWriter{Q_NULLPTR};

	QTimer					_timer;
};

#endif // BUGWRITERWINDOW_H
