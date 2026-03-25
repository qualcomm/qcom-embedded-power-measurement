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
#include <winerror.h>

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
#define PRODUCTID   0x9302
#define OUT_EP      0x01
#define IN_EP       0x82
#define IN2_EP      0x83

#define MICRO_EPM_COMM_4_NUM_ATTEMPTS 2

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
// Constant for {DFB21FFD-031F-4E30-B858-BE5B6FD65E0E}
static const GUID MICRO_EPM_4_DEVICE_INTERFACE =
{ 0xdfb21ffd, 0x31f, 0x4e30, { 0xb8, 0x58, 0xbe, 0x5b, 0x6f, 0xd6, 0x5e, 0xe } };

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static MicroEpmCommDevCtxtType gComm_4_DevCtxt[MAX_NUMBER_MODULES];
static MicroEpmCommVtblType MicroEpmComm_Vtbl;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static MicroEpmError MicroEpmComm_4_Disconnect(MicroEpmCommCtxt *pCommCtxt);
static MicroEpmError MicroEpmComm_4_Send(
    MicroEpmCommCtxt *pCommCtxt,
    quint8* pSendBuf,
    quint32 uLength);
static MicroEpmError MicroEpmComm_4_Receive(
    MicroEpmCommCtxt *pCommCtxt,
    quint8* pReceiveBuf,
    quint32 uRequestedLength);
static MicroEpmError MicroEpmComm_4_SendReceivePacketBuffer(
    MicroEpmCommCtxt *pCommCtxt,
    quint8* pSendBuf,
    quint32 uSendLength,
    quint8* pReceiveBuf,
    quint32 uRequestedLength);

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
MicroEpmError MicroEpmComm_4_Init(MicroEpmCommGlblCtxtType *pGlblCtxt)
{
   UNREFERENCED_PARAMETER(pGlblCtxt);

   memset(gComm_4_DevCtxt, 0, sizeof(gComm_4_DevCtxt));
   MicroEpmComm_Vtbl.pfnMicroEpmComm_Send = MicroEpmComm_4_Send;
   MicroEpmComm_Vtbl.pfnMicroEpmComm_Receive = MicroEpmComm_4_Receive;
   MicroEpmComm_Vtbl.pfnMicroEpmComm_SendReceivePacketBuffer = MicroEpmComm_4_SendReceivePacketBuffer;
   MicroEpmComm_Vtbl.pfnMicroEpmComm_Disconnect = MicroEpmComm_4_Disconnect;

   return MICRO_EPM_SUCCESS;
}

MicroEpmError MicroEpmComm_4_DeInit(MicroEpmCommGlblCtxtType *pGlblCtxt)
{
   UNREFERENCED_PARAMETER(pGlblCtxt);

   memset(gComm_4_DevCtxt, 0, sizeof(gComm_4_DevCtxt));

   return MICRO_EPM_SUCCESS;
}

MicroEpmError MicroEpmComm_4_Connect(MicroEpmCommGlblCtxtType *pCommGlblCtxt)
{
    MicroEpmError status = MICRO_EPM_SUCCESS;
    quint32 uNumConnected = 0;
    BOOL bResult = TRUE;
    quint8 uCommCtxtIdx;
    MicroEpmCommCtxt *pCommCtxt;
    ULONG requiredLength;
    DWORD dwIndex;
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DevInfoData;
    SP_DEVICE_INTERFACE_DATA devInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = NULL;
    MicroEpmCommDevCtxtType *pDevCtxt;

    for (uCommCtxtIdx = 0; uCommCtxtIdx < MAX_NUMBER_MODULES; uCommCtxtIdx++) {
        pDevCtxt = &gComm_4_DevCtxt[uCommCtxtIdx];
        if (pDevCtxt->hDeviceHandle) {
            CloseHandle(pDevCtxt->hDeviceHandle);
        }

        if (pDevCtxt->hWinUsbHandle) {
            WinUsb_Free(pDevCtxt->hWinUsbHandle);
        }
    }
    memset(gComm_4_DevCtxt, 0, sizeof(gComm_4_DevCtxt));

    /* Search for info handle on specified GUID */
    hDevInfo = SetupDiGetClassDevs(&MICRO_EPM_4_DEVICE_INTERFACE,
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
                      &MICRO_EPM_4_DEVICE_INTERFACE,
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
        gComm_4_DevCtxt[uNumConnected].hDeviceHandle = CreateFile (
                    pInterfaceDetailData->DevicePath,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED,
                    NULL);

        if (gComm_4_DevCtxt[uNumConnected].hDeviceHandle == INVALID_HANDLE_VALUE) {
            status = MICRO_EPM_ERROR;
            goto exit;
        }

        /* Now we initialize the winUSB handle */
        bResult = WinUsb_Initialize(gComm_4_DevCtxt[uNumConnected].hDeviceHandle,
                                    &(gComm_4_DevCtxt[uNumConnected].hWinUsbHandle));
        if (bResult == FALSE) {
            status = MICRO_EPM_ERROR;
            goto exit;
        }

        /* And setup the pipe policies */
        ULONG timeoutVal = 1000;

        WinUsb_SetPipePolicy(gComm_4_DevCtxt[uNumConnected].hWinUsbHandle,
                             OUT_EP, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal);
        WinUsb_SetPipePolicy(gComm_4_DevCtxt[uNumConnected].hWinUsbHandle,
                             IN_EP, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal);
        WinUsb_SetPipePolicy(gComm_4_DevCtxt[uNumConnected].hWinUsbHandle,
                             IN2_EP, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeoutVal);

        /* If we've made it this far we've successfully connected to a device */
        uNumConnected++;

        /* So we can free the memory we allocated */
        LocalFree(pInterfaceDetailData);
        pInterfaceDetailData = NULL;

        /* Now we move to the next one in the list(if it exists) */
        dwIndex++;
    }

    for (uCommCtxtIdx = 0; uCommCtxtIdx < uNumConnected; uCommCtxtIdx++) {
        pCommCtxt = &pCommGlblCtxt->aCommCtxts[pCommGlblCtxt->uNumModulesConnected+uCommCtxtIdx];
        pCommCtxt->pVtbl = &MicroEpmComm_Vtbl;
        pCommCtxt->uDevCtxtIdx = uCommCtxtIdx;
        pCommCtxt->bIsConnected = TRUE;
    }

    /* We've successfully connected */
    /* Update the global number of connected devices */
    pCommGlblCtxt->uNumModulesConnected += uNumConnected;

exit:
    if (pInterfaceDetailData) {
        LocalFree(pInterfaceDetailData);
    }

    if (hDevInfo) {
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    if (status != MICRO_EPM_SUCCESS) {
        for (uCommCtxtIdx = 0; uCommCtxtIdx < MAX_NUMBER_MODULES; uCommCtxtIdx++) {
            pDevCtxt = &gComm_4_DevCtxt[uCommCtxtIdx];
            if (pDevCtxt->hDeviceHandle) {
                CloseHandle(pDevCtxt->hDeviceHandle);
            }

            if (pDevCtxt->hWinUsbHandle) {
                WinUsb_Free(pDevCtxt->hWinUsbHandle);
            }
        }
        memset(gComm_4_DevCtxt, 0, sizeof(gComm_4_DevCtxt));
    }
    return status;
}

static MicroEpmError MicroEpmComm_4_Disconnect(MicroEpmCommCtxt *pCommCtxt)
{
    MicroEpmCommDevCtxtType *pDevCtxt;

    pDevCtxt = &gComm_4_DevCtxt[pCommCtxt->uDevCtxtIdx];
    if (pDevCtxt->hDeviceHandle) {
        CloseHandle(pDevCtxt->hDeviceHandle);
        pDevCtxt->hDeviceHandle = 0;
    }

    if (pDevCtxt->hWinUsbHandle) {
        WinUsb_Free(pDevCtxt->hWinUsbHandle);
        pDevCtxt->hWinUsbHandle = 0;
    }
    pCommCtxt->bIsConnected = FALSE;

    return MICRO_EPM_SUCCESS;
}

static MicroEpmError MicroEpmComm_4_Send(
    MicroEpmCommCtxt *pCommCtxt,
    quint8* pSendBuf,
    quint32 uLength)
{
#ifdef __MICRO_EPM_COMM_USE_MALLOC__
    BOOL bResult = FALSE;
    UCHAR* szBuffer;
    ULONG cbSent = 0;
    DWORD error;
    int32_t i;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_4_DevCtxt[pCommCtxt->uDevCtxtIdx];

    szBuffer = (UCHAR*) LocalAlloc(LPTR, sizeof(UCHAR) * uLength);
    if (szBuffer == NULL) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    memcpy(szBuffer, pSendBuf, uLength);

    for (i = 0; i < MICRO_EPM_COMM_4_NUM_ATTEMPTS; i++) {
        bResult = WinUsb_WritePipe(pDevCtxt->hWinUsbHandle, OUT_EP, szBuffer,
                                   uLength, &cbSent, NULL);
        if (!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, OUT_EP);

            error = GetLastError();
            if (error == ERROR_DEVICE_NOT_CONNECTED) {
                LocalFree(szBuffer);
                return MICRO_EPM_COMM_ERROR_NOT_CONNECTED;
            }

            continue;
        } else {
            break;
        }
    }

    LocalFree(szBuffer);

    if (!bResult) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
#else
    BOOL bResult = FALSE;
    ULONG cbSent = 0;
    DWORD error;
    qint32 i;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_4_DevCtxt[pCommCtxt->uDevCtxtIdx];

    for (i = 0; i < MICRO_EPM_COMM_4_NUM_ATTEMPTS; i++) {
        bResult = WinUsb_WritePipe(pDevCtxt->hWinUsbHandle, OUT_EP, pSendBuf,
                                   uLength, &cbSent, NULL);
        if (!bResult) {
            error = GetLastError();
            if (error == ERROR_DEVICE_NOT_CONNECTED) {
                /* This can happen when sending to bootloader */
                return MICRO_EPM_COMM_ERROR_NOT_CONNECTED;
            }

            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, OUT_EP);

            error = GetLastError();
            if (error == ERROR_DEVICE_NOT_CONNECTED) {
                /* This can happen when sending to bootloader */
                return MICRO_EPM_COMM_ERROR_NOT_CONNECTED;
            }

            continue;
        } else {
            break;
        }
    }

    if (!bResult) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
#endif
}

static MicroEpmError MicroEpmComm_4_Receive(
    MicroEpmCommCtxt *pCommCtxt,
    quint8* pReceiveBuf,
    quint32 uRequestedLength)
{
#ifdef __MICRO_EPM_COMM_USE_MALLOC__
    BOOL bResult = TRUE;
    ULONG cbRead = 0;
    UCHAR* szBuffer;
    int32_t i;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_4_DevCtxt[pCommCtxt->uDevCtxtIdx];

    szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*uRequestedLength);
    if (szBuffer == NULL) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    for (i = 0; i < MICRO_EPM_COMM_4_NUM_ATTEMPTS; i++) {
        bResult = WinUsb_ReadPipe(pDevCtxt->hWinUsbHandle, IN_EP, szBuffer,
                                  uRequestedLength, &cbRead, 0);

        if (!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, IN_EP);
            continue;
        } else {
            break;
        }
    }

    if (!bResult || (cbRead != uRequestedLength)) {
        LocalFree(szBuffer);
        return MICRO_EPM_COMM_ERROR;
    }
    memcpy(pReceiveBuf, szBuffer, uRequestedLength);
    LocalFree(szBuffer);

    return MICRO_EPM_SUCCESS;
#else
    BOOL bResult = TRUE;
    ULONG cbRead = 0;
    qint32 i;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_4_DevCtxt[pCommCtxt->uDevCtxtIdx];

    for (i = 0; i < MICRO_EPM_COMM_4_NUM_ATTEMPTS; i++) {
        bResult = WinUsb_ReadPipe(pDevCtxt->hWinUsbHandle, IN_EP, pReceiveBuf,
                                  uRequestedLength, &cbRead, 0);

        if (!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, IN_EP);
            continue;
        } else {
            break;
        }
    }

    if (!bResult || (cbRead != uRequestedLength)) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
#endif
}

static MicroEpmError MicroEpmComm_4_SendReceivePacketBuffer(
    MicroEpmCommCtxt *pCommCtxt,
    quint8* pSendBuf,
    quint32 uSendLength,
    quint8* pReceiveBuf,
    quint32 uRequestedLength)
{
#ifdef __MICRO_EPM_COMM_USE_MALLOC__
    BOOL bResult = TRUE;
    ULONG cbRead = 0;
    UCHAR* szBuffer;
    int32_t i;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_4_DevCtxt[pCommCtxt->uDevCtxtIdx];

    UNREFERENCED_PARAMETER(pSendBuf);
    UNREFERENCED_PARAMETER(uSendLength);

    szBuffer = (UCHAR*)LocalAlloc(LPTR, sizeof(UCHAR)*uRequestedLength);
    if (szBuffer == NULL) {
        return MICRO_EPM_OUT_OF_MEMORY;
    }

    for (i = 0; i < MICRO_EPM_COMM_4_NUM_ATTEMPTS; i++) {
        bResult = WinUsb_ReadPipe(pDevCtxt->hWinUsbHandle, IN2_EP, szBuffer,
                                  uRequestedLength, &cbRead, 0);
        if (!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, IN2_EP);
            continue;
        } else {
            break;
        }
    }

    if (!bResult || (cbRead != uRequestedLength)) {
        LocalFree(szBuffer);
        return MICRO_EPM_COMM_ERROR;
    }

    memcpy(pReceiveBuf, szBuffer, uRequestedLength);
    LocalFree(szBuffer);

    return MICRO_EPM_SUCCESS;
#else
    BOOL bResult = TRUE;
    ULONG cbRead = 0;
    qint32 i;
    MicroEpmCommDevCtxtType *pDevCtxt = &gComm_4_DevCtxt[pCommCtxt->uDevCtxtIdx];

    UNREFERENCED_PARAMETER(pSendBuf);
    UNREFERENCED_PARAMETER(uSendLength);

    for (i = 0; i < MICRO_EPM_COMM_4_NUM_ATTEMPTS; i++) {
        bResult = WinUsb_ReadPipe(pDevCtxt->hWinUsbHandle, IN2_EP, pReceiveBuf,
                                  uRequestedLength, &cbRead, 0);

        if (!bResult) {
            WinUsb_ResetPipe(pDevCtxt->hWinUsbHandle, IN2_EP);
            continue;
        } else {
            break;
        }
    }

    if (!bResult || (cbRead != uRequestedLength)) {
        return MICRO_EPM_COMM_ERROR;
    }

    return MICRO_EPM_SUCCESS;
#endif
}
