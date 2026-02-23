#ifndef ALPACAVIEWERWINDOW_H
#define ALPACAVIEWERWINDOW_H
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
// Copyright 2019-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_EPMViewerWindow.h"

// AlpacaViewer
#include "UDASPreferences.h"

// libEPM
#include "SeriesDataWindow.h"

// QCommon
#include "RecentFiles.h"
#include "UDASFile.h"

// Qt
class QCheckBox;
#include <QMap>
class QStatusBar;

typedef QMap<uint, SeriesDataWindow*> SeriesDataWindows;

class EPMViewerWindow :
	public QWidget,
	private Ui::EPMViewerWindowClass
{
	Q_OBJECT

public:
	EPMViewerWindow(UDASPreferences* preferences, QWidget* parent = Q_NULLPTR);
	virtual ~EPMViewerWindow();

	bool openAndReadPowerProject(const QString& powerRunFileName);

	static void clearSeriesDataWindows();

protected slots:
	// File Menu
	void onOpenTriggered();
	void onPreferencesTriggered();
	void onQuitTriggered();

	void onActionRecentMenuTriggered();

	// Help Menu
	void onAboutTriggered();
	void onSubmitBugReportTriggered();

	// tab widget signals
	void on__tabWidget_currentChanged(int index);

	// channel table signals
	void onCurrentCheckChanged(bool newState, HashType channelHash, HashType seriesHash);
	void onVoltageCheckChanged(bool newState, HashType channelHash, HashType seriesHash);
	void onPowerCheckChanged(bool newState, HashType channelHash, HashType seriesHash);
	void onErrorEvent(const QString& errorMessage);

	// stats combo signals
	void on__statItemCombo_currentIndexChanged(int index);

	// Zoom
	void on__zoomOutButton_clicked();
	void on__zoomInButton_clicked();
	void on__resetZoomButton_clicked();

	// Banding
	void onT0Update(qreal logicalValue);
	void onT1Update(qreal logicalValue);

	// EPM Channel Table
	void onColorDoubleClick(HashType channelHash);
	void onCurrentDataDoubleClick(HashType channelHash);
	void onVoltageDataDoubleClick(HashType channelHash);
	void onPowerDataDoubleClick(HashType channelHash);

protected:
	virtual void closeEvent(QCloseEvent* event);
	virtual void changeEvent(QEvent *e);
	virtual void resizeEvent(QResizeEvent* event);

private slots:
	void on__zoomOnCursorButton_clicked();
	void on__cursorStatsRadio_clicked();
	void on__allStatsRadio_clicked();

	void on__t0_textChanged(const QString &t0);
	void on__t1_textChanged(const QString &t1);

	void on__exportButton_clicked();

	void on__locateExportFolder_clicked();

	void on__totalAvgCurrentCheckBox_toggled(bool checked);

	void on__resetButton_clicked();

	void on__maxButton_clicked();

private:
	void rebuildRecents();
	bool loadPowerRunFile(const QString& powerRunFileName);

	void populateModels(const UDASBlock& udasBlock);

	void populateStatColumn(int column, UDASBlock& udasBlock);
	void clearStatColumn();
	void populateChannelStatList();
	void populatePowerStatList();

	void setupStatBoxSize();
	void setupCursorValues();

	void updateStatistics();
	void populateStatisticsTable();

	void updateDeltaT(const UDASBlock& udasBlock);
	void updateTotalAvgCurrent(HashType channelHash, bool state);

	static SeriesDataWindows	_seriesDataWindows;
	UDASFile					_udasFile;
	QString						_powerRunFileName;
	PRNFile						_prnFile;
	EPMProject                  _epmProject{Q_NULLPTR};
	UDASPreferences*			_preferences{Q_NULLPTR};

	RecentFiles					_recentPowerRunFiles;
	QMenu*						_recentFilesMenu{Q_NULLPTR};
	QStatusBar*					_statusBar{Q_NULLPTR};

	bool						_excelAvailable{false};
	qreal						_totalAverageCurrent{0.0};
};

#endif // ALPACAVIEWERWINDOW_H
