# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

# EPMDev Python Library
# 	Written by Michael Simpson (msimpson@qti.qualcomm.com) and edited by Biswajit Roy (biswroy@qti.qualcomm.com)
# 	Special thanks to Brian Kahne (bkahne@qti.qualcomm.com) for giving me a Python education
# 	Don't modify this file.  If you do, you run the risk of missing out on bug fixes and product updates

import logging
import os
from ctypes import *
from pathlib import Path
from sys import exit, platform

# Configure logging
logger = logging.getLogger(__name__)
log_fmt: str = logging.BASIC_FORMAT
logging.basicConfig(format=log_fmt, level=logging.INFO)


class _SetupEPM:
    """
    Internal class containing methods to configure shared library paths to EPM.
    Attributes:
            `epmLibraryPath`: The path to the EPM shared library
            `debugPythonPath`: The path to the debug python dll
            `isDebugExecution`: Whether the current execution for debugging
    """

    __epmLibraryPath: Path = None
    __debugPythonPath: Path = Path("C:/github/AlpacaRepos/__Builds/x64/Debug/bin/PythonDebug.dll")
    __isDebugExecution: bool = False

    def __init__(self) -> None:
        self.setupSharedLibraryPath()
        logger.debug(f"Configured the EPM library path to be: {self.__epmLibraryPath.as_posix()}")

        if self.__isDebugExecution:
            logger.info(f"Process ID: {os.getpid()}")

    def PythonIsDebugging(self) -> bool:
        """
        Debug function to update the dll path to a debug dll.
        """
        if self.__debugPythonPath.exists():
            try:
                __pythonDebugLib = CDLL(self.__debugPythonPath.as_posix())
                __isPythonDebuggingFunc = __pythonDebugLib.IsPythonDebugging
                self.__isDebugExecution = __isPythonDebuggingFunc()
                return True

            except Exception as error:
                logger.error(f"Could not load the 'IsPythonDebugging()' from PythonDebug shared library. {error}")
                exit(1)
        return False

    def setupSharedLibraryPath(self):
        """
        Configures the shared library path for EPM based on OS and Alpaca installation
        """
        debugLinuxLibraryPath: Path = Path("/local/mnt/workspace/github/AlpacaRepos/__Builds/Linux/Debug/lib/libEPMDev.so")
        debugWindowsLibraryPath: Path = Path("C:/github/AlpacaRepos/__Builds/x64/Debug/bin/EPMDevd.dll")
        internalLinuxLibraryPath: Path = Path("/opt/qcom/Alpaca/lib/libEPMDev.so")
        internalWindowsLibraryPath: Path = Path("C:/Program Files (x86)/Qualcomm/Alpaca/EPMDev.dll")
        externalLinuxLibraryPath: Path = Path("/opt/qcom/QEPM/lib/libEPMDev.so")
        externalWindowsLibraryPath: Path = Path("C:/Program Files (x86)/Qualcomm/QEPM/EPMDev.dll")

        pythonIsDebugging = self.PythonIsDebugging()
        currentPlatform = platform

        if currentPlatform.startswith("linux") and pythonIsDebugging:
            self.__epmLibraryPath = debugLinuxLibraryPath

        elif platform.startswith("win32") and pythonIsDebugging:
            self.__epmLibraryPath = debugWindowsLibraryPath

        elif currentPlatform.startswith("linux") and not pythonIsDebugging:
            self.__epmLibraryPath = internalLinuxLibraryPath

            # if the internal path does not resolve, it must be an external release
            if not self.__epmLibraryPath.exists():
                self.__epmLibraryPath = externalLinuxLibraryPath

        elif currentPlatform.startswith("win32") and not pythonIsDebugging:
            self.__epmLibraryPath = internalWindowsLibraryPath

            # if the internal path does not resolve, it must be an external release
            if not self.__epmLibraryPath.exists():
                self.__epmLibraryPath = externalWindowsLibraryPath

    def getEPMLibraryPath(self):
        """
        Returns the appropriate EPM library path.
        """
        return self.__epmLibraryPath.as_posix()


# the path to the EPMDev shared library
epmLibraryPath = _SetupEPM().getEPMLibraryPath()

# EPM HANDLE
BAD_EPM_HANDLE = 0

# EPM RESULTS
NO_EPM_ERROR = 0
EPM_BUFFER_TOO_SMALL = 1
EPM_BAD_EPM_HANDLE = 2
EPM_BAD_EPM_INDEX = 3
EPM_BAD_PATH = 4
EPM_ACQUIRE_FAILED = 5
EPM_BAD_CHANNEL_HASH = 6
EPM_INIT_FAILED = 7


def GetErrorString(errorCode: int) -> str:
    errorDict: dict[int,str] = {
        0: "No EPM error",
        1: "EPM buffer is too small",
        2: "Bad EPM handle",
        3: "Bad EPM index",
        4: "Bad EPM path",
        5: "EPM acquire failed",
        6: "Bad EPM channel hash",
        7: "EPM initialization failed"
    }
    return errorDict.get(errorCode, f"Error string cannot be determine. Error code: {errorCode}")

if epmLibraryPath is not None:
    try:
        epmLibrary = CDLL(epmLibraryPath)

        if not epmLibrary:
            raise Exception("The EPMDev library was not found!")

    except Exception as error:
        logger.error(
            f"""Could not locate the EPMDev.dll. Please update Alpaca.
        Error: {error}"""
        )
        logger.info("EPM library path:", epmLibraryPath)
        exit()

    try:
        initializeFunc = epmLibrary.InitializeEPMDev
        initResult = initializeFunc()
        if initResult == EPM_INIT_FAILED:
           logger.error("EPMDev failed to initialize. Please report this issue to support.")
           exit(1)

        __alpacaVersionFunc = epmLibrary.GetAlpacaVersion
        __epmVersionFunc = epmLibrary.GetEPMVersion
        __getLastErrorFunc = epmLibrary.GetLastEPMError
        __getDeviceCountFunc = epmLibrary.GetDeviceCount
        __getDeviceFunc = epmLibrary.GetPortData
        __getLoggingFunc = epmLibrary.GetLoggingState
        __setLoggingFunc = epmLibrary.SetLoggingState
        __writeToLogFunc = epmLibrary.WriteToAppLog

    except Exception as error:
        logger.error(f"""Could not load methods from EPMDev. Please update Alpaca
        and run the setup.bat at C:/ProgramData/Qualcomm/Alpaca/Python/XPlatform.
        Error: {error}""")
        exit()
else:
    print("EPMDev path was not set")
    exit()


class EPMDevice:
    """
    EPMDevice is the base class for accessing methods specific to an EPM device.
    Some methods in EPMDevice can be accessed only after opening the device using EPmDevice::Open().
    """

    __portName: str  # the port name of the connected device
    __description: str  # the description of the device as string
    __serialNumber: str  # the serial number of the device as string

    def __init__(self, portName: str, serialNumber: str, description: str):
        self.__portName = portName
        self.__serialNumber = serialNumber
        self.__description = description
        self.__epmHandle = BAD_EPM_HANDLE

    def PortName(self) -> str:
        """
        Returns the device's name of the port as string.
        """
        return self.__portName

    def Description(self) -> str:
        """
        Returns the description of the device as string.
        """
        return self.__description

    def SerialNumber(self) -> str:
        """
        Returns the serial number of the device as string.
        """
        return self.__serialNumber

    def Valid(self) -> bool:
        """
        Returns whether the device is valid as bool.
        """
        try:
            return self.__epmHandle != BAD_EPM_HANDLE

        except AttributeError as error:
            logger.error(
                f"Please check if the device is not open. Use Open() before calling Valid(). {error}"
            )
            raise RuntimeError(f"Please check if the device is not open.")

    def Open(self) -> bool:
        """
        Opens the EPM device for various EPM operations.
        Returns whether the device was opened successfully as bool.
        """
        if self.__portName:
            portName = self.__portName.encode("utf-8")
            self.__epmHandle = self.__openByNameFunc(portName)

        if self.__epmHandle == BAD_EPM_HANDLE:
            if self.__serialNumber:
                serialNumber = self.__serialNumber.encode("utf-8")
                self.__epmHandle = self.__openByNameFunc(serialNumber)

        if self.__epmHandle != BAD_EPM_HANDLE:
            return True

        return False

    def Close(self) -> None:
        """
        Closes an opened EPM device.
        Does not return a value.
        """
        self.__closeFunc(self.__epmHandle)

    def GetUUID(self) -> str:
        """
        Returns the UUID of the EPM device as string.
        """
        buf = create_string_buffer(1024)
        self.__uuidFunc(self.__epmHandle, buf, sizeof(buf))
        hardware = buf.value.decode("cp1252")
        return hardware

    def GetPlatformPath(self) -> str:
        """
        Returns the path of the current platform as string.
        """
        buf = create_string_buffer(1024)
        self.__getPlatformPathFunc(self.__epmHandle, buf, sizeof(buf))
        platformPath = buf.value.decode("cp1252")
        return platformPath

    def SetPlatformPath(self, newPath: str) -> bool:
        """
        Sets the platform path of a platform.
        Returns true if the operation was successful.
        :param newPath: The platform path as string.
        """
        newPathStr = newPath.encode("utf-8")
        result = self.__setPlatformPathFunc(self.__epmHandle, newPathStr)
        return result

    def GetPlatformCount(self) -> int:
        """
        Returns the number of available platforms in the platform path as integer.
        """
        platformCount = c_int(0)
        rc = self.__getPlatformCountFunc(self.__epmHandle, byref(platformCount))
        if rc != NO_EPM_ERROR:
            raise RuntimeError(f"Error: GetPlatformCount returned: {GetErrorString(rc)}. Error code: {rc}.")
        return platformCount.value

    def GetPlatform(self, platformIndex: int) -> str:
        """
        Returns the name of platform at the `platformIndex` as string.
        :param platformIndex: The index of the platform as integer
        """
        buf = create_string_buffer(1024)
        self.__getPlatformFunc(self.__epmHandle, platformIndex, buf, sizeof(buf))
        platform = buf.value.decode("cp1252")
        return platform

    def SetPlatform(self, platform) -> bool:
        """
        Sets the current platform to `platform`.
        :param platform: The name of the new platform as string.
        :returns: True if the set operation was successful.
        """
        newPlatform = platform.encode("utf-8")
        result = self.__setPlatformFunc(self.__epmHandle, newPlatform)
        return result

    def GetChannelCount(self) -> int:
        """
        Returns the count of the channels in the platform.
        If the channelCount is 0, `RuntimeError` is thrown.
        """
        channelCount = c_int(0)
        rc = self.__getChannelCountFunc(self.__epmHandle, byref(channelCount))
        if rc != NO_EPM_ERROR:
            raise RuntimeError(f"Error: GetChannelCount returned: {GetErrorString(rc)}. Error code: {rc}.")
        return channelCount.value

    def GetChannelName(self, channelIndex) -> str:
        """
        Returns the name of the channel based on the `channelIndex`.
        If the operation is unsuccessful, `RuntimeError` is thrown.
        :param channelIndex: The index of the channel.
        """
        buf = create_string_buffer(1024)
        channelHash = c_ulonglong(0)
        result = self.__getChannelFunc(
            self.__epmHandle, channelIndex, buf, sizeof(buf), byref(channelHash)
        )
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: GetChannelName returned: {GetErrorString(result)}. Error code: {result}.")
        channelName = buf.value.decode("cp1252")
        return channelName

    def GetChannelHash(self, channelIndex) -> int:
        """
        Returns the channel hash for the channel at `channelIndex`.
        If the operation was uncessful, a `RuntimeError` is thrown.
        :param channelIndex: The index of the channel as integer.
        :returns: The channel hash as unsigned long long data-type.
        """
        buf = create_string_buffer(1024)
        channelHash = c_ulonglong(0)
        result = self.__getChannelFunc(self.__epmHandle, channelIndex, buf, sizeof(buf), byref(channelHash))
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: GetChannelHash returned: {GetErrorString(result)}. Error code: {result}.")
        return channelHash.value

    def GetCurrentChannelState(self, channelHash) -> bool:
        """
        Returns the state of the current channel based on `channelHash`.
        :param channelHash: The channel hash of the current channel as unsigned long long.
        :returns: The state of the current channel as bool.
        """
        active = c_bool(False)
        saveRaw = c_bool(False)
        result = self.__getCurrentChannelStateFunc(self.__epmHandle, channelHash, byref(active), byref(saveRaw))

        if result != EPM_BAD_CHANNEL_HASH:
            if result != NO_EPM_ERROR:
                raise RuntimeError(f"Error: GetCurrentChannelState returned: {GetErrorString(result)}. Error code: {result}.")

        return active.value

    def SetCurrentChannelState(self, channelHash, active, saveRaw=False) -> None:
        """
        Sets the channel state of the current channel.
        :param channelHash: The channel hash of the current channel as unsigned long long.
        :param active: Whether the channel state is active as bool.
        :param saveRaw: Whether to save the data in raw format.
        """
        result = self.__setCurrentChannelStateFunc(self.__epmHandle, channelHash, active, saveRaw)
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: SetCurrentChannelState returned: {GetErrorString(result)}. Error code: {result}.")

    def GetVoltageChannelState(self, channelHash) -> bool:
        """
        Returns the state of the voltage channel based on `channelHash`.
        :param channelHash: The channel hash of the voltage channel as unsigned long long.
        :returns: The state of the voltage channel as bool.
        """
        active = c_bool(False)
        saveRaw = c_bool(False)
        result = self.__getVoltageChannelStateFunc(self.__epmHandle, channelHash, byref(active), byref(saveRaw))

        if result != EPM_BAD_CHANNEL_HASH:
            if result != NO_EPM_ERROR:
                raise RuntimeError(f"Error: GetVoltageChannelState returned: {GetErrorString(result)}. Error code: {result}.")

        return active.value

    def SetVoltageChannelState(self, channelHash, active, saveRaw=False) -> None:
        """
        Sets the channel state of the voltage channel.
        :param channelHash: The channel hash of the voltage channel as unsigned long long.
        :param active: Whether the channel state is active as bool.
        :param saveRaw: Whether to save the data in raw format.
        """
        result = self.__setVoltageChannelStateFunc(self.__epmHandle, channelHash, active, saveRaw)
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: SetVoltageChannelState returned: {GetErrorString(result)}. Error code: {result}.")

    def SetRuntimeConfig(self, runtimeFileConfigPath) -> None:
        """
        Sets the file path for the runtime config file (.rcnf)
        :param runtimeFileConfigPath: The path to the `.rcnf` file as string.
        """
        newPathStr = runtimeFileConfigPath.encode("utf-8")
        result = self.__setRuntimeConfigFunc(self.__epmHandle, newPathStr)
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: SetRuntimeConfig returned: {GetErrorString(result)}. Error code: {result}.")

    def Acquire(self, seconds: float) -> None:
        """
        Starts measuring channel data on the device for the given `seconds`.
        :param seconds: The time in seconds to measure the data.
        """
        result = self.__acquireFunc(self.__epmHandle, c_double(seconds))
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: Acquire returned: {GetErrorString(result)}. Error code: {result}.")

    def StartAcquisition(self) -> None:
        """
        Starts measuring channel data based on runtime configuration. EPM will continue
        to measure data until `StopAcquisition()` is called.
        """
        result = self.__startAcquireFunc(self.__epmHandle)
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: StartAcquisition returned: {GetErrorString(result)}. Error code: {result}.")

    def StopAcquisition(self) -> None:
        """
        Stops measuring channel data based on the runtime configuration.
        """
        result = self.__stopAcquireFunc(self.__epmHandle)
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: StopAcquisition returned: {GetErrorString(result)}. Error code: {result}.")

    def GetResultsPath(self) -> None:
        """
        Returns the path to the results directory after the acquisition as string.
        If a path is not set in the program, it returns the default results directory path.
        """
        buf = create_string_buffer(1024)
        self.__getResultsPathFunc(self.__epmHandle, buf, sizeof(buf))
        resultsPath = buf.value.decode("cp1252")
        return resultsPath

    def SetTemporaryResultsFolder(self, newResultsFolder) -> None:
        """
        Sets the path to the results directory using the path in the `newResultsFolder`.
        :param newResultsFolder: The string containing the new results directory path.
        """
        newPathStr = newResultsFolder.encode("utf-8")
        result = self.__setTemporaryResultsFolderFunc(self.__epmHandle, newPathStr)
        if result != NO_EPM_ERROR:
            raise RuntimeError(f"Error: SetTemporaryResultsFolder returned: {GetErrorString(result)}. Error code: {result}.")

    def SetupClassEntries(self) -> None:
        """
        This function maps the methods of the EPMDevice with the shared library methods.
        In case, a method does not exist in the shared library, the program will exit with an error message.
        """
        try:
            self.__openByNameFunc = epmLibrary.OpenHandleByDescription
            self.__openByNameFunc.argtypes = [c_char_p]

            self.__closeFunc = epmLibrary.CloseEPMHandle
            self.__getPlatformPathFunc = epmLibrary.GetPlatformPath
            self.__setPlatformPathFunc = epmLibrary.SetPlatformPath
            self.__getPlatformCountFunc = epmLibrary.GetPlatformCount
            self.__getPlatformFunc = epmLibrary.GetPlatform
            self.__setPlatformFunc = epmLibrary.SetPlatform

            self.__getChannelCountFunc = epmLibrary.GetChannelCount
            self.__getChannelFunc = epmLibrary.GetChannel

            self.__setRuntimeConfigFunc = epmLibrary.SetRuntimeConfig
            self.__getCurrentChannelStateFunc = epmLibrary.GetCurrentChannelState
            self.__setCurrentChannelStateFunc = epmLibrary.SetCurrentChannelState
            self.__getVoltageChannelStateFunc = epmLibrary.GetVoltageChannelState
            self.__setVoltageChannelStateFunc = epmLibrary.SetVoltageChannelState

            self.__uuidFunc = epmLibrary.GetUUID
            self.__acquireFunc = epmLibrary.Acquire
            self.__startAcquireFunc = epmLibrary.StartAcquisition
            self.__stopAcquireFunc = epmLibrary.StopAcquisition
            self.__getResultsPathFunc = epmLibrary.GetResultsPath
            self.__setTemporaryResultsFolderFunc = epmLibrary.SetTemporaryResultsFolder
        except Exception as error:
            logger.error(f"""Could not load methods from EPMDev. Please update Alpaca
            and run the setup.bat at C:/ProgramData/Qualcomm/Alpaca/Python/XPlatform.
            Error: {error}""")
            exit(1)


def AlpacaVersion() -> str:
    """
    Returns the version of Alpaca as string.
    """
    buf = create_string_buffer(1024)
    __alpacaVersionFunc(buf, sizeof(buf))
    return buf.value.decode("cp1252")


def EPMVersion() -> str:
    """
    Returns the version of EPMDev library as string.
    """
    buf = create_string_buffer(1024)
    __epmVersionFunc(buf, sizeof(buf))
    return buf.value.decode("cp1252")


def GetLastError() -> str:
    """
    Returns the last error message from EPM as string.
    """
    buf = create_string_buffer(1024)
    __getLastErrorFunc(buf, sizeof(buf))
    return buf.value.decode("cp1252")


def GetLoggingState() -> bool:
    """
    Returns the state of application logging as boolean.
    """
    state = c_bool(False)
    result = __getLoggingFunc(byref(state))
    if result != NO_EPM_ERROR:
        raise RuntimeError(f"Error: GetLoggingState returned: {GetErrorString(result)}. Error code: {result}.")
    return state.value


def SetLoggingState(state) -> None:
    """
    Whether to enable application logging based on the `state` parameter.
    :param state: The boolean state to enable/disable logging.
    """
    result = __setLoggingFunc(state)
    if result != NO_EPM_ERROR:
        raise RuntimeError(f"Error: SetLoggingState returned: {GetErrorString(result)}. Error code: {result}.")


def WriteToLog(logMessage) -> None:
    """
    Writes the `logMessage` to the application log.
    :parameter logMessage: The string message to be written to the app-log.
    """
    result = __writeToLogFunc(logMessage)
    if result != NO_EPM_ERROR:
        raise RuntimeError(f"Error: WriteToLog returned: {GetErrorString(result)}. Error code: {result}.")

def GetDeviceCount() -> int:
    """
    Returns the count of EPM devices connected to the machine.
    :returns: The count of the number of connected EPM devices.
    """
    deviceCount = c_int(0)
    rc = __getDeviceCountFunc(byref(deviceCount))
    if rc != NO_EPM_ERROR:
        raise RuntimeError(f"Error: GetChannelCount returned: {GetErrorString(rc)}. Error code: {rc}.")
    return deviceCount.value


def GetDevice(deviceIndex) -> EPMDevice:
    """
    Returns an instance of the EPMDevice based on `deviceIndex`.
    :param deviceIndex: The index of the connected EPM device.
    :returns: The EPMDevice object
    """
    buf = create_string_buffer(1024)
    __getDeviceFunc(deviceIndex, buf, sizeof(buf))
    deviceAttributes = buf.value.decode("cp1252").split(";")
    if len(deviceAttributes) >= 3:
        epmDevice = EPMDevice(deviceAttributes[0], deviceAttributes[1], deviceAttributes[2])
        epmDevice.SetupClassEntries()
    else:
        epmDevice = None

    return epmDevice


def main() -> None:
    """
    A sample program that uses the EPM APIs.
    """
    print("Alpaca Version: ", AlpacaVersion())
    print("EPM Version: ", EPMVersion())
    print("Using DLL from:", epmLibraryPath)

    deviceCount = GetDeviceCount()
    print("Device Count", deviceCount)

    if deviceCount > 0:

        index = 0
        while index < deviceCount:
            epmDevice = GetDevice(index)
            index += 1
            if epmDevice is not None:
                print(f"Found device with port name: {epmDevice.PortName()} and serial number: {epmDevice.SerialNumber()}")

        epmDevice = GetDevice(0)
        if epmDevice is not None:

            if epmDevice.Open():
                try:
                    print("UUID:", epmDevice.GetUUID())

                    print("Platform Path:", epmDevice.GetPlatformPath())

                    platformCount = epmDevice.GetPlatformCount()
                    print("Platform Count:", platformCount)
                    platformIndex = 0
                    while platformIndex < platformCount:
                        platformDesc = epmDevice.GetPlatform(platformIndex)
                        platformIndex += 1
                        platformAttributes = platformDesc.split(";")
                        print("Platform:", platformAttributes[0], " Path: ", platformAttributes[1])

                    epmDevice.SetPlatform("MTP8450")
                    epmDevice.SetRuntimeConfig("C:\\Alpaca\\Examples\\data\\SM8450_Battery.rcnf")

                    channelIndex = 0
                    channelCount = epmDevice.GetChannelCount()
                    print("Channel Count: ", channelCount)
                    print("Channels")
                    while channelIndex < channelCount:
                        hash = epmDevice.GetChannelHash(channelIndex)
                        print("   Channel: ", epmDevice.GetChannelName(channelIndex), "Current: ", epmDevice.GetCurrentChannelState(hash), "Voltage: ", epmDevice.GetVoltageChannelState(hash))
                        channelIndex = channelIndex + 1

                    epmDevice.Acquire(4)

                    epmDevice.Close()
                except RuntimeError as error:
                    print(error)

            else:
                print(f"Device with port name: {epmDevice.PortName()} can't be opened.")
        else:
            print(f"GetDevice() returned None.")


if __name__ == "__main__":
    main()
