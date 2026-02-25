#ifndef ALPACAVIEWERAPPLICATION_H
#define ALPACAVIEWERAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// AlpacaViewer
#include "UDASPreferences.h"
#include "EPMViewerWindow.h"

// QCommon
#include "AlpacaApplication.h"

// Qt
#include <QList>
#include <QTimer>

class EPMViewerApplication :
	public AlpacaApplication
{
Q_OBJECT

public:
    EPMViewerApplication(int& argc, char** argv);

    static EPMViewerApplication* appInstance();
	static EPMViewerWindow* createViewerWindow();

	void showAboutDialog();
	void quitViewer();

	UDASPreferences* getPreferences()
	{
		return &_preferences;
	}

public:
	void on_preferencesChanged();

protected:
	UDASPreferences		_preferences;

	virtual void shutDown();
};

#endif // ALPACAVIEWERAPPLICATION_H
