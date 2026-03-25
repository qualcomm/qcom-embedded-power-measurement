#ifndef EPMCONFIGURATIONWINDOW_H
#define EPMCONFIGURATIONWINDOW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
