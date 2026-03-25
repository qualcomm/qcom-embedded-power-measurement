// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// EPM Editor
#include "EPMConfigurationApplication.h"
#include "EPMConfigurationWindow.h"

int main(int argc, char *argv[])
{
	EPMConfigurationApplication a(argc, argv);

	EPMConfigurationWindow* w = EPMConfigurationApplication::createEPMEditWindow();
	QStringList arguments = QApplication::arguments();
	if (arguments.count() > 1)
	{
		QString epmConfigFile = arguments.at(1);
		if (epmConfigFile.endsWith(".ccnf", Qt::CaseInsensitive))
			w->openFile(epmConfigFile);
	}

	w->show();

	a.quitOnLastWindowClosed();

	return a.exec();
}
