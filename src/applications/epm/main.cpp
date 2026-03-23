// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMApplication.h"
#include "EPMWindow.h"
#include "QCommon.h"

int main(int argc, char *argv[])
{
	EPMApplication a(argc, argv);

	InitializeQCommon();

	EPMWindow* w = EPMApplication::createEPMWindow();
	QStringList arguments = QApplication::arguments();
	if (arguments.count() > 1)
	{
		QString runtimeConfigFile = arguments.at(1);
		if (runtimeConfigFile.endsWith(".rcnf", Qt::CaseInsensitive))
			w->openRuntimeConfig(runtimeConfigFile);
	}

	return a.exec();
}
