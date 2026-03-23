# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_resources(EPMEDITOR_RESOURCES
    resources/EPMConfigurationEditor.qrc
)

qt_add_executable(EPMConfigurationEditor
    CompileFileDialog.cpp CompileFileDialog.h
    EPMConfigurationApplication.cpp EPMConfigurationApplication.h
    EPMConfigurationWindow.cpp EPMConfigurationWindow.h EPMConfigurationWindow.ui
    EPMEditorDefines.h
    main.cpp
    Preferences.cpp Preferences.h
    PreferencesDialog.cpp PreferencesDialog.h PreferencesDialog.ui

    ${EPMEDITOR_RESOURCES}
)

set_target_properties(EPMConfigurationEditor PROPERTIES
    VERSION ${EPM_EDITOR_VERSION}
)

qepm_add_version_info(EPMConfigurationEditor
    "Qualcomm EPM Configuration Editor"
    "${EPM_EDITOR_VERSION}"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/EmpConfEditor.ico"
)

if(WIN32)
    target_link_libraries(EPMConfigurationEditor PRIVATE Qt6::AxContainer)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(EPMConfigurationEditor PRIVATE usb rt dl uuid)
endif()

QCommonConsoleSettings(EPMConfigurationEditor)

target_link_libraries(EPMConfigurationEditor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
    QEPM::QCommon
    QEPM::QCommonConsole
    QEPM::EPMLib
    QEPM::UI
)

qepm_deploy_qt(EPMConfigurationEditor)
