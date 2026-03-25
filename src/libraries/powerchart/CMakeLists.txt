# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../qcommon-console/Common.cmake)

qepm_set_library_name(PowerChart)

qt_add_library(${LIBRARY_NAME} STATIC
    QEPMChart.cpp QEPMChart.h QEPMChart.ui
    QEPMChartLive.cpp QEPMChartLive.h QEPMChartLive.ui
    QEPMCursorLines.cpp QEPMCursorLines.h
    QEPMDataSeries.cpp QEPMDataSeries.h
    QEPMDataSeriesLive.cpp QEPMDataSeriesLive.h
    QEPMPlotCurveLive.h
    AxisWidget.cpp AxisWidget.h
    CurrentChart.cpp CurrentChart.h
    CurrentChartLive.cpp CurrentChartLive.h
    HorizontalPlotZoomer.cpp HorizontalPlotZoomer.h
    LineModelEntryLive.cpp LineModelEntryLive.h
    LineSeriesModel.cpp LineSeriesModel.h
    LineSeriesModelLive.cpp LineSeriesModelLive.h
    PowerChart.cpp PowerChart.h
    PowerChart_global.h
    VoltageChart.cpp VoltageChart.h
    VoltageChartLive.cpp VoltageChartLive.h
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    POWERCHART_LIBRARY
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Gui
    Qt6::Svg
    Qt6::Widgets
    QEPM::QCommon
    QEPM::QCommonConsole
    QEPM::EPMLib
    QEPM::UI
    qwt
)

add_library(QEPM::PowerChart ALIAS ${LIBRARY_NAME})
