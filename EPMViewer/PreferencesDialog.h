#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H
// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

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
