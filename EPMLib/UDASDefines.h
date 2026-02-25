#ifndef UDASDEFINES_H
#define UDASDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include <QtGlobal>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define UDAS_MAX_FILE_NAME_LEN        13

#define UDAS_WAVEFORM_UNIT_V  1
#define UDAS_WAVEFORM_UNIT_MV 0.001
#define UDAS_WAVEFORM_UNIT_A  1
#define UDAS_WAVEFORM_UNIT_MA 0.001

#define UDAS_NO_OFFSET                  0
#define UDAS_DUMMY_CONVERT_CLOCK_PERIOD 0.000005

const double US_PER_MS{1000.0};
const double MS_PER_S{1000.0};

const quint32 kUDASHeaderVersionStrLen{2};
const quint32 kUDASHeaderLen{0x400}; // 1024
const int kDateFieldLength(64);
const int kTimeFieldLength(64);
const int kLabelFieldLength(64);
const int kTargetFieldLength(64);
const int kCommentFieldLength(512);

const quint32 kUDASBlockEOFMarker{0x270F}; // 9999
const quint32 kUDASBlockTypeStandardWaveform{1};
const int kSeriesNameLength(64);

#endif // UDASDEFINES_H
