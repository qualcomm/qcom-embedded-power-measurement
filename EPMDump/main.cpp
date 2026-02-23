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
// Copyright 2013-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

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
