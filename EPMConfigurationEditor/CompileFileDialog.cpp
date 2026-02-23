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
// Copyright 2018-2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "CompileFileDialog.h"

#include "EPMEditorDefines.h"

// QCommon
#include "ConsoleApplicationEnhancements.h"
#include "AlpacaSettings.h"

// QT
#include <QCheckBox>
#include <QDir>
#include <QFileInfo>
#include <QGridLayout>
#include <QObject>

const QByteArray kLastCompilePath(QByteArrayLiteral("lastCompilePath"));
const QByteArray kCompileFileDialog(QByteArrayLiteral("CompileFileDialog"));

CompileFileDialog::CompileFileDialog
(
	QWidget* parent,
	const QString& empFileName
) :
	QFileDialog(parent),
	_compileFile(empFileName)
{
	connect(this, &QDialog::finished, this, &CompileFileDialog::on_finsished);

	AlpacaSettings settings(kAppName);
	QString lastPath;
	QString candidateCompileName = QFileInfo(empFileName).fileName();

	settings.beginGroup(kCompileFileDialog);
	lastPath = settings.value(kLastCompilePath, documentsDataPath(kAppName)).toString();
	if (QDir().exists(lastPath) == false)
	{
		lastPath.clear();
	}

	if (candidateCompileName.isEmpty() == false)
	{
		candidateCompileName.replace(".ccnf", ".conf");
		selectFile(candidateCompileName);
	}

	setOption(QFileDialog::DontUseNativeDialog);
	setWindowTitle("Save Configuration File");
	if (lastPath.isEmpty() == false)
		setDirectory(lastPath);
	setFileMode(QFileDialog::AnyFile);
	setNameFilter("EPM Configuration File (*.conf)");
	setAcceptMode(QFileDialog::AcceptSave);

	QGridLayout* dialogLayout = qobject_cast<QGridLayout*>(layout());
	if (dialogLayout != Q_NULLPTR)
	{
		dialogLayout->setRowMinimumHeight(5, 10);

		_useRCMMapping = new QCheckBox;
		_useRCMMapping->setText("Produce RCM Mappings");
		_useRCMMapping->setChecked(settings.value("produceRCMMapping", true).toBool());
		dialogLayout->addWidget(_useRCMMapping, 6, 0);

		_useKratos = new QCheckBox;
		_useKratos->setText("Use Kratos Mappings");
		_useKratos->setChecked(settings.value("useKratosMapping", false).toBool());
		dialogLayout->addWidget(_useKratos, 6, 1);

		dialogLayout->setRowMinimumHeight(7, 10);
	}

	settings.endGroup();
}

void CompileFileDialog::on_finsished
(
	int r
)
{
	if (r == QDialog::Accepted)
	{
		AlpacaSettings settings(kAppName);

		settings.beginGroup(kCompileFileDialog);

		QStringList selectedFileList = selectedFiles();
		if (selectedFileList.isEmpty() == false)
		{
			QString saveFile = selectedFileList.at(0);
			if (saveFile.isEmpty() == false)
			{
				QFileInfo saveFileInfo(saveFile);
				QString saveFileDir = saveFileInfo.absolutePath();
				QString lastPath = settings.value(kLastCompilePath, documentsDataPath(kAppName)).toString();
				if (lastPath != saveFileDir)
				{
					settings.setValue(kLastCompilePath, saveFileDir);
				}

				_compileFile = saveFile;
			}
			else
				_compileFile.clear();

			bool checked;

			checked = _useRCMMapping->checkState() == Qt::Checked ? true : false;
			if (settings.value("produceRCMMapping", true).toBool() != checked)
				settings.setValue("produceRCMMapping", checked);

			checked = _useKratos->checkState() == Qt::Checked ? true : false;
			if (settings.value("useKratosMapping", true).toBool() != checked)
				settings.setValue("useKratosMapping", checked);
		}

		settings.endGroup();
	}
}
