# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(libraries VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

add_subdirectory(qcommon-console)
add_subdirectory(qcommon)
add_subdirectory(libexcel)
add_subdirectory(epmlib)
add_subdirectory(ui-common)
add_subdirectory(powerchart)
