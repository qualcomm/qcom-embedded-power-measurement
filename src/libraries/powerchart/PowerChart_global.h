// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef POWERCHART_GLOBAL_H
#define POWERCHART_GLOBAL_H

#ifndef QEPM_STATIC
	#if defined(POWERCHART_LIBRARY)
		#define POWERCHART_EXPORT Q_DECL_EXPORT
	#else
		#define POWERCHART_EXPORT Q_DECL_IMPORT
	#endif
#else
	#define POWERCHART_EXPORT
#endif

#endif // POWERCHART_GLOBAL_H
