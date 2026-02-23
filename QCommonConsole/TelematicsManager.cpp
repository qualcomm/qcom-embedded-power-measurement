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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TelematicsManager.h"

// QCommon
#include "AlpacaDefines.h"
#include "AppCore.h"

//Qt
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QSettings>

TelematicsManager* TelematicsManager::_instance = Q_NULLPTR;

TelematicsManager::TelematicsManager
(
	AppCore& appCore
) :
	_appCore(appCore)
{
	QString fileName;

	QCoreApplication* app = QCoreApplication::instance();
	if (app)
	{
		fileName = QDir::cleanPath(app->applicationDirPath() + QDir::separator() + "TelematicsClient");
	}
	else
	{
#ifdef Q_OS_WIN
		fileName = QDir::cleanPath("C:/Program Files (x86)/Qualcomm/Alpaca/TelematicsClient");
        _telematicsLibrary.setFileName(fileName);

        _initializeProc = (Telematics_Initialize) _telematicsLibrary.resolve("Telematics_Initialize");
#endif

#ifdef Q_OS_LINUX
        // do nothing at this point
#endif
	}

}

TelematicsManager::~TelematicsManager()
{
	_telematicsLibrary.unload();
}

bool TelematicsManager::initialize
(
	const QByteArray& productID
)
{
	if (_initializeProc != Q_NULLPTR && _valid == false)
	{
		QByteArray meta = _appCore.appName();

		_lastReturnCode = _initializeProc(productID.data(), meta.data());
		if (_lastReturnCode == TELEMATICS_CLIENT_SUCCESS)
		{
			_valid = true;

			_getVersion = (Telematics_GetVersion) _telematicsLibrary.resolve("Telematics_GetVersion");
			if (_getVersion != Q_NULLPTR)
			{
				_appCore.writeToAppLog("_getVersion valid\n");
			}

			_trackEvent = (Telematics_TrackEvent) _telematicsLibrary.resolve("Telematics_TrackEvent");
			if (_trackEvent != Q_NULLPTR)
			{
				QByteArray appVersion = _appCore.appVersion();
				_appCore.writeToAppLog("_trackEvent valid\n");
			}

			_trackMetric = (Telematics_TrackMetric) _telematicsLibrary.resolve("Telematics_TrackMetric");
			if (_trackMetric != Q_NULLPTR)
			{
				_appCore.writeToAppLog("_trackMetric valid\n");
			}
		}
	}

	if (_initializeProc == Q_NULLPTR)
	{
		_appCore.writeToAppLog("TelematicsManager::initialize == NULL\n");
	}

	return _valid;
}

QByteArray TelematicsManager::getVersion()
{
	QByteArray result;

	if (_getVersion != nullptr)
	{
		eTelematicsReturnCode rc;

		char temp[1024];

		rc = _getVersion(temp, sizeof(temp));
		if (rc == TELEMATICS_CLIENT_SUCCESS)
		{
			result = QByteArray(temp);
		}
	}

	return result;
}

void TelematicsManager::trackEvent
(
	const QByteArray &eventId,
	const QByteArray &eventData
)
{
	if (_trackEvent != Q_NULLPTR)
	{
		_trackEvent(eventId.data(), eventData.data());

		_appCore.writeToAppLog("TelematicsManager::trackEvent ");
		_appCore.writeToAppLog("Event ID:" + eventId + " Event Data: " + eventData + "\n");
	}
}

void TelematicsManager::trackMetric(const QByteArray& metricID, double metric)
{
	if (_trackMetric != Q_NULLPTR)
	{
		_trackMetric(metricID.data(), metric);

		_appCore.writeToAppLog("TelematicsManager::trackMetric ");
		_appCore.writeToAppLog("Metric ID:" + metricID + "\n");
	}
}

TelematicsManager* TelematicsManager::getInstance
(
	AppCore& appCore
)
{
	if (_instance == Q_NULLPTR)
		_instance = new TelematicsManager(appCore);

	return _instance;
}

void TelematicsManager::release()
{
	if (_instance != Q_NULLPTR)
	{
		delete _instance;
		_instance = Q_NULLPTR;
	}
}

void TelematicsManager::error(std::string errorString)
{
	qDebug() << errorString.data();
}

void TelematicsManager::warning(std::string warningString)
{
	qDebug() << warningString.data();
}

void TelematicsManager::info(std::string infoString)
{
	qDebug() << infoString.data();
}

