// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include <stdint.h>
#include <windows.h>
#include <winusb.h>
#include <Setupapi.h>
#define MICRO_EPM_COMM_EXPORTS
#include <MicroEpmComm.h>
#include "EpmLibPrivate.h"
#include "MicroEpmLibComm_priv.h"

/*----------------------------------------------------------------------------
* Type Declarations
* -------------------------------------------------------------------------*/

typedef struct _MicroEpmCommDevCtxtType {
    HANDLE hDeviceHandle;
    WINUSB_INTERFACE_HANDLE hWinUsbHandle;
} MicroEpmCommDevCtxtType;

/*----------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/
#define VENDORID    0x05C6
#define PRODUCTID   0x9301
#define IN_EP       0x81
#define IN2_EP      0x83
#define OUT_EP      0x02

#define MAX_CONNECTION_ATTEMPTS ((uint32_t) (2))

/*----------------------------------------------------------------------------
* Global Data Definitions
* -------------------------------------------------------------------------*/

// Constant for {C928B0F0-716D-4ABA-B400-4612C6954708}
static const GUID MICRO_EPM_3_DEVICE_INTERFACE =
{ 0xC928B0F0, 0x716D, 0x4ABA, { 0xB4, 0x00, 0x46, 0x12, 0xC6, 0x95, 0x47, 0x08 } };

/*----------------------------------------------------------------------------
* Static Variable Definitions
* -------------------------------------------------------------------------*/

static MicroEpmCommDevCtxtType gComm_3_DevCtxt[MAX_NUMBER_MODULES];
static MicroEpmCommVtblType MicroEpmComm_Vtbl;

/*----------------------------------------------------------------------------
* Static Function Declarations and Definitions
* -------------------------------------------------------------------------*/

static MicroEpmError MicroEpmComm_3_Disconnect(MicroEpmCommCtxt *pCommCtxt);
static MicroEpmError MicroEpmComm_3_Send(
    MicroEpmCommCtxt *pCommCtxt,
	quint8* pSendBuf,
	quint32 uLength);
static MicroEpmError MicroEpmComm_3_Receive(
    MicroEpmCommCtxt *pCommCtxt,
	quint8* pReceiveBuf,
	quint32 uRequestedLength);
static MicroEpmError MicroEpmComm_3_SendReceivePacketBuffer(
    MicroEpmCommCtxt *pCommCtxt,
	quint8* pSendBuf,
	quint32 uSendLength,
	quint8* pReceiveBuf,
	quint32 uRequestedLength);

static MicroEpmError MakeEnterSecureModeCommand(quint8 *pBuffer, quint8 length)
{
    if (length < ENTER_SECURE_MODE_CMD_LENGTH) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    pBuffer[0] = ENTER_SECURE_MODE_CMD;
    pBuffer[1] = 'H';
    pBuffer[2] = 'W';
    pBuffer[3] = '-';
    pBuffer[4] = 'E';
    pBuffer[5] = 'n';
    pBuffer[6] = 'g';
    pBuffer[7] = 'i';
    pBuffer[8] = 'n';
    pBuffer[9] = 'e';
    pBuffer[10] = 's';

    return MICRO_EPM_SUCCESS;
}

static MicroEpmError MakeMemoryWriteCommand(quint8 *pBuffer, quint8 length, quint8 size, quint32 address, quint32 uData)
{
    if (length < MEMORY_WRITE_CMD_LENGTH) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    pBuffer[0] = MEMORY_WRITE_CMD;
    pBuffer[1] = 'S';
    pBuffer[2] = 'n';
    pBuffer[3] = 'a';
    pBuffer[4] = 'p';
    pBuffer[5] = 'd';
    pBuffer[6] = 'r';
    pBuffer[7] = 'a';
    pBuffer[8] = 'g';
    pBuffer[9] = 'o';
    pBuffer[10] = 'n';
    pBuffer[11] = size;
    pBuffer[12] = (address >> 24) & 0xFF;
    pBuffer[13] = (address >> 16) & 0xFF;
    pBuffer[14] = (address >>  8) & 0xFF;
    pBuffer[15] = (address >>  0) & 0xFF;
    pBuffer[16] = (uData >> 24) & 0xFF;
    pBuffer[17] = (uData >> 16) & 0xFF;
    pBuffer[18] = (uData >>  8) & 0xFF;
    pBuffer[19] = (uData >>  0) & 0xFF;

    return MICRO_EPM_SUCCESS;
}

static MicroEpmError MakeHelloCommand(quint8 *pBuffer, quint8 length, quint8 version,
		quint8 minimumVersion, quint8 statusCode)
{
    if (length < HELLO_CMD_LENGTH) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    pBuffer[0] = HELLO_CMD;
    pBuffer[1] = version;
    pBuffer[2] = minimumVersion;
    pBuffer[3] = statusCode;

    return MICRO_EPM_SUCCESS;
}

static MicroEpmError ForceReset(MicroEpmCommCtxt *pCommCtxt)
{
    MicroEpmError status;
	quint8 aSendBuf[MAX_PACKET_SIZE];

    status = MakeEnterSecureModeCommand(aSendBuf, sizeof(aSendBuf));
    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    MicroEpmComm_Send(pCommCtxt, aSendBuf, ENTER_SECURE_MODE_CMD_LENGTH);
    status = MakeMemoryWriteCommand(aSendBuf, sizeof(aSendBuf), 8, PSOC_RESET_REGISTER_ADDRESS, 1);
    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }
    MicroEpmComm_Send(pCommCtxt, aSendBuf, MEMORY_WRITE_CMD_LENGTH);

    MicroEpmComm_Disconnect(pCommCtxt);

    return MICRO_EPM_SUCCESS;
}

static MicroEpmError TestSendReceiveHello(MicroEpmCommCtxt *pCommCtxt)
{
    MicroEpmError status;
	quint8 aSendBuf[MAX_PACKET_SIZE];

    status = MakeHelloCommand(aSendBuf, sizeof(aSendBuf), MICRO_EPM_PROTOCOL_VERSION, MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION, 0);
    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    status = MicroEpmComm_Send(pCommCtxt, aSendBuf, HELLO_CMD_LENGTH);
    if (status != MICRO_EPM_SUCCESS) {
        return status;
    }

    // Receive HELLO response
    status = MicroEpmComm_Receive(pCommCtxt, aSendBuf, HELLO_RSP_LENGTH);

    return status;
}

/*----------------------------------------------------------------------------
* Externalized Function Definitions
* -------------------------------------------------------------------------*/
MicroEpmError MicroEpmComm_3_Init(MicroEpmCommGlblCtxtType *pGlblCtxt)
{
   UNREFERENCED_PARAMETER(pGlblCtxt);

   memset(gComm_3_DevCtxt, 0, sizeof(gComm_3_DevCtxt));

   MicroEpmComm_Vtbl.pfnMicroEpmComm_Send = MicroEpmComm_3_Send;
   MicroEpmComm_Vtbl.pfnMicroEpmComm_Receive = MicroEpmComm_3_Receive;
   MicroEpmComm_Vtbl.pfnMicroEpmComm_SendReceivePacketBuffer = MicroEpmComm_3_SendReceivePacketBuffer;
   MicroEpmComm_Vtbl.pfnMicroEpmComm_Disconnect = MicroEpmComm_3_Disconnect;

   return MICRO_EPM_SUCCESS;
}

MicroEpmError MicroEpmComm_3_DeInit(MicroEpmCommGlblCtxtType *pGlblCtxt)
{
   UNREFERENCED_PARAMETER(pGlblCtxt);

   memset(gComm_3_DevCtxt, 0, sizeof(gComm_3_DevCtxt));

   return MICRO_EPM_SUCCESS;
}

MicroEpmError MicroEpmComm_3_Connect(MicroEpmCommGlblCtxtType *pCommGlblCtxt)
{
    MicroEpmError status = MICRO_EPM_SUCCESS;
	quint32 uNumConnected = 0;
    BOOL bResult = TRUE;
    BOOL bHadToReset;
	quint8 uCommCtxtIdx;
    MicroEpmCommCtxt *pCommCtxt;
	quint32 uConnectionAttempts;
    MicroEpmCommDevCtxtType *pDevCtxt;

    ULONG requiredLength;
    DWORD dwIndex;
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DevInfoData;
    SP_DEVICE_INTERFACE_DATA devInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = NULL;

    for (uConnectionAttempts = 0; uConnectionAttempts < MAX_CONNECTION_ATTEMPTS; uConnectionAttempts++) {
        for(uCommCtxtIdx = 0; uCommCtxtIdx < MAX_NUMBER_MODULES; uCommCtxtIdx++) {
            pDevCtxt = &gComm_3_DevCtxt[uCommCtxtIdx];
            if(pDevCtxt->hDeviceHandle) {
                CloseHandle(pDevCtxt->hDeviceHandle);
            }

            if(pDevCtxt->hWinUsbHandle) {
                WinUsb_Free(pDevCtxt->hWinUsbHandle);
            }
        }
        memset(gComm_3_DevCtxt, 0, sizeof(gComm_3_DevCtxt));

        /* Search for info handle on specified GUID */
        hDevInfo = SetupDiGetClassDevs(&MICRO_EPM_3_DEVICE_INTERFACE,
                                       NULL,
                                       NULL,
                                       DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

        if (hDevInfo == INVALID_HANDLE_VALUE) {
            return MICRO_EPM_ERROR;
        }
        DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        dwIndex = 0;
        while (SetupDiEnumDeviceInfo(hDevInfo, dwIndex, &DevInfoData) == TRUE) {

            devInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

            /* Get device interface information */
            bResult = SetupDiEnumDeviceInterfaces(
                          hDevInfo,
                          &DevInfoData,
                          &MICRO_EPM_3_DEVICE_INTERFACE,
                          0,
                          &devInterfaceData);
            if (bResult == false) {
                if (GetLastError() == ERROR_NO_MORE_ITEMS) {
                    break;
                } else {
                    status = MICRO_EPM_ERROR;
                    goto exit;
                }
            }

            if ((uNumConnected + pCommGlblCtxt->uNumModulesConnected) >= MAX_NUMBER_MODULES) {
                status = MICRO_EPM_TOO_MANY_DEVICES;
                goto exit;
            }

            /* Two step process with this function. First we call it with
            * arguments NULL to determine how much space we need to allocate
            */
            bResult = SetupDiGetDeviceInterfaceDetail(
                          hDevInfo,
                          &devInterfaceData,
                          NULL, 0,
                          &requiredLength,
                          NULL);

            /* The first call to this function should return false */
            if (bResult == TRUE) {
                status = MICRO_EPM_ERROR;
                goto exit;
            }
            /* and the error code should be INSUF_BUFFER */
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
                status = MICRO_EPM_ERROR;
                goto exit;
            }

            /* But it fills required length with the buffer size we need */
            /* and we use it to allocate the appropriately sized buffer */
            pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) LocalAlloc(LPTR, requiredLength);
            if (pInterfaceDetailData == NULL) {
                status = MICRO_EPM_OUT_OF_MEMORY;
                goto exit;
            }

            pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            /* Now we can call it and get the data we need */
            bResult = SetupDiGetDeviceInterfaceDetail(
                          hDevInfo,
                          &devInterfaceData,
                          pInterfaceDetailData,
                          requiredLength,
                          NULL,
                          &DevInfoData);
            if (bResult == FALSE) {
                status = MICRO_EPM_ERROR;
                goto exit;
            }

            /* We use the path to open a file to the device */
            gComm_3_DevCtxt[uNumConnected].hDeviceHandle = CreateFile (
                        pInterfaceDetailData->DevicePath,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        NULL);

            if (gComm_3_DevCtxt[uNumConnected].hDeviceHandle == INVALID_HANDLE_VALUE) {
                status = MICRO_EPM_ERROR;
                goto exit;
            }

            /* Now we intiailize the winUSB handle */
            bResult = WinUsb_Initialize(gComm_3_DevCtxt[uNumConnected].hDeviceHandle,
                                        &(gComm_3_DevCtxt[uNumConnected].hWinUsbHandle));
            if (bResult == FALSE) {
                status = MICRO_EPM_ERROR;
                goto exit;
            }

            /* And setup the pipe policies */
            ULONG timeoutVal = 1000;

            WinUsb_SetPipePolicy(gComm_3_DevCtxt[uNumConnected].hWinUsbHandle,
                                 OUT_EP, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal);
            WinUsb_SetPipePolicy(gComm_3_DevCtxt[uNumConnected].hWinUsbHandle,
                                 IN_EP, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal);
            WinUsb_SetPipePolicy(gComm_3_DevCtxt[uNumConnected].hWinUsbHandle,
                                 IN2_EP, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal);

            /* If we've made it this far we've successfully connected to a device */
            uNumConnected++;

            /* So we can free the memory we allocated */
            LocalFree(pInterfaceDetailData);
            pInterfaceDetailData = NULL;

            /* Now we move to the next one in the list(if it exists) */
            dwIndex++;
        }

        bHadToReset = FALSE;
        for (uCommCtxtIdx = 0; uCommCtxtIdx < uNumConnected; uCommCtxtIdx++) {
            pCommCtxt = &pCommGlblCtxt->aCommCtxts[pCommGlblCtxt->uNumModulesConnected+uCommCtxtIdx];
            pCommCtxt->pVtbl = &MicroEpmComm_Vtbl;
            pCommCtxt->uDevCtxtIdx = uCommCtxtIdx;
            pCommCtxt->bIsConnected = TRUE;
            status = TestSendReceiveHello(pCommCtxt);

            /* If we fail comm then we force device reset */
            if (status == MICRO_EPM_COMM_ERROR) {
                ForceReset(pCommCtxt);
                bHadToReset = TRUE;
            }
        }

        /* If we had to reset one or more devices then we need to reconnect */
        if (bHadToReset == TRUE) {
            /* Release device list resources */
            SetupDiDestroyDeviceInfoList(hDevInfo);

            for (uCommCtxtIdx = 0; uCommCtxtIdx < uNumConnected; uCommCtxtIdx++) {
                pCommCtxt = &pCommGlblCtxt->aCommCtxts[pCommGlblCtxt->uNumModulesConnected+uCommCtxtIdx];
                MicroEpmComm_Disconnect(pCommCtxt);
            }

            /* Delay while we wait for devices to reset / enumerate */
            SLEEP(8000);

            /* Restart this process */
            uNumConnected = 0;
            continue;
        } else {
            /* Otherwise we've successfully connected */
            /* Update the global number of connected devices */
            pCommGlblCtxt->uNumModulesConnected += uNumConnected;
            break;
        }
    }

exit:
    if(pInterfaceDetailData) {
        LocalFree(pInterfaceDetailData);
    }

    if(hDevInfo) {
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    if(status != MICRO_EPM_SUCCESS) {
        for(uCommCtxtIdx = 0; uCommCtxtIdx < MAX_NUMBER_MODULES; uCommCtxtIdx++) {
            pDevCtxt = &gComm_3_DevCtxt[uCommCtxtIdx];
            if(pDevCtxt->hDeviceHandle) {
                CloseHandle(pDevCtxt->hDeviceHandle);
            }

            if(pDevCtxt->hWinUsbHandle) {
                WinUsb_Free(pDevCtxt->hWinUsbHandle);
            }
        }
        memset(gComm_3_DevCtxt, 0, sizeof(gComm_3_DevCtxt));
    }
    return status;
}


static MicroEpmError MicroEpmComm_3_Disconnect(MicroEpmCommCtxt *pCommCtxt)
{
    MicroEpmCommDevCtxtType *pDevCtxt;

    pDevCtxt = &gComm_3_DevCtxt[pCommCtxt->uDevCtxtIdx];
    if(pDevCtxt->hDeviceHandle) {
        CloseHandle(pDevCtxt->hDeviceHandle);
        pDevCtxt->hDeviceHandle = 0;
    }

    if(pDevCtxt->hWinUsbHandle) {
        WinUsb_Free(pDevCtxt->hWinUsbHandle);
        pDevCtxt->hWinUsbHandle = 0;
    }
    pCommCtxt->bIsConnected = FALSE;

    return MICRO_EPM_SUCCESS;
}


static MicroEpmError MicroEpmComm_3_Send(
    MicroEpmCommCtxt *pCommCtxt,
	quint8* pSendBuf,
	quint32 uLength)
{
    qint32 i;
    BOOL bResult = FALSE;
    UCHAR* szBuffer;
    ULONG cbSent = 0;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_3_DevCtxt[pCommCtxt->uDevCtxtIdx];

    szBuffer = (UCHAR*) LocalAlloc(LPTR, sizeof(UCHAR) * uLength);
    if (szBuffer == NULL) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    memcpy(szBuffer, pSendBuf, uLength);

    for (i = 0; i < 2; i++) {
        bResult = WinUsb_WritePipe(pDevCtxt->hWinUsbHandle, OUT_EP, szBuffer,
                                   uLength, &cbSent, NULL);

        if(!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, OUT_EP);
        } else {
            break;
        }
    }

    LocalFree(szBuffer);

    if (!bResult) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}


static MicroEpmError MicroEpmComm_3_Receive(
    MicroEpmCommCtxt *pCommCtxt,
	quint8* pReceiveBuf,
	quint32 uRequestedLength)
{
    BOOL bResult;
    UCHAR* szBuffer;
    ULONG readLength;
    qint32 i;
    MicroEpmError status = MICRO_EPM_SUCCESS;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_3_DevCtxt[pCommCtxt->uDevCtxtIdx];

    szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*uRequestedLength);
    if (szBuffer == NULL) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    for (i = 0; i < 2; i++) {
        bResult = WinUsb_ReadPipe(pDevCtxt->hWinUsbHandle, IN_EP, szBuffer,
                                  uRequestedLength, &readLength, 0);
        if(!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, IN_EP);
            continue;
        }

        if (readLength != uRequestedLength) {
            status = MICRO_EPM_COMM_ERROR;
            goto exit;
        }
        memcpy(pReceiveBuf, szBuffer, uRequestedLength);
        break;
    }

    if (!bResult) {
        status = MICRO_EPM_COMM_ERROR;
    }

exit:
    LocalFree(szBuffer);
    return status;
}

static MicroEpmError MicroEpmComm_3_SendReceivePacketBuffer(
    MicroEpmCommCtxt *pCommCtxt,
	quint8* pSendBuf,
	quint32 uSendLength,
	quint8* pReceiveBuf,
	quint32 uRequestedLength)
{
    BOOL bResult = TRUE;
    ULONG cbRead = 0;
    UCHAR* szBuffer;
    qint32 i;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_3_DevCtxt[pCommCtxt->uDevCtxtIdx];

    UNREFERENCED_PARAMETER(pSendBuf);
    UNREFERENCED_PARAMETER(uSendLength);

    szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*uRequestedLength);
    if (szBuffer == NULL) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    for (i = 0; i < 2; i++) {
        bResult = WinUsb_ReadPipe(pDevCtxt->hWinUsbHandle, IN2_EP, szBuffer,
                                  uRequestedLength, &cbRead, 0);
        if(!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, IN2_EP);
            //printf("WinUsb_ReadPipe Error %d.", GetLastError());
            continue;
        }

        if (cbRead != uRequestedLength) {
            LocalFree(szBuffer);
            return MICRO_EPM_COMM_ERROR;
        }
        memcpy(pReceiveBuf, szBuffer, uRequestedLength);
        break;
    }

    LocalFree(szBuffer);

    if (!bResult) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
}
