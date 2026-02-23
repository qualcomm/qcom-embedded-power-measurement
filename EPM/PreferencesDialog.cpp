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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PreferencesDialog.h"

// EPM
#include "EPMPreferences.h"

// QCommon
#include "ApplicationEnhancements.h"

// Qt
#include <QFileDialog>

PreferencesDialog::PreferencesDialog
(
	EPMPreferences*	preferences,
	QWidget* parent
) :
	QDialog(parent),
	_preferences(preferences)
{
	setupUi(this);

	_loggingCB->setChecked(_preferences->loggingActive());
	_logPathLabel->setText(_preferences->appLogPath());
	_platformPathLabel->setText(_preferences->platformPath());
	_runtimeConfPathLabel->setText(_preferences->runtimeConfigurationsPath());
	_outputPathLabel->setText(_preferences->outputPath());

	connect(this, &QDialog::accepted, this, &PreferencesDialog::on_accepted);
}

void PreferencesDialog::on__platformPathButton_clicked()
{
	QString currentPath = _platformPathLabel->text();
	QString platformPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Select Platform Config File Path"), currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (platformPath.isEmpty() == false && platformPath != currentPath)
	{
		_platformPathLabel->setText(QDir::cleanPath(platformPath));
	}
}

void PreferencesDialog::on__runtimeConfigPathButton_clicked()
{
	QString currentPath = _runtimeConfPathLabel->text();
	QString platformPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Select Runtime Config File Path"), currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (platformPath.isEmpty() == false && platformPath != currentPath)
	{
		_runtimeConfPathLabel->setText(QDir::cleanPath(platformPath));
	}
}

void PreferencesDialog::on__outputPathButton_clicked()
{
	QString currentPath = _outputPathLabel->text();
	QString outputPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Select Output Directory Path"), currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (outputPath.isEmpty() == false && outputPath != currentPath)
	{
		_outputPathLabel->setText(QDir::cleanPath(outputPath));
	}
}

void PreferencesDialog::on__logPathButton_clicked()
{
	QString currentPath = _logPathLabel->text();
	QString outputPath = QFileDialog::getExistingDirectory(this, QStringLiteral("Select Log Directory Path"), currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (outputPath.isEmpty() == false && outputPath != currentPath)
	{
		_logPathLabel->setText(QDir::cleanPath(outputPath));
	}
}

void PreferencesDialog::on__setToDefaultButton_clicked()
{
	_platformPathLabel->setText(_preferences->defaultPlatformPath());
	_runtimeConfPathLabel->setText(_preferences->runtimeConfigurationsPath());
	_outputPathLabel->setText(_preferences->defaultOutputPath());
	_loggingCB->setChecked(_preferences->defaultLoggingState());
	_logPathLabel->setText(_preferences->defaultAppLogPath());
}

void PreferencesDialog::on_accepted()
{
	_preferences->savePlatformPath(_platformPathLabel->text());
	_preferences->saveRuntimeConfigurationsPath(_runtimeConfPathLabel->text());
	_preferences->saveOutputPath( _outputPathLabel->text());
	_preferences->saveLoggingActive(_loggingCB->isChecked());
	_preferences->saveAppLogPath(_logPathLabel->text());

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
