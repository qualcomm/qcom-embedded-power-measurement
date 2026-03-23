# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/version.cmake)

set(CMAKE_CXX_STANDARD 20)

set(QCOMMONCONSOLE_DEFINITIONS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    QEPM_STATIC
)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO")
endif()

if(UNIX)
    list(APPEND QCOMMONCONSOLE_DEFINITIONS __X86_64__)
    set(QCOMMONCONSOLE_CXX_FLAGS
        -Werror
        -Wno-unused-result
        -Wno-write-strings
        -Wno-comment
        -Wno-unused-function
        -Wno-reorder
        -Wno-unknown-pragmas
        -Wno-conversion-null
        -Wno-unused-parameter
        -Wno-overloaded-virtual
        -Wno-unused-variable
        -Wno-date-time
        -Wno-pragmas
        -fPIC
    )
    set(QCOMMONCONSOLE_LINK_FLAGS
        -Wl,--rpath=$ORIGIN
        -Wl,--rpath=$ORIGIN/lib
        -Wl,--rpath=$ORIGIN/../lib
        -fPIC
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND QCOMMONCONSOLE_DEFINITIONS _DEBUG DEBUG)
    if(WIN32)
        set(CONFIGURATION "x64/Debug")
    else()
        set(CONFIGURATION "Linux/Debug")
    endif()
else()
    list(APPEND QCOMMONCONSOLE_DEFINITIONS _NDEBUG NDEBUG)
    if(WIN32)
        set(CONFIGURATION "x64/Release")
    else()
        set(CONFIGURATION "Linux/Release")
    endif()
endif()

set(BUILDROOT ${CMAKE_SOURCE_DIR}/__Builds/${CONFIGURATION})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILDROOT}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILDROOT}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILDROOT}/lib)

if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(NOT WINDEPLOYQT_EXECUTABLE)
        set(WINDEPLOYQT_EXECUTABLE windeployqt)
    endif()
endif()

macro(qepm_set_library_name BASE_NAME)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(LIBRARY_NAME "${BASE_NAME}d")
    else()
        set(LIBRARY_NAME "${BASE_NAME}")
    endif()
endmacro()

function(_qepm_version_to_csv version_str out_var)
    string(REPLACE "." "," _csv "${version_str}")
    set(${out_var} "${_csv},0" PARENT_SCOPE)
endfunction()

function(qepm_add_version_info target_name description version)
    if(NOT WIN32)
        return()
    endif()
    cmake_parse_arguments(_VI "" "ICON" "" ${ARGN})
    get_target_property(_ttype ${target_name} TYPE)
    if(_ttype STREQUAL "SHARED_LIBRARY")
        set(VI_ORIGINAL_FILENAME "${target_name}.dll")
        set(VI_FILE_TYPE "0x2L")
    else()
        set(VI_ORIGINAL_FILENAME "${target_name}.exe")
        set(VI_FILE_TYPE "0x1L")
    endif()
    _qepm_version_to_csv("${version}"      _VI_FILE_VERSION_CSV)
    _qepm_version_to_csv("${QEPM_VERSION}" _VI_PRODUCT_VERSION_CSV)
    set(VI_FILE_DESCRIPTION    "${description}")
    set(VI_FILE_VERSION_STR    "${version}")
    set(VI_PRODUCT_VERSION_STR "${QEPM_VERSION}")
    set(VI_INTERNAL_NAME       "${target_name}")
    set(VI_FILE_VERSION_CSV    "${_VI_FILE_VERSION_CSV}")
    set(VI_PRODUCT_VERSION_CSV "${_VI_PRODUCT_VERSION_CSV}")
    if(_VI_ICON)
        set(VI_ICON_LINE "IDI_ICON1 ICON \"${_VI_ICON}\"")
    else()
        set(VI_ICON_LINE "")
    endif()
    set(_rc_out "${CMAKE_CURRENT_BINARY_DIR}/${target_name}_version_info.rc")
    configure_file(
        "${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/version_info.rc.in"
        "${_rc_out}"
        @ONLY
    )
    target_sources(${target_name} PRIVATE "${_rc_out}")
endfunction()

function(qepm_deploy_qt target_name)
    if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --$<IF:$<CONFIG:Debug>,debug,release>
                    --no-translations
                    "$<TARGET_FILE:${target_name}>"
            VERBATIM
        )
    endif()
endfunction()

function(QCommonConsoleSettings target_name)
    target_compile_definitions(${target_name} PRIVATE ${QCOMMONCONSOLE_DEFINITIONS})
    if(UNIX AND QCOMMONCONSOLE_CXX_FLAGS)
        target_compile_options(${target_name} PRIVATE ${QCOMMONCONSOLE_CXX_FLAGS})
    endif()
    if(UNIX AND QCOMMONCONSOLE_LINK_FLAGS)
        target_link_options(${target_name} PRIVATE ${QCOMMONCONSOLE_LINK_FLAGS})
    endif()
    target_include_directories(${target_name} PRIVATE ${BUILDROOT}/ui/QCommon)
endfunction()
