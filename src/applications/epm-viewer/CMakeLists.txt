# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_resources(EPMVIEWER_RESOURCES
    resources/QEPMViewer.qrc
)

qt_add_executable(EPMViewer
    EPMViewerApplication.cpp EPMViewerApplication.h
    EPMViewerDefines.h
    EPMViewerWindow.cpp EPMViewerWindow.h EPMViewerWindow.ui
    PreferencesDialog.cpp PreferencesDialog.h PreferencesDialog.ui
    main.cpp

    ${EPMVIEWER_RESOURCES}
)

set_target_properties(EPMViewer PROPERTIES
    VERSION ${EPM_VIEWER_VERSION}
)

qepm_add_version_info(EPMViewer
    "Qualcomm EPM Viewer"
    "${EPM_VIEWER_VERSION}"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/QEPMViewer.ico"
)

if(WIN32)
    target_link_libraries(EPMViewer PRIVATE Qt6::AxContainer)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(EPMViewer PRIVATE dl usb)
endif()

QCommonConsoleSettings(EPMViewer)

target_link_libraries(EPMViewer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::SerialPort
    Qt6::Svg
    Qt6::Widgets
    QEPM::QCommon
    QEPM::QCommonConsole
    QEPM::EPMLib
    QEPM::PowerChart
    QEPM::UI
)

qepm_deploy_qt(EPMViewer)
