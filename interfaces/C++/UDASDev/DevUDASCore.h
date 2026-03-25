// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// libEPM
#include "UDASPreferences.h"

// QCommon
#include "QEPMSharedLibrary.h"

class DevUDASCore:
	public QEPMSharedLibrary
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
