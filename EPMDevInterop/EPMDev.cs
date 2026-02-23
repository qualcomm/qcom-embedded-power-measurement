// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace EPMDevInterop
{
	public class EPMException : Exception
	{
		public EPMException()
		{
		}

		public EPMException(string message)
			: base(message)
		{
		}
	}

	public class EPMDevice
	{
		public EPMDevice(string portName, string description, string serialNumber)
		{
			_portName = portName;
			_description = description;
			_serialNumber = serialNumber;
		}

		public string alpacaVersion
		{
			get { return GetAlpacaVersion(); }
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetAlpacaVersion(byte[] alpacaVersion, uint bufferSize);

		static public string GetAlpacaVersion()
		{
			string result;

			byte[] output = new byte[kBufferSize];
			testResult(GetAlpacaVersion(output, kBufferSize));
			result = Encoding.ASCII.GetString(output).Trim('\0');

			return result;
		}

		public string epmVersion
		{
			get { return GetEPMVersion(); }
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetEPMVersion(byte[] epmVersion, uint bufferSize);

		static public string GetEPMVersion()
		{
			string result = "";

			byte[] output = new byte[kBufferSize];
			testResult(GetEPMVersion(output, kBufferSize));
			result = Encoding.ASCII.GetString(output).Trim('\0');

			return result;
		}

		public string lastError
		{
			get { return GetLastEPMError(); }
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetLastEPMError(byte[] error, uint bufferSize);

		static public string GetLastEPMError()
		{
			string result = "";

			byte[] output = new byte[kBufferSize];
			testResult(GetLastEPMError(output, kBufferSize));
			result = Encoding.ASCII.GetString(output).Trim('\0');

			return result;
		}

		public uint getLoggingState
		{
			get { return GetLoggingState(); }
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetLoggingState(ref bool loggingState);

		static public uint GetLoggingState()
		{
			return GetLoggingState();
		}

		public uint setLoggingState
		{
			get { return SetLoggingState(); }
		}

		[DllImport("EPMDev.dll")]
		private static extern uint SetLoggingState(bool loggingState);

		static public uint SetLoggingState()
		{
			return SetLoggingState();
		}

		[DllImport("EPMDev.dll")]
		private static extern uint WriteToAppLog(uint epmHandle, String logText);

		public uint WriteToAppLog(String logText)
		{
			return WriteToAppLog(_epmHandle, logText);
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetDeviceCount(ref uint deviceCount);

		[DllImport("EPMDev.dll")]
		private static extern uint InitializeEPMDev();

		static public uint DeviceCount()
		{
			uint deviceCount = 0;
			InitializeEPMDev();
			GetDeviceCount(ref deviceCount);
			return deviceCount;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetPortData(uint deviceIndex, byte[] portData, uint bufferSize);

		static public EPMDevice GetDevice(uint deviceIndex)
		{
			EPMDevice result = null;

			byte[] output = new byte[kBufferSize];
			uint epmResult = GetPortData(deviceIndex, output, kBufferSize);
			if (epmResult > 0)
			{
				string portData = Encoding.ASCII.GetString(output);
				string[] portAttributes = portData.Split(';');
				if (portAttributes.Length > 2)
				{
					result = new EPMDevice(portAttributes[0].Trim('\0'), portAttributes[1].Trim('\0'), portAttributes[2].Trim('\0'));
				}
			}

			return result;
		}

		public string portName
		{
			get { return PortName(); }
		}

		public string PortName()
		{
			return _portName;
		}

		public string description
		{
			get { return Description(); }
		}

		public string Description()
		{
			return _description;
		}

		public string serialNumber
		{
			get { return SerialNumber(); }
		}

		public string SerialNumber()
		{
			return _serialNumber;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint OpenHandleByDescription(string portName);

		public bool Open()
		{
			bool result = false;

			if (_portName.Length > 0)
				_epmHandle = OpenHandleByDescription(_portName);

			if (_epmHandle == BAD_EPM_HANDLE)
			{
				if (_serialNumber.Length > 0)
					_epmHandle = OpenHandleByDescription(_serialNumber);
			}

			if (_epmHandle != BAD_EPM_HANDLE)
				result = true;
			else
				throw new EPMException(_portName + " not found");

			return result;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint CloseEPMHandle(uint epmHandle);

		public bool Close()
		{
			uint epmResult = CloseEPMHandle(_epmHandle);
			_epmHandle = BAD_EPM_HANDLE;
			if (epmResult == NO_EPM_ERROR)
				return true;

			if (epmResult == BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");

			return false;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetUUID(uint epmHandle, byte[] uuid, uint bufferSize);

		public string GetUUID()
		{
			string result = "";
			if (TestHandle())
			{
				byte[] buffer = new byte[kBufferSize];
				uint epmResult = GetUUID(_epmHandle, buffer, kBufferSize);
				if (epmResult == EPM_BAD_EPM_HANDLE)
					throw new EPMException("Bad EPM Handle");
				else
					result = Encoding.ASCII.GetString(buffer).Trim('\0');
			}

			return result;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetPlatformPath(uint epmHandle, byte[] platformPath, uint bufferSize);

		public string GetPlatformPath()
		{
			string result = "";
			if (TestHandle())
			{
				byte[] buffer = new byte[kBufferSize];
				uint epmResult = GetPlatformPath(_epmHandle, buffer, kBufferSize);
				if (epmResult == EPM_BAD_EPM_HANDLE)
					throw new EPMException("Bad EPM Handle");
				else
					result = Encoding.ASCII.GetString(buffer).Trim('\0');
			}

			return result;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint SetPlatformPath(uint epmHandle, string platformPath);

		public bool SetPlatformPath(string platformPath)
		{
			uint epmResult = SetPlatformPath(_epmHandle, platformPath);
			if (epmResult == NO_EPM_ERROR)
				return true;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return false;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetPlatformCount(uint epmHandle, ref uint count);

		public uint GetPlatformCount()
		{
			uint count = 0;
			uint epmResult = GetPlatformCount(_epmHandle, ref count);
			if (epmResult == NO_EPM_ERROR)
				return count;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return count;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetPlatform(uint epmHandle, uint platformIndex, byte[] platform, uint bufferSize);

		public string GetPlatform(uint platformIndex)
		{
			string result = "";
			byte[] platformData = new byte[kBufferSize];
			uint epmResult = GetPlatform(_epmHandle, platformIndex, platformData, kBufferSize);
			result = Encoding.ASCII.GetString(platformData).Trim('\0');
			if (epmResult == NO_EPM_ERROR)
				return result;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return result;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint SetPlatform(uint epmHandle, string platformData);

		public bool SetPlatform(string platformData)
		{
			uint epmResult = SetPlatform(_epmHandle, platformData);
			if (epmResult == NO_EPM_ERROR)
				return true;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return false;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetChannelCount(uint epmHandle, ref uint channelCount);

		public uint GetChannelCount()
		{
			uint channelCount = 0;
			uint epmResult = GetChannelCount(_epmHandle, ref channelCount);
			if (epmResult == NO_EPM_ERROR)
				return channelCount;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return channelCount;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetChannel(uint epmHandle, uint channelIndex, byte[] channelDesc, uint bufferSize, ref ulong channelHash);

		public String GetChannelName(uint channelIndex)
		{
			String result = "";
			ulong channelHash = 0;
			byte[] channelDesc = new byte[kBufferSize];

			uint epmResult = GetChannel(_epmHandle, channelIndex, channelDesc, kBufferSize, ref channelHash);
			result = Encoding.ASCII.GetString(channelDesc).Trim('\0');
			if (epmResult == NO_EPM_ERROR)
				return result;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return result;
		}

		public ulong GetChannelHash(uint channelIndex)
		{
			ulong channelHash = 0;
			byte[] channelDesc = new byte[kBufferSize];

			uint epmResult = GetChannel(_epmHandle, channelIndex, channelDesc, kBufferSize, ref channelHash);
			if (epmResult == NO_EPM_ERROR)
				return channelHash;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return channelHash;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetCurrentChannelState(uint epmHandle, ulong channelHash, ref bool active, ref bool saveRaw);

		public bool GetCurrentChannelState(ulong channelHash)
		{
			bool active = false;
			bool saveRaw = false;
			uint epmResult = GetCurrentChannelState(_epmHandle, channelHash, ref active, ref saveRaw);
			if (epmResult == NO_EPM_ERROR)
				return active;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return active;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint SetCurrentChannelState(uint epmHandle, ulong channelHash, bool active, bool saveRaw);

		public void SetCurrentChannelState(ulong channelHash, bool active)
		{
			bool saveRaw = false;
			uint epmResult = SetCurrentChannelState(_epmHandle, channelHash, active, saveRaw);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		[DllImport("EPMDev.dll")]
		private static extern uint SetRuntimeConfig(uint epmHandle, String runtimeConfigFile);

		public void SetRuntimeConfig(String runtimeConfigFile)
		{
			uint epmResult = SetRuntimeConfig(_epmHandle, runtimeConfigFile);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetVoltageChannelState(uint epmHandle, ulong channelHash, ref bool active, ref bool saveRaw);

		public bool GetVoltageChannelState(ulong channelHash)
		{
			bool active = false;
			bool saveRaw = false;
			uint epmResult = GetVoltageChannelState(_epmHandle, channelHash, ref active, ref saveRaw);
			if (epmResult == NO_EPM_ERROR)
				return active;
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
			return active;
		}

		[DllImport("EPMDev.dll")]
		private static extern uint SetVoltageChannelState(uint epmHandle, ulong channelHash, bool active, bool saveRaw);

		public void SetVoltageChannelState(ulong channelHash, bool active)
		{
			bool saveRaw = false;
			uint epmResult = SetVoltageChannelState(_epmHandle, channelHash, active, saveRaw);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		[DllImport("EPMDev.dll")]
		private static extern uint SetTemporaryResultsFolder(uint epmHandle, String resultsFolder);

		public void SetTemporaryResultsFolder(String resultsFolder)
		{
			uint epmResult = SetTemporaryResultsFolder(_epmHandle, resultsFolder);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		[DllImport("EPMDev.dll")]
		private static extern uint Acquire(uint epmHandle, double seconds);

		public void Acquire(double seconds)
		{
			uint epmResult = Acquire(_epmHandle, seconds);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		[DllImport("EPMDev.dll")]
		private static extern uint StartAcquisition(uint epmHandle);

		public void StartAcquisition()
		{
			uint epmResult = StartAcquisition(_epmHandle);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		[DllImport("EPMDev.dll")]
		private static extern uint StopAcquisition(uint epmHandle);

		public void StopAcquisition()
		{
			uint epmResult = StopAcquisition(_epmHandle);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		[DllImport("EPMDev.dll")]
		private static extern uint GetResultsPath(uint epmHandle, ref byte[] resultsPath, uint bufferSize);

		public void GetResultsPath()
		{
			byte[] buffer = new byte[kBufferSize];
			uint epmResult = GetResultsPath(_epmHandle, ref buffer, kBufferSize);
			if (epmResult == EPM_BAD_EPM_HANDLE)
				throw new EPMException("Bad EPM Handle");
		}

		// Some Handy Constants
		private const uint kBufferSize = 1024;
		private const uint BAD_EPM_HANDLE = 0;

		private const uint NO_EPM_ERROR = 0;
		private const uint EPM_BUFFER_TOO_SMALL = 1;
		private const uint EPM_BAD_EPM_HANDLE = 2;
		private const uint EPM_CMD_NOT_FOUND = 3;
		private const uint EPM_BAD_INDEX = 4;

		// Instance variables
		private uint _epmHandle = 0;
		private string _portName;
		private string _description;
		private string _serialNumber;

		static private void testResult(uint epmResult)
		{
			switch (epmResult)
			{
				case NO_EPM_ERROR:
					break;

				case EPM_BAD_EPM_HANDLE:
					throw new EPMException("Bad EPM Handle");

				case EPM_BUFFER_TOO_SMALL:
					throw new EPMException("EPM Buffer Too Small");

				case EPM_CMD_NOT_FOUND:
					throw new EPMException("EPM Command Not Found");

				case EPM_BAD_INDEX:
					throw new EPMException("EPM Bad Index");

				default:
					throw new EPMException("EPM Error: " + epmResult);
			}
		}

		private bool TestHandle()
		{
			bool result = false;

			if (_epmHandle != BAD_EPM_HANDLE)
			{
				result = true;
			}
			else
			{
				throw new EPMException("Handle is bad.  Is the EPM Device is closed?");
			}

			return result;
		}
	}
}
