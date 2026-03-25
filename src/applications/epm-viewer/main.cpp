// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QEPMViewer
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

	EPMViewerWindow* QEPMViewerWindow = EPMViewerApplication::createViewerWindow();

	QStringList arguments = QApplication::arguments();
	if (arguments.count() > 1)
	{
		QString powerRunFile = arguments.at(1);
		if (powerRunFile.endsWith(".prn", Qt::CaseInsensitive))
			QEPMViewerWindow->openAndReadPowerProject(powerRunFile);
	}

	return a.exec();
}
