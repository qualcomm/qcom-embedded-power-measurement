# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../qcommon-console/Common.cmake)

qepm_set_library_name(EPMLib)

# Platform specific files
if(WIN32)
    set(PLATFORM_SOURCES
        win/EPMDevice3.cpp win/EPMDevice3.h
        win/EPMDevice4.cpp win/EPMDevice4.h
        win/WinBaseDevice.cpp win/WinBaseDevice.h
        win/WinUtils.h
    )
    set(PLATFORM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/win)
elseif(UNIX)
    set(PLATFORM_SOURCES
        linux/EPMDevice3.cpp linux/EPMDevice3.h
        linux/EPMDevice4.cpp linux/EPMDevice4.h
        linux/LibUsbInitializer.cpp linux/LibUsbInitializer.h
        linux/LinuxBaseDevice.cpp linux/LinuxBaseDevice.h
    )
    set(PLATFORM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/linux)
endif()

qt_add_library(${LIBRARY_NAME} STATIC
    BoardUuid.cpp BoardUuid.h
    DataSeries.cpp DataSeries.h
    DynamicDataSeries.cpp DynamicDataSeries.h
    EPMChannel.cpp EPMChannel.h
    EPMCompiler.cpp EPMCompiler.h
    EPMConfigFile.cpp EPMConfigFile.h
    EPMDefines.h
    EPMDevice.cpp EPMDevice.h
    EPMExportConfigFile.cpp EPMExportConfigFile.h
    EPMGlobalLib.h
    EPMIndexType.cpp EPMIndexType.h
    EPMLibDefines.h
    EPMPreferences.cpp EPMPreferences.h
    EPMProject.cpp EPMProject.h
    EPMResult.h
    EPMRun.cpp EPMRun.h
    EPMRuntimeChannel.cpp EPMRuntimeChannel.h
    EPMRuntimeConfigFile.cpp EPMRuntimeConfigFile.h
    EPMTarget.cpp EPMTarget.h
    EpmLibPrivate.cpp EpmLibPrivate.h
    EpmVersionInfo.h
    KratosMapping.cpp KratosMapping.h
    PRNFile.cpp PRNFile.h
    QTExcel.h
    RecordingInferface.h
    SPMVersion.h
    TargetInfo.h
    UDASBlock.cpp UDASBlock.h
    UDASBlockModel.cpp UDASBlockModel.h
    UDASChannelRecording.cpp UDASChannelRecording.h
    UDASDefines.h
    UDASFile.cpp UDASFile.h
    UDASRecorder.cpp UDASRecorder.h
    UDASPreferences.cpp UDASPreferences.h
    WaveForms.h

    ${PLATFORM_SOURCES}
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLATFORM_INCLUDE_DIR}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    EPMLIB_LIBRARY
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    QEPM::QCommon
    QEPM::QCommonConsole
    QEPM::LibExcel
)

if(WIN32)
    target_link_libraries(${LIBRARY_NAME} PUBLIC
        SetupAPI
        Winusb
    )
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb)
    target_link_libraries(${LIBRARY_NAME} PUBLIC PkgConfig::LIBUSB)
endif()

add_library(QEPM::EPMLib ALIAS ${LIBRARY_NAME})
