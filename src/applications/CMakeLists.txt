# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(applications VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

add_subdirectory(bugwriter)
add_subdirectory(epm)
add_subdirectory(epm-configuration-editor)
add_subdirectory(epm-scope)
add_subdirectory(epm-viewer)
add_subdirectory(epmdump)
add_subdirectory(psoc-programmer)
add_subdirectory(scldump)
