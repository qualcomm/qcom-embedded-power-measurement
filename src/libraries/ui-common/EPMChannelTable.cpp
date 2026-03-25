// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMChannelTable.h"

// libEPM
#include "ui_EPMChannelTable.h"
#include "ColorConversion.h"

// QCommon
#include "ColorSwatch.h"
#include "Range.h"

// Qt
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QResizeEvent>
#include <QScrollBar>

const QByteArray kChannelNumber{QByteArrayLiteral("ChannelNumber")};

const QByteArray kChannelHash{QByteArrayLiteral("ChannelHash")};
const int kItemChannelHash{Qt::UserRole + 1};

const QByteArray kSeriesHash{QByteArrayLiteral("SeriesHash")};
const int kItemSeriesHash{Qt::UserRole + 2};

const int kItemCategory{Qt::UserRole + 3};
const QString kCheckBox(QStringLiteral("checkbox"));

const QByteArray kInvalidChannelError(QByteArrayLiteral("Found an empty channel name while parsing EPM data."));

EPMChannelTable::EPMChannelTable
(
	QWidget* parent
) :
	QTableWidget(parent),
	_ui(new Ui::EPMChannelTableClass)
{
	_ui->setupUi(this);

	_doubleClickIcon = QIcon(":/doubleclick.png");

	connect(this, &EPMChannelTable::cellDoubleClicked, this, &EPMChannelTable::onCellDoubleClicked);
	connect(this, &EPMChannelTable::customContextMenuRequested, this, &EPMChannelTable::onCustomContextMenuRequested);

	setColumns(NoColumns);
}

EPMChannelTable::~EPMChannelTable()
{
	if (_ui)
	{
		delete _ui;
		_ui = Q_NULLPTR;
	}
}

void EPMChannelTable::setEPMProject(EPMProject epmProject)
{
	_epmProject = epmProject;

	populateChannels();
}

void EPMChannelTable::setChannelColor
(
	HashType channelHash,
	QColor newColor
)
{
	for (auto row: range(rowCount()))
	{
		QTableWidgetItem* twi;

		twi = item(row, eTitleColumn);
		if (twi != Q_NULLPTR)
		{
			HashType hash = twi->data(kItemChannelHash).toULongLong();
			if (channelHash == hash)
			{
				ColorSwatch* colorSwatch = qobject_cast<ColorSwatch*>(cellWidget(row, eColorColumn));
				if (colorSwatch != Q_NULLPTR)
				{
					colorSwatch->setColor(newColor);
				}
			}
		}
	}
}

void EPMChannelTable::setColumns(ColumnOptions columnOptions)
{
	_columns = columnOptions;

	updateTableColumns();
}

void EPMChannelTable::populateChannels()
{		
	QCheckBox* checkBox;
	QTableWidgetItem* twi;

	quint32 channelCount = _epmProject->channelCount();
	setRowCount(channelCount);

	for (auto channelIndex: range(channelCount))
	{
		EPMChannel currentChannel;
		EPMChannel voltageChannel;
		EPMChannel powerChannel;

		_epmProject->getChannelPair(channelIndex, currentChannel, voltageChannel);

		ColorSwatch* colorSwatch = new ColorSwatch;

		connect(colorSwatch, &ColorSwatch::editRequested, this, &EPMChannelTable::onColorSwatchEditRequested);

		colorSwatch->setRow(channelIndex);
		colorSwatch->setColor(ColorConversion::BasicToColor(voltageChannel->channelColor()));
		colorSwatch->setGeometry(0, 0, 20, 20);
		setCellWidget(channelIndex, eColorColumn, colorSwatch);
		colorSwatch->show();

		twi = new QTableWidgetItem(voltageChannel->channelName());
		twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
		twi->setData(kItemChannelHash, voltageChannel->channelHash());
		twi->setData(kItemCategory, voltageChannel->category());

		setItem(channelIndex, eTitleColumn, twi);

		if (_columns.testFlag(TypeColumnVisible))
		{
			twi = new QTableWidgetItem(EPMIndexTypeToString(voltageChannel->indexType()));
			twi->setData(kItemChannelHash, voltageChannel->channelHash());
			twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
			setItem(channelIndex, eTypeColumn, twi);
		}

		if (_columns.testFlag(CurrentColumnVisible) && currentChannel.isNull() == false)
		{
			checkBox = setupTableCheckbox(channelIndex, eCurrentColumn, false, eWaveFormCurrent);
			checkBox->setProperty(kChannelNumber, currentChannel->channel());
			checkBox->setProperty(kSeriesHash, currentChannel->seriesHash());
			checkBox->setProperty(kChannelHash, currentChannel->channelHash());

			if (_columns.testFlag(DataColumnVisible))
			{
				twi = new QTableWidgetItem("y[]");
				twi->setData(kItemChannelHash, currentChannel->channelHash());
				twi->setData(kItemSeriesHash, currentChannel->seriesHash());
				twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);

				setItem(channelIndex, eCurrentDataColumn, twi);
			}
		}

		if (_columns.testFlag(VoltageColumnVisible) && voltageChannel.isNull() == false)
		{
			checkBox = setupTableCheckbox(channelIndex, eVoltageColumn, false, eWaveFormVoltage);
			checkBox->setProperty(kChannelNumber, voltageChannel->channel());
			checkBox->setProperty(kSeriesHash, voltageChannel->seriesHash());
			checkBox->setProperty(kChannelHash, voltageChannel->channelHash());

			if (_columns.testFlag(DataColumnVisible))
			{
				twi = new QTableWidgetItem("y[]");
				twi->setData(kItemChannelHash, voltageChannel->channelHash());
				twi->setData(kItemSeriesHash, voltageChannel->seriesHash());
				twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
				setItem(channelIndex, eVoltageDataColumn, twi);
			}
		}

		if (_columns.testFlag(PowerColumnVisible) && powerChannel.isNull() == false)
		{
			checkBox = setupTableCheckbox(channelIndex, ePowerColumn, false, eWaveFormPower);
			checkBox->setProperty(kChannelNumber, powerChannel->channel());
			checkBox->setProperty(kSeriesHash, powerChannel->seriesHash());
			checkBox->setProperty(kChannelHash, powerChannel->channelHash());

			if (_columns.testFlag(DataColumnVisible))
			{
				twi = new QTableWidgetItem("y[]");
				twi->setData(kItemChannelHash, powerChannel->channelHash());
				twi->setData(kItemSeriesHash, powerChannel->seriesHash());
				twi->setFlags(twi->flags() & ~Qt::ItemIsEditable);
				setItem(channelIndex, ePowerDataColumn, twi);
			}
		}

		setItem(channelIndex, eHelpColumn, new QTableWidgetItem);
	}
}

void EPMChannelTable::populateChannels(UDASFile& udasFile, PRNFile& prnFile)
{
	QCheckBox* checkBox{Q_NULLPTR};
	QTableWidgetItem* twi{Q_NULLPTR};

	clearContents();

	QList<HashType> sortedHashes = udasFile.buildSortedIVHashList();

	setRowCount(sortedHashes.count());

	int rowIndex{0};

	for (const auto& channelHash: sortedHashes)
	{
		QString channelName;
		BasicColor channelColor = prnFile.channelColor(channelHash);

		UDASBlock currentBlock = udasFile.getCurrentBlockByChannelHash(channelHash);
		if (currentBlock.isNull() == false)
		{
			channelName = currentBlock->channelName();
		}

		UDASBlock voltageBlock = udasFile.getVoltageBlockByChannelHash(channelHash);
		if (voltageBlock.isNull() == false)
		{
			if (channelName.isEmpty() == true)
				channelName = voltageBlock->channelName();
		}

		UDASBlock powerBlock = udasFile.getPowerBlockByChannelHash(channelHash);
		if (powerBlock.isNull() == false)
		{
			if (channelName.isEmpty() == true)
				channelName = powerBlock->channelName();
		}

		// We're going to process empty channel names if we move further
		if (channelName.isEmpty())
		{
			_valid = false;
			emit errorOccurred(kInvalidChannelError);
			break;
		}

		ColorSwatch* colorSwatch = new ColorSwatch;

		connect(colorSwatch, &ColorSwatch::editRequested, this, &EPMChannelTable::onColorSwatchEditRequested);

		colorSwatch->setRow(rowIndex);
		colorSwatch->setColor(ColorConversion::BasicToColor(channelColor));
		colorSwatch->setGeometry(0, 0, 20, 20);
		setCellWidget(rowIndex, eColorColumn, colorSwatch);
		colorSwatch->show();

		twi = new QTableWidgetItem(channelName);
		twi->setData(kItemChannelHash, channelHash);
		setItem(rowIndex, eTitleColumn, twi);

		if (currentBlock.isNull() == false)
		{
			checkBox = setupTableCheckbox(rowIndex, eCurrentColumn, false, eWaveFormCurrent);
			checkBox->setProperty(kChannelNumber, currentBlock->_channelNumber);
			checkBox->setProperty(kChannelHash, currentBlock->channelHash());
			checkBox->setProperty(kSeriesHash, currentBlock->seriesHash());

			twi = new QTableWidgetItem(_doubleClickIcon, "");
			setItem(rowIndex, eCurrentDataColumn, twi);
		}

		if (voltageBlock.isNull() == false)
		{
			checkBox = setupTableCheckbox(rowIndex, eVoltageColumn, false, eWaveFormVoltage);
			checkBox->setProperty(kChannelNumber, voltageBlock->_channelNumber);
			checkBox->setProperty(kChannelHash, voltageBlock->channelHash());
			checkBox->setProperty(kSeriesHash, voltageBlock->seriesHash());

			twi = new QTableWidgetItem(_doubleClickIcon, "");
			setItem(rowIndex, eVoltageDataColumn, twi);
		}

		if (powerBlock.isNull() == false)
		{
			checkBox = setupTableCheckbox(rowIndex, ePowerColumn, false, eWaveFormPower);
			checkBox->setProperty(kChannelNumber, powerBlock->_channelNumber);
			checkBox->setProperty(kChannelHash, powerBlock->channelHash());
			checkBox->setProperty(kSeriesHash, powerBlock->seriesHash());

			twi = new QTableWidgetItem(_doubleClickIcon, "");
			setItem(rowIndex, ePowerDataColumn, twi);
		}

		setItem(rowIndex, eHelpColumn, new QTableWidgetItem);

		rowIndex++;
	}

	if (_valid == true)
	{
		int markerBlockCount = udasFile.markerBlockCount();
		setRowCount(rowCount() + markerBlockCount);
		for (auto index: range(markerBlockCount))
		{
			UDASBlock markerBlock = udasFile.getMarkerBlock(index);
			if (markerBlock.isNull() == false)
			{
				QString seriesName = markerBlock->seriesName();
				ColorSwatch* colorSwatch = new ColorSwatch;

				colorSwatch->setRow(rowIndex);
				colorSwatch->setColor(ColorConversion::BasicToColor(prnFile.channelColor(markerBlock->channelHash())));

				colorSwatch->setGeometry(0, 0, 20, 20);
				colorSwatch->show();
				setCellWidget(rowIndex, eColorColumn, colorSwatch);

				twi = new QTableWidgetItem(seriesName);
				twi->setData(kItemChannelHash, markerBlock->channelHash());
				setItem(rowIndex, eTitleColumn, twi);

				checkBox = setupTableCheckbox(rowIndex, eVoltageColumn, false, eWaveFormMarker);
				checkBox->setProperty(kChannelNumber, markerBlock->_channelNumber);
				checkBox->setProperty(kChannelHash, markerBlock->channelHash());
				checkBox->setProperty(kSeriesHash, markerBlock->seriesHash());

				setItem(rowIndex, ePowerColumn, new QTableWidgetItem);
				setItem(rowIndex, eHelpColumn, new QTableWidgetItem);

				rowIndex++;
			}
		}
	}
}

bool EPMChannelTable::checked
(
	int column
)
{
	switch (column)
	{
	case eCurrentColumn:
	case eVoltageColumn:
	case ePowerColumn:
		for (auto row: range(rowCount()))
		{
			QWidget* parent = qobject_cast<QWidget*>(cellWidget(row, column));
			if (parent != Q_NULLPTR)
			{
				QCheckBox* checkBox = parent->findChild<QCheckBox*>(kCheckBox);
				if (checkBox != Q_NULLPTR)
				{
					if (checkBox->isChecked())
						return true;
				}
			}
		}
		break;

	default:
		break;
	}

	return false;
}

bool EPMChannelTable::isValid()
{
	return _valid;
}

void EPMChannelTable::clear()
{
	clearContents();
	setRowCount(0);
}

bool EPMChannelTable::getChannelEntry
(
	int rowIndex,
	QString& category,
	HashType& channelHash,
	bool& currentState,
	HashType& currentSeriesHash,
	bool& voltageState,
	HashType& voltageSeriesHash,
	bool& powerState,
	HashType& powerSeriesHash
)
{
	bool result{false};

	channelHash = 0;
	currentState = false;
	currentSeriesHash = 0;
	voltageState = false;
	voltageSeriesHash = 0;
	powerState = false;
	powerSeriesHash = 0;

	if (rowIndex >= 0 && rowIndex < rowCount())
	{
		QTableWidgetItem* twi = item(rowIndex, eTitleColumn);
		channelHash = twi->data(kItemChannelHash).toUInt();
		category = twi->data(kItemCategory).toString();

		QWidget* parent;

		parent = qobject_cast<QWidget*>(cellWidget(rowIndex, eCurrentColumn));
		if (parent != Q_NULLPTR)
		{
			QCheckBox* checkBox = parent->findChild<QCheckBox*>(kCheckBox);
			if (checkBox != Q_NULLPTR)
			{
				currentState = checkBox->isChecked();
				currentSeriesHash = checkBox->property(kSeriesHash).toUInt();

				result = true;
			}
		}

		parent = qobject_cast<QWidget*>(cellWidget(rowIndex, eVoltageColumn));
		if (parent != Q_NULLPTR)
		{
			QCheckBox* checkBox = parent->findChild<QCheckBox*>(kCheckBox);
			if (checkBox != Q_NULLPTR)
			{
				voltageState = checkBox->isChecked();
				voltageSeriesHash = checkBox->property(kSeriesHash).toUInt();

				result = true;
			}
		}

		parent = qobject_cast<QWidget*>(cellWidget(rowIndex, ePowerColumn));
		if (parent != Q_NULLPTR)
		{
			QCheckBox* checkBox = parent->findChild<QCheckBox*>(kCheckBox);
			if (checkBox != Q_NULLPTR)
			{
				powerState = checkBox->isChecked();
				powerSeriesHash = checkBox->property(kSeriesHash).toUInt();

				result = true;
			}
		}
	}

	return result;
}

void EPMChannelTable::onColorSwatchEditRequested(quint32 row)
{
	HashType channelHash{0};
	if (row < static_cast<quint32>(rowCount()))
	{
		QTableWidgetItem* twi = item(row, eTitleColumn);
		channelHash = twi->data(kItemChannelHash).toULongLong();

		emit colorDoubleClick(channelHash);
	}
}

void EPMChannelTable::onCustomContextMenuRequested(const QPoint& pos)
{
	auto allStateSetter = [&] (QTableWidget* channelTable, bool state, int column)
	{
		for (auto row: range(channelTable->rowCount()))
		{
			QWidget* parent;

			parent = qobject_cast<QWidget*>(cellWidget(row, column));
			if (parent != Q_NULLPTR)
			{
				QCheckBox* checkBox = parent->findChild<QCheckBox*>(kCheckBox);
				if (checkBox != Q_NULLPTR)
				{
					checkBox->setChecked(state);
				}
			}
		}
	};

	auto allCategorySetter = [&] (QTableWidget* channelTable, const QString& category, bool state, int column)
	{
		for (auto row: range(channelTable->rowCount()))
		{
			QTableWidgetItem* twi = channelTable->item(row, eTitleColumn);
			if (twi != Q_NULLPTR)
			{
				QString itemCategory = twi->data(kItemCategory).toString();

				if (itemCategory == category)
				{
					QWidget* parent = qobject_cast<QWidget*>(cellWidget(row, column));
					if (parent != Q_NULLPTR)
					{
						QCheckBox* checkBox = parent->findChild<QCheckBox*>(kCheckBox);
						if (checkBox != Q_NULLPTR)
						{
							checkBox->setChecked(state);
						}
					}
				}
			}
		}
	};

	int row = rowAt(pos.y());
	if (row != -1)
	{
		int itemColumn = columnAt(pos.x());
		QMenu* popup{new QMenu};

		popup->addAction("All On", this, [=, this] { allStateSetter(this, true, itemColumn);});
		popup->addAction("All Off", this, [=, this] { allStateSetter(this, false, itemColumn);});

		QStringList categories = _epmProject->categories();
		if (categories.empty() == false)
		{
			popup->addSeparator();

			for (const auto& category: std::as_const(categories))
			{
				popup->addAction(category + " On", this, [=, this] { allCategorySetter(this, category, true, itemColumn);});
				popup->addAction(category + " Off", this, [=, this] { allCategorySetter(this, category, false, itemColumn);});
			}
		}

		if (popup != Q_NULLPTR)
		{
			popup->exec(mapToGlobal(pos));
			popup->deleteLater();
		}
	}
}

void EPMChannelTable::onCurrentCheckChanged
(
	bool newState
)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
	if (checkBox != Q_NULLPTR)
	{
		HashType channelHash = checkBox->property(kChannelHash).toULongLong();
		HashType seriesHash = checkBox->property(kSeriesHash).toULongLong();

		_epmProject->setCurrentChannelState(channelHash, newState);

		emit currentCheckChanged(newState, channelHash, seriesHash);
	}
}

void EPMChannelTable::onVoltageCheckChanged
(
	bool newState
)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
	if (checkBox != Q_NULLPTR)
	{
		HashType channelHash = checkBox->property(kChannelHash).toULongLong();
		HashType seriesHash = checkBox->property(kSeriesHash).toULongLong();

		_epmProject->setVoltageChannelState(channelHash, newState);

		emit voltageCheckChanged(newState, channelHash, seriesHash);
	}
}

void EPMChannelTable::onPowerCheckChanged
(
	bool newState
)
{
	QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender());
	if (checkBox != Q_NULLPTR)
	{
		HashType channelHash = checkBox->property(kChannelHash).toULongLong();
		HashType seriesHash = checkBox->property(kSeriesHash).toULongLong();

		// todo _epmProject->setPowerChannelState(channelHash, newState);

		emit powerCheckChanged(newState, channelHash, seriesHash);
	}
}

void EPMChannelTable::onCellDoubleClicked(int row,int col)
{
	quint32 channelHash = this->item(row, eTitleColumn)->data(kItemChannelHash).toUInt();

	switch (col)
	{
	case eColorColumn:
		emit colorDoubleClick(channelHash);
		break;

	case eCurrentDataColumn:
		emit currentDataDoubleClick(channelHash);
		break;
	case eVoltageDataColumn:
		emit voltageDataDoubleClick(channelHash);
		break;
	case ePowerDataColumn:
		emit powerDataDoubleClick(channelHash);
		break;
	case eHelpColumn:
		emit helpDoubleClick(channelHash);
		break;

	default:
		break;
	}
}

bool EPMChannelTable::isChecked
(
	int row, int column
)
{
	bool result{false};

	QWidget* parent = qobject_cast<QWidget*>(cellWidget(row, column));
	if (parent != Q_NULLPTR)
	{
		QCheckBox* checkBox = parent->findChild<QCheckBox*>(kCheckBox);
		if (checkBox != Q_NULLPTR)
		{
			result = checkBox->isChecked();
		}
	}

	return result;
}

void EPMChannelTable::updateTableColumns()
{
	setColumnWidth(eColorColumn, 30);

	if (_columns.testFlag(NoColumns))
	{
		hideColumn(eTypeColumn);
		hideColumn(eCurrentColumn);
		hideColumn(eVoltageColumn);
		hideColumn(ePowerColumn);
		hideColumn(eCurrentDataColumn);
		hideColumn(eVoltageDataColumn);
		hideColumn(ePowerDataColumn);
		hideColumn(eHelpColumn);
	}

	if (_columns.testFlag(TypeColumnVisible))
		showColumn(eTypeColumn);

	if (_columns.testFlag(CurrentColumnVisible))
	{
		showColumn(eCurrentColumn);
		setColumnWidth(eCurrentColumn, 30);
	}

	if (_columns.testFlag(VoltageColumnVisible))
	{
		showColumn(eVoltageColumn);
		setColumnWidth(eVoltageColumn, 30);
	}

	if (_columns.testFlag(PowerColumnVisible))
	{
		showColumn(ePowerColumn);
		setColumnWidth(ePowerColumn, 30);
	}

	if (_columns.testFlag(DataColumnVisible))
	{
		showColumn(eCurrentDataColumn);
		showColumn(eVoltageDataColumn);
		showColumn(ePowerDataColumn);

		setColumnWidth(eCurrentDataColumn, 30);
		setColumnWidth(eVoltageDataColumn, 30);
		setColumnWidth(ePowerDataColumn, 30);
	}

	if (_columns.testFlag(HelpColumnVisible))
		showColumn(eHelpColumn);
}

QCheckBox* EPMChannelTable::setupTableCheckbox
(
	int row,
	int col,
	bool state,
	int waveFormType
)
{
	QWidget* widget = new QWidget(this);
	widget->hide();

	QCheckBox* checkBox = new QCheckBox(widget);
	checkBox->setObjectName("checkbox");
	QHBoxLayout* layout = new QHBoxLayout(widget);

	widget->setLayout(layout);
	layout->setAlignment(Qt::AlignCenter);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(checkBox);
	checkBox->setChecked(state);
	setCellWidget(row, col, widget);
	widget->show();

	switch (waveFormType)
	{
	case eWaveFormPower:
		connect(checkBox, &QCheckBox::checkStateChanged, this, &EPMChannelTable::onPowerCheckChanged);
		break;

	case eWaveFormCurrent:
		connect(checkBox, &QCheckBox::checkStateChanged, this, &EPMChannelTable::onCurrentCheckChanged);
		break;

	case eWaveFormVoltage:
	case eWaveFormMarker:
		connect(checkBox, &QCheckBox::checkStateChanged, this, &EPMChannelTable::onVoltageCheckChanged);
	break;

	default:
		break;
	}
	checkBox->setProperty("row", row);
	checkBox->setProperty("col", col);

	return checkBox;
}
