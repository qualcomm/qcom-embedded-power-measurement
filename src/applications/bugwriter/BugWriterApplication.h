#ifndef BUGWRITERAPPLICATION_H
#define BUGWRITERAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QEPMApplication.h"
#include "BugWriterPreferences.h"

class BugWriterApplication :
	public QEPMApplication
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
