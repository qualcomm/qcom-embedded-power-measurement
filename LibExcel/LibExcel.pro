# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# Author: msimpson

include(../QCommon/Common.pri)

QT += widgets

QMAKE_CXXFLAGS += /constexpr:steps10485760

win32 {
	QT += axcontainer
	CONFIG += c++20
	QMAKE_CXXFLAGS += /Zi /bigobj
	QMAKE_LFLAGS += /INCREMENTAL:NO
}

TARGET = LibExcel

TEMPLATE = lib
CONFIG += dll
DESTDIR = $$STATIC_LIBPATH

DEFINES += EXCEL_LIBRARY

SOURCES += \
	QTExcel.cpp

HEADERS += \
	QTExcel.h \
	Range.h

win32 {
	SOURCES += \
		excel.cpp

	HEADERS += \
		excel.h
}
