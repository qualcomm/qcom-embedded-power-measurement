#ifndef EPMLIBDEFINES_H
#define EPMLIBDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "BoardUuid.h"
#include "PlatformID.h"

// Qt
#include <qglobal.h>
#include <QList>
#include <QSharedPointer>
#include <QString>

const quint32 MICRO_EPM_STRING_SIZE {32};
const quint32 MAX_SAMPLES_PER_SECOND{7000};

const quint32 MICRO_EPM_ROWDATA_STRING_SIZE {16};
const quint32 MICRO_EPM_CH_STRING_SIZE {16};
const quint32 MICRO_EPM_STRING_MAX_SIZE {MICRO_EPM_STRING_SIZE + 1};
const quint32 MICRO_EPM_CHANNEL_STRING_SIZE {MICRO_EPM_CH_STRING_SIZE + 1};

const quint32 EEPROM_EPM_MAX_NUMBER_CHANNELS {64};
const quint32 EEPROM_EPM_MAX_CAL_NUMBER_CHANNELS {128};

const quint32 MAX_CHAN_BUFFER_LEN {64};

const quint32 MAX_NUM_ADC_CHANNELS {128};
const quint32 MAX_NUM_GPIO_CHANNELS {8};
const quint32 MICRO_EPM_MAX_NUMBER_CHANNELS {MAX_NUM_ADC_CHANNELS + MAX_NUM_GPIO_CHANNELS};

// Device of bytes to transfer.
const quint32 MAX_PACKETS {100};
const quint32 MAX_PACKET_SIZE {64};
const quint32 BUF_SIZE {MAX_PACKET_SIZE};

const double MICRO_EPM_DEFAULT_RSENSE_MOHM{10.0};

/**********************************************
 * SPMv4 Defines                              *
 **********************************************/
const quint32 MAX_BUSES {4};
const quint32 MAX_ADCS_PER_BUS {16};
const quint32 MAX_CHANNELS_PER_BUS {32};

enum AveragingMode
{
	MICRO_EPM_AVERAGING_MODE_NONE         = 0,
	MICRO_EPM_AVERAGING_MODE_4_SAMPLES    = 1,
	MICRO_EPM_AVERAGING_MODE_16_SAMPLES   = 2,
	MICRO_EPM_AVERAGING_MODE_64_SAMPLES   = 3,
	MICRO_EPM_AVERAGING_MODE_128_SAMPLES  = 4,
	MICRO_EPM_AVERAGING_MODE_256_SAMPLES  = 5,
	MICRO_EPM_AVERAGING_MODE_512_SAMPLES  = 6,
	MICRO_EPM_AVERAGING_MODE_1024_SAMPLES = 7,
	_MICRO_EPM_AVERAGING_MODE_NUM         = 8,
	_MICRO_EPM_AVERAGING_MODE_MAX         = 0x7FFFFFFF
};

enum GpioDrive
{
	MICRO_EPM_GPIO_DRIVE_STRONG                  = 0,
	MICRO_EPM_GPIO_DRIVE_OPEN_DRAIN_DRIVE_HIGH   = 1,
	MICRO_EPM_GPIO_DRIVE_OPEN_DRAIN_DRIVE_LOW    = 2,
	MICRO_EPM_GPIO_DRIVE_RESISTIVE_PULL_UP       = 3,
	MICRO_EPM_GPIO_DRIVE_RESISTIVE_PULL_DOWN     = 4,
	MICRO_EPM_GPIO_DRIVE_RESISTIVE_PULL_UP_DOWN  = 5,
	MICRO_EPM_GPIO_DRIVE_HIGH_IMPEDANCE_DIGITAL  = 6,
	_MICRO_EPM_GPIO_DRIVE_MAX                    = 0x7FFFFFFF
};

enum MicroEpmAdcMode
{
	MICRO_EPM_ADC_MODE_CONTINUOUS = 0,
	MICRO_EPM_ADC_MODE_TRIGGERED  = 1,
	MICRO_EPM_ADC_MODE_OFF        = 2,
	_MICRO_EPM_ADC_MODE_NUM       = 3,
	_MICRO_EPM_ADC_MODE_MAX       = 0x7FFFFFFF
};

enum MicroEpmConvTime
{
	MICRO_EPM_CONV_TIME_140_US  = 0,
	MICRO_EPM_CONV_TIME_204_US  = 1,
	MICRO_EPM_CONV_TIME_332_US  = 2,
	MICRO_EPM_CONV_TIME_588_US  = 3,
	MICRO_EPM_CONV_TIME_1100_US = 4,
	MICRO_EPM_CONV_TIME_2116_US = 5,
	MICRO_EPM_CONV_TIME_4156_US = 6,
	MICRO_EPM_CONV_TIME_8244_US = 7,
	_MICRO_EPM_CONV_TIME_NUM    = 8,
	_MICRO_EPM_CONV_TIME_MAX    = 0x7FFFFFFF
};

enum MicroEpmChannelType
{
	MICRO_EPM_CHANNEL_TYPE_CURRENT = 0,
	MICRO_EPM_CHANNEL_TYPE_VOLTAGE = 1,
	MICRO_EPM_CHANNEL_TYPE_GPIO    = 2,
	_MICRO_EPM_CHANNEL_MAX         = 0x7FFFFFFF
};

enum MicroEpmChannelEnable
{
	MICRO_EPM_CHANNEL_DISABLE     = 0,
	MICRO_EPM_CHANNEL_ENABLE      = 1,
	_MICRO_EPM_CHANNEL_ENABLE_MAX = 0x7FFFFFFF
};

enum MicroEpmGpioValue
{
	MICRO_EPM_GPIO_VALUE_LOW  = 0,
	MICRO_EPM_GPIO_VALUE_HIGH = 1,
	_MICRO_EPM_GPIO_MAX       = 0x7FFFFFFF
};

enum GpioPin
{
	MICRO_EPM_GPIO_PIN_MARKER_1  = 0,
	MICRO_EPM_GPIO_PIN_MARKER_2  = 1,
	MICRO_EPM_GPIO_PIN_XO_OUT_EN = 2,
	MICRO_EPM_GPIO_PIN_EPM_INT   = 3,
	MICRO_EPM_GPIO_PIN_GPIO_4    = 4,
	MICRO_EPM_GPIO_PIN_GPIO_5    = 5,
	_MICRO_EPM_GPIO_PIN_MAX      = 0x7FFFFFFF
};

enum MicroEpmGpioDirection
{
	MICRO_EPM_GPIO_DIRECTION_INPUT  = 0,
	MICRO_EPM_GPIO_DIRECTION_OUTPUT = 1,
	_MICRO_EPM_GPIO_DIRECTION_MAX   = 0x7FFFFFFF
};

struct MicroEpmChannelData
{
	MicroEpmChannelData() = default;
	MicroEpmChannelData(const MicroEpmChannelData& copyMe)
	{
		_physicalValue = copyMe._physicalValue;
		_channel = copyMe._channel;
		_rawCode = copyMe._rawCode;
		_timeStamp = copyMe._timeStamp;
		_channelType = copyMe._channelType;
	}
	~MicroEpmChannelData() = default;

	double _physicalValue{0.};									// mV for voltage channels; mA for current channels; N/A for GPIO channels
	quint32 _channel{0xFFFFFFFF};								// ADC channel index; N/A for GPIO channels
	qint32 _rawCode{0};											// raw ADC code or raw GPIO bitmask
	double _timeStamp{-1.};										// raw timestamp of reading in microseconds
	MicroEpmChannelType _channelType{_MICRO_EPM_CHANNEL_MAX};	// the type of channel
};

typedef QList<MicroEpmChannelData> ChannelDataList;

class _Device4ChannelData
{
	Q_DISABLE_COPY(_Device4ChannelData)

public:
	_Device4ChannelData() = default;
	~_Device4ChannelData() = default;

	double getTickCount()
	{
		double result{_channelTickCount};

		_channelTickCount += _interval;

		return result;
	}

	quint32						_channelCount{1};
	double						_channelTickCount{0.};
	double						_interval{0.};
};

typedef QSharedPointer<_Device4ChannelData> Device4ChannelData;
typedef QMap<quint32, Device4ChannelData> Device4ChannelDataMap;

enum MicroEpmPlatform
{
	MICRO_EPM_PLATFORM_UNDEFINED  = 0,
	MICRO_EPM_PLATFORM_INTEGRATED = 1,
	MICRO_EPM_PLATFORM_SPM        = 2,
	_MICRO_EPM_PLATFORM_MAX       = 0x7FFFFFFF
};

enum MicroEpmBoardInfo
{
	_MICRO_EPM_BOARD_MAX      = 0x7FFFFFFF
};

enum RecordingFormat
{
	MICRO_EPM_RECORDING_FORMAT_UDAS  = 0,
	MICRO_EPM_NUM_RECORDING_FORMATS = 1,
	_MICRO_EPM_RECORDING_FORMAT_MAX  = 0x7FFFFFFF
};

struct MicroEpmChannelStat
{
	double dbAverage{0.};
	double dbMin{0.};
	double dbMax{0.};
	quint32 uNumSamples{0};
};

struct MicroEpmEepromChannelInfo
{
	quint32 uCID{0};                // Bytes 0-3. Global unique channel identifier that can be used to map the channel to a int32_t channel name, description and category.
									// Also, could be used to uniquely identify a channel configuration where there is a BOM option for multiple hardware configurations for the same channel
	quint16 uGain{0};               // Bytes 4-5. Gain of the differential amplifier.
	quint32 uSenseResistance{0};    // Bytes 6-9. Resistance of the sense resistor in milli-Ohms.
	quint16 uRailVoltage{0};        // bytes 10-11. Nominal voltage on this rail in milli-Volts.
									// Used for displaying the estimated power on current channels that don't have a paired voltage channel.
									// Not used for voltage channels.
	quint16 nCouplingIdentifier{0}; // Byte 12-13. Identifies the channel index of the corresponding paired voltage/current channel.
									// The channel index referenced by the Coupling Identifier is the global logical channel index on the board
									// (not the channel index on this individual PSoC).
	quint8 uChannelType{0};         // Byte 14: Identifies whether this is a Current (1) or Voltage (2) channel
	quint8 uChannelRevision{0};     // Byte 15. Identifies the revision of this channel (i.e. re-work revision)
	char szChannelName[MICRO_EPM_CHANNEL_STRING_SIZE];       // Bytes 16-48. Channel name.
};

struct MicroEpmChannelInfo
{
	MicroEpmChannelStat			stats;
	char						szName[MAX_CHAN_BUFFER_LEN];
	double						dbRsenseMilliOhms{MICRO_EPM_DEFAULT_RSENSE_MOHM};
	quint32						uRcmChannel{0};
	bool						_dumpRawValues{false};
};

struct EpmVersionInfo
{
	quint32 _epmID = MICRO_EPM_BOARD_ID_UNKNOWN;
	quint16 _maxSamplesPerPacket{0};
	quint16 _maxSamplesPerAveragePacket{0};
	quint16 _maxPackets{0};
	quint8 _firmwareVersion[4]{0, 0, 0, 0};
	quint8 _firmwareProtocolVersion{0};
	quint8 _firmwareLowestCompatibleProtocolVersion{0};
	quint8 _hostProtocolVersion{0};
	quint8 _hostLowestCompatibleProtocolVersion{0};
};

#pragma pack(1)
struct MicroEpmEepromChannelInfoPacked
{
	quint32 uChannelIdentifier{0};
	quint32 uSenseResistance{0};
	char szChannelName[MICRO_EPM_CH_STRING_SIZE];
};

struct MicroEpmEepromPacked
{
	quint16 uMajorVersion{0};
	quint16 uMinorVersion{0};
	uint64_t EepromProgrammedTime{0};
	char szReserved[4]{0, 0, 0, 0};
	char szSerialNumber[MICRO_EPM_STRING_SIZE];   // Serial number of this board
	char szModelNumber[MICRO_EPM_STRING_SIZE];    // Model number of this board
	quint16 uTargetIdentifier{0};                 // Target identifier
	quint16 uBoardTypeIdentifier{0};              // EPM board revision ID
	quint16 uBoardOptionIdentifier{0};
	char szReserved1[10];
	MicroEpmEepromChannelInfoPacked aChannelInfo[EEPROM_EPM_MAX_NUMBER_CHANNELS];
	uint64_t eepromCalibrationTime{0};
	char szReserved2[8];
	qint16 aCalibrationCodes[EEPROM_EPM_MAX_CAL_NUMBER_CHANNELS];
	char szReserved3[15];
	quint8 uNumberOfRework{0};                            // Number of rework performed on this board
	char szReworkIdentifier[2][MICRO_EPM_STRING_SIZE];    // History of up to 2 rework performed on this board.
	MicroEpmEepromUuid epmBoardUuid;
	char szReserved4[4];
	quint32 uEepromFailWriteCount{0};                     // Number of times a row was written, but the verification of
														  // the row failed.
	quint32 uEepromWriteCount{0};                         // Number of times any EEPROM row was written
	quint16 uRecoveryCounter{0};                          // Number of times an EEPROM has to recover from the scratch space.
	quint16 uSavedRowIndex{0};                            // Index of last EEPROM row, saved to scratch space.
	quint8 szSavedRowData[MICRO_EPM_ROWDATA_STRING_SIZE]; // Row data for the last row saved to the scratch space.
	char szReserved5[12];
	quint32 uCheckSum{0};
};
#pragma pack()

const quint16 MAX_ADC_CODE{0x7FFF};
const quint16 MAX_ADC_OVERFLOW_CODE{MAX_ADC_CODE + (MAX_ADC_CODE / 2)};

const quint32 TIMER_MASK{0xFFFFFFFF};

constexpr double MILLI_PER_BASE{1000.0};
constexpr double MICRO_PER_BASE{1000000.0};

#endif // EPMLIBDEFINES_H
