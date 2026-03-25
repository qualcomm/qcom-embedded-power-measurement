// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EmailWriter.h"

// QCommon
#include "AppCore.h"
#include "SystemInformation.h"

// Qt
#include <QMessageBox>
#include <QRegularExpression>

EmailWriter::EmailWriter()
{
	SystemInformation sysInfo;
	_author = sysInfo.userName();
}

EmailWriter::~EmailWriter()
{
}

QString EmailWriter::lastError()
{
	return _lastError;
}

QString EmailWriter::getAuthorName()
{
	return _author;
}

void EmailWriter::setAuthorName(const QString& author)
{
	_author = author;
}

bool EmailWriter::isAuthorValid()
{
	// The following regex matches:
	// Any string that begins with: slua_
	// The string: default-user
	// Any string containing numbers: b1swr0y, 4chan, 4869
	// Any string containing less than 3 characters
	static QRegularExpression re("(slua_*)|(default-user)|\\d|(^[a-zA-Z]{0,2}$)");
	return !re.match(_author).hasMatch();
}

bool EmailWriter::isEmailAddress(const QString& text)
{
	// A quick test to check if the string contains '@' in which case, it is a valid email address
	return text.contains("@");
}

bool EmailWriter::send(const QByteArray& title, const QByteArray& body, const QByteArrayList mimeFiles)
{
	bool result{false};

	QByteArrayList cc;
	QByteArray to;

	QByteArray from = _author.toLatin1() + "@qti.qualcomm.com";
	if (isEmailAddress(_author))
	from = _author.toLatin1();

	try
	{
		// TODO: Define what happens when ticket is submitted
		result = true;
	}
	catch (...)
	{
		AppCore::writeToApplicationLogLine(_lastError);
	}

	return result;
}
