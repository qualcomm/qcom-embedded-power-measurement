# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_resources(BUGWRITER_RESOURCES
    Resources/BugWriter.qrc
)

qt_add_executable(BugWriter
    BugWriterApplication.cpp BugWriterApplication.h
    BugWriterPreferences.cpp BugWriterPreferences.h
    BugWriterWindow.cpp BugWriterWindow.h BugWriterWindow.ui
    BugWriterDefines.h
    IssueWriter.cpp IssueWriter.h
    main.cpp

    ${BUGWRITER_RESOURCES}
)

set_target_properties(BugWriter PROPERTIES
    VERSION ${BUG_WRITER_VERSION}
    WIN32_EXECUTABLE TRUE
)

qepm_add_version_info(BugWriter
    "Qualcomm Bug Writer"
    "${BUG_WRITER_VERSION}"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/Resources/BugWriter.ico"
)

QCommonConsoleSettings(BugWriter)

target_link_libraries(BugWriter PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::SerialPort
    Qt6::Widgets
    QEPM::QCommon
    QEPM::QCommonConsole
)

if(WIN32)
    target_link_libraries(BugWriter PRIVATE Advapi32)
endif()

qepm_deploy_qt(BugWriter)
