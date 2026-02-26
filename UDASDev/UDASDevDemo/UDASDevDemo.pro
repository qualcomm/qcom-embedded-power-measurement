# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

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
