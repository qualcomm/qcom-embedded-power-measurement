#ifndef BUGWRITERAPPLICATION_H
#define BUGWRITERAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// QCommon
#include "AlpacaApplication.h"
#include "BugWriterPreferences.h"

class BugWriterApplication :
	public AlpacaApplication
{
Q_OBJECT

public:
	BugWriterApplication(int& argc, char** argv);

	static BugWriterApplication* appInstance();
	static BugWriterPreferences* getPreferences();

protected:
	virtual void shutDown();

private:
	BugWriterPreferences		_bugWriterPreferences;
};

#endif // BUGWRITERAPPLICATION_H
