// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Author: msimpson

#include "QCommonConsole.h"

#include "PlatformID.h"

// QT
#include <QtGlobal>

void InitializeQCommonConsole()
{
	Q_INIT_RESOURCE(QCommonConsole);

	PlatformContainer::initialize();
}
