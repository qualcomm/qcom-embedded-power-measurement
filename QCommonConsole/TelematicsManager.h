#ifndef TELEMATICSMANAGER_H
#define TELEMATICSMANAGER_H
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

// QCommon
#include "AppCore.h"

// Qt
#include <QByteArray>
#include <QLibrary>
#include <QObject>

enum eTelematicsReturnCode
{
   TELEMATICS_CLIENT_SUCCESS      = 0,
   TELEMATICS_FILE_ACCESS_ERROR   = 1,
   TELEMATICS_INVALID_MACHINE     = 2,
   TELEMATICS_INVALID_ID          = 3,
   TELEMATICS_LICENSE_UNAVAILABLE = 4,
   TELEMATICS_DECODE_ERROR        = 5,
   TELEMATICS_ENCODE_ERROR        = 6,
   TELEMATICS_SERVER_ERROR        = 7,
   TELEMATICS_PARSING_ERROR       = 8,
   TELEMATICS_STALE_UPDATE        = 9,
   TELEMATICS_FILE_SIZE_EXEEDED   = 10,
   TELEMATICS_OTHER_ERROR         = 99
};

typedef eTelematicsReturnCode (*Telematics_Initialize)(const char* productId, const char* metaData);
typedef eTelematicsReturnCode (*Telematics_GetVersion)(char* Ver, int nlen);
typedef eTelematicsReturnCode (*Telematics_TrackEvent)(const char* eventId, const char* eventData);
typedef eTelematicsReturnCode (*Telematics_TrackMetric)(const char* metricId, double metricValue);

typedef void (*SetAppCore)(AppCore* appCore);

class TelematicsManager :
	public QObject
{
Q_OBJECT

  public:
	virtual ~TelematicsManager();

	bool initialize(const QByteArray& productID);
	bool valid()
	{
		return _valid && _telematicsLibrary.isLoaded();
	}

	QByteArray getVersion();
	void trackEvent(const QByteArray& eventId, const QByteArray& eventData);
	void trackMetric(const QByteArray& eventId, double metric);

	static TelematicsManager* getInstance(AppCore& appCore);
	static void release();

	// Telematics Callbacks
	virtual void error(std::string errorString);
	virtual void warning( std::string warningString );
	virtual void info( std::string infoString);

private:
	TelematicsManager(AppCore& appCore);

	QLibrary							_telematicsLibrary;
	Telematics_Initialize				_initializeProc{Q_NULLPTR};
	Telematics_GetVersion				_getVersion{Q_NULLPTR};
	Telematics_TrackEvent				_trackEvent{Q_NULLPTR};
	Telematics_TrackMetric				_trackMetric{Q_NULLPTR};

	static TelematicsManager*			_instance;
	eTelematicsReturnCode				_lastReturnCode{TELEMATICS_CLIENT_SUCCESS};
	bool								_valid{false};
	AppCore&							_appCore;
};

#endif // TELEMATICSMANAGER_H
