#ifndef UIGLOBALLIB_H
#define UIGLOBALLIB_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#ifndef ALPACA_STATIC
	#if defined(UILIB_LIBRARY)
		#define UILIB_EXPORT Q_DECL_EXPORT
	#else
		#define UILIB_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define UILIB_EXPORT
#endif

#endif // UIGLOBALLIB_H
