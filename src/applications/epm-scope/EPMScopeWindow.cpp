// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMScopeWindow.h"

//  EPMScope
#include "AboutDialog.h"
#include "EPMScopeApplication.h"
#include "EPMScopeDefines.h"
#include "PreferencesDialog.h"

// QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ApplicationEnhancements.h"
#include "Range.h"

// Qt
#include <QCloseEvent>
#include <QDateTime>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>

const QByteArray kDefaultDeviceItem("<scan devices to update list>");

EPMScopeWindow:: EPMScopeWindow(EPMScopePreferences* preferences, QWidget* parent) :
	QMainWindow(parent),
	_preferences(preferences),
	_plotUpdateTimer(new QTimer)
{
	setupUi(this);

	setWindowTitle("EPM Scope");

	_currentChart->setYTitle("Current (mA)");
	_currentChart->setYRange(-5, 5);

	_voltageChart->setYTitle("Voltage (mV)");
	_voltageChart->setYRange(0, 4);

	_splitter->setStretchFactor(1, 2);

	on__addDeviceBtn_clicked();

	QLabel* logicalCoordinateLabel = new QLabel(_statusbar);
	logicalCoordinateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	_statusbar->setSizeGripEnabled(true);
	_statusbar->addWidget(logicalCoordinateLabel, 1);

	auto showMessageWithTimeout = [logicalCoordinateLabel](const QString& text) {
		logicalCoordinateLabel->setText(text);
	};

	connect(_currentChart, &CurrentChartLive::hoveredCoordChange, showMessageWithTimeout);
	connect(_voltageChart, &VoltageChartLive::hoveredCoordChange, showMessageWithTimeout);
	connect(_plotUpdateTimer, &QTimer::timeout, this, & EPMScopeWindow::on_plotTimerTimeout);
	connect(&_model, &EPMDeviceModel::scanDevices, this, &EPMScopeWindow::on__scanDevices_clicked);
	connect(&_model, &EPMDeviceModel::recordState, this, &EPMScopeWindow::setupRecordState);

	_recordIcon.addFile(QString::fromUtf8(":/Record.png"), QSize(), QIcon::Normal, QIcon::Off);
	_stopIcon.addFile(QString::fromUtf8(":/Stop.png"), QSize(), QIcon::Normal, QIcon::Off);
	_recordBtn->setIconSize(QSize(24, 24));

	setupRecordState(false);

	AlpacaSettings settings(kAppName);
	settings.beginGroup(kAppName);

	QPoint pos = settings.value("pos", QPoint(40, 40)).toPoint();
	move(pos);

	QSize size = settings.value("size", QSize(1200, 800)).toSize();
	resize(size);

	settings.endGroup();

	_model.setCurrentChart(_currentChart);
	_model.setVoltageChart(_voltageChart);
}

EPMScopeWindow::~ EPMScopeWindow()
{
	delete _plotUpdateTimer;
}

void  EPMScopeWindow::shutDown()
{

	if (_recording)
		on__recordBtn_clicked();

	AlpacaSettings settings(kAppName);
	settings.beginGroup(kAppName);
	settings.setValue("pos", pos());
	settings.setValue("size", size());
	settings.endGroup();

	EPMScopeApplication::removeScopeWindow(this);
}

void  EPMScopeWindow::startRecording()
{

}

void  EPMScopeWindow::stopRecording()
{

}

void  EPMScopeWindow::recordData(MicroEpmChannelData* channelData, quint32 sampleCount)
{
	_model.record(channelData, sampleCount);
}

void  EPMScopeWindow::closeEvent(QCloseEvent* event)
{
	shutDown();

	event->accept();

	QWidget::closeEvent(event);
}

void  EPMScopeWindow::changeEvent
(
	QEvent* e
)
{
	QWidget::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}

void  EPMScopeWindow::setupRecordState(bool status)
{
	_recording = status;

	if (status)
	{
		_recordBtn->setIcon(_stopIcon);
		_recordBtn->setText("Stop");
		_settingsBtn->setEnabled(false);

		_epmChannelTabWidget->setEnabled(false);

		setupActiveCharts();
	}
	else
	{
		_recordBtn->setIcon(_recordIcon);
		_recordBtn->setText("Record");
		_settingsBtn->setEnabled(true);

		_epmChannelTabWidget->setEnabled(true);
	}
}

void  EPMScopeWindow::on__recordBtn_clicked()
{
	if (_recording == false)
	{
		for (auto& device : _model.openedDevices())
			device->setLiveRecorder(this);

		_plotUpdateTimer->start(1000);
		_elapsed.start();

		_model.startRecord();
	}
	else
	{
		_plotUpdateTimer->stop();

		for (auto& device : _model.openedDevices())
			device->removeLiveRecorder();

		_model.stopRecord();

		AppCore::writeToApplicationLog(QString("           Elapsed Time: " + QString("%1 (milliseconds)\n\n").arg(_elapsed.elapsed())));
	}

}

void  EPMScopeWindow::on_plotTimerTimeout()
{
	_currentChart->replot();
	_voltageChart->replot();
}

void  EPMScopeWindow::setupActiveCharts()
{
	_currentChart->clearLineSeries();
	_voltageChart->clearLineSeries();

	_currentChart->setDuration(_preferences->windowDuration());
	_voltageChart->setDuration(_preferences->windowDuration());

	_currentChart->replot();
	_voltageChart->replot();
}

void EPMScopeWindow::on__actionAbout_triggered()
{
	AboutDialog aboutDialog;

	aboutDialog.setTitle("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">" + windowTitle().toLatin1() + "</span></p></body></html>");

	QFile file(":/About.txt");
	if (file.open(QIODevice::ReadOnly) == true)
	{
		QByteArray html = file.readAll();
		aboutDialog.setAboutText(html);
		file.close();
	}

	aboutDialog.setAppName(kAppName);
	aboutDialog.setAppVersion(kAppVersion.toLatin1());

	QPixmap pixMap = QPixmap(QString::fromUtf8(":/EPMScope_BS.png"));
	aboutDialog.setBackSplash(pixMap);

	aboutDialog.exec();
}

void EPMScopeWindow::on__actionContents_triggered()
{
	startLocalBrowser(docsRoot() + "/getting-started/02-EPM-Scope.html");
}

void EPMScopeWindow::on__actionQuit_triggered()
{
	QCoreApplication::instance()->exit();
}

void EPMScopeWindow::on__actionRateMe_triggered()
{
	EPMScopeApplication::appInstance()->showRateDialog();
}

void EPMScopeWindow::on__actionSubmitBugReport_triggered()
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/QEPM/bin/BugWriter"; // Linux Sucks
#else
	QString program = "BugWriter";
#endif

	QStringList arguments;
	arguments << "product:QEPM";
	arguments << "prodversion:" + kProductVersion;
	arguments << "application:" + windowTitle();
	arguments << "appversion:" + kAppVersion;

	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	bool started = process->startDetached();

	if (started == false)
		AppCore::writeToApplicationLog("Bug Writer failed to start.");
}

void EPMScopeWindow::on__scanDevices_clicked()
{
	while (_model.count() > 1)
		on__removeDeviceBtn_clicked();

	_model.updateDeviceModel();

	if (_model.deviceCount() > 0)
		_recordBtn->setEnabled(true);

	if (_model.deviceCount() > 1)
		_addDeviceBtn->setEnabled(true);

	if (_model.count() > 1)
		_removeDeviceBtn->setEnabled(true);
	else
		_removeDeviceBtn->setEnabled(false);
}

void EPMScopeWindow::on__addDeviceBtn_clicked()
{
	QVBoxLayout* vbl = qobject_cast<QVBoxLayout*>(_deviceGroupBox->layout());

	QHBoxLayout* hbl = _model.layout(_deviceGroupBox);
	vbl->addLayout(hbl);

	QWidget* w = _model.tableWidget(_epmChannelTabWidget);

	if (w != Q_NULLPTR)
		_epmChannelTabWidget->addTab(w, "EPM Device");

	if (_model.count() == _model.deviceCount())
	{
		_addDeviceBtn->setEnabled(false);
		_removeDeviceBtn->setEnabled(true);
	}
}

void EPMScopeWindow::on__removeDeviceBtn_clicked()
{
	QVBoxLayout* vbl = qobject_cast<QVBoxLayout*>(_deviceGroupBox->layout());

	QList<QHBoxLayout*> layoutList = vbl->findChildren<QHBoxLayout*>(Qt::FindChildrenRecursively);

	if (layoutList.count() > 1)
	{
		QHBoxLayout* hbl = layoutList.last();
		for (int idx : range(hbl->count()))
		{
			QLayoutItem* li = hbl->itemAt(idx);
			if (li != Q_NULLPTR)
			{
				QWidget* w = li->widget();
				if (w != Q_NULLPTR)
				{
					w->hide();
					w->deleteLater();
				}
			}
		}

		_model.removeLayout(hbl);
		hbl->deleteLater();
	}

	int tabCount = _epmChannelTabWidget->count();
	if (tabCount > 1)
	{
		QWidget* w = _epmChannelTabWidget->widget(tabCount-1);
		if (w != Q_NULLPTR)
		{
			_epmChannelTabWidget->removeTab(tabCount-1);
			_model.removeTableWidget(w);
		}
	}

	if (_model.deviceCount() > _model.count())
		_addDeviceBtn->setEnabled(true);

	if (_model.count() == 1)
		_removeDeviceBtn->setEnabled(false);
}

void EPMScopeWindow::on__settingsBtn_clicked()
{
	PreferencesDialog preferences(_preferences, this);

	if (preferences.exec() == QDialog::Accepted)
	{
		_currentChart->setXRange(0, _preferences->windowDuration());
		_currentChart->replot();
		_voltageChart->setXRange(0, _preferences->windowDuration());
		_voltageChart->replot();
	}
}

void EPMScopeWindow::on__actionPreferences_triggered()
{
	on__settingsBtn_clicked();
}
