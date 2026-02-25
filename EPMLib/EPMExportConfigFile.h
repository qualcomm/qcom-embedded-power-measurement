#ifndef EPMEXPORTCONFIGFILE_H
#define EPMEXPORTCONFIGFILE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// EPMLib
#include "EPMRuntimeConfigFile.h"

class EPMLIB_EXPORT EPMExportConfigFile :
	public EPMRuntimeConfigFile
{
public:
	EPMExportConfigFile();

	void close();
};

#endif // EPMEXPORTCONFIGFILE_H
