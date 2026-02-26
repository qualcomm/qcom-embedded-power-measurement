#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/
#include "QCommonConsoleGlobal.h"

#include <QRegularExpression>
#include <QString>

class QCOMMONCONSOLE_EXPORT SystemInformation
{
public:
	SystemInformation();

	QString computerName();
	QString userName();
	QString osName();
	QString osVersion();
	QString driverVersion();

	// memory
	QString totalPhysicalMemory();
	QString totalVirtualMemory();

private:
	QString						_computerName;
	QString						_userName;
	QString						_osName;
	QString						_osVersion;
	QRegularExpression			_expression;
	QString						_totalPhysicalMemory;
};

#endif // SYSTEMINFORMATION_H
