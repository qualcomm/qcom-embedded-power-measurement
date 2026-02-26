// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "BugWriterWindow.h"

#include "BugWriterApplication.h"

int main(int argc, char *argv[])
{
	BugWriterApplication a(argc, argv);

	BugWriterWindow w;
	w.show();
	return a.exec();
}
