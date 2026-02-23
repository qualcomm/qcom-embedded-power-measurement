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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "EmailWriter.h"

// QCommon
#include "AppCore.h"
#include "SMTPClient.h"
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

#if DEBUG
	to = "biswroy@qti.qualcomm.com";
//	to = "msimpson@qti.qualcomm.com";
#else
	to = "alpaca.hostdev.jira@qti.qualcomm.com";
#endif

	try
	{
		SMTPClient mailClient;
		mailClient.setHost("smtphost.qualcomm.com");
		mailClient.setPort(25);

		mailClient.setCCAddresses(cc);
		mailClient.setMimeFiles(mimeFiles);

		mailClient.send(from, to, title, body);
		result = true;
	}
	catch (const SMTPClientException& e)
	{
		_lastError = e.getMessage();
		AppCore::writeToApplicationLogLine(_lastError);
	}

	return result;
}
