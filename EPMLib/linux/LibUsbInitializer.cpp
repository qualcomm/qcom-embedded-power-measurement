#include "LibUsbInitializer.h"

// Confidential and Proprietary ? Qualcomm Technologies, Inc.

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
// Copyright ? 2013-2016 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	$Id: //depot/DM/dev/qxdm-qt/QUsb/LibUsbInitializer.cpp#10 $
	$Header: //depot/DM/dev/qxdm-qt/QUsb/LibUsbInitializer.cpp#10 $
	$Date: 2015/12/29 $
	$DateTime: 2015/12/29 12:20:51 $
	$Change: 831580 $
	$File: //depot/DM/dev/qxdm-qt/QUsb/LibUsbInitializer.cpp $
	$Revision: #10 $
	%Author: msimpson %
*/

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
