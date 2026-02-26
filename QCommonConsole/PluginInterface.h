#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "PluginInstanceInterface.h"
#include "PropertyPageInterface.h"

// QT
#include <QtPlugin>

class SendInterface;

class PluginInterface
{
public:
	virtual QString Name(void) = 0;
	virtual bool Init(const QString& pluginPath, bool resetSettings) = 0;
	
	virtual PluginInstanceInterface* Create() = 0;
	virtual void Destroy(QObject* destroyMe) = 0;

	virtual PropertyPageInterface* GetPropertyPageInterface() = 0;
	virtual void ReleasePropertyPageInterface(PropertyPageInterface* ppI) = 0;
};

#define PluginInterface_iid "com.qualcomm.simpson.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)

#endif // PLUGININTERFACE_H
