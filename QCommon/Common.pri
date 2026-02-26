# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause


include($$PWD/../QCommonConsole/version.pri)

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QCOMMON = "$$PWD"

QCOMMONCONSOLE = "$$PWD/../QCommonConsole"
LIBEPM = "$$PWD/../EPMLib"
LIBUI = "$$PWD/../UILib"

TACDEV = "$$PWD/../TACDev"
POWERCHART = "$$PWD/../PowerChart"

# third party
win32 {
	QWT = C:\qwt-6.2.0
	QWTINC = $$QWT/include
	QWTLIB = $$QWT/lib
}

linux {
	QWT = /usr/local/qwt-6.2.0
	QWTINC = $$QWT/include
	QWTLIB = $$QWT/lib

	LIBUSB = /usr/local/libusb-1.0.27
	LIBUSBINC = $$LIBUSB/include/libusb-1.0
	LIBUSBLIB = $$LIBUSB/lib
}

CONFIG += skip_target_version_ext

DEFINES += ALPACA_STATIC

message($$QCOMMONCONSOLE)

win32 {
	CONFIG += c++20
	QMAKE_CXXFLAGS += /Zi
    QMAKE_LFLAGS += /INCREMENTAL:NO
}

linux {
    CONFIG += c++2a c++1z c++14
}

CONFIG(x32) {
    message(Building 32 bit)
	win32: DEFINES += _WIN32
    linux: DEFINES += __i386__
    macx: DEFINES += __i386__
	WINTARGET = "Win32"
} else {
    message(Building 64 bit)
    linux: DEFINES += __X86_64__
    macx: DEFINES += __X86_64__
	WINTARGET = "x64"
}

CONFIG(debug, debug|release) {
	DEFINES += _DEBUG DEBUG
	win32: CONFIGURATION = "$$WINTARGET/Debug"
	mac: CONFIGURATION = "Mac/Debug"
	linux: CONFIGURATION = "Linux/Debug"
}
else {
	DEFINES += _NDEBUG NDEBUG
	win32: CONFIGURATION = "$$WINTARGET/Release"
	mac: CONFIGURATION = "Mac/Release"
	linux: CONFIGURATION = "Linux/Release"
}

BUILDROOT = $$PWD/../__Builds/$$CONFIGURATION
BINPATH = "$$BUILDROOT/bin"
DESTDIR = $$BINPATH
OBJECTS_DIR = "$$BUILDROOT/obj/$$TARGET"
STATIC_LIBPATH = "$$BUILDROOT/lib"
MOC_DIR = "$$BUILDROOT/moc/$$TARGET"
UI_DIR = "$$BUILDROOT/ui/$$TARGET"

INCLUDEPATH += "$$BUILDROOT/ui/QCommon"

unix:!mac {
#	QT_STATICLIBS = "/local/mnt/workspace/static/lib"
#	LIBS = -L$$QT_STATICLIBS -lQt5Core -lqtpcre2
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
    QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN/../lib
    QMAKE_LFLAGS += -fPIC
    QMAKE_RPATH=
    QMAKE_CXXFLAGS_WARN_ON += -Wno-pragmas
}

unix: {
	QMAKE_CXXFLAGS += -Werror -Wno-unused-result -Wno-write-strings -Wno-comment -Wno-unused-function -Wno-reorder -Wno-unknown-pragmas -Wno-conversion-null -std=c++0x
	QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder -Wno-unused-parameter -Wno-overloaded-virtual -Wno-unused-function -Wno-unused-variable -Wno-date-time
}

unix:mac {
#for xcode 7, use this one.
   #QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field -Wno-inconsistent-missing-override
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field
}

QMAKE_TARGET_COMPANY="Qualcomm, Inc."
QMAKE_TARGET_COPYRIGHT="2026 Qualcomm, Inc."
QMAKE_TARGET_DESCRIPTION="Alpaca Application Suite"
