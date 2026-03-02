#ifndef LIBUSBINITIALIZER_H
#define LIBUSBINITIALIZER_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson msimpson@qti.qualcomm.com
*/

#include <QString>

void LibUsbInitialize(void);
bool LibUsbInitialized(void);
QString LibUsbVersion(void);

#endif // LIBUSBINITIALIZER_H
