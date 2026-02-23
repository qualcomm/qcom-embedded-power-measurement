#ifndef EMAILWRITER_H
#define EMAILWRITER_H
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


// Qt
#include <QByteArray>
#include <QByteArrayList>


class EmailWriter
{
public:
	EmailWriter();
	~EmailWriter();

	QString lastError();

	QString getAuthorName();
	void setAuthorName(const QString& author);
	bool isAuthorValid();

	bool send(const QByteArray& title, const QByteArray& body, const QByteArrayList mimeFiles = QByteArrayList());

private:
	bool isEmailAddress(const QString& text);

	QString                     _author{"default-user"};
	QString                     _lastError;
};

#endif // EMAILWRITER_H
