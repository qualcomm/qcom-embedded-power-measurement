# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_resources(EPM_RESOURCES
    resources/EPM.qrc
)

qt_add_executable(EPM
    EPMApplication.cpp EPMApplication.h
    EPMWindow.cpp EPMWindow.h EPMWindow.ui
    PreferencesDialog.cpp PreferencesDialog.h PreferencesDialog.ui
    QuitEPMEvent.cpp QuitEPMEvent.h
    EPMDeviceSetup.ui
    HelpDialog.ui
    main.cpp

    ${EPM_RESOURCES}
)

set_target_properties(EPM PROPERTIES
    VERSION ${EPM_VERSION}
)

qepm_add_version_info(EPM
    "Qualcomm Embedded Power Monitor"
    "${EPM_VERSION}"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/EPM.ico"
)

if(WIN32)
    target_link_libraries(EPM PRIVATE Qt6::AxContainer)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(EPM PRIVATE usb rt dl uuid)
endif()

QCommonConsoleSettings(EPM)

target_link_libraries(EPM PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
    QEPM::QCommon
    QEPM::QCommonConsole
    QEPM::EPMLib
    QEPM::PowerChart
    QEPM::UI
)

qepm_deploy_qt(EPM)
