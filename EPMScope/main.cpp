// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

//  EPMScope
#include "EPMScopeApplication.h"

// QCommon
#include "QCommon.h"

int main
(
	int argc,
	char* argv[]
)
{
	EPMScopeApplication a(argc, argv);

	InitializeQCommon();

	EPMScopeApplication::createScopeWindow();

	return a.exec();
}
