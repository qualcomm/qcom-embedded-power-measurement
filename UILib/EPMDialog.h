// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef EPMDIALOG_H
#define EPMDIALOG_H

#include <QDialog>

namespace Ui {
class EPMDialog;
}

class EPMDialog : public QDialog
{
	Q_OBJECT

public:
	explicit EPMDialog(QWidget *parent = nullptr);
	~EPMDialog();

private:
	Ui::EPMDialog *ui;
};

#endif // EPMDIALOG_H
