// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <iostream>

bool gHaveDevice{false};
wchar_t	gDeviceName[1024];

#include "EPMDev.h"

// QCommon
#include "Range.h"

// Qt
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
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

namespace
{
	const int kExitOk{0};
	const int kExitUsage{1};
	const int kExitFailure{2};

	void printLastError(const char* context)
	{
		char lastError[1024]{};

		std::cerr << context;
		if (GetLastEPMError(lastError, sizeof(lastError)) == NO_EPM_ERROR && lastError[0] != '\0')
		{
			std::cerr << ": " << lastError;
		}
		std::cerr << std::endl;
	}

	EPMDeviceDescriptors listDevices()
	{
		EPMDeviceDescriptors devDescriptors;
		char temp[1024];
		int deviceCount{0};

		if (GetDeviceCount(&deviceCount) != NO_EPM_ERROR)
		{
			printLastError("Device count failed");
			return devDescriptors;
		}

		std::cout << "Device Count: " << deviceCount << std::endl;

		if (deviceCount > 0)
		{
			std::cout << "Devices" << std::endl;
		}

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
				}

				std::cout << "   Device: " << temp << std::endl;
			}
		}

		return devDescriptors;
	}

	void listPlatforms(EPM_HANDLE epmHandle)
	{
		char temp[1024];

		if (GetPlatformPath(epmHandle, temp, sizeof(temp)) == NO_EPM_ERROR)
		{
			std::cout << std::endl << "Platform Path: " << temp << std::endl << std::endl;
		}

		int platformCount{0};
		if (GetPlatformCount(epmHandle, &platformCount) != NO_EPM_ERROR)
		{
			printLastError("Platform count failed");
			return;
		}

		std::cout << "Platform Count: " << platformCount << std::endl << std::endl;

		for (const auto platformIndex: range(platformCount))
		{
			if (GetPlatform(epmHandle, platformIndex, temp, sizeof(temp)) != NO_EPM_ERROR)
			{
				continue;
			}

			QStringList platformAttributes = QString(temp).split(";");

			if (platformAttributes.count() >= 2)
			{
				std::cout << "Platform: " << platformAttributes[0].toLatin1().data() <<
					" path: " << platformAttributes[1].toLatin1().data() << std::endl;
			}
		}
	}

	void listChannels(EPM_HANDLE epmHandle)
	{
		char temp[1024];
		int channelCount{0};

		EPM_RESULT result = GetChannelCount(epmHandle, &channelCount);
		if (result != NO_EPM_ERROR)
		{
			std::cout << "   Channel Count Failed: " << result << std::endl;
			return;
		}

		std::cout << "Channels: " << channelCount << std::endl;

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
}

int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	QCoreApplication::setApplicationName("EPMCLI");

	QCommandLineParser parser;
	parser.setApplicationDescription(
		"QEPM command line example.\n\n"
		"Without options, lists the QEPM version, the attached EPM devices and the\n"
		"platforms known to the first (or the selected) device. Nothing is acquired.\n\n"
		"An acquisition runs only when --acquire is given, together with --platform\n"
		"and --runtime-config.");

	const QCommandLineOption helpOption = parser.addHelpOption();

	QCommandLineOption deviceOption(QStringList() << "d" << "device",
		"Device to use, by target name or serial number. Defaults to the first device.",
		"name");
	QCommandLineOption platformOption(QStringList() << "p" << "platform",
		"Platform to select before acquiring, for example MTP8450.",
		"platform");
	QCommandLineOption runtimeConfigOption(QStringList() << "r" << "runtime-config",
		"Runtime configuration (.rcnf) file to load before acquiring.",
		"file");
	QCommandLineOption acquireOption(QStringList() << "a" << "acquire",
		"Acquire for the given number of seconds. Requires --platform and --runtime-config.",
		"seconds");

	parser.addOption(deviceOption);
	parser.addOption(platformOption);
	parser.addOption(runtimeConfigOption);
	parser.addOption(acquireOption);

	if (parser.parse(QCoreApplication::arguments()) == false)
	{
		std::cerr << "EPMCLI: " << parser.errorText().toStdString() << std::endl;
		std::cerr << "Use --help for usage." << std::endl;
		return kExitUsage;
	}

	if (parser.isSet(helpOption))
	{
		std::cout << parser.helpText().toStdString();
		return kExitOk;
	}

	if (parser.positionalArguments().isEmpty() == false)
	{
		std::cerr << "EPMCLI: unexpected argument '" <<
			parser.positionalArguments().first().toStdString() << "'" << std::endl;
		std::cerr << "Use --help for usage." << std::endl;
		return kExitUsage;
	}

	const bool doAcquire = parser.isSet(acquireOption);
	double acquireSeconds{0.};

	if (doAcquire)
	{
		bool ok{false};
		acquireSeconds = parser.value(acquireOption).toDouble(&ok);
		if (ok == false || acquireSeconds <= 0.)
		{
			std::cerr << "EPMCLI: --acquire needs a positive number of seconds" << std::endl;
			return kExitUsage;
		}

		if (parser.isSet(platformOption) == false || parser.isSet(runtimeConfigOption) == false)
		{
			std::cerr << "EPMCLI: --acquire requires --platform and --runtime-config" << std::endl;
			return kExitUsage;
		}
	}
	else if (parser.isSet(platformOption) || parser.isSet(runtimeConfigOption))
	{
		std::cerr << "EPMCLI: --platform and --runtime-config are only used with --acquire" << std::endl;
		return kExitUsage;
	}

	char temp[1024];

	if (InitializeEPMDev() != NO_EPM_ERROR)
	{
		std::cerr << "Initialization Failed" << std::endl;
		return kExitFailure;
	}

	if (GetQEPMVersion(temp, sizeof(temp)) == NO_EPM_ERROR)
	{
		std::cout << "QEPM Version: " << temp << std::endl;
	}

	if (GetEPMVersion(temp, sizeof(temp)) == NO_EPM_ERROR)
	{
		std::cout << "EPM Version: " << temp << std::endl;
	}

	EPMDeviceDescriptors devDescriptors = listDevices();

	QString target = parser.value(deviceOption).trimmed();
	if (target.isEmpty())
	{
		if (devDescriptors.isEmpty())
		{
			if (doAcquire)
			{
				std::cerr << "EPMCLI: no EPM device found" << std::endl;
				return kExitFailure;
			}
			return kExitOk;
		}

		target = devDescriptors.first()._target;
	}

	EPM_HANDLE epmHandle = OpenHandleByDescription(target.toLatin1().data());
	if (epmHandle == kBadHandle)
	{
		printLastError("Open failed");
		return kExitFailure;
	}

	int exitCode{kExitOk};

	listPlatforms(epmHandle);

	if (doAcquire)
	{
		const QByteArray platform = parser.value(platformOption).toLatin1();
		const QByteArray runtimeConfig = parser.value(runtimeConfigOption).toLatin1();

		std::cout << std::endl;

		if (SetPlatform(epmHandle, platform.constData()) != NO_EPM_ERROR)
		{
			printLastError(QByteArray("Set platform failed for " + platform).constData());
			exitCode = kExitFailure;
		}
		else if (SetRuntimeConfig(epmHandle, runtimeConfig.constData()) != NO_EPM_ERROR)
		{
			printLastError(QByteArray("Load runtime config failed for " + runtimeConfig).constData());
			exitCode = kExitFailure;
		}
		else
		{
			listChannels(epmHandle);

			std::cout << "Acquiring for " << acquireSeconds << " s..." << std::endl;
			if (Acquire(epmHandle, acquireSeconds) != NO_EPM_ERROR)
			{
				printLastError("Acquire failed");
				exitCode = kExitFailure;
			}
			else if (GetResultsPath(epmHandle, temp, sizeof(temp)) == NO_EPM_ERROR)
			{
				std::cout << "Results: " << temp << std::endl;
			}
		}
	}

	CloseEPMHandle(epmHandle);

	return exitCode;
}
