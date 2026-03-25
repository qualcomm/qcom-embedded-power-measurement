# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/qcommon-console/Common.cmake)

qt_add_executable(UDASCLI
    main.cpp
)

qepm_add_version_info(UDASCLI
    "UDAS Command Line Interface"
    "${QEPM_VERSION}"
)

if(UNIX AND NOT APPLE)
    target_link_libraries(UDASCLI PRIVATE dl)
endif()

QCommonConsoleSettings(UDASCLI)

target_link_libraries(UDASCLI PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
    QEPM::QCommonConsole
    QEPM::EPMLib
    QEPM::UDASDev
)
