#ifndef EPMCONFIGFILE_H
#define EPMCONFIGFILE_H
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
// Copyright 2018-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPMLib
#include "EPMChannel.h"
#include "EPMGlobalLib.h"
#include "KratosMapping.h"

// QCommonConsole
#include "BasicColor.h"
#include "SPMVersion.h"

// Qt
#include <QByteArray>
#include <QJsonDocument>
#include <QList>
#include <QString>

const QByteArray kEPMConfigPath(QByteArrayLiteral("lastEPMPath"));
const QByteArray kLastImportPath(QByteArrayLiteral("lastImportPath"));

typedef QMap<HashType, BasicColor> SeriesColorMap;
typedef QMap<QString, BasicColor> NameToColorMap;

class EPMLIB_EXPORT EPMConfigFile
{
public:
	EPMConfigFile();

	quint32 buildChannelsFromConfiguration(EPMChannels& runtimeChannels);
	void buildPRNFile(const QString& targetPath, const QString& targetFileName);

	bool open(const QString& filePath, KratosMapping kratosMapping);
	void close();
	QString errorText();

	bool isDirty();

	void save(const QString& filePath);

	KratosMapping getKratosMappping();

	// Data
	QString author();
	void setAuthor(const QString& author);

	QString recordingAuthor();
	void setRecordingAuthor(const QString& author);

	QString target();
	void setTarget(const QString& target);

	QString label();
	void setLabel(const QString& label);

	QString description();
	void setDescription(const QString& description);

	SPMVersion spmVersion();
	void setSpmVersion(SPMVersion spmVersion);
	QString spmVersionAsText();

	quint32 channelCount();

	int getColorHashes(QList<HashType>& hashes);
	BasicColor getColor(HashType hash);

	BasicColor nextColor();

	EPMChannel at(quint32 channelIndex);
	void addChannel(const EPMChannel& addMe);
	void updateChannel(quint32 channelIndex, const EPMChannel& epmChannel);
	void removeChannel(quint32 channelIndex);
	void sort(_EPMChannel::FieldPosition column);

	QStringList getCategories();

	EPMChannel getCurrentChannelByChannelHash(HashType hash);
	EPMChannel getCurrentChannelBySeriesHash(HashType hash);

	EPMChannel getVoltageChannelByChannelHash(HashType hash);
	EPMChannel getVoltageChannelBySeriesHash(HashType hash);

	EPMChannel getMarkerChannelByChannelHash(HashType hash);
	EPMChannel getMarkerChannelBySeriesHash(HashType hash);

private:
	bool read();
	void readAChannel(QJsonObject& channelObject);

	void write();
	void writeAChannel(QJsonObject& channelObject, const EPMChannel& channel);

	void populateReservedColors();

	KratosMapping				_kratosMapping;
	bool						_dirty{false};
	QString						_errorText;
	QJsonDocument				_document;

	QString						_author;
	QString						_recordingAuthor;

	QString						_creationDate;
	QString						_modifyDate;
	QString						_target;
	QString						_label;
	QString						_description;
	SPMVersion					_spmVersion{eAlpacaVersion};
	EPMChannels					_channels;
	EPMChannels					_currentChannels;
	EPMChannels					_voltageChannels;
	EPMChannels					_powerChannels;
	QStringList					_categories;

	quint32						_colorIndex{0};
	SeriesColorMap				_seriesColorMap;

	NameToColorMap				_reservedColors;
};

#endif // EPMCONFIGFILE_H
