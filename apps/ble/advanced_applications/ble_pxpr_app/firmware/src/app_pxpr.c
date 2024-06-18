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
    app_pxpr.c

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
#include "ble/profile_ble/ble_pxpr/ble_pxpr.h"
#include "ble_dm/ble_dm.h"
#include "driver/pds/include/pds.h"
#include "driver/pds/include/pds_config.h"
#include "crypto/crypto.h"
#include "app_ble.h"
#include "app_pxpr_handler.h"
#include "app_pxpr.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_ADV_DURATION_60S    (60*100)    //Unit: 10ms
#define APP_ADV_DURATION_30S    (30*100)    //Unit: 10ms
#define APP_PXPR_MAX_CONN_NBR       (0x01U)    /**< Maximum allowing Conncetion Numbers for MBADK. */
#define APP_RANDOM_BYTE_LEN 32

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
APP_CtrlInfo_T g_ctrlInfo ;
// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

static struct APP_PXPR_ConnList_T  s_appPxprConnList[APP_PXPR_MAX_CONN_NBR]; 
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

void APP_PXPR_InitConnCharList(uint8_t connIndex)
{
    (void)memset(&s_appPxprConnList[connIndex], 0, sizeof(APP_PXPR_ConnList_T));
}

APP_PXPR_ConnList_T *APP_PXPR_GetConnListByHandle(uint16_t connHandle)
{
    uint8_t i;

    for(i=0; i<APP_PXPR_MAX_CONN_NBR; i++) 
    {
        if ((s_appPxprConnList[i].connStatus == true) && (s_appPxprConnList[i].connHandle == connHandle))
        {
            return &s_appPxprConnList[i]; 
        }
    }
    return NULL;
}


APP_PXPR_ConnList_T *APP_PXPR_GetFreeConnList(void)
{
    uint8_t i;

    for(i=0; i<APP_PXPR_MAX_CONN_NBR; i++)
    {
        if (s_appPxprConnList[i].connStatus == false)
        {
            s_appPxprConnList[i].connIndex = i;
            s_appPxprConnList[i].connStatus = true;
            return &s_appPxprConnList[i];
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
        }
        break;

        case APP_KEY_MSG_LONG_PRESS:
        {
            switch(g_ctrlInfo.state)
            {
                case APP_PXPR_STATE_CONN:
                {
                    g_ctrlInfo.bAllowNewPairing = true;
                    for(i=0; i<APP_PXPR_MAX_CONN_NBR; i++)
                    {
                         if (s_appPxprConnList[i].connStatus == true)
                         {
                             BLE_GAP_Disconnect(s_appPxprConnList[i].connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
                         }
                    }
                }
                break;
                case APP_PXPR_STATE_WITH_BOND_ADV:
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
        g_ctrlInfo.state=APP_PXPR_STATE_WITH_BOND_ADV;
        g_appLedHandler=APP_LED_StartByMode(APP_LED_MODE_ADV_DIR);
        BLE_GAP_SetAdvEnable(true, APP_ADV_DURATION_30S);
    }
    else
    {
        g_ctrlInfo.state=APP_PXPR_STATE_ADV;
        g_appLedHandler=APP_LED_StartByMode(APP_LED_MODE_ADV);
        BLE_GAP_SetAdvEnable(true, APP_ADV_DURATION_60S);
    }
}


bool APP_GetPairedDeviceId(uint8_t *p_devId)
{
    bool ret=false;
    uint8_t devIdList[8];
    uint8_t devCnt=0;

    BLE_DM_GetPairedDeviceList(devIdList, &devCnt);
    if (devCnt > 0U)
    {
        *p_devId =devIdList[0];//should be only 1 dev id
        ret=true;
    }

    return ret;
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
    bPaired=APP_GetPairedDeviceId(&g_ctrlInfo.peerDevId);
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
    for(i = 0; i < APP_PXPR_MAX_CONN_NBR; i++)
    {
        APP_PXPR_InitConnCharList(i);
    }
}


