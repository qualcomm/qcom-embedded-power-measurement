#ifndef EPMCONFIGURATIONWINDOW_H
#define EPMCONFIGURATIONWINDOW_H
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
// Copyright 2018-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_EPMConfigurationWindow.h"

// libEPM
#include "EPMConfigFile.h"
#include "KratosMapping.h"

// QCommon
#include "RecentFiles.h"

// UILib
#include "NotificationWidget.h"

class EPMConfigurationWindow :
	public QMainWindow,
	private Ui::EPMConfigurationWindow
{
Q_OBJECT

public:
	EPMConfigurationWindow(QWidget* parent = Q_NULLPTR);
	~EPMConfigurationWindow();

	bool openFile(const QString& filePath);

	void setEPMConfigFile(EPMConfigFile* epmConfigFile);
	void setupColumnProperties();

protected:
	virtual void changeEvent(QEvent* e);
	virtual void closeEvent(QCloseEvent* event);

signals:
	void checkAuthorExists();

protected slots:
	void on_actionNew_triggered();
	void on_actionOpen_triggered();
	void onActionRecentMenuTriggered();
	void on_actionSave_triggered();
	void on_actionSave_As_triggered();
	void on_actionImport_Excel_Template_triggered();
	void on_actionExit_triggered();
	void on_actionAdd_Channel_triggered();
	void on_actionDelete_Channel_triggered();
	void on_actionCompile_to_Config_triggered();
	void on_actionAbout_triggered();
	void on_actionSubmit_Bug_Report_triggered();

	void on__channelSettings_itemClicked(QTableWidgetItem* item);
	void on__channelSettings_itemDoubleClicked(QTableWidgetItem* item);

	void onColorEditRequested(quint32 row);

	void addAuthorIfNotExists();
	void onChannelEnabledStateChanged(bool newState);
	void onChannelTypeChanged(int index);
	void onTableItemChanged(QTableWidgetItem* twi);

private slots:
	void on__author_textChanged(const QString &arg1);
	void on__author_editingFinished();
	void on__target_textChanged(const QString &arg1);
	void on__platform_textChanged(const QString &arg1);
	void on__description_textChanged(const QString &arg1);
	void on__spmVersion_currentIndexChanged(int index);

private:
	RecentFiles					_recentFiles;
	EPMConfigFile*				_epmConfigFile{Q_NULLPTR};
	QString						_epmFilePath;
	static QPoint				_nextWindowPos;
	KratosMapping				_kratosMapping;
	NotificationWidget*			_nwgt{Q_NULLPTR};

	bool open();

	bool importExcel();
	bool save();
	bool saveAs();

	void populateFields();
	void rebuildRecents();

	bool testForEPMFile();
};

#endif // EPMCONFIGURATIONWINDOW_H
