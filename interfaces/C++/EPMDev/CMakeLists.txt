# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/qcommon-console/Common.cmake)

qepm_set_library_name(EPMDev)

qt_add_library(${LIBRARY_NAME} SHARED
    DevEPMCore.cpp DevEPMCore.h
    EPMDev.cpp EPMDev.h
)

QCommonConsoleSettings(${LIBRARY_NAME})

if(UNIX)
    # QCommonConsoleSettings already adds $ORIGIN search paths. Stop CMake from
    # also writing the absolute build-machine paths (build tree and Qt install
    # folder) into the RUNPATH, since the packaged library carries them to every
    # installed system.
    set_target_properties(${LIBRARY_NAME} PROPERTIES SKIP_BUILD_RPATH ON)
endif()

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
    "QEPM Device Interface Library"
    "${QEPM_VERSION}"
)

add_library(QEPM::EPMDev ALIAS ${LIBRARY_NAME})
