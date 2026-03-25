#ifndef  EPMScopeAPPLICATION_H
#define  EPMScopeAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

//  EPMScope
#include "QEPMApplication.h"
#include "EPMScopePreferences.h"
#include "EPMScopeWindow.h"

// Qt
#include <QList>
#include <QTimer>

class EPMScopeApplication :
	public QEPMApplication
{
Q_OBJECT

public:
	 EPMScopeApplication(int& argc, char** argv);

	static EPMScopeApplication* appInstance();
	static EPMScopeWindow* createScopeWindow();
	static void removeScopeWindow(EPMScopeWindow* scopeWindow);

	void showAboutDialog();
	void quitViewer();

	EPMScopePreferences* getPreferences()
	{
		return &_preferences;
	}

protected slots:
	void on__applicationTimer_timeout();

protected:
	virtual void shutDown();

private:
	static QList< EPMScopeWindow*>	_scopeWindows;
	EPMScopePreferences				_preferences;
};

#endif //  EPMScopeAPPLICATION_H
