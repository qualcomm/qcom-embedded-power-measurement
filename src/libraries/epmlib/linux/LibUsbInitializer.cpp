#include "LibUsbInitializer.h"

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <usb.h>

#include <QSettings>

class LibUsbIntializer
{
public:
	LibUsbIntializer()
	{

#if defined Q_OS_LINUX || defined Q_OS_MAC
		_initialized = false;
#endif
	}

	~LibUsbIntializer()
    {
        _initialized = false;
	}

	void Initialize()
	{
		if (_initialized)
			return;

        usb_init();

        _initialized = true;

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

            usb_set_debug(level);

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
	if (gLibUsbInitializer.Initialized() == false)
		gLibUsbInitializer.Initialize();
}

QString LibUsbVersion()
{
	QString version;

//	if (LibUsbInitialized())
//	{
//		const libusb_version* libUsbVersion = libusb_get_version();

//		version = QString("%1.%2.%3.%4").arg(libUsbVersion->major).arg(libUsbVersion->minor).arg(libUsbVersion->micro).arg(libUsbVersion->nano);
//	}

	return version;
}
