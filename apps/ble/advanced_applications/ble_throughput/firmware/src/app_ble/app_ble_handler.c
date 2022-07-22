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
#include "ble_util/byte_stream.h"
#include "stack_mgr.h"
#include "ble_l2cap.h"
#include "gatt.h"
#include "ble_gap.h"
#include "ble_smp.h"
#include "app_error_defs.h"

#include "app_ble.h"
#include "app_ble_handler.h"
#include "app_trps.h"
#include "app_adv.h"
#include "app_led.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define APP_BLE_NUM_ADDR_IN_DEV_NAME    2    /**< The number of bytes of device address included in the device name. */

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Global Variabless
// *****************************************************************************
// *****************************************************************************
APP_Database_T g_appDb;
APP_ConnList g_appConnList[APP_BLE_MAX_LINK_NUMBER];

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_BLE_ConnList_T s_bleConnList[APP_BLE_MAX_LINK_NUMBER];
static APP_BLE_ConnList_T *sp_currentBleLink = NULL;
static uint8_t s_currBleConnIdx;

#ifdef APP_PAIRING_ENABLE
static uint8_t s_pairTrigger;
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

static void APP_ClearConnListByConnHandle(uint16_t connHandle) {
    uint8_t i;

    for (i = 0; i < APP_BLE_MAX_LINK_NUMBER; i++) {
        if (s_bleConnList[i].connData.handle == connHandle) {
            memset((uint8_t *) (&s_bleConnList[i]), 0, sizeof (APP_BLE_ConnList_T));
            s_bleConnList[i].linkState = APP_BLE_STATE_STANDBY;
        }
    }
}

APP_BLE_ConnList_T *APP_GetFreeConnList(void) {
    uint8_t i;

    for (i = 0; i < APP_BLE_MAX_LINK_NUMBER; i++) {
        if (s_bleConnList[i].connData.handle == 0) {
            s_currBleConnIdx = i;
            return (&s_bleConnList[i]);
        }
    }
    return NULL;
}

static APP_BLE_ConnList_T *APP_GetConnInfoByConnHandle(uint16_t connHandle) {
    uint8_t i;

    for (i = 0; i < APP_BLE_MAX_LINK_NUMBER; i++) {
        if (s_bleConnList[i].connData.handle == connHandle) {
            return (&s_bleConnList[i]);
        }
    }
    return NULL;
}

static void APP_SwitchActiveDevice(void) {
    uint8_t i;
    uint8_t probeIndex;

    for (i = 1; i < APP_BLE_MAX_LINK_NUMBER; i++) {
        probeIndex = ((s_currBleConnIdx + i) % APP_BLE_MAX_LINK_NUMBER);

        if (s_bleConnList[probeIndex].linkState == APP_BLE_STATE_CONNECTED) {
            sp_currentBleLink = &s_bleConnList[probeIndex];
            s_currBleConnIdx = probeIndex;

            return;
        }
    }

    s_currBleConnIdx = APP_BLE_UNKNOWN_ID;
}

void APP_BleGapEvtHandler(BLE_GAP_Event_T *p_event) {
    APP_BLE_ConnList_T *p_bleConn = NULL;

    switch (p_event->eventId) {
        case BLE_GAP_EVT_CONNECTED:
        {
            APP_TRPS_ConnEvtProc(p_event);
            APP_LED_AlwaysOn(APP_LED_TYPE_GREEN, APP_LED_TYPE_NULL);

            p_bleConn = APP_GetFreeConnList();
            if (p_bleConn) {
                GATTS_UpdateBondingInfo(p_event->eventField.evtConnect.connHandle, NULL, 0, NULL); //TODO: Have to handle bonded case

                /* Update the connection parameter */
                p_bleConn->linkState = APP_BLE_STATE_CONNECTED;
                p_bleConn->connData.role = p_event->eventField.evtConnect.role; // 0x00: Central, 0x01:Peripheral
                p_bleConn->connData.handle = p_event->eventField.evtConnect.connHandle;
                p_bleConn->connData.connInterval = p_event->eventField.evtConnect.interval;
                p_bleConn->connData.connLatency = p_event->eventField.evtConnect.latency;
                p_bleConn->connData.supervisionTimeout = p_event->eventField.evtConnect.supervisionTimeout;

                /* Save Remote Device Address */
                p_bleConn->connData.remoteAddr.addrType = p_event->eventField.evtConnect.remoteAddr.addrType;
                memcpy((uint8_t *) p_bleConn->connData.remoteAddr.addr, (uint8_t *) p_event->eventField.evtConnect.remoteAddr.addr, GAP_MAX_BD_ADDRESS_LEN);

                p_bleConn->secuData.smpInitiator.addrType = p_event->eventField.evtConnect.remoteAddr.addrType;
                memcpy((uint8_t *) p_bleConn->secuData.smpInitiator.addr, (uint8_t *) p_event->eventField.evtConnect.remoteAddr.addr, GAP_MAX_BD_ADDRESS_LEN);

                sp_currentBleLink = p_bleConn;

            }
        }
            break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            //Clear connection list
            APP_ClearConnListByConnHandle(p_event->eventField.evtDisconnect.connHandle);
            APP_LED_AlwaysOn(APP_LED_TYPE_NULL, APP_LED_TYPE_NULL);

            APP_TRPS_DiscEvtProc(p_event->eventField.evtDisconnect.connHandle);

            /* Restart advertising */
            APP_ADV_Start();
            APP_SwitchActiveDevice();
        }
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            uint8_t updateStatus = APP_RES_SUCCESS;


            /* Update the connection parameter */
            if (p_event->eventField.evtConnParamUpdate.status == 0) {
                // APP_BLE_ConnList_T *p_bleConn = NULL;

                p_bleConn = APP_GetConnInfoByConnHandle(p_event->eventField.evtConnParamUpdate.connHandle);

                if (p_bleConn) {
                    p_bleConn->connData.handle = p_event->eventField.evtConnParamUpdate.connHandle;
                    p_bleConn->connData.connInterval = p_event->eventField.evtConnParamUpdate.connParam.intervalMin;
                    p_bleConn->connData.connLatency = p_event->eventField.evtConnParamUpdate.connParam.latency;
                    p_bleConn->connData.supervisionTimeout = p_event->eventField.evtConnParamUpdate.connParam.supervisionTimeout;
                }
            } else {
                updateStatus = APP_RES_FAIL;
            }

            APP_TRPS_SendUpConnParaStatusToClient(p_event->eventField.evtConnParamUpdate.connHandle, updateStatus);
        }
            break;

        case BLE_GAP_EVT_ENCRYPT_STATUS:
        {
            p_bleConn = APP_GetConnInfoByConnHandle(p_event->eventField.evtEncryptStatus.connHandle);

            if (p_event->eventField.evtEncryptStatus.status != BLE_GAP_ENCRYPT_SUCCESS) {
                BLE_GAP_Disconnect(p_event->eventField.evtEncryptStatus.connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
            }

            if (p_bleConn) {
                /* Set Encryption */
                if (p_event->eventField.evtEncryptStatus.status == BLE_GAP_ENCRYPT_SUCCESS) {
                    p_bleConn->secuData.encryptionStatus = 1; //enable
                } else {
                    p_bleConn->secuData.encryptionStatus = 0; //disable
                }
            }
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
            APP_TRPS_TxBufValidEvtProc();
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

        default:
            break;
    }
}

void APP_BleL2capEvtHandler(BLE_L2CAP_Event_T *p_event) {
    switch (p_event->eventId) {
        case BLE_L2CAP_EVT_CONN_PARA_UPDATE_REQ:
        {
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

void APP_GattEvtHandler(GATT_Event_T *p_event) {
    switch (p_event->eventId) {
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
            APP_TRPS_UpdateMtuEvtProc(p_event->eventField.onUpdateMTU.connHandle,
                    p_event->eventField.onUpdateMTU.exchangedMTU);
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

void APP_BleSmpEvtHandler(BLE_SMP_Event_T *p_event) {
    switch (p_event->eventId) {
        case BLE_SMP_EVT_PAIRING_COMPLETE:
        {
            /* TODO: implement your application code.*/

            if (p_event->eventField.evtPairingComplete.status != BLE_SMP_PAIRING_SUCCESS) {
                BLE_GAP_Disconnect(p_event->eventField.evtPairingComplete.connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
            }
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
        }
            break;

        case BLE_SMP_EVT_NOTIFY_KEYS:
        {
            /* TODO: implement your application code.*/
        }
            break;

        case BLE_SMP_EVT_PAIRING_REQUEST:
        {
            //Middleware will reply the pairing request
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

        default:
            break;
    }
}

void APP_DmEvtHandler(BLE_DM_Event_T *p_event) {
    switch (p_event->eventId) {
        case BLE_DM_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
        }
            break;

        case BLE_DM_EVT_CONNECTED:
        {
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

static void APP_HexToAscii(uint8_t byteNum, uint8_t *p_hex, uint8_t *p_ascii) {
    uint8_t i, j, c;
    uint8_t digitNum = byteNum * 2;

    if (p_hex == NULL || p_ascii == NULL)
        return;

    for (i = 0; i < digitNum; i++) {
        j = i / 2;
        c = p_hex[j] & 0x0F;

        if (c >= 0x00 && c <= 0x09) {
            p_ascii[digitNum - i - 1] = c + 0x30;
        } else if (c >= 0x0A && c <= 0x0F) {
            p_ascii[digitNum - i - 1] = c - 0x0A + 'A';
        }

        p_hex[j] /= 16;
    }
}

void APP_UpdateLocalName(uint8_t devNameLen, uint8_t *p_devName) {
    uint8_t localName[GAP_MAX_DEVICE_NAME_LEN] = {0};
    uint8_t localNameLen = 0;

    if (p_devName == NULL || devNameLen == 0) {
        BLE_GAP_Addr_T addrPara;
        uint8_t addrAscii[APP_BLE_NUM_ADDR_IN_DEV_NAME * 2];
        uint8_t digitNum = APP_BLE_NUM_ADDR_IN_DEV_NAME * 2;

        localName[localNameLen++] = 'B';
        localName[localNameLen++] = 'L';
        localName[localNameLen++] = 'E';
        localName[localNameLen++] = '_';
        localName[localNameLen++] = 'U';
        localName[localNameLen++] = 'A';
        localName[localNameLen++] = 'R';
        localName[localNameLen++] = 'T';
        localName[localNameLen++] = '_';

        BLE_GAP_GetDeviceAddr(&addrPara);

        APP_HexToAscii(APP_BLE_NUM_ADDR_IN_DEV_NAME, addrPara.addr, addrAscii);

        memcpy(&localName[localNameLen], &addrAscii[0], digitNum);

        localNameLen += digitNum;

        BLE_GAP_SetDeviceName(localNameLen, localName);
    } else {
        BLE_GAP_SetDeviceName(devNameLen, p_devName);
    }
}

APP_BLE_LinkState_T APP_GetBleState(void) {
    return (sp_currentBleLink->linkState);
}

void APP_SetBleState(APP_BLE_LinkState_T state) {
    sp_currentBleLink->linkState = state;
}

uint8_t APP_GetBleRole(void) {
    return (sp_currentBleLink->connData.role);
}

uint16_t APP_GetCurrentConnHandle(void) {
    if (sp_currentBleLink->linkState == APP_BLE_STATE_CONNECTED) {
        return sp_currentBleLink->connData.handle;
    } else {
        return 0;
    }
}

void APP_InitConnList(void) {
    uint8_t i;

    sp_currentBleLink = &s_bleConnList[0];
    s_currBleConnIdx = APP_BLE_UNKNOWN_ID;

    for (i = 0; i < APP_BLE_MAX_LINK_NUMBER; i++) {
        memset((uint8_t *) (&s_bleConnList[i]), 0, sizeof (APP_BLE_ConnList_T));
        s_bleConnList[i].linkState = APP_BLE_STATE_STANDBY;
    }
}
