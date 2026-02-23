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
// Copyright 2020-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PreferencesDialog.h"

// TAC
#include "EPMEditorDefines.h"
#include "EPMConfigurationApplication.h"

// QCommon
#include "ApplicationEnhancements.h"
#include "AlpacaSettings.h"

// Qt
#include <QFileDialog>

PreferencesDialog::PreferencesDialog
(
	QWidget* parent
) :
	QDialog(parent)
{
	setupUi(this);

	connect(this, &PreferencesDialog::preferencesChanged, EPMConfigurationApplication::epmConfigAppInstance(), &EPMConfigurationApplication::on_preferencesChanged);

	connect(this, &QDialog::accepted, this, &PreferencesDialog::on_accepted);
}

void PreferencesDialog::on__setToDefaultsButton_clicked()
{
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
	bool changed(false);

	AlpacaSettings settings(kAppName);

/*	settings.beginGroup(kTacLoggingGroup);

	bool temp;

	temp = settings.value(kTacLoggingEnabled, false).toBool();
	if (temp != _loggingCB->isChecked())
	{
		changed = true;
	}
	else
	{
		QString tempStr = settings.value(kLogLocation, defaultLoggingPath(kAppName)).toString();
		if (tempStr.toLower() != _logLocation->text().toLower())
		{
			changed = true;
		}
		else if (_powerOnDelaySpin->value() != settings.value(kPowerOnDelay, 1300).toInt())
		{
			changed = true;
		}
	}

	if (changed)
	{
		settings.setValue(kTacLoggingEnabled, _loggingCB->isChecked());
		settings.setValue(kLogLocation, _logLocation->text());
		settings.setValue(kPowerOnDelay, _powerOnDelaySpin->value());
	}

	settings.endGroup();
	*/

	if (changed)
		emit preferencesChanged(kEPMConfigurationGroup);
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
