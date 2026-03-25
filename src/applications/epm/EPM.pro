# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause


include(../QCommon/Common.pri)

QT += core gui widgets serialport

TARGET = EPM
TEMPLATE = app
RC_ICONS = $$PWD/Resources/EPM.ico

VERSION = $$EPM_VERSION

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM
INCLUDEPATH += $$POWERCHART
INCLUDEPATH += $$LIBUI

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM
DEPENDPATH += $$POWERCHART
DEPENDPATH += $$LIBUI

win32 {

	QT += axcontainer # for Excel operations

	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/PowerChartd.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/PowerChart.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libPowerChartd.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libPowerChart.a
	}
}

SOURCES += \
	main.cpp \
	EPMApplication.cpp \
	EPMWindow.cpp \
	PreferencesDialog.cpp \
    QuitEPMEvent.cpp

HEADERS += \
	EPMApplication.h \
	EPMWindow.h \
	PreferencesDialog.h \
	QuitEPMEvent.h

FORMS += \
    EPMDeviceSetup.ui \
    EPMWindow.ui \
    HelpDialog.ui \
    PreferencesDialog.ui

CONFIG(debug, debug|release) {
	mac {
		QMAKE_POST_LINK = $$PWD/debug_plugin.sh $$TARGET $$BINPATH
	}
}

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lUId -lPowerChartd -lEPMLibd -lQCommond -lQCommonConsoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lUI -lPowerChart -lEPMLib -lQCommon -lQCommonConsole
}

win32 {
	LIBS += -lLibExcel -lSetupAPI -lWinusb -lOle32
}

linux {
	LIBS += -L/usr/local/lib -lusb
}

unix {
	LIBS += -rdynamic
	linux {
		LIBS += -lrt
		LIBS += -ldl
		LIBS += -luuid
	}
	mac {
		QMAKE_LFLAGS += -F/System/Library/Frameworks/CoreFoundation.framework
		QMAKE_LFLAGS += -F/System/Library/Frameworks/IOKit.framework
		QMAKE_LFLAGS += -framework Carbon
		ICON = $$PWD/resources/emp.icns
		LIBS += -framework CoreFoundation
		LIBS += -framework IOKit
		LIBS += -lobjc
	}
}

RESOURCES += \
    Resources/EPM.qrc
