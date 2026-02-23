#ifndef WINUTILS_H
#define WINUTILS_H
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

#include <windows.h>
#include <SetupAPI.h>

class HandleMonitor
{
public:
	HandleMonitor(HANDLE handle = INVALID_HANDLE_VALUE) :
		_handle{handle}
	{
	}
	~HandleMonitor()
	{
		if (_handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(_handle);
			_handle = INVALID_HANDLE_VALUE;
		}
	}

	void montitor(HANDLE handle)
	{
		_handle = handle;
	}

private:
	HANDLE						_handle;
};

class LocalMemoryMonitor
{
public:
	LocalMemoryMonitor(HLOCAL handle = INVALID_HANDLE_VALUE) :
		_handle{handle}
	{
	}
	~LocalMemoryMonitor()
	{
		if (_handle != INVALID_HANDLE_VALUE)
		{
			LocalFree(_handle);
			_handle = INVALID_HANDLE_VALUE;
		}
	}

	void monitor(HLOCAL handle)
	{
		_handle = handle;
	}

	HLOCAL localMemory()
	{
		return _handle;
	}
private:
	HLOCAL						_handle;
};

class HDevMonitor
{
public:
	HDevMonitor(HDEVINFO handle = INVALID_HANDLE_VALUE) :
		_handle{handle}
	{
	}

	~HDevMonitor()
	{
		if (_handle != INVALID_HANDLE_VALUE)
		{
			SetupDiDestroyDeviceInfoList(_handle);
			_handle = INVALID_HANDLE_VALUE;
		}
	}

	void monitor(HDEVINFO handle)
	{
		_handle = handle;
	}

	HDEVINFO localMemory()
	{
		return _handle;
	}

private:
	HDEVINFO						_handle{INVALID_HANDLE_VALUE};
};


#endif // WINUTILS_H
