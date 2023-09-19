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
#include "app_uart.h"
#include "app_trps.h"
#include "app_timer.h"
#include "app_ble.h"
#include "drv_usart.h"
#include "app_cmd_evt_proc.h"
#include "app_led.h"
#include "app_key.h"
#include "app_error_defs.h"
#include "app_log.h"
#include "ble_dm/ble_dm.h"
#include "app_adv.h"
#ifdef APP_BLE_MULTI_LINK_ENABLE
#include "app_ble_handler.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
#ifdef APP_BLE_MULTI_LINK_ENABLE
static APP_TRP_ConnList_T       s_trpsConnList_t[APP_TRPS_MAX_LINK_NUMBER];
static APP_TRP_ConnList_T       *sp_trpsCurrentLink = NULL;
#else
static APP_TRP_ConnList_T       s_trpsConnList_t;
#endif
static uint8_t                  s_defaultUartEn;

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
#ifndef APP_BLE_MULTI_LINK_ENABLE
static void APP_TRPS_LeDataProc(APP_TRP_ConnList_T *p_connList_t)
{
    uint16_t status = APP_RES_FAIL;
    uint8_t grpId[] = {TRP_GRPID_NULL, TRP_GRPID_CHECK_SUM, TRP_GRPID_LOOPBACK, TRP_GRPID_FIX_PATTERN,
        TRP_GRPID_UART, TRP_GRPID_REV_LOOPBACK};

    if ((p_connList_t->workModeEn == false) && (p_connList_t->workMode != TRP_WMODE_UART))
    {
        if ((grpId[p_connList_t->workMode] > TRP_GRPID_NULL) 
            && (grpId[p_connList_t->workMode] < TRP_GRPID_UART))
        {
            APP_TRP_COMMON_SendErrorRsp(p_connList_t, grpId[p_connList_t->workMode]);
        }
    }

    switch (p_connList_t->workMode)
    {
        case TRP_WMODE_CHECK_SUM:
        {
            p_connList_t->checkSum = APP_TRP_COMMON_CalculateCheckSum(p_connList_t->checkSum,
                 &(p_connList_t->txTotalLeng), p_connList_t);
            if (p_connList_t->txTotalLeng == 0)
            {
                status = APP_TIMER_StopTimer(&(p_connList_t->protocolRspTmr.tmrHandle));
                if (status != APP_RES_SUCCESS)
                    APP_LOG_ERROR("APP_TIMER_PROTOCOL_RSP_01 stop error ! result=%d\n", status);
                APP_TRP_COMMON_SendCheckSumCommand(p_connList_t);
            }
        }
        break;

        case TRP_WMODE_LOOPBACK:
        {
            p_connList_t->srcType = APP_TRP_SRC_TYPE_LE;
            status = APP_TRP_COMMON_SendLeDataCircQueue(p_connList_t);
            if ((status == APP_RES_NO_RESOURCE) || (status == APP_RES_OOM))
            {
                APP_LOG_ERROR("LB Tx error: status=%d\n", status);
            }
        }
        break;

        case TRP_WMODE_UART:
        {
            APP_TRP_COMMON_SendTrpProfileDataToUART(p_connList_t);
        }
        break;

        default:
            break;
    }
}
#endif

static void APP_TRPS_VendorCmdProc(APP_TRP_ConnList_T *p_connList_t, uint8_t *p_cmd)
{
    uint16_t lastNumber, idx;
    uint8_t groupId, commandId;

    idx = 1;
    groupId = p_cmd[idx++];
    commandId = p_cmd[idx++];
    APP_LOG_INFO("Group ID = %d, Command ID = %d \n", groupId, commandId);

    switch (groupId)
    {
        case TRP_GRPID_CHECK_SUM:
        {
            if (commandId == APP_TRP_WMODE_CHECK_SUM_DISABLE)
            {
                APP_TRP_COMMON_SendCheckSumCommand(p_connList_t);
                p_connList_t->workMode = TRP_WMODE_NULL;
            }
            else if (commandId == APP_TRP_WMODE_CHECK_SUM_ENABLE)
            {
                p_connList_t->workMode = TRP_WMODE_CHECK_SUM;
                p_connList_t->workModeEn = false;
                p_connList_t->checkSum = 0;
                p_connList_t->txCtrRspFg = 0;
                g_enableUartMode &= ~APP_TRP_SERVER_UART;
            #ifdef APP_DEMO_MODE_LED_ENABLE
                g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS_CS);
            #endif
            }
            else if (commandId == APP_TRP_WMODE_CHECK_SUM)
            {
                if (p_cmd[idx] == (p_connList_t->checkSum & 0xFF))
                {
                    APP_LOG_INFO("Check sum = %x. Check sum is correct. \n", p_cmd[idx]);
                }
                else
                {
                    APP_LOG_INFO("Check sum = %x. Check sum is wrong. \n", p_cmd[idx]);
                }
            }
        }
        break;

        case TRP_GRPID_LOOPBACK:
        {
            if (commandId == APP_TRP_WMODE_LOOPBACK_DISABLE)
            {
                p_connList_t->workMode = TRP_WMODE_NULL;
            }
            else if (commandId == APP_TRP_WMODE_LOOPBACK_ENABLE)
            {
                p_connList_t->workMode = TRP_WMODE_LOOPBACK;
                p_connList_t->workModeEn = false;
                p_connList_t->txCtrRspFg = 0;
                g_enableUartMode &= ~APP_TRP_SERVER_UART;
            #ifdef APP_DEMO_MODE_LED_ENABLE
                g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS_LB);
            #endif
                trpLeRxCount = 0;
                trpLeTxCount = 0;
            }
        }
        break;

        case TRP_GRPID_FIX_PATTERN:
        {
            if (commandId == APP_TRP_WMODE_FIX_PATTERN_DISABLE)
            {
                // Send the last number
                p_connList_t->workMode = TRP_WMODE_NULL;
            }
            else if (commandId == APP_TRP_WMODE_FIX_PATTERN_ENABLE)
            {
                p_connList_t->workMode = TRP_WMODE_FIX_PATTERN;
                p_connList_t->workModeEn = false;
                p_connList_t->lastNumber = 0;
                p_connList_t->txCtrRspFg = 0;
                g_enableUartMode &= ~APP_TRP_SERVER_UART;
            #ifdef APP_DEMO_MODE_LED_ENABLE
                g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS_FP);
            #endif
                trpLeTxCount = 0;
            }
            else if (commandId == APP_TRP_WMODE_TX_LAST_NUMBER)
            {
                BUF_BE_TO_U16(&(lastNumber), &(p_cmd[idx]));
                if (lastNumber == (p_connList_t->lastNumber - 1))
                {
                    APP_LOG_INFO("The last number = %x. The last number check is successful !\n", lastNumber);
                }
                else
                {
                    APP_LOG_INFO("The last number = %x. The last number check is fail !\n", lastNumber);
                }
                p_connList_t->workMode = TRP_WMODE_NULL;
            }
            else if (commandId == APP_TRP_WMODE_ERROR_RSP)
            {
                APP_LOG_ERROR("Fixed pattern error response! \n");
            }
        }
        break;

        case TRP_GRPID_UART:
        {
            if (commandId == APP_TRP_WMODE_UART_DISABLE)
            {
                APP_TRP_COMMON_DelAllCircData(&(p_connList_t->uartCircQueue_t));
                APP_TRP_COMMON_DelAllLeCircData(&(p_connList_t->leCircQueue_t));
                if (s_defaultUartEn == 0)
                {
                    p_connList_t->workMode = TRP_WMODE_NULL;
                    g_enableUartMode &= ~APP_TRP_SERVER_UART;
                }
            }
            else if (commandId == APP_TRP_WMODE_UART_ENABLE)
            {
                p_connList_t->workMode = TRP_WMODE_UART;
                p_connList_t->txCtrRspFg = 0;
                g_enableUartMode |= APP_TRP_SERVER_UART;
            #ifdef APP_DEMO_MODE_LED_ENABLE
                g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS_UART);
            #endif
                trpLeRxCount = 0;
                trpLeTxCount = 0;
                trpUartRxCount = 0;
                trpUartTxCount = 0;
            }
        }
        break;

        case TRP_GRPID_TRANSMIT:
        {
            if (commandId == APP_TRP_WMODE_TX_DATA_END)
            {
                if (p_connList_t->workMode == TRP_WMODE_FIX_PATTERN)
                {
                    APP_TRP_COMMON_SendLastNumber(p_connList_t);
                }
                p_connList_t->workMode = TRP_WMODE_NULL;
                p_connList_t->workModeEn = false;
                APP_TRP_COMMON_DelAllLeCircData(&(p_connList_t->leCircQueue_t));
            }
            else if (commandId == APP_TRP_WMODE_TX_DATA_START)
            {
                p_connList_t->workModeEn = true;
                
                if ((p_connList_t->workMode == TRP_WMODE_FIX_PATTERN) 
                    || (p_connList_t->workMode == TRP_WMODE_REV_LOOPBACK))
                {
                    // Send the first packet
                    APP_TRP_COMMON_InitFixPatternParam(p_connList_t);
                    APP_TRP_COMMON_SendFixPatternFirstPkt(p_connList_t);
                }
                break;
            }
            else if (commandId == APP_TRP_WMODE_TX_DATA_LENGTH)
            {
                APP_UTILITY_BUF_BE_TO_U32(&(p_connList_t->txTotalLeng), &(p_cmd[idx]));
                APP_LOG_DEBUG("Tx total length = %ld \n", p_connList_t->txTotalLeng);
            }
            else if (commandId == APP_TRP_WMODE_TX_TYPE)
            {
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
            if (commandId == APP_TRP_WMODE_UPDATE_CONN_PARA)
            {
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
                   (params.supervisionTimeout >= BLE_GAP_CP_CONN_SUPERVISION_TIMEOUT_MIN) && (params.supervisionTimeout <= BLE_GAP_CP_CONN_SUPERVISION_TIMEOUT_MAX))
                {
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
                   (params.timeout >= BLE_GAP_CP_CONN_SUPERVISION_TIMEOUT_MIN) && (params.timeout <= BLE_GAP_CP_CONN_SUPERVISION_TIMEOUT_MAX))
                {
                    BLE_DM_ConnectionParameterUpdate(p_connList_t->connHandle, &params);
                }
#endif

            }
        }
        break;

        case TRP_GRPID_WMODE_SELECTION:
        {
            if((commandId >= TRP_GRPID_NULL) && (commandId < TRP_GRPID_TRANSMIT))
            {
                p_connList_t->workMode = commandId;
            }
        }
        break;

        case TRP_GRPID_REV_LOOPBACK:
        {
            if ((commandId == APP_TRP_WMODE_REV_LOOPBACK_DISABLE) || (commandId == APP_TRP_WMODE_ERROR_RSP))
            {
                p_connList_t->workMode = TRP_WMODE_NULL;
            }
        }
        break;
        
        default:
            break;
    }
}

#ifdef APP_BLE_MULTI_LINK_ENABLE
static uint16_t APP_TRPS_ReSendCommand(void)
{
    APP_TRP_ConnList_T  *p_TrpsConnList = NULL;
    uint8_t index;
    uint8_t grpId[] = {TRP_GRPID_NULL, TRP_GRPID_CHECK_SUM, TRP_GRPID_LOOPBACK, TRP_GRPID_FIX_PATTERN, 
        TRP_GRPID_UART, TRP_GRPID_REV_LOOPBACK};
    uint16_t result = APP_RES_SUCCESS;

    
    for(index = 0; index < APP_TRPS_MAX_LINK_NUMBER; index++)
    {
        if ((s_trpsConnList_t[index].connHandle != 0) && (s_trpsConnList_t[index].txCtrRspFg != 0))
        {
            p_TrpsConnList = &s_trpsConnList_t[index];

            if (p_TrpsConnList->workMode ==  TRP_WMODE_FIX_PATTERN)
            {
                if (p_TrpsConnList->txCtrRspFg & APP_TRP_SEND_GID_TX_FAIL)
                {
                    result = APP_TRP_COMMON_SendModeCommand(p_TrpsConnList, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
                }
                
                if (p_TrpsConnList->txCtrRspFg & APP_TRP_SEND_LAST_NUMBER_FAIL)
                {
                    result = APP_TRP_COMMON_SendLastNumber(p_TrpsConnList);
                }
            }
            else if ((p_TrpsConnList->workMode ==  TRP_WMODE_CHECK_SUM) && (p_TrpsConnList->txCtrRspFg & APP_TRP_SEND_CHECK_SUM_FAIL))
            {
                result = APP_TRP_COMMON_SendCheckSumCommand(p_TrpsConnList);
            }
            else if ((p_TrpsConnList->workMode ==  TRP_WMODE_REV_LOOPBACK) && (p_TrpsConnList->txCtrRspFg & APP_TRP_SEND_GID_REV_LB_FAIL))
            {
                result = APP_TRP_COMMON_SendModeCommand(p_TrpsConnList, TRP_GRPID_REV_LOOPBACK, APP_TRP_WMODE_REV_LOOPBACK_ENABLE);
            }
            else if (p_TrpsConnList->txCtrRspFg & APP_TRP_SEND_ERROR_RSP_FAIL)
            {
                if (((grpId[p_TrpsConnList->workMode] > TRP_GRPID_NULL) 
                    && (grpId[p_TrpsConnList->workMode] < TRP_GRPID_UART)) 
                    || (grpId[p_TrpsConnList->workMode] == TRP_GRPID_REV_LOOPBACK))
                {
                    result = APP_TRP_COMMON_SendErrorRsp(p_TrpsConnList, grpId[p_TrpsConnList->workMode]);
                }
            }
            else if (p_TrpsConnList->txCtrRspFg & APP_TRP_SEND_STATUS_FLAG)
            {
                result = APP_TRP_COMMON_SendUpConnParaStatus(p_TrpsConnList, TRP_GRPID_UPDATE_CONN_PARA, APP_TRP_WMODE_SNED_UP_CONN_STATUS, p_TrpsConnList->upConnParaStatus);
            }
        }

    }

    return result;
}

#if 0
static APP_TRP_ConnList_T *APP_TRPS_GetActiveConnList(void)
{
    uint8_t i;

    for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
    {
        if (s_trpsConnList_t[i].connHandle != 0)
        {
            return &s_trpsConnList_t[i];
        }
    }

    return NULL;
}
#endif

static APP_TRP_ConnList_T *APP_TRPS_GetSamllestHandleConnList(void)
{
    uint8_t i, idx = 0;
    uint16_t small = 0xFFFF;

    for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
    {
        if ((s_trpsConnList_t[i].connHandle != 0) && (s_trpsConnList_t[i].connHandle < small))
        {
            idx = i;
            small = s_trpsConnList_t[i].connHandle;
        }
    }

    if (small)
    {
        return &s_trpsConnList_t[idx];
    }
    else
    {
        return NULL;
    }
}


APP_TRP_ConnList_T *APP_TRPS_GetConnListByIndex(uint8_t index)
{
    if (index < APP_TRPS_MAX_LINK_NUMBER)
    {
        if (s_trpsConnList_t[index].connHandle != 0)
            return &s_trpsConnList_t[index];
    }
    
    return NULL;
}

APP_TRP_ConnList_T *APP_TRPS_GetConnListByConnHandle(uint16_t connHandle)
{
    uint8_t index;

    for (index = 0; index < APP_TRPS_MAX_LINK_NUMBER; index++)
    {
        if (s_trpsConnList_t[index].connHandle == connHandle)
        {
            return &s_trpsConnList_t[index];
        }
    }

    return NULL;
}

void APP_TRPS_LeRxMultiLinkProc(APP_TRP_ConnList_T *p_connList_t)
{
    uint16_t status = APP_RES_FAIL;
    //uint8_t grpId[] = {TRP_GRPID_NULL, TRP_GRPID_CHECK_SUM, TRP_GRPID_LOOPBACK, TRP_GRPID_FIX_PATTERN},
    uint8_t linkNum = APP_GetConnLinkNum(), checkListFg = false;


    sp_trpsCurrentLink = APP_TRPS_GetConnListByIndex(g_trpsTrafficPriority_t.txToken);
    
    if (sp_trpsCurrentLink == NULL)
        sp_trpsCurrentLink = APP_TRPS_ChangeNextLink(APP_TRP_LINK_TYPE_TX, &g_trpsTrafficPriority_t);
    
    if(sp_trpsCurrentLink == NULL)
        return;

    p_connList_t = sp_trpsCurrentLink;
    g_trpsTrafficPriority_t.validNumber = APP_TRP_MAX_TRANSMIT_NUM;
    
    while (g_trpsTrafficPriority_t.validNumber > 0)
    {        
        switch (sp_trpsCurrentLink->workMode)
        {
            case TRP_WMODE_CHECK_SUM:
            {
                sp_trpsCurrentLink->checkSum = APP_TRP_COMMON_CalculateCheckSum(sp_trpsCurrentLink->checkSum, &(sp_trpsCurrentLink->txTotalLeng), sp_trpsCurrentLink);
                
                if (sp_trpsCurrentLink->txTotalLeng == 0)
                {
                    status = APP_TIMER_StopTimer(&(sp_trpsCurrentLink->protocolRspTmr.tmrHandle));
                    
                    if (status != APP_RES_SUCCESS)
                        APP_LOG_ERROR("APP_TIMER_PROTOCOL_RSP_01 stop error ! result=%d\n", status);
                    
                    APP_TRP_COMMON_SendCheckSumCommand(sp_trpsCurrentLink);
                }
                sp_trpsCurrentLink->maxAvailTxNumber = 0;
            }
            break;

            case TRP_WMODE_LOOPBACK:
            {
                status = APP_TRP_COMMON_SendMultiLinkLeDataTrpProfile(&g_trpsTrafficPriority_t, sp_trpsCurrentLink);
            }
            break;

            case TRP_WMODE_UART:
            {
                if (linkNum == 1)
                    APP_TRP_COMMON_SendTrpProfileDataToUART(sp_trpsCurrentLink);

                sp_trpsCurrentLink->maxAvailTxNumber = 0;
            }
            break;

            case TRP_WMODE_REV_LOOPBACK:
            {
                uint8_t err_resp[] = "Reverse loopback content error !\n";
                uint8_t ok_resp[] = "Reverse loopback is successful !\n";

                if (sp_trpsCurrentLink->workModeEn == true)
                {
                    status = APP_TRP_COMMON_CheckMultiLinkFixPatternData(&g_trpsTrafficPriority_t, sp_trpsCurrentLink);

                    if (status != APP_RES_SUCCESS)
                    {
                        APP_UartPut(sizeof(err_resp) - 1, err_resp);
                        sp_trpsCurrentLink->workModeEn = false;
                        APP_TRP_COMMON_SendModeCommand(sp_trpsCurrentLink, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
                    }
                    else if (sp_trpsCurrentLink->rxLastNunber == sp_trpsCurrentLink->lastNumber)
                    {
                        APP_UartPut(sizeof(ok_resp) - 1, ok_resp);
                        sp_trpsCurrentLink->workModeEn = false;
                        sp_trpsCurrentLink->maxAvailTxNumber = 0;
                        APP_TRP_COMMON_SendModeCommand(sp_trpsCurrentLink, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
                    }
                    else 
                        sp_trpsCurrentLink->maxAvailTxNumber = 0;
                    
                }
                else
                    sp_trpsCurrentLink->maxAvailTxNumber = 0;
            }
            break;

            default:
            {
                sp_trpsCurrentLink->maxAvailTxNumber = 0;
            }
            break;
        }

        if ((status == APP_RES_OOM) || (status == APP_RES_INVALID_PARA))
        {
            break;
        }

        // Change to the next link if there is no resource for the dedicated link.
        if (status == APP_RES_NO_RESOURCE)
            sp_trpsCurrentLink->maxAvailTxNumber = 0;

        if (sp_trpsCurrentLink->maxAvailTxNumber == 0)
        {
            sp_trpsCurrentLink->maxAvailTxNumber = APP_TRP_MAX_TX_AVAILABLE_TIMES;
            sp_trpsCurrentLink = APP_TRPS_ChangeNextLink(APP_TRP_LINK_TYPE_TX, &g_trpsTrafficPriority_t);
            checkListFg = true;
        }
        
        if (checkListFg)
        {
            checkListFg = false;
            if ((sp_trpsCurrentLink == p_connList_t) || (sp_trpsCurrentLink == NULL))
                break;
        }
    }
}

APP_TRP_ConnList_T *APP_TRPS_ChangeNextLink(APP_TRP_LINK_TYPE_T linkType, APP_TRP_TrafficPriority_T *p_connToken)
{
    uint8_t i = 0, index = 0;
    

    if (p_connToken == NULL)
        return NULL;
    
    if (linkType == APP_TRP_LINK_TYPE_TX)
    {
        index = p_connToken->txToken;
        
        for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
        {
            index++;
            
            if (index > (APP_TRPS_MAX_LINK_NUMBER - 1))
                index = 0;
            
            if (s_trpsConnList_t[index].connHandle != 0)
                break;
        }
        
        p_connToken->txToken = index;

        if (s_trpsConnList_t[index].connHandle == 0)
        {
            return NULL;
        }
        else
        {
            return &(s_trpsConnList_t[index]);
        }
        
    }
    else if (linkType == APP_TRP_LINK_TYPE_RX)
    {
        index = p_connToken->rxToken;
        
        for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
        {
            index++;
            
            if (index > (APP_TRPS_MAX_LINK_NUMBER - 1))
                index = 0;
            
            if (s_trpsConnList_t[index].connHandle != 0)
                break;
        }
        
        p_connToken->rxToken = index;

        if (s_trpsConnList_t[index].connHandle == 0)
        {
            return NULL;
        }
        else
        {
            return &(s_trpsConnList_t[index]);
        }
        
    }

    return NULL;
}
#else
void APP_TRPS_LeRxProc(APP_TRP_ConnList_T *p_connList_t)
{
    APP_TRPS_LeDataProc(p_connList_t);
}
#endif

void APP_TRPS_EventHandler(BLE_TRSPS_Event_T *p_event)
{
#ifdef APP_BLE_MULTI_LINK_ENABLE
    switch(p_event->eventId)
    {
        case BLE_TRSPS_EVT_CTRL_STATUS:
        {
            sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(p_event->eventField.onCtrlStatus.connHandle);
            
            if (sp_trpsCurrentLink != NULL)
            {
                if (p_event->eventField.onCtrlStatus.status == BLE_TRSPS_STATUS_CTRL_OPENED)
                {
                    sp_trpsCurrentLink->trpRole = APP_TRP_SERVER_ROLE;
                    sp_trpsCurrentLink->channelEn |= APP_TRP_CTRL_CHAN_ENABLE;
                }
                else
                {
                    sp_trpsCurrentLink->trpRole = 0;
                    sp_trpsCurrentLink->channelEn &= APP_TRP_CTRL_CHAN_DISABLE;
                }
            }
        }
            break;
            
        case BLE_TRSPS_EVT_TX_STATUS:
        {
            sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(p_event->eventField.onTxStatus.connHandle);
            
            if (sp_trpsCurrentLink != NULL)
            {
                if (p_event->eventField.onTxStatus.status == BLE_TRSPS_STATUS_TX_OPENED)
                {
                    if (!(sp_trpsCurrentLink->channelEn & APP_TRP_DATA_CHAN_ENABLE))
                    {
                        uint8_t txPhys = 0, rxPhys = 0, phyOptions = 0, connLinkNum = 0;


                        connLinkNum = APP_GetConnLinkNum();
                        //Update Phy
                        #ifdef APP_BLE_CODED_PHY_ENABLE
                        txPhys = BLE_GAP_PHY_OPTION_CODED;
                        rxPhys = BLE_GAP_PHY_OPTION_CODED;
                        #ifdef APP_BLE_CODED_S2_ENABLE
                        phyOptions = BLE_GAP_PHY_PREF_S2;
                        #else
                        phyOptions = BLE_GAP_PHY_PREF_S8;
                        #endif
                        #else
                        txPhys = BLE_GAP_PHY_OPTION_2M;
                        rxPhys = BLE_GAP_PHY_OPTION_2M;
                        // phyOptions = 0;
                        #endif
                        BLE_GAP_SetPhy(p_event->eventField.onTxStatus.connHandle, txPhys, rxPhys, phyOptions);
#ifndef APP_MIDDLEWARE_ENABLE
                        //Update connection parameters
                        BLE_GAP_ConnParams_T params;

                        #ifdef APP_BLE_CODED_PHY_ENABLE
                        params.intervalMax = 0x50;  //100ms
                        params.intervalMin = 0x50;  //100ms
                        params.latency = 0;
                        params.timeout = 0x168;  //3600ms
                        #else
                        params.intervalMax = 0x10;  //20ms
                        params.intervalMin = 0x10;  //20ms
                        params.latency = 0;
                        params.supervisionTimeout = 0x48;  //720ms
                        #endif

                        if (connLinkNum >= 2)
                        {
                            params.intervalMax = 0x18;  //30ms
                            params.intervalMin = 0x18;  //30ms
                            //params.timeout = 0xC8;  //2000ms
                        }

                        BLE_GAP_UpdateConnParam(p_event->eventField.onTxStatus.connHandle, &params);
#else
                        BLE_DM_ConnParamUpdate_T params;

                        #ifdef APP_BLE_CODED_PHY_ENABLE
                        params.intervalMax = 0x50;  //100ms
                        params.intervalMin = 0x50;  //100ms
                        params.latency = 0;
                        params.timeout = 0x168;  //3600ms
                        #else
                        params.intervalMax = 0x10;  //20ms
                        params.intervalMin = 0x10;  //20ms
                        params.latency = 0;
                        params.timeout = 0x48;  //720ms
                        #endif

                        if (connLinkNum >= 2)
                        {
                            params.intervalMax = 0x18;  //30ms
                            params.intervalMin = 0x18;  //30ms
                            //params.timeout = 0xC8;  //2000ms
                        }

                        BLE_DM_ConnectionParameterUpdate(p_event->eventField.onTxStatus.connHandle, &params);
#endif
                    }

                    sp_trpsCurrentLink->channelEn |= APP_TRP_DATA_CHAN_ENABLE;
                    sp_trpsCurrentLink->type = APP_TRP_TYPE_LEGACY;
                #ifdef APP_DEMO_MODE_LED_ENABLE
                    g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS);
                #endif
                }
                else
                {
                    sp_trpsCurrentLink->channelEn &= APP_TRP_DATA_CHAN_DISABLE;

                    if (sp_trpsCurrentLink->channelEn & APP_TRCBP_DATA_CHAN_ENABLE)
                    {
                        sp_trpsCurrentLink->type = APP_TRP_TYPE_TRCBP;
                    #ifdef APP_DEMO_MODE_LED_ENABLE
                        g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS);
                    #endif
                    }
                    else
                    {
                        sp_trpsCurrentLink->type = APP_TRP_TYPE_UNKNOWN;
                    }
                }
#ifdef APP_UART_CMD_SET_ENABLE
                APP_SendTrpConnEvt(p_event->eventField.onTxStatus.connHandle, BLE_TRP_ROLE_SERVER,
                    p_event->eventField.onTxStatus.status);
#endif
                if (s_defaultUartEn)
                    sp_trpsCurrentLink->workMode = TRP_WMODE_UART;
            }
        }
            break;

        case BLE_TRSPS_EVT_RECEIVE_DATA:
        {
            sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(p_event->eventField.onReceiveData.connHandle);
            
            if (sp_trpsCurrentLink != NULL)
            {
                if (sp_trpsCurrentLink->workMode == TRP_WMODE_NULL)
                {
                    APP_TRP_COMMON_FreeLeData(sp_trpsCurrentLink);
                    break;
                }
                
                APP_TRPS_LeRxMultiLinkProc(sp_trpsCurrentLink);
            }
        }
        break;

        case BLE_TRSPS_EVT_CBFC_ENABLED:
        {
            //if (sp_trpsCurrentLink->connHandle == p_event->eventField.onCbfcEnabled.connHandle)
        }
        break;

        case BLE_TRSPS_EVT_CBFC_CREDIT:
        {
            //if (s_trpsConnList_t.connHandle == p_event->eventField.onCbfcEnabled.connHandle)
                //APP_TRPS_TxBufValidEvtProc();
                //APP_LOG_DEBUG("CBCF credit ! \n");
        }
        break;
        
        case BLE_TRSPS_EVT_VENDOR_CMD:
        {            
            sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(p_event->eventField.onVendorCmd.connHandle);
            
            if ((sp_trpsCurrentLink != NULL) && (p_event->eventField.onVendorCmd.p_payLoad[0] == APP_TRP_VENDOR_OPCODE_BLE_UART))
            {
                APP_TRPS_VendorCmdProc(sp_trpsCurrentLink, p_event->eventField.onVendorCmd.p_payLoad);
            }
        }
        break;

        default:
            break;
    }
#else
    switch(p_event->eventId)
    {
        case BLE_TRSPS_EVT_CTRL_STATUS:
        {
            if (s_trpsConnList_t.connHandle == p_event->eventField.onCtrlStatus.connHandle)
            {
                if (p_event->eventField.onCtrlStatus.status == BLE_TRSPS_STATUS_CTRL_OPENED)
                {
                    s_trpsConnList_t.trpRole = APP_TRP_SERVER_ROLE;
                    s_trpsConnList_t.channelEn |= APP_TRP_CTRL_CHAN_ENABLE;
                }
                else
                {
                    s_trpsConnList_t.trpRole = 0;
                    s_trpsConnList_t.channelEn &= APP_TRP_CTRL_CHAN_DISABLE;
                }
            }
        }
            break;
            
        case BLE_TRSPS_EVT_TX_STATUS:
        {
            if (s_trpsConnList_t.connHandle == p_event->eventField.onTxStatus.connHandle)
            {
                if (p_event->eventField.onTxStatus.status == BLE_TRSPS_STATUS_TX_OPENED)
                {
                    s_trpsConnList_t.channelEn |= APP_TRP_DATA_CHAN_ENABLE;
                    s_trpsConnList_t.type = APP_TRP_TYPE_LEGACY;
                #ifdef APP_DEMO_MODE_LED_ENABLE
                    g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS);
                #endif
                }
                else
                {
                    s_trpsConnList_t.channelEn &= APP_TRP_DATA_CHAN_DISABLE;

                    if (s_trpsConnList_t.channelEn & APP_TRCBP_DATA_CHAN_ENABLE)
                    {
                        s_trpsConnList_t.type = APP_TRP_TYPE_TRCBP;
                    #ifdef APP_DEMO_MODE_LED_ENABLE
                        g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_TRPS);
                    #endif
                    }
                    else
                    {
                        s_trpsConnList_t.type = APP_TRP_TYPE_UNKNOWN;
                    }
                }
#ifdef APP_UART_CMD_SET_ENABLE
                APP_SendTrpConnEvt(p_event->eventField.onTxStatus.connHandle, BLE_TRP_ROLE_SERVER,
                    p_event->eventField.onTxStatus.status);
#endif
                if (s_defaultUartEn)
                    s_trpsConnList_t.workMode = TRP_WMODE_UART;
            }
        }
            break;

        case BLE_TRSPS_EVT_RECEIVE_DATA:
        {
            if (s_trpsConnList_t.connHandle == p_event->eventField.onReceiveData.connHandle)
            {
                if (s_trpsConnList_t.workMode == TRP_WMODE_NULL)
                {
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
                //APP_LOG_DEBUG("CBCF credit ! \n");
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
#endif //APP_BLE_MULTI_LINK_ENABLE
}

void APP_TRPS_TrcbpsEventHandler(BLE_TRCBPS_Event_T *p_event)
{
#ifdef APP_BLE_MULTI_LINK_ENABLE
    switch(p_event->eventId)
    {
        case BLE_TRCBPS_EVT_CONNECTION_STATUS:
        {
            sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(p_event->eventField.connStatus.connHandle);
            
            if (sp_trpsCurrentLink == NULL)
                break;
            if (p_event->eventField.connStatus.status == BLE_TRCBPS_STATUS_CONNECTED)
            {
                //if (sp_trpsCurrentLink != NULL)
                {
                    if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_CTRL_CHAN)
                    {
                        sp_trpsCurrentLink->channelEn |= APP_TRCBP_CTRL_CHAN_ENABLE;
                    }
                    else if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_DATA_CHAN)
                    {
                        //TODO:
                        //sp_trpsCurrentLink->fixPattTrcbpMtu = p_event->eventField.connStatus.peerMtu - BLE_TRCBPS_SDU_LEN_FIELD_LEN;
                        sp_trpsCurrentLink->fixPattTrcbpMtu = BLE_ATT_MAX_MTU_LEN - BLE_TRCBPS_SDU_LEN_FIELD_LEN;
                        sp_trpsCurrentLink->channelEn |= APP_TRCBP_DATA_CHAN_ENABLE;
                        sp_trpsCurrentLink->type = APP_TRP_TYPE_TRCBP;
                    }
                }
            }
            else if (p_event->eventField.connStatus.status == BLE_TRCBPS_STATUS_STANDBY)
            {
                //if (sp_trpsCurrentLink != NULL)
                {
                    if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_CTRL_CHAN)
                    {
                        sp_trpsCurrentLink->channelEn &= APP_TRCBP_CTRL_CHAN_DISABLE;
                    }
                    else if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_DATA_CHAN)
                    {
                        sp_trpsCurrentLink->fixPattTrcbpMtu = 0;
                        sp_trpsCurrentLink->channelEn &= APP_TRCBP_DATA_CHAN_DISABLE;

                        if (sp_trpsCurrentLink->channelEn & APP_TRP_DATA_CHAN_ENABLE)
                        {
                            sp_trpsCurrentLink->type = APP_TRP_TYPE_LEGACY;
                        }
                        else
                        {
                            sp_trpsCurrentLink->type = APP_TRP_TYPE_UNKNOWN;
                        }
                    }
                }
            }
        }
        break;

        case BLE_TRCBPS_EVT_RECEIVE_DATA:
        {
            sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(p_event->eventField.onReceiveData.connHandle);
            
            if ((sp_trpsCurrentLink != NULL) && (p_event->eventField.onReceiveData.chanType == BLE_TRCBPS_DATA_CHAN))
            {
                if (sp_trpsCurrentLink->workMode == TRP_WMODE_NULL)
                {
                    APP_TRP_COMMON_FreeLeData(sp_trpsCurrentLink);
                    break;
                }
                
                APP_TRPS_LeRxMultiLinkProc(sp_trpsCurrentLink);
            }
        }
        break;

        case BLE_TRCBPS_EVT_VENDOR_CMD:
        {
            sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(p_event->eventField.onVendorCmd.connHandle);
            
            if ((sp_trpsCurrentLink != NULL) && (p_event->eventField.onVendorCmd.p_payLoad[0] == APP_TRP_VENDOR_OPCODE_BLE_UART))
            {
                //Parsing vendor command
                APP_TRPS_VendorCmdProc(sp_trpsCurrentLink, p_event->eventField.onVendorCmd.p_payLoad);
            }
        }
        break;

        default:
            break;
    }
#else
    switch(p_event->eventId)
    {
        case BLE_TRCBPS_EVT_CONNECTION_STATUS:
        {
            if (p_event->eventField.connStatus.status == BLE_TRCBPS_STATUS_CONNECTED)
            {
                if (p_event->eventField.connStatus.connHandle == s_trpsConnList_t.connHandle)
                {
                    if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_CTRL_CHAN)
                    {
                        s_trpsConnList_t.channelEn |= APP_TRCBP_CTRL_CHAN_ENABLE;
                    }
                    else if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_DATA_CHAN)
                    {
                        //TODO:
                        //s_trpsConnList_t.fixPattTrcbpMtu = p_event->eventField.connStatus.peerMtu - BLE_TRCBPS_SDU_LEN_FIELD_LEN;
                        s_trpsConnList_t.fixPattTrcbpMtu = BLE_ATT_MAX_MTU_LEN - BLE_TRCBPS_SDU_LEN_FIELD_LEN;
                        s_trpsConnList_t.channelEn |= APP_TRCBP_DATA_CHAN_ENABLE;
                        s_trpsConnList_t.type = APP_TRP_TYPE_TRCBP;
                    }
                }
            }
            else if (p_event->eventField.connStatus.status == BLE_TRCBPS_STATUS_STANDBY)
            {
                if (p_event->eventField.connStatus.connHandle == s_trpsConnList_t.connHandle)
                {
                    if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_CTRL_CHAN)
                    {
                        s_trpsConnList_t.channelEn &= APP_TRCBP_CTRL_CHAN_DISABLE;
                    }
                    else if (p_event->eventField.connStatus.chanType == BLE_TRCBPS_DATA_CHAN)
                    {
                        s_trpsConnList_t.fixPattTrcbpMtu = 0;
                        s_trpsConnList_t.channelEn &= APP_TRCBP_DATA_CHAN_DISABLE;

                        if (s_trpsConnList_t.channelEn & APP_TRP_DATA_CHAN_ENABLE)
                        {
                            s_trpsConnList_t.type = APP_TRP_TYPE_LEGACY;
                        }
                        else
                        {
                            s_trpsConnList_t.type = APP_TRP_TYPE_UNKNOWN;
                        }
                    }
                }
            }
        }
        break;

        case BLE_TRCBPS_EVT_RECEIVE_DATA:
        {
            if ((p_event->eventField.onReceiveData.connHandle == s_trpsConnList_t.connHandle) && 
                (p_event->eventField.onReceiveData.chanType == BLE_TRCBPS_DATA_CHAN))
            {
                if (s_trpsConnList_t.workMode == TRP_WMODE_NULL)
                {
                    APP_TRP_COMMON_FreeLeData(&s_trpsConnList_t);
                    break;
                }
                APP_TRPS_LeRxProc(&s_trpsConnList_t);
            }
        }
        break;

        case BLE_TRCBPS_EVT_VENDOR_CMD:
        {
            if ((p_event->eventField.onVendorCmd.connHandle == s_trpsConnList_t.connHandle) &&
                (p_event->eventField.onVendorCmd.p_payLoad[0] == APP_TRP_VENDOR_OPCODE_BLE_UART))
            {
                //Parsing vendor command
                APP_TRPS_VendorCmdProc(&s_trpsConnList_t, p_event->eventField.onVendorCmd.p_payLoad);
            }
        }
        break;

        default:
            break;
    }
#endif  //#ifdef APP_BLE_MULTI_LINK_ENABLE
}

void APP_TRPS_Init(void)
{
#ifdef APP_BLE_MULTI_LINK_ENABLE
    uint8_t i;

    for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
    {
        memset((uint8_t *)(&s_trpsConnList_t[i]), 0, sizeof(APP_TRP_ConnList_T));
        s_trpsConnList_t[i].maxAvailTxNumber = APP_TRP_MAX_TX_AVAILABLE_TIMES;
    }
    g_trpsTrafficPriority_t.validNumber = APP_TRP_MAX_TRANSMIT_NUM;
#else
    memset((uint8_t *) &s_trpsConnList_t, 0, sizeof(APP_TRP_ConnList_T));
#endif

#ifdef APP_UART_CMD_SET_ENABLE
    s_defaultUartEn = 0;
#else
    s_defaultUartEn = 1;
#endif
    
}

void APP_TRPS_ConnEvtProc(BLE_GAP_Event_T *p_event)
{
#ifdef APP_BLE_MULTI_LINK_ENABLE
    uint8_t i;

    for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
    {
        if (s_trpsConnList_t[i].connHandle == 0)
        {
            s_trpsConnList_t[i].connHandle = p_event->eventField.evtConnect.connHandle;
            sp_trpsCurrentLink = &s_trpsConnList_t[i];
            g_trpServerLinkNum++;
            g_trpUartQueueNum = APP_TRP_SET_QUEUE_NUMBER(g_trpClientLinkNum, g_trpServerLinkNum);
            break;
        }
    }
    
    if ((i == APP_TRPS_MAX_LINK_NUMBER) || APP_TRP_IS_OVER_CONN_MAX_DEV)
        BLE_GAP_Disconnect(p_event->eventField.evtConnect.connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
    
#else
    s_trpsConnList_t.connHandle = p_event->eventField.evtConnect.connHandle;
#endif
}

void APP_TRPS_DiscEvtProc(uint16_t connHandle)
{
#ifdef APP_BLE_MULTI_LINK_ENABLE
    sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(connHandle);
    
    if (sp_trpsCurrentLink != NULL)
    {
        APP_TRP_COMMON_DelAllLeCircData(&(sp_trpsCurrentLink->leCircQueue_t));
        APP_TRP_COMMON_DelAllCircData(&(sp_trpsCurrentLink->uartCircQueue_t));
        memset(sp_trpsCurrentLink, 0, sizeof(APP_TRP_ConnList_T));
        if (g_trpServerLinkNum)
        {
            g_trpServerLinkNum--;
            g_trpUartQueueNum = APP_TRP_SET_QUEUE_NUMBER(g_trpClientLinkNum, g_trpServerLinkNum);
        }
    }
#else
    if (s_trpsConnList_t.connHandle == connHandle)
    {
        APP_TRP_COMMON_DelAllLeCircData(&(s_trpsConnList_t.leCircQueue_t));
        APP_TRP_COMMON_DelAllCircData(&(s_trpsConnList_t.uartCircQueue_t));
        memset((uint8_t *) &s_trpsConnList_t, 0, sizeof(APP_TRP_ConnList_T));
    }
#endif
}

void APP_TRPS_SendUpConnParaStatusToClient(uint16_t connHandle, uint8_t upConnParaStatus)
{
#ifdef APP_BLE_MULTI_LINK_ENABLE
    sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(connHandle);
    
    if(sp_trpsCurrentLink != NULL)
    {
        uint8_t grpId = TRP_GRPID_UPDATE_CONN_PARA;
        uint8_t commandId = APP_TRP_WMODE_SNED_UP_CONN_STATUS;

        sp_trpsCurrentLink->upConnParaStatus = upConnParaStatus;
        APP_TRP_COMMON_SendUpConnParaStatus(sp_trpsCurrentLink, grpId, commandId, upConnParaStatus);
    }
#else
    if (s_trpsConnList_t.connHandle == connHandle)
    {
        uint8_t grpId = TRP_GRPID_UPDATE_CONN_PARA;
        uint8_t commandId = APP_TRP_WMODE_SNED_UP_CONN_STATUS;

        s_trpsConnList_t.upConnParaStatus = upConnParaStatus;        
        APP_TRP_COMMON_SendUpConnParaStatus(&s_trpsConnList_t, grpId, commandId, upConnParaStatus);
    }
#endif
}

#ifdef APP_BLE_MULTI_LINK_ENABLE
void APP_TRPS_TxBufValidEvtProc(void)
{
    APP_TRP_ConnList_T  *p_trpsTxLeLink = NULL;
    uint16_t status = APP_RES_SUCCESS;
    uint8_t checkListFg = false;



    status = APP_TRPS_ReSendCommand();
    sp_trpsCurrentLink = APP_TRPS_GetConnListByIndex(g_trpsTrafficPriority_t.txToken);
    
    if (sp_trpsCurrentLink == NULL)
        sp_trpsCurrentLink = APP_TRPS_ChangeNextLink(APP_TRP_LINK_TYPE_TX, &g_trpsTrafficPriority_t);

    if(sp_trpsCurrentLink == NULL)
        return;
    
    p_trpsTxLeLink = sp_trpsCurrentLink;
    g_trpsTrafficPriority_t.validNumber = APP_TRP_MAX_TRANSMIT_NUM;

    while (1)
    {
        if ((sp_trpsCurrentLink->workMode == TRP_WMODE_LOOPBACK) && (sp_trpsCurrentLink->workModeEn))
        {
            if (sp_trpsCurrentLink->maxAvailTxNumber > 0)
            {
                status = APP_TRP_COMMON_SendMultiLinkLeDataTrpProfile(&g_trpsTrafficPriority_t, sp_trpsCurrentLink);
                
                if (status == APP_RES_OOM)
                {
                    break;
                }
                else if ((status == APP_RES_INVALID_PARA) || (status == APP_RES_NO_RESOURCE))
                {
                    sp_trpsCurrentLink->maxAvailTxNumber = 0;
                }
            }
        }
        else if ((sp_trpsCurrentLink->workMode == TRP_WMODE_FIX_PATTERN) && (sp_trpsCurrentLink->workModeEn == true))
        {
            if (sp_trpsCurrentLink->maxAvailTxNumber > 0)
            {
                status = APP_TRP_COMMON_SendMultiLinkFixPattern(&g_trpsTrafficPriority_t, sp_trpsCurrentLink);
                
                if (status & APP_RES_COMPLETE)
                {
                    APP_TRP_COMMON_SendModeCommand(sp_trpsCurrentLink, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
                    APP_TRP_COMMON_SendLastNumber(sp_trpsCurrentLink);
                    sp_trpsCurrentLink->workModeEn = false;
                    break;
                }

                if (status != APP_RES_SUCCESS)
                {
                  sp_trpsCurrentLink->maxAvailTxNumber = 0;
                }
            }
        }
        else if ((sp_trpsCurrentLink->workMode == TRP_WMODE_UART) && (sp_trpsCurrentLink->uartCircQueue_t.usedNum > 0))
        {
            // Send data to transparent profile
            status = APP_TRP_COMMON_SendLeDataUartCircQueue(sp_trpsCurrentLink);
            
            if (status == APP_RES_OOM)
            {
                break;
            }
            sp_trpsCurrentLink->maxAvailTxNumber = 0;
        }
        else if ((sp_trpsCurrentLink->workMode == TRP_WMODE_REV_LOOPBACK) && (sp_trpsCurrentLink->workModeEn == true))
        {
            if ((sp_trpsCurrentLink->maxAvailTxNumber > 0) && (sp_trpsCurrentLink->fixPattMaxSize > 0))
            {
                status = APP_TRP_COMMON_SendMultiLinkFixPattern(&g_trpsTrafficPriority_t, sp_trpsCurrentLink);

                if ((status != APP_RES_SUCCESS) || (status & APP_RES_COMPLETE))
                {
                  sp_trpsCurrentLink->maxAvailTxNumber = 0;
                }
            }
            else
                sp_trpsCurrentLink->maxAvailTxNumber = 0;
        }
        else
        {
            sp_trpsCurrentLink->maxAvailTxNumber = 0;
        }
        
        if (g_trpsTrafficPriority_t.validNumber == 0)
            break;

        //Tx available = 0 need to change link.
        if (sp_trpsCurrentLink->maxAvailTxNumber == 0)
        {
            sp_trpsCurrentLink->maxAvailTxNumber = APP_TRP_MAX_TX_AVAILABLE_TIMES;
            sp_trpsCurrentLink = APP_TRPS_ChangeNextLink(APP_TRP_LINK_TYPE_TX, &g_trpsTrafficPriority_t);
            checkListFg = true;
        }

        if (checkListFg)
        {
            checkListFg = false;
            if ((p_trpsTxLeLink == sp_trpsCurrentLink) || (sp_trpsCurrentLink == NULL))
                break;
        }
    }
}
#else
void APP_TRPS_TxBufValidEvtProc(void)
{
    uint16_t status;
    uint8_t grpId[] = {TRP_GRPID_NULL, TRP_GRPID_CHECK_SUM, TRP_GRPID_LOOPBACK, TRP_GRPID_FIX_PATTERN};
    
    if ((s_trpsConnList_t.workMode == TRP_WMODE_LOOPBACK) && (s_trpsConnList_t.leCircQueue_t.usedNum > 0))
    {
        s_trpsConnList_t.srcType = APP_TRP_SRC_TYPE_LE;
        status = APP_TRP_COMMON_SendLeDataCircQueue(&s_trpsConnList_t);
    }
    else if (s_trpsConnList_t.workMode == TRP_WMODE_UART)
    {
        // Send data to transparent profile
        if (s_trpsConnList_t.uartCircQueue_t.usedNum > 0)
        {
            APP_TRP_COMMON_SendLeDataUartCircQueue(&s_trpsConnList_t);
        }
    }
    else if ((s_trpsConnList_t.workMode ==  TRP_WMODE_FIX_PATTERN) && (s_trpsConnList_t.workModeEn == true))
    {
        status = APP_TRP_COMMON_SendFixPattern(&s_trpsConnList_t);
        if (status & APP_RES_COMPLETE)
        {
            APP_TRP_COMMON_SendModeCommand(&s_trpsConnList_t, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
            APP_TRP_COMMON_SendLastNumber(&s_trpsConnList_t);
            s_trpsConnList_t.workModeEn = false;
        }
    }
    else if (s_trpsConnList_t.workMode ==  TRP_WMODE_FIX_PATTERN)
    {
        if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_GID_TX_FAIL)
        {
            APP_TRP_COMMON_SendModeCommand(&s_trpsConnList_t, TRP_GRPID_TRANSMIT, APP_TRP_WMODE_TX_DATA_END);
        }
        if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_LAST_NUMBER_FAIL)
        {
            APP_TRP_COMMON_SendLastNumber(&s_trpsConnList_t);
        }
    }
    else if ((s_trpsConnList_t.workMode ==  TRP_WMODE_CHECK_SUM) 
        && (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_CHECK_SUM_FAIL))
    {
        APP_TRP_COMMON_SendCheckSumCommand(&s_trpsConnList_t);
    }
    else if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_ERROR_RSP_FAIL)
    {
        if ((grpId[s_trpsConnList_t.workMode] > TRP_GRPID_NULL) 
            && (grpId[s_trpsConnList_t.workMode] < TRP_GRPID_UART))
        {
            APP_TRP_COMMON_SendErrorRsp(&s_trpsConnList_t, grpId[s_trpsConnList_t.workMode]);
        }
    }
    else if (s_trpsConnList_t.txCtrRspFg & APP_TRP_SEND_STATUS_FLAG)
    {
        APP_TRP_COMMON_SendUpConnParaStatus(&s_trpsConnList_t, TRP_GRPID_UPDATE_CONN_PARA, APP_TRP_WMODE_SNED_UP_CONN_STATUS, s_trpsConnList_t.upConnParaStatus);
    }
    
}
#endif

void APP_TRPS_UpdateMtuEvtProc(uint16_t connHandle, uint16_t exchangedMTU)
{
    #ifdef APP_BLE_MULTI_LINK_ENABLE
    sp_trpsCurrentLink = APP_TRPS_GetConnListByConnHandle(connHandle);
    
    if(sp_trpsCurrentLink != NULL)
    {
        sp_trpsCurrentLink->exchangedMTU = exchangedMTU;
        sp_trpsCurrentLink->txMTU = sp_trpsCurrentLink->exchangedMTU - ATT_HANDLE_VALUE_HEADER_SIZE;
    }
    #else
    if (s_trpsConnList_t.connHandle == connHandle)
    {
        s_trpsConnList_t.exchangedMTU = exchangedMTU;
        s_trpsConnList_t.txMTU = s_trpsConnList_t.exchangedMTU - ATT_HANDLE_VALUE_HEADER_SIZE;
    }
    #endif
}

// uint8_t APP_TRPS_GetWorkMode(void)
// {
//     return s_trpsConnList_t.workMode;
// }

bool APP_TRPS_IsUartMode(void)
{
    #ifdef APP_BLE_MULTI_LINK_ENABLE
    uint8_t i;

    for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
    {
        if (s_trpsConnList_t[i].workMode == TRP_WMODE_UART)
            return true;
    }

    return false;
    #else
    if (s_trpsConnList_t.workMode == TRP_WMODE_UART)
        return true;
    else
        return false;
    #endif
}

APP_TRP_ConnList_T *APP_TRPS_GetConnListByUartMode(void)
{
#ifdef APP_BLE_MULTI_LINK_ENABLE
    uint8_t i;

    for (i = 0; i < APP_TRPS_MAX_LINK_NUMBER; i++)
    {
        if (s_trpsConnList_t[i].workMode == TRP_WMODE_UART)
            return &(s_trpsConnList_t[i]);
    }

    return NULL;
#else
    if (s_trpsConnList_t.workMode == TRP_WMODE_UART)
        return &s_trpsConnList_t;
    else
        return NULL;
#endif
}

void APP_TRPS_KeyTrigger(uint8_t keyMesg)
{
    APP_LOG_INFO("TRPS: keyMesg=%d\n", keyMesg);

#ifdef APP_BLE_MULTI_LINK_ENABLE
    APP_TRP_ConnList_T *p_connList_t = NULL;

    if (keyMesg == APP_KEY_MSG_SHORT_PRESS)
    {
        if ((g_trpServerLinkNum < APP_TRPS_MAX_LINK_NUMBER) && !(APP_TRP_IS_CONN_MAX_DEV))
            APP_ADV_Start();
    }
    else if (keyMesg == APP_KEY_MSG_LONG_PRESS)
    {
        p_connList_t = APP_TRPS_GetSamllestHandleConnList();

        if (p_connList_t != NULL)
        {
            BLE_GAP_Disconnect(p_connList_t->connHandle, GAP_DISC_REASON_REMOTE_TERMINATE);
        }
    }
    else if (keyMesg == APP_KEY_MSG_DOUBLE_CLICK)
    {
        p_connList_t = APP_TRPS_GetSamllestHandleConnList();

        if ((p_connList_t != NULL) && (p_connList_t->workModeEn == false))
        {
            APP_TRP_COMMON_SendModeCommand(p_connList_t, TRP_GRPID_REV_LOOPBACK, APP_TRP_WMODE_REV_LOOPBACK_ENABLE);
            p_connList_t->workMode = TRP_WMODE_REV_LOOPBACK;
            p_connList_t->workModeEn = false;
            p_connList_t->txCtrRspFg = 0;
            g_enableUartMode &= ~APP_TRP_SERVER_UART;
        }
    }
#endif
}
