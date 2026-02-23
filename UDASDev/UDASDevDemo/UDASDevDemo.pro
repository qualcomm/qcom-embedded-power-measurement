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
# Copyright ©2024 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson, biswroy

include(../../QCommonConsole/Common.pri)

QT += core gui widgets serialport

TARGET = UDASDevDemo
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$UDASDEV
INCLUDEPATH += $$LIBEPM

DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$UDASDEV
DEPENDPATH += $$LIBEPM

win32 {
        CONFIG(debug, debug|release) {
                PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
                PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
                PRE_TARGETDEPS += $$STATIC_LIBPATH/UDASDevd.lib
        } else {
                PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
                PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
                PRE_TARGETDEPS += $$STATIC_LIBPATH/UDASDev.lib
        }
}

linux {
        CONFIG(debug, debug|release) {
                PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
                PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
                PRE_TARGETDEPS += $$STATIC_LIBPATH/libUDASDevd.so
        } else {
                PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
                PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
                PRE_TARGETDEPS += $$STATIC_LIBPATH/libUDASDev.so
        }
}

SOURCES += \
	main.cpp

CONFIG(debug, debug|release) {
        LIBS += -L$$STATIC_LIBPATH -lQCommonConsoled -lUDASDevd
} else {
        LIBS += -L$$STATIC_LIBPATH -lQCommonConsole -lUDASDev
}

linux {
        LIBS += -ldl
}
