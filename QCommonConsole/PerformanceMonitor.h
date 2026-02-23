#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H
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

#include <QtCore/qglobal.h>
#include <QtCore/QString>

#include "TickCount.h"

class PerformanceMonitor
{
public:
	PerformanceMonitor(const char* function, const char* file, quint32 line, quint64 tickCount = ::tickCount())
	{
		_function = function;
		_file = file;
		_line = line;
		_tickCount = tickCount;
	}

	~PerformanceMonitor();
	
	QString						_function;
	QString						_file;
	quint32						_line;
	quint64						_tickCount;
};

#ifdef Q_OS_WIN
	#define FUNCTION __FUNCTION__
#endif 

#ifdef Q_OS_LINUX
	#define FUNCTION  __PRETTY_FUNCTION__
#endif

#ifdef Q_OS_MAC
    #define FUNCTION  __func__
#endif 

#ifdef _DEBUG
	#define MONITOR_ME \
		PerformanceMonitor performanceMonitor(FUNCTION, __FILE__, __LINE__);
#else
	#define MONITOR_ME 
#endif

#endif // PERFORMANCEMONITOR_H
