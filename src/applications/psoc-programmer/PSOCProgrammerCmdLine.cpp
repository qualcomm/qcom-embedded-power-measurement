// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCProgrammerCmdLine.h"

// QCommon
#include "version.h"

// Qt
#include <QCoreApplication>

PSOCCommandLine::PSOCCommandLine
(
	const QStringList& args
) :
	CommandLineParser(args)
{
	QCoreApplication* instance = QCoreApplication::instance();

	addHelpLine(instance->applicationName());
	addHelpLine("Eeprom value programmer");
	addHelpLine(QString("Version: %1").arg(EEPROM_COMMAND));

	setVersion(EEPROM_COMMAND);

	addHelpLine("");
	addHelpLine();
	addHelpLine("");
	addHelpLine("Options");
	addHelpLine("   -l, list                         -- List Eeprom capable devices");
	addHelpLine("   -lp, listplatforms               -- List the known platforms");
	addHelpLine("   -gp, getprogram                  -- Get the programming of the selected device");
	addHelpLine("   -p, program                      -- Program the selected device");

	addHelpLine("");
	addHelpLine("Device selection (needed when more than one device is connected)");
	addHelpLine("   select=<text>                    -- Choose the device by USB serial, device name or GUID");
	addHelpLine("                                       as shown by -l. (i.e.) select=C90002030F162D84");
	addHelpLine("");
	addHelpLine("Parameters (optional), the program functions will uses these parameters");
	addHelpLine("   platformID=<numeric>             -- Set the platform id. (i.e.) platformID=8");
	addHelpLine("   serial=<text>                    -- Write a new serial number into the EEPROM. (i.e.) serial=foo");
	addHelpLine("                                       This does not select the device, use select= for that.");
	addHelpLine("   model=<text>                     -- Set the model number. (i.e.) model=bar");
	addHelpLine("");
	addHelpLine("   Enclose the parameter in a string if you want spaces in the value");
	addHelpLine("   Exit code is 0 on success and 1 on failure.");

	_listSet = isSet(QStringList{} << "-l" << "list");
	_listPlatformSet = isSet(QStringList{} << "-lp" << "listplatforms");
	_getProgramming = isSet(QStringList{} << "-gp" << "getprogram");
	_program = isSet(QStringList{} << "-p" << "program");

	_platformID = value("platformID");
	_serialNumber = value("serial");
	_modelName = value("model");
	_selector = value("select");
}

bool PSOCCommandLine::listSet()
{
	return _listSet;
}

bool PSOCCommandLine::listPlatformSet()
{
	return _listPlatformSet;
}

bool PSOCCommandLine::getProgramming()
{
	return _getProgramming;
}

bool PSOCCommandLine::programSet()
{
	return _program;
}

QByteArray PSOCCommandLine::serialNumber()
{
	return _serialNumber.toLatin1();
}

QByteArray PSOCCommandLine::modelName()
{
	return _modelName.toLatin1();
}

QString PSOCCommandLine::selector()
{
	return _selector.trimmed();
}

quint32 PSOCCommandLine::platformID()
{
	quint32 result{0};
	bool okay;
	quint32 temp;

	if (_platformID.isEmpty() == false)
	{
		temp = _platformID.toUInt(&okay);
		if (okay)
			result = temp;
	}

	return result;
}
