include(../QCommonConsole/Common.pri)

QT += core

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE

DEFINES += EPMLIB_LIBRARY

CONFIG(debug, debug|release) {
	TARGET = EPMLibd
} else {
	TARGET = EPMLib
}

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
	BoardUuid.cpp \
	DataSeries.cpp \
	DynamicDataSeries.cpp \
	EPMChannel.cpp \
	EPMCompiler.cpp \
	EPMConfigFile.cpp \
	EPMDevice.cpp \
	EPMExportConfigFile.cpp \
	EPMIndexType.cpp \
	EPMPreferences.cpp \
	EPMProject.cpp \
	EPMRun.cpp \
	EPMRuntimeChannel.cpp \
	EPMRuntimeConfigFile.cpp \
	EPMTarget.cpp \
	EpmLibPrivate.cpp \
	KratosMapping.cpp \
	PRNFile.cpp \
	UDASBlock.cpp \
	UDASBlockModel.cpp \
	UDASChannelRecording.cpp \
	UDASFile.cpp \
	UDASRecorder.cpp \
	UDASPreferences.cpp

HEADERS += \
	BoardUuid.h \
	DataSeries.h \
	DynamicDataSeries.h \
	EPMChannel.h \
	EPMCompiler.h \
	EPMConfigFile.h \
	EPMDefines.h \
	EPMDevice.h \
	EPMExportConfigFile.h \
	EPMGlobalLib.h \
	EPMIndexType.h \
	EPMLibDefines.h \
	EPMPreferences.h \
	EPMProject.h \
	EPMResult.h \
	EPMRun.h \
	EPMRuntimeChannel.h \
	EPMRuntimeConfigFile.h \
	EPMTarget.h \
	EpmLibPrivate.h \
	EpmVersionInfo.h \
	KratosMapping.h \
	PRNFile.h \
	QTExcel.h \
	RecordingInferface.h \
	SPMVersion.h \
	TargetInfo.h \
	UDASBlock.h \
	UDASBlockModel.h \
	UDASChannelRecording.h \
	UDASDefines.h \
	UDASFile.h \
	UDASRecorder.h \
	WaveForms.h \
	UDASPreferences.h

win32 {
	INCLUDEPATH += $$PWD/win/

	HEADERS += \
		win/WinUtils.h \
		win/EPMDevice3.h \
		win/EPMDevice4.h \
		win/WinBaseDevice.h

	SOURCES += \
		win/EPMDevice3.cpp \
		win/EPMDevice4.cpp \
		win/WinBaseDevice.cpp
}

linux {
	INCLUDEPATH += $$PWD/linux/

	HEADERS += \
		linux/EPMDevice3.h \
		linux/EPMDevice4.h \
		linux/LibUsbInitializer.h \
		linux/LinuxBaseDevice.h


	SOURCES += \
		linux/EPMDevice3.cpp \
		linux/EPMDevice4.cpp \
		linux/LibUsbInitializer.cpp \
		linux/LinuxBaseDevice.cpp
}

#CONFIG(debug, debug|release) {
#	LIBS += -L$$STATIC_LIBPATH -lQCommond
#} else {
#	LIBS += -L$$STATIC_LIBPATH -lQCommon
#}

#win32 {
#    LIBS += -lSetupAPI -lWinusb -lOle32 -lAdvapi32 -lShell32
#}

#unix {
#	LIBS += -lusb
#}
