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

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdint.h>
#include "osal/osal_freertos_extend.h"
#include "app_ble_handler.h"
#include "ble/service_ble/ble_cms/ble_button_led_svc.h"
#include "app_ble_custom_service.h"
#include "system/console/sys_console.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

GATTS_SendWriteRespParams_T *sp_trsRespParams;
GATTS_SendReadRespParams_T *sp_trsReadRespParams;
GATTS_SendErrRespParams_T *sp_trsErrParams;
uint16_t trsRespErrConnHandle;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
void APP_BleGapEvtHandler(BLE_GAP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            /* TODO: implement your application code.*/
            SYS_CONSOLE_PRINT("[BLE] Connected to Peer Device: 0x");
            for(int8_t idx=(GAP_MAX_BD_ADDRESS_LEN-1); idx>=0; idx--)
            {
                SYS_CONSOLE_PRINT("%02x", p_event->eventField.evtConnect.remoteAddr.addr[idx]);
            }
            SYS_CONSOLE_PRINT("\n\r[BLE] Connection Handle: %d\n\r",p_event->eventField.evtConnect.connHandle);
            G_ConnHandle = p_event->eventField.evtConnect.connHandle;
        }
        break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
            SYS_CONSOLE_PRINT("[BLE] Disconnected Handle: %d, Reason: 0x%X\n\r",p_event->eventField.evtDisconnect.connHandle,
            p_event->eventField.evtDisconnect.reason);
            G_ConnHandle = 0;
            BLE_GAP_SetAdvEnable(0x01, 0); //Enable BLE Advertisement             
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
        }
        break;

        case BLE_GAP_EVT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
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

        default:
        break;
    }
}

void APP_BleL2capEvtHandler(BLE_L2CAP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_L2CAP_EVT_CONN_PARA_UPDATE_REQ:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CONN_PARA_UPDATE_RSP:
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

void APP_GattSEvtReadHandler(GATT_EvtRead_T p_event)
{

    uint8_t error = 0;
    uint16_t status;
    SYS_CONSOLE_PRINT("[BLE] GATT Read ATTR Handle 0x%X \r\n",p_event.attrHandle);
    
    if ((p_event.attrHandle <= BUTTON_LED_START_HDL) ||
        (p_event.attrHandle > BUTTON_LED_END_HDL))
    {
        /* Not BLE Custom Service characteristic. */
        return;
    }

        switch(p_event.attrHandle)
        {
            case BUTTON_LED_HDL_CHAR_0:                            /**< Handle of characteristic 0. */
            case BUTTON_LED_HDL_CCCD_0:                            /**< Handle of characteristic 0 CCCD . */
            case BUTTON_LED_HDL_CHAR_1:                            /**< Handle of characteristic 1. */
            case BUTTON_LED_HDL_CHARVAL_1:                         /**< Handle of characteristic 1 value. */
                error = ATT_ERRCODE_APPLICATION_ERROR;
                break;
            case BUTTON_LED_HDL_CHARVAL_0:                         /**< Handle of characteristic 0 value. */                
//                SYS_CONSOLE_PRINT(" ATTR Handle Read 0x%X \r\n",p_event.attrHandle);
                break;
        }

    if ((p_event.readType == ATT_READ_REQ)
    || (p_event.readType == ATT_READ_BLOB_REQ))
    {
        if (!error)
        {
            sp_trsReadRespParams = (GATTS_SendReadRespParams_T *)OSAL_Malloc(sizeof(GATTS_SendReadRespParams_T));
            if (sp_trsReadRespParams == NULL)
            {
                return;
            }
            trsRespErrConnHandle = p_event.connHandle;
            sp_trsReadRespParams->responseType = ATT_READ_RSP;
            sp_trsReadRespParams->attrLength = 0x01;
            sp_trsReadRespParams->attrValue[0]= bleCSdata.rgbOnOffStatus;            
//            sp_trsReadRespParams->attrLength = 0x03;
//            sp_trsReadRespParams->attrValue[0]= bleCSdata.RGB_LED.Red;
//            sp_trsReadRespParams->attrValue[1]= bleCSdata.RGB_LED.Green;
//            sp_trsReadRespParams->attrValue[2]= bleCSdata.RGB_LED.Blue;
            status = GATTS_SendReadResponse(p_event.connHandle, sp_trsReadRespParams);
            if (status == MBA_RES_SUCCESS)
            {
                OSAL_Free(sp_trsReadRespParams);
                sp_trsReadRespParams = NULL;
            }
        }
        else
        {
            sp_trsErrParams = (GATTS_SendErrRespParams_T *)OSAL_Malloc(sizeof(GATTS_SendErrRespParams_T));
            if (sp_trsErrParams == NULL)
            {
                return;
            }
            trsRespErrConnHandle = p_event.connHandle;
            sp_trsErrParams->reqOpcode = p_event.readType;
            sp_trsErrParams->attrHandle = p_event.attrHandle;
            sp_trsErrParams->errorCode = error;
            status = GATTS_SendErrorResponse(p_event.connHandle, sp_trsErrParams);
            if (status == MBA_RES_SUCCESS)
            {
                OSAL_Free(sp_trsErrParams);
                sp_trsErrParams = NULL;
            }
        }
    }   
}

void APP_GattSEvtWriteHandler(GATT_EvtWrite_T p_event)
{
    uint8_t error = 0;
    uint16_t status;
    SYS_CONSOLE_PRINT("[BLE] GATT Write ATTR Handle 0x%X \r\n",p_event.attrHandle);
    
    if ((p_event.attrHandle <= BUTTON_LED_START_HDL) ||
        (p_event.attrHandle > BUTTON_LED_END_HDL))
    {
        /* Not BLE Custom Service characteristic. */
        error = ATT_ERRCODE_INVALID_HANDLE;
        return;
    }
    
        switch(p_event.attrHandle)
        {
            case BUTTON_LED_HDL_CHAR_0:                            /**< Handle of characteristic 0. */
            case BUTTON_LED_HDL_CHARVAL_0:                         /**< Handle of characteristic 0 value. */
            case BUTTON_LED_HDL_CCCD_0:                            /**< Handle of characteristic 0 CCCD . */
            case BUTTON_LED_HDL_CHAR_1:                            /**< Handle of characteristic 1. */
                error = ATT_ERRCODE_APPLICATION_ERROR;
                break;
            case BUTTON_LED_HDL_CHARVAL_1:                         /**< Handle of characteristic 1 value. */
//                SYS_CONSOLE_PRINT(" ATTR Handle %d \r\n",p_event.attrHandle);
                APP_CustomService_RGB_Callback(p_event.writeValue);
                break;
        }

    if ((p_event.writeType == ATT_WRITE_REQ)
    || (p_event.writeType == ATT_PREPARE_WRITE_REQ))
    {
        if (!error)
        {
            sp_trsRespParams = (GATTS_SendWriteRespParams_T *)OSAL_Malloc(sizeof(GATTS_SendWriteRespParams_T));
            if (sp_trsRespParams == NULL)
            {
                return;
            }
            trsRespErrConnHandle = p_event.connHandle;
            sp_trsRespParams->responseType = ATT_WRITE_RSP;
            status = GATTS_SendWriteResponse(p_event.connHandle, sp_trsRespParams);
            if (status == MBA_RES_SUCCESS)
            {
                OSAL_Free(sp_trsRespParams);
                sp_trsRespParams = NULL;
            }
        }
        else
        {
            sp_trsErrParams = (GATTS_SendErrRespParams_T *)OSAL_Malloc(sizeof(GATTS_SendErrRespParams_T));
            if (sp_trsErrParams == NULL)
            {
                return;
            }
            trsRespErrConnHandle = p_event.connHandle;
            sp_trsErrParams->reqOpcode = p_event.writeType;
            sp_trsErrParams->attrHandle = p_event.attrHandle;
            sp_trsErrParams->errorCode = error;
            status = GATTS_SendErrorResponse(p_event.connHandle, sp_trsErrParams);
            if (status == MBA_RES_SUCCESS)
            {
                OSAL_Free(sp_trsErrParams);
                sp_trsErrParams = NULL;
            }
        }
    }
}

void APP_GattEvtHandler(GATT_Event_T *p_event)
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
            APP_GattSEvtReadHandler((GATT_EvtRead_T)p_event->eventField.onRead);
        }
        break;

        case GATTS_EVT_WRITE:
        {
            /* TODO: implement your application code.*/
            APP_GattSEvtWriteHandler((GATT_EvtWrite_T)p_event->eventField.onWrite);
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

void APP_BleSmpEvtHandler(BLE_SMP_Event_T *p_event)
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
    switch(p_event->eventId)
    {
        case BLE_DM_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
        }
        break;
        
        case BLE_DM_EVT_CONNECTED:
        {
            /* TODO: implement your application code.*/
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