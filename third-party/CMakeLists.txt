# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.24)
project(third_party VERSION 1.0 LANGUAGES C CXX)

include(FetchContent)

# ---------------------------------------------------------------------------
# Qwt 6.3.0
# ---------------------------------------------------------------------------
set(QWT_LOCAL_ARCHIVE "${CMAKE_CURRENT_SOURCE_DIR}/qwt-6.3.0.zip")

if(EXISTS "${QWT_LOCAL_ARCHIVE}")
    message(STATUS "Qwt: using local archive at ${QWT_LOCAL_ARCHIVE}")
    FetchContent_Declare(
        qwt_source
        URL "file://${QWT_LOCAL_ARCHIVE}"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
else()
    FetchContent_Declare(
        qwt_source
        URL https://sourceforge.net/projects/qwt/files/qwt/6.3.0/qwt-6.3.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        TLS_VERIFY FALSE
    )
endif()

if(NOT TARGET qwt)
    # The Qwt archive has no CMakeLists.txt, so MakeAvailable only downloads and
    # extracts it. The library target is defined here from its sources.
    message(STATUS "Fetching Qwt 6.3.0...")
    FetchContent_MakeAvailable(qwt_source)

    file(GLOB QWT_SOURCES "${qwt_source_SOURCE_DIR}/src/*.cpp")

    # Exclude files that require Qt5-only headers (qgl.h removed in Qt6)
    list(FILTER QWT_SOURCES EXCLUDE REGEX "qwt_plot_glcanvas\\.cpp")

    qt_add_library(qwt STATIC
        ${QWT_SOURCES}
    )

    target_include_directories(qwt PUBLIC
        $<BUILD_INTERFACE:${qwt_source_SOURCE_DIR}/src>
    )

    target_link_libraries(qwt PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Qt6::PrintSupport
        Qt6::Svg
        Qt6::Concurrent
    )

    set_target_properties(qwt PROPERTIES
        AUTOMOC ON
    )

    # Qwt is third-party code; its warnings (e.g. Qt 6 deprecations) are not ours to fix.
    if(MSVC)
        target_compile_options(qwt PRIVATE /W0)
    else()
        target_compile_options(qwt PRIVATE -w)
    endif()
endif()
