# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_executable(EPMDump
    main.cpp
)

qepm_add_version_info(EPMDump
    "EPM Dump Utility"
    "${QEPM_VERSION}"
)

if(UNIX AND NOT APPLE)
    target_link_libraries(EPMDump PRIVATE dl usb)
endif()

QCommonConsoleSettings(EPMDump)

target_link_libraries(EPMDump PRIVATE
    Qt6::Core
    Qt6::SerialPort
    QEPM::QCommonConsole
    QEPM::EPMLib
    QEPM::EPMDev
)
