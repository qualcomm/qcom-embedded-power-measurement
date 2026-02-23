# Confidential and Proprietary – Qualcomm Technologies, Inc.

# NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
# to: DocCtrlAgent@qualcomm.com.

# RESTRICTED USE AND DISCLOSURE:
# This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
# or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
# of Qualcomm Technologies, Inc.

# Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
# QUALCOMM Incorporated trademarks are used with permission.

# This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
# and international law is strictly prohibited.

# Qualcomm Technologies, Inc.
# 5775 Morehouse Drive
# San Diego, CA 92121 U.S.A.
# Copyright ©2018 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson

QT += core serialport network

include(../QCommonConsole/Common.pri)

TARGET = EPMDump
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM
INCLUDEPATH += $$EPMDEV

DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM
DEPENDPATH += $$EPMDEV

win32 {
	CONFIG(debug, debug|release) {
		DLLDESTDIR = $$BINPATH
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMDevd.lib
	}
	else {
		DLLDESTDIR = $$BINPATH
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMDev.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMDevd.so
	}
	else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMDev.so
	}
}

SOURCES += \
		main.cpp

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lEPMLibd -lEPMDevd -lQCommonConsoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lEPMLib -lEPMDev -lQCommonConsole
}

linux {
	LIBS += -ldl -lusb
}

win32 {
	LIBS += -lSetupAPI -lWinusb
}
