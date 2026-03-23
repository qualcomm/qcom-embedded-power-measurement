# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_CURRENT_SOURCE_DIR}/../qcommon-console/Common.cmake)

qepm_set_library_name(UI)

qt_add_resources(UI_RESOURCES
    resources/UI.qrc
)

qt_add_library(${LIBRARY_NAME} STATIC
    ColorConversion.cpp ColorConversion.h
    CommandButton.cpp CommandButton.h
    CustomValidator.cpp CustomValidator.h
    EPMChannelTable.cpp EPMChannelTable.h EPMChannelTable.ui
    EPMDeviceDialog.cpp EPMDeviceDialog.h EPMDeviceDialog.ui
    EPMDeviceList.cpp EPMDeviceList.h EPMDeviceList.ui
    EPMDialog.cpp EPMDialog.h EPMDialog.ui
    HoverAwareQWindow.cpp HoverAwareQWindow.h HoverAwareQWindow.ui
    NotificationWidget.cpp NotificationWidget.h NotificationWidget.ui
    SeriesDataWindow.cpp SeriesDataWindow.h SeriesDataWindow.ui
    UILib.cpp UILib.h
    UIGlobalLib.h

    ${UI_RESOURCES}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${QEPM_VERSION}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../qcommon-console
    ../epmlib
    ../qcommon
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QEPM_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Widgets
)

add_library(QEPM::UI ALIAS ${LIBRARY_NAME})

install(TARGETS ${LIBRARY_NAME}
    BUNDLE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
