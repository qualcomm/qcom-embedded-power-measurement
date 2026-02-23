include(../QCommonConsole/Common.pri)

QT += core serialport network

TARGET = SCLDump
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
	}
}

SOURCES += \
        main.cpp

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lEPMLibd -lQCommonConsoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lEPMLib -lQCommonConsole
}

win32 {
	LIBS += -lLibExcel
}
