#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Author: msimpson

#include "ui_PreferencesDialog.h"

#include "EPMPreferences.h"

class PreferencesDialog :
	public QDialog,
	private Ui::PreferencesDialogClass
{
Q_OBJECT

public:
	PreferencesDialog(EPMPreferences* preference, QWidget* parent = Q_NULLPTR);

signals:
	void preferencesChanged();

private slots:
	void on__platformPathButton_clicked();
	void on__runtimeConfigPathButton_clicked();
	void on__outputPathButton_clicked();
	void on__logPathButton_clicked();

	void on__setToDefaultButton_clicked();

	void on_accepted();

protected:

	void changeEvent(QEvent* e);

private:
	EPMPreferences*				_preferences{Q_NULLPTR};
};

#endif // PREFERENCESDIALOG_H
