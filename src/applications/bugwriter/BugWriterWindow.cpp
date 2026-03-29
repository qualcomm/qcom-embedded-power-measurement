// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QUrl>

BugWriterWindow::BugWriterWindow
(
	QWidget* parent
) :
	QMainWindow(parent)
{
	setupUi(this);

	_issueWriter = new IssueWriter(this);

	connect(_issueWriter, &IssueWriter::deviceFlowStarted, this, &BugWriterWindow::onDeviceFlowStarted);
	connect(_issueWriter, &IssueWriter::authenticated,     this, &BugWriterWindow::onAuthenticated);
	connect(_issueWriter, &IssueWriter::issueSubmitted,    this, &BugWriterWindow::onIssueSubmitted);
	connect(_issueWriter, &IssueWriter::errorOccurred,     this, &BugWriterWindow::onIssueError);

	QStringList arguments = QCoreApplication::arguments();

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

	// Pre-fill author from saved preferences; onAuthenticated() will override with
	// the GitHub username once the stored token is validated on startup.
	QString author = BugWriterApplication::getPreferences()->lastAuthor();
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
		// Plain-text body is still useful for the saved file
		SystemInformation sysInfo;
		QString body;
		body += "Product: "             + _productCombo->currentText()   + "\n";
		body += "Product Version: "     + _productVersion->text()        + "\n\n";
		body += "Application: "         + _application->text()           + "\n";
		body += "Application Version: " + _applicationVersion->text()    + "\n\n";
		body += "User Name: "           + _authorName->text()            + "\n";
		body += "Phone number: "        + _authorPhone->text()           + "\n";
		body += "Computer Name: "       + sysInfo.computerName()         + "\n";
		body += "OS: "                  + sysInfo.osName()               + "\n";
		body += "OS Version: "          + sysInfo.osVersion()            + "\n";
		body += "Driver Version: "      + _driverVersion                 + "\n";
		body += "Physical Memory: "     + sysInfo.totalPhysicalMemory()  + " GB\n";
		body += "Virtual Memory: "      + sysInfo.totalVirtualMemory()   + " GB\n\n";

		body += "\nObserved Behavior\n" + _observedBehavior->toPlainText()  + "\n";
		body += "\nDesired Behavior\n"  + _desiredBehavior->toPlainText()   + "\n";
		body += "\nSteps to Reproduce\n"+ _stepsToReproduce->toPlainText()  + "\n";

		QString saveDir     = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		QString application = _application->text().replace(" ", "_");
		saveDir += QDir::separator() + QStringLiteral("BW_") + application
		        +  QDateTime::currentDateTime().toString("_yyyy_dd_MM_HH_mm_ss") + ".txt";
		saveDir = QDir::cleanPath(saveDir);

		QString writeFile = QFileDialog::getSaveFileName(this, "Save Bug Report File", saveDir, "Text File (*.txt *.)");
		if (!writeFile.isEmpty())
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
		_issueWriter->submitIssue(_title->text(), buildMarkdownBody());
}

bool BugWriterWindow::checkFields()
{
	bool result(true);

	if (_productVersion->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Product Version is Required.  If the Bug Report is initiated from the QEPM application, this field will automatically be filled in.");
		result = false;
	}
	else if (_productCombo->currentText() == "Not Set")
	{
		QMessageBox::critical(this, "Data Validation", "Product is Required.  If the Bug Report is initiated from the QEPM application, this field will automatically be filled in.");
		result = false;
	}
	else if (_application->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Application is Required.  If the Bug Report is initiated from the QEPM application, this field will automatically be filled in.");
		result = false;
	}
	else if (_applicationVersion->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "Application Version is Required.  If the Bug Report is initiated from the QEPM application, this field will automatically be filled in.");
		result = false;
	}
	else if (_title->text().isEmpty())
	{
		QMessageBox::critical(this, "Data Validation", "A Title is Required.");
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
	else if (IssueWriter::isAuthorValid(_authorName->text()) == false)
	{
		QMessageBox::critical(this, "Data Validation", "Check if the author name is correct.");
		result = false;
	}

	return result;
}

QString BugWriterWindow::buildMarkdownBody()
{
	SystemInformation sysInfo;
	QString body;

	body += "## Environment\n\n";
	body += "| Field | Value |\n|---|---|\n";
	body += "| Product | "             + _productCombo->currentText()   + " |\n";
	body += "| Product Version | "     + _productVersion->text()        + " |\n";
	body += "| Application | "         + _application->text()           + " |\n";
	body += "| Application Version | " + _applicationVersion->text()    + " |\n";
	body += "| Reported by | "         + _authorName->text()            + " |\n";
	body += "| Phone | "               + _authorPhone->text()           + " |\n";
	body += "| Computer | "            + sysInfo.computerName()         + " |\n";
	body += "| OS | "                  + sysInfo.osName()               + " |\n";
	body += "| OS Version | "          + sysInfo.osVersion()            + " |\n";
	body += "| Driver Version | "      + _driverVersion                 + " |\n";
	body += "| Physical Memory | "     + sysInfo.totalPhysicalMemory()  + " GB |\n";
	body += "| Virtual Memory | "      + sysInfo.totalVirtualMemory()   + " GB |\n";

#ifdef Q_OS_WINDOWS
	body += "\n**Physical Drives**\n\n";
	for (const auto& storage : QStorageInfo::mountedVolumes())
	{
		if (storage.isValid())
		{
			body += "- `" + storage.rootPath() + "`";
			body += "  ready:" + (storage.isReady() ? QString("yes") : QString("no"));
			body += "  name:" + storage.name();
			body += "  fs:" + storage.fileSystemType();
			body += "  size:" + QString::number(storage.bytesTotal() / 1000 / 1000) + " MB";
			body += "  free:" + QString::number(storage.bytesAvailable() / 1000 / 1000) + " MB\n";
		}
	}
#endif

	body += _portData;

	body += "\n## Observed Behavior\n\n";
	body += _observedBehavior->toPlainText() + "\n";

	body += "\n## Desired Behavior\n\n";
	body += _desiredBehavior->toPlainText() + "\n";

	body += "\n## Steps to Reproduce\n\n";
	body += _stepsToReproduce->toPlainText() + "\n";

	if (!_attachments.isEmpty())
	{
		static constexpr int kMaxFileSizeBytes = 10 * 1024;

		body += "\n## Attached Files\n\n";
		for (const QByteArray& pathBytes : _attachments)
		{
			const QString filePath = QString::fromLatin1(pathBytes);
			const QString fileName = QFileInfo(filePath).fileName();
			const QString ext      = QFileInfo(filePath).suffix().toLower();

			if (ext == "png" || ext == "jpg" || ext == "xpm" || ext == "bmp")
			{
				body += "_Image attached (upload manually): " + fileName + "_\n\n";
				continue;
			}

			QFile f(filePath);
			if (f.open(QIODevice::ReadOnly))
			{
				const QByteArray content = f.read(kMaxFileSizeBytes);
				const bool truncated     = (f.size() > kMaxFileSizeBytes);
				f.close();

				body += "<details><summary>" + fileName + "</summary>\n\n";
				body += "```\n";
				body += QString::fromLatin1(content);
				if (truncated)
					body += "\n... (truncated at 10 KB)";
				body += "\n```\n\n</details>\n\n";
			}
		}
	}

	return body;
}

void BugWriterWindow::on__authorName_textChanged(const QString& newAuthor)
{
	if (IssueWriter::isAuthorValid(newAuthor))
		_authorName->setStyleSheet("border: 1px solid green");
	else
		_authorName->setStyleSheet("border: 1px solid red");

	const QString phoneNumber = BugWriterApplication::getPreferences()->getUserNamePhone(newAuthor);
	if (!phoneNumber.isEmpty())
		_authorPhone->setText(phoneNumber);
}

void BugWriterWindow::on__authorPhone_editingFinished()
{
	BugWriterApplication::getPreferences()->setUserNamePhone(_authorName->text(), _authorPhone->text());
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
	const QString defaultPath = BugWriterApplication::appCore()->loggingPath();

	const QStringList filePaths = QFileDialog::getOpenFileNames(this, "Select one or more files to attach", defaultPath, "Text files (*.txt);;Log files (*.log);;Images (*.png *.xpm *.jpg)");
	if (!filePaths.isEmpty())
	{
		for (const auto& filePath : filePaths)
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

	if (!_driverVersion.isEmpty())
		_statusbar->showMessage("Driver version acquired.", 1000);
	else
		_statusbar->showMessage("Failed to acquire driver version.", 1000);

	if (_listPorts == true)
	{
		_statusbar->showMessage("Acquiring device port information...", 1000);

		_portData += "\nSerial Ports:\n";

		SerialTableModel stm;
		stm.refresh();

		for (auto deviceIndex : range(stm.rowCount()))
		{
			_portData += "         Port:"          + stm.portData(deviceIndex, 0).toString();
			_portData += " Description:"           + stm.portData(deviceIndex, 1).toString();
			_portData += " Serial Number:"         + stm.portData(deviceIndex, 2).toString();
			_portData += " Vendor:"                + stm.portData(deviceIndex, 3).toString();
			_portData += " Product:"               + stm.portData(deviceIndex, 4).toString();
			_portData += "\n";
		}

		_statusbar->showMessage("Device port information acquired.", 1000);
	}
}

void BugWriterWindow::onDeviceFlowStarted(const QString& userCode, const QString& verificationUri)
{
	QDesktopServices::openUrl(QUrl(verificationUri));

	QMessageBox::information(this, tr("GitHub Authorization"),
		tr("A browser window has been opened. Enter the following code to authorize:\n\n"
		   "     %1\n\n"
		   "Once you approve the request, issue submission will continue automatically.")
		.arg(userCode));
}

void BugWriterWindow::onAuthenticated(const QString& username)
{
	_authorName->setText(username);
	_statusbar->showMessage(tr("Signed in to GitHub as %1").arg(username), 3000);
}

void BugWriterWindow::onIssueSubmitted(int issueNumber, const QString& issueUrl)
{
	_attachments.clear();
	QMessageBox::information(this, tr("Issue Submitted"),
		tr("Issue #%1 has been filed successfully.\n\n%2").arg(issueNumber).arg(issueUrl));
}

void BugWriterWindow::onIssueError(const QString& message)
{
	QMessageBox::critical(this, tr("Submission Failed"), message);
}
