// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
		   Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "UDASDev.h"
#include "DevUDASCore.h"

// libEPM
#include "EPMExportConfigFile.h"
#include "UDASFile.h"
#include "WaveForms.h"

// QCommon
#include "mymemcpy.h"
#include <Range.h>
#include "version.h"

// QT
#include <QDir>
#include <QFileInfo>
#include <QList>

const UDAS_FILE_HANDLE kBadFileHandle{0};
const QByteArray kTimeValueSeparator(",");
const QByteArray kSeriesDataSeparator(";");

DevUDASCore gDevUDASCore;

struct UdasDevBlock
{
	UdasDevBlock() {};

	UdasDevBlock(const UdasDevBlock& copyMe)
	{
		_channelIndex = copyMe._channelIndex;
		_seriesName = copyMe._seriesName;
		_currentBlock = copyMe._currentBlock;
		_voltageBlock = copyMe._voltageBlock;
		_powerBlock = copyMe._powerBlock;
	}

	UdasDevBlock& operator=(const UdasDevBlock& assignMe)
	{
		// Guard self assignment
		if (this == &assignMe)
			return *this;

		_channelIndex = assignMe._channelIndex;
		_seriesName = assignMe._seriesName;
		_currentBlock = assignMe._currentBlock;
		_voltageBlock = assignMe._voltageBlock;
		_powerBlock = assignMe._powerBlock;

		return *this;
	}

	ChannelIndex				_channelIndex{static_cast<ChannelIndex>(-1)};
	QString						_seriesName;
	UDASBlock					_currentBlock;
	UDASBlock					_voltageBlock;
	UDASBlock					_powerBlock;
};

typedef QList<UdasDevBlock> UdasDevBlockList;

struct UdasFileHandle
{
	UdasFileHandle() {};
	UdasFileHandle(const UdasFileHandle& copyMe)
	{
		_udasFile = copyMe._udasFile;
		_udasBlocks.clear();

		for (const auto& udasBlock: copyMe._udasBlocks)
		{
			_udasBlocks.push_back(udasBlock);
		}
	}

	UdasFileHandle& operator=(const UdasFileHandle& assignMe)
	{
		// Guard self assignment
		if (this == &assignMe)
			return *this;

		_udasFile = assignMe._udasFile;

		_udasBlocks.clear();
		for (const auto& udasBlock: assignMe._udasBlocks)
		{
			_udasBlocks.push_back(udasBlock);
		}

		return *this;
	}

	bool valid()
	{
		return _udasFile != Q_NULLPTR;
	}

	UDASFile*								_udasFile{Q_NULLPTR};
	UdasDevBlockList						_udasBlocks;
};

quint32 gNextFileHandle{1};
QMap<UDAS_FILE_HANDLE, UdasFileHandle> gOpenFiles;

static UdasFileHandle getUdasFileHandle(UDAS_FILE_HANDLE udasFileHandle)
{
	UdasFileHandle result;

	if (gOpenFiles.find(udasFileHandle) != gOpenFiles.end())
		result = gOpenFiles[udasFileHandle];

	return result;
}

UDAS_RESULT InitializeUDASDev()
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	if (gDevUDASCore.initialize("UDAS", "1.0.0") == false)
		result = UDAS_INIT_FAILED;

	return result;
}

UDAS_RESULT GetAlpacaVersion
(
	char* alpacaVersion,
	int bufferSize
)
{
	UDAS_RESULT result{NO_UDAS_ERROR};

	QByteArray version{ALPACA_VERSION};

	if (version.size() < bufferSize)
	{
		my_memcpy(alpacaVersion, bufferSize, version.data(), version.size());
	}
	else
	{
		result = version.size();
	}

	return result;
}

UDAS_RESULT GetEPMVersion
(
	char* epmVersion,
	int bufferSize
)
{
	UDAS_RESULT result{NO_UDAS_ERROR};

	QByteArray version{TAC_VERSION};

	if (version.size() < bufferSize)
	{
		my_memcpy(epmVersion, bufferSize, version.data(), version.size());
	}
	else
	{
		result = version.size();
	}

	return result;
}

UDAS_RESULT GetLastUdasError
(
	char* lastError,
	int bufferSize
)
{
	UDAS_RESULT result{NO_UDAS_ERROR};

	QByteArray lastErrorBA = gDevUDASCore.lastError();
	if (lastErrorBA.size() < bufferSize)
	{
		my_memcpy(lastError, bufferSize, lastErrorBA.data(), lastErrorBA.size());
	}
	else
	{
		result = UDAS_BUFFER_TOO_SMALL;
	}

	return result;
}

UDAS_FILE_HANDLE OpenResults
(
	const char* powerRunFileName
)
{
	UDAS_FILE_HANDLE result{kBadFileHandle};

	QFileInfo fileInfo(powerRunFileName);

	QString epmPath = QDir::cleanPath(fileInfo.path() + QDir::separator() + "Params.scl");

	UDASFile* udasFile = new UDASFile;
	if (udasFile != Q_NULLPTR)
	{
		if (udasFile->open(epmPath) == true && udasFile->isValid())
		{
			PRNFile prnFile;

			if (prnFile.read(powerRunFileName))
				udasFile->updateWithPRN(prnFile);

			UdasFileHandle udasFileHandle;

			udasFileHandle._udasFile = udasFile;
			gOpenFiles[gNextFileHandle] = udasFileHandle;
			result = gNextFileHandle;
			gNextFileHandle++;
		}
		else
		{
			gDevUDASCore.setLastError(QString("File %1 not found.").arg(powerRunFileName).toLatin1());
			delete udasFile;
		}
	}

	return result;
}

UDAS_RESULT GetChannelCount
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelCount *channelCount
)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	*channelCount = 0;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);

	if (udasFileHandle._udasFile != Q_NULLPTR)
	{
		if (udasFileHandle._udasBlocks.empty() == true)
		{
			UDASFile* udasFile = udasFileHandle._udasFile;

			int blockCount = udasFile->currentBlockCount();

			for (const auto i: range(blockCount))
			{
				UDASBlock currentBlock = udasFile->getCurrentBlock(i);
				if (currentBlock.isNull() == false)
				{
					HashType hash = currentBlock->channelHash();
					UDASBlock voltageBlock = udasFile->getVoltageBlockByChannelHash(hash);
					UDASBlock powerBlock = udasFile->getPowerBlockByChannelHash(hash);

					UdasDevBlock udasDevBlock;

					udasDevBlock._channelIndex = i;
					udasDevBlock._seriesName = currentBlock->channelName().toLatin1();
					udasDevBlock._currentBlock = currentBlock;
					udasDevBlock._voltageBlock = voltageBlock;
					udasDevBlock._powerBlock = powerBlock;

					udasFileHandle._udasBlocks.push_back(udasDevBlock);
				}
			}

			gOpenFiles[fileHandle] = udasFileHandle;
			*channelCount = udasFileHandle._udasBlocks.count();

			ChannelIndex channelIndex = 0;
			UdasDevBlockList::iterator iter = udasFileHandle._udasBlocks.begin();
			while (iter != udasFileHandle._udasBlocks.end())
			{
				iter->_channelIndex = channelIndex;

				channelIndex++;
				iter++;
			}
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetChannelName
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelIndex channelIndex,
	char* seriesName,
	int bufferSize
)
{
	UDAS_RESULT result{0};

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);
	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<int>(channelIndex))
		{
			QByteArray strSeriesName = udasFileHandle._udasBlocks.at(channelIndex)._seriesName.toLatin1();
			if (strSeriesName.size() < bufferSize)
			{
				my_memcpy(seriesName, bufferSize, strSeriesName.data(), strSeriesName.size());
			}
			else
			{
				result = UDAS_BUFFER_TOO_SMALL;
			}
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetChannelIndex
(
	UDAS_FILE_HANDLE fileHandle,
	char* seriesName,
	ChannelIndex* channelIndex
)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	*channelIndex = -1;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);

	if (udasFileHandle._udasFile != Q_NULLPTR)
	{
		if (udasFileHandle._udasBlocks.empty() == false)
		{
			for (const auto &block: std::as_const(udasFileHandle._udasBlocks))
			{
				if (block._seriesName.toLower() == QString(seriesName).toLower())
				{
					*channelIndex = block._channelIndex;
					break;
				}
			}
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetCurrentChannelDataCount
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelIndex channelIndex,
	DataCount* sampleCount
)
{
	UDAS_RESULT result{0};

	*sampleCount = 0;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);
	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<int>(channelIndex))
		{
			*sampleCount = udasFileHandle._udasBlocks.at(channelIndex)._currentBlock->_sampleCount;
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetCurrentChannelData
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelIndex channelIndex,
	DataIndex dataIndex,
	double* dataPoint
)
{
	UDAS_RESULT result{0};

	*dataPoint = 0.0;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);
	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<int>(channelIndex))
		{
			*dataPoint = udasFileHandle._udasBlocks.at(channelIndex)._currentBlock->seriesValue(dataIndex);
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetVoltageChannelDataCount
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelIndex channelIndex,
	DataCount* sampleCount
)
{
	UDAS_RESULT result{0};
	*sampleCount = 0;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);
	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<int>(channelIndex))
		{
			UDASBlock voltageBlock = udasFileHandle._udasBlocks.at(channelIndex)._voltageBlock;
			if (voltageBlock.isNull() == false)
				*sampleCount = voltageBlock->_sampleCount;
			else
				result = UDAS_BAD_UDAS_INDEX;
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}


UDAS_RESULT GetVoltageChannelData
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelIndex channelIndex,
	DataIndex dataIndex,
	double* dataPoint
)
{
	UDAS_RESULT result{0};

	*dataPoint = 0.0;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);
	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<int>(channelIndex))
		{
			UDASBlock voltageBlock = udasFileHandle._udasBlocks.at(channelIndex)._voltageBlock;
			if (voltageBlock.isNull() == false)
				*dataPoint = voltageBlock->seriesValue(dataIndex);
			else
				result = UDAS_BAD_UDAS_INDEX;
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetPowerChannelDataCount
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelIndex channelIndex,
	DataCount* sampleCount
)
{
	UDAS_RESULT result{0};
	*sampleCount = 0;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);
	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<int>(channelIndex))
		{
			UDASBlock powerBlock = udasFileHandle._udasBlocks.at(channelIndex)._powerBlock;
			if (powerBlock.isNull() == false)
				*sampleCount = powerBlock->_sampleCount;
			else
				result = UDAS_BAD_UDAS_INDEX;
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetPowerChannelData
(
	UDAS_FILE_HANDLE fileHandle,
	ChannelIndex channelIndex,
	DataIndex dataIndex,
	double* dataPoint
)
{
	UDAS_RESULT result{0};

	*dataPoint = 0.0;

	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);
	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<int>(channelIndex))
		{
			UDASBlock powerBlock = udasFileHandle._udasBlocks.at(channelIndex)._powerBlock;
			if (powerBlock.isNull() == false)
				*dataPoint = powerBlock->seriesValue(dataIndex);
			else
				result = UDAS_BAD_UDAS_INDEX;
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT PopulateSeriesData(const UDASBlock& udasBlock, QByteArray& seriesData)
{
	UDAS_RESULT result{NO_UDAS_ERROR};

	if (udasBlock.isNull() == false)
	{
		QByteArray timeValueStr;
		for (quint32 idx{0}; idx < udasBlock->seriesCount(); idx++)
		{
			timeValueStr += QByteArray::number(udasBlock->seriesTime(idx)) + kTimeValueSeparator;
			timeValueStr += QByteArray::number(udasBlock->seriesValue(idx)) + kSeriesDataSeparator;
		}
		timeValueStr += kSeriesDataSeparator;
		seriesData.append(timeValueStr);
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetTimeSeries(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, char* dataPoints, const int dataSize, long long* actualSize)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);

	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<uint>(channelIndex))
		{
			QByteArray seriesData;

			if (waveFormType & (eWaveFormCurrent + 1))
			{
				UDASBlock currentBlock = udasFileHandle._udasBlocks.at(channelIndex)._currentBlock;
				if (currentBlock.isNull() == false)
				   PopulateSeriesData(currentBlock, seriesData);
			}

			if (waveFormType & (eWaveFormVoltage + 1))
			{
				UDASBlock voltageBlock = udasFileHandle._udasBlocks.at(channelIndex)._voltageBlock;
				if (voltageBlock.isNull() == false)
					PopulateSeriesData(voltageBlock, seriesData);
			}

			if (waveFormType & (eWaveFormPower + 1))
			{
				UDASBlock powerBlock = udasFileHandle._udasBlocks.at(channelIndex)._powerBlock;
				if (powerBlock.isNull() == false)
					PopulateSeriesData(powerBlock, seriesData);
			}

			if (seriesData.size() > dataSize)
				*actualSize = seriesData.size();
			else
				my_memcpy(dataPoints, dataSize, seriesData.data(), seriesData.size());
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetSeriesBlock(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, UDASBlock &seriesBlock)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);

	if (udasFileHandle.valid())
	{
		if (udasFileHandle._udasBlocks.count() > static_cast<uint>(channelIndex))
		{
			switch(waveFormType)
			{
				/*Matching waveFormType enum defined in python with library const unsigned int enum by incremeting 1*/
				case (eWaveFormCurrent + 1):
					seriesBlock = udasFileHandle._udasBlocks.at(channelIndex)._currentBlock;
					break;
				case (eWaveFormVoltage + 1):
					seriesBlock = udasFileHandle._udasBlocks.at(channelIndex)._voltageBlock;
					break;
				case (eWaveFormPower + 1):
					seriesBlock = udasFileHandle._udasBlocks.at(channelIndex)._powerBlock;
					break;
				default:
					result = UDAS_BAD_WAVEFORM_TYPE;
					break;
			}
		}
		else
		{
			result = UDAS_BAD_UDAS_INDEX;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetWaveformAvg(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *average, double startTime, double endTime)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UDASBlock seriesBlock{NULL};

	result = GetSeriesBlock(fileHandle, channelIndex, waveFormType, seriesBlock);
	if( result == NO_UDAS_ERROR && seriesBlock.isNull() == false)
	{
		seriesBlock->setRange(startTime, endTime);
		*average = seriesBlock->statAvg(startTime, endTime);
	}

	return result;
}

UDAS_RESULT GetWaveformDuration(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *duration, double startTime, double endTime)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UDASBlock seriesBlock{NULL};

	result = GetSeriesBlock(fileHandle, channelIndex, waveFormType, seriesBlock);
	if( result == NO_UDAS_ERROR && seriesBlock.isNull() == false)
	{
		seriesBlock->setRange(startTime, endTime);
		*duration = seriesBlock->statDuration(startTime, endTime);
	}

	return result;
}

UDAS_RESULT GetWaveformArea(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *area, double startTime, double endTime)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UDASBlock seriesBlock{NULL};

	result = GetSeriesBlock(fileHandle, channelIndex, waveFormType, seriesBlock);
	if( result == NO_UDAS_ERROR && seriesBlock.isNull() == false)
	{
		seriesBlock->setRange(startTime, endTime);
		*area = seriesBlock->statArea(startTime, endTime);
	}

	return result;
}

UDAS_RESULT GetWaveformPStdDev(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *populationStdDev, double startTime, double endTime)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UDASBlock seriesBlock{NULL};

	result = GetSeriesBlock(fileHandle, channelIndex, waveFormType, seriesBlock);
	if( result == NO_UDAS_ERROR && seriesBlock.isNull() == false)
	{
		seriesBlock->setRange(startTime, endTime);
		*populationStdDev = seriesBlock->statPopulationStdDev(startTime, endTime);
	}

	return result;
}

UDAS_RESULT GetWaveformMax(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *max, double startTime, double endTime)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UDASBlock seriesBlock{NULL};

	result = GetSeriesBlock(fileHandle, channelIndex, waveFormType, seriesBlock);
	if( result == NO_UDAS_ERROR && seriesBlock.isNull() == false)
	{
		seriesBlock->setRange(startTime, endTime);
		*max = seriesBlock->statMax(startTime, endTime);
	}

	return result;
}

UDAS_RESULT GetWaveformRange(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *range, double startTime, double endTime)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UDASBlock seriesBlock{NULL};

	result = GetSeriesBlock(fileHandle, channelIndex, waveFormType, seriesBlock);
	if( result == NO_UDAS_ERROR && seriesBlock.isNull() == false)
	{
		seriesBlock->setRange(startTime, endTime);
		*range = seriesBlock->statRange(startTime, endTime);
	}

	return result;
}

UDAS_RESULT GetWaveformMin(UDAS_FILE_HANDLE fileHandle, ChannelIndex channelIndex, const unsigned int waveFormType, double *min, double startTime, double endTime)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UDASBlock seriesBlock{NULL};

	result = GetSeriesBlock(fileHandle, channelIndex, waveFormType, seriesBlock);
	if( result == NO_UDAS_ERROR && seriesBlock.isNull() == false)
	{
		seriesBlock->setRange(startTime, endTime);
		*min = seriesBlock->statMin(startTime, endTime);
	}

	return result;
}

UDAS_RESULT SetExportPath(UDAS_FILE_HANDLE fileHandle, const char *exportDirectory)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);

	if (udasFileHandle.valid())
	{
		gDevUDASCore.setExportLocation(exportDirectory);
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDAS_RESULT GetExportPath(UDAS_FILE_HANDLE fileHandle, char *resultsPath, int bufferSize)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);

	if (udasFileHandle.valid())
	{
		QByteArray results = gDevUDASCore.exportLocation().toLatin1();

		if (results.size() < bufferSize)
		{
			my_memcpy(resultsPath, bufferSize, results.data(), results.size());
		}
		else
		{
			result = UDAS_BUFFER_TOO_SMALL;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}

UDASDEV_EXPORT UDAS_RESULT ExportAsCSV(UDAS_FILE_HANDLE fileHandle, const char *exportConfigfile)
{
	UDAS_RESULT result{NO_UDAS_ERROR};
	UdasFileHandle udasFileHandle = getUdasFileHandle(fileHandle);

	if (udasFileHandle.valid())
	{
		HashTuples hashTuples;

		EPMExportConfigFile* epmExportConfigFile{Q_NULLPTR};
		bool isOpen{false};

		if(exportConfigfile != NULL)
		{
			epmExportConfigFile = new EPMExportConfigFile;
			if(epmExportConfigFile->open(exportConfigfile))
				isOpen = true;
		}

		for (int seriesIdx = 0 ; seriesIdx < udasFileHandle._udasBlocks.count(); seriesIdx++)
		{
			HashType channelHash{kNoHash};
			HashType currentSeriesHash{kNoHash};
			HashType voltageSeriesHash{kNoHash};
			HashType powerSeriesHash{kNoHash};

			bool currentState{false}, voltageState{false}, powerState{false};

			if(udasFileHandle._udasBlocks.at(seriesIdx)._currentBlock != Q_NULLPTR)
			{
				currentSeriesHash = udasFileHandle._udasBlocks.at(seriesIdx)._currentBlock->seriesHash();
				channelHash = udasFileHandle._udasBlocks.at(seriesIdx)._currentBlock->channelHash();
				currentState = true;
			}
			if(udasFileHandle._udasBlocks.at(seriesIdx)._voltageBlock != Q_NULLPTR)
			{
				voltageSeriesHash = udasFileHandle._udasBlocks.at(seriesIdx)._voltageBlock->seriesHash();
				channelHash = udasFileHandle._udasBlocks.at(seriesIdx)._voltageBlock->channelHash();
				voltageState = true;
			}

			if(isOpen)
			{
				if(currentSeriesHash != kNoHash)
					currentState = epmExportConfigFile->get(currentSeriesHash)._active;

				if(voltageSeriesHash != kNoHash)
					voltageState = epmExportConfigFile->get(voltageSeriesHash)._active;
			}

			if(currentState && voltageState)
			{
				if(udasFileHandle._udasBlocks.at(seriesIdx)._powerBlock != Q_NULLPTR)
				{
					powerSeriesHash = udasFileHandle._udasBlocks.at(seriesIdx)._powerBlock->seriesHash();
					powerState = true;
				}
			}

			hashTuples.push_back(std::make_tuple(channelHash, currentState ? currentSeriesHash : 0, voltageState ? voltageSeriesHash : 0, powerState ? powerSeriesHash : 0));
		}

		if(udasFileHandle._udasFile->isValid())
			udasFileHandle._udasFile->exportAsCVS(gDevUDASCore.exportLocation(), hashTuples);

		if (epmExportConfigFile != Q_NULLPTR)
		{
			if(isOpen)
				epmExportConfigFile->close();

			delete epmExportConfigFile;
			epmExportConfigFile = Q_NULLPTR;
		}
	}
	else
	{
		result = UDAS_BAD_UDAS_HANDLE;
	}

	return result;
}
