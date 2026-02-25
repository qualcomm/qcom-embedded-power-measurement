#ifndef EPMCONFIGURATIONAPPLICATION_H
#define EPMCONFIGURATIONAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// EPM Configuration Editor
#include "EPMConfigurationWindow.h"

// QCommon
#include "AlpacaApplication.h"


class EPMConfigurationApplication :
	public AlpacaApplication
{
Q_OBJECT

public:
	EPMConfigurationApplication(int& argc, char** argv);

	static EPMConfigurationApplication* epmConfigAppInstance();
	static EPMConfigurationWindow* createEPMEditWindow();

public slots:
	void on_preferencesChanged(const QString& group);

protected:
	virtual void shutDown();
};

#endif // EPMCONFIGURATIONAPPLICATION_H
