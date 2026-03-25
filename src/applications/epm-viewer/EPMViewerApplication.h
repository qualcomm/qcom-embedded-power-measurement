#ifndef QEPMVIEWERAPPLICATION_H
#define QEPMVIEWERAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QEPMViewer
#include "UDASPreferences.h"
#include "EPMViewerWindow.h"

// QCommon
#include "QEPMApplication.h"

// Qt
#include <QList>
#include <QTimer>

class EPMViewerApplication :
	public QEPMApplication
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

#endif // QEPMVIEWERAPPLICATION_H
