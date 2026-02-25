// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "SeriesDataWindow.h"
#include "ui_SeriesDataWindow.h"

// Qt
#include <QCloseEvent>

SeriesDataWindow::SeriesDataWindow(QWidget* parent) :
	QTableView(parent),
	_ui(new Ui::SeriesDataWindow)
{
	_ui->setupUi(this);

}

SeriesDataWindow::~SeriesDataWindow()
{
	delete _ui;
}

void SeriesDataWindow::setData
(
	UDASBlock udasBlock
)
{
	_udasBlockModel = new UDASBlockModel(this);

	_udasBlockModel->setModelData(udasBlock);
	setModel(_udasBlockModel);
}

void SeriesDataWindow::closeEvent
(
	QCloseEvent* event
)
{
	setVisible(false);
	event->ignore();
}
