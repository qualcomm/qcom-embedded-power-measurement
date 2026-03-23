#ifndef EPMCONFIGURATIONAPPLICATION_H
#define EPMCONFIGURATIONAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPM Configuration Editor
#include "EPMConfigurationWindow.h"

// QCommon
#include "QEPMApplication.h"

class EPMConfigurationApplication :
	public QEPMApplication
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
