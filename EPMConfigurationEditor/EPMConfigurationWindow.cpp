// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "EPMConfigurationWindow.h"

// EPMConfigurationEditor
#include "AboutDialog.h"
#include "CompileFileDialog.h"
#include "EPMCompiler.h"
#include "EPMEditorDefines.h"
#include "EPMConfigurationApplication.h"

#ifdef Q_OS_WIN
#include "QTExcel.h"
#endif

// libEPM
#include "ColorConversion.h"
#include "EPMConfigFile.h"
#include "EPMIndexType.h"

// UIlib
#include "CustomValidator.h"
#include "TableCheckBox.h"
#include "TableComboBox.h"

// QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ApplicationEnhancements.h"
#include "ColorSwatch.h"
#include "Range.h"
#include "SystemInformation.h"

// QCommonConsole
#include "ConsoleApplicationEnhancements.h"

// Qt
#include <QCloseEvent>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QWindow>

const QString kWindowTitle(QStringLiteral("EPM Configuration Editor"));

QPoint EPMConfigurationWindow::_nextWindowPos(20, 20);
const QByteArray kRow{QByteArrayLiteral("row")};
const int kHashRole(Qt::UserRole);


EPMConfigurationWindow::EPMConfigurationWindow
(
	QWidget* parent
) :
	QMainWindow(parent),
	_recentFiles(kAppName, "EPMConfiguration")
{
	setAttribute(Qt::WA_DeleteOnClose);

	AlpacaSettings settings(kAppName);

	_kratosMapping = KratosMapping(new _KratosMapping);

	setupUi(this);

	QSize windowSize = settings.value("size", this->size()).toSize();
	QPoint windowPosition = settings.value("pos", _nextWindowPos).toPoint();

#ifdef Q_OS_WINDOWS
	actionImport_Excel_Template->setEnabled(true);
	actionImport_Excel_Template->setToolTip("Import Power Measurement Excel Template");
#else
	actionImport_Excel_Template->setEnabled(false);
#endif

	_nwgt = new NotificationWidget(_statusBar);

	if (_nwgt != Q_NULLPTR)
		_statusBar->addPermanentWidget(_nwgt);

	move(windowPosition);
	resize(windowSize);

	const QPoint horizontalMove(20, 20);

	_nextWindowPos += horizontalMove;
	if (_nextWindowPos.y() > 100)
		_nextWindowPos.setY(horizontalMove.y());

	actionCompile_to_Config->setEnabled(false);
	actionDelete_Channel->setEnabled(false);

	rebuildRecents();

	// lambda
	connect(_actionContents, &QAction::triggered, [=]{ startLocalBrowser(docsRoot() + "/getting-started/06-EPM-Config-Editor.html");});
	connect(_actionRateMe, &QAction::triggered, [=] { EPMConfigurationApplication::epmConfigAppInstance()->showRateDialog();});

	setupColumnProperties();

	_channelSettings->setItemDelegateForColumn(kIndexCol, new CustomValidator(_channelSettings, eNumericOnlyValidator));
	_channelSettings->setItemDelegateForColumn(kNameCol, new CustomValidator(_channelSettings, eNoSemiColonValidator));
	_channelSettings->setItemDelegateForColumn(kCategoryCol, new CustomValidator(_channelSettings, eNoSemiColonValidator));
	_channelSettings->setItemDelegateForColumn(kResisterCol, new CustomValidator(_channelSettings, eFloatingPointValidator));
	_channelSettings->setItemDelegateForColumn(kDescriptionCol, new CustomValidator(_channelSettings, eNoSemiColonValidator));
}

EPMConfigurationWindow::~EPMConfigurationWindow()
{
	if (_nwgt != Q_NULLPTR)
	{
		delete _nwgt;
		_nwgt = Q_NULLPTR;
	}

	if (_epmConfigFile != Q_NULLPTR)
	{
		delete _epmConfigFile;
		_epmConfigFile = Q_NULLPTR;
	}
}

void EPMConfigurationWindow::setEPMConfigFile
(
	EPMConfigFile* epmConfigFile
)
{
	Q_ASSERT(epmConfigFile != Q_NULLPTR);

	if (epmConfigFile != _epmConfigFile)
	{
		if (_epmConfigFile != Q_NULLPTR)
		{
			delete _epmConfigFile;
		}
	}

	_epmConfigFile = epmConfigFile;

	QString windowTitle(kWindowTitle);
	if (_epmFilePath.isEmpty() == false)
	{
		QFileInfo epmFileInfo(_epmFilePath);

		windowTitle += " - " +  epmFileInfo.fileName();
	}

	setWindowTitle(windowTitle);

	populateFields();
}

void EPMConfigurationWindow::setupColumnProperties()
{
	_channelSettings->setFocusPolicy(Qt::NoFocus);

	QHeaderView* headerView = _channelSettings->horizontalHeader();

	_channelSettings->resizeColumnToContents(kChartColor);
	headerView->setSectionResizeMode(kChartColor, QHeaderView::Fixed);

	_channelSettings->resizeColumnToContents(kChannelEnabled);
	headerView->setSectionResizeMode(kChannelEnabled, QHeaderView::Fixed);

	_channelSettings->setColumnWidth(kTypeCol, 80);
	headerView->setSectionResizeMode(kTypeCol, QHeaderView::Fixed);

	_channelSettings->resizeColumnToContents(kIndexCol);
	headerView->setSectionResizeMode(kIndexCol, QHeaderView::Fixed);

	_channelSettings->resizeColumnToContents(kNameCol);
	headerView->setSectionResizeMode(kNameCol, QHeaderView::Stretch);

	_channelSettings->setColumnWidth(kCategoryCol, 80);
	headerView->setSectionResizeMode(kCategoryCol, QHeaderView::Fixed);

	_channelSettings->resizeColumnToContents(kResisterCol);
	headerView->setSectionResizeMode(kResisterCol, QHeaderView::Fixed);

	_channelSettings->setColumnWidth(kDescriptionCol, 160);
	headerView->setSectionResizeMode(kDescriptionCol, QHeaderView::Stretch);

	connect(_channelSettings, &QTableWidget::itemChanged, this, &EPMConfigurationWindow::onTableItemChanged);
}

void EPMConfigurationWindow::changeEvent
(
	QEvent *e
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

void EPMConfigurationWindow::closeEvent
(
	QCloseEvent* event
)
{
	AlpacaSettings settings(kAppName);

	settings.setValue("pos", pos());
	settings.setValue("size", size());

	if (_epmConfigFile != Q_NULLPTR)
	{
		if (_epmConfigFile->isDirty())
		{
			QMessageBox::StandardButton result = QMessageBox::question(this, "Save Configuration", "The configuration file has changed.  Do you want to save it?",
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
			switch (result)
			{
			case QMessageBox::Yes:
				if (save() == false)
					event->ignore();
				break;
			case QMessageBox::No: event->accept(); break;
			case QMessageBox::Cancel: event->ignore(); break;
			default: break;
			}
		}
	}
	else
	{
		QMainWindow::closeEvent(event);
	}
}

void EPMConfigurationWindow::on_actionNew_triggered()
{
	EPMConfigFile* epmConfigFile = new EPMConfigFile;
	if (epmConfigFile != Q_NULLPTR)
	{
		if (_epmConfigFile != Q_NULLPTR)
		{
			EPMConfigurationWindow* w = new EPMConfigurationWindow;
			w->setEPMConfigFile(epmConfigFile);
			w->show();
		}
		else
		{
			setEPMConfigFile(epmConfigFile);
		}

		connect(this, &EPMConfigurationWindow::checkAuthorExists, this, &EPMConfigurationWindow::addAuthorIfNotExists);
	}
}

void EPMConfigurationWindow::on_actionOpen_triggered()
{
	if (_epmConfigFile != Q_NULLPTR)
	{
		EPMConfigurationWindow* openWindow{Q_NULLPTR};

		openWindow = new EPMConfigurationWindow;
		openWindow->show();
		openWindow->open();
	}
	else
	{
		open();
	}
}

void EPMConfigurationWindow::onActionRecentMenuTriggered()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action != Q_NULLPTR)
	{
		QString filePath = action->data().toString();
		if (filePath.isEmpty() == false)
		{
			if (_epmConfigFile != Q_NULLPTR)
			{
				EPMConfigurationWindow* openWindow;

				openWindow = new EPMConfigurationWindow;
				openWindow->show();
				openWindow->openFile(filePath);
			}
			else
			{
				openFile(filePath);
			}
		}
	}
}

void EPMConfigurationWindow::on_actionSave_triggered()
{
	save();
}

void EPMConfigurationWindow::on_actionSave_As_triggered()
{
	saveAs();
}

void EPMConfigurationWindow::on_actionImport_Excel_Template_triggered()
{
	importExcel();
}

void EPMConfigurationWindow::on_actionExit_triggered()
{
	auto topLevelWindows = QGuiApplication::topLevelWindows();
	for(const auto& topWindow: topLevelWindows)
		topWindow->close();

	QCoreApplication::instance()->exit();
}

void EPMConfigurationWindow::on_actionAdd_Channel_triggered()
{
	EPMChannel addMe = EPMChannel(new _EPMChannel);

	if (testForEPMFile())
	{
		addMe->setKratosMapping(_kratosMapping);
		_epmConfigFile->addChannel(addMe);

		populateFields();

		emit checkAuthorExists();
	}
}

void EPMConfigurationWindow::on_actionDelete_Channel_triggered()
{
	QList<QTableWidgetItem*> selectedItems = _channelSettings->selectedItems();
	if (selectedItems.isEmpty() == false)
	{
		QTableWidgetItem* firstItem = selectedItems.at(0);
		if (firstItem != Q_NULLPTR)
		{
			quint32 row = firstItem->row();
			_epmConfigFile->removeChannel(row);

			populateFields();
		}
	}
}

void EPMConfigurationWindow::on_actionCompile_to_Config_triggered()
{
	if (_epmConfigFile != Q_NULLPTR)
	{
		CompileFileDialog compileFileDialog(this, _epmFilePath);

		if (compileFileDialog.exec() == QDialog::Accepted)
		{
			KratosMapping kratosMapping = KratosMapping(new _KratosMapping);
			EPMCompiler epmCompiler(_epmConfigFile);

			kratosMapping->setRCMMapping(compileFileDialog.useRCMMapping());
			kratosMapping->useKratosMapping(compileFileDialog.useKratos());

			epmCompiler.setKratosMapping(kratosMapping);

			if (epmCompiler.compile(compileFileDialog.compileFile()) == false)
			{
				// todo
			}
		}
	}
}

void EPMConfigurationWindow::on_actionAbout_triggered()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.setTitle("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">EPM Configuration Editor</span></p></body></html>");

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

void EPMConfigurationWindow::on_actionSubmit_Bug_Report_triggered()
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/Alpaca/bin/BugWriter"; // Linux Sucks
#else
	QString program = "BugWriter";
#endif

	QStringList arguments;
	arguments << "product:Alpaca";
	arguments << "prodversion:" + kProductVersion;
	arguments << "application:EPM_Configuration_Editor";
	arguments << "appversion:" + kAppVersion;
	arguments << "listports:true";
	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	process->startDetached();
}

void EPMConfigurationWindow::on__channelSettings_itemClicked
(
	QTableWidgetItem* item
)
{
	if (item->column() == kChannelEnabled)
	{
		bool checked = item->checkState() == Qt::Checked ? true : false;
		quint32 row = static_cast<quint32>(item->row());

		EPMChannel epmChannel = _epmConfigFile->at(row);
		epmChannel->setActive(checked);
		_epmConfigFile->updateChannel(row, epmChannel);
	}

	actionDelete_Channel->setEnabled(true);
}

void EPMConfigurationWindow::on__channelSettings_itemDoubleClicked
(
	QTableWidgetItem* item
)
{
	on__channelSettings_itemClicked(item);
}

void EPMConfigurationWindow::onColorEditRequested
(
	quint32 row
)
{
	if (row != kInvalidColorRow)
	{
		EPMChannel epmChannel = _epmConfigFile->at(row);
		if (epmChannel.isNull() == false)
		{
			BasicColor basicChannelColor = epmChannel->channelColor();
			QColor channelColor = ColorConversion::BasicToColor(basicChannelColor);

			QColor colorResult = QColorDialog::getColor(channelColor, this, "Select the channel's chart color");
			if (colorResult.isValid())
			{
				if (colorResult != channelColor)
				{
					ColorSwatch* colorSwatch = qobject_cast<ColorSwatch*>(sender());
					if (colorSwatch != Q_NULLPTR)
					{
						BasicColor basicColorResult = ColorConversion::ColorToBasic(colorResult);

						epmChannel->setChannelColor(basicColorResult);
						_epmConfigFile->updateChannel(row, epmChannel);
						colorSwatch->setColor(colorResult);
					}
				}
			}
		}
	}
}

void EPMConfigurationWindow::addAuthorIfNotExists()
{
	if (_epmConfigFile != Q_NULLPTR)
	{
		if (_epmConfigFile->author().isEmpty())
		{
			SystemInformation sysInfo;
			QString	author = sysInfo.userName();

			_epmConfigFile->setAuthor(author);
			_author->setText(author);
		}
	}
}

void EPMConfigurationWindow::onChannelEnabledStateChanged(bool newState)
{
	TableCheckBox* cb = qobject_cast<TableCheckBox*>(sender());

	if (cb)
	{
		int row = cb->property(kRow).toInt();
		if (_epmConfigFile != Q_NULLPTR)
		{
			EPMChannel epmChannel = _epmConfigFile->at(row);
			epmChannel->setActive(newState);
			_epmConfigFile->updateChannel(row, epmChannel);
		}
	}
}

void EPMConfigurationWindow::onChannelTypeChanged(int index)
{
	TableComboBox* cb = qobject_cast<TableComboBox*>(sender());

	if (cb)
	{
		int row = cb->property(kRow).toInt();
		if (_epmConfigFile != Q_NULLPTR)
		{
			EPMChannel epmChannel = _epmConfigFile->at(row);
			epmChannel->setIndexType(EPMIndexType(index));
			_epmConfigFile->updateChannel(row, epmChannel);
		}
	}
}

void EPMConfigurationWindow::onTableItemChanged(QTableWidgetItem* twi)
{
	HashType row = twi->data(kHashRole).toInt();

	EPMChannel epmChannel = _epmConfigFile->at(row);

	QString itemText = twi->text();

	switch(twi->column())
	{
	case kIndexCol:
		epmChannel->setIndex(itemText.toInt());
		_epmConfigFile->updateChannel(row, epmChannel);
		break;
	case kNameCol:
		epmChannel->setChannelName(itemText);
		_epmConfigFile->updateChannel(row, epmChannel);
		break;
	case kCategoryCol:
		epmChannel->setCategory(itemText);
		_epmConfigFile->updateChannel(row, epmChannel);
		break;
	case kResisterCol:
		epmChannel->_resistorValue = itemText.toDouble();
		_epmConfigFile->updateChannel(row, epmChannel);
		break;
	case kDescriptionCol:
		epmChannel->_description = itemText;
		_epmConfigFile->updateChannel(row, epmChannel);
		break;
	default:
		break;
	}
}

bool EPMConfigurationWindow::open()
{
	bool result(false);

	AlpacaSettings settings(kAppName);

	QString initialDir = settings.value(kEPMConfigPath, documentsDataPath(kAppName)).toString();

	QString filters = "EPM Channel Configuration (*.ccnf)";

	QString fileName = QFileDialog::getOpenFileName(this, "Open an EPM Configuration", initialDir, filters);
	if (fileName.isEmpty() == false)
	{
		QFileInfo fileInfo(fileName);

		settings.setValue(kEPMConfigPath, fileInfo.absolutePath());

		result = openFile(fileName);
	}

	return result;
}

bool EPMConfigurationWindow::openFile
(
	const QString& filePath
)
{
	bool result(false);

	EPMConfigFile* epmFile = new EPMConfigFile;
	if (epmFile != Q_NULLPTR)
	{
		if (epmFile->open(filePath, _kratosMapping) == true)
		{
			_epmFilePath = filePath;
			setEPMConfigFile(epmFile);

			populateFields();

			_recentFiles.addFile(filePath);
			rebuildRecents();

			result = true;
		}
	}

	return result;
}

bool EPMConfigurationWindow::importExcel()
{
	bool result(false);

#ifdef Q_OS_WIN
	AlpacaSettings settings(kAppName);

	QString initialDir = settings.value(kLastImportPath, documentsDataPath(kAppName)).toString();

	QString filters = "Excel Power Template File (*.xlsx)";

	QString fileName = QFileDialog::getOpenFileName(this, "Open an Excel Power Template File File", initialDir, filters);
	if (fileName.isEmpty() == false)
	{
		QFileInfo fileInfo(fileName);

		settings.setValue(kLastImportPath, fileInfo.absolutePath());

		EPMConfigFile* epmFile = new EPMConfigFile;
		if (epmFile != Q_NULLPTR)
		{
			QTExcel qtExcel;

			if (qtExcel.open(fileName, true) == true)
			{
				int fileFormat;
				QString value;
				QString columnKey;
				int intValue;
				int row(4);
				bool gettingCommonSettings = true;
				bool gettingChannels(true);

				columnKey = qtExcel.getCellValue("A1").toString().toLower();

				if (columnKey == "file format")
				{
					QString reference;

					result = true;
					fileFormat = qtExcel.getCellValue("B1").toInt();

					while (gettingCommonSettings)
					{
						reference = QString("A%1").arg(row);
						columnKey = qtExcel.getCellValue(reference).toString().toLower();
						if (columnKey == ("type"))
						{
							gettingCommonSettings = false;
						}
						else
						{
							reference = QString("B%1").arg(row);
							if (columnKey == "target")
							{
								value = qtExcel.getCellValue(reference).toString();
								epmFile->setTarget(value);
							}
							else if (columnKey == "label")
							{
								value = qtExcel.getCellValue(reference).toString();
								epmFile->setLabel(value);
							}
							else if (columnKey == "description")
							{
								value = qtExcel.getCellValue(reference).toString();
								epmFile->setDescription(value);
							}
							else if (columnKey == "spm_version")
							{
								intValue = qtExcel.getCellValue(reference).toInt() - 4;
								epmFile->setSpmVersion(static_cast<SPMVersion>(intValue));
							}
						}

						row++;

						if (row > 100) // don't want a run away loop
						{
							gettingCommonSettings = false;
							gettingChannels = false;
							row = 1000;
						}
					}

					if (fileFormat == 1)
						epmFile->setSpmVersion(eLegacyVersion);

					while (gettingChannels)
					{
						try
						{
							reference = QString("A%1").arg(row);
							value = qtExcel.getCellValue(reference).toString();
							columnKey = value.toUpper();

							if (columnKey.isEmpty())
							{
								gettingChannels = false;
							}
							else if (columnKey == kMarker || columnKey == kSPM || columnKey == kRCM)
							{
								EPMChannel epmChannel = EPMChannel(new _EPMChannel);
								bool okay;

								epmChannel->setChannelColor(epmFile->nextColor());
								epmChannel->setIndexType(StringToEPMIndexType(columnKey));
								epmChannel->setIndex(qtExcel.getCellValue(QString("B%1").arg(row)).toUInt(&okay));
								if (!okay)
									epmChannel->setIndex(static_cast<quint32>(-1));
								epmChannel->setChannelName(qtExcel.getCellValue(QString("C%1").arg(row)).toString());
								epmChannel->setCategory(qtExcel.getCellValue(QString("D%1").arg(row)).toString());
								epmChannel->_description = qtExcel.getCellValue(QString("E%1").arg(row)).toString();
								epmChannel->_resistorValue = qtExcel.getCellValue(QString("F%1").arg(row)).toDouble(&okay);
								if (!okay)
									epmChannel->_resistorValue = -1;
								value = qtExcel.getCellValue(QString("G%1").arg(row)).toString().toLower();

								epmChannel->setActive(value == "t");

								epmFile->addChannel(epmChannel);
							}
						}
						catch (...)
						{

						}

						row++;
					}
				}
			}

			qtExcel.quit();

			epmFile->sort(_EPMChannel::eIndex);
			epmFile->sort(_EPMChannel::eType);

			if (fileName.endsWith(".xlsx"))
				_epmFilePath = fileName.replace(".xlsx", ".ccnf");

			setEPMConfigFile(epmFile);
			populateFields();
		}
	}

#endif

	return result;
}

bool EPMConfigurationWindow::save()
{
	bool result(true);

	if (_epmConfigFile != Q_NULLPTR)
	{
		if (_epmFilePath.isEmpty() == true)
		{
			result = saveAs();
		}
		else
		{
			_epmConfigFile->save(_epmFilePath);

			QFileInfo fileInfo(_epmFilePath);
			_statusBar->showMessage("EPM File " + fileInfo.fileName() + " saved", 5);
		}
	}

	return result;
}

bool EPMConfigurationWindow::saveAs()
{
	bool result(false);

	if (_epmConfigFile != Q_NULLPTR)
	{
		AlpacaSettings settings(kAppName);
		QString lastPath;

		lastPath = settings.value(kEPMConfigPath, documentsDataPath(kAppName)).toString();

		QString path = QFileDialog::getSaveFileName(Q_NULLPTR, "Save EPM Channel Configuration File",
			lastPath, tr("EPM Channel Configuration File (*.ccnf)"));
		if (path.isEmpty() == false)
		{
			if (lastPath != path)
			{
				settings.setValue(kEPMConfigPath, path);
			}

			_epmFilePath = path;
			_epmConfigFile->save(path);
			setEPMConfigFile(_epmConfigFile);
			result = true;
		}
	}

	return result;
}

void EPMConfigurationWindow::populateFields()
{
	_author->setText(_epmConfigFile->author());
	_target->setText(_epmConfigFile->target());
	_platform->setText(_epmConfigFile->label());
	_description->setText(_epmConfigFile->description());

	switch (_epmConfigFile->spmVersion())
	{
	case eLegacyVersion:
		_spmVersion->setCurrentIndex(0);
		break;

	case eAlpacaVersion:
		_spmVersion->setCurrentIndex(1);
		break;

	case eSPMVersion:
		_spmVersion->setCurrentIndex(2);
		break;

	case eSPM_MTPVersion:
		_spmVersion->setCurrentIndex(3);
		break;

	case eSPM_CDPVersion:
		_spmVersion->setCurrentIndex(4);
		break;
	}

	quint32 channelCount = _epmConfigFile->channelCount();
	_channelSettings->setRowCount(channelCount);

	for (auto row: range(channelCount))
	{
		QTableWidgetItem* twi{Q_NULLPTR};
		TableCheckBox* checkBox{Q_NULLPTR};
		TableComboBox* comboBox{Q_NULLPTR};

		EPMChannel epmChannel = _epmConfigFile->at(row);

		ColorSwatch* colorSwatch = new ColorSwatch;

		if (colorSwatch != Q_NULLPTR)
		{
			colorSwatch->setRow(row);
			connect(colorSwatch, &ColorSwatch::editRequested, this, &EPMConfigurationWindow::onColorEditRequested);

			colorSwatch->setColor(ColorConversion::BasicToColor(epmChannel->channelColor()));
			colorSwatch->setGeometry(0, 0, 20, 20);
			_channelSettings->setCellWidget(row, kChartColor, colorSwatch);

			colorSwatch->show();
		}

		checkBox = new TableCheckBox(_channelSettings);
		if (checkBox != Q_NULLPTR)
		{
			checkBox->setCheckState(epmChannel->active());
			checkBox->setProperty(kRow, row);
			connect(checkBox, SIGNAL(checkStateChanged(bool)), this, SLOT(onChannelEnabledStateChanged(bool)));
			_channelSettings->setCellWidget(row, kChannelEnabled, checkBox);
		}

		comboBox = new TableComboBox(_channelSettings);
		if (comboBox != Q_NULLPTR)
		{
			comboBox->setupItems({kUnset, kMarker, kRCM, kSPM}, kUnset);
			comboBox->setCurrentIndex(epmChannel->indexType());
			comboBox->setProperty(kRow, row);
			connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelTypeChanged(int)));
			_channelSettings->setCellWidget(row, kTypeCol, comboBox);
		}

		twi = new QTableWidgetItem(QString::number(epmChannel->index()));
		if (twi)
		{
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setData(kHashRole, row);
			_channelSettings->setItem(row, kIndexCol, twi);
		}

		twi = new QTableWidgetItem(epmChannel->channelName());
		if (twi)
		{
			twi->setData(kHashRole, row);
			_channelSettings->setItem(row, kNameCol, twi);
		}

		twi = new QTableWidgetItem(epmChannel->category());
		if (twi)
		{
			twi->setData(kHashRole, row);
			_channelSettings->setItem(row, kCategoryCol, twi);
		}

		twi = new QTableWidgetItem(epmChannel->_description);
		if (twi)
		{
			twi->setData(kHashRole, row);
			_channelSettings->setItem(row, kDescriptionCol, twi);
		}

		QString resistorValue = qFuzzyIsNull(epmChannel->_resistorValue) ? "-" : QString::number(epmChannel->_resistorValue);
		twi = new QTableWidgetItem(resistorValue);
		if (twi)
		{
			twi->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			twi->setData(kHashRole, row);
			_channelSettings->setItem(row, kResisterCol, twi);
		}
	}

	// Is there anything to delete, compile?
	if (channelCount == 0)
	{
		actionDelete_Channel->setEnabled(false);
		actionCompile_to_Config->setEnabled(false);
	}
	else
	{
		actionCompile_to_Config->setEnabled(true);
	}
}

void EPMConfigurationWindow::rebuildRecents()
{
	buildRecentsMenu(menuOpen_Recent, _recentFiles, this, SLOT(onActionRecentMenuTriggered()));
}

bool EPMConfigurationWindow::testForEPMFile()
{
	if (_epmConfigFile == Q_NULLPTR)
	{
		on_actionNew_triggered();
	}

	return _epmConfigFile != Q_NULLPTR;
}

void EPMConfigurationWindow::on__author_textChanged(const QString &arg1)
{
	if (testForEPMFile())
	{
		if (arg1.isEmpty() == false)
		{
			_epmConfigFile->setAuthor(arg1);
			_author->setText(arg1);
		}
	}
}

void EPMConfigurationWindow::on__author_editingFinished()
{
	if (_author->text().isEmpty())
	{
		if (_nwgt)
			_nwgt->insertNotification("Author cannot be left blank. Add your Qualcomm username as author. Eg: Biswajit Roy <biswroy@qti.qualcomm.com>", eWarnNotification);
	}
}

void EPMConfigurationWindow::on__target_textChanged(const QString &arg1)
{
	if (testForEPMFile())
	{
		_epmConfigFile->setTarget(arg1);
		_target->setText(_epmConfigFile->target());

		emit checkAuthorExists();
	}
}

void EPMConfigurationWindow::on__platform_textChanged(const QString &arg1)
{
	if (testForEPMFile())
	{
		_epmConfigFile->setLabel(arg1);
		_platform->setText(_epmConfigFile->label());

		emit checkAuthorExists();
	}
}

void EPMConfigurationWindow::on__description_textChanged(const QString &arg1)
{
	if (testForEPMFile())
	{
		_epmConfigFile->setDescription(arg1);
		_description->setText(_epmConfigFile->description());

		emit checkAuthorExists();
	}
}

void EPMConfigurationWindow::on__spmVersion_currentIndexChanged(int index)
{
	if(testForEPMFile())
	{
		_epmConfigFile->setSpmVersion(static_cast<SPMVersion>(index));
		_spmVersion->setCurrentIndex(_epmConfigFile->spmVersion());

		emit checkAuthorExists();
	}
}
