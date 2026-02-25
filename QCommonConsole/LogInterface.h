#ifndef LOGS_H
#define LOGS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// Qt
#include <QByteArray>
#include <QMap>
#include <QMutex>
#include <QSharedPointer>
#include <QTextStream>

class _Category;

typedef QSharedPointer<_Category> Category;
typedef QMap<quint32, Category>  Categories;

class _LogInterface
{
public:
	enum DebugLevel
	{
		None = 0x00,
		Informative = 0x01,
		Warning = 0x02,
		Critical = 0x04,
		Fatal = 0x08
	};
	Q_DECLARE_FLAGS(DebugLevels, DebugLevel)

	_LogInterface();
	virtual ~_LogInterface();

	virtual bool Open() = 0;
	virtual void Close() = 0;

	// Category Management
	quint32 AddCategory(const QByteArray& categoryName);
	QStringList CategoryNames();
	QByteArray CategoryName(quint32 categoryID);
	bool GetDebugLevels(quint32 categoryID, bool& informative, bool& warning, bool& critical, bool& fatal);
	bool SetDebugLevels(quint32 categoryID, bool& informative, bool& warning, bool& critical, bool& fatal);
	
	void Log(quint32 categoryID, DebugLevel debugLevel, const QString& message, bool newLine = true);
	void Log(quint32 categoryID, DebugLevel debugLevel, const QByteArray& message, bool newLine = true);
	void LogInHex(quint32 categoryID, DebugLevel debugLevel, const QByteArray& message, bool newLine = true);

protected:
	QMutex						_mutex;
	quint32						_nextCategoryID;
	QTextStream					_debugStream;
	Categories					_categories;
	Categories					_activeCategories;
};

typedef QSharedPointer<_LogInterface> LogInterface;

class LogHelper
{
public:
	LogHelper(const QByteArray& categoryName) :
		_categoryName(categoryName),
		_categoryID(0)
	{
	}

	void SetLogInterface(LogInterface logInterface)
	{
		if (logInterface.isNull() == false)
		{
			_logInterface = logInterface;
			
			_categoryID = _logInterface->AddCategory(_categoryName);
		}
	}
		
	void Log(_LogInterface::DebugLevel debugLevel, const QString& message, bool newLine = true)
	{
		if (_logInterface.isNull() == false)
			_logInterface->Log(_categoryID, debugLevel, message.toLatin1(), newLine);
	}

	void Log(_LogInterface::DebugLevel debugLevel, const QByteArray& message, bool newLine = true)
	{
		if (_logInterface.isNull() == false)
			_logInterface->Log(_categoryID, debugLevel, message, newLine);
	}

	void LogInHex(_LogInterface::DebugLevel debugLevel, const QByteArray& message, bool newLine = true)
	{
		if (_logInterface.isNull() == false)
			_logInterface->LogInHex(_categoryID, debugLevel, message, newLine);
	}

private:
	LogInterface				_logInterface;
	QByteArray					_categoryName;
	quint32						_categoryID;
};
#endif // LOGS_H
