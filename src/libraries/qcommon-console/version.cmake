# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/version.h" _version_lines
    REGEX "#define [A-Z_]+[ \t]+\"[0-9]+\\.[0-9]+\\.[0-9]+\"")
foreach(_line IN LISTS _version_lines)
    string(REGEX MATCH "#define ([A-Z_]+)[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+)\"" _ "${_line}")
    if(CMAKE_MATCH_1)
        set(${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")
    endif()
endforeach()
