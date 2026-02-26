// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <windows.h>
#endif

#include "MicroEpmLib_priv.h"
#include "Host_Communications_Packets.h"
#include "MicroEpmLogUdas.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define SPMV4_MAJOR_VERSION                (1)
#define MICRO_EPM_DEFAULT_AVERAGING_MODE   MICRO_EPM_AVERAGING_MODE_NONE
#define MICRO_EPM_DEFAULT_ADC_MODE         MICRO_EPM_ADC_MODE_CONTINUOUS
#define MICRO_EPM_DEFAULT_CONV_TIME        MICRO_EPM_CONV_TIME_140_US
#define MICRO_EPM_DEFAULT_SET_PERIOD       (140)
#define MICRO_EPM_DEFAULT_DATA_RATE        (0xffffffff)
#define MICRO_EPM_DEFAULT_RSENSE_MOHM      (10.0)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
// Useful for viewing buffered data packets
#pragma pack(1)
typedef struct {
    uint8_t uCommand;
    uint8_t uStatus;
    uint8_t uNumSamples;
    uint8_t uFirstChannel;
    uint32_t uChannelMask;
    uint32_t uStartTime;
    uint32_t uEndTime;
	quint16 aData[24];
} _MicroEpmGetDataPacketType;
#pragma pack()

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static qint32 guNumOverflow = 0, guNumErrors = 0, guNumErrorsTime = 0;

static uint8_t gbIsMicroEpmInitialized = FALSE;

static uint32_t guNumModules = 0;
static uint32_t guNumTargets = 0;

static MicroEpmHandle gMicroEpmDevCtxt[MAX_NUMBER_MODULES];
static uint32_t gMicroEpmIndex[MAX_NUMBER_TARGETS][MAX_NUMBER_MODULES_PER_TARGET] = { { 0 } };
static uint32_t gNumberOfChildModules[MAX_NUMBER_TARGETS] = { 0 };

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static MicroEpmErrorType SendReceiveHelloPacket(MicroEpmHandle *pDevCtxt)
{
    MicroEpmErrorType status;
    MicroEpmVersionInfoType version;
    uint8_t uNumChannels;
    uint8_t uStatusCode;
    uint8_t bSpiConnected;
    uint8_t bUsbConnected;

    memset(pDevCtxt->aEpmOutBuf, 0, sizeof(pDevCtxt->aEpmOutBuf));
    memset(pDevCtxt->aEpmInBuf, 0, sizeof(pDevCtxt->aEpmInBuf));

    status = MakeHelloCommand(pDevCtxt->aEpmOutBuf, sizeof(pDevCtxt->aEpmOutBuf),
                              MICRO_EPM_PROTOCOL_VERSION,
                              MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION, 0);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send HELLO command
    status = MicroEpmComm_Send(pDevCtxt->hComm, pDevCtxt->aEpmOutBuf, HELLO_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive HELLO response
    status = MicroEpmComm_Receive(pDevCtxt->hComm, pDevCtxt->aEpmInBuf,
                                  HELLO_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse HELLO response
    if (pDevCtxt->aEpmInBuf[0] != HELLO_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseHelloResponse(pDevCtxt->aEpmInBuf, HELLO_RSP_LENGTH, &version,
                                &uNumChannels, &uStatusCode, &bSpiConnected, &bUsbConnected);
    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (EPM_PROTOCOL_STATUS_POWER_ON_TEST_FAILED == uStatusCode) {
        return MICRO_EPM_POWER_ON_TEST_FAILED;
    } else if (EPM_PROTOCOL_STATUS_SUCCESS != uStatusCode) {
        return MICRO_EPM_COMM_ERROR;
    }

    version.uHostProtocolVersion = MICRO_EPM_PROTOCOL_VERSION;
    version.uMaxPackets = MAX_PACKETS;

    pDevCtxt->version = version;
    pDevCtxt->uNumChannels = uNumChannels;

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSetGpioDrive_3(
    MicroEpmHandle *hDevice,
    MicroEpmGpioPinType gpioChannel,
    MicroEpmGpioDriveType drive)
{
    MicroEpmErrorType status;
    MicroEpmGpioPinType receivedPin;
    MicroEpmGpioDriveType receivedDrive;

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetGpioDriveCommand(hDevice->aEpmOutBuf,
                                     sizeof(hDevice->aEpmOutBuf), gpioChannel, drive);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               SET_GPIO_DRIVE_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  SET_GPIO_DRIVE_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != SET_GPIO_DRIVE_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetGpioDriveResponse(hDevice->aEpmInBuf,
                                       SET_GPIO_VALUE_RSP_LENGTH, &receivedPin, &receivedDrive);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (receivedPin != gpioChannel) {
        return MICRO_EPM_COMM_ERROR;
    }

    if (receivedDrive != drive) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType CountNumberOfSpmBoards(
    uint32_t *puNumberOfSpmBoards,
    uint32_t uNumberOfModules)
{
    uint32_t i;
    MicroEpmErrorType status;
    MicroEpmGpioValueType GpioVal;
    uint32_t numSpm = 0;

    for (i = 0; i < uNumberOfModules; i++) {
        if (gMicroEpmDevCtxt[i].bIsSpmV4 == TRUE) {
            continue;
        }

        status = MicroEpmEnterSecureMode(&gMicroEpmDevCtxt[i]);
        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
        status = MicroEpmSetGpioDrive_3(&gMicroEpmDevCtxt[i],
                                        MICRO_EPM_GPIO_PIN_GPIO_5,
                                        MICRO_EPM_GPIO_DRIVE_RESISTIVE_PULL_DOWN);
        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
        /* Detect whether we have two targets connected or just one SPM board */
        status = MicroEpmGetGpioValue(&gMicroEpmDevCtxt[i],
                                      MICRO_EPM_GPIO_PIN_GPIO_5, &GpioVal);
        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
        // Spm board detected
        if (GpioVal == MICRO_EPM_GPIO_VALUE_HIGH) {
            numSpm++;
        }
    }

    if (puNumberOfSpmBoards != NULL) {
        *puNumberOfSpmBoards = numSpm;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType DetectTargets(
    uint32_t uNumberOfModules,
    uint32_t uNumberOfSpm)
{
    uint32_t i;
    uint32_t j;
    MicroEpmErrorType status;
    MicroEpmGpioValueType GpioVal;
    uint32_t abIsUnpairedModule[MAX_NUMBER_MODULES];
	qint32 nComparison;
    uint32_t uCountSingleModules = 0;
    uint32_t uCountSpmTargets = 0;
    uint32_t uUnprogrammedCount = 0;
    uint32_t uUnpairedModules = 0;
    uint32_t UnprogrammedIndexOne = 0;
    uint32_t UnprogrammedIndexTwo = 0;

    // Setup initial pairing array and count unprogrammed modules
    for (i = 0; i < uNumberOfModules; i++) {
        abIsUnpairedModule[i] = TRUE;
        uUnpairedModules++;
        if ((gMicroEpmDevCtxt[i].bIsProgrammed == FALSE) &&
            (gMicroEpmDevCtxt[i].bIsSpmV4 == FALSE)) {
            uUnprogrammedCount++;
        }
    }

    // If we have more unprogrammed modules than number of
    // PSOCs on SPM we know that we can't determine platforms
    if (uUnprogrammedCount > MAX_NUMBER_MODULES_PER_TARGET) {
        return MICRO_EPM_NOT_PROGRAMMED;
    }

    // do detection of EEPROM and pairing here
    // This pairs all programmed SPM boards
    for (i = 0; i < uNumberOfModules; i++) {
        if (gMicroEpmDevCtxt[i].bIsProgrammed == TRUE &&
            abIsUnpairedModule[i] == TRUE) {
            for (j = 0; j < uNumberOfModules; j++) {
                // We don't compare serial numbers to ourself otherwise we would
                // pair to ourself
                if (j == i) {
                    continue;
                }
                nComparison = strcmp(gMicroEpmDevCtxt[i].TargetInfo.szSerialNumber,
                                     gMicroEpmDevCtxt[j].TargetInfo.szSerialNumber);

                // If we found a serial match
                if (nComparison == 0) {
                    // Mark the modules as paired
                    abIsUnpairedModule[i] = FALSE;
                    uUnpairedModules--;
                    abIsUnpairedModule[j] = FALSE;
                    uUnpairedModules--;

                    uCountSpmTargets++;

                    // Found two paired modules but need to determine master
                    status = MicroEpmGetGpioValue(&gMicroEpmDevCtxt[i],
                                                  MICRO_EPM_GPIO_PIN_GPIO_5, &GpioVal);
                    if (status != MICRO_EPM_SUCCESS) {
                        return status;
                    }
                    // if master
                    if (GpioVal == MICRO_EPM_GPIO_VALUE_LOW) {
                        gMicroEpmIndex[guNumTargets][0] = i;
                        gMicroEpmIndex[guNumTargets][1] = j;
                    } else {
                        // Verify other paired PSOC is master
                        status = MicroEpmGetGpioValue(&gMicroEpmDevCtxt[j],
                                                      MICRO_EPM_GPIO_PIN_GPIO_5, &GpioVal);
                        if (status != MICRO_EPM_SUCCESS) {
                            return status;
                        }
                        if (GpioVal == MICRO_EPM_GPIO_VALUE_LOW) {
                            gMicroEpmIndex[guNumTargets][0] = j;
                            gMicroEpmIndex[guNumTargets][1] = i;
                        } else {
                            // No master found
                            return MICRO_EPM_ERROR;
                        }

                    }

                    gNumberOfChildModules[guNumTargets] = MAX_NUMBER_MODULES_PER_TARGET;
                    guNumTargets++;

                    break;
                }
            }
        }
    }

    // If there are only programmed SPM boards we can exit now
    if (uUnpairedModules == 0) {
        // basic error checking
        if (uCountSpmTargets != uNumberOfSpm) {
            return MICRO_EPM_ERROR;
        }
        return MICRO_EPM_SUCCESS;
    }

    // Otherwise we need to detect programmed single modules
    for (i = 0; i < uNumberOfModules; i++) {
        // If it's unpaired but programmed now we assume it's a single PSOC module
        if (gMicroEpmDevCtxt[i].bIsProgrammed == TRUE &&
            abIsUnpairedModule[i] == TRUE) {
            abIsUnpairedModule[i] = FALSE;
            uUnpairedModules--;

            gMicroEpmIndex[guNumTargets][0] = i;
            gNumberOfChildModules[guNumTargets] = 1;
            guNumTargets++;
            uCountSingleModules++;
        }
    }

    if (uUnpairedModules == 0) {
        // basic error checking
        if (uCountSpmTargets != uNumberOfSpm) {
            return MICRO_EPM_ERROR;
        }
        if (uCountSpmTargets * MAX_NUMBER_MODULES_PER_TARGET + uCountSingleModules != uNumberOfModules) {
            return MICRO_EPM_ERROR;
        }
        return MICRO_EPM_SUCCESS;
    }

    // Any modules left now are unprogrammed. We need to determine their platform
    // If it's one then we know it's a single psoc device
    if (uUnpairedModules == 1) {
        for (i = 0; i < uNumberOfModules; i++) {
            if (abIsUnpairedModule[i] == TRUE) {
                gMicroEpmIndex[guNumTargets][0] = i;
                gNumberOfChildModules[guNumTargets] = 1;
                guNumTargets++;
                uCountSingleModules++;

                return MICRO_EPM_SUCCESS;
            }
        }
    }

    // It appears we have either an SPM board or two unprogrammed modules
    // Now to determine which case we have

    for (i = 0; i < uNumberOfModules; i++) {
        if (abIsUnpairedModule[i] == TRUE) {
            if (uUnpairedModules > 1) {
                UnprogrammedIndexTwo = i;
                uUnpairedModules--;
            } else {
                UnprogrammedIndexOne = i;
                uUnpairedModules--;
                break;
            }
        }
    }

    status = MicroEpmGetGpioValue(&gMicroEpmDevCtxt[UnprogrammedIndexOne],
                                  MICRO_EPM_GPIO_PIN_GPIO_5, &GpioVal);
    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }
    if (GpioVal == MICRO_EPM_GPIO_VALUE_LOW) {
        status = MicroEpmGetGpioValue(&gMicroEpmDevCtxt[UnprogrammedIndexTwo],
                                      MICRO_EPM_GPIO_PIN_GPIO_5, &GpioVal);
        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
        // SPM board detected
        if (GpioVal == MICRO_EPM_GPIO_VALUE_HIGH) {
            gMicroEpmIndex[guNumTargets][0] = UnprogrammedIndexOne;
            gMicroEpmIndex[guNumTargets][1] = UnprogrammedIndexTwo;
            gNumberOfChildModules[guNumTargets] = 2;

            uCountSpmTargets++;
            guNumTargets++;

            return MICRO_EPM_SUCCESS;
        } else { // two single modules detected
            return MICRO_EPM_NOT_PROGRAMMED;
        }
    } else {
        status = MicroEpmGetGpioValue(&gMicroEpmDevCtxt[UnprogrammedIndexTwo],
                                      MICRO_EPM_GPIO_PIN_GPIO_5, &GpioVal);
        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
        // Unsupported configuration detected
        if (GpioVal == MICRO_EPM_GPIO_VALUE_HIGH) {
            return MICRO_EPM_ERROR;
        } else { // SPM board detected
            gMicroEpmIndex[guNumTargets][0] = UnprogrammedIndexTwo;
            gMicroEpmIndex[guNumTargets][1] = UnprogrammedIndexOne;
            gNumberOfChildModules[guNumTargets] = 2;

            uCountSpmTargets++;
            guNumTargets++;
            return MICRO_EPM_SUCCESS;
        }
    }
}

static MicroEpmErrorType MicroEpmGetRawTimestamp(
    MicroEpmHandle *hDevice,
    uint32_t *puRawTimestamp)
{
    MicroEpmErrorType status;

    if (puRawTimestamp == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeGetSystemTimestampCommand(hDevice->aEpmOutBuf,
                                           sizeof(hDevice->aEpmOutBuf));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               GET_SYSTEM_TIMESTAMP_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  GET_SYSTEM_TIMESTAMP_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != GET_SYSTEM_TIMESTAMP_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseGetSystemTimestampResponse(hDevice->aEpmInBuf,
                                             GET_SYSTEM_TIMESTAMP_RSP_LENGTH,
                                             puRawTimestamp);

    *puRawTimestamp = (uint32_t)(TIMER_MASK) - *puRawTimestamp;

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSetRawTimestamp(
    MicroEpmHandle *hDevice,
    uint32_t uRawTimestamp)
{
    MicroEpmErrorType status;
    uint32_t uNewTimestamp;

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    uRawTimestamp = (uint32_t)(TIMER_MASK) - uRawTimestamp;

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetSystemTimestampCommand(hDevice->aEpmOutBuf,
                                           sizeof(hDevice->aEpmOutBuf),
                                           uRawTimestamp);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               SET_SYSTEM_TIMESTAMP_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  SET_SYSTEM_TIMESTAMP_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != SET_SYSTEM_TIMESTAMP_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetSystemTimestampResponse(hDevice->aEpmInBuf,
                                             SET_SYSTEM_TIMESTAMP_RSP_LENGTH,
                                             &uNewTimestamp);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendEnableDisable(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint32_t uBitmask;
    uint8_t uReceivedStatusCode;
    uint32_t uReceivedBitmask;
    uint8_t uBus;

    for (uBus = 0; uBus < hDevice->uNumBuses; uBus++) {
        uBitmask = hDevice->uEnabledChannelBitmask[uBus];

        memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
        memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

        status = MakeEnableDisableChannelCommand(hDevice->aEpmOutBuf,
                                                 sizeof(hDevice->aEpmOutBuf),
                                                 uBus,
                                                 uBitmask);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Send command
        status = MicroEpmComm_Send(hDevice->hComm,
                                   hDevice->aEpmOutBuf,
                                   ENABLE_DISABLE_CHANNEL_CMD_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Receive response
        status = MicroEpmComm_Receive(hDevice->hComm,
                                      hDevice->aEpmInBuf,
                                      ENABLE_DISABLE_CHANNEL_RSP_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Parse response
        if (hDevice->aEpmInBuf[0] != ENABLE_DISABLE_CHANNEL_RSP) {
            return MICRO_EPM_COMM_ERROR;
        }

        status = ParseEnableDisableChannelResponse(hDevice->aEpmInBuf,
                                                   ENABLE_DISABLE_CHANNEL_RSP_LENGTH,
                                                   &uReceivedStatusCode,
                                                   &uReceivedBitmask);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        if (uBitmask != uReceivedBitmask || EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
            return MICRO_EPM_COMM_ERROR;
        }
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendAveragingMode(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t uReceivedStatusCode;
    uint8_t uBus;

    for (uBus = 0; uBus < hDevice->uNumBuses; uBus++) {
        memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
        memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

        status = MakeSetAveragingCommand(hDevice->aEpmOutBuf,
                                         sizeof(hDevice->aEpmOutBuf),
                                         uBus,
                                         hDevice->aeAveragingMode[uBus]);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Send command
        status = MicroEpmComm_Send(hDevice->hComm,
                                   hDevice->aEpmOutBuf,
                                   SET_AVERAGING_CMD_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Receive response
        status = MicroEpmComm_Receive(hDevice->hComm,
                                      hDevice->aEpmInBuf,
                                      SET_AVERAGING_RSP_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Parse response
        if (hDevice->aEpmInBuf[CMD_ADDRESS] != SET_AVERAGING_RSP) {
            return MICRO_EPM_COMM_ERROR;
        }

        status = ParseSetAveragingResponse(hDevice->aEpmInBuf,
                                           SET_AVERAGING_RSP_LENGTH,
                                           &uReceivedStatusCode);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        if (EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
            return MICRO_EPM_COMM_ERROR;
        }
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendAdcMode(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t uReceivedStatusCode;

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetAdcModeCommand(hDevice->aEpmOutBuf,
                                   sizeof(hDevice->aEpmOutBuf),
                                   hDevice->eAdcMode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               SET_ADC_MODE_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  SET_ADC_MODE_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[CMD_ADDRESS] != SET_ADC_MODE_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetAdcModeResponse(hDevice->aEpmInBuf,
                                     SET_ADC_MODE_RSP_LENGTH,
                                     &uReceivedStatusCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType VerifyTriggeredModeInput(MicroEpmHandle *hDevice)
{
    uint32_t uBus;
    uint32_t uAdc;
    uint32_t uAdcChannelMask;
    MicroEpmConvTimeType eRefCurrentConvTime = _MICRO_EPM_CONV_TIME_NUM;
    MicroEpmConvTimeType eRefVoltageConvTime = _MICRO_EPM_CONV_TIME_NUM;
    MicroEpmAveragingModeType eRefAvgMode = _MICRO_EPM_AVERAGING_MODE_NUM;
    uint8_t bFirst = TRUE;

    for (uBus = 0; uBus < MAX_BUSES; uBus++) {
        for (uAdc = 0; uAdc < MAX_ADCS_PER_BUS; uAdc++) {
            uAdcChannelMask = (hDevice->uEnabledChannelBitmask[uBus] >> (uAdc * 2)) & 0x3;
            if (uAdcChannelMask == 3) {
                // Voltage and current are enabled for this ADC
                if (bFirst == TRUE) {
                    eRefCurrentConvTime = hDevice->aeConvTime[uBus][uAdc * 2];
                    eRefVoltageConvTime = hDevice->aeConvTime[uBus][uAdc * 2 + 1];
                    eRefAvgMode = hDevice->aeAveragingMode[uBus][uAdc];
                    bFirst = FALSE;
                } else {
                    if (hDevice->aeConvTime[uBus][uAdc * 2] != eRefCurrentConvTime ||
                        hDevice->aeConvTime[uBus][uAdc * 2 + 1] != eRefVoltageConvTime ||
                        hDevice->aeAveragingMode[uBus][uAdc] != eRefAvgMode) {
                        // Channel settings do not match
                        return MICRO_EPM_INVALID_TRIGGERED_MODE_SETTINGS;
                    }
                }
            } else if (uAdcChannelMask != 0) {
                // Both current and voltage paired channels were not enabled
                return MICRO_EPM_INVALID_TRIGGERED_MODE_SETTINGS;
            }
        }
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendSetPeriod(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t uReceivedStatusCode;

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetSetPeriodCommand(hDevice->aEpmOutBuf,
                                     sizeof(hDevice->aEpmOutBuf),
                                     hDevice->uSetPeriod);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               SET_SET_PERIOD_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  SET_SET_PERIOD_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[CMD_ADDRESS] != SET_SET_PERIOD_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetSetPeriodResponse(hDevice->aEpmInBuf,
                                       SET_SET_PERIOD_RSP_LENGTH,
                                       &uReceivedStatusCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendConvTime(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t uReceivedStatusCode;
    uint8_t uBus;

    for (uBus = 0; uBus < hDevice->uNumBuses; uBus++) {
        memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
        memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

        status = MakeSetConversionTimeCommand(hDevice->aEpmOutBuf,
                                             sizeof(hDevice->aEpmOutBuf),
                                             uBus,
                                             hDevice->aeConvTime[uBus]);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Send command
        status = MicroEpmComm_Send(hDevice->hComm,
                                   hDevice->aEpmOutBuf,
                                   SET_CONVERSION_TIME_CMD_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Receive response
        status = MicroEpmComm_Receive(hDevice->hComm,
                                      hDevice->aEpmInBuf,
                                      SET_CONVERSION_TIME_RSP_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Parse response
        if (hDevice->aEpmInBuf[CMD_ADDRESS] != SET_CONVERSION_TIME_RSP) {
            return MICRO_EPM_COMM_ERROR;
        }

        status = ParseSetConversionTimeResponse(hDevice->aEpmInBuf,
                                                SET_CONVERSION_TIME_RSP_LENGTH,
                                                &uReceivedStatusCode);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        if (EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
            return MICRO_EPM_COMM_ERROR;
        }
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendDataRateGovernor(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t uReceivedStatusCode;

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetDataRateGovernorCommand(hDevice->aEpmOutBuf,
                                            sizeof(hDevice->aEpmOutBuf),
                                            hDevice->uMaxDataRate);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               SET_DATA_RATE_GOVERNOR_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  SET_DATA_RATE_GOVERNOR_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[CMD_ADDRESS] != SET_DATA_RATE_GOVERNOR_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetDataRateGovernorResponse(hDevice->aEpmInBuf,
                                              SET_DATA_RATE_GOVERNOR_RSP_LENGTH,
                                              &uReceivedStatusCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendApplySettings(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t uReceivedStatusCode;

    // Send apply settings command
    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeApplySettingsCommand(hDevice->aEpmOutBuf, sizeof(hDevice->aEpmOutBuf));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               APPLY_SETTINGS_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  APPLY_SETTINGS_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[CMD_ADDRESS] != APPLY_SETTINGS_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseApplySettingsResponse(hDevice->aEpmInBuf,
                                        APPLY_SETTINGS_RSP_LENGTH,
                                        &uReceivedStatusCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

/*****************************************************
*  Packet format
*
* Index(Byte)  Description
*     0     Command (0x0C)
*     1     Device index (0)
*     2     GPIO Mask (reading from GPIO_In_Reg)
*     3     Channel Index (first channel reading in packet)
*     4     Channel Bitmask
*     8     Timestamp Start
*     12    Timestamp End
*     16    Reading 0
*     18    Reading 1
*      .
*      .
*     63    Reading 23
*
****************************************************/
static MicroEpmErrorType MicroEpmGetData_3(
    MicroEpmHandle *hDevice,
    MicroEpmChannelDataType *pDataArray,
    uint32_t uArraySize,
    uint32_t *puNumSamples,
    uint32_t uNumPackets,
    uint32_t *puNumOverflow,
    uint32_t *puNumEmpty)
{
    MicroEpmErrorType status = MICRO_EPM_SUCCESS;
    double timestamp_start =0.0, timestamp_start_1=0.0;
    double timestamp_end=0.0, timestamp_end_1=0.0;
    double timestamp_inc = 0.0;
    double cur_timestamp =0.0;
    uint32_t gpio_status = 0;
    uint32_t channel_mask = 0;
    uint32_t channel_status = 0;
    uint32_t i = 0;
    uint32_t channelIdx = 0;
    uint32_t enabledChannelsList[32] = {0};
    uint32_t numEnabledChannels = 0;
	qint32 nRaw = 0;
    uint32_t uChannel = 0;
    uint32_t uMaxArraySampleSize = 0;
    uint32_t uOutBufferIdx = 0;
    uint32_t uPacket = 0;
    uint8_t *aEpmInBuf;
    uint32_t uNumEmpty = 0;
    uint32_t uNumOverflow = 0;
    uint32_t uNumSamplesInPacket = 0;

    if ((puNumSamples == NULL) || (uNumPackets > MAX_PACKETS)) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    *puNumSamples = 0;

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBufBig, 0, BUF_SIZE * MAX_PACKETS);

    status = MakeGetBufferedDataCommand(hDevice->aEpmOutBuf,
                                        sizeof(hDevice->aEpmOutBuf));

    status = MicroEpmComm_SendReceivePacketBuffer(hDevice->hComm,
             hDevice->aEpmOutBuf,
             sizeof(hDevice->aEpmOutBuf),
             hDevice->aEpmInBufBig,
             BUF_SIZE * uNumPackets);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    for (uPacket = 0; uPacket < uNumPackets; uPacket++) {
        uMaxArraySampleSize = min(hDevice->version.uMaxSamplesPerPacket, uArraySize);
        if ( uMaxArraySampleSize ==0)
            break;
        numEnabledChannels = 0;
        uNumSamplesInPacket = 0;
        aEpmInBuf = &hDevice->aEpmInBufBig[uPacket * BUF_SIZE];

        if (aEpmInBuf[1] & 0x20) {
            // empty packet
            uNumEmpty++;
            continue;
        }
        if (aEpmInBuf[1] & 0x10) {
            // buffer overflowed.
            uNumOverflow++;
            guNumOverflow++;
            continue;
        }

        if (aEpmInBuf[0] != GET_BUFFERED_DATA_RSP) {
            guNumErrors++;
        }

#ifdef  TIME_24BIT
        timestamp_start_1 =  ((uint32_t)aEpmInBuf[10] << 16)
                             | ((uint32_t)aEpmInBuf[9] << 8)
                             | (uint32_t)aEpmInBuf[8];

        timestamp_end_1 = ((uint32_t)aEpmInBuf[14] << 16)
                          | ((uint32_t)aEpmInBuf[13] << 8)
                          | (uint32_t)aEpmInBuf[12];
#else
        timestamp_start_1 = ((uint32_t)aEpmInBuf[11] << 24)
                            | ((uint32_t)aEpmInBuf[10] << 16)
                            | ((uint32_t)aEpmInBuf[9] << 8)
                            | (uint32_t)aEpmInBuf[8];

        timestamp_end_1 = ((uint32_t)aEpmInBuf[15] << 24)
                          | ((uint32_t)aEpmInBuf[14] << 16)
                          | ((uint32_t)aEpmInBuf[13] << 8)
                          | (uint32_t)aEpmInBuf[12];
#endif

        if ((timestamp_start_1 == 0) || (timestamp_end_1 == 0)) {
            guNumErrorsTime++;
        }

        timestamp_start = (uint32_t)(TIMER_MASK) - timestamp_start_1;
        timestamp_end = (uint32_t)(TIMER_MASK) - timestamp_end_1;
        channel_mask = (((uint32_t)aEpmInBuf[7] << 24)
                        | ((uint32_t)aEpmInBuf[6] << 16)
                        | ((uint32_t)aEpmInBuf[5] << 8)
                        | (uint32_t)aEpmInBuf[4]);
        gpio_status = aEpmInBuf[2];
        channel_status = aEpmInBuf[3];
        cur_timestamp = timestamp_start;
        timestamp_inc = (timestamp_end - timestamp_start) / (hDevice->version.uMaxSamplesPerPacket - 2);

        for (i = 0; i < 32; i++) {
            if (channel_mask & (1 << i)) {
                if ((channel_status & 0x1f) == i) {
                    channelIdx = numEnabledChannels;
                }
                enabledChannelsList[numEnabledChannels] = i;
                numEnabledChannels++;
            }
        }

        pDataArray[uOutBufferIdx].uTarget = hDevice->uTarget;
        pDataArray[uOutBufferIdx].uModule = hDevice->uModule;
        pDataArray[uOutBufferIdx].eChannelType = MICRO_EPM_CHANNEL_TYPE_GPIO;
        pDataArray[uOutBufferIdx].nRawCode = gpio_status;
        pDataArray[uOutBufferIdx].uRawTimestamp = (uint32_t) timestamp_start;
        uOutBufferIdx++;
        uNumSamplesInPacket++;

        for (i = 0; i < uMaxArraySampleSize - 1; i++) {
            uChannel = enabledChannelsList[channelIdx];

            if (hDevice->uEnabledChannelBitmask[0] & (1 << uChannel)) {
                if (uChannel == 31) {
                    pDataArray[uOutBufferIdx].eChannelType = MICRO_EPM_CHANNEL_TYPE_GPIO;
                } else if (hDevice->uVoltageChannelBitmask & (1 << uChannel)) {
                    pDataArray[uOutBufferIdx].eChannelType = MICRO_EPM_CHANNEL_TYPE_VOLTAGE;
                } else {
                    pDataArray[uOutBufferIdx].eChannelType = MICRO_EPM_CHANNEL_TYPE_CURRENT;
                }

                pDataArray[uOutBufferIdx].uTarget = hDevice->uTarget;
                pDataArray[uOutBufferIdx].uModule = hDevice->uModule;
                pDataArray[uOutBufferIdx].uChannel = uChannel;
                nRaw = aEpmInBuf[16 + i * 2] + (aEpmInBuf[16 + i * 2 + 1] << 8);

                if (pDataArray[uOutBufferIdx].eChannelType == MICRO_EPM_CHANNEL_TYPE_GPIO) {
                    nRaw = nRaw >> 8;
                } else if (pDataArray[uOutBufferIdx].eChannelType == MICRO_EPM_CHANNEL_TYPE_VOLTAGE) {
                    /* Voltage channels use the 12-bit ADC */
                    nRaw = nRaw << 3;
                    if (nRaw > MAX_ADC_CODE) {
                        if (nRaw > MAX_ADC_OVERFLOW_CODE) {
                            nRaw = 0;
                        } else {
                            nRaw = MAX_ADC_CODE;
                        }
                    }
                } else {
                    // eChannelType == MICRO_EPM_CHANNEL_TYPE_CURRENT

                    // if fastmode
                    //{
                    //nRaw = nRaw - 0x800;
                    //nRaw = nRaw << 4;
                    //}
                    if (nRaw > MAX_ADC_CODE) {
                        if (nRaw > MAX_ADC_OVERFLOW_CODE) {
                            nRaw = (int16_t)nRaw;
                        } else {
                            nRaw = MAX_ADC_CODE;
                        }
                    }
                }
                pDataArray[uOutBufferIdx].nRawCode = nRaw;
                pDataArray[uOutBufferIdx].uRawTimestamp = (uint32_t) cur_timestamp;
                uOutBufferIdx++;
                uNumSamplesInPacket++;
            }  /* end if */
            channelIdx++;
            cur_timestamp += timestamp_inc;
            if (channelIdx >= numEnabledChannels) {
                channelIdx = 0;
            }
        }  /* end for */
        *puNumSamples += uNumSamplesInPacket;
        if (uArraySize <=  uNumSamplesInPacket)
            uArraySize = 0 ;
        else
            uArraySize -= uNumSamplesInPacket;
    }

    if (puNumEmpty) {
        *puNumEmpty = uNumEmpty;
    }

    if (puNumOverflow) {
        *puNumOverflow = uNumOverflow;
    }

    return MICRO_EPM_SUCCESS;
}

static int MicroEpmGetDataSortCompareFunction(
    const void *p1,
    const void *p2)
{
    MicroEpmChannelDataType *a1 = (MicroEpmChannelDataType *)p1;
    MicroEpmChannelDataType *a2 = (MicroEpmChannelDataType *)p2;

    if (a1->uRawTimestamp == a2->uRawTimestamp) {
        return 0;
    } else if (a1->uRawTimestamp < a2->uRawTimestamp) {
        return -1;
    } else {
        return 1;
    }
}

/*****************************************************
*  Packet format
*
* Index(Byte)  Description
*     0     Command (0x0C)
*     1     Epmty [7], Overflowed [6], Unused [5], GPIOData [4], Bus index [3:0]
*     2     Number of samples in packet
*     3     Channel Index (first channel reading in packet)
*     4     Channel Bitmask
*     8     Timestamp of first set
*     12    Timestamp of last set
*     16    Reading 0 LSB
*     17    Reading 1 MSB
*      .
*      .
*      .
*     62    Reading 23 LSB
*     63    Reading 23 MSB
*
****************************************************/
static MicroEpmErrorType MicroEpmGetData_4(
    MicroEpmHandle *hDevice,
    MicroEpmChannelDataType *pDataArray,
    uint32_t uDataArrayLength,
    uint32_t *puNumSamples,
    uint32_t uNumPackets,
    uint32_t *puNumOverflow,
    uint32_t *puNumEmpty)
{
    MicroEpmErrorType status;
    uint32_t uStartTime;
    uint32_t uEndTime;
    uint32_t uChannelMask;
    uint32_t uSample;
    uint32_t uChannelIdx = 0;
    uint32_t uNumSamplesInPacket;
    uint32_t auEnabledChannelsList[32];
    uint32_t uNumEnabledChannels = 0;
	qint32 nRaw = 0;
    uint32_t uChannel;
    uint32_t uPacket;
    uint8_t *auEpmInBuf;
    uint32_t uFirstChannel;
    uint32_t uNumEmpty = 0;
    uint32_t uNumOverflow = 0;
    uint32_t uNumSamples = 0;
    uint32_t uNumChannelsInFirstSet;
    uint32_t uNumSets;
    uint32_t uSetPeriod;
    uint32_t uCurrentTime;
    uint8_t uBus;
    uint8_t bIsAdcDataPacket;

    // TODO
    SYSTEMTIME start;
    SYSTEMTIME stop;
    static uint32_t uGDMax = 0;
    static uint32_t uWDMax = 0;
    uint32_t delta;

    // Parameter checking
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    } else if (pDataArray == NULL || puNumSamples == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    } else if (uNumPackets == 0 || uNumPackets > hDevice->version.uMaxPackets) {
        return MICRO_EPM_INVALID_PARAMETER;
    } else if (uDataArrayLength < (uNumPackets * hDevice->version.uMaxSamplesPerPacket)) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    // Get the buffered data
    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBufBig, 0, BUF_SIZE * MAX_PACKETS);

    status = MakeGetBufferedDataCommand(hDevice->aEpmOutBuf,
                                        sizeof(hDevice->aEpmOutBuf));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    GetSystemTime(&start);
    status = MicroEpmComm_SendReceivePacketBuffer(hDevice->hComm,
                                                  hDevice->aEpmOutBuf,
                                                  sizeof(hDevice->aEpmOutBuf),
                                                  hDevice->aEpmInBufBig,
                                                  BUF_SIZE * uNumPackets);
    // TODO
    GetSystemTime(&stop);
    delta = (stop.wMinute*60*1000 + stop.wSecond*1000 + stop.wMilliseconds) - (start.wMinute*60*1000 + start.wSecond*1000 + start.wMilliseconds);
    if (delta > uGDMax) {
        uGDMax = delta;
        printf("ALPACA GD %u\t%u\n", delta, uGDMax);
    }

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Extract the samples from each packet of buffered data
    for (uPacket = 0; uPacket < uNumPackets; uPacket++) {
        auEpmInBuf = &hDevice->aEpmInBufBig[uPacket * BUF_SIZE];

        if (auEpmInBuf[0] != GET_BUFFERED_DATA_RSP) {
            // bad packet format - discard packet
            guNumErrors++;
            continue;
        } else if (auEpmInBuf[1] & 0x80) {
            // empty packet - discard packet
            uNumEmpty++;
            continue;
        }

        uNumSamplesInPacket = auEpmInBuf[2];
        if (uNumSamplesInPacket == 0) {
            // invalid packet - discard
            guNumErrors++;
            continue;
        }

        if (auEpmInBuf[1] & 0x40) {
            // buffer overflowed - do not discard
            uNumOverflow++;
            guNumOverflow++;
        }

        uBus = auEpmInBuf[1] & 0xf;

        if (auEpmInBuf[1] & 0x10) {
            // GPIO data
            if (hDevice->uGpioEnMask == 0) {
                // client hasn't enabled GPIOs - ignore this packet
                continue;
            }
            bIsAdcDataPacket = FALSE;
        } else {
            // ADC data
            bIsAdcDataPacket = TRUE;

            if (uBus >= hDevice->uNumBuses) {
                // invalid packet - discard
                guNumErrors++;
                continue;
            }
        }

        uFirstChannel = auEpmInBuf[3];

        uChannelMask = (((uint32_t)auEpmInBuf[7] << 24)
                        | ((uint32_t)auEpmInBuf[6] << 16)
                        | ((uint32_t)auEpmInBuf[5] << 8)
                        | (uint32_t)auEpmInBuf[4]);

        if (uChannelMask == 0) {
            // no channels are enabled - discard packet
            guNumErrors++;
            continue;
        }

#ifdef  TIME_24BIT
        uStartTime =  ((uint32_t)auEpmInBuf[10] << 16) |
                      ((uint32_t)auEpmInBuf[9] << 8) |
                      (uint32_t)auEpmInBuf[8];

        uEndTime = ((uint32_t)auEpmInBuf[14] << 16) |
                   ((uint32_t)auEpmInBuf[13] << 8) |
                    (uint32_t)auEpmInBuf[12];
#else
        uStartTime = ((uint32_t)auEpmInBuf[11] << 24) |
                     ((uint32_t)auEpmInBuf[10] << 16) |
                     ((uint32_t)auEpmInBuf[9] << 8) |
                      (uint32_t)auEpmInBuf[8];

        uEndTime = ((uint32_t)auEpmInBuf[15] << 24) |
                   ((uint32_t)auEpmInBuf[14] << 16) |
                   ((uint32_t)auEpmInBuf[13] << 8) |
                    (uint32_t)auEpmInBuf[12];
#endif

        uStartTime = (uint32_t)(TIMER_MASK) - uStartTime;
        uEndTime = (uint32_t)(TIMER_MASK) - uEndTime;

        if (bIsAdcDataPacket == TRUE) {
            // Get the array of enabled channels
            uNumEnabledChannels = 0;
            for (uChannel = 0; uChannel < 32; uChannel++) {
                if (uChannelMask & (1 << uChannel)) {
                    auEnabledChannelsList[uNumEnabledChannels] = uChannel;
                    uNumEnabledChannels++;
                }
            }

            // Get the index to the first channel
            uChannelIdx = uNumEnabledChannels;

            for (uChannel = 0; uChannel < uNumEnabledChannels; uChannel++) {
                if (auEnabledChannelsList[uChannel] == uFirstChannel) {
                    // Index to first channel enabled is found
                    uChannelIdx = uChannel;
                    break;
                }
            }

            if (uChannelIdx == uNumEnabledChannels) {
                // The first channel was not found in the list of enabled channels
                guNumErrors++;
                continue;
            }

            // Get the number of sets
            // First, get the number of channels in the first set
            uNumChannelsInFirstSet = uNumEnabledChannels - uChannelIdx;
            if (uNumChannelsInFirstSet >= uNumSamplesInPacket) {
                // All the samples belong to the same set
                uNumSets = 1;
            } else {
                // Partial 1st set + N full sets
                uNumSets = 1 + ((uNumSamplesInPacket - uNumChannelsInFirstSet) / uNumEnabledChannels);

                if (((uNumSamplesInPacket - uNumChannelsInFirstSet) % uNumEnabledChannels) > 0) {
                    // Add in a partial final set
                    uNumSets++;
                }
            }
        } else {
            // GPIO case
            uNumSets = uNumSamplesInPacket;
        }

        // Get the set period
        if (uNumSets == 1) {
            uSetPeriod = 0;
        } else {
            if (uEndTime > uStartTime) {
                // The typical case
                // --------------s=============e----------
                // 0                                    MASK
                uSetPeriod = (uEndTime - uStartTime) / (uNumSets - 1);
            } else {
                // The time has rolled over
                // ====e-----------------------s==========
                // 0                                    MASK
                uSetPeriod = (TIMER_MASK - uStartTime + uEndTime) / (uNumSets - 1);
            }
        }

        uCurrentTime = uStartTime;

        for (uSample = 0; uSample < uNumSamplesInPacket; uSample++) {
            if (bIsAdcDataPacket == TRUE) {
                uint32_t uMicroEpmChanIdx;

                // ADC data
                uChannel = auEnabledChannelsList[uChannelIdx];
                uMicroEpmChanIdx = uChannel + uBus * MAX_CHANNELS_PER_BUS;

                if (hDevice->uEnabledChannelBitmask[uBus] & (1 << uChannel)) {
                    nRaw = (int16_t)(auEpmInBuf[16 + uSample * 2] + (auEpmInBuf[16 + uSample * 2 + 1] << 8));
                    if (uChannel & 1) {
                        pDataArray[uNumSamples].eChannelType = MICRO_EPM_CHANNEL_TYPE_VOLTAGE;
                        pDataArray[uNumSamples].dbPhysical = (double)nRaw * SPMV4_V_MV_PER_CODE;
                    } else {
                        pDataArray[uNumSamples].eChannelType = MICRO_EPM_CHANNEL_TYPE_CURRENT;
                        pDataArray[uNumSamples].dbPhysical = (double)nRaw * SPMV4_I_UV_PER_CODE /
                                                             hDevice->aChannelInfo[uMicroEpmChanIdx].dbRsenseMilliOhms;
                    }

                    pDataArray[uNumSamples].uTarget = hDevice->uTarget;
                    pDataArray[uNumSamples].uModule = hDevice->uModule;
                    pDataArray[uNumSamples].uChannel = uMicroEpmChanIdx;
                    pDataArray[uNumSamples].nRawCode = nRaw;
                    pDataArray[uNumSamples].uRawTimestamp = uCurrentTime;
                    uNumSamples++;
                }  /* end if */

                // Increment to the next channel
                uChannelIdx++;
                if (uChannelIdx >= uNumEnabledChannels) {
                    // Increment to the next set
                    uChannelIdx = 0;

                    // Increment the current time by the set period
                    if ((TIMER_MASK - uCurrentTime) >= uSetPeriod) {
                        uCurrentTime += uSetPeriod;
                    } else {
                        // uCurrentTime needs to roll over
                        uCurrentTime = uSetPeriod - (TIMER_MASK - uCurrentTime);
                    }
                }
            } else {
                // GPIO data
                pDataArray[uNumSamples].uTarget = hDevice->uTarget;
                pDataArray[uNumSamples].uModule = hDevice->uModule;
                pDataArray[uNumSamples].uChannel = 0;
                pDataArray[uNumSamples].eChannelType = MICRO_EPM_CHANNEL_TYPE_GPIO;
                pDataArray[uNumSamples].dbPhysical = 0;
				pDataArray[uNumSamples].nRawCode = (qint32)(auEpmInBuf[16 + uSample * 2]);
                pDataArray[uNumSamples].uRawTimestamp = uCurrentTime;
                uNumSamples++;

                // Increment the current time by the set period
                if ((TIMER_MASK - uCurrentTime) >= uSetPeriod) {
                    uCurrentTime += uSetPeriod;
                } else {
                    // uCurrentTime needs to roll over
                    uCurrentTime = uSetPeriod - (TIMER_MASK - uCurrentTime);
                }
            }
        }  /* end for samples */
    } /* end for packets */

    *puNumSamples = uNumSamples;

    if (puNumEmpty) {
        *puNumEmpty = uNumEmpty;
    }

    if (puNumOverflow) {
        *puNumOverflow = uNumOverflow;
    }

    if (*puNumSamples > 1) {
        qsort(pDataArray, *puNumSamples, sizeof(MicroEpmChannelDataType), MicroEpmGetDataSortCompareFunction);
    }

    if (puNumSamples > 0) {
        GetSystemTime(&start);
        UdasRecordData(hDevice, pDataArray, *puNumSamples);
        // TODO
        GetSystemTime(&stop);
        delta = (stop.wMinute*60*1000 + stop.wSecond*1000 + stop.wMilliseconds) - (start.wMinute*60*1000 + start.wSecond*1000 + start.wMilliseconds);
        if (delta > uWDMax) {
            uWDMax = delta;
            printf("ALPACA WD %u\t%u\n", delta, uWDMax);
        }
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmSendGpioEnable(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t returnCode = 0;

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetGpioBufferStatusCommand(hDevice->aEpmOutBuf,
                                            sizeof(hDevice->aEpmOutBuf),
                                            hDevice->uGpioEnMask);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               SET_GPIO_BUFFER_STATUS_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  SET_GPIO_BUFFER_STATUS_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != SET_GPIO_BUFFER_STATUS_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetGpioBufferStatusResponse(hDevice->aEpmInBuf,
                                              SET_GPIO_BUFFER_STATUS_RSP_LENGTH,
                                              &returnCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (returnCode != EPM_PROTOCOL_STATUS_SUCCESS) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

static MicroEpmErrorType MicroEpmGetEpmID(MicroEpmHandle *hDevice, MicroEpmBoardIdType *pEpmID)
{
    MicroEpmErrorType status;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (pEpmID == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    status = MakeGetEpmIDCommand(hDevice->aEpmOutBuf,GET_EPM_ID_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
      return status;
    }

     // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,GET_EPM_ID_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,GET_EPM_ID_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != GET_EPM_ID_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseGetEpmIDResponse(hDevice->aEpmInBuf,GET_EPM_ID_RSP_LENGTH,pEpmID);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    return MICRO_EPM_SUCCESS;
}

static void MicroEpmLogSample(MicroEpmChannelStatType *pStats, double dbPhysical)
{
    if (pStats->uNumSamples == 0) {
        pStats->dbAverage = dbPhysical;
        pStats->dbMin = dbPhysical;
        pStats->dbMax = dbPhysical;
    } else {
        if (dbPhysical > pStats->dbMax) {
            pStats->dbMax = dbPhysical;
        }
        if (dbPhysical < pStats->dbMin) {
            pStats->dbMin = dbPhysical;
        }
    }
    pStats->uNumSamples++;
    pStats->dbAverage += (dbPhysical - pStats->dbAverage) / pStats->uNumSamples;
}

static DWORD WINAPI MicroEpmGetDataThread(LPVOID pCtxt)
{
    MicroEpmHandle *hDevice = (MicroEpmHandle *)pCtxt;
    MicroEpmErrorType status;
    uint32_t uMaxNumSamples;
    MicroEpmChannelDataType *pDataArray;
    uint32_t uNumSamples;
    uint32_t uNumOverflow;
    uint32_t uNumEmpty;
    uint32_t uSample;
    DWORD ret = MICRO_EPM_THREAD_SUCCESS;

    /* Need to boost thread priority to avoid buffer overflows */
    if (!SetThreadPriority(hDevice->hGetData, THREAD_PRIORITY_HIGHEST)) {
        return MICRO_EPM_THREAD_ERROR;
    }

    uMaxNumSamples = hDevice->version.uMaxPackets * hDevice->version.uMaxSamplesPerPacket;
    pDataArray = (MicroEpmChannelDataType *)malloc(sizeof(MicroEpmChannelDataType) * uMaxNumSamples);
    if (pDataArray == NULL) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    status = MicroEpmClearBuffer(hDevice);
    if (status != MICRO_EPM_SUCCESS) {
        free(pDataArray);
        return MICRO_EPM_THREAD_ERROR;
    }

    while (hDevice->bAcquiring) {
        status = MicroEpmGetData(hDevice,
                                 pDataArray,
                                 uMaxNumSamples,
                                 &uNumSamples,
                                 hDevice->version.uMaxPackets,
                                 &uNumOverflow,
                                 &uNumEmpty);
        if (status != MICRO_EPM_SUCCESS) {
            free(pDataArray);
            return MICRO_EPM_THREAD_ERROR;
        }

        if (uNumOverflow != 0) {
            /* Return an error, but don't stop sampling */
            // TODO
            printf("ERROR: ALPACA BUFFER OVERFLOW OCCURRED\n");
            ret = MICRO_EPM_THREAD_OVERFLOW;
        }

        for (uSample = 0; uSample < uNumSamples; uSample++) {
            MicroEpmChannelDataType *pData = &pDataArray[uSample];
            MicroEpmChannelStatType *pStats;

            if (pData->eChannelType == MICRO_EPM_CHANNEL_TYPE_GPIO) {
                uint32_t uGpio;

                for (uGpio = 0; uGpio < MAX_NUM_GPIO_CHANNELS; uGpio++) {
                    double dbVal;

                    pStats = &hDevice->aChannelInfo[MAX_NUM_ADC_CHANNELS + uGpio].stats;

                    if (pData->nRawCode & (1 << uGpio)) {
                        dbVal = GPIO_HIGH_V * MILLI_PER_BASE;
                    } else {
                        dbVal = GPIO_LOW_V * MILLI_PER_BASE;
                    }

                    MicroEpmLogSample(pStats, dbVal);
                }
            } else {
                pStats = &hDevice->aChannelInfo[pData->uChannel].stats;

                MicroEpmLogSample(pStats, pData->dbPhysical);
            }
        }
    }

    free(pDataArray);

    return ret;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*
 * Init / DeInit
 */

MicroEpmErrorType MicroEpmInit(void)
{
    MicroEpmErrorType status = MICRO_EPM_SUCCESS;
	qint32 i;

    if (!gbIsMicroEpmInitialized) {
        for (i = 0; i < MAX_NUMBER_MODULES; i++) {
            memset(&gMicroEpmDevCtxt[i], 0, sizeof(MicroEpmHandle));
        }

        status = MicroEpmComm_Init();

        if (MICRO_EPM_SUCCESS == status) {
            gbIsMicroEpmInitialized = TRUE;
        }
    }

    return status;
}

MicroEpmErrorType MicroEpmDeInit(void)
{
    MicroEpmErrorType status = MICRO_EPM_SUCCESS;
    uint32_t i;

    for (i = 0; i < guNumTargets; i++) {
        (void)MicroEpmCloseTarget(i);
    }

    (void)MicroEpmDisconnect();

    for (i = 0; i < MAX_NUMBER_MODULES; i++) {
        memset(&gMicroEpmDevCtxt[i], 0, sizeof(MicroEpmHandle));
    }

    gbIsMicroEpmInitialized = FALSE;

    guNumModules = 0;
    guNumTargets = 0;

    return status;
}

MicroEpmErrorType MicroEpmConnect(uint32_t *puNumberOfTargetsConnected)
{
    MicroEpmErrorType status = MICRO_EPM_SUCCESS;
    MicroEpmHandle *pEpmDevCtxt;
    MicroEpmGpioValueType GpioVal;
    uint32_t numberOfModules = 0;
    uint32_t uNumberOfUnprogrammedModules = 0;
    uint32_t i;
    uint32_t uNumberOfSpm = 0;
    uint32_t uChannel;
    uint32_t uAdc;
    uint8_t uBus;

    if (puNumberOfTargetsConnected == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    *puNumberOfTargetsConnected = 0;

    if (!gbIsMicroEpmInitialized) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    for (i = 0; i < MAX_NUMBER_MODULES; i++) {
        if (gMicroEpmDevCtxt[i].aEpmInBufBig) {
            free(gMicroEpmDevCtxt[i].aEpmInBufBig);
        }
        gMicroEpmDevCtxt[i].aEpmInBufBig = NULL;
        memset(&gMicroEpmDevCtxt[i], 0, sizeof(MicroEpmHandle));
    }

    status = MicroEpmComm_Connect(&numberOfModules);

    if (status != MICRO_EPM_SUCCESS) {
        if (status == MICRO_EPM_TOO_MANY_DEVICES) {
            *puNumberOfTargetsConnected = MAX_NUMBER_MODULES;
        }
        return status;
    }

    guNumModules = numberOfModules;
    for (i = 0; i < numberOfModules; i++) {
        pEpmDevCtxt = &gMicroEpmDevCtxt[i];
        pEpmDevCtxt->aEpmInBufBig = (uint8_t *)malloc(BUF_SIZE*MAX_PACKETS);
        if (!pEpmDevCtxt->aEpmInBufBig) {
            goto error;
        }

        status = MicroEpmComm_GetHandle(i, &pEpmDevCtxt->hComm);
        if (status != MICRO_EPM_SUCCESS) {
            goto error;
        }

        /* Send Hello command */
        status = SendReceiveHelloPacket(pEpmDevCtxt);
        if (status != MICRO_EPM_SUCCESS) {
            goto error;
        }

        pEpmDevCtxt->bIsConnected = TRUE;

        if (pEpmDevCtxt->version.uFirmwareVersion[0] == SPMV4_MAJOR_VERSION) {
            // SPMv4
            pEpmDevCtxt->bIsSpmV4 = TRUE;
            pEpmDevCtxt->bPaused = FALSE;
            pEpmDevCtxt->bInSecureMode = FALSE;
            pEpmDevCtxt->uNumBuses = MAX_BUSES;
            pEpmDevCtxt->eAdcMode = MICRO_EPM_DEFAULT_ADC_MODE;
            pEpmDevCtxt->uSetPeriod = MICRO_EPM_DEFAULT_SET_PERIOD;
            pEpmDevCtxt->uMaxDataRate = MICRO_EPM_DEFAULT_DATA_RATE;
            pEpmDevCtxt->uNumChannels *= 2;   // SPMv4 reports 64 channels, but each has V & I
            pEpmDevCtxt->uGpioEnMask = 0x0;
            pEpmDevCtxt->version.uMaxSamplesPerPacket = MAX_SAMPLES_PER_PACKET;
            pEpmDevCtxt->version.uMaxSamplesPerAveragePacket = 0;
            pEpmDevCtxt->version.uHostLowestCompatibleProtocolVersion = MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION_SPMV4;
            pEpmDevCtxt->bAcquiring = FALSE;
            pEpmDevCtxt->hGetData = NULL;

            if (pEpmDevCtxt->version.uFirmwareProtocolVersion >= 7) {
                status = MicroEpmGetEpmID(pEpmDevCtxt,&pEpmDevCtxt->version.EpmID);
                if (status != MICRO_EPM_SUCCESS) {
                    return status;
                }
            } else {
                pEpmDevCtxt->version.EpmID = MICRO_EPM_BOARD_ID_SPMV4;
            }

            for (uBus = 0; uBus < MAX_BUSES; uBus++) {
                for (uAdc = 0; uAdc < MAX_ADCS_PER_BUS; uAdc++) {
                    pEpmDevCtxt->aeAveragingMode[uBus][uAdc] = MICRO_EPM_DEFAULT_AVERAGING_MODE;
                }

                for (uChannel = 0; uChannel < MAX_CHANNELS_PER_BUS; uChannel++) {
                    pEpmDevCtxt->aeConvTime[uBus][uChannel] = MICRO_EPM_DEFAULT_CONV_TIME;
                }
            }

            for (uChannel = 0; uChannel < MICRO_EPM_MAX_NUMBER_CHANNELS; uChannel++) {
                MicroEpmChannelInfoType *pChanInfo = &pEpmDevCtxt->aChannelInfo[uChannel];
                pChanInfo->szName[0] = '\0';
                pChanInfo->dbRsenseMilliOhms = MICRO_EPM_DEFAULT_RSENSE_MOHM;
                pChanInfo->uRcmChannel = uChannel;
                memset(&pChanInfo->stats, 0, sizeof(MicroEpmChannelStatType));
            }
        } else {
            // SPMv3
            pEpmDevCtxt->bIsSpmV4 = FALSE;
            pEpmDevCtxt->uNumBuses = 1;
            pEpmDevCtxt->version.uMaxSamplesPerPacket = MAX_SAMPLES_PER_PACKET + 1;   // SPMv3 has GPIO data in the header
            pEpmDevCtxt->version.uMaxSamplesPerAveragePacket = MAX_SAMPLES_PER_AVERAGE_PACKET;
            pEpmDevCtxt->version.uHostLowestCompatibleProtocolVersion = MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION_SPMV3;
            pEpmDevCtxt->version.EpmID = MICRO_EPM_BOARD_ID_SPMV3;
        }

        (void)MicroEpmEepromRead(pEpmDevCtxt, FALSE);
        if (pEpmDevCtxt->bIsProgrammed == FALSE) {
            uNumberOfUnprogrammedModules++;
        }
    }

    if (uNumberOfUnprogrammedModules > MAX_NUMBER_MODULES_PER_TARGET) {
        return MICRO_EPM_NOT_PROGRAMMED;
    }

    status = CountNumberOfSpmBoards(&uNumberOfSpm, numberOfModules);
    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // If there's just 1 spm connected then detect master/slave using
    // gpio to make it easy to program
    if (uNumberOfSpm == 1 && guNumModules == MAX_NUMBER_MODULES_PER_TARGET) {
        for (i = 0; i < guNumModules; i++) {
            status = MicroEpmGetGpioValue(&gMicroEpmDevCtxt[i],
                                          MICRO_EPM_GPIO_PIN_GPIO_5, &GpioVal);
            if (status != MICRO_EPM_SUCCESS) {
                goto error;
            }
            /* Determine master / slave status on SPM board */
            if (GpioVal == MICRO_EPM_GPIO_VALUE_LOW) {
                gMicroEpmIndex[0][0] = i;
                guNumTargets++;
            } else {
                gMicroEpmIndex[0][1] = i;
            }
            gNumberOfChildModules[0] = 2;
        }
    } else {
        status = DetectTargets(numberOfModules, uNumberOfSpm);
        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
    }

    *puNumberOfTargetsConnected = guNumTargets;

    return MICRO_EPM_SUCCESS;

error:
    MicroEpmDisconnect();

    return MICRO_EPM_ERROR;
}

// TODO: send goodbye packet for SPMv4?
MicroEpmErrorType MicroEpmDisconnect(void)
{
    uint32_t i;
    MicroEpmHandle *pEpmDevCtxt;
    MicroEpmErrorType status;
    MicroEpmErrorType retStatus = MICRO_EPM_SUCCESS;

    if (!gbIsMicroEpmInitialized) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    for (i = 0; i < MAX_NUMBER_MODULES; i++) {
        pEpmDevCtxt = &gMicroEpmDevCtxt[i];

        if (pEpmDevCtxt->bIsConnected == TRUE) {
            // Send good-bye command here, if ever needed
            pEpmDevCtxt->bIsConnected = FALSE;
        }

        if (pEpmDevCtxt->hComm != NULL) {
            status = MicroEpmComm_Disconnect(pEpmDevCtxt->hComm);

            if (status != MICRO_EPM_SUCCESS) {
                retStatus = status;
            }

            pEpmDevCtxt->hComm = NULL;
        }

        if (pEpmDevCtxt->aEpmInBufBig) {
            free(pEpmDevCtxt->aEpmInBufBig);
            pEpmDevCtxt->aEpmInBufBig = NULL;
        }
    }

    for (i = 0; i < MAX_NUMBER_TARGETS; i++) {
        gNumberOfChildModules[i] = 0;
    }

    guNumModules = 0;
    guNumTargets = 0;

    return retStatus;
}

MicroEpmErrorType MicroEpmOpenTarget(
    uint32_t uTarget,
    uint32_t* puNumberOfModulesOnTarget)
{
    if (puNumberOfModulesOnTarget == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uTarget >= guNumTargets) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    *puNumberOfModulesOnTarget = gNumberOfChildModules[uTarget];

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmCloseTarget(uint32_t uTarget)
{
    uint32_t uModule;

    if (uTarget >= guNumTargets) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    for (uModule = 0; uModule < gNumberOfChildModules[uTarget]; uModule++) {
        MicroEpmHandle *hDevice = &gMicroEpmDevCtxt[ gMicroEpmIndex[uTarget][uModule] ];
        if (hDevice->bAcquiring) {
            (void)MicroEpmStopAcquisition(hDevice, NULL);
        }
    }

    gNumberOfChildModules[uTarget] = 0;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmGetModuleHandle(
    uint32_t uTarget,
    uint32_t uModule,
    MicroEpmHandle **hDevice)
{
    if (uTarget >= guNumTargets) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uModule >= gNumberOfChildModules[uTarget]) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    *hDevice = &gMicroEpmDevCtxt[ gMicroEpmIndex[uTarget][uModule] ];

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmGetVersionInfo(
    MicroEpmHandle *hDevice,
    MicroEpmVersionInfoType *versionInfo)
{
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    *versionInfo = hDevice->version;

    return MICRO_EPM_SUCCESS;
}

/*
 * Timestamp
 */

MicroEpmErrorType MicroEpmGetTimestamp(
    MicroEpmHandle *hDevice,
    uint32_t *puRawTimestamp)
{
    MicroEpmErrorType status;
    uint32_t uReceivedTimestamp;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (puRawTimestamp == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    status = MicroEpmGetRawTimestamp(hDevice, &uReceivedTimestamp);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    *puRawTimestamp = uReceivedTimestamp;

    return status;
}

MicroEpmErrorType MicroEpmSetTimestamp(
    MicroEpmHandle *hDevice,
    uint32_t uRawTimestamp)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    return MicroEpmSetRawTimestamp(hDevice, uRawTimestamp);
}

/*
 * Channel Configuration
 */

MicroEpmErrorType MicroEpmSetChannelEnable(
    MicroEpmHandle *hDevice,
    uint32_t uChannel,
    MicroEpmChannelEnableType eEnable)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    if (uChannel >= MAX_NUM_ADC_CHANNELS) {
        /* GPIO channel */
        uint32_t uGpio = uChannel - hDevice->uNumChannels;

        if (eEnable == MICRO_EPM_CHANNEL_ENABLE) {
            hDevice->uGpioEnMask |= 1 << uGpio;
        } else if (eEnable == MICRO_EPM_CHANNEL_DISABLE) {
            hDevice->uGpioEnMask &= ~(1 << uGpio);
        } else {
            return MICRO_EPM_ERROR;
        }
    } else {
        /* Power measurement channel */
        uint32_t uBus = uChannel / MAX_CHANNELS_PER_BUS;
        uint32_t bitmask;

        if (uBus >= MAX_BUSES) {
           return MICRO_EPM_INVALID_PARAMETER;
        }

        if (eEnable == MICRO_EPM_CHANNEL_ENABLE) {
            bitmask = hDevice->uEnabledChannelBitmask[uBus];
            bitmask |= (1 << (uChannel % MAX_CHANNELS_PER_BUS));
            hDevice->uEnabledChannelBitmask[uBus] = bitmask;
        } else if (eEnable == MICRO_EPM_CHANNEL_DISABLE) {
            bitmask = hDevice->uEnabledChannelBitmask[uBus];
            bitmask &= ~(1 << (uChannel % MAX_CHANNELS_PER_BUS));
            hDevice->uEnabledChannelBitmask[uBus] = bitmask;
        } else {
            return MICRO_EPM_ERROR;
        }
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetChannelName(
    MicroEpmHandle *hDevice,
    uint32_t uChannel,
    const char *pszName)
{
    if (hDevice == NULL || pszName == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (strlen(pszName) >= (MAX_CHAN_BUFFER_LEN - 1)) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    strcpy_s(hDevice->aChannelInfo[uChannel].szName, MAX_CHAN_BUFFER_LEN, pszName);

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetRsense(
    MicroEpmHandle *hDevice,
    uint32_t uChannel,
    double dbRsenseMilliOhms)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uChannel >= hDevice->uNumChannels) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    hDevice->aChannelInfo[uChannel].dbRsenseMilliOhms = dbRsenseMilliOhms;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetRcmChannel(
    MicroEpmHandle *hDevice,
    uint32_t uChannel,
    uint32_t uRcmChannel)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    hDevice->aChannelInfo[uChannel].uRcmChannel = uRcmChannel;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetChannelType(
    MicroEpmHandle *hDevice,
    uint32_t uChannel,
    MicroEpmChannelType channelType)
{
    uint32_t bitmask;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uChannel > 31) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    bitmask = hDevice->uVoltageChannelBitmask;

    if (channelType == MICRO_EPM_CHANNEL_TYPE_VOLTAGE) {
        bitmask |= (1 << uChannel);
    } else if (channelType == MICRO_EPM_CHANNEL_TYPE_CURRENT) {
        bitmask &= ~(1 << uChannel);
    } else {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    hDevice->uVoltageChannelBitmask = bitmask;

    return MICRO_EPM_SUCCESS;
}

/*
 * Channel Configuration - SPMv3
 */

MicroEpmErrorType MicroEpmApplyChannelEnableSettings(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint32_t bitmask;
    uint8_t receivedStatusCode;
    uint32_t uReceivedBitmask;
    uint8_t uBus;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 == TRUE) {
       return MICRO_EPM_NOT_IMPLEMENTED;
    }

    for (uBus = 0; uBus < hDevice->uNumBuses; uBus++) {
        bitmask = hDevice->uEnabledChannelBitmask[uBus];

        memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
        memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

        status = MakeEnableDisableChannelCommand(hDevice->aEpmOutBuf,
                 sizeof(hDevice->aEpmOutBuf),
                 uBus, bitmask);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Send command
        status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                                   ENABLE_DISABLE_CHANNEL_CMD_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Receive response
        status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                      ENABLE_DISABLE_CHANNEL_RSP_LENGTH);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        // Parse response
        if (hDevice->aEpmInBuf[0] != ENABLE_DISABLE_CHANNEL_RSP) {
            return MICRO_EPM_COMM_ERROR;
        }

        status = ParseEnableDisableChannelResponse(hDevice->aEpmInBuf,
                 ENABLE_DISABLE_CHANNEL_RSP_LENGTH, &receivedStatusCode,
                 &uReceivedBitmask);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        if (bitmask != uReceivedBitmask || EPM_PROTOCOL_STATUS_SUCCESS != receivedStatusCode) {
            return MICRO_EPM_COMM_ERROR;
        }

        SLEEP(25);
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmApplyChannelTypeSettings(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint32_t bitmask;
    uint8_t receivedDevice;
    uint32_t uReceivedBitmask;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 == TRUE) {
       return MICRO_EPM_NOT_IMPLEMENTED;
    }

    bitmask = hDevice->uVoltageChannelBitmask;

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetChannelTypesCommand(hDevice->aEpmOutBuf,
                                        sizeof(hDevice->aEpmOutBuf), 0, bitmask);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               SET_CHANNEL_TYPES_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  SET_CHANNEL_TYPES_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != SET_CHANNEL_TYPES_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetChannelTypesResponse(hDevice->aEpmInBuf,
                                          SET_CHANNEL_TYPES_RSP_LENGTH, &receivedDevice, &uReceivedBitmask);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (bitmask != uReceivedBitmask) {
        return MICRO_EPM_COMM_ERROR;
    }

    SLEEP(25);

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetChannelSwitchDelay(
    MicroEpmHandle *hDevice,
    uint32_t uDelayInMicroSeconds,
    uint32_t *puActualDelay)
{
    MicroEpmErrorType status;
    uint8_t ReceivedDevice = 0;
    uint32_t uActualDelay;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 == TRUE) {
       return MICRO_EPM_NOT_IMPLEMENTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MicroEpmPauseAdcConversions(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    SLEEP(25);

    status = MakeSetChannelSwitchDelayCommand(hDevice->aEpmOutBuf,
             sizeof(hDevice->aEpmOutBuf), 0, uDelayInMicroSeconds);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               SET_CHANNEL_SWITCH_DELAY_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  SET_CHANNEL_SWITCH_DELAY_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != SET_CHANNEL_SWITCH_DELAY_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetChannelSwitchDelayResponse(hDevice->aEpmInBuf,
                                                SET_CHANNEL_SWITCH_DELAY_RSP_LENGTH,
                                                &ReceivedDevice,
                                                &uActualDelay);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (puActualDelay != NULL) {
        *puActualDelay = uActualDelay;
    }

    status = MicroEpmUnpauseAdcConversions(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    SLEEP(25);

    return MICRO_EPM_SUCCESS;
}

/*
 * Channel Configuration - SPMv4
 */

MicroEpmErrorType MicroEpmSetAveragingMode(
    MicroEpmHandle *hDevice,
    uint32_t uVoltageChannel,
    uint32_t uCurrentChannel,
    MicroEpmAveragingModeType eAveragingMode)
{
    uint32_t uBus;
    uint32_t uChannelOnBus;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uVoltageChannel >= hDevice->uNumChannels) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uCurrentChannel >= hDevice->uNumChannels) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uCurrentChannel + 1 != uVoltageChannel) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (eAveragingMode >= _MICRO_EPM_AVERAGING_MODE_NUM) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 != TRUE) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    uBus = uCurrentChannel / MAX_CHANNELS_PER_BUS;

    if (uBus >= MAX_BUSES) {
       return MICRO_EPM_INVALID_PARAMETER;
    }

    uChannelOnBus = (uCurrentChannel / 2) % MAX_ADCS_PER_BUS;

    hDevice->aeAveragingMode[uBus][uChannelOnBus] = eAveragingMode;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetAdcMode(
    MicroEpmHandle *hDevice,
    MicroEpmAdcModeType eAdcMode)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (eAdcMode >= _MICRO_EPM_ADC_MODE_NUM) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 != TRUE) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    hDevice->eAdcMode = eAdcMode;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetSetPeriod(
    MicroEpmHandle *hDevice,
    uint32_t uSetPeriod)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uSetPeriod == 0) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 != TRUE) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    hDevice->uSetPeriod = uSetPeriod;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetConversionTime(
    MicroEpmHandle *hDevice,
    uint32_t uChannel,
    MicroEpmConvTimeType eConvTime)
{
    uint32_t uBus;
    uint32_t uChannelOnBus;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uChannel >= hDevice->uNumChannels) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (eConvTime >= _MICRO_EPM_CONV_TIME_NUM) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 != TRUE) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    uBus = uChannel / MAX_CHANNELS_PER_BUS;

    if (uBus >= MAX_BUSES) {
       return MICRO_EPM_INVALID_PARAMETER;
    }

    uChannelOnBus = uChannel % MAX_CHANNELS_PER_BUS;

    hDevice->aeConvTime[uBus][uChannelOnBus] = eConvTime;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetDataRateGovernor(
    MicroEpmHandle *hDevice,
    uint32_t uMaxDataRate)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uMaxDataRate == 0) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 != TRUE) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    hDevice->uMaxDataRate = uMaxDataRate;

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmApplySettings(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 != TRUE) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    if (hDevice->eAdcMode == MICRO_EPM_ADC_MODE_TRIGGERED) {
        if (VerifyTriggeredModeInput(hDevice) != MICRO_EPM_SUCCESS) {
            return MICRO_EPM_ERROR;
        }
    }

    // Send channel enable / disable settings
    status = MicroEpmSendEnableDisable(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send averaging mode settings
    status = MicroEpmSendAveragingMode(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send ADC mode setting
    status = MicroEpmSendAdcMode(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send set period setting
    status = MicroEpmSendSetPeriod(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send conversion time settings
    status = MicroEpmSendConvTime(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send max data rate setting
    status = MicroEpmSendDataRateGovernor(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send GPIO enable setting
    status = MicroEpmSendGpioEnable(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send apply settings command
    status = MicroEpmSendApplySettings(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    return MICRO_EPM_SUCCESS;
}

/*
 * Buffered Data
 */

MicroEpmErrorType MicroEpmGetData(
    MicroEpmHandle *hDevice,
    MicroEpmChannelDataType *pDataArray,
    uint32_t uDataArrayLength,
    uint32_t *puNumSamples,
    uint32_t uNumPackets,
    uint32_t *puNumOverflow,
    uint32_t *puNumEmpty)
{
   if (!hDevice->bIsSpmV4) {
      return MicroEpmGetData_3(hDevice, pDataArray, uDataArrayLength, puNumSamples, uNumPackets, puNumOverflow, puNumEmpty);
   } else {
      return MicroEpmGetData_4(hDevice, pDataArray, uDataArrayLength, puNumSamples, uNumPackets, puNumOverflow, puNumEmpty);
   }
}

MicroEpmErrorType MicroEpmClearBuffer(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t uReturnCode;
    uint8_t uBitmask = 0xff;   // Currently unused on SPMv3 & SPMv4

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeClearBufferCommand(hDevice->aEpmOutBuf,
                                    sizeof(hDevice->aEpmOutBuf),
                                    uBitmask);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               CLEAR_BUFFER_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  CLEAR_BUFFER_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != CLEAR_BUFFER_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseClearBufferResponse(hDevice->aEpmInBuf,
                                      CLEAR_BUFFER_RSP_LENGTH, &uReturnCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (uReturnCode != EPM_PROTOCOL_STATUS_SUCCESS) {
        return MICRO_EPM_COMM_ERROR;
    }

    if (!(hDevice->bIsSpmV4 == FALSE && hDevice->uEnabledChannelBitmask[0] == 0)) {
        // Get 1 packet of buffered data because to avoid receiving one stale packet
        // This is because the buffer on the PSoC is pre-filled
        memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
        memset(hDevice->aEpmInBufBig, 0, BUF_SIZE * MAX_PACKETS);

        status = MakeGetBufferedDataCommand(hDevice->aEpmOutBuf,
                                            sizeof(hDevice->aEpmOutBuf));

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }

        status = MicroEpmComm_SendReceivePacketBuffer(hDevice->hComm,
                                                      hDevice->aEpmOutBuf,
                                                      sizeof(hDevice->aEpmOutBuf),
                                                      hDevice->aEpmInBufBig,
                                                      BUF_SIZE);
    }

    return status;
}

MicroEpmErrorType
MicroEpmPauseAdcConversions(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 == TRUE && hDevice->bPaused == TRUE) {
        return MICRO_EPM_SUCCESS;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakePauseAdcConversionsCommand(hDevice->aEpmOutBuf,
                                            sizeof(hDevice->aEpmOutBuf));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               PAUSE_ADC_CONVERSIONS_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  PAUSE_ADC_CONVERSIONS_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != PAUSE_ADC_CONVERSIONS_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParsePauseAdcConversionsResponse(hDevice->aEpmInBuf,
                                              PAUSE_ADC_CONVERSIONS_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    SLEEP(15);

    if (hDevice->bIsSpmV4 == TRUE) {
        hDevice->bPaused = TRUE;
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmUnpauseAdcConversions(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 == TRUE && hDevice->bPaused == FALSE) {
        return MICRO_EPM_SUCCESS;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeUnpauseAdcConversionsCommand(hDevice->aEpmOutBuf,
             sizeof(hDevice->aEpmOutBuf));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               UNPAUSE_ADC_CONVERSIONS_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  UNPAUSE_ADC_CONVERSIONS_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != UNPAUSE_ADC_CONVERSIONS_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseUnpauseAdcConversionsResponse(hDevice->aEpmInBuf,
                                                UNPAUSE_ADC_CONVERSIONS_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (hDevice->bIsSpmV4 == TRUE) {
        hDevice->bPaused = FALSE;
    }

    return MICRO_EPM_SUCCESS;
}

/*
 * GPIO
 */

MicroEpmErrorType MicroEpmGetGpioValue(
    MicroEpmHandle *hDevice,
    MicroEpmGpioPinType gpioChannel,
    MicroEpmGpioValueType *pGpioValueType)
{
    MicroEpmErrorType status;
    MicroEpmGpioPinType receivedPin;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (pGpioValueType == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeGetGpioValueCommand(hDevice->aEpmOutBuf,
                                     sizeof(hDevice->aEpmOutBuf), gpioChannel);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               GET_GPIO_VALUE_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  GET_GPIO_VALUE_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != GET_GPIO_VALUE_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseGetGpioValueResponse(hDevice->aEpmInBuf,
                                       GET_GPIO_VALUE_RSP_LENGTH, &receivedPin, pGpioValueType);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (receivedPin != gpioChannel) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetGpioValue(
    MicroEpmHandle *hDevice,
    MicroEpmGpioPinType gpioChannel,
    MicroEpmGpioValueType gpioValue)
{
    MicroEpmErrorType status;
    MicroEpmGpioPinType receivedPin;
    MicroEpmGpioValueType receivedValue;

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeSetGpioValueCommand(hDevice->aEpmOutBuf,
                                     sizeof(hDevice->aEpmOutBuf), gpioChannel, gpioValue);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               SET_GPIO_VALUE_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  SET_GPIO_VALUE_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != SET_GPIO_VALUE_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseSetGpioValueResponse(hDevice->aEpmInBuf,
                                       SET_GPIO_VALUE_RSP_LENGTH, &receivedPin, &receivedValue);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (receivedPin != gpioChannel) {
        return MICRO_EPM_COMM_ERROR;
    }

    if (receivedValue != gpioValue) {
        return MICRO_EPM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmSetGpioBufferEnable(
    MicroEpmHandle *hDevice,
    uint8_t uGpioEnMask)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 != TRUE) {
        return MICRO_EPM_NOT_IMPLEMENTED;
    }

    hDevice->uGpioEnMask = uGpioEnMask;

    return MICRO_EPM_SUCCESS;
}

/*
 * Misc
 */

MicroEpmErrorType MicroEpmEnterSecureMode(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;
    uint8_t returnCode = 0;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (hDevice->bIsSpmV4 == TRUE && hDevice->bInSecureMode == TRUE) {
        return MICRO_EPM_SUCCESS;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeEnterSecureModeCommand(hDevice->aEpmOutBuf,
                                        sizeof(hDevice->aEpmOutBuf));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               ENTER_SECURE_MODE_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  ENTER_SECURE_MODE_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != ENTER_SECURE_MODE_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseEnterSecureModeResponse(hDevice->aEpmInBuf,
                                          ENTER_SECURE_MODE_RSP_LENGTH, &returnCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (returnCode != EPM_PROTOCOL_STATUS_SUCCESS) {
        return MICRO_EPM_COMM_ERROR;
    }

    if (hDevice->bIsSpmV4 == TRUE) {
        hDevice->bInSecureMode = TRUE;
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmEnterBootloader(MicroEpmHandle *hDevice)
{
    MicroEpmErrorType status;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    status = MicroEpmEnterSecureMode(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeEnterBootloaderCommand(hDevice->aEpmOutBuf,
                                        sizeof(hDevice->aEpmOutBuf));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               ENTER_BOOTLOADER_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS && status != MICRO_EPM_COMM_ERROR_NOT_CONNECTED) {
        return status;
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmIna231RegisterRead(
    MicroEpmHandle *hDevice,
    uint8_t uBus,
    uint8_t uAddress,
    uint8_t uRegister,
	quint16 *puData)
{
    MicroEpmErrorType status;
    uint8_t bWasPaused;
    uint8_t uReceivedStatusCode;

    if (hDevice == NULL || puData == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (uBus >= MAX_BUSES) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    // Enter secure mode
    status = MicroEpmEnterSecureMode(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Pause
    bWasPaused = hDevice->bPaused;

    if (bWasPaused == FALSE) {
        status = MicroEpmPauseAdcConversions(hDevice);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
    }

    // Read the register
    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeIna231RegisterReadCommand(hDevice->aEpmOutBuf,
                                           sizeof(hDevice->aEpmOutBuf),
                                           uBus,
                                           uAddress,
                                           uRegister);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               INA231_REGISTER_READ_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  INA231_REGISTER_READ_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != INA231_REGISTER_READ_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseIna231RegisterReadResponse(hDevice->aEpmInBuf,
                                             INA231_REGISTER_READ_RSP_LENGTH,
                                             &uReceivedStatusCode,
                                             puData);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (uReceivedStatusCode != EPM_PROTOCOL_STATUS_SUCCESS) {
        return MICRO_EPM_COMM_ERROR;
    }

    // Unpause
    if (bWasPaused == FALSE) {
        status = MicroEpmUnpauseAdcConversions(hDevice);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmIna231RegisterWrite(
    MicroEpmHandle *hDevice,
    uint8_t uBus,
    uint8_t uAddress,
    uint8_t uRegister,
	quint16 uData)
{
    MicroEpmErrorType status;
    uint8_t bWasPaused;
    uint8_t uReceivedStatusCode;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (uBus >= MAX_BUSES) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    // Enter secure mode
    status = MicroEpmEnterSecureMode(hDevice);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Pause
    bWasPaused = hDevice->bPaused;

    if (bWasPaused == FALSE) {
        status = MicroEpmPauseAdcConversions(hDevice);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeIna231RegisterWriteCommand(hDevice->aEpmOutBuf,
                                            sizeof(hDevice->aEpmOutBuf),
                                            uBus,
                                            uAddress,
                                            uRegister,
                                            uData);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               INA231_REGISTER_WRITE_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  INA231_REGISTER_WRITE_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != INA231_REGISTER_WRITE_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseIna231RegisterWriteResponse(hDevice->aEpmInBuf,
                                              INA231_REGISTER_WRITE_RSP_LENGTH,
                                              &uReceivedStatusCode);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    if (uReceivedStatusCode != EPM_PROTOCOL_STATUS_SUCCESS) {
        return MICRO_EPM_COMM_ERROR;
    }

    // Unpause
    if (bWasPaused == FALSE) {
        status = MicroEpmUnpauseAdcConversions(hDevice);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmPowerOnTest(
    MicroEpmHandle *hDevice,
    uint8_t uNumIterations,
    uint8_t uNumBuses,
	const quint16 *pauAdcPopulatedMask,
	quint16 *pauAdcFailedMask)
{
    MicroEpmErrorType status = MICRO_EPM_SUCCESS;
    uint8_t uReceivedStatusCode;
    uint8_t bWasPaused;
	quint16 auEnabledAdcMask[MAX_BUSES] = {0};
	quint16 auAdcFailedMask[MAX_BUSES] = {0};
    uint8_t uBus;
    uint8_t uAdc;
    uint8_t bAdcEnabled = FALSE;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }
    if (uNumIterations == 0) {
        return MICRO_EPM_INVALID_PARAMETER;
    }
    if (uNumBuses > MAX_BUSES) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    // Pause
    bWasPaused = hDevice->bPaused;

    if (bWasPaused == FALSE) {
        status = MicroEpmPauseAdcConversions(hDevice);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
    }

    // Send power on test command
    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    if (pauAdcPopulatedMask == NULL) {
        for (uBus = 0; uBus < MAX_BUSES; uBus++) {
            for (uAdc = 0; uAdc < MAX_ADCS_PER_BUS; uAdc++) {
                if (hDevice->uEnabledChannelBitmask[uBus] & (0x3 << (uAdc * 2))) {
                    auEnabledAdcMask[uBus] |= 1 << uAdc;
                    bAdcEnabled = TRUE;
                }
            }
        }
        if (bAdcEnabled == TRUE) {
            status = MakePowerOnTestCommand(hDevice->aEpmOutBuf, sizeof(hDevice->aEpmOutBuf), uNumIterations, MAX_BUSES, auEnabledAdcMask);
        } else {
            status = MICRO_EPM_ERROR;
            goto errorUnpause;
        }
    } else {
        status = MakePowerOnTestCommand(hDevice->aEpmOutBuf, sizeof(hDevice->aEpmOutBuf), uNumIterations, uNumBuses, pauAdcPopulatedMask);
    }

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm,
                               hDevice->aEpmOutBuf,
                               POWER_ON_TEST_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        goto errorUnpause;
    }

    /* Must sleep because test code takes some time to execute */
    SLEEP(1000);

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm,
                                  hDevice->aEpmInBuf,
                                  POWER_ON_TEST_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        goto errorUnpause;
    }

    // Parse response
    if (hDevice->aEpmInBuf[CMD_ADDRESS] != POWER_ON_TEST_RSP) {
        status = MICRO_EPM_COMM_ERROR;
        goto errorUnpause;
    }

    status = ParsePowerOnTestResponse(hDevice->aEpmInBuf,
                                      POWER_ON_TEST_RSP_LENGTH,
                                      &uReceivedStatusCode,
                                      auAdcFailedMask);

    if (status != MICRO_EPM_SUCCESS) {
        goto errorUnpause;
    }

    if (pauAdcFailedMask != NULL) {
        for (uBus = 0; uBus < uNumBuses; uBus++) {
            pauAdcFailedMask[uBus] = auAdcFailedMask[uBus];
        }
    }

    if (EPM_PROTOCOL_STATUS_SUCCESS != uReceivedStatusCode) {
        status = MICRO_EPM_POWER_ON_TEST_FAILED;
        goto errorUnpause;
    }

    // Unpause
    if (bWasPaused == FALSE) {
        status = MicroEpmUnpauseAdcConversions(hDevice);

        if (status != MICRO_EPM_SUCCESS) {
            return status;
        }
    }

    return MICRO_EPM_SUCCESS;

errorUnpause:
    if (bWasPaused == FALSE) {
        (void)MicroEpmUnpauseAdcConversions(hDevice);
    }

    return status;
}


MICRO_EPM_API MicroEpmErrorType MicroEpmTacCommand(MicroEpmHandle *hDevice, const uint8_t *pCommandStr,uint8_t cmd_size,uint8_t *pRsp,uint8_t *pRspSize,uint8_t RspBufSize)
{
    MicroEpmErrorType status;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    if (RspBufSize < EPM_TAC_RSP_LENGTH) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if ((hDevice->version.EpmID != MICRO_EPM_BOARD_ID_ALPACA) &&
        (hDevice->version.EpmID != MICRO_EPM_BOARD_ID_ALPACA_V2) &&
        (hDevice->version.EpmID != MICRO_EPM_BOARD_ID_ALPACA_V3) &&
        (hDevice->version.EpmID != MICRO_EPM_BOARD_ID_ALPACA_V3P1) &&
        (hDevice->version.EpmID != MICRO_EPM_BOARD_ID_MICROEPM_TAC)) {
        return MICRO_EPM_TAC_CMD_NOT_SUPPORTED;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeEpmTacCommand(hDevice->aEpmOutBuf,sizeof(hDevice->aEpmOutBuf),pCommandStr,cmd_size);

    if (status != MICRO_EPM_SUCCESS) {
      return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               (cmd_size +2));

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  BUF_SIZE);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != EPM_TAC_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseEpmTacResponse(hDevice->aEpmInBuf,EPM_TAC_RSP_LENGTH,pRsp,RspBufSize,pRspSize);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    return MICRO_EPM_SUCCESS;
}

MicroEpmErrorType MicroEpmCtiControlCommand(MicroEpmHandle *hDevice, uint8_t ControlBit)
{
    MicroEpmErrorType status;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    if (hDevice->version.uFirmwareProtocolVersion < 8) {
        return MICRO_EPM_VERSION_MISMATCH_OLD_FIRMWARE;
    }

    memset(hDevice->aEpmOutBuf, 0, sizeof(hDevice->aEpmOutBuf));
    memset(hDevice->aEpmInBuf, 0, sizeof(hDevice->aEpmInBuf));

    status = MakeCtiControlCommand(hDevice->aEpmOutBuf,sizeof(hDevice->aEpmOutBuf),ControlBit);

    if (status != MICRO_EPM_SUCCESS) {
      return status;
    }

    // Send command
    status = MicroEpmComm_Send(hDevice->hComm, hDevice->aEpmOutBuf,
                               CTI_CONTROL_CMD_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive response
    status = MicroEpmComm_Receive(hDevice->hComm, hDevice->aEpmInBuf,
                                  CTI_CONTROL_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Parse response
    if (hDevice->aEpmInBuf[0] != CTI_CONTROL_RSP) {
        return MICRO_EPM_COMM_ERROR;
    }

    status = ParseCtiControlResponse(hDevice->aEpmInBuf,CTI_CONTROL_RSP_LENGTH);

    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    return MICRO_EPM_SUCCESS;
}

MICRO_EPM_API MicroEpmErrorType MicroEpmStartRecording(
    MicroEpmHandle *hDevice,
    MicroEpmRecordingFormatType eFormat,
    const char *pszLogFolder)
{
    if (hDevice == NULL || pszLogFolder == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (eFormat != MICRO_EPM_RECORDING_FORMAT_UDAS) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    return UdasStartRecording(hDevice, pszLogFolder);
}

MICRO_EPM_API MicroEpmErrorType MicroEpmStopRecording(MicroEpmHandle *hDevice)
{
    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (hDevice->bIsConnected != TRUE) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    return UdasStopRecording(hDevice);
}

MICRO_EPM_API MicroEpmErrorType MicroEpmStartAcquisition(MicroEpmHandle *hDevice)
{
    uint32_t uChannel;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (!hDevice->bIsConnected) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    if (hDevice->bAcquiring) {
        return MICRO_EPM_ERROR_ALREADY_ACQUIRING;
    }

    for (uChannel = 0; uChannel < MICRO_EPM_MAX_NUMBER_CHANNELS; uChannel++) {
        MicroEpmChannelInfoType *pChanInfo = &hDevice->aChannelInfo[uChannel];
        memset(&pChanInfo->stats, 0, sizeof(MicroEpmChannelStatType));
    }

    hDevice->bAcquiring = TRUE;

    hDevice->hGetData = CreateThread(NULL,
                                     0,
                                     MicroEpmGetDataThread,
                                     hDevice,
                                     0,
                                     NULL);
    if (hDevice->hGetData == NULL) {
        hDevice->bAcquiring = FALSE;
        return MICRO_EPM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}

MICRO_EPM_API MicroEpmErrorType MicroEpmStopAcquisition(MicroEpmHandle *hDevice, uint8_t *pbOverflowed)
{
    DWORD exitCode;

    if (hDevice == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (!hDevice->bIsConnected) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    if (pbOverflowed != NULL) {
        *pbOverflowed = FALSE;
    }

    if (!hDevice->bAcquiring) {
        return MICRO_EPM_SUCCESS;
    }

    hDevice->bAcquiring = FALSE;
    WaitForSingleObject(hDevice->hGetData, INFINITE);

    if (!GetExitCodeThread(hDevice->hGetData, &exitCode)) {
        CloseHandle(hDevice->hGetData);
        hDevice->hGetData = NULL;
        return MICRO_EPM_ERROR;
    }

    if (exitCode == MICRO_EPM_THREAD_OVERFLOW) {
        if (pbOverflowed != NULL) {
            *pbOverflowed = TRUE;
        }
    } else if (exitCode != MICRO_EPM_THREAD_SUCCESS) {
        CloseHandle(hDevice->hGetData);
        hDevice->hGetData = NULL;
        return MICRO_EPM_ERROR;
    }

    CloseHandle(hDevice->hGetData);
    hDevice->hGetData = NULL;

    return MICRO_EPM_SUCCESS;
}

MICRO_EPM_API MicroEpmErrorType MicroEpmGetChannelStats(MicroEpmHandle *hDevice, uint32_t uChannel, MicroEpmChannelStatType *pStats)
{
    if (hDevice == NULL || pStats == NULL) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (uChannel >= MICRO_EPM_MAX_NUMBER_CHANNELS) {
        return MICRO_EPM_INVALID_PARAMETER;
    }

    if (!hDevice->bIsConnected) {
        return MICRO_EPM_NOT_CONNECTED;
    }

    if (hDevice->bAcquiring) {
        return MICRO_EPM_ERROR_ALREADY_ACQUIRING;
    }

    *pStats = hDevice->aChannelInfo[uChannel].stats;

    return MICRO_EPM_SUCCESS;
}
