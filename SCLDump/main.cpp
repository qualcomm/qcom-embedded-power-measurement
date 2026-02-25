// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// libEPM
#include "UDASFile.h"

// QCommon
#include "Range.h"

// Qt
#include <QCoreApplication>

// C++
#include <iostream>

using namespace std;

void printUsage()
{

}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);


	if (a.arguments().count() >= 2)
	{
		UDASFile udasFile;

		QString fileName = a.arguments().at(1);

		if (fileName.endsWith(".scl"))
		{
			if (udasFile.open(fileName) == true)
			{
				cout << "Dump of File: " << fileName.toLatin1().data() << std::endl;
				cout << "   Target: " << udasFile._target.toLatin1().data() << std::endl;
				cout << "   Duration: " << udasFile._duration << std::endl;
				cout << "   Max Current Range: " << udasFile._maxCurrentRange << std::endl;
				cout << "   Max Voltage Range: " << udasFile._maxVoltageRange << std::endl;
				cout << "   Max Power Range: " << udasFile._maxPowerRange << std::endl;
				cout << "   Date: " << udasFile._date.toLatin1().data() << std::endl;
				cout << "   Time: " << udasFile._time.toLatin1().data() << std::endl;
				cout << "   Label: " << udasFile._label.toLatin1().data() << std::endl << std::endl;

				cout << "   Current Block Count: " << udasFile.currentBlockCount() << std::endl;
				for (auto blockIndex : range(udasFile.currentBlockCount()))
				{
					UDASBlock udasBlock = udasFile.getCurrentBlock(blockIndex);
					if (udasBlock.isNull() == false)
					{
						cout << "   Block Index" << blockIndex << std::endl;
						cout << "   Channel Number " << udasBlock->_channelNumber << std::endl;
						cout << "   Channel Name " << udasBlock->channelName().toLatin1().data() << std::endl;
						cout << "   Channel Color " << udasBlock->channelColor().name().toLatin1().data() << std::endl;
						cout << "   Series Name " << udasBlock->seriesName().toLatin1().data() << std::endl;
						cout << "   Duration " << udasBlock->duration() << std::endl;
						cout << "   Data File Number " << udasBlock->_dataFileNumber << std::endl;
						cout << "   Time Between Samples " << udasBlock->_timeBetweenSamples << std::endl;
						cout << "   Time Delta From Zero " << udasBlock->_timeDeltaFromZero << std::endl;
						cout << "   Time Duration " << udasBlock->_timeDuration << std::endl;
						cout << "   Data Range " << udasBlock->_dataRange << std::endl;
						cout << "   Sample Min " << udasBlock->_minSample << std::endl;
						cout << "   Sample Max " << udasBlock->_maxSample << std::endl;
						cout << "   Sample Count " << udasBlock->_sampleCount << " Data Points: " << udasBlock->_ySeries.count() << std::endl << std::endl;
					}
				}

				cout << "   Voltage Block Count: " << udasFile.voltageBlockCount() << std::endl;
				for (auto blockIndex : range(udasFile.voltageBlockCount()))
				{
					UDASBlock udasBlock = udasFile.getVoltageBlock(blockIndex);
					if (udasBlock.isNull() == false)
					{
						cout << "   Block Index" << blockIndex << std::endl;
						cout << "   Channel Number " << udasBlock->_channelNumber << std::endl;
						cout << "   Channel Name " << udasBlock->channelName().toLatin1().data() << std::endl;
						cout << "   Channel Color " << udasBlock->channelColor().name().toLatin1().data() << std::endl;
						cout << "   Series Name " << udasBlock->seriesName().toLatin1().data() << std::endl;
						cout << "   Duration " << udasBlock->duration() << std::endl;
						cout << "   Data File Number " << udasBlock->_dataFileNumber << std::endl;
						cout << "   Time Between Samples " << udasBlock->_timeBetweenSamples << std::endl;
						cout << "   Time Delta From Zero " << udasBlock->_timeDeltaFromZero << std::endl;
						cout << "   Time Duration " << udasBlock->_timeDuration << std::endl;
						cout << "   Data Range " << udasBlock->_dataRange << std::endl;
						cout << "   Sample Min " << udasBlock->_minSample << std::endl;
						cout << "   Sample Max " << udasBlock->_maxSample << std::endl;
						cout << "   Sample Count " << udasBlock->_sampleCount << " Data Points: " << udasBlock->_ySeries.count() << std::endl << std::endl;
					}
				}

				udasFile.close();
			}
		}
		else
		{
			printUsage();
		}
	}
	else
	{
		printUsage();
	}
}
