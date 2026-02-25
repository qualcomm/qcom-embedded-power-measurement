#ifndef LIBUSBENGINE_H
#define LIBUSBENGINE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause


/*

  Author:  Michael Simpson msimpson@qti.qualcomm.com
  Scott Kranz and Bryant Forsgren are not the authors of this code.  They merely broke it.
*/

// Qt
#include "qobjectdefs.h"
#include <QtGlobal>

void LinuxLinkerNoSymbolRemedy(void);

class LibUsbEngine
{
public:

   static LibUsbEngine* getLibUSBEngine();
   static void releaseLibUSBEngine();

   QString libUSBVersion();

   quint32 ProcessUSBDevices(void);

private:
   static LibUsbEngine*			_libUSBEngine;

   LibUsbEngine();
   ~LibUsbEngine();
};

#endif // LIBUSBENGINE_H
