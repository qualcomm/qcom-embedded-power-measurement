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
	// This open-source build has no configured ticket-submission backend
	// (e.g. mail relay or issue-tracker REST API). Rather than silently
	// report success without actually filing a ticket, fail explicitly so
	// the caller can direct the user to save the report to a text file
	// instead.
	_lastError = "No ticket-submission backend is configured in this build. "
	             "Use 'Save as Text File' and send the file to your support contact.";
	AppCore::writeToApplicationLogLine(_lastError);
	return false;
}
