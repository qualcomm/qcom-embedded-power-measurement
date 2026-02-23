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
# Copyright ©2018-2024 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

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
