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
# Copyright 2018-2023 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson

include(../QCommon/Common.pri)

QT += core gui widgets serialport network opengl svg

TARGET = EPMViewer

TEMPLATE = app
RC_ICONS = $$PWD/Resources/AlpacaViewer.ico

VERSION = $$EPM_VIEWER_VERSION

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$POWERCHART
INCLUDEPATH += $$LIBUI
INCLUDEPATH += $$LIBEPM
INCLUDEPATH += $$QWTINC

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$POWERCHART
DEPENDPATH += $$LIBUI
DEPENDPATH += $$LIBEPM
DEPENDPATH += $$QWTINC

win32 {
	QT += axcontainer # for Excel operations

	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/PowerChartd.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UId.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/PowerChart.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UI.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libPowerChartd.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUId.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libqwt.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libPowerChart.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUI.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libqwt.a
	}
}

SOURCES += \
	EPMViewerApplication.cpp \
	PreferencesDialog.cpp \
	main.cpp \
	EPMViewerWindow.cpp

HEADERS += \
	EPMViewerApplication.h \
	EPMViewerDefines.h \
	EPMViewerWindow.h \
	PreferencesDialog.h

FORMS += \
		EPMViewerWindow.ui \
		PreferencesDialog.ui

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lUId -lPowerChartd -lEPMLibd -lQCommond  -lQCommonConsoled
	LIBS += -L$$QWTLIB -lqwtd
} else {
	LIBS += -L$$STATIC_LIBPATH -lUI -lPowerChart -lEPMLib -lQCommon -lQCommonConsole
	LIBS += -L$$QWTLIB -lqwt
}

linux {
	LIBS += -L/usr/local/lib -lusb
}

win32 {
	LIBS += -lLibExcel -lSetupAPI -lWinusb -lAdvapi32 -lOle32
}

RESOURCES += \
	Resources/AlpacaViewer.qrc
