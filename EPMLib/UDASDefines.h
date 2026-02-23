#ifndef UDASDEFINES_H
#define UDASDEFINES_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
