#ifndef EPMGLOBALLIB_H
#define EPMGLOBALLIB_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#ifndef ALPACA_STATIC
	#if defined(EPMLIB_LIBRARY)
		#define EPMLIB_EXPORT Q_DECL_EXPORT
	#else
		#define EPMLIB_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define EPMLIB_EXPORT
#endif

#endif // EPMGLOBALLIB_H
