#ifndef EPMWINDOW_H
#define EPMWINDOW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
			Arkojit Sen (arkosen@qti.qualcomm.com)
*/

#include "ui_EPMWindow.h"
#include "NotificationWidget.h"
// libEPM
#include "EPMProject.h"
#include "EPMRun.h"

// QCommon
#include "RecentFiles.h"

// Qt
class QAction;
#include <QMap>
class QMenuBar;

class EPMWindow :
	public QMainWindow,
	private Ui::EPMWindowClass
{
	Q_OBJECT

public:
	explicit EPMWindow(QWidget* parent = Q_NULLPTR);
	~EPMWindow();

	void shutDown();

	QString getCurrentPlatform();
	QStringList getPlatforms();
	bool selectPlatform(const QString& platform);

	bool selectFirstDevice();
	bool selectDeviceByName(const QString& deviceName);

	bool setTemporaryResultsFolder(const QString& resultsFolder);

	QString getLastResultsPath()
	{
		QString result(_lastResultsPath);

		_lastResultsPath.clear();

		return result;
	}

	bool loadRuntimeConfigFromAutomationFile(const QString& runtimeConfigFile);
	bool openRuntimeConfig(const QString& runtimeFile);

	QString lastError();

signals:
	void progress(const quint8 value=10, NotificationLevel level=eInfoNotification);

protected:
	virtual void changeEvent(QEvent* e);
	virtual void closeEvent(QCloseEvent* event);
	virtual void showEvent(QShowEvent* event);

public slots:
	// File Menu
	void onLoadRunConfigurationTriggered();
	void onLoadExportConfigurationTriggered();
	void onActionRecentMenuTriggered();
	void onSaveRunConfigurationTriggered();
	void onSaveAsRunConfigurationTriggered();
	void onSaveExportConfigurationTriggered();
	void onSaveAsExportConfigurationTriggered();
	void onPreferencesTriggered();
	void onQuitTriggered();

	// Help Menu
	void onAboutTriggered();
	void onSubmitBugReportTriggered();

	void on__deviceList_currentRowChanged(int currentRow);
	void on__platformComboBox_currentIndexChanged(int index);

	bool on__startAcquisitionButton_clicked();

	void on__channelTable_cellClicked(int row, int column);
	void on__channelTable_customContextMenuRequested(const QPoint &pos);
	void on__categoryList_itemChanged(QListWidgetItem* item);
	void on_deviceScanStarted();
	void on_deviceCountChanged(uint deviceCount);

private:
	void restoreSettings();
	void saveSettings();

	void rebuildRecents();

	void loadPlatformConfigFile(const QString& filePath);
	bool setupPlatform();
	void setupPlatformCombo();
	void setPlatformLabel(const QString& platform);
	bool setupOutputPath();
	void setupTable();
	void updateTable();
	void updateGroup(const QString& groupName, bool activeState);

	void saveRuntimeConfig();
	void loadRuntimeConfig();
	void saveExportConfig();
	bool openExportConfig(const QString &exportFile);
	void loadExportConfig();

	void buildRuntimeChannels(EPMRun& epmRun);

	void updatePreferences();

	QMenuBar*					_menuBar{Q_NULLPTR};
	int							_currentIndex{-2};
	QString						_lastError;

	EPMProject					_epmProject;

	RecentFiles					_recentRuntimeConfigFiles;

	QMenu*						_recentLoadConfigurations {Q_NULLPTR};
	QString						_lastResultsPath;
	NotificationWidget*			_nwgt{Q_NULLPTR};
	bool 						_firstShow = true;
};

#endif // EPMWINDOW_H
