#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ui_PreferencesDialog.h"

#include "UDASPreferences.h"

class PreferencesDialog :
	public QDialog,
	private Ui::PreferencesDialogClass
{
Q_OBJECT

public:
	PreferencesDialog(UDASPreferences* preferences, QWidget* parent = Q_NULLPTR);

signals:
	void preferencesChanged();

private slots:
	void on__setToDefaultsButton_clicked();
	void on__browseForLogPathButton_clicked();
	void on_accepted();

	void on__browseForExportPathButton_clicked();

protected:
	UDASPreferences*		_preferences{Q_NULLPTR};

	void changeEvent(QEvent* e);
};

#endif // PREFERENCESDIALOG_H
