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

# Author: msimpson, biswroy, arkosen

include(../QCommon/Common.pri)

QT += core gui widgets serialport multimedia multimediawidgets

TARGET = DeviceCatalog
TEMPLATE = app
RC_ICONS = $$PWD/Resources/DeviceCatalog.ico

VERSION = $$DEVICE_CATALOG_VERSION    # major.minor.patch

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM
INCLUDEPATH += $$LIBUI
INCLUDEPATH += ../UILib

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM
DEPENDPATH += $$LIBUI

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UId.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UI.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUId.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUI.a
	}
}

SOURCES += \
	DeviceCatalogApplication.cpp \
	DeviceSelectionDialog.cpp \
	main.cpp \
	DeviceCatalog.cpp

HEADERS += \
	DeviceCatalog.h \
	DeviceCatalogApplication.h \
	DeviceSelectionDialog.h

FORMS += \
	DeviceCatalog.ui \
	DeviceSelectionDialog.ui

CONFIG(debug, debug|release) {
		LIBS += -L$$STATIC_LIBPATH -lQCommond -lEPMLibd -lQCommonConsoled -lUId
} else {
		LIBS += -L$$STATIC_LIBPATH -lQCommon -lEPMLib -lQCommonConsole -lUI
}

win32 {
		LIBS += -lSetupAPI -lWinusb -lOle32
}

linux {
		LIBS += -lusb
		LIBS += -ldl
}

LIBS += -L$$STATIC_LIBPATH -lftd2xx

RESOURCES += \
	DeviceCatalog.qrc

SUBDIRS += \
	../UILib/UILib.pro
