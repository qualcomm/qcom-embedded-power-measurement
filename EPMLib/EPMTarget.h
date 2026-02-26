#ifndef EPMTARGET_H
#define EPMTARGET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "EPMGlobalLib.h"

// Qt
#include <QString>
#include <QList>

struct EPMTargetEntry
{
	EPMTargetEntry() = default;
	EPMTargetEntry(const EPMTargetEntry& copyMe) = default;
	~EPMTargetEntry() = default;

	QString						_name;
	QString						_path;
};

typedef QList<EPMTargetEntry> EPMTargetEntryList;

class EPMLIB_EXPORT EPMTarget
{
public:
	EPMTarget() = default;
	~EPMTarget() = default;

	QString platformPath();
	bool setPlatformPath(const QString& platformPath);

	quint32 getPlatformCount();
	bool getPlatform(quint32 platformIndex, QString& name, QString& path);

private:
	QString						_path;
	EPMTargetEntryList			_platformEntryList;
};

#endif // EPMTARGET_H
