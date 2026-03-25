# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause


include(../QCommonConsole/Common.pri)

QT += core serialport

CONFIG(debug, debug|release) {
	TARGET = EPMDevd
} else {
	TARGET = EPMDev
}

TEMPLATE = lib
CONFIG += dll
DLLDESTDIR = $$BINPATH
DESTDIR = $$STATIC_LIBPATH

DEFINES += EPMDEV_LIBRARY

INCLUDEPATH += $$LIBEPM
INCLUDEPATH += $$QCOMMONCONSOLE

DEPENDPATH += $$LIBEPM
DEPENDPATH += $$QCOMMONCONSOLE

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
	DevEPMCore.cpp \
	EPMDev.cpp

HEADERS += \
	DevEPMCore.h \
	EPMDev.h

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lEPMLibd -lQCommonConsoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lEPMLib -lQCommonConsole
}

win32 {
	LIBS += -lLibExcel -lSetupAPI -lWinusb -lOle32
}

linux {
	LIBS += -lusb
}
