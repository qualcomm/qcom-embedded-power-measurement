#ifndef EPMCOMPILER_H
#define EPMCOMPILER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause
/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
class EPMConfigFile;
#include "EPMChannel.h"
#include "EPMGlobalLib.h"
#include "KratosMapping.h"

// Qt
#include <QJsonArray>
#include <QString>

class EPMLIB_EXPORT EPMCompiler
{
public:
	EPMCompiler(EPMConfigFile* compileMe);

	void setKratosMapping(KratosMapping kratosMapping)
	{
		_kratosMapping = kratosMapping;
	}

	bool compile(const QString& targetPath);

private:
	EPMConfigFile*				_epmConfigFile{Q_NULLPTR};
	KratosMapping				_kratosMapping;

	void compileChannel(EPMChannel channel, QJsonArray& channelArray);
	void compileAMarkerChannel(EPMChannel channel, QJsonArray& channelArray);
	void compileARCMChannel(EPMChannel channel, QJsonArray& channelArray);
	void compileASPMChannel(EPMChannel channel, QJsonArray& channelArray);
};

#endif // EPMCOMPILER_H
