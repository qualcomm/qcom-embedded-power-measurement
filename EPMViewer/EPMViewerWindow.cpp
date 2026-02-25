// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "EPMViewerWindow.h"

// AlpacaViewer
#include "EPMViewerApplication.h"
#include "EPMViewerDefines.h"
#include "UDASPreferences.h"
#include "PreferencesDialog.h"

// QCommmon
#include "AboutDialog.h"
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ApplicationEnhancements.h"
#include "Range.h"

// LibExcel
#ifdef Q_OS_WINDOWS
	#include "QTExcel.h"
#endif

// libEPM
#include "ColorConversion.h"
#include "SeriesDataWindow.h"

// Qt
#include <QCheckBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMetaMethod>
#include <QProcess>
#include <QResizeEvent>
#include <QSet>
#include <QStatusBar>
#include <QTimer>

SeriesDataWindows EPMViewerWindow::_seriesDataWindows;

EPMViewerWindow::EPMViewerWindow
(
	UDASPreferences* preferences,
	QWidget* parent
) :
	QWidget(parent),
	_preferences(preferences),
	_recentPowerRunFiles(kAppName, "AVRunConfigRecents")
{
	setupUi(this);

	if (EPMViewerApplication::appInstance()->isInternalBuild())
		setWindowTitle("Alpaca Viewer");
	else
		setWindowTitle("EPM Viewer");

#ifdef Q_OS_WINDOWS
	_excelAvailable = QTExcel::excelAvailable();
#endif

#ifdef Q_OS_LINUX
	_excelAvailable = false;
#endif

	_channelTable->setColumns(EPMChannelTable::CurrentColumnVisible | EPMChannelTable::VoltageColumnVisible | EPMChannelTable::DataColumnVisible);

	_epmProject = EPMProject(new _EPMProject(_preferences));
	_channelTable->setEPMProject(_epmProject);

	connect(_channelTable, &EPMChannelTable::colorDoubleClick, this, &EPMViewerWindow::onColorDoubleClick);

	connect(_channelTable, &EPMChannelTable::currentCheckChanged, this, &EPMViewerWindow::onCurrentCheckChanged);
	connect(_channelTable, &EPMChannelTable::voltageCheckChanged, this, &EPMViewerWindow::onVoltageCheckChanged);
	connect(_channelTable, &EPMChannelTable::errorOccurred, this, &EPMViewerWindow::onErrorEvent);

	_powerChannelTable->setColumns(EPMChannelTable::PowerColumnVisible | EPMChannelTable::DataColumnVisible);
	connect(_powerChannelTable, &EPMChannelTable::colorDoubleClick, this, &EPMViewerWindow::onColorDoubleClick);
	connect(_powerChannelTable, &EPMChannelTable::powerCheckChanged, this, &EPMViewerWindow::onPowerCheckChanged);

	_currentChart->setYTitle("Current (mA)");
	_currentChart->setYRange(-5, 5);
	_currentChart->setXRange(0, 5);

	_voltageChart->setYTitle("Voltage (V)");
	_voltageChart->setYRange(0, 4);
	_voltageChart->setXRange(0, 5);

	_powerChart->setYTitle("Power (mW)");
	_powerChart->setYRange(-10, 10);
	_powerChart->setXRange(0, 4);

	QMenuBar* _menuBar = new QMenuBar;

	QMenu* fileMenu = new QMenu("File");
	fileMenu->addAction("Open...", this, &EPMViewerWindow::onOpenTriggered);

	_recentFilesMenu = new QMenu(tr("Recent Power Run Files"));
	fileMenu->addMenu(_recentFilesMenu);

	fileMenu->addSeparator();
	fileMenu->addAction("Preferences...", this, &EPMViewerWindow::onPreferencesTriggered);
	fileMenu->addSeparator();
	fileMenu->addAction("Quit", this, &EPMViewerWindow::onQuitTriggered);
	_menuBar->addMenu(fileMenu);

	QMenu* helpMenu = new QMenu("Help");
	// lambda
	helpMenu->addAction("Contents", [&]{ startLocalBrowser(docsRoot() + "/getting-started/04-EPM-Viewer.html");});
	helpMenu->addSeparator();
	helpMenu->addAction("About...", this, &EPMViewerWindow::onAboutTriggered);
	helpMenu->addAction("Rate Me...", [&]{ EPMViewerApplication::appInstance()->showRateDialog();});
	helpMenu->addAction(QIcon(":/BugWriter.png"), "Submit Bug Report", this, &EPMViewerWindow::onSubmitBugReportTriggered);

	_menuBar->addMenu(helpMenu);

	_topLayout->setMenuBar(_menuBar);
	_statusBar = new QStatusBar(this);
	QLabel* logicalCoordinateLabel = new QLabel(_statusBar);
	logicalCoordinateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	_statusBar->setSizeGripEnabled(true);
	_statusBar->addWidget(logicalCoordinateLabel, 1);

	_topLayout->addWidget(_statusBar);

	_recentPowerRunFiles.hideExtensions(true);

	rebuildRecents();

	AlpacaSettings settings(kAppName);
	settings.beginGroup("EPMViewerWindow");

	QPoint pos = settings.value("pos", QPoint(40, 40)).toPoint();
	move(pos);

	QSize size = settings.value("size", QSize(1200, 800)).toSize();
	resize(size);

	settings.endGroup();

	auto showMessageWithTimeout = [logicalCoordinateLabel](const QString& text) {
		logicalCoordinateLabel->setText(text);
	};

	connect(_currentChart, &CurrentChart::hoveredCoordChange, showMessageWithTimeout);
	connect(_voltageChart, &VoltageChart::hoveredCoordChange, showMessageWithTimeout);
	connect(_powerChart, &PowerChart::hoveredCoordChange, showMessageWithTimeout);

	connect(_voltageChart, &AlpacaChart::zoomRectChanged, [this] (const QRectF& zoomRect)
	{
		QSignalBlocker signalBlocker(_currentChart);

		_currentChart->setXZoom(zoomRect.left(), zoomRect.right());
	});

	connect(_currentChart, &AlpacaChart::zoomRectChanged, [this] (const QRectF& zoomRect)
	{
		QSignalBlocker signalBlocker(_voltageChart);

		_voltageChart->setXZoom(zoomRect.left(), zoomRect.right());
	});

	connect(_currentChart, &AlpacaChart::t0Changed, this, &EPMViewerWindow::onT0Update);
	connect(_voltageChart, &AlpacaChart::t0Changed, this, &EPMViewerWindow::onT0Update);
	connect(_powerChart, &AlpacaChart::t0Changed, this, &EPMViewerWindow::onT0Update);

	connect(_currentChart, &AlpacaChart::t1Changed, this, &EPMViewerWindow::onT1Update);
	connect(_voltageChart, &AlpacaChart::t1Changed, this, &EPMViewerWindow::onT1Update);
	connect(_powerChart, &AlpacaChart::t1Changed, this, &EPMViewerWindow::onT1Update);

	connect(_channelTable, &EPMChannelTable::currentDataDoubleClick, this, &EPMViewerWindow::onCurrentDataDoubleClick);
	connect(_channelTable, &EPMChannelTable::voltageDataDoubleClick, this, &EPMViewerWindow::onVoltageDataDoubleClick);
	connect(_powerChannelTable, &EPMChannelTable::powerDataDoubleClick, this, &EPMViewerWindow::onPowerDataDoubleClick);

	setupStatBoxSize();
}

EPMViewerWindow::~EPMViewerWindow()
{
}

bool EPMViewerWindow::openAndReadPowerProject
(
	const QString& powerRunFileName
)
{
	bool result(false);

	QFileInfo fileInfo(powerRunFileName);

	QString epmPath = QDir::cleanPath(fileInfo.path() + QDir::separator() + "Params.scl");

	if (_udasFile.open(epmPath))
	{
		if (_prnFile.read(powerRunFileName))
		{
			_powerRunFileName = powerRunFileName;

			_udasFile.updateWithPRN(_prnFile);

			_recentPowerRunFiles.addFile(powerRunFileName);

			result = true;

			if (_udasFile.isValid() == false)
			{
				QMessageBox::warning(this, "UDAS File is Empty", QString("%1 contains no waveforms.").arg(epmPath));
			}
			else
			{
				_channelTable->populateChannels(_udasFile, _prnFile);
				_powerChannelTable->populateChannels(_udasFile, _prnFile);

				if (_channelTable->isValid() && _powerChannelTable->isValid())
				{
					_currentChart->setXRange(0.0, _udasFile._duration);
					_voltageChart->setXRange(0.0, _udasFile._duration);
					_powerChart->setXRange(0.0, _udasFile._duration);

					_name->setText(_udasFile._label);
					_startDate->setText(_udasFile._date);
					_startTime->setText(_udasFile._time);
					_duration->setText(QString::number(_udasFile._duration));

					_currentChart->setYRange(0, _udasFile._maxCurrentRange);
					_voltageChart->setYRange(0, _udasFile._maxVoltageRange);
					_powerChart->setYRange(0.0, _udasFile._maxPowerRange);

					_deltaT->setText(QString::number(_udasFile._duration));
					_1DivDeltaT->setText(QString::number(1.0 / _udasFile._duration));

					UDASBlock udasBlock;

					for (auto index: range(_udasFile.currentBlockCount()))
					{
						UDASBlock udasBlock = _udasFile.getCurrentBlock(index);
						if (udasBlock.isNull() == false)
						{
							populateModels(udasBlock);
						}
					}

					for (auto index: range(_udasFile.voltageBlockCount()))
					{
						UDASBlock udasBlock = _udasFile.getVoltageBlock(index);
						if (udasBlock.isNull() == false)
						{
							populateModels(udasBlock);
						}
					}

					for (auto index: range(_udasFile.markerBlockCount()))
					{
						UDASBlock udasBlock = _udasFile.getMarkerBlock(index);
						if (udasBlock.isNull() == false)
						{
							populateModels(udasBlock);
						}
					}

					for (auto index: range(_udasFile.powerBlockCount()))
					{
						UDASBlock udasBlock = _udasFile.getPowerBlock(index);
						if (udasBlock.isNull() == false)
						{
							populateModels(udasBlock);
						}
					}

					populateChannelStatList();

					_currentChart->setCursorLines(1.0, _udasFile._duration);
					_voltageChart->setCursorLines(1.0, _udasFile._duration);
					_powerChart->setCursorLines(1.0, _udasFile._duration);

					_currentChart->replot();
					_voltageChart->replot();
					_powerChart->replot();

					setupCursorValues();

					_exportButton->setEnabled(true);
				}
			}
		}
	}
	else
	{
		QString message;
		QFileInfo fileInfo(epmPath);

		if (fileInfo.exists())
		{
			message = QString("%1 can't be opened.  Permissions?  Already Open?").arg(powerRunFileName);
		}
		else
		{
			message = QString("%1 not found.").arg(powerRunFileName);
			_recentPowerRunFiles.removeFile(powerRunFileName);
			rebuildRecents();
		}

		QMessageBox::warning(this, "File Open Error", message);
	}

	return result;
}

void EPMViewerWindow::clearSeriesDataWindows()
{

}

void EPMViewerWindow::onOpenTriggered()
{
	QString filters = "Power Run File (*.prn)";

	QString powerRunFile = QFileDialog::getOpenFileName(this, "Open an EPM Power Run file", _preferences->outputPath(), filters);
	if (powerRunFile.isEmpty() == false)
	{
		loadPowerRunFile(powerRunFile);
	}
}

void EPMViewerWindow::onPreferencesTriggered()
{
	PreferencesDialog preferencesDialog(EPMViewerApplication::appInstance()->getPreferences(), this);

	preferencesDialog.exec();
}

void EPMViewerWindow::onQuitTriggered()
{
	QCoreApplication::instance()->exit();
}

void EPMViewerWindow::onAboutTriggered()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.setTitle("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">Alpaca Viewer</span></p></body></html>");

	QFile file(":/About.txt");
	if (file.open(QIODevice::ReadOnly) == true)
	{
		QByteArray html = file.readAll();
		aboutDialog.setAboutText(html);
		file.close();
	}

	aboutDialog.setAppName(kAppName);
	aboutDialog.setAppVersion(kAppVersion.toLatin1());
	QPixmap pixMap = QPixmap(QString::fromUtf8(":/AlpacaViewer_BS.png"));
	aboutDialog.setBackSplash(pixMap);

	aboutDialog.exec();
}

void EPMViewerWindow::onSubmitBugReportTriggered()
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/Alpaca/bin/BugWriter"; // Linux Sucks
#else
	QString program = "BugWriter";
#endif

	QStringList arguments;
	arguments << "product:Alpaca";
	arguments << "prodversion:" + kProductVersion;
	arguments << "application:Alpaca_Viewer";
	arguments << "appversion:" + kAppVersion;

	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	process->startDetached();
}

void EPMViewerWindow::on__tabWidget_currentChanged
(
	int index
)
{
	switch (index)
	{
	case 0:
		populateChannelStatList();
		break;

	case 1:
		populatePowerStatList();
		break;

	default:
		break;
	}
}

void EPMViewerWindow::onCurrentCheckChanged
(
	bool newState,
	HashType channelHash,
	HashType seriesHash
)
{
	Q_UNUSED(channelHash);

	qreal min;
	qreal max;

	_currentChart->setSeriesState(seriesHash, newState);
	_currentChart->calculateRange(min, max);
	_currentChart->setYRange(min, max);

	_currentChart->replot();

	updateTotalAvgCurrent(channelHash, newState);
}

void EPMViewerWindow::onVoltageCheckChanged
(
	bool newState,
	HashType channelHash,
	HashType seriesHash
)
{
	Q_UNUSED(channelHash);

	qreal min;
	qreal max;

	_voltageChart->setSeriesState(seriesHash, newState);
	_voltageChart->calculateRange(min, max);
	_voltageChart->setYRange(min, max);

	_voltageChart->replot();
}

void EPMViewerWindow::onPowerCheckChanged
(
	bool newState,
	HashType channelHash,
	HashType seriesHash
)
{
	Q_UNUSED(channelHash);

	qreal min;
	qreal max;

	_powerChart->setSeriesState(seriesHash, newState);
	_powerChart->calculateRange(min, max);
	_powerChart->setYRange(min, max);

	_powerChart->replot();
}

void EPMViewerWindow::onErrorEvent(const QString &errorMessage)
{
	QMessageBox::critical(this, "EPM Viewer error", QString("Error: %1").arg(errorMessage));
	_channelTable->clear();
	_powerChannelTable->clear();
}

const int kColumnTotal(1);

void EPMViewerWindow::populateStatColumn
(
	int column,
	UDASBlock& udasBlock
)
{
	QTableWidgetItem* twi;

	if (_allStatsRadio->isChecked())
	{
		twi = new QTableWidgetItem(QString::number(udasBlock->_statistics.avg(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(0, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->duration(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(1, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->duration() * udasBlock->_statistics.avg(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(2, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_statistics.populationStdDeviation(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(3, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_statistics.max(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(4, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_statistics.max() - udasBlock->_statistics.min(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(5, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_statistics.min(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(6, column, twi);
	}
	else
	{
		twi = new QTableWidgetItem(QString::number(udasBlock->_rangedStatistics.avg(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(0, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->rangeDuration(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(1, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->rangeDuration() * udasBlock->_rangedStatistics.avg(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(2, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_rangedStatistics.populationStdDeviation(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(3, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_rangedStatistics.max(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(4, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_rangedStatistics.max() - udasBlock->_rangedStatistics.min(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(5, column, twi);

		twi = new QTableWidgetItem(QString::number(udasBlock->_rangedStatistics.min(), 'g', 3));
		twi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		_statsTable->setItem(6, column, twi);
	}

	switch (udasBlock->_waveFormType)
	{
	case eWaveFormCurrent:
		twi = _statsTable->item(0, 2);
		twi->setText("mA");
		twi = _statsTable->item(2, 2);
		twi->setText("mA-Sec");
		twi = _statsTable->item(4, 2);
		twi->setText("mA");
		twi = _statsTable->item(5, 2);
		twi->setText("mA");
		twi = _statsTable->item(6, 2);
		twi->setText("mA");
		break;

	case eWaveFormVoltage:
		twi = _statsTable->item(0, 2);
		twi->setText("V");
		twi = _statsTable->item(2, 2);
		twi->setText("V-Sec");
		twi = _statsTable->item(4, 2);
		twi->setText("V");
		twi = _statsTable->item(5, 2);
		twi->setText("V");
		twi = _statsTable->item(6, 2);
		twi->setText("V");
		break;

	case eWaveFormPower:
		twi = _statsTable->item(0, 2);
		twi->setText("mW");
		twi = _statsTable->item(2, 2);
		twi->setText("mW-Sec");
		twi = _statsTable->item(4, 2);
		twi->setText("mW");
		twi = _statsTable->item(5, 2);
		twi->setText("mW");
		twi = _statsTable->item(6, 2);
		twi->setText("mW");
		break;

	default:
		break;
	}
}

void EPMViewerWindow::clearStatColumn()
{
	QTableWidgetItem* twi = new QTableWidgetItem("-");
	_statsTable->setItem(0, 1, twi);

	twi = new QTableWidgetItem("-");
	_statsTable->setItem(1, 1, twi);

	twi = new QTableWidgetItem("-");
	_statsTable->setItem(2, 1, twi);

	twi = new QTableWidgetItem("-");
	_statsTable->setItem(3, 1, twi);

	twi = new QTableWidgetItem("-");
	_statsTable->setItem(4, 1, twi);

	twi = new QTableWidgetItem("-");
	_statsTable->setItem(5, 1, twi);

	twi = new QTableWidgetItem("-");
	_statsTable->setItem(6, 1, twi);
}

void EPMViewerWindow::populateChannelStatList()
{
	_statItemCombo->clear();

	HashList hashList = _udasFile.buildSortedIVHashList();
	for (const auto& hash: hashList)
	{
		UDASBlock currentBlock = _udasFile.getCurrentBlockByChannelHash(hash);
		if (currentBlock.isNull() == false)
		{
			_statItemCombo->addItem(currentBlock->seriesName(), currentBlock->_channelNumber);
		}

		UDASBlock voltageBlock = _udasFile.getVoltageBlockByChannelHash(hash);
		if (voltageBlock.isNull() == false)
		{
			_statItemCombo->addItem(voltageBlock->seriesName(), voltageBlock->_channelNumber);
		}
	}

	for (auto index: range(_udasFile.markerBlockCount()))
	{
		UDASBlock markerBlock = _udasFile.getMarkerBlock(index);
		if (markerBlock.isNull() == false)
		{
			_statItemCombo->addItem(markerBlock->seriesName(), markerBlock->_channelNumber);
		}
	}

	_statItemCombo->setCurrentIndex(0);

	setupStatBoxSize();
}

void EPMViewerWindow::populatePowerStatList()
{
	_statItemCombo->clear();

	for (auto index: range(_udasFile.powerBlockCount()))
	{
		UDASBlock powerBlock = _udasFile.getPowerBlock(index);
		if (powerBlock.isNull() == false)
		{
			_statItemCombo->addItem(powerBlock->seriesName(), powerBlock->_channelNumber);
		}
	}

	_statItemCombo->setCurrentIndex(0);
}

void EPMViewerWindow::setupStatBoxSize()
{
	int _statTableSize = _statsTable->width();
	int fourtyPercent = static_cast<int>(static_cast<double>(_statTableSize) * .35);
	int theRest = _statTableSize - (fourtyPercent * 2);
	_statsTable->setColumnWidth(0, fourtyPercent);
	_statsTable->setColumnWidth(1, theRest);
	_statsTable->setColumnWidth(2, fourtyPercent);
}

void EPMViewerWindow::setupCursorValues()
{
	_t0->setText(QString::number(_currentChart->cursorT0(), 'f'));
	_t1->setText(QString::number(_currentChart->cursorT1(), 'f'));
}

void EPMViewerWindow::updateStatistics()
{
	qreal start = _currentChart->cursorT0();
	qreal end = _currentChart->cursorT1();

	for (auto index: range(_udasFile.blockCount()))
	{
		UDASBlock udasBlock = _udasFile.getBlock(index);
		if (udasBlock.isNull() == false)
		{
			udasBlock->setRange(start, end);
		}
	}
}

void EPMViewerWindow::populateStatisticsTable()
{
	QString blockName = _statItemCombo->currentText();
	if (blockName.isEmpty() == false)
	{
		UDASBlock udasBlock = _udasFile.getBlockBySeriesName(blockName);
		if (udasBlock.isNull() == false)
		{
			populateStatColumn(kColumnTotal, udasBlock);
			updateDeltaT(udasBlock);
		}
		else
		{
			clearStatColumn();
		}
	}
	else
	{
		clearStatColumn();
	}
}

void EPMViewerWindow::updateDeltaT(const UDASBlock& udasBlock)
{
	double multiplier = 1000.0;

	if (udasBlock.isNull() == false)
	{
		qreal logicalDeltaT = udasBlock->deltaTime();
		qreal invLogicalDeltaT = 1/logicalDeltaT;

		if(logicalDeltaT < 1)
		{
			logicalDeltaT = logicalDeltaT * multiplier;	
			
			_deltaTLabel->setText("ms");
			_1DivDeltaTLabel->setText("s");
		}
		else
		{
			invLogicalDeltaT = invLogicalDeltaT * multiplier;

			_deltaTLabel->setText("s");
			_1DivDeltaTLabel->setText("ms");
		}

		QString deltaT = QString::number(logicalDeltaT, 'f', 6);
		QString invDeltaT = QString::number(invLogicalDeltaT, 'f', 6);

		_deltaT->setText(deltaT.left(deltaT.indexOf('.') + 5));
		_1DivDeltaT->setText(invDeltaT.left(invDeltaT.indexOf('.') + 5));
	}
}

void EPMViewerWindow::updateTotalAvgCurrent(HashType channelHash, bool state)
{
	qreal avgCurrent{0.0};
	UDASBlock currentBlock = _udasFile.getCurrentBlockByChannelHash(channelHash);

	if(currentBlock.isNull() == false)
	{
		avgCurrent = currentBlock->_statistics.avg();
		if(state)
			_totalAverageCurrent += avgCurrent;
		else
			_totalAverageCurrent -= avgCurrent;
	}

	if(_totalAvgCurrentCheckBox->isChecked())
	{
		_totalAvgCurrent->setText(QString::number(_totalAverageCurrent, 'f', 3));
	}
}

void EPMViewerWindow::on__statItemCombo_currentIndexChanged
(
	int index
)
{
	QString blockName = _statItemCombo->itemText(index);
	if (blockName.isEmpty() == false)
	{
		UDASBlock udasBlock = _udasFile.getBlockBySeriesName(blockName);
		if (udasBlock.isNull() == false)
		{
			populateStatColumn(kColumnTotal, udasBlock);
		}
		else
		{
			clearStatColumn();
		}
	}
	else
	{
		clearStatColumn();
	}
}

void EPMViewerWindow::on__zoomOutButton_clicked()
{
	QSignalBlocker currentChartBlocker(_currentChart);
	QSignalBlocker voltageChartBlocker(_voltageChart);
	_currentChart->zoomOut();
	_voltageChart->zoomOut();

	setupCursorValues();
}

void EPMViewerWindow::on__zoomInButton_clicked()
{
	QSignalBlocker currentChartBlocker(_currentChart);
	QSignalBlocker voltageChartBlocker(_voltageChart);
	_currentChart->zoomIn();
	_voltageChart->zoomIn();

	setupCursorValues();
}

void EPMViewerWindow::on__resetZoomButton_clicked()
{
	_currentChart->resetZoom();
	_voltageChart->resetZoom();

	setupCursorValues();
}

void EPMViewerWindow::closeEvent
(
	QCloseEvent* event
)
{
	AlpacaSettings settings(kAppName);
	settings.beginGroup("EPMViewerWindow");
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.endGroup();

	for (auto& seriesDataWindow: _seriesDataWindows)
	{
		seriesDataWindow->close();
		seriesDataWindow->deleteLater();
	}

	QWidget::closeEvent(event);
}

void EPMViewerWindow::changeEvent
(
	QEvent* e
)
{
	QWidget::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void EPMViewerWindow::resizeEvent
(
	QResizeEvent* event
)
{
	event->setAccepted(true);
}

void EPMViewerWindow::rebuildRecents()
{
	buildRecentsMenu(_recentFilesMenu, _recentPowerRunFiles, this, SLOT(onActionRecentMenuTriggered()));
}

bool EPMViewerWindow::loadPowerRunFile
(
	const QString& powerRunFileName
)
{
	bool result{false};

	if(_powerRunFileName.isEmpty() == false)
	{
		EPMViewerWindow* alpacaViewer = EPMViewerApplication::createViewerWindow();
		result = alpacaViewer->openAndReadPowerProject(powerRunFileName);
	}
	else
		result = openAndReadPowerProject(powerRunFileName);

	return result;
}

void EPMViewerWindow::populateModels
(
	const UDASBlock& udasBlock
)
{
	AlpacaChart* alpacaChart(Q_NULLPTR);

	switch (udasBlock->_waveFormType)
	{
	case eWaveFormCurrent:
		alpacaChart = _currentChart;
		break;

	case eWaveFormVoltage:
		alpacaChart = _voltageChart;
		break;

	case eWaveFormPower:
		alpacaChart = _powerChart;
		break;

	default:
		break;
	}

	if (alpacaChart != Q_NULLPTR)
	{
		alpacaChart->addLineSeries(udasBlock);
	}
}

void EPMViewerWindow::onActionRecentMenuTriggered()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action != Q_NULLPTR)
	{
		QString filePath = action->data().toString();
		if (filePath.isEmpty() == false)
		{
			loadPowerRunFile(filePath);
		}
	}
}

void EPMViewerWindow::onT0Update(qreal logicalValue)
{
	_t0->setText(QString::number(logicalValue, 'f'));
	_t0->repaint();
}

void EPMViewerWindow::onT1Update(qreal logicalValue)
{
	_t1->setText(QString::number(logicalValue, 'f'));
	_t1->repaint();
}

void EPMViewerWindow::onColorDoubleClick
(
	HashType channelHash
)
{
	BasicColor originalColor;

	UDASBlock udasCurrentBlock = _udasFile.getCurrentBlockByChannelHash(channelHash);
	UDASBlock udasVoltageBlock = _udasFile.getVoltageBlockByChannelHash(channelHash);
	UDASBlock udasPowerBlock = _udasFile.getPowerBlockByChannelHash(channelHash);

	if (udasCurrentBlock.isNull() == false)
		originalColor = udasCurrentBlock->channelColor();
	else if (udasVoltageBlock.isNull() == false)
		originalColor = udasVoltageBlock->channelColor();
	else if (udasPowerBlock.isNull() == false)
		originalColor = udasPowerBlock->channelColor();
	else
		originalColor = BasicColor(255, 0, 0);

	QColor newColor = QColorDialog::getColor(ColorConversion::BasicToColor(originalColor), this, "Select a new channel color");

	if (newColor.isValid())
	{
		_prnFile.setChannelColor(channelHash, ColorConversion::ColorToBasic(newColor));

		_prnFile.write(_powerRunFileName);

		if (udasCurrentBlock.isNull() == false)
		{
			udasCurrentBlock->setChannelColor(ColorConversion::ColorToBasic(newColor));
			_currentChart->setSeriesColor(udasCurrentBlock);
		}

		if (udasVoltageBlock.isNull() == false)
		{
			udasVoltageBlock->setChannelColor(ColorConversion::ColorToBasic(newColor));

			_voltageChart->setSeriesColor(udasVoltageBlock);
		}

		if (udasPowerBlock.isNull() == false)
		{
			udasPowerBlock->setChannelColor(ColorConversion::ColorToBasic(newColor));

			_powerChart->setSeriesColor(udasPowerBlock);
		}

		_channelTable->setChannelColor(channelHash, newColor);
		_powerChannelTable->setChannelColor(channelHash, newColor);
	}
}

void EPMViewerWindow::onCurrentDataDoubleClick
(
	HashType channelHash
)
{
	SeriesDataWindow* seriesDataWindow{Q_NULLPTR};

	UDASBlock udasBlock = _udasFile.getCurrentBlockByChannelHash(channelHash);
	if (udasBlock.isNull() == false)
	{
		auto seriesDataWindowIter = _seriesDataWindows.find(udasBlock->seriesHash());
		if (seriesDataWindowIter == _seriesDataWindows.end())
		{
			seriesDataWindow = new SeriesDataWindow;
			seriesDataWindow->setWindowTitle("Series Data - " + udasBlock->seriesName());
			seriesDataWindow->setData(udasBlock);
			seriesDataWindow->show();

			_seriesDataWindows.insert(udasBlock->seriesHash(), seriesDataWindow);
		}
		else
		{
			seriesDataWindow = seriesDataWindowIter.value();
			if (seriesDataWindow->isVisible() == false)
				seriesDataWindow->setVisible(true);

			seriesDataWindow->raise();
		}
	}
}

void EPMViewerWindow::onVoltageDataDoubleClick
(
	HashType channelHash
)
{
	SeriesDataWindow* seriesDataWindow{Q_NULLPTR};

	UDASBlock udasBlock = _udasFile.getVoltageBlockByChannelHash(channelHash);
	if (udasBlock.isNull() == false)
	{
		auto seriesDataWindowIter = _seriesDataWindows.find(udasBlock->seriesHash());
		if (seriesDataWindowIter == _seriesDataWindows.end())
		{
			seriesDataWindow = new SeriesDataWindow;
			seriesDataWindow->setWindowTitle("Series Data - " + udasBlock->seriesName());
			seriesDataWindow->setData(udasBlock);
			seriesDataWindow->show();

			_seriesDataWindows.insert(udasBlock->seriesHash(), seriesDataWindow);
		}
		else
		{
			seriesDataWindow = seriesDataWindowIter.value();
			if (seriesDataWindow->isVisible() == false)
				seriesDataWindow->setVisible(true);

			seriesDataWindow->raise();
		}
	}
}

void EPMViewerWindow::onPowerDataDoubleClick
(
	HashType channelHash
)
{
	SeriesDataWindow* seriesDataWindow{Q_NULLPTR};

	UDASBlock udasBlock = _udasFile.getPowerBlockByChannelHash(channelHash);
	if (udasBlock.isNull() == false)
	{
		auto seriesDataWindowIter = _seriesDataWindows.find(udasBlock->seriesHash());
		if (seriesDataWindowIter == _seriesDataWindows.end())
		{
			seriesDataWindow = new SeriesDataWindow;
			seriesDataWindow->setWindowTitle("Series Data - " + udasBlock->seriesName());
			seriesDataWindow->setData(udasBlock);
			seriesDataWindow->show();

			_seriesDataWindows.insert(udasBlock->seriesHash(), seriesDataWindow);
		}
		else
		{
			seriesDataWindow = seriesDataWindowIter.value();
			if (seriesDataWindow->isVisible() == false)
				seriesDataWindow->setVisible(true);

			seriesDataWindow->raise();
		}
	}
}

void EPMViewerWindow::on__zoomOnCursorButton_clicked()
{
	_currentChart->zoomToCursor();
	_voltageChart->zoomToCursor();
}

void EPMViewerWindow::on__cursorStatsRadio_clicked()
{
	populateStatisticsTable();
}

void EPMViewerWindow::on__allStatsRadio_clicked()
{
	populateStatisticsTable();
}

void EPMViewerWindow::on__t0_textChanged(const QString &t0)
{
	bool okay{false};

	qreal logicalCordinate = t0.toDouble(&okay);
	if (okay)
	{
		QSignalBlocker signalBlock1(_currentChart);
		QSignalBlocker signalBlock2(_voltageChart);
		QSignalBlocker signalBlock3(_powerChart);

		_currentChart->setCursorT0(logicalCordinate);
		_voltageChart->setCursorT0(logicalCordinate);
		_powerChart->setCursorT0(logicalCordinate);

		updateStatistics();
		populateStatisticsTable();
	}
}

void EPMViewerWindow::on__t1_textChanged(const QString &t1)
{
	bool okay{false};

	qreal logicalCordinate = t1.toDouble(&okay);
	if (okay)
	{
		QSignalBlocker signalBlock1(_currentChart);
		QSignalBlocker signalBlock2(_voltageChart);
		QSignalBlocker signalBlock3(_powerChart);

		_currentChart->setCursorT1(logicalCordinate);
		_voltageChart->setCursorT1(logicalCordinate);
		_powerChart->setCursorT1(logicalCordinate);

		updateStatistics();
		populateStatisticsTable();
	}
}

void EPMViewerWindow::on__exportButton_clicked()
{
	QString exportDirectory = _preferences->exportLocation();
	bool exportOnlyActiveItems{_preferences->exportSelectedItems()};
	bool exportByTimespan{_preferences->useTimespan()};
	bool exportAsCSV{_preferences->useCSV()};

	HashTuples hashTuples;

	for (auto rowIndex: range(_channelTable->rowCount()))
	{
		QString category;
		HashType channelHash;
		bool currentState, voltageState, powerState;
		HashType currentSeriesHash;
		HashType voltageSeriesHash;
		HashType powerSeriesHash;

		if (_channelTable->getChannelEntry(rowIndex, category, channelHash, currentState, currentSeriesHash, voltageState, voltageSeriesHash, powerState, powerSeriesHash))
		{
			if (exportOnlyActiveItems)
			{
				if (currentState || voltageState)
					hashTuples.push_back(std::make_tuple(channelHash, currentState ? currentSeriesHash : 0, voltageState ? voltageSeriesHash : 0, powerState ? powerSeriesHash : 0));
			}
			else
			{
				hashTuples.push_back(std::make_tuple(channelHash, currentSeriesHash, voltageSeriesHash, powerState ? powerSeriesHash : 0));
			}
		}

		if (_powerChannelTable->getChannelEntry(rowIndex, category, channelHash, currentState, currentSeriesHash, voltageState, voltageSeriesHash, powerState, powerSeriesHash))
		{
			if (exportOnlyActiveItems)
			{
				if (powerState)
					hashTuples.push_back(std::make_tuple(channelHash, currentState ? currentSeriesHash : 0, voltageState ? voltageSeriesHash : 0, powerState ? powerSeriesHash : 0));
			}
			else
			{
				hashTuples.push_back(std::make_tuple(channelHash, currentSeriesHash, voltageSeriesHash, powerSeriesHash));
			}
		}
	}

	bool result {false};
	QString errorMessage{"Empty Hash Tuple"};

	if (hashTuples.empty() == false)
	{
		if (exportByTimespan)
		{
			qreal start{_currentChart->cursorT0()};
			qreal end{_currentChart->cursorT1()};

			_udasFile.setExportTimeSpan(start, end);
		}

		if (_excelAvailable && exportAsCSV == false)
			result = _udasFile.exportAsExcel(exportDirectory, hashTuples, _preferences->quitExcelOnFinish());
		else
			result = _udasFile.exportAsCVS(exportDirectory, hashTuples);

		if (result == false)
			errorMessage = _udasFile.lastErrorMessage();
	}

	if (result == false)
	{
		QMessageBox::warning(this, "Export Error", errorMessage);
	}
}

void EPMViewerWindow::on__locateExportFolder_clicked()
{
	launchFolder(_preferences->exportLocation());
}

void EPMViewerWindow::on__totalAvgCurrentCheckBox_toggled(bool checked)
{
	if(checked)
		_totalAvgCurrent->setText(QString::number(_totalAverageCurrent, 'f', 3));
	else
		_totalAvgCurrent->clear();
}

void EPMViewerWindow::on__resetButton_clicked()
{
	_t0->setText(QString::number(_udasFile._duration/4, 'f'));
	_t1->setText(QString::number(_udasFile._duration * 3/4, 'f'));
}

void EPMViewerWindow::on__maxButton_clicked()
{
	_t0->setText(QString::number(0.0, 'f'));
	_t1->setText(QString::number(_udasFile._duration, 'f'));
}
