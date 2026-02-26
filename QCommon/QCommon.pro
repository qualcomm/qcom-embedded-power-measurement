# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# Author: msimpson

include(Common.pri)

QT += gui widgets xml serialport concurrent network

CONFIG(debug, debug|release) {
    TARGET = QCommond
} else {
    TARGET = QCommon
}

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH
DEFINES += QCOMMON_LIBRARY

INCLUDEPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$QCOMMONCONSOLE

win32 {
		CONFIG(debug, debug|release) {
			PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		} else {
			PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		}
}

linux {
		CONFIG(debug, debug|release) {
			PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		} else {
			PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		}
}

SOURCES += \
    AboutDialog.cpp \
    AlpacaApplication.cpp \
    ApplicationEnhancements.cpp \
	ColorSwatch.cpp \
    HappinessDialog.cpp \
    IntFilteredEditor.cpp \
    LEDWidget.cpp \
    PainterEffects.cpp \
	PinLED.cpp \
    ProgressDialog.cpp \
    QCommon.cpp \
	StyledLabel.cpp \
    TableCheckBox.cpp \
	TableComboBox.cpp

HEADERS += \
    AboutDialog.h \
    AlpacaApplication.h \
    ApplicationEnhancements.h \
	ColorSwatch.h \
	CustomComboBox.h \
    HappinessDialog.h \
    IntFilteredEditor.h \
    LEDWidget.h \
    PainterEffects.h \
	PinLED.h \
	ProgressDialog.h \
    QCommon.h \
    QCommonGlobal.h \
	StyledLabel.h \
    TableCheckBox.h \
	TableComboBox.h

FORMS += \
	AboutDialog.ui \
    HappinessDialog.ui \
    PinLED.ui \
    ProgressDialog.ui

RESOURCES += \
    QCommon.qrc
