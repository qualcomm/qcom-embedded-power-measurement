# UDASDev Python Library
# Author: Biswajit Roy <biswroy@qti.qualcomm.com>,
# Usage: The library can be used to perform post-processing on the results produced through EPMDev.
# Note: This file is part of the Alpaca / QEPM installation and cannot be distributed separately.

import logging
import os
from ctypes import *
from pathlib import Path
from sys import exit, platform

from enum import IntFlag

# Configure logging
logger = logging.getLogger(__name__)
log_fmt: str = logging.BASIC_FORMAT
logging.basicConfig(format=log_fmt, level=logging.INFO)


class _SetupUDAS:
    """
    Internal class containing methods to configure shared library paths to UDAS.
    """

    # The path to the UDAS shared library
    __udasLibraryPath: Path = None
    # The path to the debug python dll
    __debugPythonPath: Path = Path("C:/github/AlpacaRepos/__Builds/x64/Debug/bin/PythonDebug.dll")
    # Whether the current execution for debugging
    __isDebugExecution: bool = False

    def __init__(self) -> None:
        self.setupSharedLibraryPath()
        logger.debug(f"Configured the UDAS library path to be: {self.__udasLibraryPath.as_posix()}")

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
                return self.__isDebugExecution

            except Exception as error:
                logger.error(f"Could not load the 'IsPythonDebugging()' from PythonDebug shared library. {error}")
                exit(1)
        return False

    def setupSharedLibraryPath(self) -> None:
        """
        Configures the shared library path for UDAS based on OS and Alpaca installation
        """
        debugLinuxLibraryPath: Path = Path("/local/mnt/workspace/github/AlpacaRepos/__Builds/Linux/Debug/lib/libUDASDevd.so")
        debugWindowsLibraryPath: Path = Path("C:/github/AlpacaRepos/__Builds/x64/Debug/bin/UDASDevd.dll")
        internalLinuxLibraryPath: Path = Path("/opt/qcom/Alpaca/lib/libUDASDev.so")
        internalWindowsLibraryPath: Path = Path("C:/Program Files (x86)/Qualcomm/Alpaca/UDASDev.dll")
        externalLinuxLibraryPath: Path = Path("/opt/qcom/QEPM/lib/libUDASDev.so")
        externalWindowsLibraryPath: Path = Path("C:/Program Files (x86)/Qualcomm/QEPM/UDASDev.dll")

        pythonIsDebugging = self.PythonIsDebugging()
        currentPlatform = platform

        if currentPlatform.startswith("linux") and pythonIsDebugging:
            self.__udasLibraryPath = debugLinuxLibraryPath

        elif platform.startswith("win32") and pythonIsDebugging:
            self.__udasLibraryPath = debugWindowsLibraryPath

        elif currentPlatform.startswith("linux") and not pythonIsDebugging:
            self.__udasLibraryPath = internalLinuxLibraryPath

            # if the internal path does not resolve, it must be an external release
            if not self.__udasLibraryPath.exists():
                self.__udasLibraryPath = externalLinuxLibraryPath

        elif currentPlatform.startswith("win32") and not pythonIsDebugging:
            self.__udasLibraryPath = internalWindowsLibraryPath

            # if the internal path does not resolve, it must be an external release
            if not self.__udasLibraryPath.exists():
                self.__udasLibraryPath = externalWindowsLibraryPath

    def getUDASLibraryPath(self) -> str:
        """
        Returns the appropriate UDAS library path as string.
        """
        return self.__udasLibraryPath.as_posix()

# UDAS RESULTS
NO_UDAS_ERROR = 0
UDAS_BUFFER_TOO_SMALL = 1
UDAS_BAD_UDAS_HANDLE = 2
UDAS_BAD_UDAS_INDEX = 3
UDAS_BAD_WAVEFORM_TYPE = 4
UDAS_INIT_FAILED = 5


def GetErrorString(errorCode: int) -> str:
    errorDict: dict[int,str] = {
        0: "No UDAS error",
        1: "UDAS buffer is too small",
        2: "Bad UDAS handle. Does the EPM results directory path exist?",
        3: "Bad UDAS index",
        4: "Bad UDAS Waveform. Does the passed waveform type exist in WaveFormType enum?",
        5: "UDAS initialization failed"
    }
    return errorDict.get(errorCode, f"Error string cannot be determine. Error code: {errorCode}")


global __tacLib
global __alpacaVersionFunc
global __tacVersionFunc
global __getDeviceCountFunc
global __getDeviceFunc

# the path to the EPMDev shared library
udasLibraryPath = _SetupUDAS().getUDASLibraryPath()

if udasLibraryPath is not None:
    try:
        udasLibrary = CDLL(udasLibraryPath)

        if not udasLibrary:
            raise RuntimeError("UDASDev not found")
        else:
            initializeFunc = udasLibrary.InitializeUDASDev
            initResult = initializeFunc()
            if initResult == UDAS_INIT_FAILED:
                logger.error(f"Error: {GetErrorString(initResult)}")
                exit(1)

            __alpacaVersionFunc = udasLibrary.GetAlpacaVersion
            __epmVersionFunc = udasLibrary.GetEPMVersion
            __getLastUdasError = udasLibrary.GetLastUdasError

    except Exception as error:
        logger.error(f"Error with UDASDev shared Object. {error}")
        exit(1)

class WaveFormType(IntFlag):
    """
    WaveFormType Enum class defines constant values for different types of series measurements.

    Attributes:
        eWaveFormCurrent (int): Represents the current waveform type (value = 1).
        eWaveFormVoltage (int): Represents the voltage waveform type (value = 2).
        eWaveFormPower (int): Represents the power waveform type (value = 4).
    """
    # Here defined values incremented by 1 compared to WaveFormType Enum in UDASDev library for python use-case but has similar work behaviour
    eWaveFormCurrent = 1
    eWaveFormVoltage = 2
    eWaveFormPower = 4

class ChannelData:
    """
    ChannelData class containing methods to get the time series values of current, voltage, and power.
    """
    __currentChannel: dict[c_double, c_double] = {}
    __voltageChannel: dict[c_double, c_double] = {}
    __powerChannel: dict[c_double, c_double] = {}

    def __init__(self, waveFormType, dataPoints) -> None:
        """
        Initializes the ChannelData object and populates the channel data based on the series type.
        """
        self.__PopulateChannelData(waveFormType, dataPoints)

    def CurrentChannel(self)-> dict[c_double, c_double]:
        """
        Returns the current channel dictionary of series data
        """
        currentChannel = {}
        if self.__currentChannel:
            currentChannel = self.__currentChannel
        return currentChannel
    
    def VoltageChannel(self)-> dict[c_double, c_double]:
        """
        Returns the voltage channel dictionary of series data
        """
        voltageChannel = {}
        if self.__voltageChannel:
            voltageChannel = self.__voltageChannel
        return voltageChannel
    
    def PowerChannel(self)-> dict[c_double, c_double]:
        """
        Returns the power channel dictionary of series data
        """
        powerChannel = {}
        if self.__powerChannel:
            powerChannel = self.__powerChannel
        return powerChannel

    def __SetCurrentChannel(self, seriesDataList):
        """
        Set the current channel from series data list
        :param seriesDataList: Char-Array which contains series data of current channel separated by (;)
        """
        try:
            seriesData = seriesDataList.split(';')
            for timeValueStr in seriesData:
                if timeValueStr:
                    timeValueStr = timeValueStr.split(',')
                    timeOffSet = float(timeValueStr[0])
                    currentValue = float(timeValueStr[1])
                    self.__currentChannel[timeOffSet] = currentValue
                
        except Exception as error:
            raise RuntimeError(f"Failed to set current series, Error: ChannelData returned: {error}")
        
    def __SetVoltageChannel(self, seriesDataList):
        """
        Set the voltage channel from series data list
        :param seriesDataList: Char-Array which contains series data of voltage channel separated by (;)
        """
        try:
            seriesData = seriesDataList.split(';')
            for timeValueStr in seriesData:
                if timeValueStr:
                    timeValueStr = timeValueStr.split(',')
                    timeOffSet = float(timeValueStr[0])
                    voltageValue = float(timeValueStr[1])
                    self.__voltageChannel[timeOffSet] = voltageValue
                
        except Exception as error:
            raise RuntimeError(f"Failed to set voltage series, Error: ChannelData returned: {error}")
        
    def __SetPowerChannel(self, seriesDataList):
        """
        Set the power channel from series data list
        :param seriesDataList: Char-Array which contains series data of power channel separated by (;)
        """
        try:
            seriesData = seriesDataList.split(';')
            for timeValueStr in seriesData:
                if timeValueStr:
                    timeValueStr = timeValueStr.split(',')
                    timeOffSet = float(timeValueStr[0])
                    powerValue = float(timeValueStr[1])
                    self.__powerChannel[timeOffSet] = powerValue
                
        except Exception as error:
            raise RuntimeError(f"Failed to set power series, Error: ChannelData returned: {error}")

    def __PopulateChannelData(self, waveFormType, dataPoints):
        """
        Populate the channel data variable as per enum WaveFormType and buffer data 'dataPoints' 
        :param waveFormType: Enum of WaveFormType
        :param dataPoints: Char buffer data which contains time series values
        """
        try:
            dataPointsDecoded = dataPoints.value.decode('utf-8')
            seriesDataList = dataPointsDecoded.split(';;')

            index = 0
            if waveFormType & WaveFormType.eWaveFormCurrent:
                self.__SetCurrentChannel(seriesDataList[index])
                index = index + 1

            if waveFormType & WaveFormType.eWaveFormVoltage:
                self.__SetVoltageChannel(seriesDataList[index])
                index = index + 1

            if waveFormType & WaveFormType.eWaveFormPower:
                self.__SetPowerChannel(seriesDataList[index])

        except Exception as error:
            raise RuntimeError(f"Failed to populate channel data, Error: ChannelData returned: {error}")

class UDASDevice:

    __fileHandle:int = UDAS_BAD_UDAS_HANDLE

    def __init__(self) -> None:
        self.__SetupClassEntries()

    def OpenResults(self, pathToResultsFolder) -> None:
        """
        Opens the EPM results directory to load EPM data.
        :param pathToResultsFolder: The path to the captured EPM data. Usually located at: C:/Users/<username>/Documents/Alpaca/Results
        """
        resultsDir = pathToResultsFolder.encode('utf-8')
        self.__fileHandle = self.__openResults(resultsDir)
    
    def GetChannelCount(self) -> int:
        """
        Returns the total count of channels present in the EPM result. Each channel is a unique GPIO rail on the debug board.
        """
        seriesCount = c_int(0)
        retcode = self.__getChannelCount(self.__fileHandle, byref(seriesCount))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetChannelCount returned: {GetErrorString(retcode)}")
        return seriesCount.value

    def GetChannelName(self, channelIndex) -> str:
        """
        Returns the name of the captured channel as string, present at the channelIndex
        :param channelIndex:  A integer representing the index of the channel and it must be less than udasDev.GetChannelCount()
        :returns:  The name of the channel present at the `channelIndex`
        """
        channelName = create_string_buffer(1024)
        retcode = self.__getChannelName(self.__fileHandle, channelIndex, channelName, sizeof(channelName))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetChannelName returned: {GetErrorString(retcode)}")
        return channelName.value.decode('utf-8')
    
    def GetChannelIndex(self, channelName) -> str:
        """
        Returns the index of the channel for a given `channelName` if the channel exists in the EPM data.
        :param channelName:  The string containing the name of the EPM data series
        :returns:  The index of the channel corresponding to the `channelName`
        """
        channelIndex = c_int(0)
        channelNameEncoded = channelName.encode('utf-8')
        retcode = self.__getChannelIndex(self.__fileHandle, channelNameEncoded, byref(channelIndex))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetChannelIndex returned: {GetErrorString(retcode)}")
        return channelIndex.value

    def GetCurrentChannelDataCount(self, channelIndex) -> int:
        """
        Returns the total number of data-points (also known as `series`) in the current channel at `channelIndex`
        :param channelIndex:  The integer representing the index of the channel
        :returns:  A integer value representing the total number of data-points in the current channel
        """
        dataCount = c_int(0)
        retcode = self.__getCurrentChannelDataCount(self.__fileHandle, channelIndex, byref(dataCount))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetCurrentChannelDataCount returned: {GetErrorString(retcode)}")
        return dataCount.value

    def GetCurrentChannelData(self, channelIndex, dataIndex) -> float:
        """
        Returns the current value for a given `channelIndex` and `dataIndex`
        :param channelIndex:  The integer representing the index of the channel
        :param dataIndex:  The integer representing the index of the data-point (also known as `series`) in a choosen current channel
        :returns:  A floating point current value for the channel at the `dataIndex` 
        """
        result = c_double(0)
        retcode = self.__getCurrentChannelData(self.__fileHandle, channelIndex, dataIndex, byref(result))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetCurrentChannelData returned: {GetErrorString(retcode)}")
        return result.value

    def GetVoltageChannelDataCount(self, channelIndex) -> int:
        """
        Returns the total number of data-points (also known as `series`) in the voltage channel at `channelIndex`
        :param channelIndex::  The integer representing the index of the channel
        :returns:  A integer value representing the total number of data-points in the voltage channel
        """
        dataCount = c_int(0)
        retcode = self.__getVoltageChannelDataCount(self.__fileHandle, channelIndex, byref(dataCount))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetVoltageChannelDataCount returned: {GetErrorString(retcode)}")
        return dataCount.value

    def GetVoltageChannelData(self, channelIndex, dataIndex) -> float:
        """
        Returns the voltage value for a given `channelIndex` and `dataIndex`
        :param channelIndex:  The index of the channel in the EPM data
        :param dataIndex:  The integer representing the index of the data-point (also known as `series`) in a choosen voltage channel
        :returns:  A floating point voltage value for the channel at the `dataIndex` 
        """
        result = c_double(0)
        retcode = self.__getVoltageChannelData(self.__fileHandle, channelIndex, dataIndex, byref(result))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetVoltageChannelData returned: {GetErrorString(retcode)}")
        return result.value

    def GetPowerChannelDataCount(self, channelIndex) -> int:
        """
        Returns the total number of data-points (also known as `series`) in the power channel at `channelIndex`
        :param channelIndex:  The integer representing the index of the channel
        :returns::  A integer value representing the total number of data-points in the power channel
        """
        dataCount = c_int(0)
        retcode = self.__getPowerChannelDataCount(self.__fileHandle, channelIndex, byref(dataCount))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetPowerChannelDataCount returned: {GetErrorString(retcode)}")
        return dataCount.value

    def GetPowerChannelData(self, channelIndex, dataIndex) -> float:
        """
        Returns the power value for a given `channelIndex` and `dataIndex`
        :param channelIndex:  The integer representing the index of the channel
        :param dataIndex:  The integer representing the index of the data-point (also known as `series`) in a choosen power channel
        :returns:  A floating point power value for the channel at the `dataIndex`
        """
        result = c_double(0)
        retcode = self.__getPowerChannelData(self.__fileHandle, channelIndex, dataIndex, byref(result))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetPowerChannelData returned: {GetErrorString(retcode)}")
        return result.value

    def GetTimeSeries(self, channelIndex, waveFormType) -> ChannelData:
        """
        Returns data in the form of ChannelData object. The object contains time-series of current/voltage/power levels for a given series denoted by its index, `channelIndex`
        :param channelIndex:  The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower) or combination of any using `OR` operator
        :returns:  A `ChannelData` object containing current/voltage/power data-points Use CurrentChannel(), VoltageChannel(), PowerChannel() to get time-series
        """
        dataPoints = create_string_buffer(1024)
        actualSize = c_long(0)
        retcode = self.__getTimeSeries(self.__fileHandle, channelIndex, waveFormType, dataPoints, sizeof(dataPoints), byref(actualSize))

        dataPoints = create_string_buffer(actualSize.value + 1)
        retcode = self.__getTimeSeries(self.__fileHandle, channelIndex, waveFormType, dataPoints, sizeof(dataPoints), byref(actualSize))
        
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Failed to get time series data, Error: GetTimeSeries: {retcode}")
        
        channelData = ChannelData(waveFormType, dataPoints)

        return channelData
    
    def GetWaveformAvg(self, channelIndex, waveFormType, startTime = None, endTime = None) -> float:
        """
        Returns the average value of the data-points for a given `channelIndex` and `waveFormType`
        :param channelIndex:  The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower)
        :param startTime:  Optional start time
        :param endTime:  Optional end time 
        :returns:  A floating point of average value
        """
        result = c_double(0)
        if(startTime is not None and endTime is not None):
            startTime = c_double(startTime)
            endTime = c_double(endTime)
        
        retcode = self.__getWaveformAvg(self.__fileHandle, channelIndex, waveFormType, byref(result), startTime, endTime)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetWaveformAvg returned: {GetErrorString(retcode)}")
        return result.value
    
    def GetWaveformDuration(self, channelIndex, waveFormType, startTime = None, endTime = None) -> float:
        """
        Returns the duration of the measured channel at `channelIndex` and for `waveFormType`
        :param channelIndex: The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower)
        :param startTime:  Optional start time
        :param endTime:  Optional end time 
        :returns:  A floating point of duration value of the measured channel
        """
        result = c_double(0)
        if(startTime is not None and endTime is not None):
            startTime = c_double(startTime)
            endTime = c_double(endTime)

        retcode = self.__getWaveformDuration(self.__fileHandle, channelIndex, waveFormType, byref(result), startTime, endTime)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetWaveformDuration returned: {GetErrorString(retcode)}")
        return result.value
    
    def GetWaveformArea(self, channelIndex, waveFormType, startTime = None, endTime = None) -> float:
        """
        Returns the area under curve/waveform for a given `channelIndex` and `waveFormType`
        :param channelIndex: The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower)
        :param startTime:  Optional start time
        :param endTime:  Optional end time
        :returns:  A floating point of area value
        """
        result = c_double(0)
        if(startTime is not None and endTime is not None):
            startTime = c_double(startTime)
            endTime = c_double(endTime)
        
        retcode = self.__getWaveformArea(self.__fileHandle, channelIndex, waveFormType, byref(result), startTime, endTime)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetWaveformArea returned: {GetErrorString(retcode)}")
        return result.value
    
    def GetWaveformPStdDev(self, channelIndex, waveFormType, startTime = None, endTime = None) -> float:
        """
        Returns the population standard deviation for a given `channelIndex` and `waveFormType`
        :param channelIndex: The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower)
        :param startTime:  Optional start time
        :param endTime:  Optional end time
        :returns:  A floating point of population standard deviation value 
        """
        result = c_double(0)
        if(startTime is not None and endTime is not None):
            startTime = c_double(startTime)
            endTime = c_double(endTime)
        
        retcode = self.__getWaveformPStdDev(self.__fileHandle, channelIndex, waveFormType, byref(result), startTime, endTime)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetWaveformPStdDev returned: {GetErrorString(retcode)}")
        return result.value
    
    def GetWaveformMax(self, channelIndex, waveFormType, startTime = None, endTime = None) -> float:
        """
        Returns the maxima data-point for a given `channelIndex` and `waveFormType`
        :param channelIndex: The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower)
        :param startTime:  Optional start time
        :param endTime:  Optional end time
        :returns: A floating point of maxima data-point value 
        """
        result = c_double(0)
        if(startTime is not None and endTime is not None):
            startTime = c_double(startTime)
            endTime = c_double(endTime)
        
        retcode = self.__getWaveformMax(self.__fileHandle, channelIndex, waveFormType, byref(result), startTime, endTime)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetWaveformMax returned: {GetErrorString(retcode)}")
        return result.value
    
    def GetWaveformRange(self, channelIndex, waveFormType, startTime = None, endTime = None) -> float:
        """
        Returns the range between maximum and minimum data-point for a given `channelIndex` and `waveFormType`
        :param channelIndex: The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower)
        :param startTime:  Optional start time
        :param endTime:  Optional end time
        :returns: A floating point of maximum and minimum difference value
        """
        result = c_double(0)
        if(startTime is not None and endTime is not None):
            startTime = c_double(startTime)
            endTime = c_double(endTime)
        
        retcode = self.__getWaveformRange(self.__fileHandle, channelIndex, waveFormType, byref(result), startTime, endTime)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetWaveformRange returned: {GetErrorString(retcode)}")
        return result.value
    
    def GetWaveformMin(self, channelIndex, waveFormType, startTime = None, endTime = None) -> float:
        """
        Returns the minima data-point for a given `channelIndex` and `waveFormType`
        :param channelIndex: The integer representing the index of the channel
        :param waveFormType:  The values defined in the WaveFormType enum (i.e. eWaveFormCurrent, eWaveFormVoltage, eWaveFormPower)
        :param startTime:  Optional start time
        :param endTime:  Optional end time
        :returns: A floating point of minima data-point value 
        """
        result = c_double(0)
        if(startTime is not None and endTime is not None):
            startTime = c_double(startTime)
            endTime = c_double(endTime)
        
        retcode = self.__getWaveformMin(self.__fileHandle, channelIndex, waveFormType, byref(result), startTime, endTime)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetWaveformMin returned: {GetErrorString(retcode)}")
        return result.value
    
    def SetExportPath(self, exportDirectory) -> None:
        """
        Set the export path for the UDAS data in CSV format
        :param newExportFolder:  The string containing the export directory path
        """
        exportDirectoryStr = exportDirectory.encode("utf-8")
        retcode = self.__setExportPath(self.__fileHandle, exportDirectoryStr)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: SetExportPath returned: {GetErrorString(retcode)}")
        
    def GetExportPath(self) -> str:
        """
        Returns the path to the export directory if set using the SetExportPath(exportDirectory). Otherwise, returns a default path
        :returns:  A string value representing the path to the export directory
        """
        buf = create_string_buffer(1024)
        retcode = self.__getExportPath(self.__fileHandle, buf, sizeof(buf))
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: GetExportPath returned: {GetErrorString(retcode)}")
        
        resultsPath = buf.value.decode("cp1252")
        return resultsPath
    
    def ExportAsCSV(self, exportConfig = None) -> None:
        """
        It converts the EPM channel data into CSV and output at GetExportPath() location if set. Without the exportConfiguration, all channels will be exported to CSV
        :param exportConfig:  Optional configuration to allow selected channels to be exported into CSV
        """
        if exportConfig is not None:
            exportConfig = exportConfig.encode('utf-8')
        retcode = self.__exportAsCSV(self.__fileHandle, exportConfig)
        if retcode != NO_UDAS_ERROR:
            raise RuntimeError(f"Error: ExportAsCSV returned: {GetErrorString(retcode)}")

    def __SetupClassEntries(self) -> None:
        self.__openResults = udasLibrary.OpenResults
        self.__getChannelCount = udasLibrary.GetChannelCount
        self.__getChannelName = udasLibrary.GetChannelName
        self.__getChannelIndex = udasLibrary.GetChannelIndex
        self.__getCurrentChannelDataCount = udasLibrary.GetCurrentChannelDataCount
        self.__getCurrentChannelData = udasLibrary.GetCurrentChannelData
        self.__getVoltageChannelDataCount = udasLibrary.GetVoltageChannelDataCount
        self.__getVoltageChannelData = udasLibrary.GetVoltageChannelData
        self.__getPowerChannelDataCount = udasLibrary.GetPowerChannelDataCount
        self.__getPowerChannelData = udasLibrary.GetPowerChannelData
        self.__getTimeSeries = udasLibrary.GetTimeSeries
        self.__getWaveformAvg = udasLibrary.GetWaveformAvg
        self.__getWaveformDuration = udasLibrary.GetWaveformDuration
        self.__getWaveformArea = udasLibrary.GetWaveformArea
        self.__getWaveformPStdDev = udasLibrary.GetWaveformPStdDev
        self.__getWaveformMax = udasLibrary.GetWaveformMax
        self.__getWaveformRange = udasLibrary.GetWaveformRange
        self.__getWaveformMin = udasLibrary.GetWaveformMin
        self.__setExportPath = udasLibrary.SetExportPath
        self.__getExportPath = udasLibrary.GetExportPath
        self.__exportAsCSV = udasLibrary.ExportAsCSV


def AlpacaVersion() -> str:
    """
    Returns the version of Alpaca
    """
    buf = create_string_buffer(1024)
    __alpacaVersionFunc(buf, sizeof(buf))
    alpacaVersion = buf.value.decode("utf-8")
    return alpacaVersion


def EPMVersion() -> str:
    """
    Returns the version of EPM
    """
    buf = create_string_buffer(1024)
    __epmVersionFunc(buf, sizeof(buf))
    epmVersion = buf.value.decode("utf-8")
    return epmVersion

def GetLastUdasError() -> str:
    """
    Returns the error string in case of an UDAS error. Otherwise, returns empty string
    """
    buf = create_string_buffer(1024)
    __getLastUdasError(buf, sizeof(buf))
    udasError = buf.value.decode("utf-8")
    return udasError

def main() -> None:
    """
    Sample python program to demonstrate how UDAS APIs can
    be used to perform post-processing on the EPM results.
    """
    # get the alpaca version
    alpacaVersion = AlpacaVersion()
    print(f"Alpaca Version: {alpacaVersion}")

    # get the EPM version
    epmVersion = EPMVersion()
    print(f"EPM Version: {epmVersion}")

    # set the epm results path
    epmResultsDirectory = "C:\\Users\\anmojais\\Documents\\Alpaca\\Results\\CRD8380X2_2024_23_12_12_36_02\\CRD8380X2_2024_23_12_12_36_02.prn"
    udasDev = UDASDevice()
    udasDev.OpenResults(epmResultsDirectory)
    
    # get the total number of channel captured in the EPM result
    channelCount = udasDev.GetChannelCount()
    for channelIdx in range(0, channelCount):
        dataCount = udasDev.GetCurrentChannelDataCount(channelIdx)
        print(f"Count of current data-points in series '{udasDev.GetChannelName(channelIdx)}' is {dataCount}")
        for dataIdx in range(dataCount):
            print(f"        Current data-point at index {dataIdx} is {udasDev.GetCurrentChannelData(channelIdx, dataIdx)}")

        dataCount = udasDev.GetVoltageChannelDataCount(channelIdx)
        print(f"Count of voltage data-points in series '{udasDev.GetChannelName(channelIdx)}' is {dataCount}")
        for dataIdx in range(dataCount):
            print(f"        Voltage data-point at index {dataIdx} is {udasDev.GetVoltageChannelData(channelIdx, dataIdx)}")

        dataCount = udasDev.GetPowerChannelDataCount(channelIdx)
        print(f"Count of power data-points in series '{udasDev.GetChannelName(channelIdx)}' is {dataCount}")
        for dataIdx in range(dataCount):
            print(f"        Power data-point at index {dataIdx} is {udasDev.GetPowerSeriesData(channelIdx, dataIdx)}")

    # get time series data for particular channel index of channels for given WaveFormType
    for channelIdx in range(0, channelCount):
        print("\nSeries Name:", udasDev.GetSeriesName(channelIdx))
        channelData = udasDev.GetTimeSeries(channelIdx, WaveFormType.eWaveFormCurrent|WaveFormType.eWaveFormVoltage|WaveFormType.eWaveFormPower)

        for time, value in channelData.CurrentChannel().items():
            print(f"        Current data-point at time: {time} is: {value}")

        for time, value in channelData.VoltageChannel().items():
            print(f"        Voltage data-point at time: {time} is: {value}")

        for time, value in channelData.PowerChannel().items():
            print(f"        Power data-point at time: {time} is: {value}")

    for channelIdx in range(0, channelCount):
        print("\nSeries Name:", udasDev.GetSeriesName(channelIdx))
        # get statistical data for particular channelIdx of channels for given WaveFormType
        print(f"        Average current is: {udasDev.GetWaveformAvg(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Total measurement current duration is: {udasDev.GetWaveformDuration(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Area of current duration is:: {udasDev.GetWaveformArea(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Current population standard deviation is: {udasDev.GetWaveformPStdDev(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Maximum current is: {udasDev.GetWaveformMax(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Current range is: {udasDev.GetWaveformRange(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Minimum current is: {udasDev.GetWaveformMin(channelIdx, WaveFormType.eWaveFormCurrent)}")
    
    # Set a temporary export folder location of UDAS data in CSV format
    udasDev.SetExportPath("C:\\Alpaca\\Examples\\data")

    udasDev.ExportAsCSV()
    # Get export result folder location of UDAS data in CSV format
    exportPath = udasDev.GetExportPath()
    print(f"Get export result folder location: {exportPath}")


if __name__ == "__main__":
    main()
