// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <iostream>

bool gHaveDevice{false};
wchar_t	gDeviceName[1024];

#include "EPMDev.h"

// QCommon
#include "Range.h"

// Qt
#include <QList>
#include <QString>
#include <QStringList>

struct EPMDeviceDesc
{
	EPMDeviceDesc() = default;
	EPMDeviceDesc(const EPMDeviceDesc& copyMe) = default;
	~EPMDeviceDesc() = default;

	QString					_target;
	QString					_serial;
	QString					_uuid;
};

typedef QList<EPMDeviceDesc> EPMDeviceDescriptors;

int main(int argc, char* argv[])
{
	(void) argc;
	(void) argv;

	char temp[1024];

	EPM_RESULT result;

	result = InitializeEPMDev();
	if (result != NO_EPM_ERROR)
	{
		std::cout << "Initialization Failed" << std::endl;
		return 0;
	}

	result = GetQEPMVersion(temp, sizeof(temp));
	if (result == NO_EPM_ERROR)
		std::cout << "QEPM Version: " << temp << std::endl;

	result = GetEPMVersion(temp, sizeof(temp));
	if (result == NO_EPM_ERROR)
		std::cout << "EPM Version: " << temp << std::endl;

	int deviceCount;
	result = GetDeviceCount(&deviceCount);
	if (result == NO_EPM_ERROR)
	{
		std::cout << "Device Count: " << deviceCount << std::endl;

		if (deviceCount > 0)
		{
			EPMDeviceDescriptors devDescriptors;
			QString firstTarget;

			std::cout << "Devices" << std::endl;
			for (const auto deviceIndex: range(deviceCount))
			{
				if (GetPortData(deviceIndex, temp, sizeof(temp)) > 0)
				{
					QStringList attributes = QString(temp).split(";");

					if (attributes.count() >= 3)
					{
						EPMDeviceDesc epmDevDesc;

						epmDevDesc._target = attributes.at(0);
						epmDevDesc._serial = attributes.at(1);
						epmDevDesc._uuid = attributes.at(2);

						devDescriptors.push_back(epmDevDesc);

						if (firstTarget.isEmpty())
							firstTarget = epmDevDesc._target;
					}

					std::cout << "   Device: " << temp << std::endl;
				}
			}

			EPM_HANDLE epmHandle = OpenHandleByDescription(firstTarget.toLatin1().data());
			if (epmHandle != kBadHandle)
			{
				if (GetPlatformPath(epmHandle, temp, sizeof(temp) )== NO_EPM_ERROR)
				{
					std::cout << std::endl << "Platform Path: " << temp << std::endl << std::endl;;
				}

				int platformCount;
				result = GetPlatformCount(epmHandle, &platformCount);
				std::cout << "Platform Count: " << platformCount << std::endl << std::endl;;

				for (const auto platformIndex: range(platformCount))
				{
					GetPlatform(epmHandle, platformIndex, temp, sizeof(temp));

					QStringList platformAttributes = QString(temp).split(";");

					if (platformAttributes.count() >= 2)
					{
						std::cout << "Platform: " << platformAttributes[0].toLatin1().data() <<
							" path: " << platformAttributes[1].toLatin1().data() << std::endl << std::endl;
					}
				}

				if (SetPlatform(epmHandle, "MTP8450") == NO_EPM_ERROR)
				{
					// absolute path
					if (SetRuntimeConfig(epmHandle, "C:\\QEPM\\Examples\\data\\SM8450_Battery.rcnf") == NO_EPM_ERROR) // from preferences
					{
						int channelCount;

						result = GetChannelCount(epmHandle, &channelCount);
						if (result == NO_EPM_ERROR)
						{
							std::cout << "Channels " << temp << std::endl;

							for (const auto& channelIndex: range(channelCount))
							{
								unsigned long long hash;
								if (GetChannel(epmHandle, channelIndex, temp, sizeof(temp), &hash) == NO_EPM_ERROR)
								{
									bool currentValid{false};
									bool currentState{false};
									bool currentSaveRaw{false};
									bool voltageValid{false};
									bool voltageState{false};
									bool voltageSaveRaw{false};

									if (GetCurrentChannelState(epmHandle, hash, &currentState, &currentSaveRaw) == NO_EPM_ERROR)
									{
										currentValid = true;
									}

									if (GetVoltageChannelState(epmHandle, hash, &voltageState, &voltageSaveRaw) == NO_EPM_ERROR)
									{
										voltageValid = true;
									}

									std::cout << "   Channel: " << std::string(temp) <<
										" current: " << (currentValid ? (currentState ? "true" : "false") : "-") <<
										" voltage: " << (voltageValid ? (voltageState ? "true" : "false") : "-") <<
										std::endl;
								}
							}
						}
						else
						{
							std::cout << QString("   Channel Count Failed: %1\n").arg(result).toLatin1().data();
						}

						std::cout << "Acquiring..." << std::endl;
						Acquire(epmHandle, 4.);
					}
				}

				CloseEPMHandle(epmHandle);
			}
		}
	}

	return 0;
}
