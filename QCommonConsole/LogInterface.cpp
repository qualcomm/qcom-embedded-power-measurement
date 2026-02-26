// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "LogInterface.h"

// QCommon
#include "Hexify.h"

class _Category
{
public:
	_Category()
	{

	}

	_Category(const _Category& copyMe)
	{
		_categoryName = copyMe._categoryName;
		_debugLevels = copyMe._debugLevels;
	}

	QByteArray					_categoryName;
	_LogInterface::DebugLevels	_debugLevels;
};


_LogInterface::_LogInterface() :
	_nextCategoryID(1)
{

}

_LogInterface::~_LogInterface()
{
	_categories.clear();
	_activeCategories.clear();
}

quint32 _LogInterface::AddCategory
(
	const QByteArray& categoryName
)
{
	quint32 categoryID(-1);

	QMutexLocker lock(&_mutex);

	Categories::iterator category = _categories.begin();
	while (category != _categories.end())
	{
		if ((*category)->_categoryName == categoryName)
		{
			categoryID = category.key();
			break;
		}
	}
	if (categoryID == -1)
	{
		categoryID = _nextCategoryID++;
		Category category = Category(new _Category);
		category->_categoryName = categoryName;
		_categories.insert(categoryID, category);
	}

	return categoryID;
}

QStringList _LogInterface::CategoryNames(void)
{
	QStringList result;

	return result;
}

QByteArray _LogInterface::CategoryName(quint32 categoryID)
{
	QByteArray result;

	return result;
}

bool _LogInterface::GetDebugLevels
(
	quint32 categoryID, 
	bool& informative, 
	bool& warning, 
	bool& critical, 
	bool& fatal
)
{
	bool result(false);

	return result;
}

bool _LogInterface::SetDebugLevels
(
	quint32 categoryID, 
	bool& informative, 
	bool& warning, 
	bool& critical, 
	bool& fatal
)
{
	bool result(false);

	return result;
}

void _LogInterface::Log
(
	quint32 categoryID, 
	DebugLevel debugLevel, 
	const QString& message,
	bool newLine // = true
)
{
   Log(categoryID, debugLevel, message.toLatin1(), newLine);
}

void _LogInterface::Log
(
	quint32 categoryID, 
	DebugLevel debugLevel, 
	const QByteArray& message,
	bool newLine // = true
)
{
	QMutexLocker lock(&_mutex);

	if (_debugStream.device() != nullptr)
	{
	}
}

void _LogInterface::LogInHex
(
	quint32 categoryID, 
	DebugLevel debugLevel, 
	const QByteArray& message,
	bool newLine // = true
)
{
   Log(categoryID, debugLevel, Hexify(message), newLine);
}
