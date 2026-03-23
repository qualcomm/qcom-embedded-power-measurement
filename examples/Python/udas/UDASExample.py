#!/usr/bin/env python3

import UDASDev
from UDASDev import WaveFormType


def visualize(dataIdx, seriesName, currentDataList, voltageDataList, powerDataList) -> None:
    """
    Example function to visualize the EPM data on a graph. Requires matplotlib installation (pip install matplotlib)
    """
    import matplotlib.pyplot as plt
    
    plt.title(f"EPM Data for {seriesName}")

    plt.subplot(2, 2, 1) 
    plt.plot(dataIdx, currentDataList, 'r')
    plt.xlabel("Time")
    plt.ylabel("Current Value")

    plt.subplot(2, 2, 2)
    plt.plot(dataIdx, voltageDataList, 'g')
    plt.xlabel("Time")
    plt.ylabel("Voltage Value")

    plt.subplot(2, 2, 3)
    plt.plot(dataIdx, powerDataList, 'b')
    plt.xlabel("Time")
    plt.ylabel("Power Value")
    plt.show()


def main() -> None:
    # get the alpaca version
    alpacaVersion = UDASDev.AlpacaVersion()
    print(f"Alpaca Version: {alpacaVersion}")

    # get the EPM version
    epmVersion = UDASDev.EPMVersion()
    print(f"EPM Version: {epmVersion}")

    epmResultsDirectory = "C:\\Users\\anmojais\\Documents\\Alpaca\\Results\\CRD8380X2_2024_23_12_12_36_02\\CRD8380X2_2024_23_12_12_36_02.prn"
    udasDev = UDASDev.UDASDevice()
    udasDev.OpenResults(epmResultsDirectory)
    
    channelCount = udasDev.GetChannelCount()
    for channelIdx in range(0, channelCount):
        currentChannelDataList = []
        voltageChannelDataList = []
        powerChannelDataList = []
        
        dataCount = udasDev.GetCurrentChannelDataCount(channelIdx)
        channelName = udasDev.GetChannelName(channelIdx)
        print(f"Count of current data-points in series '{channelName}' is {dataCount}")
        for dataIdx in range(dataCount):
            currentChannelData = udasDev.GetCurrentChannelData(channelIdx, dataIdx)
            print(f"        Current data-point at index {dataIdx} is {currentChannelData}")
            currentChannelDataList.append(currentChannelData)

        dataCount = udasDev.GetVoltageSeriesDataCount(channelIdx)
        print(f"Count of voltage data-points in series '{channelName}' is {dataCount}")
        voltageChannelData = udasDev.GetVoltageChannelData(channelIdx, dataIdx)
        for dataIdx in range(dataCount):
            print(f"        Voltage data-point at index {dataIdx} is {udasDev.GetVoltageChannelData(channelIdx, dataIdx)}")
            voltageChannelDataList.append(round(voltageChannelData, 2))

        dataCount = udasDev.GetPowerSeriesDataCount(channelIdx)
        print(f"Count of power data-points in series '{channelName}' is {dataCount}")
        powerChannelData = udasDev.GetPowerChannelData(channelIdx, dataIdx)
        for dataIdx in range(dataCount):
            print(f"        Power data-point at index {dataIdx} is {udasDev.GetPowerChannelData(channelIdx, dataIdx)}")
            powerChannelDataList.append(round(powerChannelData, 2))


        # dataIndexList = [idx for idx in range(dataCount)]
        # visualize(dataIndexList, seriesName, currentSeriesDataList, voltageSeriesDataList, powerSeriesDataList)

    # get time series data for particular channelIdx of series for given WaveFormType
    for channelIdx in range(0, channelCount):
        print("\nSeries Name:", udasDev.GetSeriesName(channelIdx))
        channelData = udasDev.GetTimeSeries(channelIdx, WaveFormType.eWaveFormCurrent|WaveFormType.eWaveFormVoltage|WaveFormType.eWaveFormPower)

        for time, value in channelData.CurrentChannel().items():
            print(f"        Current data-point at time: {time} is: {value}")

        for time, value in channelData.VoltageChannel().items():
            print(f"        Voltage data-point at time: {time} is: {value}")

        for time, value in channelData.PowerChannel().items():
            print(f"        Power data-point at time: {time} is: {value}")

    # get statistical data for particular channelIdx of series for given WaveFormType
    for channelIdx in range(0, channelCount):
        print("\nSeries Name:", udasDev.GetSeriesName(channelIdx))
        print(f"        Average current is: {udasDev.GetWaveformAvg(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Total measurement current duration is: {udasDev.GetWaveformDuration(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Area of current duration is: {udasDev.GetWaveformArea(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Current population standard deviation is: {udasDev.GetWaveformPStdDev(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Maximum current is: {udasDev.GetWaveformMax(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Current range is: {udasDev.GetWaveformRange(channelIdx, WaveFormType.eWaveFormCurrent)}")
        print(f"        Minimum current is: {udasDev.GetWaveformMin(channelIdx, WaveFormType.eWaveFormCurrent)}")

    # Set a temporary export folder location for UDAS data in CSV format
    udasDev.SetExportPath("C:\\Alpaca\\Examples\\data")

    udasDev.ExportAsCSV()
    # Get export result folder location of UDAS data in CSV format
    exportPath = udasDev.GetExportPath()
    print(f"Get export result folder location: {exportPath}")

if __name__ == "__main__":
    main()
