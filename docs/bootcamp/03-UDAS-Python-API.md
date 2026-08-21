# UDAS Python API

QEPM provides the UDAS API to help you perform post-processing and analysis on Embedded Power Measurement (EPM) results.
The UDAS APIs are offered for the following programming languages:

- Python
- C++

We will discuss in detail about the UDAS Python API. If you have not already installed the library,
please refer to the [installing the libraries](./01-Bootcamp.md#installing-the-libraries) section.

To test if the installation is working properly, please refer to the 
[testing the setup](./01-Bootcamp.md#testing-the-setup) section.

## Example script

An example script to demonstrate the use of UDASDev API to enumerate EPM data in Python is provided here:
`C:\QEPM\Examples\Python\AutomationTestUDAS`. To try out the example script, first
capture EPM data using the [EPM](../getting-started/04-Embedded-Power-Measurement.md) tool or [EPM API](./02-EPM-Python-API.md), then update the results path in the
example script to your EPM data path, typically `C:\Users\<USERNAME>\Documents\EPM\Results`.

## Class reference

**UDASDevice** - The class representing a UDAS interface. All operations on the EPM results should happen through this interface.
To get started, create a UDASDevice object.

The available methods from the UDASDevice object is described in the API reference table.

The following terms are used widely in the API reference table:

| Terminology | Definition |
| ------------- | ---------- |
| Channel | When you collect EPM data, you collect data corresponding to a GPIO rail. This rail is mapped to the platform in use. It is advised to enable only the required channels for data collection to ensure accuracy |
| WaveformType | The channel offers 2 kinds of data, current and voltage. The third kind is power which is inferred from the former 2 types |
| Series / data-points | The channel data that you've collected for a certain duration, contains numerous data-points spread over time. We have termed this `series`. Series and data-points are used interchangeably |
| exportDirectory | This is the path where the exported data will be produced. If no path is provided, the export location can be found from `Embedded Power Measurement (EPM) > File menu > Preferences > Output path` |
| exportConfiguration | Not to be confused with `exportDirectory`. This is a configuration that you can generate from the Embedded Power Measurement (EPM) tool to allow only selected channels to be exported. Turn ON channels to export in EPM and save the configuration as Export configuration. Use the configuration path as parameter to `ExportAsCSV(exportConfiguration)` |

## API reference

**UDASDev** API:

The following methods will be available to you as soon as you import the **UDASDev** module using `import UDASDev`.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| EPMVersion() | None | string | It returns the installed version of EPM as string |
| GetLastUdasError() | None | string | It returns the last error (if any) otherwise returns empty string |

**UDASDevice** API:


### General purpose

The following methods will be available to you when you create a **UDASDevice** object. These APIs will help you to access the data in the EPM results directory in a meaningful way.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| OpenResults(powerRunFileName) | `powerRunFileName`: The path to the EPM Power Run file (*.prn) | None | The API loads the EPM data into a UDASDevice object |
| GetChannelCount() | None | integer | The API returns the total count of channels present in the EPM result. Each channel is a unique GPIO rail on the debug board |
| GetChannelName(channelIndex) | `channelIndex`: The index of the channel denoted by integer | string | The API returns the name of the captured series  as string, present at the `channelIndex`. For example: `channelIndex: 0` may return `BATTERY` which could be the name of a channel, acquired in the EPM data |
| GetChannelIndex(channelName) | `channelName`: The name of the channel as string | integer | The API returns the `channelIndex` corresponding to the `channelName`. For example: `BATTERY` as channelName may return `0` as the index of channel, provided the appropriate channel data is available |
| GetCurrentChannelDataCount(channelIndex) | `channelIndex`: The integer representing the index of the channel | integer | The API returns the total number of data-points (also known as `series`) in the current channel at `channelIndex`. If you wish to get the time data-point along with current levels, please see: `GetTimeSeries(channelIndex, waveFormType)` |
| GetCurrentChannelData(channelIndex, dataIndex) | `channelIndex`: The integer representing the index of the channel, `dataIndex`: The integer representing the index of the data-point (also known as `series`) in a choosen current channel | float | The API returns the current value for a given `channelIndex` and `dataIndex` |
| GetVoltageChannelDataCount(channelIndex) | `channelIndex`: The integer representing the index of the channel | integer | The API returns the total number of data-points in the voltage channel at `channelIndex` |
| GetVoltageChannelData(channelIndex, dataIndex) | `channelIndex`: The integer representing the index of the channel, `dataIndex`: The integer representing the index of the data-point in the voltage channel | float | The API returns the voltage value for a given `channelIndex` and `dataIndex` |
| GetPowerChannelDataCount(channelIndex) | `channelIndex`: The integer representing the index of the channel | integer | The API returns the total number of data-points in the power channel at `channelIndex` |
| GetPowerChannelData(channelIndex, dataIndex) | `channelIndex`: The integer representing the index of the channel, `dataIndex`: The integer representing the index of the data-point in the power channel | float | The API returns the power value for a given `channelIndex` and `dataIndex` |

> The `channelIndex` is always less than `GetChannelCount()` and `dataIndex` is always less than `Get[Current/Voltage/Power]ChannelDataCount()`

### Timeseries channel data

The following set of API(s) will provide timing information along with data-points for the acquired channels.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| GetTimeSeries(channelIndex, waveFormType) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum | `ChannelData`: The object containing current/voltage/power data-points Use `CurrentChannel()`, `VoltageChannel()`, `PowerChannel()` to get time-series | The API returns data in the form of `ChannelData` object The object contains time-series of current/voltage/power levels for a given series denoted by its index, `channelIndex` |

### Statistics and analytics

The following set of API(s) helps to determine statistics / analytics of the collected data.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| GetWaveformAvg(channelIndex, waveFormType, startTime, endTime) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum (i.e. `eWaveFormCurrent`, `eWaveFormVoltage`, `eWaveFormPower`), `startTime`: Optional start time, `endTime`: Optional end time | float | The API returns the average value of the data-points for a given `channelIndex` and `waveFormType` |
| GetWaveformDuration(channelIndex, waveFormType, startTime, endTime) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum, `startTime`: Optional start time, `endTime`: Optional end time | float | The API returns the duration of the measured channel at `channelIndex` and for `waveFormType` |
| GetWaveformArea(channelIndex, waveFormType, startTime, endTime) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum, `startTime`: Optional start time, `endTime`: Optional end time | float | The API returns the area under curve/waveform for a given `channelIndex` and `waveFormType` |
| GetWaveformPStdDev(channelIndex, waveFormType, startTime, endTime) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum, `startTime`: Optional start time, `endTime`: Optional end time | float | The API returns the population standard deviation for a given `channelIndex` and `waveFormType` |
| GetWaveformMax(channelIndex, waveFormType, startTime, endTime) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum, `startTime`: Optional start time, `endTime`: Optional end time | float | The API returns the maxima data-point for a given `channelIndex` and `waveFormType` |
| GetWaveformRange(channelIndex, waveFormType, startTime, endTime) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum, `startTime`: Optional start time, `endTime`: Optional end time | float | The API returns the range between maximum and minimum data-point for a given `channelIndex` and `waveFormType` |
| GetWaveformMin(channelIndex, waveFormType, startTime, endTime) | `channelIndex`: The integer representing the index of the channel, `waveFormType`: The values defined in the `WaveFormType` enum, `startTime`: Optional start time, `endTime`: Optional end time | float | The API returns the minima data-point for a given `channelIndex` and `waveFormType` |

### Export to CSV

The following set of API(s) helps to export the EPM data into CSV.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| SetExportPath(exportDirectory) | `exportDirectory`: The string containing the export directory path | None | Configure the export path for the CSV output using `exportDirectory` |
| GetExportPath() | None | string | The API returns the path to the export directory if set using the `SetExportPath(exportDirectory)`. Otherwise, returns a default path |
| ExportAsCSV(exportConfiguration) | `exportConfiguration`: Optional configuration to allow selected channels to be exported into CSV | None | The API converts the EPM channel data into CSV and outputs at `GetExportPath()` location if set. Without the `exportConfiguration`, all channels will be exported to CSV. This may take some time for large datasets. |
