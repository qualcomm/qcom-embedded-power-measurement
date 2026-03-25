// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "EPMDevice.h"

// libMicroEPM
#include "MicroEpmErrors.h"
#include "MicroEpmAPI.h"

// QCommon
#include "KratosMapping.h"
#include "Range.h"

// Qt
#include <QDateTime>
#include <QDir>
#include <QTextStream>

struct EpmImpl
{
	MicroEpmDevice*				_deviceHandle;
	MicroEpmVersionInfoType		_versionInfo;
	MicroEpmTargetInfoType		_targetInfoType;
};

static QString EPMErrorToString(MicroEpmErrorType status);

class MicroEPMInitializer
{
public:
	MicroEPMInitializer() :
		_connected(false)
	{
	}

	~MicroEPMInitializer()
	{
		disconnect();
	}

	bool connect(void)
	{
		if (_connected)
			disconnect();

		MicroEpmErrorType error;

		error = MicroEpmInit();

		if (error == MICRO_EPM_SUCCESS)
		{
			_connected = true;
		}
		else
		{
			_errorString = "MicroEpmInit() == " + EPMErrorToString(error);
		}

		return _connected;
	}

	bool disconnect(void)
	{
		if (_connected)
		{
			MicroEpmErrorType error;

			error = MicroEpmDeInit();
			if (error == MICRO_EPM_SUCCESS)
			{
				_connected = false;
			}
			else
			{
				_errorString = "MicroEpmDeInit() == " + EPMErrorToString(error);
			}
		}

		return _connected = false;
	}

	bool						_connected;
	QString						_errorString;
};

static MicroEPMInitializer gMicroEPMInitializer;

static QString MicroEpmConvertUuidToString(MicroEpmBoardUuidType uuid);

_EPMDevice::_EPMDevice() :
	_portIndex(kInvalidDevice),
	_impl(new EpmImpl)
{

}

_EPMDevice::~_EPMDevice()
{
	if (_impl != Q_NULLPTR)
	{
		delete _impl;
		_impl = Q_NULLPTR;
	}
}

EPMDeviceMap _EPMDevice::getEpmDevices
(
	QString& deviceError
)
{
	EPMDeviceMap result;

	MicroEpmErrorType status;

	if (gMicroEPMInitializer.connect() == true)
	{
		quint32 targetCount;

		status = MicroEpmConnect(&targetCount);
		if (status == MICRO_EPM_SUCCESS)
		{
			quint32 portIdx = 0;

			deviceError.clear();

			for (auto target: basic_range<quint32>(targetCount))
			{
				quint32 numModules;
				MicroEpmDevice* hDevice;

				status = MicroEpmOpenTarget(0, &numModules);
				if (status == MICRO_EPM_SUCCESS)
				{
					status = MicroEpmGetModuleHandle(target, 0, &hDevice);
					if (status == MICRO_EPM_SUCCESS)
					{
						MicroEpmTargetInfoType targetInfo;

						status = MicroEpmGetTargetInfo(hDevice, &targetInfo);
						if (status == MICRO_EPM_SUCCESS)
						{
							EPMDevice epmDevice(new _EPMDevice);

							epmDevice->_portIndex = portIdx;
							epmDevice->_port = "EPM_PORT" + QString::number(portIdx);
							epmDevice->_impl->_deviceHandle = hDevice;
							epmDevice->_name = QString(targetInfo.szModelNumber).trimmed();
							epmDevice->_serialNumber = QString(targetInfo.szSerialNumber).trimmed();
							epmDevice->_uuid = MicroEpmConvertUuidToString(targetInfo.epmBoardUuid);

							MicroEpmGetVersionInfo(hDevice, &epmDevice->_impl->_versionInfo);
							MicroEpmGetTargetInfo(hDevice, &epmDevice->_impl->_targetInfoType);

							result[portIdx] = epmDevice;

							portIdx++;
						}
						else
						{
							deviceError += "MicroEpmGetTargetInfo() == " + EPMErrorToString(status);
						}
					}
					else
					{
						deviceError += "MicroEpmGetModuleHandle() == " + EPMErrorToString(status);
					}
				}
				else
				{
					deviceError += "MicroEpmOpenTarget() == " + EPMErrorToString(status);
				}
			}
		}
		else
		{
			deviceError = "MicroEpmConnect() == " + EPMErrorToString(status);
		}
	}
	else
	{
		deviceError = gMicroEPMInitializer._errorString;
	}

	return result;
}

void _EPMDevice::setOutputDirectory
(
	const QString& outputDirectory
)
{
	_outputDirectory = outputDirectory;
	log("Output directory set to " + _outputDirectory);
}

void _EPMDevice::setLog
(
	ThreadedLog threadedLog
)
{
	_threadedLog = threadedLog;
}

void _EPMDevice::addChannel
(
	const EPMRunChannel& epmRunChannel
)
{
	MicroEpmErrorType status;
	MicroEpmDevice* devHandle(_impl->_deviceHandle);

	if (devHandle != Q_NULLPTR)
	{
		status = MicroEpmSetChannelName(devHandle, static_cast<quint32>(epmRunChannel._channel), epmRunChannel._name.toLatin1().data());
		if (status != MICRO_EPM_SUCCESS)
		{
			QString message = QString("MicroEpmSetChannelName: %1, Name = %2").arg(epmRunChannel._channel).arg(epmRunChannel._name);
			log(message + EPMErrorToString(status));
		}

		if (epmRunChannel._channelType == EPMRunChannel::eCurrent)
		{
			status = MicroEpmSetRsense(devHandle, static_cast<quint32>(epmRunChannel._channel), epmRunChannel._resistorValue);
			if (status != MICRO_EPM_SUCCESS)
			{
				QString message = QString("MicroEpmSetRsense: %1, RValue = %2").arg(epmRunChannel._channel).arg(epmRunChannel._resistorValue);
				log(message + EPMErrorToString(status));
			}
		}

		if (epmRunChannel._rcmChannel != kInvalidChannel)
		{
			status = MicroEpmSetRcmChannel(devHandle, static_cast<quint32>(epmRunChannel._channel), static_cast<quint32>(epmRunChannel._rcmChannel));
			if (status != MICRO_EPM_SUCCESS)
			{
				QString message = QString("MicroEpmSetRcmChannel: %1, RCM Channel = %2").arg(epmRunChannel._channel).arg(epmRunChannel._rcmChannel);
				log(message + EPMErrorToString(status));
			}
		}

		log(QString("Added Channel: %1-%2").arg(epmRunChannel._name).arg(epmRunChannel._channel));

		_runChannels[static_cast<quint32>(epmRunChannel._channel)] = epmRunChannel;
	}
	else
	{
		log("_EPMDevice::addChannel: devHandle != Q_NULLPTR");
	}
}

void _EPMDevice::removeChannel
(
	const EPMRunChannel& epmRunChannel
)
{
	MicroEpmErrorType status;
	MicroEpmDevice* devHandle(_impl->_deviceHandle);

	if (devHandle != Q_NULLPTR)
	{
		status = MicroEpmSetChannelEnable(devHandle, static_cast<quint32>(epmRunChannel._channel), MICRO_EPM_CHANNEL_DISABLE);
		if (status != MICRO_EPM_SUCCESS)
		{
			QString message = QString("MicroEpmSetChannelEnable: %1, MICRO_EPM_CHANNEL_DISABLE").arg(epmRunChannel._channel);
			log(message + EPMErrorToString(status));
		}
		else
		{
			log(QString("Removed Channel: %1-%2").arg(epmRunChannel._name).arg(epmRunChannel._channel));
		}

		_runChannels.remove(static_cast<quint32>(epmRunChannel._channel));
	}
}

void _EPMDevice::acquire()
{
	MicroEpmErrorType status;
	MicroEpmDevice* devHandle(_impl->_deviceHandle);

	_results.clear();

	if (devHandle != Q_NULLPTR)
	{
		status = MicroEpmSetTimestamp(devHandle, 0);
		if (status != MICRO_EPM_SUCCESS)
		{
			QString message = QString("MicroEpmSetTimestamp: ") + EPMErrorToString(status);
			log(message);

			throw (message);
		}

		for (const auto& runChannel: _runChannels)
		{
			status = MicroEpmSetChannelEnable(devHandle, runChannel._channel, MICRO_EPM_CHANNEL_ENABLE);
			if (status != MICRO_EPM_SUCCESS)
			{
				QString message = QString("MicroEpmSetChannelEnable: %1, MICRO_EPM_CHANNEL_ENABLE ").arg(runChannel.key()) + EPMErrorToString(status);
				log(message);

				throw message;
			}
		}

		status = MicroEpmStartRecording(devHandle, MICRO_EPM_RECORDING_FORMAT_UDAS, _outputDirectory.toLatin1().data());
		if (status != MICRO_EPM_SUCCESS)
		{
			QString message = QString("MicroEpmStartRecording: %1, MICRO_EPM_RECORDING_FORMAT_UDAS ").arg(_outputDirectory) + EPMErrorToString(status);
			log(message);

			throw (message);
		}

		status = MicroEpmApplySettings(devHandle);
		if (status != MICRO_EPM_SUCCESS)
		{
			QString message = QString("MicroEpmApplySettings: ") + EPMErrorToString(status);
			log(message);

			throw message;
		}

		status = MicroEpmStartAcquisition(devHandle);
		if (status != MICRO_EPM_SUCCESS)
		{
			QString message = QString("MicroEpmStartAcquisition: ") + EPMErrorToString(status);
			log(message);

			throw message;
		}
	}
}

void _EPMDevice::stop()
{
	quint8 overflowed;
	MicroEpmErrorType status;
	MicroEpmDevice* devHandle(_impl->_deviceHandle);

	status = MicroEpmStopAcquisition(devHandle, &overflowed);
	if (status != MICRO_EPM_SUCCESS)
	{
		QString message = QString("MicroEpmStopAcquisition: ") + EPMErrorToString(status);
		log(message);
	}

	status = MicroEpmStopRecording(devHandle);
	if (status != MICRO_EPM_SUCCESS)
	{
		QString message = QString("MicroEpmStopRecording: ") + EPMErrorToString(status);
		log(message);
	}

	for (const auto& runChannel: _runChannels)
	{
		status = MicroEpmSetChannelEnable(devHandle, runChannel._channel, MICRO_EPM_CHANNEL_DISABLE);
		if (status != MICRO_EPM_SUCCESS)
		{
			QString message = QString("MicroEpmSetChannelEnable: MICRO_EPM_CHANNEL_DISABLE: %1").arg(runChannel.key()) + EPMErrorToString(status);
			log(message);
		}
	}

	status = MicroEpmApplySettings(devHandle);
	if (status != MICRO_EPM_SUCCESS)
	{
		QString message = QString("MicroEpmApplySettings: ") + EPMErrorToString(status);
		log(message);
	}

	MicroEpmChannelStatType stats;

	for (const auto& runChannel: _runChannels)
	{
		status = MicroEpmGetChannelStats(devHandle, runChannel._channel, &stats);
		if (status == MICRO_EPM_SUCCESS)
		{
			EPMResult epmResult;

			epmResult._channel = runChannel._channel;
			epmResult._channelName = runChannel._name;

			if (runChannel._channelType == EPMRunChannel::eCurrent)
				epmResult._units = "mA";
			else
				epmResult._units = "mV";

			epmResult._average = stats.dbAverage;
			epmResult._min = stats.dbMin;
			epmResult._max = stats.dbMin;
			epmResult._sampleCount = stats.uNumSamples;

			_results.append(epmResult);

			log(QString("Results: %1 %2 samples").arg(epmResult._channel).arg(epmResult._sampleCount));
		}
		else
		{

		}
	}

	_runChannels.clear();
}

void _EPMDevice::log
(
	const QString& message
)
{
	if (_threadedLog.isNull() == false)
	{
		_threadedLog->addLogEntry(QDateTime::currentDateTime().toString() + " " + message + "\n");
	}
}

QString EPMErrorToString
(
	MicroEpmErrorType status
)
{
	QString result = QString("Error: %1 ").arg(status);

	switch (status)
	{
	case MICRO_EPM_SUCCESS: result += "Success"; break;
	case MICRO_EPM_ERROR: result += "Error"; break;
	case MICRO_EPM_VERSION_MISMATCH_OLD_HOST: result += "Version Mismatch Old Host"; break;
	case MICRO_EPM_VERSION_MISMATCH_OLD_FIRMWARE: result += "Version Mismatch Old Firmware"; break;
	case MICRO_EPM_COMM_ERROR: result += "COMM Error"; break;
	case MICRO_EPM_OUT_OF_MEMORY: result += "Out of Memory"; break;
	case MICRO_EPM_NOT_CONNECTED: result += "Not Connected"; break;
	case MICRO_EPM_INVALID_PARAMETER: result += "Invalid Parameter"; break;
	case MICRO_EPM_OVERFLOW: result += "Overflow"; break;
	case MICRO_EPM_NOT_PROGRAMMED: result += "Not Programmed"; break;
	case MICRO_EPM_INVALID_CHECKSUM: result += "Invalid Checksum"; break;
	case MICRO_EPM_TOO_MANY_DEVICES: result += "Too Many Devices"; break;
	case MICRO_EPM_NOT_IMPLEMENTED: result += "Not Implemented"; break;
	case MICRO_EPM_POWER_ON_TEST_FAILED: result += "Power On Test Failed"; break;
	case MICRO_EPM_INVALID_PARSED_DATA: result += "Invalid Parsed Data"; break;
	case MICRO_EPM_NOT_ERASED: result += "Not Erased"; break;
	case MICRO_EPM_FLASH_IMAGE_FAILED: result += "Flash Image Failed"; break;
	case MICRO_EPM_INVALID_TRIGGERED_MODE_SETTINGS: result += "Invalid Triggered Mode Settings"; break;
	case MICRO_EPM_COMM_ERROR_NOT_CONNECTED: result += "COMM Error Not Connected"; break;
	case MICRO_EPM_TAC_CMD_NOT_SUPPORTED: result += "TAC Command Not Supported"; break;
	case MICRO_EPM_FILE_ERROR: result += "File Error"; break;
	case MICRO_EPM_INVALID_FILE_PATH: result += "Invalid File Path"; break;
	case MICRO_EPM_ERROR_NO_CHAN_EN: result += "Error No Channels Enabled"; break;
	case MICRO_EPM_ERROR_ALREADY_ACQUIRING: result += "Error Already Acquiring"; break;
	default:
		break;
	}

	return result;
}

QString MicroEpmConvertUuidToString
(
	MicroEpmBoardUuidType uuid
)
{
	QString result = QString("%1-%2-%3-%4-%5%6%7%8%9%10").arg(uuid.uPart1, 8, 16, QChar('0')).
			arg(uuid.usPart2, 4, 16, QChar('0')).arg(uuid.usPart3, 4, 16, QChar('0')).arg(uuid.usPart4, 4, 16, QChar('0')).
			arg(uuid.aucPart5[0], 2, 16, QChar('0')).arg(uuid.aucPart5[1], 2, 16, QChar('0')).arg(uuid.aucPart5[2], 2, 16, QChar('0')).
			arg(uuid.aucPart5[3], 2, 16, QChar('0')).arg(uuid.aucPart5[4], 2, 16, QChar('0')).arg(uuid.aucPart5[5], 2, 16, QChar('0'));

	return result;
}
