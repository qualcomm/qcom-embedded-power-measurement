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
# Copyright ©2018 Qualcomm Technologies, Inc.
# All rights reserved.
# Qualcomm Technologies Confidential and Proprietary

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
