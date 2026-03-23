# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../../libraries/qcommon-console/Common.cmake)

qt_add_resources(EPMSCOPE_RESOURCES
    resources/QEPMScope.qrc
)

qt_add_executable(EPMScope
    DevicePropertiesDialog.cpp DevicePropertiesDialog.h DevicePropertiesDialog.ui
    EPMDeviceModel.cpp EPMDeviceModel.h
    EPMScopeApplication.cpp EPMScopeApplication.h
    EPMScopeDefines.h
    EPMScopePreferences.cpp EPMScopePreferences.h
    EPMScopeWindow.cpp EPMScopeWindow.h EPMScopeWindow.ui
    PreferencesDialog.cpp PreferencesDialog.h PreferencesDialog.ui
    main.cpp

    ${EPMSCOPE_RESOURCES}
)

set_target_properties(EPMScope PROPERTIES
    VERSION ${EPM_SCOPE_VERSION}
)

qepm_add_version_info(EPMScope
    "Qualcomm EPM Scope"
    "${EPM_SCOPE_VERSION}"
    ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/QEPMScope.ico"
)

if(WIN32)
    target_link_libraries(EPMScope PRIVATE Qt6::AxContainer)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(EPMScope PRIVATE dl usb)
endif()

QCommonConsoleSettings(EPMScope)

target_link_libraries(EPMScope PRIVATE
    Qt6::Concurrent
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

qepm_deploy_qt(EPMScope)
