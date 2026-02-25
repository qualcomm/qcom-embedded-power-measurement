#ifndef EMAILWRITER_H
#define EMAILWRITER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
