#ifndef WAVEFORMS_H
#define WAVEFORMS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

enum WaveFormType
{
	eWaveFormUnset = -1,
	eWaveFormCurrent = 0,
	eWaveFormVoltage = 1,
	eWaveFormDMMCurrent = 2,
	eWaveFormPower = 3,
	eWaveFormMarker = 4
};

#endif // WAVEFORMS_H
