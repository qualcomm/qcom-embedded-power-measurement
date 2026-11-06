// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EepromCmd
#include "PSOCProgrammerCmdLine.h"

// epmLib
#include "EPMDevice.h"

// QCommon
#include "AppCore.h"
#include "QCommonConsole.h"

#include "Range.h"

// Qt
#include <QCoreApplication>
#include <QDateTime>

// C++
#include <iostream>

using namespace std;

PlatformIDs gPsocIDS;

const QByteArray kAppName{"PSOCProgrammer"};
const QByteArray kAppVersion{"1.3.0"};

void listDevices()
{
	auto deviceCount = _EPMDevice::deviceCount();

	if (deviceCount > 0)
	{
		cout << "Devices" << endl;
		for (auto index : range(deviceCount))
		{
			auto epmDevice = _EPMDevice::device(index);

			cout << "   Device:" << epmDevice->getTargetName().toLatin1().data() <<
				 " USB Serial:" << epmDevice->serialNumber().toLatin1().data() <<
				 " GUID:" << epmDevice->getUUID().toLatin1().data() <<
				 " Platform ID:" << epmDevice->getPlatformIDString().toLatin1().data() << endl;
		}
	}
	else
	{
		cout << "No devices found." << endl;
	}

	cout << endl;
}

void listPlatforms()
{
	PlatformContainer::initialize();
	PlatformIDList platformIDs = PlatformContainer::getDebugBoardsOfType(ePSOC);

	cout << "Platforms" << endl;
	for (const auto& platformID: platformIDs)
	{
		cout << "   Platform: " << platformID->_description.toLatin1().data() << " ID: " << QString::number(platformID->_platformID).toLatin1().data() << endl;
	}

	cout << endl;
}

static bool deviceMatches(EPMDevice& epmDevice, const QString& selector)
{
	const QString wanted = selector.trimmed();
	if (wanted.isEmpty())
		return false;

	const QStringList candidates{epmDevice->serialNumber().trimmed(), epmDevice->getTargetName().trimmed(),
		epmDevice->getUUID().trimmed()};

	for (const auto& candidate : candidates)
	{
		if (candidate.isEmpty() == false && candidate.compare(wanted, Qt::CaseInsensitive) == 0)
			return true;
	}

	return false;
}

static void printDeviceChoices(const EPMDevices& epmDevices)
{
	for (const auto& epmDevice : epmDevices)
	{
		if (epmDevice.isNull())
			continue;

		cout << "   Device:" << epmDevice->getTargetName().toLatin1().data() <<
			 " USB Serial:" << epmDevice->serialNumber().toLatin1().data() <<
			 " GUID:" << epmDevice->getUUID().toLatin1().data() << endl;
	}
}

// Picks the device to work on. With select=, the matching device is used even when several are
// connected. Without it, exactly one device must be connected, as before.
static EPMDevice selectDevice(PSOCCommandLine& commandLine, EPMDevices& epmDevices)
{
	EPMDevice result;

	const QString selector = commandLine.selector();
	_EPMDevice::getEPMDevices(epmDevices);

	if (epmDevices.isEmpty())
	{
		cout << "No devices found." << endl;
		return result;
	}

	if (selector.isEmpty())
	{
		if (epmDevices.count() == 1)
			return epmDevices.at(0);

		cout << epmDevices.count() << " devices are connected. Choose one with select=<USB serial | device name | GUID>:" << endl;
		printDeviceChoices(epmDevices);
		cout << "   (i.e.) PSOCProgrammer -p platformID=7 select=" <<
			epmDevices.at(0)->serialNumber().toLatin1().data() << endl;
		return result;
	}

	EPMDevices matches;
	for (auto& epmDevice : epmDevices)
	{
		if (epmDevice.isNull() == false && deviceMatches(epmDevice, selector))
			matches.push_back(epmDevice);
	}

	if (matches.count() == 1)
		return matches.at(0);

	if (matches.isEmpty())
		cout << "No connected device matches select=" << selector.toLatin1().data() << ". Connected devices:" << endl;
	else
		cout << "More than one connected device matches select=" << selector.toLatin1().data() << ":" << endl;

	printDeviceChoices(matches.isEmpty() ? epmDevices : matches);

	return result;
}

bool getProgramming(PSOCCommandLine& commandLine)
{
	bool result{false};
	EPMDevices epmDevices;

	EPMDevice epmDevice = selectDevice(commandLine, epmDevices);
	if (epmDevice.isNull())
		return false;

	// open() on Windows appends the USB serial again, so read it before opening.
	const QString usbSerial = epmDevice->serialNumber();

	try
	{
		epmDevice->open();

		if (epmDevice->programmed())
		{
			TargetInfo targetInfo;

			epmDevice->eepromRead();

			targetInfo = epmDevice->_targetInfo;

			std::cout << "Device: " << std::endl;

			quint8 chipset = epmDevice->_version._firmwareVersion[1];

			QString chipsetString{"Unknown"};
			switch (chipset)
			{
			case 3:
				chipsetString = "LP038";
				break;

			case 4:
				chipsetString = "LP030";
				break;
			}

			std::cout << "   Chipset: " << chipsetString.toLatin1().constData() << std::endl;
			std::cout << "   USB Serial: " << usbSerial.toLatin1().constData() << std::endl;
			std::cout << "   Serial Number: " << targetInfo.serialNumber().constData() << std::endl;
			QString platformDesc = PlatformContainer::toString(targetInfo._platformIdentifier);
			std::cout << QString("   Platform ID: %1 (%2)").arg(platformDesc).arg(targetInfo._platformIdentifier).toLatin1().constData() << std::endl;
			std::cout << "   Model Name: " << targetInfo.modelName().constData() << std::endl;
			std::cout << "   UUID: " << targetInfo._epmBoardUuid.toQUuid().toString().toLatin1().constData() << std::endl;
			QDateTime eepromTime = QDateTime::fromSecsSinceEpoch(targetInfo._eepromProgrammedTime);
			std::cout << "   Program Time: " << eepromTime.toString().toLatin1().constData() << std::endl;
		}
		else
		{
			std::cout << "The device has not been programmed." << std::endl;
		}

		result = true;
		epmDevice->close();
	}
	catch (EPMException& error)
	{
		cout << "Could not read the device: " << error.what() << endl;
	}
	catch (...)
	{
		cout << "Could not read the device." << endl;
	}

	return result;
}

bool programDevice(PSOCCommandLine& commandLine)
{
	bool result{false};

	EPMDevices epmDevices;

	QByteArray serialNumber = commandLine.serialNumber();
	QByteArray modelName = commandLine.modelName();
	quint32 platformID = commandLine.platformID();

	if (serialNumber.isEmpty() && modelName.isEmpty() && platformID == 0)
	{
		cout << "Parameters are empty. Nothing to program" << endl;
		return false;
	}

	EPMDevice epmDevice = selectDevice(commandLine, epmDevices);
	if (epmDevice.isNull())
		return false;

	// open() on Windows appends the USB serial again, so read it before opening.
	const QString usbSerial = epmDevice->serialNumber();

	TargetInfo targetInfo;

	try
	{
		epmDevice->open();

		if (epmDevice->programmed())
		{
			epmDevice->eepromRead();

			targetInfo = epmDevice->_targetInfo;
		}
		else
			targetInfo._eepromProgrammedTime = QDateTime::currentDateTime().toSecsSinceEpoch();

		if (targetInfo._epmBoardUuid.toQUuid().isNull())
		{
			QUuid uuid = QUuid::createUuid();
			targetInfo._epmBoardUuid = uuid;
		}

		if (platformID != 0)
			targetInfo._platformIdentifier = platformID;

		if (serialNumber.isEmpty() == false)
			targetInfo.setSerialNumber(serialNumber);

		if (modelName.isEmpty() == false)
			targetInfo.setModelName(modelName);

		epmDevice->_targetInfo = targetInfo;

		try
		{
			epmDevice->eepromWrite();

			cout << "EEPROM Updated" << endl << "   The EEPROM of device " <<
				usbSerial.toLatin1().data() << " has been updated with the new data." << endl;

			result = true;
		}
		catch (EPMException& error)
		{
			cout << "EEPROM Error" << endl << "The EEPROM has not been updated with the new data: " << error.what() << endl;
		}

		epmDevice->close();
	}
	catch (EPMException& epmError)
	{
		if (epmError.epmError() == MICRO_EPM_NOT_PROGRAMMED)
		{
			cout << "Unprogrammed Debug Board \n" << endl <<
				"This debug board has not been programmed by the factory. "
				"The QEPM group has no idea who that contact is for your group. Ask the person who supplied the MTP or device.  You are going to have "
				"to find the board supplier for your group." << endl;
		}
		else
		{
			cout << "Could not open the device: " << epmError.what() << endl;
		}
	}

	return result;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	a.setApplicationName(kAppName);
	a.setApplicationVersion(kAppVersion);

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;

	preferencesBase.setAppName(kAppName, kAppVersion);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

	PSOCCommandLine parser(a.arguments());

	int exitCode{0};

	if (parser.helpRequested() == false)
	{
		bool earlyReturn{false}; // if the user asks for information items, don't program the device

		_EPMDevice::updateDeviceList();

		if (parser.versionRequested())
		{
			std::cout << "Version: " << parser.version().data() << endl;
			earlyReturn = true;
		}

		if (parser.listSet())
		{
			listDevices();
			earlyReturn = true;
		}

		if (parser.listPlatformSet())
		{
			listPlatforms();
			earlyReturn = true;
		}

		if (parser.getProgramming())
		{
			if (getProgramming(parser) == false)
				exitCode = 1;
			earlyReturn = true;
		}

		if (earlyReturn == false)
		{
			if (parser.programSet())
			{
				if (programDevice(parser) == false)
					exitCode = 1;
			}
		}
	}
	else
	{
		QString helpText = parser.helpText();
		std::cout << helpText.toLatin1().data();
	}

	return exitCode;
}
