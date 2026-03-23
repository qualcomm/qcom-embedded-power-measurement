#!/usr/bin/env python3

from EPMDev import *

def main() :
	print("Alpaca Version: ", AlpacaVersion())
	print("EPM Version: ",  EPMVersion())

	deviceCount  = GetDeviceCount()
	print("Device Count", deviceCount)

	if (deviceCount > 0) :

		index = 0
		while (index < deviceCount) :
			epmDevice = GetDevice(index)
			index += 1
			if (epmDevice is not None) :
				print (f"Found device with port name: {epmDevice.PortName()} and serial number: {epmDevice.SerialNumber()}")

		epmDevice = GetDevice(0)
		if (epmDevice is not None) :		
			if (epmDevice.Open()) :
				try :
					print("UUID:", epmDevice.GetUUID())
		
					print("Platform Path:", epmDevice.GetPlatformPath())

					platformCount = epmDevice.GetPlatformCount()
					print("Platform Count:", platformCount)
					platformIndex = 0
					while (platformIndex < platformCount) :
						platformDesc = epmDevice.GetPlatform(platformIndex)
						platformIndex += 1
						platformAttributes = platformDesc.split(";")
						print("   Platform:", platformAttributes[0], " Path: " , platformAttributes[1])

					epmDevice.SetPlatform("MTP8450")
					epmDevice.SetRuntimeConfig(r"C:\Alpaca\Examples\data\SM8450_Battery.rcnf")

					channelIndex = 0
					channelCount = epmDevice.GetChannelCount()
					print("Channel Count: ", channelCount)
					print("Channels")
					while (channelIndex < channelCount) :
						hash = epmDevice.GetChannelHash(channelIndex)
						print("   Channel: ", epmDevice.GetChannelName(channelIndex),  "current: ", epmDevice.GetCurrentChannelState(hash),
							"voltage: ", epmDevice.GetVoltageChannelState(hash))
						channelIndex += 1

					epmDevice.Acquire(4.0)

					epmDevice.Close()
				except RuntimeError as error :
					print(error)
				
			else :
				print(f"Device with port name: {epmDevice.PortName()} can't be opened.")
		else :
			print(f"Device with port name: {epmDevice.PortName()} not found.")


if __name__ == "__main__" :
	main()