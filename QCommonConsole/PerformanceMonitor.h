#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
