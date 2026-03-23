# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_executable(PSOCProgrammer
    PSOCProgrammerCmdLine.cpp PSOCProgrammerCmdLine.h
    main.cpp
)

qepm_add_version_info(PSOCProgrammer
    "PSOC Programmer Utility"
    "${QEPM_VERSION}"
)

if(UNIX AND NOT APPLE)
    target_link_libraries(PSOCProgrammer PRIVATE usb)
endif()

QCommonConsoleSettings(PSOCProgrammer)

target_link_libraries(PSOCProgrammer PRIVATE
    Qt6::Core
    QEPM::QCommonConsole
    QEPM::EPMLib
)
