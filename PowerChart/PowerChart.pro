include(../QCommon/Common.pri)

QT += gui svg widgets

CONFIG(debug, debug|release) {
	TARGET = PowerChartd
} else {
	TARGET = PowerChart
}

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH

DEFINES += POWERCHART_LIBRARY

INCLUDEPATH += $$QCOMMON
INCLUDEPATH += $$QCOMMONCONSOLE
INCLUDEPATH += $$LIBEPM
INCLUDEPATH += $$LIBUI
INCLUDEPATH += $$QWTINC

DEPENDPATH += $$QCOMMON
DEPENDPATH += $$QCOMMONCONSOLE
DEPENDPATH += $$LIBEPM
DEPENDPATH += $$LIBUI
DEPENDPATH += $$QWTINC

win32 {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommond.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsoled.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLibd.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UId.lib
		PRE_TARGETDEPS += $$QWTLIB/Qwtd.lib
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommon.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/QCommonConsole.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/EPMLib.lib
		PRE_TARGETDEPS += $$STATIC_LIBPATH/UI.lib
		PRE_TARGETDEPS += $$QWTLIB/Qwt.lib
	}
}

linux {
	CONFIG(debug, debug|release) {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommond.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsoled.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLibd.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUId.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libqwt.a
	} else {
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommon.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libQCommonConsole.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libEPMLib.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libUI.a
		PRE_TARGETDEPS += $$STATIC_LIBPATH/libqwt.a
	}
}

SOURCES += \
	AlpacaChart.cpp \
	AlpacaChartLive.cpp \
	AlpacaCursorLines.cpp \
	AlpacaDataSeries.cpp \
	AlpacaDataSeriesLive.cpp \
	AxisWidget.cpp \
	CurrentChart.cpp \
	CurrentChartLive.cpp \
	HorizontalPlotZoomer.cpp \
	LineModelEntryLive.cpp \
	LineSeriesModel.cpp \
	LineSeriesModelLive.cpp \
	PowerChart.cpp \
	VoltageChart.cpp \
	VoltageChartLive.cpp

HEADERS += \
	AlpacaChart.h \
	AlpacaChartLive.h \
	AlpacaCursorLines.h \
	AlpacaDataSeries.h \
	AlpacaDataSeriesLive.h \
	AlpacaPlotCurveLive.h \
	AxisWidget.h \
	CurrentChart.h \
	CurrentChartLive.h \
	HorizontalPlotZoomer.h \
	LineModelEntryLive.h \
	LineSeriesModel.h \
	LineSeriesModelLive.h \
	PowerChart.h \
	PowerChart_global.h \
	VoltageChart.h \
	VoltageChartLive.h

FORMS += \
	AlpacaChart.ui \
	AlpacaChartLive.ui

# LIBS += -L$$STATIC_LIBPATH -lqwt

CONFIG(debug, debug|release) {
	LIBS += -L$$STATIC_LIBPATH -lqwtd
} else {
	LIBS += -L$$STATIC_LIBPATH -lqwt
}

