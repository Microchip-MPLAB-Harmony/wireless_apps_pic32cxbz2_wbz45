/*******************************************************************************
  Application Transparent Server Role Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_trps.c

  Summary:
    This file contains the Application Transparent Server Role functions for this project.

  Description:
    This file contains the Application Transparent Server Role functions for this project.
    The implementation of demo mode is included in this file.
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
#include "app.h"
#include "ble_util/byte_stream.h"
#include "stack_mgr.h"

#include "app_trp_common.h"
#include "app_trps.h"
#include "app_timer.h"
#include "app_ble.h"

#include "app_led.h"

#include "app_error_defs.h"
#include "system/console/sys_console.h"
#include "ble_dm/ble_dm.h"
#include "app_adv.h"


// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

static APP_TRP_ConnList_T s_trpsConnList_t;

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

static void APP_TRPS_LeDataProc(APP_TRP_ConnList_T *p_connList_t) {
    uint16_t status = APP_RES_FAIL;
    uint8_t grpId[] = {TRP_GRPID_NULL, TRP_GRPID_CHECK_SUM, TRP_GRPID_LOOPBACK, TRP_GRPID_FIX_PATTERN,
        TRP_GRPID_UART, TRP_GRPID_REV_LOOPBACK};

    if (p_connList_t->workModeEn == false) {
        if ((grpId[p_connList_t->workMode] > TRP_GRPID_NULL)
                && (grpId[p_connList_t->workMode] < TRP_GRPID_UART)) {
            APP_TRP_COMMON_SendErrorRsp(p_connList_t, grpId[p_connList_t->workMode]);
        }
    }

    switch (p_connList_t->workMode) {
        case TRP_WMODE_CHECK_SUM:
        {
            p_connList_t->checkSum = APP_TRP_COMMON_CalculateCheckSum(p_connList_t->checkSum,
                    &(p_connList_t->txTotalLeng), p_connList_t);
            if (p_connList_t->txTotalLeng == 0) {
                status = APP_TIMER_StopTimer(&(p_connList_t->protocolRspTmr.tmrHandle));
                if (status != APP_RES_SUCCESS)
                    SYS_CONSOLE_PRINT("APP_TIMER_PROTOCOL_RSP_01 stop error ! result=%d\r\n", status);
                APP_TRP_COMMON_SendCheckSumCommand(p_connList_t);
            }
        }
            break;

        case TRP_WMODE_LOOPBACK:
        {
            p_connList_t->srcType = APP_TRP_SRC_TYPE_LE;
            status = APP_TRP_COMMON_SendLeDataCircQueue(p_connList_t);
            if ((status == APP_RES_NO_RESOURCE) || (status == APP_RES_OOM)) {
                SYS_CONSOLE_PRINT("LB Tx error: status=%d\r\n", status);
            }
        }
            break;

        default:
            break;
    }
}

static void APP_TRPS_VendorCmdProc(APP_TRP_ConnList_T *p_connList_t, uint8_t *p_cmd) {
    uint16_t lastNumber, idx;
    uint8_t groupId, commandId;

    idx = 1;
    groupId = p_cmd[idx++];
    commandId = p_cmd[idx++];
    SYS_CONSOLE_PRINT("Group ID = %d, Command ID = %d \r\n", groupId, commandId);

    switch (groupId) {
        case TRP_GRPID_CHECK_SUM:
        {
            if (commandId == APP_TRP_WMODE_CHECK_SUM_DISABLE) {
                APP_TRP_COMMON_SendCheckSumCommand(p_connList_t);
                p_connList_t->workMode = TRP_WMODE_NULL;
            } else if (commandId == APP_TRP_WMODE_CHECK_SUM_ENABLE) {
                p_connList_t->workMode = TRP_WMODE_CHECK_SUM;
                p_connList_t->workModeEn = false;
                p_connList_t->checkSum = 0;
                p_connList_t->txCtrRspFg = 0;

            } else if (commandId == APP_TRP_WMODE_CHECK_SUM) {
                if (p_cmd[idx] == (p_connList_t->checkSum & 0xFF)) {
                    SYS_CONSOLE_PRINT("Check sum = %x. Check sum is correct. \r\n", p_cmd[idx]);
                } else {
                    SYS_CONSOLE_PRINT("Check sum = %x. Check sum is wrong. \r\n", p_cmd[idx]);
                }
            }
        }
            break;

        case TRP_GRPID_LOOPBACK:
        {
            if (commandId == APP_TRP_WMODE_LOOPBACK_DISABLE) {
                p_connList_t->workMode = TRP_WMODE_NULL;
            } else if (commandId == APP_TRP_WMODE_LOOPBACK_ENABLE) {
                p_connList_t->workMode = TRP_WMODE_LOOPBACK;
                p_connList_t->workModeEn = false;
                p_connList_t->txCtrRspFg = 0;
            }
        }
            break;

        case TRP_GRPID_FIX_PATTERN:
        {
            if (commandId == APP_TRP_WMODE_FIX_PATTERN_DISABLE) {
                // Send the last number
                p_connList_t->workMode = TRP_WMODE_NULL;
            } else if (commandId == APP_TRP_WMODE_FIX_PATTERN_ENABLE) {
                p_connList_t->workMode = TRP_WMODE_FIX_PATTERN;
                p_connList_t->workModeEn = false;
                p_connList_t->lastNumber = 0;
                p_connList_t->txCtrRspFg = 0;

            } else if (commandId == APP_TRP_WMODE_TX_LAST_NUMBER) {
                BUF_BE_TO_U16(&(lastNumber), &(p_cmd[idx]));
                if (lastNumber == (p_connList_t->lastNumber - 1)) {
                    SYS_CONSOLE_PRINT("The last number = %x. The last number check is successful !\r\n", lastNumber);
                } else {
                    SYS_CONSOLE_PRINT("The last number = %x. The last number check is fail !\r\n", lastNumber);
                }
                p_connList_t->workMode = TRP_WMODE_NULL;
            } else if (commandId == APP_TRP_WMODE_ERROR_RSP) {
                SYS_CONSOLE_PRINT("Fixed pattern error response! \r\n");
            }
        }
            break;

        case TRP_GRPID_UART:
        {
            //not supported
            if (commandId == APP_TRP_WMODE_UART_ENABLE){
                APP_TRP_COMMON_SendErrorRsp(p_connList_t, TRP_GRPID_UART);
                SYS_CONSOLE_PRINT("TRP_GRPID_UART is not supported! \r\n");
            }
        }
            break;

        case TRP_GRPID_TRANSMIT:
        {
            if (commandId == APP_TRP_WMODE_TX_DATA_END) {
                if (p_connList_t->workMode == TRP_WMODE_FIX_PATTERN) {
                    APP_TRP_COMMON_SendLastNumber(p_connList_t);
                }
                p_connList_t->workMode = TRP_WMODE_NULL;
                p_connList_t->workModeEn = false;
                APP_TRP_COMMON_DelAllLeCircData(&(p_connList_t->leCircQueue_t));
            } else if (commandId == APP_TRP_WMODE_TX_DATA_START) {
                p_connList_t->workModeEn = true;

                if (p_connList_t->workMode == TRP_WMODE_FIX_PATTERN) {
                    // Send the first packet
                    APP_TRP_COMMON_InitFixPatternParam(p_connList_t);
                    APP_TRP_COMMON_SendFixPatternFirstPkt(p_connList_t);
                }
                break;
            } else if (commandId == APP_TRP_WMODE_TX_DATA_LENGTH) {
                APP_UTILITY_BUF_BE_TO_U32(&(p_connList_t->txTotalLeng), &(p_cmd[idx]));
                SYS_CONSOLE_PRINT("Tx total length = %ld \r\n", p_connList_t->txTotalLeng);
            } else if (commandId == APP_TRP_WMODE_TX_TYPE) {
                p_connList_t->type = p_cmd[idx];

                if (p_connList_t->type == APP_TRP_TYPE_LEGACY)
                    p_connList_t->lePktLeng = p_connList_t->txMTU;
                else if (p_connList_t->channelEn & APP_TRCBP_DATA_CHAN_ENABLE)
                    p_connList_t->lePktLeng = p_connList_t->fixPattTrcbpMtu;
            }
        }
            break;

        case TRP_GRPID_UPDATE_CONN_PARA:
        {
            if (commandId == APP_TRP_WMODE_UPDATE_CONN_PARA) {
#ifndef APP_MIDDLEWARE_ENABLE

                BLE_GAP_ConnParams_T params;

                BUF_BE_TO_U16(&params.intervalMin, &p_cmd[3]);
                BUF_BE_TO_U16(&params.intervalMax, &p_cmd[5]);
                BUF_BE_TO_U16(&params.latency, &p_cmd[7]);
                BUF_BE_TO_U16(&params.supervisionTimeout, &p_cmd[9]);

                /* Validate connection parameters */
                if ((params.intervalMin >= BLE_GAP_CP_MIN_CONN_INTVAL_MIN) && (params.intervalMin <= BLE_GAP_CP_MIN_CONN_INTVAL_MAX) &&
                        (params.intervalMax >= BLE_GAP_CP_MAX_CONN_INTVAL_MIN) && (params.intervalMax <= BLE_GAP_CP_MAX_CONN_INTVAL_MAX) &&
                        (params.latency >= BLE_GAP_CP_LATENCY_MIN) && (params.latency <= BLE_GAP_CP_LATENCY_MAX) &&
                        (params.supervisionTimeout >= BLE_GAP_CP_CONN_SUPV_TIMEOUT_MIN) && (params.supervisionTimeout <= BLE_GAP_CP_CONN_SUPV_TIMEOUT_MAX)) {
                    //Update connection parameters
                    BLE_GAP_UpdateConnParam(p_connList_t->connHandle, &params);
                }
#else
                BLE_DM_ConnParamUpdate_T params;

                BUF_BE_TO_U16(&params.intervalMin, &p_cmd[3]);
                BUF_BE_TO_U16(&params.intervalMax, &p_cmd[5]);
                BUF_BE_TO_U16(&params.latency, &p_cmd[7]);
                BUF_BE_TO_U16(&params.timeout, &p_cmd[9]);

                /* Validate connection parameters */
                if ((params.intervalMin >= BLE_GAP_CP_MIN_CONN_INTVAL_MIN) && (params.intervalMin <= BLE_GAP_CP_MIN_CONN_INTVAL_MAX) &&
                        (params.intervalMax >= BLE_GAP_CP_MAX_CONN_INTVAL_MIN) && (params.intervalMax <= BLE_GAP_CP_MAX_CONN_INTVAL_MAX) &&
                        (params.latency >= BLE_GAP_CP_LATENCY_MIN) && (params.latency <= BLE_GAP_CP_LATENCY_MAX) &&
                        (params.timeout >= BLE_GAP_CP_CONN_SUPV_TIMEOUT_MIN) && (params.timeout <= BLE_GAP_CP_CONN_SUPV_TIMEOUT_MAX)) {
                    BLE_DM_ConnectionParameterUpdate(p_connList_t->connHandle, &params);
                }
#endif

            }
        }
            break;

        case TRP_GRPID_WMODE_SELECTION:
        {
            if ((commandId >= TRP_GRPID_NULL) && (commandId < TRP_GRPID_TRANSMIT)) {
                p_connList_t->workMode = commandId;
            }
        }
            break;

        case TRP_GRPID_REV_LOOPBACK:
        {
            if ((commandId == APP_TRP_WMODE_REV_LOOPBACK_DISABLE) || (commandId == APP_TRP_WMODE_ERROR_RSP)) {
                p_connList_t->workMode = TRP_WMODE_NULL;
            }
        }
            break;

        default:
            break;
    }
}

void APP_TRPS_LeRxProc(APP_TRP_ConnList_T *p_connList_t) {
    APP_TRPS_LeDataProc(p_connList_t);
}

void APP_TRPS_EventHandler(BLE_TRSPS_Event_T *p_event) {
    switch (p_event->eventId) {
        case BLE_TRSPS_EVT_CTRL_STATUS:
        {
            if (s_trpsConnList_t.connHandle == p_event->eventField.onCtrlStatus.connHandle) {
                if (p_event->eventField.onCtrlStatus.status == BLE_TRSPS_STATUS_CTRL_OPENED) {
                    s_trpsConnList_t.trpRole = APP_TRP_SERVER_ROLE;
                    s_trpsConnList_t.channelEn |= APP_TRP_CTRL_CHAN_ENABLE;
                } else {
                    s_trpsConnList_t.trpRole = 0;
                    s_trpsConnList_t.channelEn &= APP_TRP_CTRL_CHAN_DISABLE;
                }
            }
        }
            break;

        case BLE_TRSPS_EVT_TX_STATUS:
        {
            if (s_trpsConnList_t.connHandle == p_event->eventField.onTxStatus.connHandle) {
                if (p_event->eventField.onTxStatus.status == BLE_TRSPS_STATUS_TX_OPENED) {
                    if (!(s_trpsConnList_t.channelEn & APP_TRP_DATA_CHAN_ENABLE)) {
                        uint8_t txPhys = 0, rxPhys = 0, phyOptions = 0;

                        //Update Phy
                        txPhys = BLE_GAP_PHY_OPTION_2M;
                        rxPhys = BLE_GAP_PHY_OPTION_2M;
                        // phyOptions = 0;

                        BLE_GAP_SetPhy(p_event->eventField.onTxStatus.connHandle, txPhys, rxPhys, phyOptions);

                        BLE_DM_ConnParamUpdate_T params;

                        params.intervalMax = 0x10; //20ms
                        params.intervalMin = 0x10; //20ms
                        params.latency = 0;
                        params.timeout = 0x48; //720ms

                        BLE_DM_ConnectionParameterUpdate(p_event->eventField.onTxStatus.connHandle, &params);
                    }

                    s_trpsConnList_t.channelEn |= APP_TRP_DATA_CHAN_ENABLE;
                    s_trpsConnList_t.type = APP_TRP_TYPE_LEGACY;

                } else {
                    s_trpsConnList_t.channelEn &= APP_TRP_DATA_CHAN_DISABLE;

                    if (s_trpsConnList_t.channelEn & APP_TRCBP_DATA_CHAN_ENABLE) {
                        s_trpsConnList_t.type = APP_TRP_TYPE_TRCBP;
                    } else {
                        s_trpsConnList_t.type = APP_TRP_TYPE_UNKNOWN;
                    }
                }
            }
        }
            break;

        case BLE_TRSPS_EVT_RECEIVE_DATA:
        {
            if (s_trpsConnList_t.connHandle == p_event->eventField.onReceiveData.connHandle) {
                if (s_trpsConnList_t.workMode == TRP_WMODE_NULL) {
                    APP_TRP_COMMON_FreeLeData(&s_trpsConnList_t);
                    break;
                }
                APP_TRPS_LeRxProc(&s_trpsConnList_t);
            }
        }
            break;

        case BLE_TRSPS_EVT_CBFC_ENABLED:
        {
            // if (s_trpsConnList_t.connHandle == p_event->eventField.onCbfcEnabled.connHandle)
        }
            break;

        case BLE_TRSPS_EVT_CBFC_CREDIT:
        {
            //if (s_trpsConnList_t.connHandle == p_event->eventField.onCbfcEnabled.connHandle)
            //APP_TRPS_TxBufValidEvtProc();
            //SYS_CONSOLE_PRINT("CBCF credit ! \n");
        }
            break;

        case BLE_TRSPS_EVT_VENDOR_CMD:
        {
            if ((s_trpsConnList_t.connHandle == p_event->eventField.onVendorCmd.connHandle)
                    && (p_event->eventField.onVendorCmd.p_payLoad[0] == APP_TRP_VENDOR_OPCODE_BLE_UART))
                APP_TRPS_VendorCmdProc(&s_trpsConnList_t, p_event->eventField.onVendorCmd.p_payLoad);
        }
            break;

        default:
            break;
    }
}

void APP_TRPS_Init(void) {

    memset((uint8_t *) & s_trpsConnList_t, 0, sizeof (APP_TRP_ConnList_T));

}

void APP_TRPS_ConnEvtProc(BLE_GAP_Event_T *p_event) {

    s_trpsConnList_t.connHandle = p_event->eventField.evtConnect.connHandle;
}

void APP_TRPS_DiscEvtProc(uint16_t connHandle) {
    if (s_trpsConnList_t.connHandle == connHandle) {
        APP_TRP_COMMON_DelAllLeCircData(&(s_trpsConnList_t.leCircQueue_t));
        memset((uint8_t *) & s_trpsConnList_t, 0, sizeof (APP_TRP_ConnList_T));
    }
}

void APP_TRPS_SendUpConnParaStatusToClient(uint16_t connHandle, uint8_t upConnParaStatus) {
    if (s_trpsConnList_t.connHandle == connHandle) {
        uint8_t grpId = TRP_GRPID_UPDATE_CONN_PARA;
        uint8_t commandId = APP_TRP_WMODE_SNED_UP_CONN_STATUS;

        s_trpsConnList_t.upConnParaStatus = upConnParaStatus;
        APP_TRP_COMMON_SendUpConnParaStatus(&s_trpsConnList_t, grpId, commandId, upConnParaStatus);
    }
}

void APP_TRPS_TxBufValidEvtProc(void) {
    uint16_t status = APP_RES_SUCCESS;
    uint8_t grpId[] = {TRP_GRPID_NULL, TRP_GRPID_CHECK_SUM, TRP_GRPID_LOOPBACK,
		TRP_GRPID_FIX_PATTERN};

    if ((s_trpsConnList_t.workMode == TRP_WMODE_LOOPBACK) && (s_trpsConnList_t.leCircQueue_t.usedNum > 0)) {
        s_trpsConnList_t.srcType = APP_TRP_SRC_TYPE_LE;
        status = APP_TRP_COMMON_SendLeDataCircQueue(&s_trpsConnList_t);
    } else if ((s_trpsConnList_t.workMode == TRP_WMODE_FIX_PATTERN) && (s_trpsConnList_t.workModeEn == true)) {
        status = APP_TRP_COMMON_SendFixPattern(&s_trpsConnList_t);
        if (status & APP_RES_COMPLETE) {
            APP_TRP_COMMON_SendModeCommand(&s_trpsConnList_t, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
            APP_TRP_COMMON_SendLastNumber(&s_trpsConnList_t);
            s_trpsConnList_t.workModeEn = false;
        }
    } else if (s_trpsConnList_t.workMode == TRP_WMODE_FIX_PATTERN) {
        if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_GID_TX_FAIL) {
            APP_TRP_COMMON_SendModeCommand(&s_trpsConnList_t, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
        }
        if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_LAST_NUMBER_FAIL) {
            APP_TRP_COMMON_SendLastNumber(&s_trpsConnList_t);
        }
    } else if ((s_trpsConnList_t.workMode == TRP_WMODE_CHECK_SUM)
            && (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_CHECK_SUM_FAIL)) {
        APP_TRP_COMMON_SendCheckSumCommand(&s_trpsConnList_t);
    } else if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_ERROR_RSP_FAIL) {
        if ((grpId[s_trpsConnList_t.workMode] > TRP_GRPID_NULL)
                && (grpId[s_trpsConnList_t.workMode] < TRP_GRPID_UART)) {
            APP_TRP_COMMON_SendErrorRsp(&s_trpsConnList_t, grpId[s_trpsConnList_t.workMode]);
        }
    } else if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_STATUS_FLAG) {
        APP_TRP_COMMON_SendUpConnParaStatus(&s_trpsConnList_t, TRP_GRPID_UPDATE_CONN_PARA, APP_TRP_WMODE_SNED_UP_CONN_STATUS, s_trpsConnList_t.upConnParaStatus);
    }

}

void APP_TRPS_UpdateMtuEvtProc(uint16_t connHandle, uint16_t exchangedMTU) {
    if (s_trpsConnList_t.connHandle == connHandle) {
        s_trpsConnList_t.exchangedMTU = exchangedMTU;
        s_trpsConnList_t.txMTU = s_trpsConnList_t.exchangedMTU - ATT_HANDLE_VALUE_HEADER_SIZE;
    }
}

// uint8_t APP_TRPS_GetWorkMode(void)
// {
//     return s_trpsConnList_t.workMode;
// }
