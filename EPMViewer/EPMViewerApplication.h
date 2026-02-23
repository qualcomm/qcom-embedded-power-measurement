#ifndef ALPACAVIEWERAPPLICATION_H
#define ALPACAVIEWERAPPLICATION_H
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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
