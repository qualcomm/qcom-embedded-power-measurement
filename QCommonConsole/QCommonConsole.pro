# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# Author: msimpson, biswroy

include(Common.pri)

QT += xml serialport concurrent network

CONFIG(debug, debug|release) {
	TARGET = QCommonConsoled
} else {
	TARGET = QCommonConsole
}

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$STATIC_LIBPATH
DEFINES += QCOMMONCONSOLE_LIBRARY

INCLUDEPATH += $$SERIAL
INCLUDEPATH += $$LIBUSBINC

DEPENDPATH += $$SERIAL
DEPENDPATH += $$LIBUSBINC

win32 {
	DEFINES += FTD2XX_STATIC
}

linux {
	INCLUDEPATH += $$LINUXFTDI
}

HEADERS += \
	AlpacaDefines.h \
	AlpacaDevice.h \
	AlpacaScript.h \
	AlpacaSettings.h \
	AlpacaSharedLibrary.h \
	AppCore.h \
	BasicColor.h \
	Button.h \
	ByteCountFormatter.h \
	CSV.h \
	CharBit.h \
	CommandGroup.h \
	CommandLineParser.h \
	ConsoleApplicationEnhancements.h \
	ConsoleInterface.h \
	CountingStream.h \
	DataPoints.h \
	DateCheckFailEvent.h \
	DebugBoardType.h \
	DriveThread.h \
	EncryptedString.h \
	ErrorParameter.h \
	FTDIChipset.h \
	FTDIDevice.h \
	FTDIPinSet.h \
	FTDIPlatformConfiguration.h \
	FrameCoder.h \
	FramePackage.h \
	Hexify.h \
	KeyState.h \
	LibUsbEngine.h \
	LibUsbInitializer.h \
	LicenseFailEvent.h \
	LicenseManager.h \
	LogInterface.h \
	MathFunctions.h \
	Notification.h \
	PIC32CXDevice.h \
	PIC32CXPlatformConfiguration.h \
	PIC32CXSerialTableModel.h \
	PSOCDevice.h \
	PSOCPlatformConfiguration.h \
	PSOCSerialTableModel.h \
	PerformanceMonitor.h \
	PinID.h \
	PlatformConfiguration.h \
	PlatformConfigurationException.h \
	PlatformID.h \
	PluginInterface.h \
	PreferencesBase.h \
	PreferencesBase.h \
	ProcessUtilities.h \
	ProtocolInterface.h \
	QCommonConsole.h \
	QCommonConsoleGlobal.h \
	QualcommChartColor.h \
	QualcommColors.h \
	QuitAppEvent.h \
	Range.h \
	RangedContainer.h \
	ReceiveInterface.h \
	RecentFiles.h \
	SMTPClient.h \
	ScriptVariable.h \
	SendInterface.h \
	SerialPort.h \
	SerialPortInfo.h \
	SerialPreferences.h \
	SerialTableModel.h \
	Statistics.h \
	StringProof.h \
	StringUtilities.h \
	SubSystemInterface.h \
	SystemInformation.h \
	SystemUsername.h \
	TACCommand.h \
	TACCommandHashes.h \
	TACCommands.h \
	TACException.h \
	TACPreferences.h \
	Tabs.h \
	TelematicsManager.h \
	ThreadedLog.h \
	Throttle.h \
	TickCount.h \
	USBDescriptors.h \
	mymemcpy.h \
	private/TACDriveThread.h \
	private/TACLiteCoder.h \
	private/TACLiteCommand.h \
	private/TACLiteDriveThread.h \
	private/TACLiteProtocol.h \
	private/TACPIC32CXCoder.h \
	private/TACPIC32CXCommand.h \
	private/TACPIC32CXDriveThread.h \
	private/TACPIC32CXProtocol.h \
	private/TACPsocCoder.h \
	private/TACPsocCommand.h \
	private/TACPsocDriveThread.h \
	private/TACPsocProtocol.h \
	version.h

SOURCES += \
	AlpacaDefines.cpp \
	AlpacaDevice.cpp \
	AlpacaScript.cpp \
	AlpacaSettings.cpp \
	AlpacaSharedLibrary.cpp \
	AppCore.cpp \
	BasicColor.cpp \
	Button.cpp \
	CSV.cpp \
	CharBit.cpp \
	CommandLineParser.cpp \
	ConsoleApplicationEnhancements.cpp \
	CountingStream.cpp \
	DebugBoardType.cpp \
	DriveThread.cpp \
	EncryptedString.cpp \
	FTDIChipset.cpp \
	FTDIDevice.cpp \
	FTDIPlatformConfiguration.cpp \
	FrameCoder.cpp \
	LibUsbEngine.cpp \
	LibUsbInitializer.cpp \
	LicenseFailEvent.cpp \
	LicenseManager.cpp \
	PIC32CXDevice.cpp \
	PIC32CXPlatformConfiguration.cpp \
	PIC32CXSerialTableModel.cpp \
	PSOCDevice.cpp \
	PSOCPlatformConfiguration.cpp \
	PSOCSerialTableModel.cpp \
	PlatformConfiguration.cpp \
	PlatformConfigurationException.cpp \
	PlatformID.cpp \
	PreferencesBase.cpp \
	ProcessUtilities.cpp \
	ProtocolInterface.cpp \
	QCommonConsole.cpp \
	QualcommChartColor.cpp \
	QuitAppEvent.cpp \
	RecentFiles.cpp \
	SMTPClient.cpp \
	ScriptVariable.cpp \
	SerialPort.cpp \
	SerialPortInfo.cpp \
	SerialTableModel.cpp \
	Statistics.cpp \
	StringProof.cpp \
	StringUtilities.cpp \
	SystemInformationWin.cpp \
	SystemUsername.cpp \
	TACCommand.cpp \
	TACCommands.cpp \
	TACPreferences.cpp \
	Tabs.cpp \
	TelematicsManager.cpp \
	ThreadedLog.cpp \
	TickCount.cpp \
	USBDescriptors.cpp \
	mymemcpy.cpp \
	private/TACDriveThread.cpp \
	private/TACLiteCoder.cpp \
	private/TACLiteCommand.cpp \
	private/TACLiteDriveThread.cpp \
	private/TACLiteProtocol.cpp \
	private/TACPIC32CXCoder.cpp \
	private/TACPIC32CXCommand.cpp \
	private/TACPIC32CXDriveThread.cpp \
	private/TACPIC32CXProtocol.cpp \
	private/TACPsocCoder.cpp \
	private/TACPsocCommand.cpp \
	private/TACPsocDriveThread.cpp \
	private/TACPsocProtocol.cpp


RESOURCES += \
	resources/QCommonConsole.qrc
