// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// libEPM
#include "UDASPreferences.h"

// QCommon
#include "AlpacaSharedLibrary.h"

class DevUDASCore:
	public AlpacaSharedLibrary
{
public:
	DevUDASCore();
	~DevUDASCore();

	bool initialize(const QByteArray& appName, const QByteArray& appVersion);

	bool licenseIsValid();

	void setLoggingState(bool state);
	bool getLoggingState();

	void setExportLocation(const QString& exportLocation);

	QString exportLocation();

	AppCore* appCore();

private:
	bool						_initialized{false};
	QString						_exportLocation;
	UDASPreferences				_preferences;
};
