# EPM Python API

QEPM provides the EPM API to help you automate operations that you
can perform using EPM Scope, Embedded Power Measurement (EPM), and the EEPROM Utility.
The EPM APIs are offered for the following programming languages:

- Python
- C#
- C++

We will discuss in detail about the EPM Python API. If you have not already
installed the library, please refer to
[installing the libraries](./01-Bootcamp.md#installing-the-libraries) section.

To test if the installation is working properly, please refer to the 
[testing the setup](./01-Bootcamp.md#testing-the-setup) section.

## Class reference

**EPMDevice** - The class that represents an EPM device. It has the following attributes:

- Port name: The name of the COM port
- Description: The description of the EPM device
- Serial number: The serial number of the EPM device

The available methods from the EPMDevice object is described in the API reference table.

## API reference

**EPMDev** API:

The following methods will be available to you as soon as you import the **EPMDev** module using `import EPMDev`.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| EPMVersion() | None | string | It returns the installed version of EPM as string. |
| GetLastError() | None | string | It returns the last error message from EPM as string. |
| GetLoggingState() | None | boolean | It returns the state of application logging as boolean. |
| SetLoggingState(state) | state: The boolean state to enable/disable logging. | None | Whether to enable application logging based on the `state` parameter. |
| WriteToLog(log_message) | log_message: The string message to be written to the app-log. | None | It writes the `log_message` to the application log.|
| GetAppLogPath() | None | string | It returns a string containing the path to the app-log. |
| SetAppLogPath(app_log_path) | app_log_path: The path to the app-log. | None | It sets the path for the app-log and throws `RuntimeError` if the operation is unsuccessful. |
| GetDeviceCount() | None | integer | It returns the number of connected EPM devices. |
| GetDevice(device_index) | device_index: The index of the device (integer) | EPMDevice | It returns the EPMDevice object for the given device index. |

**EPMDevice** API:

The following methods will be available to you when you create a **EPMDevice** object. Some methods here will be available once the device is opened using
`epm_device.Open()` where the epm_device is an instance of `EPMDevice` type.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| PortName() | None | string | It returns the device's name of the port as string. |
| Description() | None | string | It returns the description of the device as string. |
| SerialNumber() | None | string | It returns the serial number of the device as string. |
| Valid() | None | boolean | It returns whether the device is valid as bool. |
| Open() | None | boolean | It opens the EPM device for various EPM operations and returns whether the device was opened successfully as bool. |
| Close() | None | None | It closes an opened EPM device. It does not return a value. |
| GetUUID() | None | string | It returns the UUID of the EPM device as string. |
| GetPlatformPath() | None | string | It returns the path of the current platform as string. |
| SetPlatformPath(new_path) | new_path: The string containing the new platform path | boolean |  It sets the platform path of a platform and returns true if the operation was successful. |
| GetPlatformCount() | None | integer | Returns the number of available platforms in the platform path as integer. |
| GetPlatform(platform_index) | platform_index: The index of the platform as an integer | string | It returns the name of platform at the `platform_index` as string. |
| SetPlatform(platform) | platform: The name of the new platform as a string. | boolean | Sets the current platform to `platform`. |
| GetChannelCount() | None | integer | It returns the count of the channels in the platform. If the channelCount is 0, `RuntimeError` is thrown. |
| GetChannelName(channel_index) | channel_index: The index of the channel as integer. | string | It returns the name of the channel based on the `channel_index`. If the operation is unsuccessful, `RuntimeError` is thrown. |
| GetChannelHash(channel_index) | channel_index: The index of the channel as integer. | integer | It returns the channel hash for the channel at `channel_index`. If the operation is uncessful, a `RuntimeError` is thrown. |
| GetCurrentChannelState(channel_hash) | channel_hash: The channel hash of the current channel as unsigned long long. | boolean | It returns the state of the current channel based on `channel_hash`. |
| SetCurrentChannelState(channel_hash, active, save_raw) | channel_hash: The channel hash of the voltage channel as unsigned long long. active: Whether the channel state is active as bool. save_raw: Whether to save the data in raw format. | None | It sets the channel state of the current channel. |
| GetVoltageChannelState(channel_hash) | channel_hash: The channel hash of the voltage channel as unsigned long long. | boolean | It returns the state of the voltage channel based on `channel_hash`. |
| SetVoltageChannelState(channel_hash, active, save_raw) | channel_hash: The channel hash of the voltage channel as unsigned long long. active: Whether the channel state is active as bool. save_raw: Whether to save the data in raw format. | None | It sets the channel state of the voltage channel. |
| SetRuntimeConfig(runtime_config_file_path) | runtime_config_file_path: The path to the `.rcnf` file as string. | None | It sets the file path for the runtime config file (.rcnf). |
| Acquire(seconds) | seconds: The amount of time to record as a floating point number. | None | It starts measuring channel data on the device for the given `seconds`. The recorded data can be found from `GetResultsPath()` |
| StartAcquisition() | None | None | It starts measuring channel data based on runtime configuration. EPM will continue to measure data until `StopAcquisition()` is called. |
| StopAcquisition() | None | None | It stops measuring channel data based on the runtime configuration. |
| GetResultsPath() | None | string | It returns the path to the results directory after the acquisition. If no path has been set, it returns the default results directory path. |
| SetTemporaryResultsFolder(new_results_folder) | new_results_folder: The string containing the new results directory path. | None | It sets the path to the results directory using the path in the `new_results_folder`. |
