# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

qepm_set_library_name(QCommonConsole)

qt_add_library(${LIBRARY_NAME} STATIC
    AlpacaDefines.cpp AlpacaDefines.h
    AlpacaSettings.cpp AlpacaSettings.h
    QEPMSharedLibrary.cpp QEPMSharedLibrary.h
    AppCore.cpp AppCore.h
    BasicColor.cpp BasicColor.h
    CommandLineParser.cpp CommandLineParser.h
    ConsoleApplicationEnhancements.cpp ConsoleApplicationEnhancements.h
    ConsoleInterface.h
    CountingStream.cpp CountingStream.h
    CSV.cpp CSV.h
    DataPoints.h
    DebugBoardType.cpp DebugBoardType.h
    mymemcpy.cpp mymemcpy.h
    Notification.h
    PlatformID.cpp PlatformID.h
    PreferencesBase.cpp PreferencesBase.h
    QCommonConsole.cpp QCommonConsole.h
    QCommonConsoleGlobal.h
    QualcommChartColor.cpp QualcommChartColor.h
    QualcommColors.h
    QuitAppEvent.cpp QuitAppEvent.h
    Range.h
    RangedContainer.h
    ReceiveInterface.h
    RecentFiles.cpp RecentFiles.h
    SendInterface.h
    SerialPort.cpp SerialPort.h
    SerialPortInfo.cpp SerialPortInfo.h
    SerialPreferences.h
    SerialTableModel.cpp SerialTableModel.h
    Statistics.cpp Statistics.h
    StringUtilities.cpp StringUtilities.h
    SubSystemInterface.h
    SystemInformation.h
    SystemUsername.cpp SystemUsername.h
    ThreadedLog.cpp ThreadedLog.h
    TickCount.cpp TickCount.h
    version.h
)

if(WIN32)
    target_sources(${LIBRARY_NAME} PRIVATE
        SystemInformationWin.cpp
    )
endif()

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${QEPM_VERSION}
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QCOMMONCONSOLE_LIBRARY
    QEPM_STATIC
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG;RELEASE>
    $<$<PLATFORM_ID:Windows>:FTD2XX_STATIC>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    Qt6::SerialPort
    Qt6::Xml
)

add_library(QEPM::QCommonConsole ALIAS ${LIBRARY_NAME})
