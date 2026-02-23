#pragma once

// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright © 2013-2016 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
   $Id: //depot/DM/dev/qxdm-qt/QUsb/UsbServer.h#17 $
   $Header: //depot/DM/dev/qxdm-qt/QUsb/UsbServer.h#17 $
   $Date: 2017/08/31 $
   $DateTime: 2017/08/31 12:26:33 $
   $Change: 1000981 $
   $File: //depot/DM/dev/qxdm-qt/QUsb/UsbServer.h $
   $Revision: #17 $
   %Author: msimpson %
   */

#include "qtusb_global.h"

// Qt
#include <QObject>
#include <QTextStream>

// QtUsb
#include "Engines/UsbEngine.h"
#include "ProtocolDevice.h"
#include "QUsbDevice.h"

#ifdef Q_OS_LINUX
bool RetrieveAdminPassword(QString& password);
#endif

class QTUSBSHARED_EXPORT UsbServer :
   public QObject
{
   Q_OBJECT

public:
   UsbServer();
   ~UsbServer();

   void GetDevices(QStringList& devices);

   UsbDeviceItem GetDeviceItem(const QString& path)
   {
      return m_usbEngine->GetDeviceItem(path);
   }

   DiagDevice GetDevice(const QString& devicePath);

   void Dump(QTextStream& textStream, QUsbDevices& devices);

   static bool InstallDriver(quint16 vendorID, quint16 productID);

   void addDiagPort(const QString& portName, const QString& portLabel)
   {
      if (NULL != m_usbEngine)
      {
         m_usbEngine->addDiagPort(portName, portLabel);
      }
   }
   void removeDiagPort(const QString& portName)
   {
      if (NULL != m_usbEngine)
      {
         m_usbEngine->removeDiagPort(portName);
      }
   }

signals:
   void DeviceAdded(const QString& path);
   void DeviceRemoved(const QString& path);
   void DeviceDisconnected(const QString& path);

   public slots:
   void on_deviceConnect(const QString& path);
   void on_deviceDiconnect(const QString& path); // a device has fired a disconnect
   void on_deviceRemovedNotify(const QString& path);

private:
   UsbEngine					m_usbEngine;
   bool							m_deviceRemoveNotified;

#ifdef Q_OS_LINUX
   static bool InstallLinuxDriver(quint32 vendorID);
#endif
};
