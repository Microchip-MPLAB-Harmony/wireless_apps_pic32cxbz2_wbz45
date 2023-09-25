/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/


/*******************************************************************************
  Application BLE Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_module.c

  Summary:
    This file contains the Application implementation for this project.

  Description:
    This file contains the Application implementation for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_module.h"
#include "ble_dm/ble_dm.h"
#include "app_led.h"
#include "driver/pds/include/pds.h"
#include "driver/pds/include/pds_config.h"
#include "crypto/crypto.h"

#ifdef WC_NO_RNG
#include "peripheral/trng/plib_trng.h"
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_ADV_DURATION_60S    (60*100)    //Unit: 10ms
#define APP_ADV_DURATION_30S    (30*100)    //Unit: 10ms
#define APP_RANDOM_BYTE_LEN 32

#define APP_BLE_GAP_RANDOM_SUB_TYPE_MASK           (0xC0U)     /**< Random Address Mask: Bit 7, Bit6. Bit 7 is the Most significant bit */
#define APP_BLE_GAP_RESOLVABLE_ADDR                (0x40U)     /**< (bit7:bit6) of BLE address is 01 then it is resolvable private address */
#define APP_BLE_GAP_NON_RESOLVABLE_ADDR            (0x00U)     /**< (bit7:bit6) of BLE address is 00 then it is non-resolvable private address */
#define APP_BLE_GAP_STATIC_ADDR                    (0xC0U)     /**< (bit7:bit6) of BLE address is 11 then it is static private address */
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
typedef enum APP_PdsAppItem_T{
    PDS_APP_ITEM_ID_1 = (PDS_MODULE_APP_OFFSET),
    PDS_APP_ITEM_ID_2

}APP_PdsAppItem_T;
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

APP_PairedDevInfo_T g_pairedDevInfo;
APP_ExtPairedDevInfo_T g_extPairedDevInfo;

bool g_bAllowNewPairing;
APP_NotificationInfo_T g_NtfyInfo;
APP_GattClientInfo_T g_GattClientInfo;
// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_ExtPairedDevInfo_T s_extPairedDevInfoBuf;

PDS_DECLARE_FILE(PDS_APP_ITEM_ID_1, sizeof(APP_ExtPairedDevInfo_T), &s_extPairedDevInfoBuf,FILE_INTEGRITY_CONTROL_MARK);
// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
static void app_RunAncsCmdbyState(void);

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
void APP_KeyFunction(APP_KEY_MSG_T msg)
{
    switch (msg)
    {
        case APP_KEY_MSG_SHORT_PRESS:
        {
            GPIO_PinToggle(GPIO_PIN_RB7);

            if (g_pairedDevInfo.state == APP_ANCS_STATE_CONN)
            {
                app_RunAncsCmdbyState();
            }
        }
        break;

        case APP_KEY_MSG_LONG_PRESS:
        {
            switch (g_pairedDevInfo.state)
            {
                case APP_ANCS_STATE_CONN:
                {
                    g_bAllowNewPairing=true;
                    BLE_GAP_Disconnect(g_pairedDevInfo.connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
                }
                break;

                case APP_ANCS_STATE_ADV_DIR:
                {
                    //Stop advertising
                    if (BLE_GAP_SetAdvEnable(false, 0) == MBA_RES_SUCCESS)
                    {
                        //Start for new pairing
                        //Clear paired device info
                        APP_InitPairedDeviceInfo();
                        //Set a new IRK
                        APP_SetLocalIRK();
                        //Set a new local address-Random Static Address
                        APP_GenerateRandomStaticAddress(&g_extPairedDevInfo.localAddr);
                        BLE_GAP_SetDeviceAddr(&g_extPairedDevInfo.localAddr);
                        //Clear filter accept list
                        APP_SetFilterAcceptList(false);
                        //Clear resolving list
                        APP_SetResolvingList(false);
                        //Set the configuration of advertising
                        APP_ConfigAdvDirect(false);
                        //Start advertising
                        APP_EnableAdvDirect(false);
                    }
                    else
                    {   //For debug
                        APP_LED_Stop(g_appLedHandler);
                    }
                }
                break;

                default:
                break;
            }
        }
        break;

        case APP_KEY_MSG_DOUBLE_CLICK:
        {
            if (g_pairedDevInfo.state == APP_ANCS_STATE_CONN)
            {
                if (g_NtfyInfo.state == APP_ANCS_IND_APP_ATTR)
                {
                    BLE_ANCS_PerformNtfyAction(g_pairedDevInfo.connHandle, g_NtfyInfo.ntfyId, BLE_ANCS_ACTION_ID_NEGATIVE);
                }
            }
        }
        break;

        default:
        break;
    }
}

void APP_GenerateRandomData(uint8_t *pData, uint8_t dataLen)
{
    uint8_t       out[APP_RANDOM_BYTE_LEN];

#ifndef WC_NO_RNG
    CRYPT_RNG_CTX mcRng;
    
    CRYPT_RNG_Initialize(&mcRng);
    CRYPT_RNG_Get(&mcRng, &out[0]);
    CRYPT_RNG_BlockGenerate(&mcRng, out, APP_RANDOM_BYTE_LEN);
    CRYPT_RNG_Deinitialize(&mcRng);
#else
    uint32_t random_part;
    for(uint8_t i = 0; i < sizeof(out); i +=sizeof(random_part)){
        random_part = TRNG_ReadData();
        memcpy(&out[i], &random_part, (sizeof(out) - i)>=sizeof(random_part)?sizeof(random_part):(sizeof(out) - i) );
    }
#endif    
    if (dataLen <= APP_RANDOM_BYTE_LEN)
    {
        memcpy(pData, out, dataLen);
    }
    else
    {
        memcpy(pData, out, APP_RANDOM_BYTE_LEN);
    }
}

void APP_GenerateRandomStaticAddress(BLE_GAP_Addr_T *pAddr)
{
    //Get a random address and Configure it to Random Static Address
    APP_GenerateRandomData(pAddr->addr, GAP_MAX_BD_ADDRESS_LEN);
    pAddr->addr[GAP_MAX_BD_ADDRESS_LEN-1] &= ~APP_BLE_GAP_RANDOM_SUB_TYPE_MASK;
    pAddr->addr[GAP_MAX_BD_ADDRESS_LEN-1] |= APP_BLE_GAP_STATIC_ADDR;
    pAddr->addrType = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
}

void APP_SetLocalIRK(void)
{
    BLE_GAP_LocalPrivacyParams_T localPrivacyParams;

    //Set a new IRK
    memset(&localPrivacyParams, 0x00, sizeof(BLE_GAP_LocalPrivacyParams_T));
    localPrivacyParams.addrTimeout = BLE_GAP_RPA_TIMEOUT_MAX;
    APP_GenerateRandomData(localPrivacyParams.localIrk, 16);
    BLE_GAP_SetLocalPrivacy(false, &localPrivacyParams);
}

void APP_ConfigAdvDirect(bool bDirect)
{
    BLE_GAP_AdvParams_T             advParam;

    memset(&advParam, 0, sizeof(BLE_GAP_AdvParams_T));
    advParam.intervalMin = 32;     /* Advertising Interval Min */
    advParam.intervalMax = 32;     /* Advertising Interval Max */
    //Windows/ Android/ iOS support the reconnection using ADV_IND. So using ADV_IND for pairing and reconnection.
    advParam.type = BLE_GAP_ADV_TYPE_ADV_IND;        /* Advertising Type */
    advParam.advChannelMap = BLE_GAP_ADV_CHANNEL_ALL;        /* Advertising Channel Map */
    if (bDirect)//Paired already
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_SCAN_CONNECT;     /* Advertising Filter Policy */
    }
    else
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_DEFAULT;     /* Advertising Filter Policy */
    }
    BLE_GAP_SetAdvParams(&advParam);
}

void APP_EnableAdvDirect(bool bDirect)
{
    if (bDirect)
    {
        g_pairedDevInfo.state=APP_ANCS_STATE_ADV_DIR;
        g_appLedHandler=APP_LED_StartByMode(APP_LED_MODE_ADV_DIR);
        BLE_GAP_SetAdvEnable(true, APP_ADV_DURATION_30S);
    }
    else
    {
        g_pairedDevInfo.state=APP_ANCS_STATE_ADV;
        g_appLedHandler=APP_LED_StartByMode(APP_LED_MODE_ADV);
        BLE_GAP_SetAdvEnable(true, APP_ADV_DURATION_60S);
    }
}

uint16_t APP_GetExtPairedDevInfoFromFlash(APP_ExtPairedDevInfo_T *pExtInfo)
{
    if (PDS_IsAbleToRestore(PDS_APP_ITEM_ID_1) == false)
        return MBA_RES_INVALID_PARA;

    if (PDS_Restore(PDS_APP_ITEM_ID_1))
    {
        memcpy(pExtInfo, &s_extPairedDevInfoBuf, sizeof(APP_ExtPairedDevInfo_T));
        return MBA_RES_SUCCESS;
    }
    else
    {
        return MBA_RES_FAIL;
    }
}

uint16_t APP_SetExtPairedDevInfoInFlash(APP_ExtPairedDevInfo_T *pExtInfo)
{
    memcpy(&s_extPairedDevInfoBuf, pExtInfo, sizeof(APP_ExtPairedDevInfo_T));

    if (PDS_Store(PDS_APP_ITEM_ID_1))
    {
        return MBA_RES_SUCCESS;
    }
    else
    {
        return MBA_RES_FAIL;
    }
}

bool APP_GetPairedDeviceId(uint8_t *pDevId)
{
    bool ret=false;
    uint8_t devIdList[BLE_DM_MAX_PAIRED_DEVICE_NUM];
    uint8_t devCnt=0;

    BLE_DM_GetPairedDeviceList(devIdList, &devCnt);
    if (devCnt > 0)
    {
        *pDevId =devIdList[0];//should be only 1 dev id
        ret=true;
    }

    return ret;
}

bool APP_GetPairedDeviceAddr(BLE_GAP_Addr_T *pAddr)
{
    bool ret=false;
    BLE_DM_PairedDevInfo_T  *p_devInfo;

    p_devInfo = OSAL_Malloc(sizeof(BLE_DM_PairedDevInfo_T));
    if (p_devInfo)
    {
        if (BLE_DM_GetPairedDevice(g_pairedDevInfo.peerDevId, p_devInfo) ==MBA_RES_SUCCESS)
        {
            memcpy(pAddr, &p_devInfo->remoteAddr, sizeof(BLE_GAP_Addr_T));
            ret=true;
        }
        OSAL_Free(p_devInfo);
    }

    return ret;
}

void APP_SetResolvingList(bool bSet)
{
    uint8_t devIdList[]={g_pairedDevInfo.peerDevId};
    uint8_t privacyModeList[]={BLE_GAP_PRIVACY_MODE_DEVICE};

    if (bSet)
    {
        //Set Resolving List
        BLE_DM_SetResolvingList(1, devIdList, privacyModeList);
    }
    else
    {
        //Clear Resolving List
        BLE_DM_SetResolvingList(0, NULL, NULL);
    }
}

void APP_SetFilterAcceptList(bool bSet)
{
    uint8_t devIdList[]={g_pairedDevInfo.peerDevId};

    if (bSet)
    {
        //Set Filter Accept List
        BLE_DM_SetFilterAcceptList(1, devIdList);
    }
    else
    {
        //Clear Filter Accept List
        BLE_DM_SetFilterAcceptList(0, NULL);
    }
}

void APP_InitPairedDeviceInfo(void)
{
    g_pairedDevInfo.bPaired=false;
    g_pairedDevInfo.bAddrLoaded=false;

    memset(&g_NtfyInfo, 0x00, sizeof(APP_NotificationInfo_T));
    memset(&g_GattClientInfo, 0x00, sizeof(APP_GattClientInfo_T));
}

static void app_PdsWriteCompleteCb(PDS_MemId_t memoryId)
{
    switch (memoryId)
    {
        case PDS_APP_ITEM_ID_1://PDS_MODULE_APP_OFFSET
        {
            //finish writing
        }
        break;

        case PDS_APP_ITEM_ID_2:
        {
            //finish writing
        }
        break;

        default:
        break;
    }
}

void APP_RegisterPdsCb(void)
{
    PDS_RegisterWriteCompleteCallback(app_PdsWriteCompleteCb);
}

static void app_RunAncsCmdbyState(void)
{
    BLE_ANCS_NtfyAttrsMask_T ntfyAttMask;
    BLE_ANCS_AppAttrsMask_T appAttrMask;

    switch (g_NtfyInfo.state)
    {
        case APP_ANCS_IND_NTFY_ADDED:
        {
            ntfyAttMask.appId=1;
            ntfyAttMask.title=1;
            ntfyAttMask.subtitle=1;
            ntfyAttMask.msg=1;
            ntfyAttMask.msgSize=1;
            ntfyAttMask.date=1;
            ntfyAttMask.positiveAction=1;
            ntfyAttMask.negativeAction=1;
            BLE_ANCS_GetNtfyAttr(g_pairedDevInfo.connHandle, g_NtfyInfo.ntfyId, ntfyAttMask);
        }
        break;

        case APP_ANCS_IND_NTFY_ATTR:
        {
            appAttrMask.displayName=1;
            BLE_ANCS_GetAppAttr(g_pairedDevInfo.connHandle, g_NtfyInfo.appId, appAttrMask);
        }
        break;

        case APP_ANCS_IND_APP_ATTR:
        {
            BLE_ANCS_PerformNtfyAction(g_pairedDevInfo.connHandle, g_NtfyInfo.ntfyId, BLE_ANCS_ACTION_ID_POSITIVE);
        }
        break;

        default:
        break;
    }
}

