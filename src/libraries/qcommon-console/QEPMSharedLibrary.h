#ifndef QEPMSHAREDLIBRARY_H
#define QEPMSHAREDLIBRARY_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause


#include "QCommonConsoleGlobal.h"

// QCommon
#include "AppCore.h"
#include "PreferencesBase.h"

class QCOMMONCONSOLE_EXPORT QEPMSharedLibrary
{
public:
	explicit QEPMSharedLibrary();
	~QEPMSharedLibrary();

	bool initialize(const QByteArray& appName, const QByteArray& appVersion, PreferencesBase* preferencesBase);

	static AppCore* getAppCore();

	bool licenseIsValid();

	void setLoggingState(bool state);
	bool getLoggingState();

	QByteArray lastError()
	{
		QByteArray result{_lastError};

		_lastError.clear();
		return result;
	}

	void setLastError(const QByteArray& lastError)
	{
		_lastError = lastError;
	}
protected:
	QString						_appName;
	QString						_appVersion;
	QByteArray					_lastError;
	static AppCore*				_appCore;
	bool						_validLicense{true};
};

#endif // QEPMSHAREDLIBRARY_H
