# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# Author: msimpson, biswroy

QT += core gui widgets serialport network

include(../QCommon/Common.pri)

TARGET = EPMConfigurationEditor
TEMPLATE = app
RC_ICONS = $$PWD/Resources/EmpConfEditor.ico

VERSION = $$EPM_EDITOR_VERSION

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM
INCLUDEPATH += $$LIBUI

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM
DEPENDPATH += $$LIBUI

win32 {

	QT += axcontainer # for Excel operations

	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UId.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UI.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUId.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUI.a
	}
}

QMAKE_INFO_PLIST = $$PWD/EmpConfEditor.plist

SOURCES += \
	Preferences.cpp \
	PreferencesDialog.cpp \
	main.cpp \
	EPMConfigurationApplication.cpp \
	EPMConfigurationWindow.cpp \
	CompileFileDialog.cpp

HEADERS += \
	EPMConfigurationApplication.h \
	EPMConfigurationWindow.h \
	EPMEditorDefines.h \
	CompileFileDialog.h \
	Preferences.h \
	PreferencesDialog.h

FORMS += \
	EPMConfigurationWindow.ui \
	PreferencesDialog.ui

CONFIG(debug, debug|release) {
	mac {
		QMAKE_POST_LINK = $$PWD/debug_plugin.sh $$TARGET $$BINPATH
	}
}

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lUId -lEPMLibd -lQCommond -lQCommonConsoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lUI -lEPMLib -lQCommon -lQCommonConsole
}

win32 {
	LIBS += -lLibExcel -lSetupAPI -lWinusb
}

unix {
	LIBS += -L/usr/local/lib
	LIBS += -rdynamic
	linux {
		LIBS += -lusb
		LIBS += -lrt
		LIBS += -ldl
		LIBS += -luuid
	}
	mac {
		QMAKE_LFLAGS += -F/System/Library/Frameworks/CoreFoundation.framework
		QMAKE_LFLAGS += -F/System/Library/Frameworks/IOKit.framework
		QMAKE_LFLAGS += -framework Carbon
		ICON = $$PWD/resources/qxdm.icns
		LIBS += -framework CoreFoundation
		LIBS += -framework IOKit
		LIBS += -lobjc
	}
}

RESOURCES += \
	Resources/EPMConfigurationEditor.qrc
