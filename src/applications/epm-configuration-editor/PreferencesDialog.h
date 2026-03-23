#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ui_PreferencesDialog.h"

class PreferencesDialog :
	public QDialog,
	private Ui::PreferencesDialogClass
{
Q_OBJECT

public:
	PreferencesDialog(QWidget* parent = Q_NULLPTR);

signals:
	void preferencesChanged(const QString& preferenceGroup);

private slots:
	void on__setToDefaultsButton_clicked();
	void on__browseForLogPathButton_clicked();
	void on_accepted();

protected:
	void changeEvent(QEvent* e);
};

#endif // PREFERENCESDIALOG_H
