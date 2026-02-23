#ifndef EPMLIBPRIVATE_H
#define EPMLIBPRIVATE_H
/*============================================================================
  @file MicroEpmLib_priv.h


               Copyright (c) 2012-2014, 2016-2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //source/qcom/qct/core/hwengines/epm/microepm/libmicroepm/src/MicroEpmLib_priv.h#2 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

// QCommon
#include "StringUtilities.h"

// EPM
#include "EPMLibDefines.h"
#include "MicroEpmCommands.h"
#include "WaveForms.h"

#include <cstdint>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define GET_HIGHEST_8BITS(x)      (((x) >> 24) & 0xFF)
#define GET_UPPER_MIDDLE_8BITS(x) (((x) >> 16) & 0xFF)
#define GET_LOWER_MIDDLE_8BITS(x) (((x) >>  8) & 0xFF)
#define GET_LOWEST_8BITS(x)        ((x) & 0xFF)

#define GET_UPPER_8BITS(x) (((x) >>  8) & 0xFF)
#define GET_LOWER_8BITS(x)  ((x) & 0xFF)


/* Packet Status Codes */
#define EPM_PROTOCOL_STATUS_SUCCESS                   ((uint8_t)0)
#define EPM_PROTOCOL_STATUS_ERROR                     ((uint8_t)1)
#define EPM_PROTOCOL_STATUS_CLIENT_BUSY               ((uint8_t)2)
#define EPM_PROTOCOL_STATUS_POWER_ON_TEST_FAILED      ((uint8_t)3)

#define COMM_BUFFER_SIZE                               MAX_PACKET_SIZE

#define MAX_NUMBER_TARGETS (6)
#define MAX_NUMBER_MODULES_PER_TARGET (2)
#define MAX_NUMBER_MODULES (MAX_NUMBER_TARGETS * MAX_NUMBER_MODULES_PER_TARGET)
#define MAX_ADC_CODE 0x7fff
#define MAX_ADC_OVERFLOW_CODE (MAX_ADC_CODE + (MAX_ADC_CODE / 2))

#ifdef  TIME_24BIT
#define TIMER_MASK 0xFFFFFF
#else
#define TIMER_MASK 0xFFFFFFFF
#endif
#define EPM_TIMESTAMP_START                      (TIMER_MASK)
#define MAX_SAMPLES_PER_PACKET                   (24)
#define MAX_SAMPLES_PER_AVERAGE_PACKET           (27)

#define SPMV4_V_MV_PER_CODE (1.25)
#define SPMV4_I_UV_PER_CODE (2.5)

#define EEPROM_SIZE                              (2048)
#define EEPROM_ROW_SIZE                          (16)
#define EEPROM_NUMBER_OF_ROWS                    (EEPROM_SIZE/EEPROM_ROW_SIZE)
#define EEPROM_CHECKSUM_SIZE                     (4)
#define EEPROM_MAJOR_VERSION                     ((uint16_t) 2)
#define EEPROM_MINOR_VERSION                     ((uint16_t) 1)
#define EEPROM_CHANINFO_CHANNEL_NAME_SIZE        (32)

#define PSOC_RESET_REGISTER_ADDRESS              (0x400046f6u)

#define GPIO_HIGH_V 1.8
#define GPIO_LOW_V  0

/* Packet Locations */
const quint32 CMD_ADDRESS {0x00};
const quint32 DATA_ADDRESS {0x01};

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

enum MicroEpmThreadStatus
{
    MICRO_EPM_THREAD_SUCCESS  = 0,
    MICRO_EPM_THREAD_ERROR    = 1,
    MICRO_EPM_THREAD_OVERFLOW = 2
};

void createChannelName(const QString& seriesName, HashType& seriesHash, QString& channelName, HashType& channelHash, WaveFormType& waveFormType);

#endif // EPMLIBPRIVATE_H
