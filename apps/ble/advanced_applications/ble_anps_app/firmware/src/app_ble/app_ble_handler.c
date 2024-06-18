/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
    app_ble_handler.c

  Summary:
    This file contains the Application BLE functions for this project.

  Description:
    This file contains the Application BLE functions for this project.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdint.h>
#include "osal/osal_freertos_extend.h"
#include "app_ble_handler.h"
#include "ble_dm/ble_dm_dds.h"
#include "app_anps.h"
#include "app_led.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
#define BLE_ANS_UUID                   (0x1811U)
#define COMPLETE_LIST_OF_16_BIT_UUIDS  (0x03U)
void APP_BleGapEvtHandler(const BLE_GAP_Event_T *p_event)
{
    APP_ANPS_ConnList_T *p_conn = NULL;
    switch(p_event->eventId)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ENCRYPT_STATUS:
        {
            /* TODO: implement your application code.*/
            if (p_event->eventField.evtEncryptStatus.status == GAP_STATUS_PIN_KEY_MISSING)
            {
                BLE_DM_ProceedSecurity(p_event->eventField.evtEncryptStatus.connHandle, 1);
				p_conn = APP_AnpsGetConnListByHandle(p_event->eventField.evtEncryptStatus.connHandle);
				if (p_conn == NULL)
				{ 
					return;
				}
				p_conn->bSecurityStatus = true;
            }
        }
        break;

        case BLE_GAP_EVT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
            const uint8_t  *p_data = p_event->eventField.evtAdvReport.advData;
            uint8_t  adTypeLen = 0U;
            uint16_t uuid16, i, ret;
            uint16_t strLen = p_event->eventField.evtAdvReport.length;

            if (g_anpsCtrlInfo.state == APP_ANPS_STATE_CONNECTING)
            {
                break;
            }
            for (i=0U; i<strLen; i=i+adTypeLen+1U)
            {
                uint8_t adType;
                adTypeLen = *(p_data+i);
                adType    = *(p_data+i+1U);
                if (adType == COMPLETE_LIST_OF_16_BIT_UUIDS)
                {
					uuid16  = (*(p_data+i+2U)) + (*(p_data+i+3U)<<8);
                    if (uuid16 == BLE_ANS_UUID)
                    {
                        BLE_GAP_Addr_T remoteAddr;
                        uint8_t devCnt;
                        if (g_anpsCtrlInfo.state == APP_ANPS_STATE_WITH_BOND_SCAN)
                        {
                            BLE_DM_GetFilterAcceptList(&devCnt, &remoteAddr);
                            if (memcmp(&remoteAddr, &p_event->eventField.evtAdvReport.addr, sizeof(BLE_GAP_Addr_T)) != 0)
                            {
                                break;
                            }
                        }
                        ret = APP_CreateConnection(&p_event->eventField.evtAdvReport.addr);
                        if (ret == MBA_RES_SUCCESS)
                        {
                            g_anpsCtrlInfo.state = APP_ANPS_STATE_CONNECTING;
                            APP_ScanEnable(APP_ANPS_STATE_CONNECTING);
                        }
                        break;
                    }
                }
            }
        }
        break;

        case BLE_GAP_EVT_ENC_INFO_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_REMOTE_CONN_PARAM_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_EXT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_TX_BUF_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_DEVICE_NAME_CHANGED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_AUTH_PAYLOAD_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PHY_UPDATE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_SCAN_REQ_RECEIVED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_DIRECT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_SYNC_EST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_SYNC_LOST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_SET_TERMINATED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_SCAN_TIMEOUT:
        {
            /* TODO: implement your application code.*/
            g_anpsCtrlInfo.state = APP_ANPS_STATE_IDLE;
            APP_LED_Stop(g_appLedHandler);
        }
        break;

        case BLE_GAP_EVT_TRANSMIT_POWER_REPORTING:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_COMPL:
        {
            /* TODO: implement your application code.*/

        }
        break;

        case BLE_GAP_EVT_PATH_LOSS_THRESHOLD:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        {
        }
        break;
    }
}

void APP_BleL2capEvtHandler(const BLE_L2CAP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_L2CAP_EVT_CONN_PARA_UPD_REQ:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CONN_PARA_UPD_RSP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_CONN_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_CONN_FAIL_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_SDU_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_ADD_CREDITS_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_DISC_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;        

        default:
        break;
    }
}

void APP_GattEvtHandler(const GATT_Event_T *p_event)
{   
    switch(p_event->eventId)
    {
        case GATTC_EVT_ERROR_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_PRIM_SERV_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_PRIM_SERV_BY_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_CHAR_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_DESC_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_READ_USING_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_READ_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_WRITE_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_HV_NOTIFY:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_HV_INDICATE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_READ:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_WRITE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_HV_CONFIRM:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case ATT_EVT_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case ATT_EVT_UPDATE_MTU:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_CHAR_BY_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_SERVICE_CHANGE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_CLIENT_FEATURE_CHANGE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_CLIENT_CCCDLIST_CHANGE:
        {
            /* TODO: implement your application code.*/
            OSAL_Free(p_event->eventField.onClientCccdListChange.p_cccdList);
        }
        break;

        case GATTC_EVT_PROTOCOL_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_PROTOCOL_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;        
    }
}

void APP_BleSmpEvtHandler(const BLE_SMP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_SMP_EVT_PAIRING_COMPLETE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_SECURITY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_NUMERIC_COMPARISON_CONFIRM_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_PASSKEY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_DISPLAY_PASSKEY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_NOTIFY_KEYS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_PAIRING_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_OOB_DATA_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_SC_OOB_DATA_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_KEYPRESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_GEN_SC_OOB_DATA_DONE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;        
    }
}

void APP_DmEvtHandler(BLE_DM_Event_T *p_event)
{
    APP_ANPS_ConnList_T *p_conn = NULL;
    bool bPaired;
    uint8_t devId;

    switch(p_event->eventId)
    {
        case BLE_DM_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
            p_conn = APP_AnpsGetConnListByHandle(p_event->connHandle);
            if (p_conn == NULL)
            {
                return;
            }
            APP_AnpsInitConnList(p_conn->connIndex);
            g_anpsCtrlInfo.state=APP_ANPS_STATE_IDLE;
            bPaired=APP_GetPairedDeviceId(&devId);
            if (bPaired == true)
            {
                APP_SetResolvingList(true);
            }
            if (g_anpsCtrlInfo.bAllowNewPairing || (bPaired == false))
            {
                //Clear filter accept list
                APP_SetFilterAcceptList(false);
                //Clear resolving list
                APP_ScanEnable(APP_ANPS_STATE_SCAN);
            }
            else
            {
                //Clear filter accept list
                APP_SetFilterAcceptList(true);
                //Clear resolving list
                APP_ScanEnable(APP_ANPS_STATE_WITH_BOND_SCAN);
            }
        }
        break;
        
        case BLE_DM_EVT_CONNECTED:
        {
            /* TODO: implement your application code.*/
            p_conn = APP_AnpsGetFreeConnList();
            if (p_conn == NULL)
            { 
                return;
            }
            p_conn->connHandle=p_event->connHandle;
            g_anpsCtrlInfo.bAllowNewPairing=false;
            g_anpsCtrlInfo.state=APP_ANPS_STATE_CONNECTED;
            g_appLedHandler=APP_LED_StartByMode(APP_LED_MODE_CONN);
            bPaired=APP_GetPairedDeviceId(&devId);
            if ((bPaired == true) && (p_conn->bSecurityStatus == false))
			{
				BLE_DM_ProceedSecurity(p_conn->connHandle, 0);
			}
        }
        break;

        case BLE_DM_EVT_SECURITY_START:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_SUCCESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_FAIL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_PAIRED_DEVICE_FULL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_PAIRED_DEVICE_UPDATED:
        {
            /* TODO: implement your application code.*/
            BLE_DM_PairedDevInfo_T  *p_devInfo;

            p_devInfo = OSAL_Malloc(sizeof(BLE_DM_PairedDevInfo_T));
            if (p_devInfo != NULL)
            {
                if (BLE_DM_GetPairedDevice(p_event->peerDevId, p_devInfo) == MBA_RES_SUCCESS)
                {
                    //There is previous pairing data
                    if (g_anpsCtrlInfo.peerDevId < BLE_DM_MAX_PAIRED_DEVICE_NUM)
                    {
                        //Paired with different device from previous paired device, delete previous pairing data
                        if (g_anpsCtrlInfo.peerDevId != p_event->peerDevId)
                        {
                            BLE_DM_DeletePairedDevice(g_anpsCtrlInfo.peerDevId);
                        }
                    }
                    g_anpsCtrlInfo.peerDevId =p_event->peerDevId;
                }
                else
                {
                    /* TODO: implement your application code.*/
                }
                OSAL_Free(p_devInfo);
            }
        }
        break;

        case BLE_DM_EVT_CONN_UPDATE_SUCCESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_CONN_UPDATE_FAIL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;
    }
}