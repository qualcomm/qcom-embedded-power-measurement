#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ui_PreferencesDialog.h"

#include "EPMScopePreferences.h"

class PreferencesDialog :
	public QDialog,
	private Ui::PreferencesDialogClass
{
Q_OBJECT

public:
	PreferencesDialog(EPMScopePreferences* preferences, QWidget* parent = Q_NULLPTR);

signals:
	void preferencesChanged();

private slots:
	void on__platformPathButton_clicked();
	void on__runtimePathButton_clicked();
	void on__outputPathButton_clicked();
	void on__logPathButton_clicked();

	void on__setToDefaultButton_clicked();

	void on_accepted();

protected:
	EPMScopePreferences*		_preferences{Q_NULLPTR};

	void changeEvent(QEvent* e);
};

#endif // PREFERENCESDIALOG_H
