// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/
#include "EPMDevice.h"

//libEPM
#include "EPMDevice3.h"
#include "EPMDevice4.h"
#include "EpmLibPrivate.h"
#include "RecordingInferface.h"

#ifdef Q_OS_LINUX
#include "LinuxBaseDevice.h"
#endif

// QCommon
#include "Range.h"
#include "StringUtilities.h"

// QT
#include <QDateTime>
#include <QThread>

#define SPMV4_MAJOR_VERSION                (1)
#define MICRO_EPM_DEFAULT_AVERAGING_MODE   MICRO_EPM_AVERAGING_MODE_NONE
#define MICRO_EPM_DEFAULT_ADC_MODE         MICRO_EPM_ADC_MODE_CONTINUOUS
#define MICRO_EPM_DEFAULT_CONV_TIME        MICRO_EPM_CONV_TIME_140_US
#define MICRO_EPM_DEFAULT_SET_PERIOD       (140)
#define MICRO_EPM_DEFAULT_DATA_RATE        (0xffffffff)

#define MAX_ADC_CHANNELS 128
#define MAX_GPIO_CHANNELS 1
#define MAX_CHANNELS (MAX_ADC_CHANNELS + MAX_GPIO_CHANNELS)
#define GPIO_CHANNEL_INDEX MAX_ADC_CHANNELS
#define CHANNELS_PER_BUS 32
#define CHANNELS_PER_DEVICE 2
#define ADC_PER_BUS 16
#define TYPE_GPIO 0
#define TYPE_VOLTAGE 1
#define TYPE_CURRENT 2
#define MAX_BUSES 4
#define MAX_SPM_CHAN 63

class EPMPauseMemory
{
public:
	EPMPauseMemory(_EPMDevice* epmDevice) :
		_epmDevice{epmDevice}
	{

	}

	void setPausedState()
	{
		_paused = _epmDevice->_paused;
		if (_paused == false)
			_epmDevice->pauseAdcConversions(false);
	}

	~EPMPauseMemory()
	{
		if (_paused == false)
			_epmDevice->unpauseAdcConversions(false);
	}

private:
	_EPMDevice*					_epmDevice = Q_NULLPTR;
	bool						_paused = false;
};

EPMDevices _EPMDevice::_activeDevices;
EPMDevices _EPMDevice::_inactiveDevices;

_EPMDevice::_EPMDevice()
{
}

_EPMDevice::~_EPMDevice()
{
}

void _EPMDevice::addDevice
(
	EPMDevice& epmDevice
)
{
	if (epmDevice.isNull() == false)
	{
		HashType hash = epmDevice->hash();
		for (const auto& device: std::as_const(_activeDevices))
		{
			if (hash == device->hash())
			{
				device->_active = true;
				device->log(QString("Active Device %1 Set Active").arg(device->getTargetName()));
				return;
			}
		}

		int index{0};
		for (const auto& device: std::as_const(_inactiveDevices))
		{
			if (hash == device->hash()) // move the inactive device to active
			{
				device->_active = true;
				device->log(QString("Inactive Device %1 Set Active").arg(device->getTargetName()));

				_activeDevices.append(device);
				_inactiveDevices.removeAt(index);
				return;
			}

			index++;
		}

		if (initializeDevice(epmDevice) == true)
		{
			epmDevice->_active = true;
			epmDevice->log(QString("Device %1 Initialized and Set Active").arg(epmDevice->getTargetName()));

			_activeDevices.append(epmDevice);
		}
		else
		{
			if (epmDevice->_programmed == false && epmDevice->_errorStatus == false)
			{
				_activeDevices.append(epmDevice);

				epmDevice->log(QString("Unprogrammed EPM device: %1").arg(epmDevice->_devicePath.data()));
			}
			else
				epmDevice->log(QString("Device seen, but can't be opened %1").arg(epmDevice->_devicePath.data()));
		}
	}
}

bool _EPMDevice::initializeDevice
(
	EPMDevice& epmDevice
)
{
	bool result{false};

	try
	{
		epmDevice->open();
		epmDevice->eepromRead();

		result = epmDevice->_programmed;
	}
	catch (EPMException& error)
	{
		epmDevice->log(QString("_EPMDevice::initializeDevice failed %1").arg(error.what()));

		if (error.epmError() == MICRO_EPM_COMM_ERROR || error.epmError() == MICRO_EPM_OS_ERROR)
		{
			uint nativeError = error.nativeError();
			switch(nativeError)
			{
				case kUSBSendError:
				case kUSBReceiveError:
				case kUSBOpenError:
					epmDevice->_errorStatus = true;
					result = false;
					break;
				default:
					break;
			}
		}
	}

	epmDevice->close();

	return result;
}

static uint32_t GetBusOfChannel(uint32_t uChannel)
{
   return uChannel / CHANNELS_PER_BUS;
}

static uint32_t GetDeviceOfChannel(uint32_t uChannel)
{
   if (uChannel % 2)
   {
	  return ((uChannel % CHANNELS_PER_BUS) - 1) / CHANNELS_PER_DEVICE;
   }
   else
   {
	  return (uChannel % CHANNELS_PER_BUS) / CHANNELS_PER_DEVICE;
   }
}

void _EPMDevice::getNumBusesAndAdcMasks()
{
   uint32_t uChannel;
   uint32_t uBus;
   uint32_t uAdc;
   uint8_t uBusesEnabled = 0;

   for (uChannel = 0; uChannel < MAX_CHANNELS; uChannel++)
   {
	  if (uChannel >= MAX_ADC_CHANNELS)
	  {
		 continue;
	  }
	  
	  uBus = GetBusOfChannel(uChannel);
	  uAdc = GetDeviceOfChannel(uChannel);
	  if (!(uBusesEnabled & (1 << uBus)))
	  {
		 _numBuses = _numBuses + 1;
		 uBusesEnabled |= (1 << uBus);
	  }

	  _adcPopulatedMask[uBus] |= (1 << uAdc);
   }
}

void _EPMDevice::open()
{
	if (!_initialized)
	{
		try
		{
			sendHelloCommand();

			if (_version._firmwareVersion[0] == SPMV4_MAJOR_VERSION) // SPMv4
			{
				_isSpmV4 = true;
				_paused = false;
				_secureMode = false;
				_numBuses = MAX_BUSES;
				_adcMode = MICRO_EPM_DEFAULT_ADC_MODE;
				_setPeriod = MICRO_EPM_DEFAULT_SET_PERIOD;
				_maxDataRate = MICRO_EPM_DEFAULT_DATA_RATE;
				_channelCount *= 2;   // SPMv4 reports 64 channels, but each has V & I
				_gpioEnMask = 0x0;
				_version._maxSamplesPerPacket = MAX_SAMPLES_PER_PACKET;
				_version._maxSamplesPerAveragePacket = 0;
				_version._hostLowestCompatibleProtocolVersion = MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION_SPMV4;
				_acquiring = false;

				if (_version._firmwareProtocolVersion >= 7)
					_version._epmID = getPlatformID();
				else
					_version._epmID = MICRO_EPM_BOARD_ID_UNKNOWN;

				for (uint uBus = 0; uBus < MAX_BUSES; uBus++)
				{
					for (uint uAdc = 0; uAdc < MAX_ADCS_PER_BUS; uAdc++)
					{
						_averagingMode[uBus][uAdc] = MICRO_EPM_DEFAULT_AVERAGING_MODE;
					}

					for (uint uChannel = 0; uChannel < MAX_CHANNELS_PER_BUS; uChannel++)
					{
						_convTime[uBus][uChannel] = MICRO_EPM_DEFAULT_CONV_TIME;
					}
				}

				for (uint uChannel = 0; uChannel < MICRO_EPM_MAX_NUMBER_CHANNELS; uChannel++)
				{
					MicroEpmChannelInfo* pChanInfo = &_channelInfo[uChannel];
					pChanInfo->szName[0] = '\0';
					pChanInfo->dbRsenseMilliOhms = MICRO_EPM_DEFAULT_RSENSE_MOHM;
					pChanInfo->uRcmChannel = uChannel;
					pChanInfo->stats = MicroEpmChannelStat();
					//memset(&pChanInfo->stats, 0, sizeof(MicroEpmChannelStat));
				}
			}
			else
			{
				// SPMv3
				_isSpmV4 = false;
				_numBuses = 1;
				_version._maxSamplesPerPacket = MAX_SAMPLES_PER_PACKET + 1;   // SPMv3 has GPIO data in the header
				_version._maxSamplesPerAveragePacket = MAX_SAMPLES_PER_AVERAGE_PACKET;
				_version._hostLowestCompatibleProtocolVersion = MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION_SPMV3;
				_version._epmID = MICRO_EPM_BOARD_ID_UNKNOWN;
			}

			clearRunChannelBitmap();

			_initialized = true;

		}
		catch (EPMException& error)
		{
			throw error;
		}
	}
}

EpmErrorCode _EPMDevice::send
(
	unsigned char* buffer,
	quint32 bufferSize
)
{
	EpmErrorCode result{};

	Q_UNUSED(buffer)
	Q_UNUSED(bufferSize)

	return result;
}

EpmErrorCode _EPMDevice::receive
(
	unsigned char* buffer,
	quint32 bufferSize
)
{
	EpmErrorCode result{};

	Q_UNUSED(buffer)
	Q_UNUSED(bufferSize)

	return result;
}

bool _EPMDevice::checkReturnCode
(
	quint8 commandCode
)
{
	return _inputBuffer[CMD_ADDRESS] == commandCode;
}

void _EPMDevice::log
(
	const QString& message
)
{
	AppCore::writeToApplicationLog(QDateTime::currentDateTime().toString() + " " + message + "\n");
}

void _EPMDevice::clearRunChannelBitmap()
{
	memset(&_enabledChannelBitmask, 0, sizeof(_enabledChannelBitmask));
}

void _EPMDevice::setupRunChannelBitmap()
{
	for (const auto& runChannel: std::as_const(_runChannels))
	{
		setChannelEnable(runChannel->channel(), MICRO_EPM_CHANNEL_ENABLE);
	}
}

quint32 _EPMDevice::countNumberOfSpmBoards()
{
	quint32 result{0};

	for (const auto& device : std::as_const(_activeDevices))
	{
		if (device->_isSpmV4 == true || device->_isSPM)
			continue;

		try
		{
			MicroEpmGpioValue gpioVal;

			device->enterSecureMode();
			device->setGpioDrive_3(MICRO_EPM_GPIO_PIN_GPIO_5, MICRO_EPM_GPIO_DRIVE_RESISTIVE_PULL_DOWN);
			gpioVal = device->getGpioValue(MICRO_EPM_GPIO_PIN_GPIO_5);

			if (gpioVal == MICRO_EPM_GPIO_VALUE_HIGH)
			{
				device->_isSPM = true;
				result++;
			}
		}
		catch (EPMException& epmError)
		{
			Q_UNUSED(epmError)
		}
	}

	return result;
}

EPMDevice _EPMDevice::device
(
	quint32 index
)
{
	EPMDevice result;

	if (index < static_cast<quint32>(_activeDevices.count()))
		result = _activeDevices.at(index);

	return result;
}

EPMDevice _EPMDevice::deviceByHash(HashType hash)
{
	EPMDevice result;

	result = _EPMDevice::deviceByHash(hash, _activeDevices);

	return result;
}

EPMDevice _EPMDevice::deviceByHash(HashType hash, const EPMDevices &epmDevices)
{
	EPMDevice result;

	for (const auto& epmDevice: epmDevices)
	{
		if (epmDevice->hash() == hash)
		{
			result = epmDevice;
			break;
		}
	}

	return result;
}

EPMDevice _EPMDevice::deviceByName
(
	const QString& name
)
{
	return _EPMDevice::deviceByName(name, _activeDevices);
}

EPMDevice _EPMDevice::deviceByName(const QString &name, const EPMDevices &epmDevices)
{
	EPMDevice result;

	for (const auto& epmDevice: epmDevices)
	{
		if (epmDevice->getTargetName().contains(name, Qt::CaseInsensitive))
		{
			result = epmDevice;
			break;
		}
		if (epmDevice->getSerialNumber().contains(name, Qt::CaseInsensitive))
		{
			result = epmDevice;
			break;
		}
	}

	return result;
}

quint32 _EPMDevice::deviceCount()
{
	return _activeDevices.count();
}

void _EPMDevice::eepromErase()
{
	quint8 rowData[EEPROM_ROW_SIZE];
	qint32 uNumEepromRows;

	try
	{
		enterSecureMode();

		for (uNumEepromRows = 0; uNumEepromRows < EEPROM_NUMBER_OF_ROWS; uNumEepromRows++)
		{
			MakeEepromWriteCommand((quint8) uNumEepromRows, rowData);

			send(EEPROM_WRITE_CMD_LENGTH);
			receive(EEPROM_WRITE_RSP_LENGTH, EEPROM_WRITE_RSP);

			ParseEepromWriteResponse((quint8) uNumEepromRows, rowData);
		}
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::eepromRead
(
	bool ignoreChecksum
)
{
	quint8* paEeprom;

	if (_connected == false)
		throw MICRO_EPM_NOT_CONNECTED;

	try
	{
		if (_version._firmwareVersion[2] <= 2)
			enterSecureMode();

		paEeprom = (quint8*) &_eeprom;

		for (auto uNumEepromRows: range(EEPROM_NUMBER_OF_ROWS))
		{
			MakeEepromReadCommand((quint8) uNumEepromRows);

			send(EEPROM_READ_CMD_LENGTH);
			receive(EEPROM_READ_RSP_LENGTH, EEPROM_READ_RSP);

			// Parse response
			ParseEepromReadResponse((quint8) uNumEepromRows, paEeprom);
		}

		parseEepromData(paEeprom, ignoreChecksum);
	}
	catch (EPMException& error)
	{
		if (error.epmError() == MICRO_EPM_NOT_PROGRAMMED)
		{

		}
		else
			throw error;
	}

}

void _EPMDevice::eepromWrite()
{
	quint8 rowData[EEPROM_ROW_SIZE];
	quint8* paEeprom;

	try
	{
		enterSecureMode();
		setupEepromData();

		paEeprom = (quint8*) &_eeprom;

		for (auto uNumEepromRows: range(EEPROM_NUMBER_OF_ROWS))
		{
			for (auto uEepromRowSize: range(EEPROM_ROW_SIZE))
			{
				rowData[uEepromRowSize] = paEeprom[uNumEepromRows * EEPROM_ROW_SIZE + uEepromRowSize];
			}

			MakeEepromWriteCommand((quint8) uNumEepromRows, rowData);

			send(EEPROM_WRITE_CMD_LENGTH);
			receive(EEPROM_WRITE_RSP_LENGTH, EEPROM_WRITE_RSP);

			ParseEepromWriteResponse((quint8) uNumEepromRows, rowData);
		}
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::enterSecureMode()
{
	quint8 returnCode = 0;

	if (_connected == false)
		throw MICRO_EPM_NOT_CONNECTED;

	if (_isSpmV4 == true && _secureMode == true)
		return;

	try
	{
		MakeEnterSecureModeCommand();

		send(ENTER_SECURE_MODE_CMD_LENGTH);
		receive(ENTER_SECURE_MODE_RSP_LENGTH, ENTER_SECURE_MODE_RSP);

		ParseEnterSecureModeResponse(&returnCode);

		if (returnCode != EPM_PROTOCOL_STATUS_SUCCESS)
			throw MICRO_EPM_COMM_ERROR;

		if (_isSpmV4 == true)
			_secureMode = true;

	}
	catch (EPMException& error)
	{
		throw error;
	}
}

MicroEpmGpioValue _EPMDevice::getGpioValue
(
	GpioPin gpioChannel
)
{
	MicroEpmGpioValue result;
	GpioPin receivedPin;


	if (_connected != true)
		throw MICRO_EPM_NOT_CONNECTED;

	try
	{
		MakeGetGpioValueCommand(gpioChannel);

		send(GET_GPIO_VALUE_CMD_LENGTH);
		receive(GET_GPIO_VALUE_RSP_LENGTH, GET_GPIO_VALUE_RSP);

		ParseGetGpioValueResponse(&receivedPin, &result);

		if (receivedPin != gpioChannel)
			throw MICRO_EPM_COMM_ERROR;
	}
	catch (EPMException& error)
	{
		throw error;
	}

	return result;
}

quint32 _EPMDevice::getRawTimestamp()
{
	quint32 result(0);

	if (!_connected)
		throw MICRO_EPM_NOT_CONNECTED;

	try
	{
		MakeGetSystemTimestampCommand();

		send(GET_SYSTEM_TIMESTAMP_CMD_LENGTH);
		receive(GET_SYSTEM_TIMESTAMP_RSP_LENGTH, GET_SYSTEM_TIMESTAMP_RSP);

		ParseGetSystemTimestampResponse(&result);
		
		result = static_cast<quint32>(TIMER_MASK - result);
	}
	catch (EPMException& error)
	{
		throw error;
	}

	return result;
}

HashType _EPMDevice::hash()
{
	if (_hash == 0)
	{
		_hash = ::arrayHash(_devicePath);
	}

	return _hash;
}

void _EPMDevice::setUDASRecorder
(
	RecordingInterface* recordingInterface
)
{
	_udasRecorder = recordingInterface;
}

void _EPMDevice::removeUDASRecorder
(
)
{
	_udasRecorder = Q_NULLPTR;
}

void _EPMDevice::setLiveRecorder
(
	RecordingInterface* recordingInterface
)
{
	_liveRecorder = recordingInterface;
}

void _EPMDevice::removeLiveRecorder
(
)
{
	_liveRecorder = Q_NULLPTR;
}

void _EPMDevice::acquire()
{
	quint32 uChannel;

	if (_connected == false)
		throw EPMException(MICRO_EPM_NOT_CONNECTED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_CONNECTED));

	if (_acquiring == true)
		throw EPMException(MICRO_EPM_ERROR_ALREADY_ACQUIRING, 0, EPMErrorToByteArray(MICRO_EPM_ERROR_ALREADY_ACQUIRING));

	for (uChannel = 0; uChannel < MICRO_EPM_MAX_NUMBER_CHANNELS; uChannel++)
	{
		MicroEpmChannelInfo* pChanInfo = &_channelInfo[uChannel];
		pChanInfo->stats = MicroEpmChannelStat();
		//memset(&pChanInfo->stats, 0, sizeof(MicroEpmChannelStat));
	}

	_acquiring = true;
	_threadExitCode = MICRO_EPM_SUCCESS;

	applySettings();

	start(QThread::HighestPriority);
}

void _EPMDevice::stop()
{
	if (_connected == false)
		throw MICRO_EPM_NOT_CONNECTED;

	if (_acquiring == false)
		return;

	_acquiring = false;

	wait();
}

void _EPMDevice::addChannel
(
	const EPMChannel epmRunChannel
)
{
	try
	{
		try
		{
			setChannelName(static_cast<quint32>(epmRunChannel->channel()), epmRunChannel->seriesName().toLatin1().data());
		}
		catch (EPMException& error)
		{
			QString message = QString("Error MicroEpmSetChannelName: %1, Name = %2").arg(epmRunChannel->channel()).arg(epmRunChannel->seriesName());
			log(message + error.what());
			throw error;
		}

		setChannelRaw(static_cast<quint32>(epmRunChannel->channel()), epmRunChannel->dumpRawValues());

		if (epmRunChannel->waveForm() == WaveFormType::eWaveFormCurrent)
		{
			try
			{
				setRsense(static_cast<quint32>(epmRunChannel->channel()), epmRunChannel->_resistorValue);
			}
			catch (EPMException& error)
			{
				QString message = QString("Error MicroEpmSetRsense: %1, RValue = %2").arg(epmRunChannel->channel()).arg(epmRunChannel->_resistorValue);
				log(message + error.what());
			}
		}

		try
		{
			setRcmChannel(static_cast<quint32>(epmRunChannel->channel()), static_cast<quint32>(epmRunChannel->rcmChannel()));
		}
		catch (EPMException& error)
		{
			QString message = QString("MicroEpmSetRcmChannel: %1, RCM Channel = %2").arg(epmRunChannel->channel()).arg(epmRunChannel->rcmChannel());
			log(message + error.what());
		}

		log(QString("Added Channel: %1-%2").arg(epmRunChannel->seriesName()).arg(epmRunChannel->channel()));

		_runChannels[static_cast<quint32>(epmRunChannel->channel())] = epmRunChannel;
	}
	catch (EPMException& error)
	{
		Q_UNUSED(error)
	}
}

void _EPMDevice::removeChannel
(
	const EPMChannel epmRunChannel
)
{
	try
	{
		setChannelEnable(static_cast<quint32>(epmRunChannel->channel()), MICRO_EPM_CHANNEL_DISABLE);
		log(QString("Removed Channel: %1-%2").arg(epmRunChannel->seriesName()).arg(epmRunChannel->channel()));
	}
	catch (EPMException& error)
	{
		QString message = QString("MicroEpmSetChannelEnable: %1, MICRO_EPM_CHANNEL_DISABLE").arg(epmRunChannel->channel());
		log(message + error.what());
	}

	_runChannels.remove(static_cast<quint32>(epmRunChannel->channel()));
}

void _EPMDevice::applySettings()
{
	if (!_connected)
		throw MICRO_EPM_NOT_CONNECTED;

	if (!_isSpmV4)
		throw MICRO_EPM_NOT_IMPLEMENTED;

	if (_adcMode == MICRO_EPM_ADC_MODE_TRIGGERED)
		verifyTriggeredModeInput();

	setupRunChannelBitmap();

	sendEnableDisable();
	sendAveragingMode();
	sendAdcMode();
	sendSetPeriod();
	sendConvTime();
	sendDataRateGovernor();
	sendGpioEnable();
	sendApplySettings();

	setRawTimestamp(0);
}

/**
  Reads an INA231 register.

  The packet buffer will be paused while reading the register. If already paused
  then the packet buffer will stay paused. If unpaused then this function will pause,
  read the register and unpause.

  @return
  MICRO_EPM_SUCCESS is returned if successful\n
  Otherwise, a corresponding error code is returned.
*/
void _EPMDevice::ina231RegisterRead
(
	quint8 uBus,
	quint8 uAddress,
	quint8 uRegister,
	quint16 *puData
)
{
	EPMPauseMemory epmPause(this);
	quint8 uReceivedStatusCode;

	if (_connected == false)
		throw MICRO_EPM_NOT_CONNECTED;

	if (uBus >= MAX_BUSES)
		throw MICRO_EPM_INVALID_PARAMETER;

	try
	{
		enterSecureMode();

		epmPause.setPausedState();

		MakeIna231RegisterReadCommand(uBus, uAddress, uRegister);

		send(INA231_REGISTER_READ_CMD_LENGTH);
		receive(INA231_REGISTER_READ_RSP_LENGTH, INA231_REGISTER_READ_RSP);

		ParseIna231RegisterReadResponse(&uReceivedStatusCode, puData);

		if (uReceivedStatusCode != EPM_PROTOCOL_STATUS_SUCCESS)
			throw MICRO_EPM_COMM_ERROR;
	}
	catch (EPMException& status)
	{
		throw status;
	}
}


//  Writes an INA231 register.

//  The packet buffer will be paused while writing the register. If already paused
//  then the packet buffer will stay paused. If unpaused then this function will pause,
//  write the register and unpause.

void _EPMDevice::ina231RegisterWrite
(
	quint8 uBus,
	quint8 uAddress,
	quint8 uRegister,
	quint16 uData
)
{
	EPMPauseMemory epmPause(this);

	if (!_connected)
		throw MICRO_EPM_NOT_CONNECTED;

	if (uBus >= MAX_BUSES)
		throw MICRO_EPM_INVALID_PARAMETER;

	try
	{
		quint8 receivedStatusCode;

		enterSecureMode();

		epmPause.setPausedState();

		MakeIna231RegisterWriteCommand(uBus, uAddress, uRegister, uData);

		send(INA231_REGISTER_WRITE_CMD_LENGTH);
		receive(INA231_REGISTER_WRITE_RSP_LENGTH, INA231_REGISTER_WRITE_RSP);

		ParseIna231RegisterWriteResponse(&receivedStatusCode);

		if (receivedStatusCode != EPM_PROTOCOL_STATUS_SUCCESS)
			throw MICRO_EPM_COMM_ERROR;

	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::parseEepromData
(
	quint8* aData,
	bool ignoreChecksum
)
{
	quint32 checksum{0};
	quint32 calcChecksum{0};
	quint32 i, j;
	MicroEpmEepromPacked* pEeprom = reinterpret_cast<MicroEpmEepromPacked*>(aData);

	_readEeprom = true;

	_targetInfo._eepromProgrammedTime = pEeprom->EepromProgrammedTime;

	if (_targetInfo._eepromProgrammedTime != 0)
	{
		// Zero out the destination to clear old data and set the final null bit in the string
		memset(_targetInfo._serialNumber, 0 , MICRO_EPM_STRING_SIZE);
		memset(_targetInfo._modelName, 0, MICRO_EPM_STRING_SIZE);

		for (i = 0; i < MICRO_EPM_STRING_SIZE; i++)
		{
			_targetInfo._serialNumber[i] = pEeprom->szSerialNumber[i];
			_targetInfo._modelName[i] = pEeprom->szModelNumber[i];
		}

		_targetInfo._targetIdentifier = static_cast<TargetIdentityInfo>(pEeprom->uTargetIdentifier);
		_targetInfo._platformIdentifier = static_cast<PlatformID>(pEeprom->uBoardTypeIdentifier);
		_targetInfo._boardOptionIdentifier = static_cast<MicroEpmBoardInfo>(pEeprom->uBoardOptionIdentifier);
		_targetInfo._eepromWriteCount = pEeprom->uEepromFailWriteCount;

		/* Add two rework history, future requirement */
		for (i = 0; i < EEPROM_EPM_MAX_NUMBER_CHANNELS; i++)
		{
			aEepromChannelInfo[i].uSenseResistance = pEeprom->aChannelInfo[i].uSenseResistance;
			aEepromChannelInfo[i].uCID = pEeprom->aChannelInfo[i].uChannelIdentifier;

			for (j = 0; j < (sizeof(pEeprom->aChannelInfo[i].szChannelName)/sizeof(pEeprom->aChannelInfo[i].szChannelName[0])); j++)
			{
				aEepromChannelInfo[i].szChannelName[j] = pEeprom->aChannelInfo[i].szChannelName[j];
			}

			aEepromChannelInfo[i].szChannelName[j] = 0;
		}

		for (i = 0; i < EEPROM_EPM_MAX_CAL_NUMBER_CHANNELS; i++)
			aCalibrationCodes[i] = pEeprom->aCalibrationCodes[i];

		eepromCalibrationTime = (time_t) pEeprom->eepromCalibrationTime;

		_targetInfo._epmBoardUuid = BoardUuid(pEeprom->epmBoardUuid);

		/* Validate checksum */
		if (ignoreChecksum == false)
		{
			checksum = pEeprom->uCheckSum;
			for (i = 0; i < EEPROM_SIZE - EEPROM_CHECKSUM_SIZE; i++)
			{
				calcChecksum += aData[i];
			}

			if (calcChecksum != checksum)
				throw EPMException(MICRO_EPM_ERROR, 0, EPMErrorToString(MICRO_EPM_INVALID_CHECKSUM).toLatin1());
		}

		_programmed = true;
	}
}

/**
  Pauses ADC conversions by pausing the packet buffer.
*/
void _EPMDevice::pauseAdcConversions
(
	bool throwsException //= true
)
{
	try
	{
		if (_connected == true)
		{
			if (_isSpmV4 == true && _paused == true)
			{
			}
			else
			{
				MakePauseAdcConversionsCommand();

				send(PAUSE_ADC_CONVERSIONS_CMD_LENGTH);
				receive(PAUSE_ADC_CONVERSIONS_RSP_LENGTH, PAUSE_ADC_CONVERSIONS_RSP);

				ParsePauseAdcConversionsResponse();

				QThread::msleep(15);

				if (_isSpmV4 == true)
					_paused = true;
			}
		}
		else
		{
			throw MICRO_EPM_NOT_CONNECTED;
		}
	}
	catch (EPMException& error)
	{
		if (throwsException)
			throw error;
	}
}

void _EPMDevice::sendAdcMode()
{
	quint8 receivedStatusCode;

	try
	{
		MakeSetAdcModeCommand(_adcMode);

		send(SET_ADC_MODE_CMD_LENGTH);
		receive(SET_ADC_MODE_RSP_LENGTH, SET_ADC_MODE_RSP);

		ParseSetAdcModeResponse(&receivedStatusCode);

		if (EPM_PROTOCOL_STATUS_SUCCESS != receivedStatusCode)
			throw MICRO_EPM_COMM_ERROR;
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::sendApplySettings()
{
	try
	{
		quint8 receivedStatusCode;

		MakeApplySettingsCommand();

		send(APPLY_SETTINGS_CMD_LENGTH);
		receive(APPLY_SETTINGS_RSP_LENGTH, APPLY_SETTINGS_RSP);

		ParseApplySettingsResponse(&receivedStatusCode);

		if (EPM_PROTOCOL_STATUS_SUCCESS != receivedStatusCode)
			throw MICRO_EPM_COMM_ERROR;
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::logSample
(
	MicroEpmChannelStat& stats,
	double physical
)
{
	if (stats.uNumSamples == 0)
	{
		stats.dbAverage = physical;
		stats.dbMin = physical;
		stats.dbMax = physical;
	}
	else
	{
		if (physical > stats.dbMax)
		{
			stats.dbMax = physical;
		}

		if (physical < stats.dbMin)
		{
			stats.dbMin = physical;
		}
	}

	stats.uNumSamples++;
	stats.dbAverage += (physical - stats.dbAverage) / stats.uNumSamples;
}

PlatformID _EPMDevice::getPlatformID()
{
	PlatformID result(MICRO_EPM_BOARD_ID_UNKNOWN);

	try
	{
		MakeGetEpmIDCommand();
		send(GET_EPM_ID_CMD_LENGTH);
		receive(GET_EPM_ID_RSP_LENGTH, GET_EPM_ID_RSP);

		ParseGetEpmIDResponse(&result);
	}
	catch (EPMException& error)
	{
		Q_UNUSED(error)
	}

	return result;
}

void _EPMDevice::enterBootloader()
{
	 try
	 {
		quint8 returnCode;

		enterSecureMode();

		MakeEnterBootloaderCommand();

		send(ENTER_BOOTLOADER_CMD_LENGTH);
		receive(ENTER_BOOTLOADER_RSP_LENGTH, ENTER_BOOTLOADER_RSP);

		ParseEnterBootloaderResponse(&returnCode);
	 }
	 catch (EPMException& error)
	 {
		throw error;
	 }
}

void _EPMDevice::sendAveragingMode()
{
	quint8 uBus;

	try
	{
		quint8 receivedStatusCode;

		for (uBus = 0; uBus < _numBuses; uBus++)
		{
			MakeSetAveragingCommand(uBus, _averagingMode[uBus]);

			send(SET_AVERAGING_CMD_LENGTH);
			receive(SET_AVERAGING_RSP_LENGTH, SET_AVERAGING_RSP);

			ParseSetAveragingResponse(&receivedStatusCode);

			if (EPM_PROTOCOL_STATUS_SUCCESS != receivedStatusCode)
				throw MICRO_EPM_COMM_ERROR;
		}
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::sendConvTime()
{
	quint8 uBus;

	try
	{
		quint8 receivedStatusCode;

		for (uBus = 0; uBus < _numBuses; uBus++)
		{
			MakeSetConversionTimeCommand(uBus, _convTime[uBus]);

			send(SET_CONVERSION_TIME_CMD_LENGTH);
			receive(SET_CONVERSION_TIME_RSP_LENGTH, SET_CONVERSION_TIME_RSP);

			ParseSetConversionTimeResponse(&receivedStatusCode);

			if (EPM_PROTOCOL_STATUS_SUCCESS != receivedStatusCode)
				throw MICRO_EPM_COMM_ERROR;
		}
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::sendDataRateGovernor()
{
	quint8 receivedStatusCode;

	try
	{
		MakeSetDataRateGovernorCommand(_maxDataRate);

		send(SET_DATA_RATE_GOVERNOR_CMD_LENGTH);
		receive(SET_DATA_RATE_GOVERNOR_RSP_LENGTH, SET_DATA_RATE_GOVERNOR_RSP);

		ParseSetDataRateGovernorResponse(&receivedStatusCode);

		if (EPM_PROTOCOL_STATUS_SUCCESS != receivedStatusCode)
			throw MICRO_EPM_COMM_ERROR;
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::sendGpioEnable()
{
	quint8 returnCode;

	try
	{
		MakeSetGpioBufferStatusCommand(_gpioEnMask);

		send(SET_GPIO_BUFFER_STATUS_CMD_LENGTH);
		receive(SET_GPIO_BUFFER_STATUS_RSP_LENGTH, SET_GPIO_BUFFER_STATUS_RSP);

		ParseSetGpioBufferStatusResponse(&returnCode);

		if (EPM_PROTOCOL_STATUS_SUCCESS != returnCode)
			throw MICRO_EPM_COMM_ERROR;
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::sendEnableDisable()
{
	try
	{	quint32 uBitmask;
		quint8 uReceivedStatusCode;
		quint32 uReceivedBitmask;

		for (auto uBus: range(_numBuses))
		{
			uBitmask = _enabledChannelBitmask[uBus];

			MakeEnableDisableChannelCommand(uBus, uBitmask);

			send(ENABLE_DISABLE_CHANNEL_CMD_LENGTH);
			receive(ENABLE_DISABLE_CHANNEL_RSP_LENGTH, ENABLE_DISABLE_CHANNEL_RSP);

			ParseEnableDisableChannelResponse(&uReceivedStatusCode, &uReceivedBitmask);

			if (uBitmask != uReceivedBitmask || EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode)
				throw MICRO_EPM_COMM_ERROR;
		}
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::clearBuffer()
{
	try
	{
		quint8 uBitmask{0xff};   // Currently unused on SPMv3 & SPMv4
		quint8 returnCode;

		MakeClearBufferCommand(uBitmask);

		send(CLEAR_BUFFER_CMD_LENGTH);
		receive(CLEAR_BUFFER_RSP_LENGTH, CLEAR_BUFFER_RSP);

		ParseClearBufferResponse(&returnCode);

		if (returnCode != EPM_PROTOCOL_STATUS_SUCCESS)
			throw MICRO_EPM_COMM_ERROR;

		if (!(_isSpmV4 == false && _enabledChannelBitmask[0] == 0))
		{
			quint8 pDevice; 
			quint8 pGpioStatus; 
			quint8 pChannelIndex; 
			quint32 puChannelMask; 
			quint32 puStartTimestamp; 
			quint32 puEndTimestamp; 
			quint16 pReadings[1024];
					
			// Get 1 packet of buffered data because to avoid receiving one stale packet
			// This is because the buffer on the PSoC is pre-filled
			MakeGetBufferedDataCommand();
					
			send(GET_BUFFERED_DATA_CMD_LENGTH);
			receive(GET_BUFFERED_DATA_RSP_LENGTH, GET_BUFFERED_DATA_RSP);
	
			ParseGetBufferedDataResponse(&pDevice, &pGpioStatus, &pChannelIndex, &puChannelMask, &puStartTimestamp, &puEndTimestamp, pReadings);
			
			readBulkData(BUF_SIZE);
		}
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::sendHelloCommand()
{
	quint8 uStatusCode;
	quint8 bSpiConnected;
	quint8 bUsbConnected;

	try 
	{
		MakeHelloCommand(MICRO_EPM_PROTOCOL_VERSION, MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION, 0);
	
		send(HELLO_CMD_LENGTH);
		receive(HELLO_RSP_LENGTH, HELLO_RSP);
	
		ParseHelloResponse(&_version, &_channelCount, &uStatusCode, &bSpiConnected, &bUsbConnected);
	
		switch (uStatusCode)
		{
		case EPM_PROTOCOL_STATUS_POWER_ON_TEST_FAILED:
			throw MICRO_EPM_POWER_ON_TEST_FAILED;
			break;
	
		case EPM_PROTOCOL_STATUS_SUCCESS:
			_version._hostProtocolVersion = MICRO_EPM_PROTOCOL_VERSION;
			_version._maxPackets = MAX_PACKETS;
			break;
	
		default:
			throw MICRO_EPM_COMM_ERROR;
		}
	} 
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::tacCommand
(
	const QByteArray& commandStr,
	QByteArray& response
)
{
	if (!_connected)
		throw EPMException(MICRO_EPM_NOT_CONNECTED, 0, "Device Not Connected");

	if (_version._epmID == MICRO_EPM_BOARD_ID_UNKNOWN)
		throw EPMException(MICRO_EPM_TAC_CMD_NOT_SUPPORTED, 0, "TAC Not Supported");

	try
	{
		quint32 commandSize = MakeTacCommand(commandStr);

		send(commandSize);
		receive(BUF_SIZE, EPM_TAC_RSP);

		ParseTacResponse(response);
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::sendSetPeriod()
{
	try
	{
		quint8 receivedStatusCode;

		MakeSetSetPeriodCommand(_setPeriod);

		send(SET_SET_PERIOD_CMD_LENGTH);
		receive(SET_SET_PERIOD_RSP_LENGTH, SET_SET_PERIOD_RSP);

		ParseSetSetPeriodResponse(&receivedStatusCode);

		if (EPM_PROTOCOL_STATUS_SUCCESS != receivedStatusCode)
			throw MICRO_EPM_COMM_ERROR;
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::setGpioDrive_3
(
		GpioPin gpioChannel,
		GpioDrive drive
		)
{
	if (_connected == true)
	{
		GpioPin receivedPin;
		GpioDrive receivedDrive;

		MakeSetGpioDriveCommand(gpioChannel, drive);

		send(SET_GPIO_DRIVE_CMD_LENGTH);
		receive(SET_GPIO_DRIVE_RSP_LENGTH, SET_GPIO_DRIVE_RSP);

		ParseSetGpioDriveResponse(&receivedPin, &receivedDrive);

		if (receivedPin != gpioChannel || receivedDrive != drive)
			throw MICRO_EPM_COMM_ERROR;
	}
	else
	{
		throw MICRO_EPM_NOT_CONNECTED;
	}
}

void _EPMDevice::setRawTimestamp
(
		quint32 rawTimestamp
		)
{
	if (!_connected)
		throw MICRO_EPM_NOT_CONNECTED;

	rawTimestamp = static_cast<quint32>(TIMER_MASK - rawTimestamp);

	try
	{
		quint32 newTimestamp;

		MakeSetSystemTimestampCommand(rawTimestamp);

		send(SET_SYSTEM_TIMESTAMP_CMD_LENGTH);
		receive(SET_SYSTEM_TIMESTAMP_RSP_LENGTH, SET_SYSTEM_TIMESTAMP_RSP);

		ParseSetSystemTimestampResponse(&newTimestamp);
	}
	catch (EPMException& error)
	{
		throw error;
	}
}

void _EPMDevice::setupEepromData()
{
	quint32 uChecksum = 0;
	quint32 i, j;
	quint8* aData;

	aData = (quint8*) &_eeprom;

	for (i = 0; i < MICRO_EPM_STRING_SIZE; i++)
	{
		_eeprom.szSerialNumber[i] = _targetInfo._serialNumber[i];
		_eeprom.szModelNumber[i] = _targetInfo._modelName[i];
	}

	_eeprom.uMajorVersion = EEPROM_MAJOR_VERSION;
	_eeprom.uMinorVersion = EEPROM_MINOR_VERSION;
	_eeprom.EepromProgrammedTime = _targetInfo._eepromProgrammedTime;
	_eeprom.uTargetIdentifier = (quint16) _targetInfo._targetIdentifier;
	_eeprom.uBoardTypeIdentifier = (quint16) _targetInfo._platformIdentifier;
	_eeprom.uBoardOptionIdentifier = (quint16) _targetInfo._boardOptionIdentifier;
	_eeprom.uEepromWriteCount = _targetInfo._eepromWriteCount;

	for (i= 0; i< EEPROM_EPM_MAX_CAL_NUMBER_CHANNELS; i++)
	{
		_eeprom.aCalibrationCodes[i] = aCalibrationCodes[i];
	}

	for (i = 0; i < EEPROM_EPM_MAX_NUMBER_CHANNELS; i++)
	{
		_eeprom.aChannelInfo[i].uSenseResistance = aEepromChannelInfo[i].uSenseResistance;
		_eeprom.aChannelInfo[i].uChannelIdentifier = aEepromChannelInfo[i].uCID;

		for (j = 0; j < (sizeof(_eeprom.aChannelInfo[i].szChannelName)/sizeof(_eeprom.aChannelInfo[i].szChannelName[0])); j++)
		{
			_eeprom.aChannelInfo[i].szChannelName[j] = aEepromChannelInfo[i].szChannelName[j];
		}
	}

	_eeprom.epmBoardUuid = _targetInfo._epmBoardUuid.toMicroEPMUuid();

	for (i = 0; i < EEPROM_SIZE - EEPROM_CHECKSUM_SIZE; i++)
		 uChecksum += aData[i];

	_eeprom.uCheckSum = uChecksum;
}

/**
  Resumes ADC conversions by unpausing the packet buffer.
*/
void _EPMDevice::unpauseAdcConversions
(
	bool throwsException //= true
)
{
	try
	{
		if (_connected == true)
		{
			if (_isSpmV4 == true && _paused == false)
			{
			}
			else
			{
				MakeUnpauseAdcConversionsCommand();

				send(UNPAUSE_ADC_CONVERSIONS_CMD_LENGTH);
				receive(UNPAUSE_ADC_CONVERSIONS_RSP_LENGTH, UNPAUSE_ADC_CONVERSIONS_RSP);

				ParseUnpauseAdcConversionsResponse();

				if (_isSpmV4 == true)
				{
					_paused = false;
				}
			}
		}
		else
		{
			throw MICRO_EPM_NOT_CONNECTED;
		}
	}
	catch (EPMException& error)
	{
		if (throwsException)
			throw error;
	}
}

void _EPMDevice::updateDeviceList()
{
	for (auto& device: _EPMDevice::_activeDevices)
	{
		device->_active = false;

		AppCore* appCore = AppCore::getAppCore();
		if (appCore != Q_NULLPTR)
			appCore->writeToAppLog(QString("Device %1 set to inactive\n").arg(device->_targetInfo._modelName));
	}

#ifdef Q_OS_LINUX
	_LinuxBaseDevice::updateDeviceList();

#endif

#ifdef Q_OS_WIN
	_EPMDevice3::updateDeviceList();
	_EPMDevice4::updateDeviceList();
#endif

}

quint32 _EPMDevice::getEPMDevices
(
	EPMDevices& epmDevices
)
{
	epmDevices.clear();

	updateDeviceList();

	for (const auto& activeDevice : std::as_const(_activeDevices))
	{
		epmDevices.push_back(activeDevice);
	}

	return epmDevices.count();
}

void _EPMDevice::verifyTriggeredModeInput()
{
	uint32_t uBus;
	uint32_t uAdc;
	uint32_t uAdcChannelMask;

	MicroEpmConvTime eRefCurrentConvTime = _MICRO_EPM_CONV_TIME_NUM;
	MicroEpmConvTime eRefVoltageConvTime = _MICRO_EPM_CONV_TIME_NUM;
	AveragingMode eRefAvgMode = _MICRO_EPM_AVERAGING_MODE_NUM;
	bool first{true};

	for (uBus = 0; uBus < MAX_BUSES; uBus++)
	{
		for (uAdc = 0; uAdc < MAX_ADCS_PER_BUS; uAdc++)
		{
			uAdcChannelMask = (_enabledChannelBitmask[uBus] >> (uAdc * 2)) & 0x3;
			if (uAdcChannelMask == 3)
			{
				// Voltage and current are enabled for this ADC
				if (first == true)
				{
					eRefCurrentConvTime = _convTime[uBus][uAdc * 2];
					eRefVoltageConvTime = _convTime[uBus][uAdc * 2 + 1];
					eRefAvgMode = _averagingMode[uBus][uAdc];
					first = false;
				}
				else
				{
					if (_convTime[uBus][uAdc * 2] != eRefCurrentConvTime ||
							_convTime[uBus][uAdc * 2 + 1] != eRefVoltageConvTime ||
							_averagingMode[uBus][uAdc] != eRefAvgMode)
					{
						// Channel settings do not match
						throw MICRO_EPM_INVALID_TRIGGERED_MODE_SETTINGS;
					}
				}
			}
			else if (uAdcChannelMask != 0)
			{
				// Both current and voltage paired channels were not enabled
				throw MICRO_EPM_INVALID_TRIGGERED_MODE_SETTINGS;
			}
		}
	}
}

TargetInfo _EPMDevice::getTargetInfo()
{
	if(_programmed == false)
		throw EPMException(MICRO_EPM_NOT_PROGRAMMED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_PROGRAMMED));

	return _targetInfo;
}

QString _EPMDevice::getTargetName()
{
	return QString(_targetInfo._modelName);
}

QString _EPMDevice::getSerialNumber()
{
	return QString(_targetInfo._serialNumber);
}

QString _EPMDevice::getPlatformIDString()
{
	return PlatformContainer::toString(_targetInfo._platformIdentifier);
}

QString _EPMDevice::getUUID()
{
	return MicroEpmConvertUuidToString(_targetInfo._epmBoardUuid);
}

void _EPMDevice::setChannelEnable
(
	uint32_t uChannel,
	MicroEpmChannelEnable eEnable
)
{
	if (uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS)
		throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, EPMErrorToByteArray(MICRO_EPM_INVALID_PARAMETER));

	if (!_connected)
		throw EPMException(MICRO_EPM_NOT_CONNECTED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_CONNECTED));

	if (uChannel >= MAX_NUM_ADC_CHANNELS)
	{
		/* GPIO channel */
		uint32_t uGpio = uChannel - _channelCount;

		if (eEnable == MICRO_EPM_CHANNEL_ENABLE)
		{
			_gpioEnMask |= 1 << uGpio;
		}
		else if (eEnable == MICRO_EPM_CHANNEL_DISABLE)
		{
			_gpioEnMask &= ~(1 << uGpio);
		}
		else
		{
			throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "Bad Value for eEnable");
		}
	}
	else
	{
		/* Power measurement channel */
		uint32_t uBus = uChannel / MAX_CHANNELS_PER_BUS;
		uint32_t bitmask;

		if (uBus >= MAX_BUSES)
		{
			throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "uBus >= MAX_BUSES");
		}

		if (eEnable == MICRO_EPM_CHANNEL_ENABLE)
		{
			bitmask = _enabledChannelBitmask[uBus];
			bitmask |= (1 << (uChannel % MAX_CHANNELS_PER_BUS));
			_enabledChannelBitmask[uBus] = bitmask;
		}
		else if (eEnable == MICRO_EPM_CHANNEL_DISABLE)
		{
			bitmask = _enabledChannelBitmask[uBus];
			bitmask &= ~(1 << (uChannel % MAX_CHANNELS_PER_BUS));
			_enabledChannelBitmask[uBus] = bitmask;
		}
		else
		{
			throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "eEnable parameter is incorrect");
		}
	}
}

void _EPMDevice::setChannelName
(
	uint32_t uChannel,
	const char* pszName
)
{
	if (uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS)
		throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "(uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS");

	if (_connected != true)
		throw EPMException(MICRO_EPM_NOT_CONNECTED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_CONNECTED));

	if (strlen(pszName) >= (MAX_CHAN_BUFFER_LEN - 1))
		throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "pszName too large");

	memcpy(&_channelInfo[uChannel].szName, pszName, MAX_CHAN_BUFFER_LEN);
}

void _EPMDevice::setChannelRaw
(
	uint32_t uChannel,
	bool dumpRawValues
)
{
	_channelInfo[uChannel]._dumpRawValues = dumpRawValues;
}

void _EPMDevice::setRsense
(
	uint32_t uChannel,
	double dbRsenseMilliOhms
)
{

	if (uChannel >= _channelCount)
		throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "uChannel >= _channelCount");

	if (_connected != true)
		throw EPMException(MICRO_EPM_NOT_CONNECTED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_CONNECTED));

	if (dbRsenseMilliOhms <= 0.0)
		_channelInfo[uChannel].dbRsenseMilliOhms = MICRO_EPM_DEFAULT_RSENSE_MOHM;
	else
		_channelInfo[uChannel].dbRsenseMilliOhms = dbRsenseMilliOhms;
}

void _EPMDevice::setRcmChannel
(
	uint32_t uChannel,
	uint32_t uRcmChannel
)
{

	if (uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS)
		throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS");

	if (_connected != true)
		throw EPMException(MICRO_EPM_NOT_CONNECTED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_CONNECTED));

	_channelInfo[uChannel].uRcmChannel = uRcmChannel;
}

void _EPMDevice::setChannelType
(
	uint32_t uChannel,
	MicroEpmChannelType channelType
)
{
	uint32_t bitmask;

	if (uChannel > 31)
		throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "uChannel > 31");

	if (!_connected)
		throw EPMException(MICRO_EPM_NOT_CONNECTED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_CONNECTED));

	if (_isSpmV4)
		throw EPMException(MICRO_EPM_NOT_IMPLEMENTED, 0, EPMErrorToByteArray(MICRO_EPM_NOT_IMPLEMENTED));

	bitmask = uVoltageChannelBitmask;

	switch (channelType)
	{
	case MICRO_EPM_CHANNEL_TYPE_VOLTAGE:
		bitmask |= (1 << uChannel);
		break;

	case MICRO_EPM_CHANNEL_TYPE_CURRENT:
		bitmask &= ~(1 << uChannel);
		break;

	default:
		throw EPMException(MICRO_EPM_INVALID_PARAMETER, 0, "Bad Channel Type");
	}

	uVoltageChannelBitmask = bitmask;
}

void _EPMDevice::run()
{
	uint32_t uMaxNumSamples;
	MicroEpmChannelData* channelData{Q_NULLPTR};
	uint32_t sampleCount;
	uint32_t uNumOverflow;
	uint32_t uNumEmpty;
	uint32_t uSample;

	_threadExitCode = MICRO_EPM_SUCCESS;

	try
	{
		uMaxNumSamples = _version._maxPackets * _version._maxSamplesPerPacket;
		channelData = new MicroEpmChannelData[uMaxNumSamples];
		if (channelData == Q_NULLPTR)
			throw EPMException(MICRO_EPM_OUT_OF_MEMORY, 0, EPMErrorToByteArray(MICRO_EPM_OUT_OF_MEMORY));

		if (_acquiring)
		{
			if (_udasRecorder)
			{
				_udasRecorder->setEPMDevice(this);
				_udasRecorder->startRecording();
			}

			if (_liveRecorder)
			{
				_liveRecorder->setEPMDevice(this);
				_liveRecorder->startRecording();
			}
		}

		while (_acquiring)
		{
			try
			{
				getBulkData(channelData, uMaxNumSamples, sampleCount, _version._maxPackets, uNumOverflow, uNumEmpty);

				if (sampleCount > 0)
				{
					if (_udasRecorder)
						_udasRecorder->recordData(channelData, sampleCount);

					if (_liveRecorder)
						_liveRecorder->recordData(channelData, sampleCount);
				}

				if (uNumOverflow != 0)
					_threadExitCode = MICRO_EPM_OVERFLOW;

				for (uSample = 0; uSample < sampleCount; uSample++)
				{
					MicroEpmChannelData* pData = &channelData[uSample];

					if (pData->_channelType == MICRO_EPM_CHANNEL_TYPE_GPIO)
					{
						quint32 uGpio;

						for (uGpio = 0; uGpio < MAX_NUM_GPIO_CHANNELS; uGpio++)
						{
							double dbVal;

							if (pData->_rawCode & (1 << uGpio))
							{
								dbVal = GPIO_HIGH_V * MILLI_PER_BASE;
							} else
							{
								dbVal = GPIO_LOW_V * MILLI_PER_BASE;
							}

							logSample(_channelInfo[MAX_NUM_ADC_CHANNELS + uGpio].stats, dbVal);
						}
					}
					else
					{
						logSample(_channelInfo[pData->_channel].stats, pData->_physicalValue);
					}
				}
			}
			catch (EPMException& error)
			{
				_acquiring = false;
				_threadExitCode = error.epmError();
			}
			catch (...)
			{
				_acquiring = false;
				_threadExitCode = MICRO_EPM_ERROR;
			}
		}
	}
	catch (EPMException& error)
	{
		_acquiring = false;
		_threadExitCode = error.epmError();
	}

	try
	{
		if (_udasRecorder)
		{
			_udasRecorder->stopRecording();
			_udasRecorder->setEPMDevice(Q_NULLPTR);
		}

		if (_liveRecorder)
		{
			_liveRecorder->stopRecording();
			_liveRecorder->setEPMDevice(Q_NULLPTR);
		}

		clearRunChannelBitmap();
		sendEnableDisable();
	}
	catch (...)
	{
		_acquiring = false;
		_threadExitCode = MICRO_EPM_ERROR;
	}

	if (channelData)
		delete [] channelData;
}

void _EPMDevice::MakeHelloCommand
(
	quint8 version, 
	quint8 minimumVersion, 
	quint8 statusCode
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = HELLO_CMD;
	_outputBuffer[1] = version;
	_outputBuffer[2] = minimumVersion;
	_outputBuffer[3] = statusCode;
}

void _EPMDevice::MakeGoodbyeCommand
(
	quint8 status
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = GOODBYE_CMD;
	_outputBuffer[1] = status;
}

void _EPMDevice::MakeEnterSecureModeCommand()
{
	clearOutputBuffer();
	
	_outputBuffer[0] = ENTER_SECURE_MODE_CMD;
	_outputBuffer[1] = 'H';
	_outputBuffer[2] = 'W';
	_outputBuffer[3] = '-';
	_outputBuffer[4] = 'E';
	_outputBuffer[5] = 'n';
	_outputBuffer[6] = 'g';
	_outputBuffer[7] = 'i';
	_outputBuffer[8] = 'n';
	_outputBuffer[9] = 'e';
	_outputBuffer[10] = 's';
}

void _EPMDevice::MakeEnterBootloaderCommand()
{
	clearOutputBuffer();

	_outputBuffer[0] = ENTER_BOOTLOADER_CMD;
	_outputBuffer[1] = 'Q';
	_outputBuffer[2] = 'u';
	_outputBuffer[3] = 'a';
	_outputBuffer[4] = 'l';
	_outputBuffer[5] = 'c';
	_outputBuffer[6] = 'o';
	_outputBuffer[7] = 'm';
	_outputBuffer[8] = 'm';
}

void _EPMDevice::MakeSoftwareResetCommand()
{
	clearOutputBuffer();

	_outputBuffer[0] = SOFTWARE_RESET_CMD;
	_outputBuffer[1] = 'S';
	_outputBuffer[2] = 'c';
	_outputBuffer[3] = 'o';
	_outputBuffer[4] = 'r';
	_outputBuffer[5] = 'p';
	_outputBuffer[6] = 'i';
	_outputBuffer[7] = 'o';
	_outputBuffer[8] = 'n';
	_outputBuffer[9] = 'K';
	_outputBuffer[10] = 'r';
	_outputBuffer[11] = 'a';
	_outputBuffer[12] = 'i';
	_outputBuffer[13] = 't';
}

void _EPMDevice::MakeLoopbackRequest
(
	quint8 aData[]
)
{
	clearOutputBuffer();

	_outputBuffer[0] = LOOPBACK_REQUEST;

	for (auto i: range(LOOPBACK_RESPONSE_LENGTH - 1))
		_outputBuffer[i+1] = aData[i];
}

void _EPMDevice::MakeIna231RegisterReadCommand
(
	quint8 uBus,
	quint8 uAddress,
	quint8 uRegister
)
{
	clearOutputBuffer();

	_outputBuffer[0] = INA231_REGISTER_READ_CMD;
	_outputBuffer[1] = uBus;
	_outputBuffer[2] = uAddress;
	_outputBuffer[3] = uRegister;
}

void _EPMDevice::MakeIna231RegisterWriteCommand
(
	quint8 uBus, 
	quint8 uAddress, 
	quint8 uRegister, 
	quint16 uData
)
{
	clearOutputBuffer();

	_outputBuffer[0] = INA231_REGISTER_WRITE_CMD;
	_outputBuffer[1] = uBus;
	_outputBuffer[2] = uAddress;
	_outputBuffer[3] = uRegister;
	_outputBuffer[4] = (uData >>  8) & 0xFF;
	_outputBuffer[5] = (uData >>  0) & 0xFF;
}

void _EPMDevice::MakePowerOnTestCommand
(
	quint8 uNumIterations,
	quint8 uNumBuses,
	const quint16* pauAdcPopulatedMask
)
{
	clearOutputBuffer();

	_outputBuffer[0] = POWER_ON_TEST_CMD;
	_outputBuffer[1] = uNumIterations;
	_outputBuffer[2] = uNumBuses;
	_outputBuffer[3] = (pauAdcPopulatedMask[0] >> 8) & 0xFF;
	_outputBuffer[4] = pauAdcPopulatedMask[0] & 0xFF;
	_outputBuffer[5] = (pauAdcPopulatedMask[1] >> 8) & 0xFF;
	_outputBuffer[6] = pauAdcPopulatedMask[1] & 0xFF;
	_outputBuffer[7] = (pauAdcPopulatedMask[2] >> 8) & 0xFF;
	_outputBuffer[8] = pauAdcPopulatedMask[2] & 0xFF;
	_outputBuffer[9] = (pauAdcPopulatedMask[3] >> 8) & 0xFF;
	_outputBuffer[10] = pauAdcPopulatedMask[3] & 0xFF;
}

void _EPMDevice::MakeGetEpmIDCommand()
{
	clearOutputBuffer();
	
	_outputBuffer[0] = GET_EPM_ID_CMD;
}

void _EPMDevice::MakeCtiControlCommand
(
	quint8 controlbit
)
{	
	clearOutputBuffer();
	
	_outputBuffer[0] = CTI_CONTROL_CMD;
	_outputBuffer[1] = controlbit;
}

void _EPMDevice::MakeGetBufferedDataCommand()
{
	clearOutputBuffer();
	
	_outputBuffer[0] = GET_BUFFERED_DATA_CMD;
}

void _EPMDevice::MakeClearBufferCommand(quint8 uBitmask)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = CLEAR_BUFFER_CMD;
	_outputBuffer[1] = uBitmask;
}

void _EPMDevice::MakePauseAdcConversionsCommand()
{
	clearOutputBuffer();
	
	_outputBuffer[0] = PAUSE_ADC_CONVERSIONS_CMD;
}

void _EPMDevice::MakeUnpauseAdcConversionsCommand()
{
	clearOutputBuffer();
	
	_outputBuffer[0] = UNPAUSE_ADC_CONVERSIONS_CMD;
}

void _EPMDevice::MakeEnableDisableChannelCommand
(
	quint8 device,
	quint32 uChannelBitmask
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = ENABLE_DISABLE_CHANNEL_CMD;
	_outputBuffer[1] = device;
	_outputBuffer[2] = (uChannelBitmask >> 24) & 0xFF;
	_outputBuffer[3] = (uChannelBitmask >> 16) & 0xFF;
	_outputBuffer[4] = (uChannelBitmask >>  8) & 0xFF;
	_outputBuffer[5] = (uChannelBitmask >>  0) & 0xFF;
}

void _EPMDevice::MakeSetChannelTypesCommand
(
	quint8 device, 
	quint32 uChannelBitmask
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = SET_CHANNEL_TYPES_CMD;
	_outputBuffer[1] = device;
	_outputBuffer[2] = (uChannelBitmask >> 24) & 0xFF;
	_outputBuffer[3] = (uChannelBitmask >> 16) & 0xFF;
	_outputBuffer[4] = (uChannelBitmask >>  8) & 0xFF;
	_outputBuffer[5] = (uChannelBitmask >>  0) & 0xFF;
}

void _EPMDevice::MakeEepromReadCommand
(
	quint8 row
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = EEPROM_READ_CMD;
	_outputBuffer[1] = row;
}

void _EPMDevice::MakeEepromWriteCommand
(
	quint8 row,
	quint8 aData[]
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = EEPROM_WRITE_CMD;
	_outputBuffer[1] = row;
	
	for (auto i: range(EEPROM_ROW_SIZE))
	{
		_outputBuffer[2 + i] = aData[i];
	}
}

void _EPMDevice::MakeGetGpioValueCommand
(
	GpioPin pin
)
{
	clearOutputBuffer();

	_outputBuffer[0] = GET_GPIO_VALUE_CMD;
	_outputBuffer[1] = (quint8) pin;
}

void _EPMDevice::MakeSetGpioValueCommand
(
	GpioPin pin,
	MicroEpmGpioValue value
)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_GPIO_VALUE_CMD;
	_outputBuffer[1] = (quint8) pin;
	_outputBuffer[2] = (quint8) value;
}

void _EPMDevice::MakeSetGpioDriveCommand
(
	GpioPin pin,
	GpioDrive drive
)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_GPIO_DRIVE_CMD;
	_outputBuffer[1] = (quint8) pin;
	_outputBuffer[2] = (quint8) drive;
}

void _EPMDevice::MakeGetGpioDriveCommand
(
	GpioPin pin
)
{
	clearOutputBuffer();

	_outputBuffer[0] = GET_GPIO_DRIVE_CMD;
	_outputBuffer[1] = (quint8) pin;
}

void _EPMDevice::MakeSetGpioDirectionCommand
(
	GpioPin pin,
	MicroEpmGpioDirection direction
)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_GPIO_DIRECTION_CMD;
	_outputBuffer[1] = (quint8) pin;
	_outputBuffer[2] = (quint8) direction;
}

void _EPMDevice::MakeSetGpioBufferStatusCommand
(
	quint8 uGpios
)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_GPIO_BUFFER_STATUS_CMD;
	_outputBuffer[1] = uGpios;
}

void _EPMDevice::MakeGetGpioBufferCommand()
{
	clearOutputBuffer();

	_outputBuffer[0] = GET_GPIO_BUFFER_CMD;
}

void _EPMDevice::MakeSetChannelSwitchDelayCommand
(
	quint8 device, 
	quint32 uDelay
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = SET_CHANNEL_SWITCH_DELAY_CMD;
	_outputBuffer[1] = device;
	_outputBuffer[2] = (uDelay >> 24) & 0xFF;
	_outputBuffer[3] = (uDelay >> 16) & 0xFF;
	_outputBuffer[4] = (uDelay >>  8) & 0xFF;
	_outputBuffer[5] = (uDelay >>  0) & 0xFF;
}

void _EPMDevice::MakeSetAveragingCommand
(
		quint8 uBus,
		const AveragingMode* paeAveragingMode
		)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = SET_AVERAGING_CMD;
	_outputBuffer[1] = uBus;
	
	for (auto uAdc: range(MAX_ADCS_PER_BUS))
		_outputBuffer[2 + uAdc] = (quint8) paeAveragingMode[uAdc];
}

void _EPMDevice::MakeSetAdcModeCommand
(
	MicroEpmAdcMode eAdcMode
)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_ADC_MODE_CMD;
	_outputBuffer[1] = (quint8)eAdcMode;
}

void _EPMDevice::MakeSetSetPeriodCommand(quint32 uSetPeriod)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_SET_PERIOD_CMD;
	_outputBuffer[1] = (uSetPeriod >> 24) & 0xFF;
	_outputBuffer[2] = (uSetPeriod >> 16) & 0xFF;
	_outputBuffer[3] = (uSetPeriod >>  8) & 0xFF;
	_outputBuffer[4] = (uSetPeriod >>  0) & 0xFF;
}

void _EPMDevice::MakeSetConversionTimeCommand
(
	quint8 uBus,
	const MicroEpmConvTime* paeConvTime
)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_CONVERSION_TIME_CMD;
	_outputBuffer[1] = uBus;

	for (auto uChannel: range(MAX_CHANNELS_PER_BUS))
		_outputBuffer[2 + uChannel] = (quint8) paeConvTime[uChannel];
}

void _EPMDevice::MakeSetDataRateGovernorCommand
(
	quint32 uMaxDataRate
)
{
	clearOutputBuffer();

	_outputBuffer[0] = SET_DATA_RATE_GOVERNOR_CMD;
	_outputBuffer[1] = (uMaxDataRate >> 24) & 0xFF;
	_outputBuffer[2] = (uMaxDataRate >> 16) & 0xFF;
	_outputBuffer[3] = (uMaxDataRate >>  8) & 0xFF;
	_outputBuffer[4] = (uMaxDataRate >>  0) & 0xFF;
}

void _EPMDevice::MakeApplySettingsCommand()
{
	clearOutputBuffer();

	_outputBuffer[0] = APPLY_SETTINGS_CMD;
}

quint32 _EPMDevice::MakeTacCommand
(
	const QByteArray &commandString
)
{
	QByteArray commandCandidate{commandString};
	commandCandidate.replace(QByteArray("\n"), QByteArray(""));
	commandCandidate.replace(QByteArray("\r"), QByteArray(""));

	qsizetype commandSize{commandCandidate.size()};

	Q_ASSERT(static_cast<quint64>(commandSize) < sizeof(_outputBuffer) - 2);
	
	clearOutputBuffer();
	
	_outputBuffer[0] = EPM_TAC_CMD;
	for (auto index: range(commandSize))
		_outputBuffer[1 + index] = commandCandidate[index];
	
	_outputBuffer[1 + commandSize] = '\r';

	return commandSize + 2;
}

void _EPMDevice::MakeGetSystemTimestampCommand()
{
	clearOutputBuffer();
	
	_outputBuffer[0] = GET_SYSTEM_TIMESTAMP_CMD;
}

void _EPMDevice::MakeSetSystemTimestampCommand
(
	quint32 uTimestamp
)
{
	clearOutputBuffer();
	
	_outputBuffer[0] = SET_SYSTEM_TIMESTAMP_CMD;
	_outputBuffer[1] = (uTimestamp >> 24) & 0xFF;
	_outputBuffer[2] = (uTimestamp >> 16) & 0xFF;
	_outputBuffer[3] = (uTimestamp >>  8) & 0xFF;
	_outputBuffer[4] = (uTimestamp >>  0) & 0xFF;
}

void _EPMDevice::ParseHelloResponse
(
	EpmVersionInfo* pVersion, 
	quint8* pNumberOfChannels, 
	quint8* puStatus, 
	quint8* pbSpiConnected, 
	quint8* pbUsbConnected
)
{
	Q_ASSERT(pVersion != Q_NULLPTR);
	Q_ASSERT(pNumberOfChannels != Q_NULLPTR);
	Q_ASSERT(puStatus != Q_NULLPTR);
	Q_ASSERT(pbSpiConnected != Q_NULLPTR);
	Q_ASSERT(pbUsbConnected != Q_NULLPTR);
	
	pVersion->_firmwareProtocolVersion = _inputBuffer[1];
	pVersion->_firmwareLowestCompatibleProtocolVersion = _inputBuffer[2];
	pVersion->_firmwareVersion[0] = _inputBuffer[3];
	pVersion->_firmwareVersion[1] = _inputBuffer[4];
	pVersion->_firmwareVersion[2] = _inputBuffer[5];
	pVersion->_firmwareVersion[3] = _inputBuffer[6];
	
	*pNumberOfChannels = _inputBuffer[7];
	*puStatus = (_inputBuffer[8] & EPM_HELLO_STATUS_STATUS_MASK) >> EPM_HELLO_STATUS_STATUS_SHIFT;
	*pbSpiConnected = (_inputBuffer[8] & EPM_HELLO_STATUS_SPI_MASK) >> EPM_HELLO_STATUS_SPI_SHIFT;
	*pbUsbConnected = (_inputBuffer[8] & EPM_HELLO_STATUS_USB_MASK) >> EPM_HELLO_STATUS_USB_SHIFT;
}

void _EPMDevice::ParseGoodbyeResponse
(
	quint8* pReturnCode
)
{	
	Q_ASSERT(pReturnCode != Q_NULLPTR);
	
	*pReturnCode = _inputBuffer[1];
}

void _EPMDevice::ParseGetBufferedDataResponse
(
	quint8* pDevice, 
	quint8* pGpioStatus, 
	quint8* pChannelIndex, 
	quint32* puChannelMask, 
	quint32* puStartTimestamp, 
	quint32* puEndTimestamp, 
	quint16 pReadings[]
)
{
	Q_ASSERT(pDevice != Q_NULLPTR);
	Q_ASSERT(pGpioStatus != Q_NULLPTR);
	Q_ASSERT(pChannelIndex != Q_NULLPTR);
	Q_ASSERT(puChannelMask != Q_NULLPTR);
	Q_ASSERT(puStartTimestamp != Q_NULLPTR);
	Q_ASSERT(puEndTimestamp != Q_NULLPTR);
	
	*pDevice = _inputBuffer[1];
	*pGpioStatus = _inputBuffer[2];
	*pChannelIndex = _inputBuffer[3];
	*puChannelMask = (_inputBuffer[4] << 24) | (_inputBuffer[5] << 16) | (_inputBuffer[6] << 8) | _inputBuffer[7];
	
	*puStartTimestamp = (_inputBuffer[8] << 24) | (_inputBuffer[9] << 16) | (_inputBuffer[10] << 8) | _inputBuffer[11];
	*puEndTimestamp = (_inputBuffer[12] << 24) | (_inputBuffer[13] << 16) | (_inputBuffer[14] << 8) | _inputBuffer[15];
	
	for (auto i: range(24)) 
		pReadings[i] = (_inputBuffer[ 16 + 2 * i ] << 8) | _inputBuffer[ 16 + 2 * i + 1 ];
}

void _EPMDevice::ParseClearBufferResponse(quint8* puReturnCode)
{
	Q_ASSERT(puReturnCode != Q_NULLPTR);
	
	*puReturnCode = _inputBuffer[1];
}

void _EPMDevice::ParsePauseAdcConversionsResponse()
{
}

void _EPMDevice::ParseUnpauseAdcConversionsResponse()
{
}

void _EPMDevice::ParseEnableDisableChannelResponse
(
		quint8* pStatus, 
		quint32* puChannelBitmask
		)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	Q_ASSERT(puChannelBitmask != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
	*puChannelBitmask = (_inputBuffer[2] << 24) | (_inputBuffer[3] << 16) | (_inputBuffer[4] << 8) | _inputBuffer[5];
}

void _EPMDevice::ParseSetChannelTypesResponse
(
		quint8* pDevice, 
		quint32* puChannelBitmask
		)
{
	Q_ASSERT(pDevice != Q_NULLPTR);
	Q_ASSERT(puChannelBitmask != Q_NULLPTR);
	
	*pDevice = _inputBuffer[1];
	*puChannelBitmask = (_inputBuffer[2] << 24) | (_inputBuffer[3] << 16) | (_inputBuffer[4] << 8) | _inputBuffer[5];
}

void _EPMDevice::ParseSetChannelSwitchDelayResponse
(
	quint8* pDevice, 
	quint32* puDelay
)
{
	Q_ASSERT(pDevice != Q_NULLPTR);
	Q_ASSERT(puDelay != Q_NULLPTR);
	
	*pDevice = _inputBuffer[1];
	*puDelay = (_inputBuffer[2] << 24) | (_inputBuffer[3] << 16) | (_inputBuffer[4] << 8) | _inputBuffer[5];
}

void _EPMDevice::ParseSetAveragingResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParseSetAdcModeResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParseSetSetPeriodResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParseSetConversionTimeResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParseSetDataRateGovernorResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParseApplySettingsResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParseEepromReadResponse
(
	quint8 uRow, 
	quint8* pData
)
{
	quint16 index = 0x3;
	
	Q_ASSERT(pData != Q_NULLPTR);
	
	if (uRow >= EEPROM_NUMBER_OF_ROWS) 
		throw MICRO_EPM_INVALID_PARAMETER;
	
	if ((EEPROM_READ_RSP != _inputBuffer[0]) || (_inputBuffer[1] != 0) || (_inputBuffer[2] != uRow)) 
		throw MICRO_EPM_INVALID_PARSED_DATA;
	
	for(auto i: range(EEPROM_ROW_SIZE)) 
		pData[i + (uRow * EEPROM_ROW_SIZE)] = _inputBuffer[index + i];
}

void _EPMDevice::ParseEepromWriteResponse
(
	quint8 uRow, 
	quint8* pData
)
{
	quint16 index = 0x3;
	
	Q_ASSERT(pData != Q_NULLPTR);
	
	if ((EEPROM_WRITE_RSP != _inputBuffer[0]) || (_inputBuffer[1] != 0) || (_inputBuffer[2] != uRow)) 
		throw MICRO_EPM_INVALID_PARSED_DATA;
	
	for (auto i: range(EEPROM_ROW_SIZE)) 
	{
		if (pData[i] != _inputBuffer[index + i]) 
		{
			throw MICRO_EPM_INVALID_PARSED_DATA;
		}
	}
}

void _EPMDevice::ParseGetGpioValueResponse
(
	GpioPin* pin, 
	MicroEpmGpioValue* reading
)
{
	Q_ASSERT(pin != Q_NULLPTR);
	Q_ASSERT(reading != Q_NULLPTR);
	
	*pin = (GpioPin) _inputBuffer[1];
	*reading = (MicroEpmGpioValue) _inputBuffer[2];
}

void _EPMDevice::ParseSetGpioValueResponse
(
	GpioPin* pin, 
	MicroEpmGpioValue* value
)
{
	Q_ASSERT(pin != Q_NULLPTR);
	Q_ASSERT(value != Q_NULLPTR);
	
	*pin = (GpioPin) _inputBuffer[1];
	*value = (MicroEpmGpioValue) _inputBuffer[2];
}

void _EPMDevice::ParseSetGpioDirectionResponse
(
	GpioPin* pin, 
	MicroEpmGpioDirection* direction
)
{
	Q_ASSERT(pin != Q_NULLPTR);
	Q_ASSERT(direction != Q_NULLPTR);
	
	*pin = (GpioPin) _inputBuffer[1];
	*direction = (MicroEpmGpioDirection) _inputBuffer[2];
}

void _EPMDevice::ParseSetGpioDriveResponse
(
	GpioPin* pin, 
	GpioDrive* drive
)
{
	Q_ASSERT(pin != Q_NULLPTR);
	Q_ASSERT(drive != Q_NULLPTR);
	
	*pin = (GpioPin) _inputBuffer[1];
	*drive = (GpioDrive) _inputBuffer[2];
}

void _EPMDevice::ParseGetGpioDriveResponse
(
	GpioPin* pin, 
	GpioDrive* drive
)
{
	Q_ASSERT(pin != Q_NULLPTR);
	Q_ASSERT(drive != Q_NULLPTR);
	
	*pin = (GpioPin) _inputBuffer[1];
	*drive = (GpioDrive) _inputBuffer[2];
}

void _EPMDevice::ParseSetGpioBufferStatusResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParseGetGpioBufferResponse
(
	quint8* pStatus, 
	quint8* pGpioData, 
	quint32* puTimestamp
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	Q_ASSERT(pGpioData != Q_NULLPTR);
	Q_ASSERT(puTimestamp != Q_NULLPTR);
	
	*pStatus =  _inputBuffer[1];
	*pGpioData =  _inputBuffer[2];
	*puTimestamp = (_inputBuffer[3] << 24) | (_inputBuffer[4] << 16) | (_inputBuffer[5] << 8) | _inputBuffer[6];
}

void _EPMDevice::ParseMemoryReadResponse
(
	quint8* pSize, 
	quint32* puAddress, 
	quint32* puData
)
{	
	Q_ASSERT(pSize != Q_NULLPTR);
	Q_ASSERT(puAddress != Q_NULLPTR);
	Q_ASSERT(puData != Q_NULLPTR);
	
	*pSize = _inputBuffer[1];
	*puAddress = (_inputBuffer[2] << 24) | (_inputBuffer[3] << 16) | (_inputBuffer[4] << 8) | _inputBuffer[5];
	*puData = (_inputBuffer[6] << 24) | (_inputBuffer[7] << 16) | (_inputBuffer[8] << 8) | _inputBuffer[9];
}

void _EPMDevice::ParseMemoryWriteBytesResponse
(
	quint8* pSize, 
	quint32* puAddress, 
	quint8* puData
)
{
	Q_ASSERT(pSize != Q_NULLPTR);
	Q_ASSERT(puAddress != Q_NULLPTR);
	Q_ASSERT(puData != Q_NULLPTR);
	
	*pSize = _inputBuffer[1];
	*puAddress = (_inputBuffer[2] << 24) | (_inputBuffer[3] << 16) | (_inputBuffer[4] << 8) | _inputBuffer[5];
	
	for (auto i: range(*pSize)) 
		puData[i] = _inputBuffer[6 + i];
}

void _EPMDevice::ParseEnterSecureModeResponse(quint8* pReturnCode)
{
	Q_ASSERT(pReturnCode != Q_NULLPTR);
	
	*pReturnCode = _inputBuffer[1];
}

void _EPMDevice::ParseEnterBootloaderResponse(quint8* pReturnCode)
{
	Q_ASSERT(pReturnCode != Q_NULLPTR);
	
	*pReturnCode = _inputBuffer[1];
}

void _EPMDevice::ParseSoftwareResetResponse(quint8* pRetCode)
{
	Q_ASSERT(pRetCode != Q_NULLPTR);
	
	*pRetCode = _inputBuffer[1];
}

void _EPMDevice::ParseLoopbackResponse
(
	quint8 aData[]
)
{
	for (auto i: range(LOOPBACK_RESPONSE_LENGTH - 1)) 
		aData[i] = _inputBuffer[i+1];
}

void _EPMDevice::ParseIna231RegisterReadResponse
(
	quint8* pStatus, 
	quint16* puData
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	Q_ASSERT(puData != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
	*puData = (_inputBuffer[2] << 8) | _inputBuffer[3];
}

void _EPMDevice::ParseIna231RegisterWriteResponse
(
	quint8* pStatus
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
}

void _EPMDevice::ParsePowerOnTestResponse
(
	quint8* pStatus, 
	quint16* pauAdcFailedMask
)
{
	Q_ASSERT(pStatus != Q_NULLPTR);
	Q_ASSERT(pauAdcFailedMask != Q_NULLPTR);
	
	*pStatus = _inputBuffer[1];
	pauAdcFailedMask[0] = _inputBuffer[2] << 8;
	pauAdcFailedMask[0] |= (quint16) _inputBuffer[3];
	
	pauAdcFailedMask[1] = _inputBuffer[4] << 8;
	pauAdcFailedMask[1] |= (quint16) _inputBuffer[5];
	
	pauAdcFailedMask[2] = _inputBuffer[6] << 8;
	pauAdcFailedMask[2] |= (quint16) _inputBuffer[7];
	
	pauAdcFailedMask[3] = _inputBuffer[8] << 8;
	pauAdcFailedMask[3] |= (quint16) _inputBuffer[9];
}

void _EPMDevice::ParseGetEpmIDResponse
(
	PlatformID* pPlatformID
)
{	
	if (_inputBuffer[1] != 0 )
		throw MICRO_EPM_ERROR;
	
	*pPlatformID = _inputBuffer[2];
}

void _EPMDevice::ParseCtiControlResponse()
{
	if (_inputBuffer[1] != 0 ) 
		throw MICRO_EPM_ERROR;
}

void _EPMDevice::ParseTacResponse
(
	QByteArray& response
)
{
	if (_inputBuffer[0] != EPM_TAC_RSP)
		throw EPMException(MICRO_EPM_COMM_ERROR, 0, QString("%1 expected, %2 recieved.").arg(EPM_TAC_RSP).arg(_inputBuffer[0]).toLatin1());

	quint32 responseSize = static_cast<quint32>(_inputBuffer[2]);

	response.clear();

	switch (_inputBuffer[1])
	{
	case 0:
		for (auto index: range(responseSize))
			response += _inputBuffer[3 + index] ;

		response += '\0' ;
		break;

	case 1:
		throw EPMException(MICRO_EPM_TAC_CMD_NOT_SUPPORTED, 0, QString("Command not Supported").toLatin1());

	default:
		throw EPMException(MICRO_EPM_ERROR_INVALID_RESPONSE_CMD, 0, QString("%1 not equal 0.").arg(_inputBuffer[1]).toLatin1());
	}
}

void _EPMDevice::ParseGetSystemTimestampResponse
(
	quint32* puTimestamp
)
{
	Q_ASSERT(puTimestamp != Q_NULLPTR);
	
	*puTimestamp = (_inputBuffer[1] << 24) | (_inputBuffer[2] << 16) | (_inputBuffer[3] << 8) | _inputBuffer[4];
}

void _EPMDevice::ParseSetSystemTimestampResponse
(
		quint32* puTimestamp
		)
{
	Q_ASSERT(puTimestamp != Q_NULLPTR);
	
	*puTimestamp = (_inputBuffer[1] << 24) | (_inputBuffer[2] << 16) | (_inputBuffer[3] << 8) | _inputBuffer[4];
}

void _EPMDevice::ParseMemoryWriteResponse
(
	quint8* pSize,
	quint32* puAddress,
	quint32* puData
)
{
	Q_ASSERT(pSize != Q_NULLPTR);
	Q_ASSERT(puAddress != Q_NULLPTR);
	Q_ASSERT(puData != Q_NULLPTR);
	
	*pSize = _inputBuffer[1];
	*puAddress = (_inputBuffer[2] << 24) | (_inputBuffer[3] << 16) | (_inputBuffer[4] << 8) | _inputBuffer[5];
	*puData = (_inputBuffer[6] << 24) | (_inputBuffer[7] << 16) | (_inputBuffer[8] << 8) | _inputBuffer[9];
}

void _EPMDevice::ParseMemoryReadBytesResponse
(
	quint8* pSize, 
	quint32* puAddress, 
	quint8* puData
)
{
	Q_ASSERT(pSize != Q_NULLPTR);
	Q_ASSERT(puAddress != Q_NULLPTR);
	Q_ASSERT(puData != Q_NULLPTR);
	
	*pSize = _inputBuffer[1];
	*puAddress  = (_inputBuffer[2] << 24) | (_inputBuffer[3] << 16) | (_inputBuffer[4] << 8) | _inputBuffer[5];
	
	for (auto i: range(*pSize)) 
		puData[i] = _inputBuffer[6 + i];
}


