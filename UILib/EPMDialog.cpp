#include "EPMDialog.h"
#include "ui_EPMDialog.h"

EPMDialog::EPMDialog(
	QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::EPMDialog)
{
	ui->setupUi(this);
}

EPMDialog::~EPMDialog()
{
	delete ui;
}
