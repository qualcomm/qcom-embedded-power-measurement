#ifndef WINUTILS_H
#define WINUTILS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
