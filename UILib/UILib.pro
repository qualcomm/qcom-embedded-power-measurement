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
# Copyright ©2022 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

# Author: msimpson

include(../QCommon/Common.pri)

QT += core gui widgets serialport multimedia multimediawidgets

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM

CONFIG(debug, debug|release) {
	TARGET = UId
} else {
	TARGET = UI
}

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
	}
}

linux {

	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
	}
}

SOURCES += \
	ColorConversion.cpp \
	CommandButton.cpp \
	CustomValidator.cpp \
	EPMChannelTable.cpp \
	EPMDeviceDialog.cpp \
	EPMDeviceList.cpp \
	HoverAwareQWindow.cpp \
	I2CWidget.cpp \
	MediaPlaybackFrame.cpp \
	NotificationWidget.cpp \
	SeriesDataWindow.cpp \
	TACFrame.cpp \
	TerminalWidget.cpp \
	UserWidget.cpp \
	UILib.cpp \
	VariableInput.cpp

HEADERS += \
	ColorConversion.h \
	CommandButton.h \
	CustomValidator.h \
	EPMChannelTable.h \
	EPMDeviceDialog.h \
	EPMDeviceList.h \
	EPMDialog.h \
	HoverAwareQWindow.h \
	I2CWidget.h \
	MediaPlaybackFrame.h \
	NotificationWidget.h \
	SeriesDataWindow.h \
	TACFrame.h \
	TerminalWidget.h \
	UserWidget.h \
	UILib.h \
	UIGlobalLib.h \
	VariableInput.h

RESOURCES += \
	resources/UILib.qrc

FORMS += \
	EPMChannelTable.ui \
	EPMDeviceDialog.ui \
	EPMDeviceList.ui \
	EPMDialog.ui \
	HoverAwareQWindow.ui \
	I2CWidget.ui \
	MediaPlaybackFrame.ui \
	NotificationWidget.ui \
	SeriesDataWindow.ui \
	TACFrame.ui \
	TerminalWidget.ui \
	UserWidget.ui \
	VariableInput.ui
