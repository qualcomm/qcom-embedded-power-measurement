// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
			Michael Simpson (msimpson@qti.qualcomm.com)
*/

// UDASDev
#include "../UDASDev.h"

// Qt
#include <QByteArray>
#include <QDir>
#include <QList>

// C++
#include <iostream>

// libEPM
#include "WaveForms.h"

int main(int argc, char* argv[])
{
	Q_UNUSED(argc)
	Q_UNUSED(argv)

	UDAS_RESULT udasResult;

	udasResult = InitializeUDASDev();
	if (udasResult != NO_UDAS_ERROR)
	{
		std::cout << "Initialization Failed" << std::endl;
		return 0;
	}

	char temp[1024];

	udasResult = GetAlpacaVersion(temp, sizeof(temp));
	if (udasResult == NO_UDAS_ERROR)
		std::cout << "Alpaca Version: " << temp << std::endl;

	udasResult = GetEPMVersion(temp, sizeof(temp));
	if (udasResult == NO_UDAS_ERROR)
		std::cout << "TAC Version: " << temp << std::endl;

	std::cout << "Attempting to open the results path..." << std::endl;

	UDAS_FILE_HANDLE handle{UDAS_BAD_UDAS_HANDLE};
	QFileInfo prnFile("C:\\Users\\anmojais\\Documents\\Alpaca\\Results\\CRD8380X2_2025_25_04_00_55_40\\CRD8380X2_2025_25_04_00_55_40.prn");

	if (prnFile.exists())
	{
		QString path = prnFile.filePath();
		handle = OpenResults(path.toStdString().c_str());
		if (handle == UDAS_BAD_UDAS_HANDLE)
			std::cout << "Error occured while setting the results path to " << path.toStdString().c_str() << std::endl;
	}
	else
		std::cout << prnFile.filePath().toStdString() << " file does not exist." << std::endl;

	ChannelCount channelCount{0};
	udasResult = GetChannelCount(handle, &channelCount);
	std::cout << "Count of collected channels: " << channelCount << std::endl;

	if (udasResult == NO_UDAS_ERROR)
	{
		long long actualSize(0);
		udasResult = GetTimeSeries(handle, 0, WaveFormType::eWaveFormCurrent + 1, temp, 1024, &actualSize);

		if (udasResult == NO_UDAS_ERROR && actualSize > 0)
		{
			char* buffer = new char[actualSize];
			udasResult = GetTimeSeries(handle, 0, WaveFormType::eWaveFormCurrent + 1, buffer, actualSize, &actualSize);

			delete[] buffer;
		}

		if(udasResult != NO_UDAS_ERROR)
			std::cout << "[GetTimeSeries]: UDAS error code: " << udasResult << std::endl;
	}


	if (udasResult == NO_UDAS_ERROR)
	{
		DataCount dataCount{INT_MAX};
		for (ChannelIndex idx{0}; idx<channelCount; idx++)
		{
			udasResult = GetCurrentChannelDataCount(handle, idx, &dataCount);
			if (udasResult == NO_UDAS_ERROR)
			{
				std::cout << "Current data-points: " << dataCount << std::endl;
				for (DataCount dataPoint{0}; dataPoint<dataCount; dataPoint++)
				{
					double currentValue{0};
					GetCurrentChannelData(handle, idx, dataPoint, &currentValue);
					std::cout << "      Current value at index '" << dataPoint << "' is '" << currentValue << "'\n";
				}
			}
			else
				std::cout << "[GetCurrentChannelDataCount]: UDAS error code: " << udasResult << std::endl;

			udasResult = GetVoltageChannelDataCount(handle, idx, &dataCount);
			if (udasResult == NO_UDAS_ERROR)
			{
				std::cout << "Voltage data-points: " << dataCount << std::endl;
				for (DataCount dataPoint{0}; dataPoint<dataCount; dataPoint++)
				{
					double voltageValue{0};
					GetVoltageChannelData(handle, idx, dataPoint, &voltageValue);
					std::cout << "      Voltage value at index '" << dataPoint << "' is '" << voltageValue << "'\n";
				}
			}
			else
				std::cout << "[GetVoltageChannelDataCount]: UDAS error code: " << udasResult << std::endl;

			udasResult = GetPowerChannelDataCount(handle, idx, &dataCount);
			if (udasResult == NO_UDAS_ERROR)
			{
				std::cout << "Power data-points: " << dataCount << std::endl;
				for (DataCount dataPoint{0}; dataPoint<dataCount; dataPoint++)
				{
					double powerValue{0};
					GetPowerChannelData(handle, idx, dataPoint, &powerValue);
					std::cout << "      Power value at index '" << dataPoint << "' is '" << powerValue << "'\n";
				}
			}
			else
				std::cout << "[GetPowerChannelDataCount]: UDAS error code: " << udasResult << std::endl;
		}
	}

	if (udasResult == NO_UDAS_ERROR)
	{
		for (ChannelIndex idx{0}; idx<channelCount; idx++)
		{
			GetChannelName(handle, idx, temp, sizeof(temp));
			std::cout << "Series name at idx " << idx << " is :" << temp << std::endl;

			double avg{0};
			GetWaveformAvg(handle, idx, WaveFormType::eWaveFormCurrent + 1, &avg, 1, 2);
			std::cout << "        Average current is: " << avg << std::endl;

			double duration{0};
			GetWaveformDuration(handle, idx, WaveFormType::eWaveFormCurrent + 1, &duration, 1, 2);
			std::cout << "        Current duration is: " << duration << std::endl;

			double area{0};
			GetWaveformArea(handle, idx, WaveFormType::eWaveFormCurrent + 1, &area, 1, 2);
			std::cout << "        Area of current duration is: " << duration << std::endl;

			double populationStdDev{0};
			GetWaveformPStdDev(handle, idx, WaveFormType::eWaveFormCurrent + 1, &populationStdDev, 1, 2);
			std::cout << "        Current population standard deviation is: " << populationStdDev << std::endl;

			double max{0};
			GetWaveformMax(handle, idx, WaveFormType::eWaveFormCurrent + 1, &max, 1, 2);
			std::cout << "        Maximum current is: " << max << std::endl;

			double range{0};
			GetWaveformRange(handle, idx, WaveFormType::eWaveFormCurrent + 1, &range, 1, 2);
			std::cout << "        Current range is: " << range << std::endl;

			double min{0};
			GetWaveformMin(handle, idx, WaveFormType::eWaveFormCurrent + 1, &min, 1, 2);
			std::cout << "        Minimum current is: " << min << std::endl;
		}
	}

	if (udasResult == NO_UDAS_ERROR)
	{
		QString configFilePath = "C:/Users/anmojais/Documents/Alpaca/Export Configurations/CRD8380X2_XXXX.ecnf";

		QFile file(configFilePath);
		if (file.exists())
		{
			udasResult = ExportAsCSV(handle, configFilePath.toStdString().data());
			if (udasResult == NO_UDAS_ERROR)
			{
				udasResult = GetExportPath(handle, temp, sizeof(temp));
				if (udasResult == NO_UDAS_ERROR)
					std::cout << "Get export result folder location: " << temp << std::endl;
			}
			else
			{
				std::cout << "Error: Failed to export the EPM data in CSV format: " << udasResult << std::endl;
			}
		}
		else
		{
			std::cout << "Error: Export configuration file does not exist!";
		}
	}

	return 0;
}
