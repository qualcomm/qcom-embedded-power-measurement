# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause


include(../QCommonConsole/Common.pri)

QT += core

DLLDESTDIR = $$BINPATH

CONFIG(debug, debug|release) {
	TARGET = UDASDevd
} else {
	TARGET = UDASDev
}

TEMPLATE = lib
CONFIG += dll
DESTDIR = $$STATIC_LIBPATH

DEFINES += UDASDEV_LIBRARY

INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM

DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM

win32 {
    CONFIG(debug, debug|release) {
        PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
        PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
    } else {
        PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
        PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
    }
}

linux {
    CONFIG(debug, debug|release) {
        PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
        PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
    } else {
        PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
        PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
    }
}

SOURCES += \
	DevUDASCore.cpp \
	UDASDev.cpp

HEADERS += \
	DevUDASCore.h \
	UDASDev.h

CONFIG(debug, debug|release) {
    LIBS += -L$$STATIC_LIBPATH -lEPMLibd -lQCommonConsoled
} else {
    LIBS += -L$$STATIC_LIBPATH -lEPMLib -lQCommonConsole
}

win32 {
    LIBS += -lLibExcel
}
