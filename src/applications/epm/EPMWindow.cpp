// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMWindow.h"

// EPM
#include "AboutDialog.h"
#include "EPMApplication.h"
#include "EPMDefines.h"
#include "EPMPreferences.h"
#include "PreferencesDialog.h"

//QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ApplicationEnhancements.h"
#include "ProgressDialog.h"
#include "Range.h"

//Qt
#include <QCoreApplication>
#include <QCloseEvent>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QSet>
#include <QStandardPaths>
#include <QTextStream>

const QString kAllGroups(QStringLiteral("All Groups"));
const QString kEmptyGroup(QStringLiteral("<Empty Group>"));
const QString kOn(QStringLiteral("On"));
const QString kOff(QStringLiteral("Off"));
const QString kRaw(QStringLiteral("Raw"));

const int kType(0);
const int kIndex(1);
const int kCurrentChannel(4);
const int kVoltageChannel(5);

const int kMeasureState(Qt::UserRole + 1);
const int kKeyRole(Qt::UserRole + 2);
const int kChannelHashRole(Qt::UserRole + 3);
const int kSeriesHashRole(Qt::UserRole + 4);
const int kIndexTypeRole(Qt::UserRole + 5);
const int kResistorValueRole(Qt::UserRole + 6);
const int kCategoryRole(Qt::UserRole + 7);

enum MeasureState
{
	eOff,
	eOn,
	eRaw
};

// lambda
auto twiSetter = [] (QTableWidgetItem* twi, bool state, bool disabled = false)
{
	if (disabled == false)
	{
		if (state)
		{
			twi->setForeground(Qt::green);
			twi->setText(kOn);
			twi->setData(kMeasureState, eOn);
		}
		else
		{
			twi->setForeground(Qt::red);
			twi->setText(kOff);
			twi->setData(kMeasureState, eOff);
		}
	}
	else
	{
		twi->setForeground(Qt::gray);
		twi->setText("-");
		twi->setData(kMeasureState, eOff);
	}
};

EPMWindow::EPMWindow
(
	QWidget* parent
) :
	QMainWindow(parent),
	_recentRuntimeConfigFiles(kAppName, "EPMRunConfigRecents")
{
	setupUi(this);

	EPMPreferences* preferences = EPMApplication::epmAppInstance()->getPreferences();
	_epmProject = _EPMProject::createEPMProject(preferences);

	connect(_durationSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[=](double d)
		{
			preferences->setRunDuration(d);
		});

	_durationSpin->setValue(preferences->runDuration());

	_menuBar = menuBar();

	QMenu* fileMenu = new QMenu("File");
	fileMenu->addAction("Load Run Configuration...", this, &EPMWindow::onLoadRunConfigurationTriggered);
	fileMenu->addAction("Load Export Configuration...", this, &EPMWindow::onLoadExportConfigurationTriggered);

	_recentLoadConfigurations = new QMenu(tr("Recent Configuration Files"));
	fileMenu->addMenu(_recentLoadConfigurations);

	fileMenu->addAction("Save Run Configuration", this, &EPMWindow::onSaveRunConfigurationTriggered);
	fileMenu->addAction("Save As Run Configuration...", this, &EPMWindow::onSaveAsRunConfigurationTriggered);
	fileMenu->addAction("Save Export Configuration", this, &EPMWindow::onSaveExportConfigurationTriggered);
	fileMenu->addAction("Save As Export Configuration...", this, &EPMWindow::onSaveAsExportConfigurationTriggered);
	fileMenu->addSeparator();
	fileMenu->addAction("Preferences...", this, &EPMWindow::onPreferencesTriggered);
	fileMenu->addSeparator();
	fileMenu->addAction("Quit", this, &EPMWindow::onQuitTriggered);
	_menuBar->addMenu(fileMenu);

	QMenu* helpMenu = new QMenu("Help");
	// lambda
	helpMenu->addAction("Contents", [&]{ startLocalBrowser(docsRoot() + "/getting-started/05-Embedded-Power-Measurement.html");});
	helpMenu->addSeparator();
	helpMenu->addAction("About...", this, &EPMWindow::onAboutTriggered);
	helpMenu->addAction("Rate Me...", [&]{ EPMApplication::epmAppInstance()->showRateDialog();});
	helpMenu->addAction(QIcon(":/BugWriter.png"), "Submit Bug Report", this, &EPMWindow::onSubmitBugReportTriggered);

	_menuBar->addMenu(helpMenu);
	_topLayout->setMenuBar(_menuBar);

	_nwgt = new NotificationWidget(_statusbar);
	_statusbar->addPermanentWidget(_nwgt);

	restoreSettings();
	_platformPathLabel->setText(preferences->platformPath());

	setupPlatformCombo();
	rebuildRecents();

	updatePreferences();

	connect(this, &EPMWindow::progress, _nwgt, &NotificationWidget::progress);
	connect(_epmDeviceList, &EPMDeviceList::deviceScanStarted, this, &EPMWindow::on_deviceScanStarted);
	connect(_epmDeviceList, &EPMDeviceList::deviceCountUpdated, this, &EPMWindow::on_deviceCountChanged);

}

EPMWindow::~EPMWindow()
{
	if (_nwgt != Q_NULLPTR)
	{
		delete _nwgt;
		_nwgt = Q_NULLPTR;
	}
}

void EPMWindow::shutDown()
{
}

QString EPMWindow::getCurrentPlatform()
{
	return _platformComboBox->currentText();
}

QStringList EPMWindow::getPlatforms()
{
	QStringList result;

	for (auto platformIndex: range(1, _platformComboBox->count())) // first platform, index 0, is a descriptor
	{
		result += _platformComboBox->itemText(platformIndex);
	}

	return result;
}

bool EPMWindow::selectPlatform
(
	const QString& platform
)
{
	bool result(false);

	if (_platformComboBox->findText(platform, Qt::MatchFixedString) != -1)
	{
		_platformComboBox->setCurrentText(platform);
		result = true;
	}
	else
	{
		_lastError = QString("Platform %1 not found.").arg(platform);
	}

	return result;
}

bool EPMWindow::selectFirstDevice()
{
	return _epmDeviceList->selectFirstDevice();
}

bool EPMWindow::selectDeviceByName(const QString &deviceName)
{
	return _epmDeviceList->selectDeviceByName(deviceName);
}

QString EPMWindow::lastError()
{
	QString result(_lastError);

	_lastError.clear();

	return result;
}

void EPMWindow::changeEvent
(
	QEvent* e
)
{
	QMainWindow::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;

	default:
		break;
	}
}

void EPMWindow::closeEvent
(
	QCloseEvent* event
)
{
	shutDown();

	event->accept();

	EPMApplication::removeEPMWindow(this);
}

void EPMWindow::showEvent
(
	QShowEvent* event
)
{
    QMainWindow::showEvent(event);
    
    if (_firstShow)
    {
        _firstShow = false;

        QTimer::singleShot(0, this, [this]() {
            _epmDeviceList->refresh();
        });
    }
}

bool EPMWindow::on__startAcquisitionButton_clicked()
{
	EPMDevice selectedDevice = _epmDeviceList->currentDevice();
	if (selectedDevice.isNull() == false)
	{
		_epmProject->setEPMDevice(selectedDevice);

		ProgressDialog* progressDialog(Q_NULLPTR);

		if (isVisible())
		{
			progressDialog = new ProgressDialog("Running Acquisition", static_cast<quint32>(EPMApplication::getPreferences()->runDuration()), this);
			progressDialog->open();

			EPMApplication::epmAppInstance()->setOverrideCursor(Qt::WaitCursor);
		}

		QElapsedTimer elapsed;

		elapsed.start();

		_epmProject->acquire(EPMApplication::getPreferences()->runDuration());

		if (progressDialog != Q_NULLPTR)
		{
			EPMApplication::epmAppInstance()->restoreOverrideCursor();

			progressDialog->close();
			progressDialog->deleteLater();
		}
	}

	return true;
}

void EPMWindow::onLoadRunConfigurationTriggered()
{
	loadRuntimeConfig();
}

void EPMWindow::onLoadExportConfigurationTriggered()
{
	loadExportConfig();
}

void EPMWindow::onActionRecentMenuTriggered()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action != Q_NULLPTR)
	{
		openRuntimeConfig(action->data().toString());
	}
}

void EPMWindow::onSaveRunConfigurationTriggered()
{
	if (_epmProject->runtimeConfigFilePath().isEmpty())
		onSaveAsRunConfigurationTriggered();
	else
		saveRuntimeConfig();
}

void EPMWindow::onSaveAsRunConfigurationTriggered()
{
	QString defPath = EPMApplication::getPreferences()->runtimeConfigurationsPath();

	defPath = QDir::cleanPath(defPath + QDir::separator() + _epmProject->target() + "_XXXX");

	QString saveFileName = QFileDialog::getSaveFileName(this, "Save Runtime Configuration As...", defPath, tr("EPM Runtime Config (*.rcnf)"));
	if (saveFileName.isEmpty() == false)
	{
		bool result = _epmProject->setRuntimeConfigurationFile(saveFileName);
		if (result)
			saveRuntimeConfig();
	}
}

void EPMWindow::onSaveExportConfigurationTriggered()
{
	if (_epmProject->exportConfigFilePath().isEmpty())
		onSaveAsExportConfigurationTriggered();
	else
		saveExportConfig();
}

void EPMWindow::onSaveAsExportConfigurationTriggered()
{
	QString defPath = EPMApplication::getPreferences()->defaultExportConfigurationsPath();

	defPath = QDir::cleanPath(defPath + QDir::separator() + _epmProject->target() + "_XXXX");

	QString saveFileName = QFileDialog::getSaveFileName(this, "Save Export Configuration As...", defPath, tr("EPM Export Config (*.ecnf)"));
	if (saveFileName.isEmpty() == false)
	{
		bool result = _epmProject->setExportConfigurationFile(saveFileName);
		if (result)
			saveExportConfig();
	}
}

void EPMWindow::onPreferencesTriggered()
{
	PreferencesDialog preferences(EPMApplication::getPreferences(), this);

	if (preferences.exec() == QDialog::Accepted)
	{
		updatePreferences();
	}
}

void EPMWindow::onQuitTriggered()
{
	EPMApplication::epmAppInstance()->quitEPM();
}

void EPMWindow::onAboutTriggered()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.setTitle("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">Embedded Power Measurement</span></p></body></html>");

	QFile file(":/About.txt");
	if (file.open(QIODevice::ReadOnly) == true)
	{
		QByteArray html = file.readAll();
		aboutDialog.setAboutText(html);
		file.close();
	}

	aboutDialog.setAppName(kAppName);
	aboutDialog.setAppVersion(kAppVersion.toLatin1());
	QPixmap pixMap = QPixmap(QString::fromUtf8(":/EPM_BS.png"));
	aboutDialog.setBackSplash(pixMap);

	aboutDialog.exec();
}

void EPMWindow::onSubmitBugReportTriggered()
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/QEPM/bin/BugWriter"; // Linux Sucks
#else
	QString program = "BugWriter";
#endif

	QStringList arguments;
	arguments << "product:QEPM";
	arguments << "prodversion:" + kProductVersion;
	arguments << "application:Embedded_Power_Measurement";
	arguments << "appversion:" + kAppVersion;

	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	process->startDetached();
}

void EPMWindow::on__deviceList_currentRowChanged(int currentRow)
{
	if (currentRow != -1)
	{
		EPMDevice currentDevice = _epmDeviceList->currentDevice();
		if (currentDevice->programmed())
			_startAcquisitionButton->setEnabled(true);
		else
			_startAcquisitionButton->setEnabled(false);
	}
	else
	{

		_startAcquisitionButton->setEnabled(false);
	}
}

void EPMWindow::on__platformComboBox_currentIndexChanged
(
	int index
)
{
	if (index != _currentIndex)
	{
		_categoryList->clear();
		_channelTable->clearContents();
		_channelTable->setRowCount(0);

		if (index <= 0)
		{
			setPlatformLabel("");
		}
		else
		{
			QString comboText = _platformComboBox->currentText();
			QString absoluteFilePath = _platformComboBox->currentData().toString();

			setPlatformLabel(comboText);
			loadPlatformConfigFile(absoluteFilePath);

			EPMApplication::getPreferences()->saveLastPlatform(absoluteFilePath);
		}

		_currentIndex = index;
	}
}

void EPMWindow::on__channelTable_cellClicked
(
	int row,
	int column
)
{
	if (column == kCurrentChannel || column == kVoltageChannel)
	{
		// update the UI
		Qt::KeyboardModifiers keyboardModifier = QGuiApplication::keyboardModifiers();

		QTableWidgetItem* twi = _channelTable->item(row, column);
		if (twi != Q_NULLPTR)
		{
			if (twi->foreground() == Qt::gray)
				return;

			HashType channelHash = twi->data(kChannelHashRole).toInt();
			MeasureState measureState = static_cast<MeasureState>(twi->data(kMeasureState).toInt());
			bool rawState = false;

			if (keyboardModifier & Qt::ControlModifier)
				rawState = true;

			bool shouldMeasure = (measureState == eOff);
			if (shouldMeasure)
			{
				if (rawState)
				{
					twi->setData(kMeasureState, eRaw);
					twi->setText(kRaw);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					twi->setForeground(Qt::darkRed);
				}
				else
				{
					twi->setData(kMeasureState, eOn);
					twi->setText(kOn);
					twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
					twi->setForeground(Qt::green);
				}
			}
			else
			{
				rawState = false;
				twi->setData(kMeasureState, eOff);
				twi->setText(kOff);
				twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				twi->setForeground(Qt::red);
			}

			// update the model
			if (column == kCurrentChannel)
				_epmProject->setCurrentChannelState(channelHash, shouldMeasure, rawState);
			else
				_epmProject->setVoltageChannelState(channelHash, shouldMeasure, rawState);
		}
	}
}

void EPMWindow::on__channelTable_customContextMenuRequested
(
	const QPoint& pos
)
{
	auto stateSetter = [] (EPMProject epmProject, QTableWidget* channelTable, bool state, int column, EPMIndexType epmIndexType)
	{
		for (auto row: range(channelTable->rowCount()))
		{
			// Update the UI
			EPMIndexType typeIndex(eUnsetIndex);
			bool isMarker(false);

			QTableWidgetItem* item = channelTable->item(row, column);
			QTableWidgetItem* typeItem = channelTable->item(row, kType);
			HashType channelHash = item->data(kChannelHashRole).toULongLong();

			typeIndex = static_cast<EPMIndexType>(typeItem->data(kIndexTypeRole).toInt());
			if (typeIndex == eMarkerIndex)
				isMarker = true;

			if (epmIndexType == eUnsetIndex)
				typeIndex = eUnsetIndex;

			if (epmIndexType == typeIndex)
			{
				if (isMarker == false)
				{
					twiSetter(item, state);
					if (column == kCurrentChannel)
					{
						epmProject->setCurrentChannelState(channelHash, state);
					}
					else
					{
						epmProject->setVoltageChannelState(channelHash, state);
					}
				}
				else
				{
					if (column == kCurrentChannel)
					{
						twiSetter(item, false, true);
						epmProject->setCurrentChannelState(channelHash, false);
					}
					else
					{
						twiSetter(item, state);
						epmProject->setVoltageChannelState(channelHash, state);
					}
				}
			}

		}
	};

	auto categorySetter = [] (EPMProject epmProject, QTableWidget* channelTable, bool state, int column, const QString& category)
	{
		for (auto row: range(channelTable->rowCount()))
		{
			QTableWidgetItem* item = channelTable->item(row, column);
			QTableWidgetItem* typeItem = channelTable->item(row, kType);

			QString itemCategory = typeItem->data(kCategoryRole).toString();

			if (itemCategory == category)
			{
				HashType hashtype = item->data(kChannelHashRole).toULongLong();
				twiSetter(item, state);

				if (column == kCurrentChannel)
					epmProject->setCurrentChannelState(hashtype, state);
				else
					epmProject->setVoltageChannelState(hashtype, state);
			}
		}
	};

	QTableWidgetItem* item = _channelTable->itemAt(pos);
	QTableWidgetItem* typeItem = _channelTable->item(item->row(), kType);
	if (item != Q_NULLPTR && typeItem != Q_NULLPTR)
	{
		int itemColumn(item->column());

		QMenu* popup = new QMenu;

		popup->addAction("All On", this, [=, this] { stateSetter(_epmProject, _channelTable, true, itemColumn, eUnsetIndex);});
		popup->addAction("All Off", this, [=, this] { stateSetter(_epmProject, _channelTable, false, itemColumn, eUnsetIndex);});
		popup->addSeparator();
		popup->addAction("All RCMs On", this, [=, this] { stateSetter(_epmProject, _channelTable, true, itemColumn, eRCMIndex);});
		popup->addAction("All RCMs Off", this, [=, this] { stateSetter(_epmProject, _channelTable, false, itemColumn, eRCMIndex);});
		popup->addAction("All SPMs On", this, [=, this] { stateSetter(_epmProject, _channelTable, true, itemColumn, eSPMIndex);});
		popup->addAction("All SPMs Off", this, [=, this] { stateSetter(_epmProject, _channelTable, false, itemColumn, eSPMIndex);});

		if (itemColumn == kVoltageChannel)
		{
			popup->addAction("All Markers On", this, [=, this] { stateSetter(_epmProject, _channelTable, true, itemColumn, eMarkerIndex);});
			popup->addAction("All Markers Off", this, [=, this] { stateSetter(_epmProject, _channelTable, false, itemColumn, eMarkerIndex);});
		}

		QStringList categories = _epmProject->categories();
		if (categories.isEmpty() == false)
		{
			popup->addSeparator();

			for (const auto& category: categories)
			{
				popup->addAction(category + " On", this, [=, this] { categorySetter(_epmProject, _channelTable, true, itemColumn, category);});
				popup->addAction(category + " Off", this, [=, this] { categorySetter(_epmProject, _channelTable, false, itemColumn, category);});
			}
		}

		if (popup != Q_NULLPTR)
		{
			popup->exec(_channelTable->mapToGlobal(pos));
			popup->deleteLater();
		}
	}
}

void EPMWindow::on__categoryList_itemChanged
(
	QListWidgetItem* item
)
{
	bool activeState(item->checkState() == Qt::Checked ? true : false);
	QString itemText(item->text());

	if (itemText == kAllGroups)
	{
		_categoryList->blockSignals(true);

		for (auto row: range(_categoryList->count()))
		{
			QListWidgetItem* groupItem = _categoryList->item(row);
			if (groupItem->text() != kAllGroups)
			{
				groupItem->setCheckState(activeState ? Qt::Checked : Qt::Unchecked);
				updateGroup(groupItem->text(), activeState);
			}
		}

		_categoryList->blockSignals(false);
	}
	else
	{
		updateGroup(itemText, activeState);
	}
}

void EPMWindow::restoreSettings()
{
	AlpacaSettings settings(kAppName);
	QPoint defaultPos(-1, -1);
	QSize defaultSize(-1, -1);

	QPoint pos = settings.value("pos", defaultPos).toPoint();
	QSize size = settings.value("size", defaultSize).toSize();

	if (pos != defaultPos)
		move(pos);

	if (size != defaultSize)
		resize(size);

	_durationSpin->blockSignals(true);
	_durationSpin->setValue(EPMApplication::getPreferences()->runDuration());
	_durationSpin->blockSignals(false);
}

void EPMWindow::saveSettings()
{
	AlpacaSettings settings(kAppName);

	settings.setValue("pos", pos());
	settings.setValue("size", size());
}

void EPMWindow::rebuildRecents()
{
	buildRecentsMenu(_recentLoadConfigurations, _recentRuntimeConfigFiles, this, SLOT(onActionRecentMenuTriggered()));
}

void EPMWindow::loadPlatformConfigFile
(
	const QString& filePath
)
{
	if (_epmProject->loadPlatformConfigFile(filePath))
	{
		_spmVersionLabel->setText(_epmProject->spmVersion());

		QListWidgetItem* lwi;

		lwi = new QListWidgetItem(kAllGroups);
		lwi->setCheckState(Qt::Checked);
		_categoryList->addItem(lwi);

		for (const auto& category: _epmProject->categories())
		{
			if (category.isEmpty())
				lwi = new QListWidgetItem(kEmptyGroup);
			else
				lwi = new QListWidgetItem(category);

			lwi->setCheckState(Qt::Checked);
			_categoryList->addItem(lwi);
		}

		setupTable();
	}
}

void EPMWindow::setupPlatformCombo()
{
	QString lastPlatform = EPMApplication::getPreferences()->lastPlatform();

	_platformPathLabel->setText(EPMApplication::getPreferences()->platformPath());

	_categoryList->clear();
	_channelTable->clearContents();
	_channelTable->setRowCount(0);

	_platformComboBox->setCurrentIndex(0);
	_platformComboBox->clear();
	_platformComboBox->addItem("<select a config file>");

	quint32 platformCount =	_epmProject->getPlatformCount();
	for (const auto& platformIndex: range(platformCount))
	{
		QString platformName;
		QString platformPath;

		if (_epmProject->getPlatform(platformIndex, platformName, platformPath))
		{
			_platformComboBox->addItem(platformName, platformPath);

			if (platformPath == lastPlatform)
			{
				_platformComboBox->blockSignals(true);
				_platformComboBox->setCurrentText(platformName);
				_platformComboBox->blockSignals(false);
				loadPlatformConfigFile(platformPath);
				setPlatformLabel(platformName);
			}
		}
	}

	_currentIndex = -2;
}

bool EPMWindow::setupPlatform()
{
	bool result(false);

	setPlatformLabel(EPMApplication::getPreferences()->platformPath());
	setupPlatformCombo();

	return result;
}

bool EPMWindow::setupOutputPath()
{
	bool result(false);

	QString outputPath;

	outputPath = EPMApplication::getPreferences()->outputPath();

	if (QDir(outputPath).exists() == true)
	{
		result = true;

		_outputPathLabel->blockSignals(true);
		_outputPathLabel->setText(outputPath);
		_outputPathLabel->blockSignals(false);
	}

	return result;
}

bool EPMWindow::setTemporaryResultsFolder
(
	const QString &resultsFolder
)
{
	bool result;

	result = QDir().mkpath(resultsFolder);

	if (result)
		_epmProject->setTemporaryResultsDirectory(resultsFolder);

	return result;
}

void EPMWindow::setPlatformLabel
(
	const QString& platform
)
{
	QString platformLabel = QString("<html><head/><body><p><span style=\" font-size:10pt; font-weight:600;\">%1</span></p></body></html>").arg(platform);
	_platformLabel->setText(platformLabel);
}

void EPMWindow::setupTable()
{
	EPMChannel currentChannel;
	EPMChannel voltageChannel;

	_channelTable->clearContents();

	quint32 channelCount = _epmProject->channelCount();
	_channelTable->setRowCount(channelCount);

	for (auto row: range(channelCount))
	{
		if (_epmProject->getChannelPair(row, currentChannel, voltageChannel) == true)
		{
			QTableWidgetItem* twi;
			bool rowEnabled(voltageChannel->active() == true);
			bool markerRow(voltageChannel->indexType() == eMarkerIndex);

			quint32 key = voltageChannel->key();

			twi = new QTableWidgetItem(EPMIndexTypeToString(voltageChannel->indexType()));
			twi->setData(kKeyRole, key);
			twi->setData(kIndexTypeRole, voltageChannel->indexType());
			twi->setData(kChannelHashRole, voltageChannel->channelHash());
			twi->setData(kCategoryRole, voltageChannel->category());

			_channelTable->setItem(row, kType, twi);

			twi = new QTableWidgetItem(QString::number(voltageChannel->index()));
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			_channelTable->setItem(row, kIndex, twi);

			twi = new QTableWidgetItem(voltageChannel->channelName());
			_channelTable->setItem(row, 2, twi);

			twi = new QTableWidgetItem(voltageChannel->category());
			_channelTable->setItem(row, 3, twi);

			// Current Column Setup
			twi = new QTableWidgetItem(kOn);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			if (markerRow == false)
			{
				if (currentChannel.isNull() == false)
				{
					twi->setData(kChannelHashRole, currentChannel->channelHash());
					twi->setData(kSeriesHashRole, currentChannel->seriesHash());
				}

				if (rowEnabled)
				{
					twiSetter(twi, true);
				}
				else
				{
					twiSetter(twi, false);
				}
			}
			else
			{
				twiSetter(twi, false, true);
			}
			_channelTable->setItem(row, kCurrentChannel, twi);

			// Voltage Column Setup
			twi = new QTableWidgetItem(kOn);
			twi->setData(kChannelHashRole, voltageChannel->channelHash());
			twi->setData(kSeriesHashRole, voltageChannel->seriesHash());
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			if (rowEnabled)
			{
				twiSetter(twi, true);
			}
			else
			{
				twiSetter(twi, false);
			}
			_channelTable->setItem(row, kVoltageChannel, twi);

			QString resistorText = qFuzzyIsNull(voltageChannel->_resistorValue) ? "-" : QString::number(voltageChannel->_resistorValue, 'f', 2);
			resistorText.replace(".00", ".0");
			twi = new QTableWidgetItem(resistorText);
			twi->setData(kResistorValueRole, voltageChannel->_resistorValue);
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			_channelTable->setItem(row, 6, twi);

			twi = new QTableWidgetItem(voltageChannel->_description);
			_channelTable->setItem(row, 7, twi);
		}
	}

	_channelTable->resizeColumnsToContents();
}

void EPMWindow::updateTable()
{
	for (auto rowIndex: range(_channelTable->rowCount()))
	{
		QTableWidgetItem* twi;
		bool active;
		bool saveRaw;

		twi = _channelTable->item(rowIndex, kCurrentChannel);
		HashType channelHash = twi->data(kChannelHashRole).toULongLong();

		if (_epmProject->getCurrentChannelState(channelHash, active, saveRaw) == true)
		{
			twiSetter(twi, active);
		}
		else
		{
			twiSetter(twi, false, true);
		}

		twi = _channelTable->item(rowIndex, kVoltageChannel);
		channelHash = twi->data(kChannelHashRole).toULongLong();

		if (_epmProject->getVoltageChannelState(channelHash, active, saveRaw) == true)
		{
			twiSetter(twi, active);
		}
		else
		{
			twiSetter(twi, false, true);
		}
	}
}

void EPMWindow::updateGroup
(
	const QString& groupName,
	bool activeState
)
{
	bool emptyGroup(groupName == kEmptyGroup);

	if (groupName != kAllGroups)
	{
		int rowCount(_channelTable->rowCount());
		for (int row(0); row < rowCount; row++)
		{
			QTableWidgetItem* twi = _channelTable->item(row, 3);
			if (emptyGroup == false)
			{
				if (twi->text().contains(groupName))
				{
					if (activeState)
						_channelTable->showRow(row);
					else
						_channelTable->hideRow(row);
				}
			}
			else
			{
				if (twi->text().isEmpty() == true)
				{
					if (activeState)
						_channelTable->showRow(row);
					else
						_channelTable->hideRow(row);
				}
			}
		}
	}
}

void EPMWindow::saveRuntimeConfig()
{
	_epmProject->saveRuntimeConfigurationFile();
	_recentRuntimeConfigFiles.addFile(_epmProject->runtimeConfigFilePath());
	rebuildRecents();
}

bool EPMWindow::openRuntimeConfig(const QString &runtimeFile)
{
	bool result{false};

	if (runtimeFile.isEmpty() == false)
	{
		if ((result = _epmProject->loadRuntimeConfigFile(runtimeFile)) == true)
		{
			updateTable();
		}
	}

	return result;
}

void EPMWindow::loadRuntimeConfig()
{
	EPMPreferences* preference = EPMApplication::epmAppInstance()->getPreferences();
	QString defPath = preference->runtimeConfigurationsPath();

	QString openFileName = QFileDialog::getOpenFileName(this, "Load Runtime Configuration...", defPath, tr("EPM Runtime Config (*.rcnf)"));
	if (openFileName.isEmpty() == false)
	{
		if (openRuntimeConfig(openFileName) == true)
		{
			_recentRuntimeConfigFiles.addFile(openFileName);
			rebuildRecents();
		}
	}
}

void EPMWindow::saveExportConfig()
{
	_epmProject->saveExportConfigurationFile();
}

bool EPMWindow::openExportConfig(const QString &exportFile)
{
	bool result{false};

	if (exportFile.isEmpty() == false)
	{
		if ((result = _epmProject->loadExportConfigFile(exportFile)) == true)
		{
			updateTable();
		}
	}

	return result;
}

void EPMWindow::loadExportConfig()
{
	QString defPath = EPMApplication::getPreferences()->defaultExportConfigurationsPath();

	QString openFileName = QFileDialog::getOpenFileName(this, "Load Export Configuration...", defPath, tr("EPM Export Config (*.ecnf)"));
	if (openFileName.isEmpty() == false)
	{
		if (openExportConfig(openFileName) == false)
			QMessageBox::critical(this, kAppName + ": Improper configuration format", "Could not load export configuration. Are you trying to load a valid export configuration?");
	}
}

bool EPMWindow::loadRuntimeConfigFromAutomationFile
(
	const QString& runtimeConfigFile
)
{
	QString runTimeConfigPath(runtimeConfigFile);

	if (runTimeConfigPath.endsWith(".rcnf", Qt::CaseInsensitive) == false)
	{
		runTimeConfigPath += ".rcnf";
	}

	QFileInfo fileInfo(runTimeConfigPath);
	QString fileInfoPath = fileInfo.path().remove('.');
	if (fileInfoPath.isEmpty())
	{
		EPMPreferences* preference = EPMApplication::epmAppInstance()->getPreferences();
		QString defPath = preference->runtimeConfigurationsPath();

		runTimeConfigPath = QDir::cleanPath(defPath + QDir::separator() + runTimeConfigPath);
	}

	return _epmProject->loadRuntimeConfigFile(runTimeConfigPath);
}

void EPMWindow::updatePreferences()
{
	if (_platformPathLabel->text() != EPMApplication::getPreferences()->platformPath())
	{
		_epmProject->setPlatformPath(EPMApplication::getPreferences()->platformPath());
		setupPlatform();
	}

	if (_outputPathLabel->text() != EPMApplication::getPreferences()->outputPath())
		setupOutputPath();

	bool loggingEnabled = EPMApplication::getPreferences()->loggingActive();

	AppCore::getAppCore()->setAppLogging(loggingEnabled);
	AppCore::getAppCore()->setRunLogging(loggingEnabled);
}

void EPMWindow::on_deviceScanStarted()
{
		emit progress(kProgressActive);
}

void EPMWindow::on_deviceCountChanged(uint deviceCount)
{
	if (_nwgt != Q_NULLPTR && deviceCount == 0)
		_nwgt->insertNotification("EPM supports PSOC and SPMv4 debug boards. Please connect a PSOC/SPMv4 debug board to continue.", eInfoNotification);

	emit progress(kProgressMax);
}
