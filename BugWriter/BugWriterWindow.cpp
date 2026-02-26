// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/
#include "BugWriterWindow.h"

// Bugwriter
#include "BugWriterApplication.h"

// QCommon
#include "AboutDialog.h"
#include "Range.h"
#include "SystemInformation.h"
#include "SerialTableModel.h"
#include "version.h"

//Qt
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStorageInfo>

BugWriterWindow::BugWriterWindow
(
	QWidget* parent
) :
	QMainWindow(parent)
{
	setupUi(this);

	QStringList arguments = QCoreApplication::arguments();
	_emailWriter = new EmailWriter;

	for (const auto& arg: arguments)
	{
		QStringList commandSplit = arg.split(QChar(':'), Qt::SkipEmptyParts);
		if (commandSplit.count() >= 2)
		{
			QString value = commandSplit.at(1);
			value.replace("_", " ");

			QString label = commandSplit.at(0);
			if (label == "prodversion")
			{
				_productVersion->setText(value);
				_productVersion->setReadOnly(true);
			}
			else if (label == "product")
			{
				_productCombo->setCurrentText(value);
				_productCombo->setEnabled(false);
			}
			else if (label == "application")
			{
				_application->setText(value);
				_application->setReadOnly(true);
			}
			else if (label == "appversion")
			{
				_applicationVersion->setText(value);
				_applicationVersion->setReadOnly(true);
			}
			else if (label == "listports")
			{
				_listPorts = true;
			}
		}
	}

	QString author = _emailWriter->getAuthorName();

	// If the author name is invalid, try to look up in the preferences
	if (_emailWriter->isAuthorValid() == false)
	{
		author = BugWriterApplication::getPreferences()->lastAuthor();
	}

	_authorName->setText(author);

	_timer.setInterval(1000);
	_timer.setSingleShot(true);

	_timer.start();

	connect(&_timer, &QTimer::timeout, this, &BugWriterWindow::onTimerTimeout);
}

BugWriterWindow::~BugWriterWindow()
{
}

void BugWriterWindow::on__quitButton_clicked()
{
	BugWriterApplication::appInstance()->quit();
}

void BugWriterWindow::on__saveAsTextFileButton_clicked()
{
	if (checkFields())
	{
		QString body = buildBody();

		QString saveDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

		QString application = _application->text();
		application = application.replace(" ", "_");
		application = "BW_" + application;

		saveDir += QDir::separator() + application;
		saveDir += QDateTime::currentDateTime().toString("_yyyy_dd_MM_HH_mm_ss") + ".txt";
		saveDir = QDir::cleanPath(saveDir);

		QString writeFile = QFileDialog::getSaveFileName(this, "Save Bug Report File", saveDir, "Text File (*.txt *.)");
		if (writeFile.isEmpty() == false)
		{
			QFile bugReportFile(writeFile);

			bugReportFile.open(QIODevice::WriteOnly);
			bugReportFile.write(body.toLatin1());
			bugReportFile.close();
		}
	}
}

void BugWriterWindow::on__submitButton_clicked()
{
	if (checkFields())
	{
		if (_emailWriter->send(_title->text().toLatin1(), buildBody().toLatin1(), _attachments))
		{
			_attachments.clear();
			QMessageBox::information(this, "Ticket Submitted", "Your issue is now shared with the Alpaca support. You will soon receive a Jira ticket on the email id corresponding to your username");
		}
		else
		{
			QMessageBox::critical(this, "Submit Failed.", "Save your report to a text file and send that text file to alpaca.hostdev.jira.");
		}
	}
}

bool BugWriterWindow::checkFields()
{
	bool result(true);

	if (_productVersion->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Product Version is Required.  If the Bug Report is initiated from the Alpaca application, this field will automatically be filled in.");
		result = false;
	}
	else if (_productCombo->currentText() == "Not Set")
	{
		QMessageBox::critical(this, "Data Validation", "Product is Required.  If the Bug Report is initiated from the Alpaca application, this field will automatically be filled in.");
		result = false;
	}
	else if (_application->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Application is Required.  If the Bug Report is initiated from the Alpaca application, this field will automatically be filled in.");
		result = false;
	}
	else if (_applicationVersion->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Application Version is Required.  If the Bug Report is initiated from the Alpaca application, this field will automatically be filled in.");
		result = false;
	}
	else if (_title->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "A Title is Required.  This title is used as the Jira ticket title when submitted through email.");
		result = false;
	}
	else if (_observedBehavior->toPlainText().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Enter some information about the issue.  Some engineers will say, \"It doesn't work.\""
			  "That text isn't useful and will result in your bug going to the bottom of the queue as it requires extra investigation.");
		result = false;
	}
	else if (_desiredBehavior->toPlainText().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Enter some information about what you want it to behave.  Some engineers assume it's obvious what it should do."
			"But the hardware and test engineers are the subject matter experts.");
		result = false;
	}
	else if (_stepsToReproduce->toPlainText().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Enter some information about how to reproduce the issue.  Everyone has a different work flow."
			"We do extensive testing with our workflow.  It may work for us and not for you and the only way to figure this out is to reproduce your workflow.");
		result = false;
	}
	else if (_emailWriter->isAuthorValid() == false)
	{
		QMessageBox::critical(this, "Data Validation", "Check if the author name is correct.");
		result = false;
	}

	return result;
}

const int kPortName(0);
const int kDescription(1);
const int kSerialNumber(2);
const int kVendor(3);
const int kProduct(4);

QString BugWriterWindow::buildBody()
{
	SystemInformation systemInformation;

	QString body;

	body = _title->text() + "\n\n";

	body += "Product: " + _productCombo->currentText() + "\n";
	body += "Product Version: " + _productVersion->text() + "\n\n";
	body += "Application: " + _application->text() + "\n";
	body += "Application Version: " + _applicationVersion->text() + "\n\n";
	body += "User Name: " + _authorName->text() + "\n";
	body += "Phone number: " + _authorPhone->text() + "\n";
	body += "Computer Name: " + systemInformation.computerName() + "\n";
	body += "OS : " + systemInformation.osName() + "\n";
	body += "OS Version: " + systemInformation.osVersion() + "\n";
	body += "Driver Version: " + _driverVersion + "\n";
	body += "Physical Memory: " + systemInformation.totalPhysicalMemory() + "GB \n";
	body += "Virtual Memory: " + systemInformation.totalVirtualMemory() + "GB \n\n";

#ifdef Q_OS_WINDOWS
	body += "Physical Drives:\n";

	for (auto& storage: QStorageInfo::mountedVolumes())
	{
		if (storage.isValid())
		{
			body += "    " +  storage.rootPath();

			body += "         isReady:" + (storage.isReady() ? QString("true") : QString("false"));
			body += "         isReadOnly:" + (storage.isReadOnly() ? QString("true") : QString("false"));
			body += "         name:" + storage.name();
			body += "         fileSystemType:" + storage.fileSystemType();
			body += "         size:" + QString::number(storage.bytesTotal()/1000/1000) + " MB";
			body += "         availableSize:" + QString::number(storage.bytesAvailable()/1000/1000) + " MB";
		}
	}
#endif

	body += _portData;

	body += "\n\nObserved Behavior \n";
	body += _observedBehavior->toPlainText() + "\n";

	body += "\nDesired Behavior \n";
	body += _desiredBehavior->toPlainText() + "\n";

	body += "\nSteps to Reproduce \n";
	body += _stepsToReproduce->toPlainText() + "\n";

	AppCore::writeToApplicationLogLine("*****************");
	AppCore::writeToApplicationLogLine("Last ticket body:");
	AppCore::writeToApplicationLogLine("*****************");
	AppCore::writeToApplicationLog(body);

	return body;
 }

void BugWriterWindow::on__authorName_textChanged(const QString &newAuthor)
{
	_emailWriter->setAuthorName(newAuthor);
	if(_emailWriter->isAuthorValid())
		_authorName->setStyleSheet("border: 1px solid green");
	else
		_authorName->setStyleSheet("border: 1px solid red");

	QString phoneNumber = BugWriterApplication::getPreferences()->getUserNamePhone(newAuthor);
	if (!phoneNumber.isEmpty())
	{
		_authorPhone->setText(phoneNumber);
	}
}

void BugWriterWindow::on__authorPhone_editingFinished()
{
	QString authorName = _authorName->text();
	QString authorPhone = _authorPhone->text();
	BugWriterApplication::getPreferences()->setUserNamePhone(authorName, authorPhone);
}


void BugWriterWindow::on_actionAbout_triggered()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.setTitle("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">Bug Writer</span></p></body></html>");

	QFile file(":/About.txt");
	if (file.open(QIODevice::ReadOnly) == true)
	{
		QByteArray html = file.readAll();
		aboutDialog.setAboutText(html);
		file.close();
	}

	QString kAppName("Bug Writer");
	const QString kAppVersion(BUG_WRITER_VERSION);

	aboutDialog.setAppName(kAppName);
	aboutDialog.setAppVersion(kAppVersion.toLatin1());
	QPixmap pixMap = QPixmap(QString::fromUtf8(":/BugWriter.png"));
	aboutDialog.setBackSplash(pixMap);

	aboutDialog.exec();
}

void BugWriterWindow::on__attachmentBtn_clicked()
{
	QStringList filePaths;

	QString defaultPath = BugWriterApplication::appCore()->loggingPath();

	filePaths = QFileDialog::getOpenFileNames(this, "Select one or more files to attach", defaultPath, "Text files (*.txt);;Log files (*.log);;Images (*.png *.xpm *.jpg)");
	if (filePaths.isEmpty() == false)
	{
		for (const auto &filePath: filePaths)
		{
			_attachments.append(filePath.toLatin1());
			BugWriterApplication::appCore()->writeToApplicationLogLine("Attached file: " + filePath.toLatin1());
		}
	}
	else
		_statusbar->showMessage("No file(s) attached!", 5000);
}

void BugWriterWindow::on_actionQuit_triggered()
{
	BugWriterApplication::appInstance()->quit();
}


void BugWriterWindow::onTimerTimeout()
{
	_statusbar->showMessage("Acquiring installed driver version...", 1000);

	SystemInformation systemInformation;
	_driverVersion = systemInformation.driverVersion();

	if (_driverVersion.isEmpty() == false)
		_statusbar->showMessage("Driver version acquired.", 1000);
	else
		_statusbar->showMessage("Failed to acquire driver version.", 1000);

	if (_listPorts == true)
	{
		_statusbar->showMessage("Acquiring device port information...", 1000);

		_portData += "\nSerial Ports:\n";

		SerialTableModel stm;

		stm.refresh();

		for (auto deviceIndex: range(stm.rowCount()))
		{
			_portData += "         Port:";
			_portData += stm.portData(deviceIndex, kPortName).toString();

			_portData += " Description:";
			_portData += stm.portData(deviceIndex, kDescription).toString();

			_portData += " Serial Number:";
			_portData += stm.portData(deviceIndex, kSerialNumber).toString();

			_portData += " Vendor:";
			_portData += stm.portData(deviceIndex, kVendor).toString();

			_portData += " Product:";
			_portData += stm.portData(deviceIndex, kProduct).toString();

			_portData += "\n";
		}

		_statusbar->showMessage("Device port information acquired.", 1000);
	}

}
