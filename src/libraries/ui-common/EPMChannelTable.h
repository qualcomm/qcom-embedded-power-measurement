#ifndef EPMCHANNELTABLE_H
#define EPMCHANNELTABLE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPMLib
#include "EPMGlobalLib.h"
#include "PRNFile.h"
#include "UDASFile.h"

#include "EPMProject.h"

// Qt
class QCheckBox;
#include <QIcon>
#include <QTableWidget>

//typedef QList<HashType> SeriesList;

namespace Ui {
	class EPMChannelTableClass;
}

class EPMLIB_EXPORT EPMChannelTable :
	public QTableWidget
{
	Q_OBJECT

public:
	enum ColumnOption
	{
		NoColumns = 0x0,
		TypeColumnVisible = 0x01,
		CurrentColumnVisible = 0x02,
		VoltageColumnVisible = 0x04,
		PowerColumnVisible = 0x08,
		DataColumnVisible = 0x10,
		HelpColumnVisible = 0x20
	};

	enum ColumnsIndex
	{
		eColorColumn = 0,
		eTitleColumn,
		eTypeColumn,
		eCurrentColumn,
		eVoltageColumn,
		ePowerColumn,
		eCurrentDataColumn,
		eVoltageDataColumn,
		ePowerDataColumn,
		eHelpColumn
	};

	Q_DECLARE_FLAGS(ColumnOptions, ColumnOption)

	EPMChannelTable(QWidget* parent = Q_NULLPTR);
	~EPMChannelTable();

	void setEPMProject(EPMProject epmProject);
	void setChannelColor(HashType channelHash, QColor newColor);

	void setColumns(ColumnOptions columnOptions);

	void populateChannels();
	void populateChannels(UDASFile& udasFile, PRNFile& prnFile);
	bool checked(int column);
    bool isValid();
	void clear();

	QString lastError()
	{
		return _lastError;
	}

	bool getChannelEntry(int rowIndex, QString& category, HashType& channelHash, bool& currentState,
		HashType& currentSeriesHash, bool& voltageState, HashType& voltageSeriesHash, bool& powerState,
		HashType& powerSeriesHash);

signals:
	void colorDoubleClick(HashType channelHash);
	void currentDataDoubleClick(HashType channelHash);
	void voltageDataDoubleClick(HashType channelHash);
	void powerDataDoubleClick(HashType channelHash);
	void helpDoubleClick(HashType channelHash);

	void currentCheckChanged(bool newState, HashType channelHash, HashType seriesHash);
	void voltageCheckChanged(bool newState, HashType channelHash, HashType seriesHas);
	void powerCheckChanged(bool newState, HashType channelHash, HashType seriesHas);

	void errorOccurred(const QString& errorMessage);

private slots:
	void onColorSwatchEditRequested(quint32 row);
	void onCustomContextMenuRequested(const QPoint& pos);

	void onCurrentCheckChanged(bool newState);
	void onVoltageCheckChanged(bool newState);
	void onPowerCheckChanged(bool newState);
	void onCellDoubleClicked(int row, int col);

private:
	bool isChecked(int row, int column);
	void updateTableColumns();

	QCheckBox* setupTableCheckbox(int row, int col, bool state, int waveFormType);

	Ui::EPMChannelTableClass*	_ui{Q_NULLPTR};
	EPMProject					_epmProject;
    ColumnOptions				_columns{NoColumns};
	QString						_lastError;
    bool                        _valid{true};

	QIcon						_doubleClickIcon;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EPMChannelTable::ColumnOptions)

#endif // EPMCHANNELTABLE_H
