#ifndef EPMAPPLICATION_H
#define EPMAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPM
class EPMWindow;
#include "EPMPreferences.h"

// QCommon
#include "AlpacaApplication.h"

// Qt
#include <QList>

class EPMApplication :
	public AlpacaApplication
{
Q_OBJECT

public:
	EPMApplication(int& argc, char** argv);

	static EPMApplication* epmAppInstance();
	static EPMWindow* createEPMWindow();
	static void removeEPMWindow(EPMWindow* empWindow);

	void quitEPM();

	static EPMPreferences* getPreferences()
	{
		return &EPMApplication::epmAppInstance()->_preferences;
	}

protected:
	virtual void shutDown();

private:
	EPMPreferences				_preferences;
	static QList<EPMWindow*>	_epmWindows;
};

#endif // EPMAPPLICATION_H
