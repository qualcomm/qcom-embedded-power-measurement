#ifndef EPMAPPLICATION_H
#define EPMAPPLICATION_H
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
