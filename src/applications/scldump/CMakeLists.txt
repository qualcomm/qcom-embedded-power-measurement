# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_executable(SCLDump
    main.cpp
)

qepm_add_version_info(SCLDump
    "SCL Dump Utility"
    "${QEPM_VERSION}"
)

QCommonConsoleSettings(SCLDump)

target_link_libraries(SCLDump PRIVATE
    Qt6::Core
    Qt6::SerialPort
    QEPM::QCommonConsole
    QEPM::EPMLib
)
