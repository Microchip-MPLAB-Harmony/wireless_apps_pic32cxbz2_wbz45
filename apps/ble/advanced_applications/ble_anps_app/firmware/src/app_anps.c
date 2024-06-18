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
    app_anps.c

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
#include "app_anps.h"
#include "ble_dm/ble_dm.h"
#include "app_led.h"
#include "driver/pds/include/pds.h"
#include "driver/pds/include/pds_config.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_SCAN_DURATION_60S    (60*10)    //Unit: 100ms
#define APP_SCAN_DURATION_30S    (30*10)    //Unit: 100ms
#define APP_ANPS_MAX_CONN_NBR    (1U) 
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

APP_CtrlInfo_T g_anpsCtrlInfo;
// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_ANPS_ConnList_T     s_appAnpsConnList[APP_ANPS_MAX_CONN_NBR];
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

static void app_RunAnpsCmdbyState(uint16_t connHandle)
{
    APP_ANPS_ConnList_T *p_conn = NULL;
    p_conn = APP_AnpsGetConnListByHandle(connHandle);
    if (p_conn == NULL)
    { 
        return;
    }
    switch (p_conn->ntfyInfo)
    {
        case APP_ANPS_NTFY_NEW_ALERT:
        {
            uint8_t catId = BLE_ANPS_CAT_ID_SIMPLE_ALERT;
            uint8_t num = 2;
            const uint8_t txtStr[12] = "Mark.Smith";
            uint16_t txtStrLen = sizeof(txtStr);

            BLE_ANPS_SendNewAlert(connHandle, catId, num, txtStrLen, txtStr);
            p_conn->ntfyInfo = APP_ANPS_NTFY_UNREAD_ALERT;
        }
        break;
        case APP_ANPS_NTFY_UNREAD_ALERT:
        {
            uint8_t catId = BLE_ANPS_CAT_ID_SIMPLE_ALERT;
            uint8_t unreadCnt = 3;

            BLE_ANPS_SendUnreadAlertStat(connHandle, catId, unreadCnt);
             p_conn->ntfyInfo = APP_ANPS_NTFY_NEW_ALERT;
        }
        break;
        default:
        {
        }
        break;
    }
}

void APP_AnpsInitConnList(uint8_t connIndex)
{
    (void)memset((uint8_t *)&s_appAnpsConnList[connIndex], 0, sizeof(APP_ANPS_ConnList_T));
}

APP_ANPS_ConnList_T *APP_AnpsGetConnListByHandle(uint16_t connHandle)
{
    uint8_t i;

    for(i=0U; i<APP_ANPS_MAX_CONN_NBR; i++)
    {
        if ((s_appAnpsConnList[i].bConnStatus == true) && (s_appAnpsConnList[i].connHandle == connHandle))
        {
            return &s_appAnpsConnList[i];
        }
    }
    return NULL;
}

APP_ANPS_ConnList_T *APP_AnpsGetFreeConnList(void)
{
    uint8_t i;

    for(i=0U; i<APP_ANPS_MAX_CONN_NBR; i++)
    {
        if (s_appAnpsConnList[i].bConnStatus == false)
        {
            s_appAnpsConnList[i].bConnStatus = true;
            s_appAnpsConnList[i].connIndex = i;
            return &s_appAnpsConnList[i];
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

            if (g_anpsCtrlInfo.state == APP_ANPS_STATE_CONNECTED)
            {
                for(i=0U; i<APP_ANPS_MAX_CONN_NBR; i++)
                {
                    if (s_appAnpsConnList[i].bConnStatus == true)
                    {
                        app_RunAnpsCmdbyState(s_appAnpsConnList[i].connHandle);
                    }
                }
            }
        }
        break;

        case APP_KEY_MSG_LONG_PRESS:
        {
            switch (g_anpsCtrlInfo.state)
            {
                case APP_ANPS_STATE_CONNECTED:
                {
                    g_anpsCtrlInfo.bAllowNewPairing=true;
                    for(i=0U; i<APP_ANPS_MAX_CONN_NBR; i++)
                    {
                        if (s_appAnpsConnList[i].bConnStatus == true)
                        {
                            BLE_GAP_Disconnect(s_appAnpsConnList[i].connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
                        }
                    }
                }
                break;

                case APP_ANPS_STATE_WITH_BOND_SCAN:
                {
                    uint16_t ret;
                    //Start for new pairing
                    ret  = APP_ScanEnable(APP_ANPS_STATE_IDLE);
					if (ret != MBA_RES_SUCCESS)
					{}
                    //Clear filter accept list
                    APP_SetFilterAcceptList(false);
                    //Clear resolving list
                    ret = APP_ScanEnable(APP_ANPS_STATE_SCAN);
					if (ret != MBA_RES_SUCCESS)
					{}
                }
                break;

                default:
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

void APP_SetResolvingList(bool isSet)
{
    if (isSet)
    {
        uint8_t devId;
        bool bPaired=APP_GetPairedDeviceId(&devId);
        if (bPaired == true)
        {
            uint8_t devIdList[1];
            uint8_t privacyModeList[1];
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
        bool bPaired=APP_GetPairedDeviceId(&devId);;
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

uint16_t APP_ScanEnable(APP_ANPS_States_T scanType)
{
    uint16_t ret;
    switch (scanType)
    {
        case APP_ANPS_STATE_WITH_BOND_SCAN:
        {
            g_anpsCtrlInfo.state = scanType;
            g_appLedHandler  = APP_LED_StartByMode(APP_LED_MODE_WITH_BOND);
            ret = BLE_GAP_SetScanningEnable(true, BLE_GAP_SCAN_FD_DISABLE, BLE_GAP_SCAN_MODE_GENERAL_DISCOVERY, APP_SCAN_DURATION_30S);
        }
        break;
        case APP_ANPS_STATE_SCAN:
        {
            g_anpsCtrlInfo.state = scanType;
            g_appLedHandler  = APP_LED_StartByMode(APP_LED_MODE_WITHOUT_BOND);
            ret = BLE_GAP_SetScanningEnable(true, BLE_GAP_SCAN_FD_DISABLE, BLE_GAP_SCAN_MODE_GENERAL_DISCOVERY, APP_SCAN_DURATION_60S);
        }
        break;
        default:
        {
            ret = BLE_GAP_SetScanningEnable(false, BLE_GAP_SCAN_FD_DISABLE, BLE_GAP_SCAN_MODE_GENERAL_DISCOVERY, 0);
        }
        break;
    }
    return ret; 
}

uint16_t APP_CreateConnection(const BLE_GAP_Addr_T *peerAddr)
{
    BLE_GAP_CreateConnParams_T createConnParam;
    createConnParam.scanInterval                  = 0x0020;
    createConnParam.scanWindow                    = 0x0020;
    createConnParam.filterPolicy                  = BLE_GAP_INIT_FP_FAL_NOT_USED;
    (void)memcpy(&createConnParam.peerAddr, peerAddr, sizeof(BLE_GAP_Addr_T));
    createConnParam.connParams.intervalMin        = 0x000C;
    createConnParam.connParams.intervalMax        = 0x000C;
    createConnParam.connParams.latency            = 0x0000;
    createConnParam.connParams.supervisionTimeout = 0x100;
    return BLE_GAP_CreateConnection(&createConnParam);
}

void APP_InitBleConfig(void)
{
    bool    bPaired;
    uint8_t i;

    bPaired=APP_GetPairedDeviceId(&g_anpsCtrlInfo.peerDevId);
    if (bPaired)
    {
        APP_SetFilterAcceptList(true);
        APP_SetResolvingList(true);
    }
    for(i = 0U; i < APP_ANPS_MAX_CONN_NBR; i++)
    {
        APP_AnpsInitConnList(i);
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
