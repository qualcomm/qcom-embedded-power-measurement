# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/qcommon-console/Common.cmake)

qepm_set_library_name(EPMDev)

qt_add_library(${LIBRARY_NAME} SHARED
    DevEPMCore.cpp DevEPMCore.h
    EPMDev.cpp EPMDev.h
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    EPMDEV_LIBRARY
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    Qt6::SerialPort
    QEPM::EPMLib
    QEPM::QCommonConsole
)

qepm_add_version_info(${LIBRARY_NAME}
    "EPM Device Interface Library"
    "${QEPM_VERSION}"
)

add_library(QEPM::EPMDev ALIAS ${LIBRARY_NAME})
