#ifndef SERIESDATAWINDOW_H
#define SERIESDATAWINDOW_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMGlobalLib.h"
#include "UDASBlockModel.h"

// QT
#include <QTableView>

namespace Ui {
class SeriesDataWindow;
}

class EPMLIB_EXPORT SeriesDataWindow :
	public QTableView
{
	Q_OBJECT

public:
	SeriesDataWindow(QWidget* parent = Q_NULLPTR);
	~SeriesDataWindow();

	void setData(UDASBlock udasBlock);

protected:
	virtual void closeEvent(QCloseEvent* event);

private:
	Ui::SeriesDataWindow*		_ui{Q_NULLPTR};
	UDASBlockModel*				_udasBlockModel{Q_NULLPTR};
};

#endif // SERIESDATAWINDOW_H
