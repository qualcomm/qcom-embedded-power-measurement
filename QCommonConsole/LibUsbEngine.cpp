// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson msimpson@qti.qualcomm.com
*/

#include "LibUsbEngine.h"
#include "LibUsbInitializer.h"

LibUsbEngine* LibUsbEngine::_libUSBEngine{Q_NULLPTR};

LibUsbEngine::LibUsbEngine()
{

}

LibUsbEngine::~LibUsbEngine()
{

}

LibUsbEngine *LibUsbEngine::getLibUSBEngine()
{
	if (LibUsbEngine::_libUSBEngine == Q_NULLPTR)
	{
		LibUsbEngine::_libUSBEngine = new LibUsbEngine;
		::LibUsbInitialize();
	}

	return LibUsbEngine::_libUSBEngine;
}

void LibUsbEngine::releaseLibUSBEngine()
{
	if (LibUsbEngine::_libUSBEngine != Q_NULLPTR)
	{
		delete LibUsbEngine::_libUSBEngine;
		LibUsbEngine::_libUSBEngine = Q_NULLPTR;
	}
}

QString LibUsbEngine::libUSBVersion()
{
	return ::LibUsbVersion();
}


