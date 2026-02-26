// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "EFSModel.h"
#include "EFSSubSystem.h"

EFSModel::EFSModel(QObject* parent)
	: QAbstractItemModel(parent)
{

}

EFSModel::~EFSModel()
{

}

void EFSModel::SetEFSSubSystem
(
	EFSSubSystem* eFSSubSystem
)
{
	_eFSSubSystem = eFSSubSystem;
}