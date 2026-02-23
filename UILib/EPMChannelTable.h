#ifndef EPMCHANNELTABLE_H
#define EPMCHANNELTABLE_H
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
// Copyright 2020-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)	
*/

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
