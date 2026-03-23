#ifndef PLATFORMID_H
#define PLATFORMID_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include "DebugBoardType.h"

#include <QMap>
#include <QSharedPointer>
#include <QString>

typedef quint32 PlatformID;

const PlatformID MICRO_EPM_BOARD_ID_UNKNOWN{0};

struct _PlatformEntry
{
	_PlatformEntry() {}
	_PlatformEntry(PlatformID platformID, DebugBoardType boardType, const QString& description, const QString& path = QString(), const QByteArray& usbDescriptor = QByteArray());
	~_PlatformEntry() = default;

	PlatformID					_platformID{0};
	DebugBoardType				_boardtype{eUnknownDebugBoard};
	QString						_description;
	QByteArray					_usbDescriptor;
	QString						_path;
};

typedef QSharedPointer<_PlatformEntry> PlatformEntry;
typedef QList<PlatformEntry> PlatformIDList;
typedef QMap<PlatformID, PlatformEntry> PlatformIDs;
typedef PlatformIDs::const_iterator PlatformIDIterator;

class QCOMMONCONSOLE_EXPORT PlatformContainer
{
public:
	PlatformContainer() = delete;
	PlatformContainer(const PlatformContainer& copyMe) = delete;
	~PlatformContainer() = delete;

	static void initialize();
	static PlatformIDList getEntries();
	static void addEntry(PlatformEntry platformEntry);

	static QString toString(PlatformID platformID);

	static PlatformID fromUSBDescriptor(const QByteArray& usbDescriptor);

	static DebugBoardType getDebugBoardType(PlatformID platformID);
	static PlatformIDList getDebugBoards();
	static PlatformIDList getDebugBoardsOfType(DebugBoardType debugBoardType);

private:
	static void initializeDynamic();

	static PlatformIDs			_platformIds;

};

#endif // PLATFORMID_H
