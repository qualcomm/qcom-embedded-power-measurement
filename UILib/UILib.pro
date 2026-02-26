# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

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
