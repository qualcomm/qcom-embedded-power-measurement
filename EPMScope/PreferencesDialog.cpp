// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PreferencesDialog.h"

//  EPMScope
#include "EPMScopePreferences.h"

// QCommon
#include "ApplicationEnhancements.h"
#include "AlpacaSettings.h"

// Qt
#include <QFileDialog>


PreferencesDialog::PreferencesDialog
(
	EPMScopePreferences* preferences,
	QWidget* parent
) :
	QDialog(parent),
	_preferences(preferences)
{
	setupUi(this);

	_windowDuration->setValue(_preferences->windowDuration());
	_saveResultsData->setChecked(_preferences->saveRunData());
	_platformPathLabel->setText(_preferences->platformPath());
	_runtimeConfPathLabel->setText(_preferences->runtimeConfigurationsPath());
	_outputPathLabel->setText(_preferences->outputPath());
	_loggingCB->setChecked(_preferences->loggingActive());
	_logPathLabel->setText(_preferences->appLogPath());

	connect(this, &QDialog::accepted, this, &PreferencesDialog::on_accepted);
}

void PreferencesDialog::on__platformPathButton_clicked()
{
	QString currentPath = _platformPathLabel->text();
	QString platformPath = QFileDialog::getExistingDirectory(this, "Select Platform Config File Path", currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (platformPath.isEmpty() == false && platformPath != currentPath)
	{
		_platformPathLabel->setText(QDir::cleanPath(platformPath));
	}
}

void PreferencesDialog::on__runtimePathButton_clicked()
{
	QString currentPath = _runtimeConfPathLabel->text();
	QString platformPath = QFileDialog::getExistingDirectory(this, "Select Runtime Config File Path", currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (platformPath.isEmpty() == false && platformPath != currentPath)
	{
		_runtimeConfPathLabel->setText(QDir::cleanPath(platformPath));
	}
}

void PreferencesDialog::on__outputPathButton_clicked()
{
	QString currentPath = _outputPathLabel->text();
	QString outputPath = QFileDialog::getExistingDirectory(this, "Select Output Directory Path", currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (outputPath.isEmpty() == false && outputPath != currentPath)
	{
		_outputPathLabel->setText(QDir::cleanPath(outputPath));
	}
}

void PreferencesDialog::on__logPathButton_clicked()
{
	QString currentPath = _logPathLabel->text();
	QString outputPath = QFileDialog::getExistingDirectory(this, "Select Log Directory Path", currentPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (outputPath.isEmpty() == false && outputPath != currentPath)
	{
		_logPathLabel->setText(QDir::cleanPath(outputPath));
	}
}

void PreferencesDialog::on__setToDefaultButton_clicked()
{
	_windowDuration->setValue(_preferences->defaultWindowDuration());
	_saveResultsData->setChecked(_preferences->defaultSaveData());
	_platformPathLabel->setText(_preferences->defaultPlatformPath());
	_runtimeConfPathLabel->setText(_preferences->defaultRuntimeConfigurationsPath());
	_outputPathLabel->setText(_preferences->defaultOutputPath());
	_loggingCB->setChecked(_preferences->defaultLoggingState());
	_logPathLabel->setText(_preferences->defaultAppLogPath());
}

void PreferencesDialog::on_accepted()
{
	bool changed(false);

	try
	{
		if (_preferences->windowDuration() != _windowDuration->value())
			throw true;

		if (_preferences->saveRunData() != _saveResultsData->isChecked())
			throw true;

		if (_preferences->loggingActive() != _loggingCB->isChecked())
			throw true;

		if (_preferences->platformPath().toLower() != _platformPathLabel->text().toLower())
			throw true;

		if (_preferences->runtimeConfigurationsPath().toLower() != _runtimeConfPathLabel->text().toLower())
			throw true;

		if (_preferences->outputPath().toLower() != _outputPathLabel->text().toLower())
			throw true;

		if (_preferences->appLogPath().toLower() != _logPathLabel->text().toLower())
			throw true;
	}
	catch (bool e)
	{
		Q_UNUSED(e)

		changed = true;
	}

	if (changed)
	{
		_preferences->saveWindowDuration(_windowDuration->value());
		_preferences->saveSaveRunData(_saveResultsData->isChecked());
		_preferences->saveAppLogPath(_logPathLabel->text());
		_preferences->saveLoggingActive(_loggingCB->isChecked());
		_preferences->savePlatformPath(_platformPathLabel->text());
		_preferences->saveRuntimeConfigurationsPath(_runtimeConfPathLabel->text());
		_preferences->saveOutputPath(_outputPathLabel->text());
	}

	if (changed)
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
