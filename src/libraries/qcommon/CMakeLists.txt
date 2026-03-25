# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

include(${CMAKE_SOURCE_DIR}/src/libraries/qcommon-console/Common.cmake)

qepm_set_library_name(QCommon)

qt_add_resources(QCOMMON_RESOURCES
    resources/QCResource.qrc
)

qt_add_library(${LIBRARY_NAME} STATIC
    AboutDialog.cpp AboutDialog.h AboutDialog.ui
    QEPMApplication.cpp QEPMApplication.h
    ApplicationEnhancements.cpp ApplicationEnhancements.h
    ColorSwatch.cpp ColorSwatch.h
    CustomComboBox.h
    HappinessDialog.cpp HappinessDialog.h HappinessDialog.ui
    IntFilteredEditor.cpp IntFilteredEditor.h
    PainterEffects.cpp PainterEffects.h
    ProgressDialog.cpp ProgressDialog.h ProgressDialog.ui
    QCommon.cpp QCommon.h
    QCommonGlobal.h
    StyledLabel.cpp StyledLabel.h
    TableCheckBox.cpp TableCheckBox.h
    TableComboBox.cpp TableComboBox.h

    ${QCOMMON_RESOURCES}
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
    VERSION ${QEPM_VERSION}
)

QCommonConsoleSettings(${LIBRARY_NAME})

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ../qcommon-console
)

target_compile_definitions(${LIBRARY_NAME} PUBLIC
    QEPM_STATIC
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_UP_TO=0x060600
    $<$<CONFIG:Debug>:DEBUG;_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG;_NDEBUG>
)

target_link_libraries(${LIBRARY_NAME} PUBLIC
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
    Qt6::Widgets
    Qt6::Xml
)

add_library(QEPM::QCommon ALIAS ${LIBRARY_NAME})

install(TARGETS ${LIBRARY_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
