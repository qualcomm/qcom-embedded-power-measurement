# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/qcommon-console/Common.cmake)

qepm_set_library_name(UDASDev)

qt_add_library(${LIBRARY_NAME} SHARED
    DevUDASCore.cpp DevUDASCore.h
    UDASDev.cpp UDASDev.h
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    UDASDEV_LIBRARY
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    QEPM::EPMLib
    QEPM::QCommonConsole
)

qepm_add_version_info(${LIBRARY_NAME}
    "UDAS Device Interface Library"
    "${QEPM_VERSION}"
)

add_library(QEPM::UDASDev ALIAS ${LIBRARY_NAME})
