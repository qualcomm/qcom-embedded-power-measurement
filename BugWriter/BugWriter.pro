include(../QCommon/Common.pri)

QT += core gui widgets serialport network

TARGET = BugWriter
TEMPLATE = app
RC_ICONS = $$PWD/Resources/BugWriter.ico

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
	}
}

VERSION = $$BUG_WRITER_VERSION    # major.minor.patch

DESTDIR = $$BINPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE

SOURCES += \
	BugWriterApplication.cpp \
	BugWriterPreferences.cpp \
	EmailWriter.cpp \
	main.cpp \
	BugWriterWindow.cpp

HEADERS += \
	BugWriterApplication.h \
	BugWriterDefines.h \
	BugWriterPreferences.h \
	BugWriterWindow.h \
	EmailWriter.h

FORMS += \
	BugWriterWindow.ui

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lQCommond -lQCommonConsoled
} else {
	LIBS += -L$$STATIC_LIBPATH -lQCommon -lQCommonConsole
}

win32 {
	LIBS += -lAdvapi32
}

LIBS += -L$$STATIC_LIBPATH -lftd2xx

RESOURCES += \
	Resources/BugWriter.qrc
