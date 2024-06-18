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
    app_anpc.c

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
#include "app_anpc.h"
#include "ble_dm/ble_dm.h"
#include "app_led.h"
#include "driver/pds/include/pds.h"
#include "driver/pds/include/pds_config.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_ADV_DURATION_60S    (60*100)    //Unit: 10ms
#define APP_ADV_DURATION_30S    (30*100)    //Unit: 10ms
#define APP_ANPC_MAX_CONN_NBR   (1U)

#define APP_BLE_GAP_RANDOM_SUB_TYPE_MASK           (0xC0U)     /**< Random Address Mask: Bit 7, Bit6. Bit 7 is the Most significant bit */
#define APP_BLE_GAP_RESOLVABLE_ADDR                (0x40U)     /**< (bit7:bit6) of BLE address is 01 then it is resolvable private address */
#define APP_BLE_GAP_NON_RESOLVABLE_ADDR            (0x00U)     /**< (bit7:bit6) of BLE address is 00 then it is non-resolvable private address */
#define APP_BLE_GAP_STATIC_ADDR                    (0xC0U)     /**< (bit7:bit6) of BLE address is 11 then it is static private address */
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
APP_CtrlInfo_T      g_anpcCtrlInfo;
// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_ANPC_ConnList_T s_appAnpcConnList[APP_ANPC_MAX_CONN_NBR];
// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

static void app_RunAnpcCmdbyState(uint16_t connHandle)
{
    APP_ANPC_ConnList_T *p_conn = NULL;
    p_conn = APP_AnpcGetConnListByHandle(connHandle);
    if (p_conn == NULL)
    { 
        return;
    }
    if (p_conn->bCccdEnable == false)
    {
        APP_ANPC_EnableCccd(connHandle, true);
    }
    else
    {
        APP_ANPC_EnableCccd(connHandle, false);
    }
}

void APP_AnpcInitConnList(uint8_t connIndex)
{
    (void)memset((uint8_t *)&s_appAnpcConnList[connIndex], 0, sizeof(APP_ANPC_ConnList_T));
}

APP_ANPC_ConnList_T *APP_AnpcGetConnListByHandle(uint16_t connHandle)
{
    uint8_t i;

    for(i=0; i<APP_ANPC_MAX_CONN_NBR; i++)
    {
        if ((s_appAnpcConnList[i].bConnStatus == true) && (s_appAnpcConnList[i].connHandle == connHandle))
        {
            return &s_appAnpcConnList[i];
        }
    }
    return NULL;
}

APP_ANPC_ConnList_T *APP_AnpcGetFreeConnList(void)
{
    uint8_t i;

    for(i=0; i<APP_ANPC_MAX_CONN_NBR; i++)
    {
        if (s_appAnpcConnList[i].bConnStatus == false)
        {
            s_appAnpcConnList[i].bConnStatus = true;
            s_appAnpcConnList[i].connIndex = i;
            return &s_appAnpcConnList[i];
        }
    }
    return NULL;
}

void APP_KeyFunction(APP_KEY_MSG_T msg)
{
    uint8_t i;
    switch (msg)
    {
        case APP_KEY_MSG_SHORT_PRESS:
        {
            GPIO_PinToggle(GPIO_PIN_RB7);

            if (g_anpcCtrlInfo.state == APP_ANPC_STATE_CONN)
            {
                for(i=0; i<APP_ANPC_MAX_CONN_NBR; i++)
                {
                    if (s_appAnpcConnList[i].bConnStatus == true)
                    {
                        app_RunAnpcCmdbyState(s_appAnpcConnList[i].connHandle);
                    }
                }
            }
        }
        break;

        case APP_KEY_MSG_LONG_PRESS:
        {
            switch (g_anpcCtrlInfo.state)
            {
                case APP_ANPC_STATE_CONN:
                {
                    g_anpcCtrlInfo.bAllowNewPairing = true;
                    for(i=0; i<APP_ANPC_MAX_CONN_NBR; i++)
                    {
                        if (s_appAnpcConnList[i].bConnStatus == true)
                        {
                            BLE_GAP_Disconnect(s_appAnpcConnList[i].connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
                        }
                    }
                }
                break;
                case APP_ANPC_STATE_WITH_BOND_ADV:
                {   //Stop advertising
                    if (BLE_GAP_SetAdvEnable(false, 0) == MBA_RES_SUCCESS)
                    {   //Start for new pairing
                        //Clear filter accept list
                        APP_SetFilterAcceptList(false);
                        //Set the configuration of advertising
                        APP_ConfigAdv(APP_ADV_TYPE_ADV);
                        //Start advertising
                        APP_EnableAdv(APP_ADV_TYPE_ADV);
                    }
                    else
                    {   //For debug
                        uint16_t ret;
                        ret  = APP_LED_Stop(g_appLedHandler);
                        if (ret != APP_RES_SUCCESS)
                        {
                            //if error occurs
                        }
                    }
                }
                break;
                default:
				{}
                break;
            }
        }
        break;

        case APP_KEY_MSG_DOUBLE_CLICK:
        {
        }
        break;

        default:
        break;
    }
}

void APP_ConfigAdv(uint8_t advType)
{
    BLE_GAP_AdvParams_T             advParam;

    (void)memset(&advParam, 0, sizeof(BLE_GAP_AdvParams_T));
    advParam.intervalMin = 32;     /* Advertising Interval Min */
    advParam.intervalMax = 32;     /* Advertising Interval Max */
    //Windows/ Android/ iOS support the reconnection using ADV_IND. So using ADV_IND for pairing and reconnection.
    advParam.type = BLE_GAP_ADV_TYPE_ADV_IND;        /* Advertising Type */
    advParam.advChannelMap = BLE_GAP_ADV_CHANNEL_ALL;        /* Advertising Channel Map */
    if (advType == APP_ADV_TYPE_WITH_BOND_ADV)//Paired already
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_SCAN_CONNECT;     /* Advertising Filter Policy */
    }
    else
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_DEFAULT;     /* Advertising Filter Policy */
    }
    BLE_GAP_SetAdvParams(&advParam);
}

void APP_EnableAdv(uint8_t advType)
{
    if (advType == APP_ADV_TYPE_WITH_BOND_ADV)
    {
        g_anpcCtrlInfo.state = APP_ANPC_STATE_WITH_BOND_ADV;
        g_appLedHandler  = APP_LED_StartByMode(APP_LED_MODE_ADV_DIR);
        BLE_GAP_SetAdvEnable(true, APP_ADV_DURATION_30S);
    }
    else
    {
        g_anpcCtrlInfo.state = APP_ANPC_STATE_ADV;
        g_appLedHandler  = APP_LED_StartByMode(APP_LED_MODE_ADV);
        BLE_GAP_SetAdvEnable(true, APP_ADV_DURATION_60S);
    }
}

void APP_SetResolvingList(bool isSet)
{
    if (isSet)
    {
        uint8_t devId;
        bool    bPaired=APP_GetPairedDeviceId(&devId);
        if (bPaired == true)
        {
            uint8_t privacyModeList[1];
            uint8_t devIdList[1];
            devIdList[0] = devId;
            privacyModeList[0] = BLE_GAP_PRIVACY_MODE_DEVICE;
            //Set Resolving List
            BLE_DM_SetResolvingList(1, devIdList, privacyModeList);
        }
    }
    else
    {
        //Clear Resolving List
        BLE_DM_SetResolvingList(0, NULL, NULL);
    }
}

void APP_SetFilterAcceptList(bool isSet)
{
    if (isSet)
    {
        uint8_t devId;
        bool    bPaired=APP_GetPairedDeviceId(&devId);
        if (bPaired == true)
        {
            uint8_t devIdList[1];
            devIdList[0] = devId;
            //Set Filter Accept List
            BLE_DM_SetFilterAcceptList(1, devIdList);
        }
    }
    else
    {
        //Clear Filter Accept List
        BLE_DM_SetFilterAcceptList(0, NULL);
    }
}

void APP_InitBleConfig(void)
{
    BLE_GAP_AdvParams_T advParam;
    bool                bPaired;
    uint8_t             i;
    //Check if paired device exists, load the info of paired device from pds.
    bPaired=APP_GetPairedDeviceId(&g_anpcCtrlInfo.peerDevId);
    //Advertise Parameter
    (void)memset(&advParam, 0, sizeof(BLE_GAP_AdvParams_T));
    advParam.intervalMin = 32;
    advParam.intervalMax = 32;
    //Windows/ Android/ iOS support the reconnection using ADV_IND. So using ADV_IND for pairing and reconnection.
    advParam.type = BLE_GAP_ADV_TYPE_ADV_IND;

    advParam.advChannelMap = BLE_GAP_ADV_CHANNEL_ALL;
    if (bPaired)
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_SCAN_CONNECT;
    }
    else
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_DEFAULT;
    }
    BLE_GAP_SetAdvParams(&advParam);

    //If paired device exists, set resolving list
    if (bPaired)//Paired already
    {
        APP_SetFilterAcceptList(true);
        APP_SetResolvingList(true);
    }
    for(i = 0; i < APP_ANPC_MAX_CONN_NBR; i++)
    {
        APP_AnpcInitConnList(i);
    }
}

void APP_ANPC_EnableCccd(uint16_t connHandle, bool enable)
{
    APP_ANPC_ConnList_T *p_conn = NULL;
    uint16_t ret = MBA_RES_SUCCESS;
    p_conn = APP_AnpcGetConnListByHandle(connHandle);
    if (p_conn == NULL)
    {
        return;
    }
    if (p_conn->bDiscovered == false)
	{
		return;
	}
    p_conn->bCccdEnable = enable;

    if (p_conn->bNewAlertEnable !=  enable)
    {
        ret = BLE_ANPC_EnableNewAlertNtfy(connHandle, enable);
    }
    else if (p_conn->bUnreadAlertEnable !=  enable)
    {
        ret = BLE_ANPC_EnableUnreadAlertNtfy(connHandle, enable);
    }
    else
    {
    }
    if (ret == MBA_RES_SUCCESS)
    {
        p_conn->bEnableCccdRetry = false;
    }
    else
    {
        p_conn->bEnableCccdRetry = true;
    }
}

bool APP_GetPairedDeviceId(uint8_t *p_devId)
{
    bool ret=false;
    uint8_t devCnt=0;
    uint8_t *p_devIdList = OSAL_Malloc(BLE_DM_MAX_PAIRED_DEVICE_NUM);
    if (p_devIdList != NULL)
    {
        BLE_DM_GetPairedDeviceList(p_devIdList, &devCnt);
        if (devCnt > 0U)
        {
            *p_devId =p_devIdList[0];//should be only 1 dev id
            ret=true;
        }
        OSAL_Free(p_devIdList);
    }
    return ret;
}
