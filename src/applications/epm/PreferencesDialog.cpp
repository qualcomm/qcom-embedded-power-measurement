// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
