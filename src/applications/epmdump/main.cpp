// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPMDev
#include "EPMDev.h"

// QCommonConsole
#include "Range.h"

// Qt
#include <QCoreApplication>

// C++
#include <iostream>

using namespace std;

const int kBufferSize(1024);

struct EPMDeviceDesc
{
	EPMDeviceDesc() = default;
	EPMDeviceDesc(const EPMDeviceDesc& copyMe) = default;
	~EPMDeviceDesc() = default;

	QString					_target;
	QString					_serial;
	QString					_uuid;
};

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	char errorString[kBufferSize];
	bool forQuts{false};

	if (argc > 1)
	{
		if (a.arguments().at(1) == "-quts")
			forQuts = true;
	}

	char temp[kBufferSize];
	EPM_RESULT result;

	result = InitializeEPMDev();
	if (result != NO_EPM_ERROR)
	{
		std::cout << "Initialization Failed" << std::endl;
		return 0;
	}

	memset(errorString, '\0', kBufferSize);

	int deviceCount(0);
	result = GetDeviceCount(&deviceCount);
	if (result == NO_EPM_ERROR)
	{
		if (deviceCount > 0)
		{
			if (!forQuts)
				cout << "Devices:" << endl;

			for (auto index : range(deviceCount))
			{
				if (GetPortData(index, temp, sizeof(temp)) > 0)
				{
					if (forQuts)
						std::cout << temp << std::endl;

					else
					{
						QStringList attributes = QString(temp).split(";");

						if (attributes.count() >= 3)
						{
							EPMDeviceDesc epmDevDesc;

							epmDevDesc._target = attributes.at(0);
							epmDevDesc._serial = attributes.at(1);
							epmDevDesc._uuid = attributes.at(2);

							cout << "   Device:" << epmDevDesc._target.toStdString() << " GUID:" <<
								epmDevDesc._uuid.toStdString() << endl;
						}
					}
				}
			}

			GetLastEPMError(errorString, kBufferSize);
		}
		else
		{
			if (strcmp(errorString, "") == 0)
			{
				cout << "No devices found." << endl;
			}
			else
			{
				cout << errorString << endl;
			}
		}
	}
}
