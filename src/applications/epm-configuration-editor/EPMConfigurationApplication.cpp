// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMConfigurationApplication.h"

// EPM
#include "EPMEditorDefines.h"

// QCommon
#include "AlpacaDefines.h"
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"

EPMConfigurationApplication::EPMConfigurationApplication
(
	int& argc,
	char** argv
) :
	QEPMApplication (argc, argv, kAppName, kAppVersion)
{
	setApplicationName(kProductName);
	setOrganizationName(kOrganizationName);
	setOrganizationDomain(QStringLiteral("www.qualcomm.com"));

	setupApplicationStyle();

	QSettings AlpacaSettings;

	if (isNewInstall(AlpacaSettings, kVersionGUID))
	{
		renewInstallRegistry(AlpacaSettings, kVersionGUID);
	}
}

EPMConfigurationApplication* EPMConfigurationApplication::epmConfigAppInstance()
{
	return qobject_cast<EPMConfigurationApplication*>(QCoreApplication::instance());
}

EPMConfigurationWindow* EPMConfigurationApplication::createEPMEditWindow()
{
	EPMConfigurationWindow* result;

	result = new EPMConfigurationWindow;
	//if (_runningAsServer == false)
		result->show();

	//_epmWindows.append(result);

		return result;
}

void EPMConfigurationApplication::on_preferencesChanged(const QString& group)
{
	Q_UNUSED(group)
}

void EPMConfigurationApplication::shutDown()
{
	QCoreApplication::quit();
}

