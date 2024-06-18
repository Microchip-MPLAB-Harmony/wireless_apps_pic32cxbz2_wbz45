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
   app_pxpm.c

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
#include "app_pxpm.h"
#include "ble_dm/ble_dm.h"
#include "driver/pds/include/pds.h"
#include "driver/pds/include/pds_config.h"
#include "crypto/crypto.h"
#include <string.h>
#include "mba_error_defs.h"
#include "app_pxpm.h"
#include "app_timer.h"
#include "app_pxpm_handler.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_SCAN_DURATION_60S    (60*10)    //Unit: 100ms
#define APP_SCAN_DURATION_30S    (30*10)    //Unit: 100ms
#define APP_PXPM_MAX_CONN_NBR    BLE_GAP_MAX_LINK_NBR    /**< Maximum allowing Conncetion Numbers for MBADK. */
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

APP_CtrlInfo_T g_ctrlInfo;
// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_PXPM_ConnList_T s_appPxpmConnList[APP_PXPM_MAX_CONN_NBR]; 
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

APP_PXPM_ConnList_T *APP_PXPM_GetConnListByHandle(uint16_t connHandle)
{
    uint8_t i;
    for(i=0; i<APP_PXPM_MAX_CONN_NBR; i++)
    {
       if ((s_appPxpmConnList[i].connstatus == true) && (s_appPxpmConnList[i].connHandle == connHandle))
       { 
           return &s_appPxpmConnList[i];
       } 
    }
    return NULL;
}

APP_PXPM_ConnList_T *APP_PXPM_GetFreeConnList(void)
{
    uint8_t i;
    for(i=0; i<APP_PXPM_MAX_CONN_NBR; i++)
    {
    if (s_appPxpmConnList[i].connstatus == false)
       {
           s_appPxpmConnList[i].connIndex = i;
           s_appPxpmConnList[i].connstatus = true;
           return &s_appPxpmConnList[i];
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
            switch (g_ctrlInfo.state)
            {
                case APP_PXP_IND_STA_CONNECTED:
                {
                    g_ctrlInfo.bAllowNewPairing=true;
                    for(i=0U; i<APP_PXPM_MAX_CONN_NBR; i++)
                    {
                        if (s_appPxpmConnList[i].connstatus == true)
                        {
                            BLE_GAP_Disconnect(s_appPxpmConnList[i].connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
                        }
                    }
                }
                break;

                case APP_PXP_IND_STA_WITH_BOND_SCAN:
                {
                    uint16_t ret;
                    //Start for new pairing
                    ret  = APP_ScanEnable(APP_PXP_IND_STA_IDLE);
					if (ret != MBA_RES_SUCCESS)
					{}
                    //Clear filter accept list
                    APP_SetFilterAcceptList(false);
                    //Clear resolving list
                    ret = APP_ScanEnable(APP_PXP_IND_STA_SCAN);
					if (ret != MBA_RES_SUCCESS)
					{}
                }
                break;

                default:
                {
                }
                break;
            }
         }
         break;
 
         case APP_KEY_MSG_DOUBLE_CLICK:
         {
         }
         break;
 
         default:
         {
         }
         break;
     }
 }

void APP_RssiMonitorHandler(APP_TIMER_TmrElem_T *timerElem){  

    int8_t rssi =0 , link_budget = 0; 
    APP_PXPM_TxPower_T *monitor_handle = ((APP_PXPM_TxPower_T *)timerElem->p_tmrParam);
    if(MBA_RES_SUCCESS == BLE_GAP_GetRssi(monitor_handle->connhandle, &rssi))
    {
        link_budget = monitor_handle->tx_power - rssi ;
        APP_PXPM_RssiCheck(monitor_handle->connhandle,link_budget);
    }
}


void APP_PXPM_InitConnCharList(uint8_t connIndex)
{
    if(NULL ==memset((uint8_t *)&s_appPxpmConnList[connIndex], 0, sizeof(APP_PXPM_ConnList_T)))
    {
        return;
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
        bool bPaired=APP_GetPairedDeviceId(&devId);
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


uint16_t APP_ScanEnable(APP_PXPM_States_T scanType)
{
    uint16_t ret;
    switch (scanType)
    {
        case APP_PXP_IND_STA_WITH_BOND_SCAN:
        {
            g_ctrlInfo.state = scanType;
            ret = BLE_GAP_SetScanningEnable(true, BLE_GAP_SCAN_FD_DISABLE, BLE_GAP_SCAN_MODE_GENERAL_DISCOVERY, APP_SCAN_DURATION_30S);
        }
        break;
        case APP_PXP_IND_STA_SCAN:
        {
            g_ctrlInfo.state = scanType;
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
uint16_t APP_CreateConnection(BLE_GAP_Addr_T *peerAddr)
{
   BLE_GAP_CreateConnParams_T createConnParam;
   createConnParam.scanInterval = 0x0020;
   createConnParam.scanWindow = 0x0020;
   createConnParam.filterPolicy = BLE_GAP_INIT_FP_FAL_NOT_USED;
   if(NULL == memcpy(&createConnParam.peerAddr, peerAddr, sizeof(BLE_GAP_Addr_T)))
   {
       return MBA_RES_NO_RESOURCE;
   }
   createConnParam.connParams.intervalMin = 0x000C;
   createConnParam.connParams.intervalMax = 0x000C;
   createConnParam.connParams.latency = 0x0000;
   createConnParam.connParams.supervisionTimeout = 0x100;
   return BLE_GAP_CreateConnection(&createConnParam);
}

void APP_InitBleConfig(void)
{
    bool    bPaired;
    uint8_t i;

    bPaired=APP_GetPairedDeviceId(&g_ctrlInfo.peerDevId);
    if (bPaired)
    {
        APP_SetFilterAcceptList(true);
        APP_SetResolvingList(true);
    }
    for(i = 0U; i < APP_PXPM_MAX_CONN_NBR; i++)
    {
        APP_PXPM_InitConnCharList(i);
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
 
uint16_t APP_PXPM_SetPathLossReportingParams(BLE_GAP_PathLossReportingParams_T *params)
{

    uint16_t ret ;
    if(MBA_RES_SUCCESS == (ret = BLE_GAP_SetPathLossReportingParams(params)))
    {
        if(MBA_RES_SUCCESS == (ret = BLE_GAP_SetPathLossReportingEnable(params->connHandle,true)))
        {
        }
    }

    return ret;
}






