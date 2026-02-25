#ifndef EPMDEVICE_H
#define EPMDEVICE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/


#include "EPMGlobalLib.h"

// QCommonConsole
#include "AppCore.h"
#include "StringUtilities.h"

//EPMLib
#include "EPMLibDefines.h"
#include "EPMChannel.h"
#include "MicroEpmErrors.h"
class RecordingInterface;
#include "TargetInfo.h"

// Qt
#include <QMap>
#include <QSharedPointer>
#include <QThread>
#include <QSet>

class _EPMDevice;

typedef QSharedPointer<_EPMDevice> EPMDevice;
typedef QList<EPMDevice> EPMDevices;
typedef QSet<HashType> EPMDeviceHashes;


class EPMLIB_EXPORT _EPMDevice :
	public QThread
{
	Q_OBJECT
public:
	_EPMDevice();
	~_EPMDevice();

	bool programmed()
	{
		return _programmed;
	}

	virtual QString serialNumber() = 0;

	HashType hash();
	QString devicePath()
	{
		return _devicePath;
	}

	void setUDASRecorder(RecordingInterface* recordingInterface);
	void removeUDASRecorder();

	void setLiveRecorder(RecordingInterface* recordingInterface);
	void removeLiveRecorder();

	void addChannel(const EPMChannel epmRunChannel);
	void removeChannel(const EPMChannel epmRunChannel);

	void applySettings();

	void acquire();
	void stop();

	EpmErrorCode threadExitCode()
	{
		return _threadExitCode;
	}

	static void updateDeviceList();
	static quint32 getEPMDevices(EPMDevices& tacDevices);
	static quint32 deviceCount();
	static EPMDevice device(quint32 index);
	static EPMDevice deviceByHash(HashType hash);
	static EPMDevice deviceByHash(HashType hash, const EPMDevices& epmDevices);
	static EPMDevice deviceByName(const QString& name);
	static EPMDevice deviceByName(const QString& name, const EPMDevices& epmDevices);
	static void addDevice(EPMDevice& epmDevice);
	static bool initializeDevice(EPMDevice& epmDevice);
	void getNumBusesAndAdcMasks();

	QString connectionStatus()
	{
		return _connectionStatus;
	}

	virtual void open();
	virtual void close() = 0;

	EpmErrorCode send(unsigned char* buffer, quint32 bufferSize);
	EpmErrorCode receive(unsigned char* buffer, quint32 bufferSize);

	void enterSecureMode();
	void verifyTriggeredModeInput();

	static quint32 countNumberOfSpmBoards();

	void sendHelloCommand();

	// TacCommand
	void tacCommand(const QByteArray& commandStr, QByteArray& response);

	// Channel Commands
	void sendEnableDisable();

	// Buffered Data
	void clearBuffer();
	void pauseAdcConversions(bool throwsException = true);
	void unpauseAdcConversions(bool throwsException = true);

	// GPIO
	void setGpioDrive_3(GpioPin gpioChannel, GpioDrive drive);
	MicroEpmGpioValue getGpioValue(GpioPin gpioChannel);

	// Eeprom
	void setupEepromData();
	void eepromErase();
	void eepromRead(bool ignoreChecksum = true);
	void eepromWrite();
	void parseEepromData(quint8* aData, bool ignoreChecksum);

	// target info
	TargetInfo getTargetInfo();
	QString getTargetName();
	QString getSerialNumber();
	QString getPlatformIDString();
	QString getUUID();

	// Timestamps
	quint32 getRawTimestamp();
	void setRawTimestamp(quint32 uRawTimestamp);

	// Register
	void ina231RegisterRead(quint8 uBus, quint8 uAddress, quint8 uRegister, quint16* puData);
	void ina231RegisterWrite(quint8 uBus, quint8 uAddress, quint8 uRegister,  quint16 uData);

	// Channel Config - SPMv3
	//MicroEpmError MakeSetChannelSwitchDelayCommand(quint8* pBuffer, quint8 length, quint8 device, quint32 uDelay);

	// Channel Config - SPMv4
	PlatformID getPlatformID();


	/**
	  Used to signal the firmware to enter the bootloader. This function may not
	  return successfully even if the device enters the bootloader. This is due
	  to the loss of the communciations link when the switch is made.
	*/
	void enterBootloader();

	void sendAveragingMode();
	void sendAdcMode();
	void sendSetPeriod();
	void sendConvTime();
	void sendDataRateGovernor();
	void sendGpioEnable();
	void sendApplySettings();

	bool						_initialized{false};
	HashType					_hash{0};
	QByteArray					_devicePath;
	bool						_active{false};
	quint32						_index{0xFFFFFFFF};
	_EPMChannel::ChannelType	_channelType{_EPMChannel::eNotSet};

	MicroEpmEepromChannelInfo	aEepromChannelInfo[EEPROM_EPM_MAX_NUMBER_CHANNELS];
	MicroEpmChannelInfo			_channelInfo[MICRO_EPM_MAX_NUMBER_CHANNELS];
	EpmVersionInfo				_version;
	MicroEpmEepromPacked		_eeprom;
	TargetInfo					_targetInfo;
	quint32						_enabledChannelBitmask[MAX_BUSES];
	qint16						aCalibrationCodes[EEPROM_EPM_MAX_CAL_NUMBER_CHANNELS];
	quint8						_inputBuffer[BUF_SIZE];
	quint8						_outputBuffer[BUF_SIZE];
	time_t						eepromCalibrationTime;
	quint8						_bigBuffer[BUF_SIZE*  MAX_PACKETS];
	quint32						uTarget{0};
	quint32						uVoltageChannelBitmask{0};   
	quint16						_adcPopulatedMask[MAX_BUSES];
	quint32						uTimestampOffset{0};
	bool						_timestampOffsetPositive{false};
	bool						_connected{false};
	quint8						_channelCount{0};
	bool						_readEeprom{false};
	bool						_programmed{false};
	bool						_isSPM{false};
	bool						_isSpmV4{false};
	AveragingMode				_averagingMode[MAX_BUSES][MAX_ADCS_PER_BUS];		// SPMv4 only
	MicroEpmConvTime			_convTime[MAX_BUSES][MAX_CHANNELS_PER_BUS];			// SPMv4 only
	MicroEpmAdcMode				_adcMode{MICRO_EPM_ADC_MODE_CONTINUOUS};										// SPMv4 only
	RecordingFormat				eFormat{_MICRO_EPM_RECORDING_FORMAT_MAX};										// SPMv4 only
	quint32						_setPeriod{0};										// SPMv4 only
	quint32						_maxDataRate{0};									// SPMv4 only
	quint8						_numBuses{0};										// SPMv4 only
	bool						_paused{false};										// SPMv4 only
	bool						_secureMode{false};									// SPMv4 only
	quint8						_gpioEnMask{0};										// SPMv4 only
	bool						_acquiring{false};									// SPMv4 only
	bool						_errorStatus{false};

protected:

	inline void clearBigBuffer()
	{
		memset(_bigBuffer, 0, sizeof (_bigBuffer));
	}

	inline void clearInputBuffer()
	{
		memset(_inputBuffer, 0, sizeof(_inputBuffer));
	}

	inline void clearOutputBuffer()
	{
		memset(_outputBuffer, 0, sizeof(_outputBuffer));
	}

	virtual void send(quint32 uLength) = 0;
	virtual void receive(quint32 uRequestedLength, quint8 commandCode) = 0;
	virtual void readBulkData(quint32 uRequestedLength) = 0;
	
	virtual void getBulkData(MicroEpmChannelData* channelData, quint32 channelDataSize, quint32& sampleCount, quint32 packetCount,
		quint32& overflowCount, quint32& emptyCount) = 0;

	void logSample(MicroEpmChannelStat& stats, double physical);

	bool checkReturnCode(quint8 commandCode);

	void log(const QString& message);

	static EPMDevices				_activeDevices;
	static EPMDevices				_inactiveDevices;

	AppCore*						_appCore{Q_NULLPTR};
	EPMChannelMap					_runChannels;
	QString							_resultsPath;
	QString							_connectionStatus;
	EpmErrorCode					_threadExitCode;
	RecordingInterface*				_udasRecorder{Q_NULLPTR};
	RecordingInterface*				_liveRecorder{Q_NULLPTR};

	void clearRunChannelBitmap();
	void setupRunChannelBitmap();

	void setChannelEnable(uint32_t uChannel, MicroEpmChannelEnable eEnable);
	void setChannelName(uint32_t uChannel, const char* pszName);
	void setChannelRaw(uint32_t uChannel, bool dumpRawValues);
	void setRsense(uint32_t uChannel, double dbRsenseMilliOhms);
	void setRcmChannel(uint32_t uChannel, uint32_t uRcmChannel);
	void setChannelType(uint32_t uChannel, MicroEpmChannelType channelType);


protected: // QThread
	virtual void run();

	qint32 _overflowCount{0};
	qint32 _errorCount{0};
	qint32 _timeErrorCount{0};

// Packet builders
	
	// Init / Deinit
	void MakeHelloCommand(quint8 version, quint8 minimumVersion, quint8 statusCode);
	void MakeGoodbyeCommand(quint8 status);
	
	// Misc
	void MakeEnterSecureModeCommand();
	void MakeEnterBootloaderCommand();
	void MakeSoftwareResetCommand();
	void MakeLoopbackRequest(quint8 aData[]);
	void MakeIna231RegisterReadCommand(quint8 uBus, quint8 uAddress, quint8 uRegister);
	void MakeIna231RegisterWriteCommand(quint8 uBus, quint8 uAddress, quint8 uRegister, quint16 uData);
	void MakePowerOnTestCommand(quint8 uNumIterations, quint8 uNumBuses, const quint16* pauAdcPopulatedMask);
	void MakeGetEpmIDCommand();
	void MakeCtiControlCommand(quint8 controlbit);
	
	// Buffered Data
	void MakeGetBufferedDataCommand();
	void MakeClearBufferCommand(quint8 uBitmask);
	void MakePauseAdcConversionsCommand();
	void MakeUnpauseAdcConversionsCommand();
		
	// Channel Config
	void MakeEnableDisableChannelCommand(quint8 device, quint32 uChannelBitmask);
	void MakeSetChannelTypesCommand(quint8 device, quint32 uChannelBitmask);
	
	// Channel Config - SPMv3
	void MakeSetChannelSwitchDelayCommand(quint8 device, quint32 uDelay);
	
	// Channel Config - SPMv4
	void MakeSetAveragingCommand(quint8 uBus, const AveragingMode* paeAveragingMode);
	void MakeSetAdcModeCommand(MicroEpmAdcMode eAdcMode);
	void MakeSetSetPeriodCommand(quint32 uSetPeriod);
	void MakeSetConversionTimeCommand(quint8 uBus, const MicroEpmConvTime* paeConvTime);
	void MakeSetDataRateGovernorCommand(quint32 uMaxDataRate);
	void MakeApplySettingsCommand();

	// EEPROM
	void MakeEepromReadCommand(quint8 row);
	void MakeEepromWriteCommand(quint8 row, quint8 aData[]);
	
	// GPIO
	void MakeGetGpioValueCommand(GpioPin pin);
	void MakeSetGpioValueCommand(GpioPin pin, MicroEpmGpioValue value);
	void MakeSetGpioDriveCommand(GpioPin pin, GpioDrive drive);
	void MakeGetGpioDriveCommand(GpioPin pin);
	void MakeSetGpioDirectionCommand(GpioPin pin, MicroEpmGpioDirection direction);
	void MakeSetGpioBufferStatusCommand(quint8 uGpios);
	void MakeGetGpioBufferCommand();

	// TAC command
	quint32 MakeTacCommand(const QByteArray& commandString);
	
	// Timestamp
	void MakeGetSystemTimestampCommand();
	void MakeSetSystemTimestampCommand(quint32 uTimestamp);
	
// Packet Parsers

	// Init / Deinit
	void ParseHelloResponse(EpmVersionInfo* pVersion, quint8* pNumberOfChannels, quint8* puStatus, quint8* pbSpiConnected, quint8* pbUsbConnected);
	void ParseGoodbyeResponse(quint8* pReturnCode);
	
	// Buffered Data
	void ParseGetBufferedDataResponse(quint8* pDevice, quint8* pGpioStatus, quint8* pChannelIndex, quint32* puChannelMask, quint32* puStartTimestamp,
		quint32* puEndTimestamp, quint16 pReadings[]);	
	void ParseClearBufferResponse(quint8* puReturnCode);
	void ParsePauseAdcConversionsResponse();
	void ParseUnpauseAdcConversionsResponse();

	// Channel Config
	void ParseEnableDisableChannelResponse(quint8* pStatus, quint32* puChannelBitmask);
	void ParseSetChannelTypesResponse(quint8* pDevice, quint32* puChannelBitmask);
	
	// Channel Config - SPMv3
	void ParseSetChannelSwitchDelayResponse(quint8* pDevice, quint32* puDelay);

	// Channel Config - SPMv4
	void ParseSetAveragingResponse(quint8* pStatus);
	void ParseSetAdcModeResponse(quint8* pStatus);
	void ParseSetSetPeriodResponse(quint8* pStatus);
	void ParseSetConversionTimeResponse(quint8* pStatus);
	void ParseSetDataRateGovernorResponse(quint8* pStatus);
	void ParseApplySettingsResponse(quint8* pStatus);
	
	// EEPROM
	void ParseEepromReadResponse(quint8 uRow, quint8* pData);
	void ParseEepromWriteResponse(quint8 uRow, quint8* pData);
	
	// GPIO
	void ParseGetGpioValueResponse(GpioPin* pin, MicroEpmGpioValue* reading);	
	void ParseSetGpioValueResponse(GpioPin* pin, MicroEpmGpioValue* value);
	void ParseSetGpioDirectionResponse(GpioPin* pin, MicroEpmGpioDirection* direction);
	void ParseSetGpioDriveResponse(GpioPin* pin, GpioDrive* drive);
	void ParseGetGpioDriveResponse(GpioPin* pin, GpioDrive* drive);
	void ParseSetGpioBufferStatusResponse(quint8* pStatus);
	void ParseGetGpioBufferResponse(quint8* pStatus, quint8* pGpioData, quint32* puTimestamp);
	
	// Memory
	void ParseMemoryReadResponse(quint8* pSize, quint32* puAddress, quint32* puData);
	void ParseMemoryWriteResponse(quint8* pSize, quint32* puAddress, quint32* puData);
	void ParseMemoryReadBytesResponse(quint8* pSize, quint32* puAddress, quint8* puData);
	void ParseMemoryWriteBytesResponse(quint8* pSize, quint32* puAddress, quint8* puData);
	
	// Misc
	void ParseEnterSecureModeResponse(quint8* pReturnCode);
	void ParseEnterBootloaderResponse(quint8* pReturnCode);
	void ParseSoftwareResetResponse(quint8* pRetCode);
	void ParseLoopbackResponse(quint8 aData[]);
	void ParseIna231RegisterReadResponse(quint8* pStatus, quint16* puData);
	void ParseIna231RegisterWriteResponse(quint8* pStatus);
	void ParsePowerOnTestResponse(quint8* pStatus, quint16* pauAdcFailedMask);
	void ParseGetEpmIDResponse(PlatformID* pPlatformID);
	void ParseCtiControlResponse();
	
	// TAC Command
	void ParseTacResponse(QByteArray& response);
	
	// Timestamp
	void ParseGetSystemTimestampResponse(quint32* puTimestamp);
	void ParseSetSystemTimestampResponse(quint32* puTimestamp);
	
};

#endif //EPMDEVICE_H
