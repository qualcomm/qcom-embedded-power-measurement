// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "UsbServer.h"

//Qt
#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include <QDebug>

#if defined Q_OS_LINUX || defined Q_OS_MAC
// libusb
#include "libusb.h"
#endif

#ifdef Q_OS_MAC

struct libusb_device_transparent
{
   quint8			m_unused[118];
   unsigned long	session_data;
} __attribute__((aligned(1)));

#endif

#ifdef Q_OS_WIN
#include "Engines\WinUsbEngine.h"
#include "QPSTDevice.h"
#endif

static bool gInitialized(false);
static bool Initialize(void);

UsbServer::UsbServer()
{
   m_usbEngine = GetUsbEngine();
   m_deviceRemoveNotified = false;
   if (!m_usbEngine.isNull())
   {
      connect(m_usbEngine.data(), SIGNAL(DeviceAdded(const QString&)), this, SIGNAL(DeviceAdded(const QString&)));
	  connect(m_usbEngine.data(), SIGNAL(DeviceRemoved(const QString&)), this, SLOT(on_deviceRemovedNotify(const QString&)));
   }
}

UsbServer::~UsbServer()
{
}

void UsbServer::GetDevices
(
QStringList& devices
)
{
   return m_usbEngine->GetDevices(devices);
}

void UsbServer::on_deviceConnect
(
const QString& deviceKey
)
{
   qDebug() << "Device Added:" << deviceKey << " Thread ID:" << QThread::currentThreadId();
   emit DeviceAdded(deviceKey);
}

void UsbServer::on_deviceRemovedNotify(const QString&path)
{
	
   m_deviceRemoveNotified = true;
   emit DeviceRemoved(path);

}

void UsbServer::on_deviceDiconnect
(
const QString& deviceKey
)
{
   try
   {
      if (m_deviceRemoveNotified)
      {
         m_usbEngine->RemoveDeviceItem(deviceKey);
         m_deviceRemoveNotified = false;
         qDebug() << "Device Removed:" << deviceKey << " Thread ID:" << QThread::currentThreadId();

      }
      else
      {
         emit DeviceDisconnected(deviceKey);
         qDebug() << "Device disconnected:" << deviceKey << " Thread ID:" << QThread::currentThreadId();

      }
   }
   catch (...)
   {
      qCritical() << "Exception Occured.";
   }

   //emit DeviceRemoved(deviceKey); 
}

DiagDevice UsbServer::GetDevice
(
const QString& deviceKey
)
{
   DiagDevice result;
   try
   {
#if defined Q_OS_LINUX || defined Q_OS_MAC
      ssize_t count;
      libusb_device** devs;

      if (Initialize())
      {
         count = libusb_get_device_list(NULL, &devs);
         if (count > 0)
         {
            for (int index = 0; index < (count - 1); index++)
            {
               libusb_device* dev = devs[index];

#ifdef Q_OS_LINUX
               uint8_t portNumbers[20];
               int portNumbersSize = sizeof(portNumbers);

               portNumbersSize = libusb_get_port_numbers(dev, portNumbers, portNumbersSize);
               if (portNumbersSize != LIBUSB_ERROR_OVERFLOW)
               {
                  QString libUsbPortPath(QString::number(libusb_get_bus_number(dev)));

                  for (int index = 0; index < portNumbersSize; index++)
                     libUsbPortPath += "." + QString::number(portNumbers[index]);

                  if (deviceKey == libUsbPortPath)
                  {
                     libusb_device_descriptor desc;

                     if (libusb_get_device_descriptor(dev, &desc) >= 0)
                     {
                        result = DiagDevice(new _QUsbDevice(dev, desc, deviceKey));

                        result->connect(result.data(), SIGNAL(Disconnected(QString)), this, SLOT(on_deviceDiconnect(QString)));

                        break;
                     }
                  }
               }
#endif

#ifdef Q_OS_MAC

               libusb_device_transparent* transDev = (libusb_device_transparent*)dev;

               if (deviceKey == QString("%1").arg(transDev->session_data))
               {
                  libusb_device_descriptor desc;

                  if (libusb_get_device_descriptor(dev, &desc) >= 0)
                  {
                     UsbDeviceItem device = m_usbEngine->GetDeviceItem(deviceKey);
                     result = DiagDevice(new _QUsbDevice(dev, desc, deviceKey));

                     result->m_name = device->m_product;
                     result->connect(result.data(), SIGNAL(Disconnected(QString)), this, SLOT(on_deviceDiconnect(QString)));

                     break;
                  }
               }
#endif
            }

            libusb_free_device_list(devs, 1);
         }
      }
#endif

#ifdef Q_OS_WIN
      WinUsbEngine* winUsbEngine = static_cast<WinUsbEngine*>(m_usbEngine.data());
      if (winUsbEngine != NULL)
      {
         result = winUsbEngine->CreateDevice(deviceKey.toShort());
         if (result)
         {
            result->connect(result.data(), SIGNAL(Disconnected(QString)), this, SLOT(on_deviceDiconnect(QString)));
         }
         else
         {
            qDebug() << "Error:Create Device failed.Device Key:" << deviceKey;
         }
      }
      else
      {
         qDebug() << "Error: Windows Usb Engine not valid.";
      }
#endif
   }
   catch (...)
   {
      qCritical() << "Exception Occured.";
   }
   return result;
}

bool UsbServer::InstallDriver
(
quint16 vendorID,
quint16 productID
)
{
   Q_UNUSED(vendorID)
      Q_UNUSED(productID)

#ifdef Q_OS_LINUX
      return InstallLinuxDriver(vendorID);
#else
      return true;
#endif
}

#ifdef Q_OS_LINUX
#include <QProcess>

const Char* kUdevWriterScript = "udev-writer.sh";

// Linux 2.6 and greater used udev for access control.  This takes an admin priviledge.
bool UsbServer::InstallLinuxDriver
(
quint32 vendorID
)
{
   bool result(false);
   QString password;

   if (vendorID > 0 && RetrieveAdminPassword(password))
   {
      QString udevFile = QString("99-Vendor%1.rules").arg(vendorID, 4, 16, QChar('0'));
      QString udevPath = QString("/etc/udev/rules.d/") + udevFile;
      QString udevRule = QString("SUBSYSTEM==\"usb\", ATTR{idVendor}==\"%1\", MODE=\"066\", GROUP=\"plugdev\"").arg(vendorID, 4, 16, QChar('0'));

      QProcess sudoProcess;

      sudoProcess.setProcessChannelMode(QProcess::MergedChannels);
      sudoProcess.start(QString("./") + kUdevWriterScript, QStringList() << password << udevRule << udevFile << udevPath);

      if (sudoProcess.waitForFinished())
      {
         QByteArray processResults = sudoProcess.readAll();

         if (processResults.contains("root"))
            result = true;
      }

      sudoProcess.close();
   }

   return result;
}

#endif

bool Initialize()
{
   if (!gInitialized)
   {
#ifdef Q_OS_LINUX
      QString udevScriptPath = QCoreApplication::applicationDirPath() + QDir::separator() + kUdevWriterScript;

      if (!QFile(udevScriptPath).exists())
      {
         QFile resourceFile(":/config/udev-writer.sh");

         if (resourceFile.open(QIODevice::ReadOnly))
         {
            QByteArray scriptContents = resourceFile.readAll();

            QFile scriptFile(udevScriptPath);

            if (scriptFile.open(QIODevice::WriteOnly))
            {
               scriptFile.write(scriptContents);
               scriptFile.close();

               if (scriptFile.setPermissions((QFile::Permissions) 0x7777))
                  gInitialized = true;
            }

            resourceFile.close();
         }
      }
      else
#endif
         gInitialized = true;
   }

   return gInitialized;
}

