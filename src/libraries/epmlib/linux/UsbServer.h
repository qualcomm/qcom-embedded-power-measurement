#pragma once

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
