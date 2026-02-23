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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "UDASFile.h"
#include "UDASDefines.h"

// QCommonConsole
#include "ConsoleApplicationEnhancements.h"
#include "CountingStream.h"
#include "CSV.h"
#include "QualcommColors.h"
#include "TickCount.h"

// Excel
# ifdef Q_OS_WINDOWS
	#include "QTExcel.h"
#endif

// Qt
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSet>

// C++
#include <algorithm>
#include <string>

using namespace std;

UDASFile::UDASFile()
{
	reset();

	_dataStream.setByteOrder(QDataStream::BigEndian);
}

void UDASFile::reset()
{
	_paramsFile.clear();
	_paramsPath.clear();

	_maxCurrentRange = 0.0;
	_maxVoltageRange = 0.0;
	_maxPowerRange = 0.0;


	_currentBlocks.clear();
	_voltageBlocks.clear();
	_powerBlocks.clear();
	_markerBlocks.clear();
}

QString UDASFile::lastErrorMessage()
{
	QString lastErrorMessage = _lastErrorMessage;

	_lastErrorMessage.clear();

	return lastErrorMessage;
}

bool UDASFile::open
(
	const QString& paramsPath
)
{
	bool result(false);

	reset();

	_paramsFile = paramsPath;
	if (_paramsFile.endsWith("params.scl", Qt::CaseInsensitive) == false)
		_paramsFile += QDir::separator() + QString("Params.scl");

	_paramsPath = QFileInfo(_paramsFile).absolutePath();

	_file.setFileName(_paramsFile);

	if (_file.open(QIODevice::ReadOnly))
	{
		_dataStream.setDevice(&_file);

		if (readDataSetHeader())
		{
			while (readABlock(_dataStream)) ;

			auto sortLambda = [] (UDASBlock& ep, UDASBlock& ep2) -> bool
			{
				return ep->_channelNumber < ep2->_channelNumber;
			};

			buildVirtualBlocks();

			std::sort(_currentBlocks.begin(), _currentBlocks.end(), sortLambda);
			std::sort(_voltageBlocks.begin(), _voltageBlocks.end(), sortLambda);
			std::sort(_powerBlocks.begin(), _powerBlocks.end(), sortLambda);

			result = true;
		}

		_file.close();
	}

	return result;
}

void UDASFile::updateWithPRN
(
	const PRNFile &prnFile
)
{
	_target = prnFile.target();

	for (const auto& currentBlock: std::as_const(_currentBlocks))
	{
		currentBlock->setChannelColor(prnFile.channelColor(currentBlock->channelHash()));
	}

	for (const auto& voltageBlock: std::as_const(_voltageBlocks))
	{
		voltageBlock->setChannelColor(prnFile.channelColor(voltageBlock->channelHash()));
	}

	for (const auto& markerBlock: std::as_const(_markerBlocks))
	{
		markerBlock->setChannelColor(prnFile.channelColor(markerBlock->channelHash()));
	}

	for (const auto& powerBlock: std::as_const(_powerBlocks))
	{
		powerBlock->setChannelColor(prnFile.channelColor(powerBlock->channelHash()));
	}
}

bool UDASFile::create
(
	const QString &paramsFile
)
{	
	bool result{true};

	reset();

	_paramsFile = paramsFile;
	_paramsPath = QFileInfo(_paramsFile).absolutePath();

	setCurrentTime();

	return result;
}

void UDASFile::addBlock
(
	UDASBlock& addMe
)
{
	if (addMe->duration() > _duration)
		_duration = addMe->duration();
	switch (addMe->_waveFormType)
	{
	case eWaveFormCurrent:
		_currentBlocks.append(addMe);
		_allBlocks.append(addMe);
		break;

	case eWaveFormVoltage:
		_voltageBlocks.append(addMe);
		_allBlocks.append(addMe);
		break;

	case eWaveFormPower:
		_powerBlocks.append(addMe);
		_allBlocks.append(addMe);
		break;

	default:
		Q_ASSERT("Bad Block Type, UDASFILE::addBlock");
	}
}

bool UDASFile::save()
{
	bool result{false};

	_file.setFileName(_paramsFile);
	if (_file.open(QIODevice::WriteOnly))
	{
		_dataStream.setDevice(&_file);

		writeDataSetHeader();
		for (const auto& currentBlock: std::as_const(_currentBlocks))
			currentBlock->write(_dataStream);

		for (const auto& voltageBlock: std::as_const(_voltageBlocks))
			voltageBlock->write(_dataStream);

		for (const auto& powerBlock: std::as_const(_powerBlocks))
			powerBlock->write(_dataStream);

		writeEOFBlock();

		_file.close();

		result = true;
	}

	return result;
}

void UDASFile::close()
{

}

void UDASFile::setExportTimeSpan
(
	qreal start,
	qreal end
)
{
	if (start < 0.)
		_exportStart = 0;
	else
		_exportStart = start;

	if (end > _duration)
		_exportEnd = _duration;
	else
		_exportEnd = end;

	if (_exportStart > _exportEnd)
	{
		std::swap(_exportStart, _exportEnd);
	}
}

void buildExportEntries
(
	ExportEntries& exportEntries,
	HashTuples hashTuples
)
{
	for (const auto& hashTuple: hashTuples)
	{
		ExportEntry exportEntry;

		exportEntry._channelHash = std::get<0>(hashTuple);
		exportEntry._currentHash = std::get<1>(hashTuple);
		exportEntry._voltageHash = std::get<2>(hashTuple);
		exportEntry._powerHash = std::get<3>(hashTuple);

		exportEntries.push_back(exportEntry);
	}
}

bool UDASFile::exportAsCVS
(
	const QString& destinationFolder,
	HashTuples& exportedSeries
)
{
	bool result{false};

	Q_UNUSED(destinationFolder)

	if (exportedSeries.isEmpty() == false)
	{
		ExportEntries exportEntries;
		UDASBlockVector exportedBlocks;

		buildExportEntries(exportEntries, exportedSeries);

		// Get the blocks to be exported
		for (const auto& exportEntry: std::as_const(exportEntries))
		{
			if (exportEntry._currentHash != kNoHash)
			{
				UDASBlock currentBlock = getCurrentBlockBySeriesHash(exportEntry._currentHash);
				if (currentBlock.isNull() == false)
					exportedBlocks.push_back(currentBlock);
			}

			if (exportEntry._voltageHash != kNoHash)
			{
				UDASBlock voltageBlock = getVoltageBlockBySeriesHash(exportEntry._voltageHash);
				if (voltageBlock.isNull() == false)
					exportedBlocks.push_back(voltageBlock);
			}

			if (exportEntry._powerHash != kNoHash)
			{
				UDASBlock powerBlock = getPowerBlockBySeriesHash(exportEntry._powerHash);
				if (powerBlock.isNull() == false)
					exportedBlocks.push_back(powerBlock);
			}
		}

		if (exportedBlocks.isEmpty() == false)
		{
			QString fileName = destinationFolder + QDir::separator() + _target + createFilenameTimeStamp() + ".csv";
			QFile csvFile(fileName);

			if (csvFile.open(QIODevice::WriteOnly) == true)
			{
				result = true;

				QTextStream textStream(&csvFile);

				QStringList csvEntries;

				csvEntries << "Start Time:" << QString(_date + " " + _time) << "Duration:" << QString::number(_duration) <<
					"Target:" << _target;
				writeCSVLine(textStream, csvEntries);

				csvEntries.clear();
				csvEntries << " " << " ";
				writeCSVLine(textStream, csvEntries);

				qreal increment = timeIncrement(exportEntries);
				if (increment == 0.0)
					increment = timeIncrement();

				qreal timeOffset{_exportStart};
				quint32 rowIndex = (_exportStart / _duration) * static_cast<qreal>(exportedBlocks.at(0)->_sampleCount);

				csvEntries.clear();
				csvEntries << "Time Offset";

				for (const auto& exportedBlock: std::as_const(exportedBlocks))
				{
					csvEntries << exportedBlock->seriesName();
				}
				writeCSVLine(textStream, csvEntries);

				while (timeOffset < _exportEnd)
				{
					csvEntries.clear();
					csvEntries << QString::number(timeOffset);

					for (const auto& exportedBlock: std::as_const(exportedBlocks))
					{
						csvEntries << QString::number(exportedBlock->seriesValue(rowIndex));
					}
					writeCSVLine(textStream, csvEntries);

					timeOffset += increment;
					rowIndex++;
				}

				csvFile.close();
			}
		}
	}

	_exportStart = 0.;
	_exportEnd = _duration;

	return result;
}

bool UDASFile::exportAsExcel
(
	const QString& destinationFolder,
	HashTuples& exportedSeries,
	bool quitOnFinish
)
{
	bool result{false};

#ifdef Q_OS_WINDOWS
	if (exportedSeries.isEmpty() == false)
	{
		ExportEntries exportEntries;

		buildExportEntries(exportEntries, exportedSeries);

		QTExcel qtExcel;

		QString templateName = QCoreApplication::applicationDirPath() + QDir::separator() + "export.xlsx";
		QString fileName = destinationFolder + QDir::separator() + _target + createFilenameTimeStamp() + ".xlsx";

		if (QFile::copy(templateName, fileName) == true)
		{
			auto fileTest = [&]()
			{
				return QFile::exists(fileName);
			};

			if (sleepUntil(fileTest, 5000))
			{
				if (qtExcel.open(fileName, true) == true)
				{
					if (qtExcel.selectWorkbook(1) == true)
					{
						if (qtExcel.selectWorksheet(1) == true)
						{
							QString reference;
							const int kHeaderRow{4};
							const int kDataRow{5};

							reference = qtExcel.produceCell("A", 1);
							qtExcel.setCellValue(reference, QString("Start Time:"));
							qtExcel.setCellHorizontalAlignment(reference, QTExcel::eHAlignRight);

							reference = qtExcel.produceCell("B", 1);
							qtExcel.setCellValue(reference, _date + " " + _time);

							reference = qtExcel.produceCell("C", 1);
							qtExcel.setCellValue(reference, QString("Duration:"));
							qtExcel.setCellHorizontalAlignment(reference, QTExcel::eHAlignRight);

							reference = qtExcel.produceCell("D", 1);
							qtExcel.setCellValue(reference, _duration);

							reference = qtExcel.produceCell("E", 1);
							qtExcel.setCellValue(reference, QString("Target:"));
							qtExcel.setCellHorizontalAlignment(reference, QTExcel::eHAlignRight);

							reference = qtExcel.produceCell("F", 1);
							qtExcel.setCellValue(reference, _target);

							reference = qtExcel.produceCell("A", kHeaderRow);
							qtExcel.setCellValue(reference, QString("Time Offset"));

							qreal increment = timeIncrement(exportEntries);
							if (increment == 0.0)
								increment = timeIncrement();

							if (increment == 0.0)
							{
								reference = qtExcel.produceCell("A", kHeaderRow);
								qtExcel.setCellValue(reference, QString("Bad Time Increment"));
							}
							else
							{
								qreal timeOffset{_exportStart};
								quint32 rowIndex = kDataRow;
								quint32 columnIndex = 0;

								while (timeOffset < _exportEnd)
								{
									reference = qtExcel.produceCell("A", rowIndex);
									qtExcel.setCellValue(reference, timeOffset);

									timeOffset += increment;
									rowIndex++;
								}

								quint32 totalElements{0};
								quint32 startElement{0};
								quint32 endElement{0};

								columnIndex = 2;
								for (const auto& exportEntry: std::as_const(exportEntries))
								{
									if (exportEntry._currentHash !=  kNoHash)
									{
										rowIndex = kHeaderRow;

										UDASBlock currentBlock = getCurrentBlockBySeriesHash(exportEntry._currentHash);
										if (currentBlock.isNull() == false)
										{
											if (totalElements == 0)
											{
												totalElements = currentBlock->_sampleCount;
												startElement = (_exportStart / _duration) * static_cast<qreal>(totalElements);
												endElement = (_exportEnd / _duration) * static_cast<qreal>(totalElements);
											}

											QString columnName = qtExcel.stringFromColumnIndex(columnIndex);
											reference = qtExcel.produceCell(columnName, rowIndex);
											qtExcel.setCellValue(reference, currentBlock->seriesName());
											columnIndex++;
											rowIndex++;

											std::span seriesSpan(currentBlock->_ySeries);

											for (const auto& seriesEntry: seriesSpan.subspan(startElement, endElement - startElement))
											{
												reference = qtExcel.produceCell(columnName, rowIndex);
												qtExcel.setCellValue(reference, seriesEntry);
												qtExcel.setCellHorizontalAlignment(reference, QTExcel::eHAlignRight);
												rowIndex++;
											}
										}
									}

									if (exportEntry._voltageHash !=  kNoHash)
									{
										rowIndex = kHeaderRow;

										UDASBlock voltageBlock = getVoltageBlockBySeriesHash(exportEntry._voltageHash);
										if (voltageBlock.isNull() == false)
										{
											if (totalElements == 0)
											{
												totalElements = voltageBlock->_sampleCount;
												startElement = (_exportStart / _duration) * static_cast<qreal>(totalElements);
												endElement = (_exportEnd / _duration) * static_cast<qreal>(totalElements);
											}

											QString columnName = qtExcel.stringFromColumnIndex(columnIndex);
											reference = qtExcel.produceCell(columnName, rowIndex);
											qtExcel.setCellValue(reference, voltageBlock->seriesName());
											columnIndex++;
											rowIndex++;

											std::span seriesSpan(voltageBlock->_ySeries);

											for (const auto& seriesEntry: seriesSpan.subspan(startElement, endElement - startElement))
											{
												reference = qtExcel.produceCell(columnName, rowIndex);
												qtExcel.setCellValue(reference, seriesEntry);
												qtExcel.setCellHorizontalAlignment(reference, QTExcel::eHAlignRight);
												rowIndex++;
											}
										}
									}

									if (exportEntry._powerHash !=  kNoHash)
									{
										rowIndex = kHeaderRow;

										UDASBlock powerBlock = getPowerBlockBySeriesHash(exportEntry._powerHash);
										if (powerBlock.isNull() == false)
										{
											if (totalElements == 0)
											{
												totalElements = powerBlock->_sampleCount;
												startElement = (_exportStart / _duration) * static_cast<qreal>(totalElements);
												endElement = (_exportEnd / _duration) * static_cast<qreal>(totalElements);
											}

											QString columnName = qtExcel.stringFromColumnIndex(columnIndex);
											reference = qtExcel.produceCell(columnName, rowIndex);
											qtExcel.setCellValue(reference, powerBlock->seriesName());
											columnIndex++;
											rowIndex++;

											std::span seriesSpan(powerBlock->_ySeries);

											for (const auto& seriesEntry: seriesSpan.subspan(startElement, endElement - startElement))
											{
												reference = qtExcel.produceCell(columnName, rowIndex);
												qtExcel.setCellValue(reference, seriesEntry);
												qtExcel.setCellHorizontalAlignment(reference, QTExcel::eHAlignRight);
												rowIndex++;
											}
										}
									}
								}

								columnIndex = (columnIndex < 6) ? 6 : columnIndex;

								qtExcel.setColumnWidths(1, columnIndex, 16.0);
							}

							qtExcel.save();
						}
					}

					if (quitOnFinish)
						qtExcel.quit();

					result = true;
				}
				else
					_lastErrorMessage = "Unable to open Excel file " + fileName;
			}
			else
				_lastErrorMessage = "Unable to open Excel file " + fileName;
		}
		else
			_lastErrorMessage = "Unable to create Excel file " + fileName;
	}

	_exportStart = 0.;
	_exportEnd = _duration;
#endif

	return result;
}

qreal UDASFile::timeIncrement
(
	ExportEntries& exportEntries
)
{
	qreal result{0.};
	quint32 count{0};

	UDASBlock entryBlock;

	for (const auto& exportEntry: exportEntries)
	{
		if (exportEntry._currentHash != kNoHash)
		{
			entryBlock = getCurrentBlockBySeriesHash(exportEntry._currentHash);
			if (entryBlock.isNull() == false)
			{
				result += entryBlock->_timeBetweenSamples;
				count++;
			}
		}

		if (exportEntry._voltageHash != kNoHash)
		{
			entryBlock = getVoltageBlockBySeriesHash(exportEntry._voltageHash);
			if (entryBlock.isNull() == false)
			{
				result += entryBlock->_timeBetweenSamples;
				count++;
			}
		}

		if (exportEntry._powerHash != kNoHash)
		{
			entryBlock = getPowerBlockBySeriesHash(exportEntry._powerHash);
			if (entryBlock.isNull() == false)
			{
				result += entryBlock->_timeBetweenSamples;
				count++;
			}
		}
	}

	if (count)
		result = result / count;

	return result;
}

qreal UDASFile::timeIncrement()
{
	qreal result{0.};

	if (_currentBlocks.count() > 0)
		result = _currentBlocks.at(0)->_timeBetweenSamples;
	else if (_voltageBlocks.count() > 0)
		result = _voltageBlocks.at(0)->_timeBetweenSamples;
	else if (_powerBlocks.count() > 0)
		result = _powerBlocks.at(0)->_timeBetweenSamples;

	return result;
}

UDASBlock UDASFile::getBlock(int index)
{
	UDASBlock result;

	if (index < _allBlocks.count())
		result = _allBlocks.at(index);

	return result;
}

UDASBlock UDASFile::getBlockBySeriesName
(
	const QString& seriesName
)
{
	UDASBlock result;

	for (const auto& currentBlock: std::as_const(_currentBlocks))
	{
		if (currentBlock->seriesName() == seriesName)
		{
			result = currentBlock;
			break;
		}
	}

	if (result.isNull())
	{
		for (const auto& voltageBlock: std::as_const(_voltageBlocks))
		{
			if (voltageBlock->seriesName() == seriesName)
			{
				result = voltageBlock;
				break;
			}
		}
	}

	if (result.isNull())
	{
		for (const auto& markerBlock: std::as_const(_markerBlocks))
		{
			if (markerBlock->seriesName() == seriesName)
			{
				result = markerBlock;
				break;
			}
		}
	}

	if (result.isNull())
	{
		for (const auto& powerBlock: std::as_const(_powerBlocks))
		{
			if (powerBlock->seriesName() == seriesName)
			{
				result = powerBlock;
				break;
			}
		}
	}

	return result;
}

UDASBlock UDASFile::getCurrentBlock
(
	int index
)
{
	UDASBlock result;

	if (index < _currentBlocks.count())
		result = _currentBlocks.at(index);

	return result;
}

UDASBlock UDASFile::getCurrentBlockByChannelHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_currentBlocks))
	{
		if (block->channelHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

UDASBlock UDASFile::getCurrentBlockBySeriesHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_currentBlocks))
	{
		if (block->seriesHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

UDASBlock UDASFile::getVoltageBlock
(
	int index
)
{
	UDASBlock result;

	if (index < _voltageBlocks.count())
		result = _voltageBlocks.at(index);

	return result;
}

UDASBlock UDASFile::getVoltageBlockByChannelHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_voltageBlocks))
	{
		if (block->channelHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

UDASBlock UDASFile::getVoltageBlockBySeriesHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_voltageBlocks))
	{
		if (block->seriesHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

UDASBlock UDASFile::getPowerBlock
(
	int index
)
{
	UDASBlock result;

	if (index < _powerBlocks.count())
		result = _powerBlocks.at(index);

	return result;
}

UDASBlock UDASFile::getPowerBlockByChannelHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_powerBlocks))
	{
		if (block->channelHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

UDASBlock UDASFile::getPowerBlockBySeriesHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_powerBlocks))
	{
		if (block->seriesHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

UDASBlock UDASFile::getMarkerBlock
(
	int index
)
{
	UDASBlock result;

	if (index < _markerBlocks.count())
		result = _markerBlocks.at(index);

	return result;
}

UDASBlock UDASFile::getMarkerBlockByChannelHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_markerBlocks))
	{
		if (block->channelHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

UDASBlock UDASFile::getMarkerBlockBySeriesHash
(
	HashType hash
)
{
	UDASBlock result;

	for (const auto& block: std::as_const(_markerBlocks))
	{
		if (block->seriesHash() == hash)
		{
			result = block;
			break;
		}
	}

	return result;
}

HashList UDASFile::buildSortedIVHashList()
{
	HashList result;
	QSet<HashType> memo;

	auto currentBlockIter = _currentBlocks.begin();
	auto voltageBlockIter = _voltageBlocks.begin();

	while (currentBlockIter != _currentBlocks.end() && voltageBlockIter != _voltageBlocks.end())
	{
		HashType hash;
		uint currentChannelNumber(0xFFFFFFFF);
		uint voltageChannelNumber(0xFFFFFFFF);

		if (currentBlockIter != _currentBlocks.end())
		{
			currentChannelNumber = (*currentBlockIter)->_channelNumber;
		}

		if (voltageBlockIter != _voltageBlocks.end())
		{
			voltageChannelNumber = (*voltageBlockIter)->_channelNumber;
		}

		if (currentChannelNumber <= voltageChannelNumber)
		{
			hash = (*currentBlockIter)->channelHash();
			if (memo.contains(hash) == false)
			{
				memo.insert(hash);
				result.push_back(hash);
			}

			currentBlockIter++;
		}
		else
		{
			hash = (*voltageBlockIter)->channelHash();
			if (memo.contains(hash) == false)
			{
				memo.insert(hash);
				result.push_back(hash);
			}

			voltageBlockIter++;
		}
	}

	return result;
}

BasicColor UDASFile::getChannelColor
(
	HashType channelHash
)
{
	auto udasBlock = getCurrentBlockByChannelHash(channelHash);
	if (udasBlock.isNull())
	{
		udasBlock = getVoltageBlockByChannelHash(channelHash);

		if (udasBlock.isNull())
		{
			udasBlock = getPowerBlockByChannelHash(channelHash);
			if (udasBlock.isNull())
			{
				udasBlock = getMarkerBlockByChannelHash(channelHash);
			}
		}
	}

	if (udasBlock.isNull() == false)
		return udasBlock->channelColor();

	return kWhite;
}

bool UDASFile::readDataSetHeader()
{
	bool result{false};

	quint8 temp[1024];
	char* tempPtr(reinterpret_cast<char*>(&temp[0]));

	quint32 versionStringLength;

	_dataStream >> versionStringLength;
	if (versionStringLength == kUDASHeaderVersionStrLen)
	{
		quint8 aChar;

		_dataStream >> aChar;
		if (aChar == 'v')
		{
			int version{0};

			_dataStream >> aChar;

			switch (aChar)
			{
			case '6':
				version = 6;
				break;

			case '7':
				version = 7;
				break;
			}

			if (version != 0)
			{
				quint32 headerInfoLength;
				_dataStream >> headerInfoLength;

				if (headerInfoLength == kUDASHeaderLen)
				{
					_dataStream.readRawData(tempPtr, kDateFieldLength);
					_date = tempPtr;

					_dataStream.readRawData(tempPtr, kTimeFieldLength);
					_time = tempPtr;

					_dataStream.readRawData(tempPtr, kLabelFieldLength);
					_label = tempPtr;
					if (_label.isEmpty())
						_label = _paramsPath;

					_dataStream.readRawData(tempPtr, kCommentFieldLength);
					_comments = QByteArray(tempPtr, kCommentFieldLength).trimmed();

					_dataStream >> _duration;
					_exportEnd = _duration;

					result = _file.seek(kUDASHeaderLen);
				}
			}
		}
	}

	return result;
}

void UDASFile::writeDataSetHeader()
{	
	quint8 temp[1024];
	int sizeOfTemp = sizeof(temp);
	char* tempPtr(reinterpret_cast<char*>(&temp[0]));

	CountingStream countingStream;

	countingStream << kUDASHeaderVersionStrLen;
	countingStream << static_cast<quint8>('6');
	countingStream << static_cast<quint8>('v');
	countingStream << kUDASHeaderLen;

	countingStream.writeRawData(Q_NULLPTR, kDateFieldLength);
	countingStream.writeRawData(Q_NULLPTR, kTimeFieldLength);
	countingStream.writeRawData(Q_NULLPTR, kLabelFieldLength);
	countingStream.writeRawData(Q_NULLPTR, kCommentFieldLength);
	countingStream << _duration;

	_dataStream << kUDASHeaderVersionStrLen;
	_dataStream << static_cast<quint8>('v');
	_dataStream << static_cast<quint8>('6');
	_dataStream << kUDASHeaderLen;

	memset(tempPtr, 0, sizeOfTemp);
	memcpy(tempPtr, _date.toLatin1().data(), _date.size());
	_dataStream.writeRawData(tempPtr ,kDateFieldLength);

	memset(tempPtr, 0, sizeOfTemp);
	memcpy(tempPtr, _time.toLatin1().data(), _time.size());
	_dataStream.writeRawData(tempPtr, kTimeFieldLength);

	memset(tempPtr, 0, sizeOfTemp);
	if (_label.isEmpty())
	{
		int indexOf;

		QFileInfo fileInfo(_paramsFile);

		QString path = fileInfo.path();
		indexOf = path.lastIndexOf('\\');
		if (indexOf != -1)
			path = path.mid(indexOf + 1);
		else
		{
			indexOf = path.lastIndexOf('/');
			if (indexOf != -1)
				path = path.mid(indexOf + 1);
			else
				path = path.right(8);
		}

		_label = path;
	}
	memcpy(tempPtr, _label.toLatin1().data(), _label.size());
	_dataStream.writeRawData(tempPtr, kLabelFieldLength);

	memset(tempPtr, 0, sizeOfTemp);
	memcpy(tempPtr, _comments.toLatin1().data(), _comments.size());
	_dataStream.writeRawData(tempPtr, kCommentFieldLength);

	_dataStream << _duration;

	memset(tempPtr, 0, sizeof(temp));
	_dataStream.writeRawData(tempPtr, kUDASHeaderLen - countingStream.count());
}

void UDASFile::writeEOFBlock()
{
	_dataStream << kUDASBlockEOFMarker;
	_dataStream << static_cast<quint32>(0);
}

void UDASFile::setCurrentTime()
{
	QDateTime now = QDateTime::currentDateTime();
	_date = now.toString("dd-MMM-yyyy");
	_time = now.toString("hh:mm:ss");
}

bool UDASFile::readABlock
(
	QDataStream& dataStream
)
{
	bool moreBlocks(true);

	if (dataStream.atEnd() == false)
	{
		quint32 blockType;
		quint32 blockLength;

		dataStream >> blockType;
		dataStream >> blockLength;

		qint64 pos = dataStream.device()->pos();

		if (blockType == kUDASBlockTypeStandardWaveform)
		{
			UDASBlock udasBlock = UDASBlock(new _UDASBlock(this));

			if (udasBlock->read(dataStream))
			{
				switch (udasBlock->_waveFormType)
				{
				case eWaveFormCurrent:
					udasBlock->_dataRange *= 1000;

					if (_maxCurrentRange < udasBlock->_dataRange)
						_maxCurrentRange = udasBlock->_dataRange;

					_currentBlocks.append(udasBlock);
					_allBlocks.append(udasBlock);

					break;

				case eWaveFormVoltage:
					if (_maxVoltageRange < udasBlock->_dataRange)
						_maxVoltageRange = udasBlock->_dataRange;

					_voltageBlocks.append(udasBlock);
					_allBlocks.append(udasBlock);
					break;

				case eWaveFormMarker:
					if (_maxPowerRange < udasBlock->_dataRange)
						_maxPowerRange = udasBlock->_dataRange;

					_markerBlocks.append(udasBlock);
					_allBlocks.append(udasBlock);
					break;

				case eWaveFormPower:
					if (_maxPowerRange < udasBlock->_dataRange)
						_maxPowerRange = udasBlock->_dataRange;

					_powerBlocks.append(udasBlock);
					_allBlocks.append(udasBlock);
					break;

				default:
					break;
				}
			}
		}
		else if (blockType == kUDASBlockEOFMarker)
		{
			moreBlocks = false;
		}
		else
		{
			moreBlocks = _file.seek(pos + blockLength);
		}
	}
	else
	{
		moreBlocks = false;
	}

	return moreBlocks;
}

void UDASFile::buildVirtualBlocks()
{
	for (auto& voltageBlock: _voltageBlocks)
	{
		HashType blockHash(voltageBlock->channelHash());

		auto currentBlock = getCurrentBlockByChannelHash(blockHash);
		if (currentBlock.isNull() == false)
		{
			UDASBlock powerBlock = UDASBlock(new _UDASBlock(voltageBlock->_parent));

			powerBlock->_channelNumber = voltageBlock->_channelNumber;
			powerBlock->_waveFormType = eWaveFormPower;
			powerBlock->setSeriesName(voltageBlock->channelName() + "_P");
			powerBlock->_timeBetweenSamples = voltageBlock->_timeBetweenSamples;
			powerBlock->_timeDeltaFromZero = voltageBlock->_timeDeltaFromZero;
			powerBlock->_clockPeriod = voltageBlock->_clockPeriod;
			powerBlock->_sampleCount = voltageBlock->_sampleCount;

			int currentCount = currentBlock->_ySeries.count();
			int voltageCount(static_cast<int>(voltageBlock->_sampleCount));

			powerBlock->_ySeries.resize(voltageCount);

			int dataIndex(0);
			while (dataIndex < voltageCount)
			{
				qreal powerValue = voltageBlock->_ySeries[dataIndex];
				if (dataIndex < currentCount)
					powerValue *= currentBlock->_ySeries[dataIndex];
				else
					powerValue = 0;

				if (powerValue < powerBlock->_minSample)
					powerBlock->_minSample = powerValue;

				if (powerValue > powerBlock->_maxSample)
					powerBlock->_maxSample = powerValue;

				powerBlock->_ySeries[dataIndex] = powerValue;

				dataIndex++;
			}

			powerBlock->_statistics.setSpan(powerBlock->_ySeries);

			powerBlock->_dataRange = powerBlock->_maxSample - powerBlock->_minSample;

			_powerBlocks.append(powerBlock);	
			_allBlocks.append(powerBlock);
		}
	}
}
