#ifndef EPMDEFINES_H
#define EPMDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qtu.qualcomm.com)
*/

// QCommon
#include "version.h"

// Qt
#include <QString>

const QString kAppName("EPMEditor");
const QString kAppVersion(EPM_EDITOR_VERSION);
const QString kInitialString(kAppName + ", version " + kAppVersion);

// EPM Channel Columns
const int kChartColor(0);
const int kChannelEnabled(1);
const int kTypeCol(2);
const int kIndexCol(3);
const int kNameCol(4);
const int kCategoryCol(5);
const int kResisterCol(6);
const int kDescriptionCol(7);

// Type Combo values
const int kUnsetIndex(0);
const int kTypeMarker(1);
const int kTypeRCM(2);
const int kTypeSPM(3);

const QString kEPMConfigurationGroup(kAppName);

#endif // ALPACADEFINES_H
