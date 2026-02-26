// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "LibUsbInitializer.h"

// Linux
#ifdef Q_OS_LINUX
//	#include "libusb.h"
#endif

// Qt
#include <QSettings>

class LibUsbIntializer
{
public:
	LibUsbIntializer()
	{
		_initialized = false;
	}

	~LibUsbIntializer()
	{
		if (_initialized)
		{
#ifdef Q_OS_LINUX
//			libusb_exit(NULL);
			_initialized = false;
#endif
		}
	}

	void Initialize()
	{
		if (_initialized)
			return;
#ifdef Q_OS_LINUX
//		_initialized = libusb_init(NULL) == 0;
#endif
		if (_initialized)
		{
			QSettings settings("libusb", "debug_level");
	
			settings.beginGroup("libusb");
	
			int level = settings.value("debug_level", 99).toInt();
			if (level == 99)
			{
				settings.setValue("debug_level", 0);
				level = 0;
			}
			else if (level < 0)
				level = 0;
			else if (level > 4)
				level = 4;

#ifdef Q_OS_LINUX
//			libusb_set_debug(NULL, level);
#endif
			settings.endGroup();
		}
	}
	
	bool Initialized(void)
	{
		return _initialized;
	}

	bool						_initialized{false};
} gLibUsbInitializer;

void LibUsbInitialize()
{
	gLibUsbInitializer.Initialize();
}

bool LibUsbInitialized()
{
	if (gLibUsbInitializer.Initialized() == false)
		gLibUsbInitializer.Initialize();
		
	return gLibUsbInitializer.Initialized();
}

QString LibUsbVersion()
{
	QString version;

#ifdef Q_OS_LINUX
	if (LibUsbInitialized())
	{
//		const libusb_version* libUsbVersion = libusb_get_version();

//		version = QString("%1.%2.%3.%4").arg(libUsbVersion->major).arg(libUsbVersion->minor).arg(libUsbVersion->micro).arg(libUsbVersion->nano);
	}
#endif
	return version;
}
