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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PreferencesDialog.h"

// EPM Viewer
#include "EPMViewerApplication.h"

// MikesExcel
#ifdef Q_OS_WINDOWS
    #include "QTExcel.h"
#endif

// Qt
#include <QFileDialog>

PreferencesDialog::PreferencesDialog
(
	UDASPreferences* preferences,
	QWidget* parent
) :
	QDialog(parent),
	_preferences(preferences)
{
	setupUi(this);

	connect(this, &PreferencesDialog::preferencesChanged, EPMViewerApplication::appInstance(), &EPMViewerApplication::on_preferencesChanged);

	_loggingCB->setChecked(_preferences->loggingActive());
	_logLocation->setText(_preferences->appLogPath());
	_exportActiveItems->setChecked(_preferences->exportSelectedItems());
	_useCSVCheckbox->setChecked(preferences->useCSV());
	_useATimeSpanCheckBox->setChecked(preferences->useTimespan());

#ifdef Q_OS_WINDOWS
	if (QTExcel::excelAvailable())
	{
		_exitExcelCB->setChecked(_preferences->quitExcelOnFinish());
	}
	else
	{
		_exitExcelCB->setChecked(false);
		_exitExcelCB->setEnabled(false);
		_exitExcelCB->setToolTip("Excel isn't Available");
	}
#endif

#ifdef Q_OS_LINUX
    _exitExcelCB->setChecked(false);
    _exitExcelCB->setEnabled(false);
    _exitExcelCB->setToolTip("Excel isn't Available");
#endif

	_exportLocation->setText(_preferences->exportLocation());

	connect(this, &QDialog::accepted, this, &PreferencesDialog::on_accepted);
}

void PreferencesDialog::on__setToDefaultsButton_clicked()
{
	_loggingCB->setChecked(_preferences->defaultLoggingState());
	_logLocation->setText(_preferences->defaultAppLogPath());
	_exportActiveItems->setChecked(_preferences->defaultExportSelectedItems());
	_exitExcelCB->setChecked(_preferences->defaultQuitExcelOnFinish());
	_exportLocation->setText(_preferences->defaultExportLocation());
	_useCSVCheckbox->setChecked(_preferences->defaultUseCSV());
	_useATimeSpanCheckBox->setChecked(_preferences->defaultUseTimespan());

	on_accepted();
}

void PreferencesDialog::on__browseForLogPathButton_clicked()
{
	QString currentDir = QDir::cleanPath(_logLocation->text());
	QString newLogLocation = QDir::cleanPath(QFileDialog::getExistingDirectory(this, "Select logs directory", currentDir));
	if (newLogLocation.isEmpty() == false)
	{
		if (newLogLocation.toLower() != currentDir.toLower())
		{
			_logLocation->setText(newLogLocation);
		}
	}
}

void PreferencesDialog::on_accepted()
{
	_preferences->saveLoggingActive(_loggingCB->isChecked());
	_preferences->saveAppLogPath(_logLocation->text());
	_preferences->saveExportSelectedItems(_exportActiveItems->isChecked());
	_preferences->saveQuitExcelOnFinish(_exitExcelCB->isChecked());
	_preferences->saveExportLocation(_exportLocation->text());
	_preferences->saveUseCSV(_useCSVCheckbox->isChecked());
	_preferences->saveUseTimespan(_useATimeSpanCheckBox->isChecked());

	emit preferencesChanged();
}

void PreferencesDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;

	default:
		break;
	}
}

void PreferencesDialog::on__browseForExportPathButton_clicked()
{
	QString currentDir = QDir::cleanPath(_exportLocation->text());
	QString newExportLocation = QDir::cleanPath(QFileDialog::getExistingDirectory(this, "Select Export Directory", currentDir));
	if (newExportLocation.isEmpty() == false)
	{
		if (newExportLocation.toLower() != currentDir.toLower())
		{
			_exportLocation->setText(newExportLocation);
		}
	}
}
