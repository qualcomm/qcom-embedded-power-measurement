// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// AlpacaViewer
#include "EPMViewerApplication.h"

// QCommon
#include "QCommon.h"
#include "UILib.h"

int main
(
	int argc,
	char* argv[]
)
{
	EPMViewerApplication a(argc, argv);

	InitializeQCommon();
	InitializeUILib();

	EPMViewerWindow* alpacaViewerWindow = EPMViewerApplication::createViewerWindow();

	QStringList arguments = QApplication::arguments();
	if (arguments.count() > 1)
	{
		QString powerRunFile = arguments.at(1);
		if (powerRunFile.endsWith(".prn", Qt::CaseInsensitive))
			alpacaViewerWindow->openAndReadPowerProject(powerRunFile);
	}

	return a.exec();
}
