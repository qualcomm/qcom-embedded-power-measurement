# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../qcommon-console/Common.cmake)

qepm_set_library_name(LibExcel)

qt_add_library(${LIBRARY_NAME} SHARED
    QTExcel.cpp
)

if(WIN32)
    target_sources(${LIBRARY_NAME} PRIVATE
        excel.cpp excel.h
    )
endif()

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
    EXCEL_LIBRARY
)

if(MSVC)
    target_compile_options(${LIBRARY_NAME} PRIVATE
        /constexpr:steps10485760
    )
endif()

target_link_libraries(${LIBRARY_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(WIN32)
    target_link_libraries(${LIBRARY_NAME} PRIVATE
        Qt6::AxContainer
    )
endif()

qepm_add_version_info(${LIBRARY_NAME}
    "Excel Integration Library"
    "${QEPM_VERSION}"
)

add_library(QEPM::LibExcel ALIAS ${LIBRARY_NAME})

install(TARGETS ${LIBRARY_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
