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
  Application BLE Profile Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ancs_handler.c

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
#include <stdio.h>
#include "app_ancs_handler.h"
#include "app_uart.h"
#include "app_module.h"
#include "ble_dm/ble_dm.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

static char s_strBuf[512];
// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
void APP_AncsEvtHandler(BLE_ANCS_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_ANCS_EVT_DISC_COMPLETE_IND:
        {
            /* TODO: implement your application code.*/
            g_GattClientInfo.bDiscoveryDone= true;
        }
        break;

        case BLE_ANCS_EVT_ERR_ATTR_BUF_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_ANCS_EVT_ERR_RECOMPOSE_BUF_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_ANCS_EVT_NTFY_ADDED_IND:
        {
            /* TODO: implement your application code.*/
            sprintf(s_strBuf,
                    "BLE_ANCS_EVT_NTFY_ADDED_IND.\r\n"
                    "connHandle: 0x%.4X\r\n"
                    "NotificationUID: 0x%.8X\r\n"
                    "EventFlags:\r\n"
                    "  silent: %d\r\n"
                    "  important: %d\r\n"
                    "  preExisting: %d\r\n"
                    "  positiveAction: %d\r\n"
                    "  negativeAction: %d\r\n"
                    "CategoryID: 0x%.2X\r\n"
                    "CategoryCount: 0x%.2X\r\n"
                    "-> Press Button 1 to Get Notification Attributes:\r\n",
                    p_event->eventField.evtNtfyInd.connHandle,
                    (unsigned int)p_event->eventField.evtNtfyInd.ntfyId,
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.silent ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.important ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.preExisting ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.positiveAction ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.negativeAction ? 1: 0),
                    p_event->eventField.evtNtfyInd.categoryId,
                    p_event->eventField.evtNtfyInd.categoryCount);
            APP_UartMsg(s_strBuf);

            g_NtfyInfo.bNtfyAdded=true;
            g_NtfyInfo.ntfyId = p_event->eventField.evtNtfyInd.ntfyId;
            g_NtfyInfo.ntfyEvtFlagMask = p_event->eventField.evtNtfyInd.ntfyEvtFlagMask;
            g_NtfyInfo.state = APP_ANCS_IND_NTFY_ADDED;

        } 
        break;

        case BLE_ANCS_EVT_NTFY_MODIFIED_IND:  
        {
            /* TODO: implement your application code.*/
            sprintf(s_strBuf,
                    "BLE_ANCS_EVT_NTFY_MODIFIED_IND.\r\n"
                    "connHandle: 0x%.4X\r\n"
                    "NotificationUID: 0x%.8X\r\n"
                    "EventFlags:\r\n"
                    "  silent: %d\r\n"
                    "  important: %d\r\n"
                    "  preExisting: %d\r\n"
                    "  positiveAction: %d\r\n"
                    "  negativeAction: %d\r\n"
                    "CategoryID: 0x%.2X\r\n"
                    "CategoryCount: 0x%.2X\r\n",
                    p_event->eventField.evtNtfyInd.connHandle,
                    (unsigned int)p_event->eventField.evtNtfyInd.ntfyId,
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.silent ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.important ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.preExisting ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.positiveAction ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.negativeAction ? 1: 0),
                    p_event->eventField.evtNtfyInd.categoryId,
                    p_event->eventField.evtNtfyInd.categoryCount);
            APP_UartMsg(s_strBuf);

            g_NtfyInfo.ntfyId = p_event->eventField.evtNtfyInd.ntfyId;
            g_NtfyInfo.ntfyEvtFlagMask = p_event->eventField.evtNtfyInd.ntfyEvtFlagMask;
        }
        break;

        case BLE_ANCS_EVT_NTFY_REMOVED_IND:  
        {
            /* TODO: implement your application code.*/
            sprintf(s_strBuf,
                    "BLE_ANCS_EVT_NTFY_REMOVED_IND.\r\n"
                    "connHandle: 0x%.4X\r\n"
                    "NotificationUID: 0x%.8X\r\n"
                    "EventFlags:\r\n"
                    "  silent: %d\r\n"
                    "  important: %d\r\n"
                    "  preExisting: %d\r\n"
                    "  positiveAction: %d\r\n"
                    "  negativeAction: %d\r\n"
                    "CategoryID: 0x%.2X\r\n"
                    "CategoryCount: 0x%.2X\r\n",
                    p_event->eventField.evtNtfyInd.connHandle,
                    (unsigned int)p_event->eventField.evtNtfyInd.ntfyId,
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.silent ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.important ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.preExisting ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.positiveAction ? 1: 0),
                    (p_event->eventField.evtNtfyInd.ntfyEvtFlagMask.negativeAction ? 1: 0),
                    p_event->eventField.evtNtfyInd.categoryId,
                    p_event->eventField.evtNtfyInd.categoryCount);
            APP_UartMsg(s_strBuf);

            g_NtfyInfo.bNtfyAdded=false;
            g_NtfyInfo.ntfyId = p_event->eventField.evtNtfyInd.ntfyId;
            g_NtfyInfo.ntfyEvtFlagMask = p_event->eventField.evtNtfyInd.ntfyEvtFlagMask;
            g_NtfyInfo.state = APP_ANCS_IND_NTFY_REMOVED;
        }
        break;

        case BLE_ANCS_EVT_NTFY_ATTR_IND:
        {
            /* TODO: implement your application code.*/
            BLE_ANCS_DecodeNtfyAttrs_T  *p_data;

            sprintf(s_strBuf,
                    "BLE_ANCS_EVT_NTFY_ATTR_IND.\r\n"
                    "connHandle: 0x%.4X\r\n"
                    "NotificationUID: 0x%.8X\r\n"
                    "bitmask:\r\n"
                    "  appId: %s\r\n"
                    "  title: %s\r\n"
                    "  subtitle: %s\r\n"
                    "  msg: %s\r\n"
                    "  msgSize: %d\r\n"
                    "  date: %s\r\n"
                    "  positiveAction: %s\r\n"
                    "  negativeAction: %s\r\n"
                    "-> Press Button 1 to Get App Attributes:\r\n",
                    p_event->eventField.evtNtfyAttrInd.connHandle,
                    (unsigned int)p_event->eventField.evtNtfyAttrInd.p_data->ntfyId,
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.appId ? (char *)p_event->eventField.evtNtfyAttrInd.p_data->appId: ""),
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.title ? (char *)p_event->eventField.evtNtfyAttrInd.p_data->p_title: ""),
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.subtitle ? (char *)p_event->eventField.evtNtfyAttrInd.p_data->p_subtitle: ""),
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.msg ? (char *)p_event->eventField.evtNtfyAttrInd.p_data->p_msg: ""),
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.msgSize ? p_event->eventField.evtNtfyAttrInd.p_data->msgSize: 0),
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.date ? (char *)p_event->eventField.evtNtfyAttrInd.p_data->date: ""),
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.positiveAction ? (char *)p_event->eventField.evtNtfyAttrInd.p_data->positiveAction: ""),
                    (p_event->eventField.evtNtfyAttrInd.p_data->bitmask.negativeAction ? (char *)p_event->eventField.evtNtfyAttrInd.p_data->negativeAction: ""));
            APP_UartMsg(s_strBuf);

            p_data=p_event->eventField.evtNtfyAttrInd.p_data;
            g_NtfyInfo.ntfyAttrBitMask=p_data->bitmask;
            g_NtfyInfo.state = APP_ANCS_IND_NTFY_ATTR;

            if (p_data->bitmask.appId)
            {
                memcpy(g_NtfyInfo.appId, p_data->appId, BLE_ANCS_MAX_APPID_LEN);
            }
            if (p_data->bitmask.positiveAction)
            {
                memcpy(g_NtfyInfo.positiveAction, p_data->positiveAction, BLE_ANCS_MAX_POS_ACTION_LABEL_LEN);
            }
            if (p_data->bitmask.negativeAction)
            {
                memcpy(g_NtfyInfo.negativeAction, p_data->negativeAction, BLE_ANCS_MAX_NEG_ACTION_LABEL_LEN);
            }
        }
        break;

        case BLE_ANCS_EVT_APP_ATTR_IND:
        {
            /* TODO: implement your application code.*/
            sprintf(s_strBuf,
                    "BLE_ANCS_EVT_APP_ATTR_IND.\r\n"
                    "connHandle: 0x%.4X\r\n"
                    "appId: %s\r\n"
                    "displayName: %s\r\n"
                    "->%s%s%s%s :\r\n",
                    p_event->eventField.evtAppAttrInd.connHandle,
                    p_event->eventField.evtAppAttrInd.p_data->appId,
                    p_event->eventField.evtAppAttrInd.p_data->displayName,
                    (g_NtfyInfo.ntfyEvtFlagMask.positiveAction ? " Press Button1: ": ""),
                    (g_NtfyInfo.ntfyEvtFlagMask.positiveAction ? (char *)g_NtfyInfo.positiveAction: ""),
                    (g_NtfyInfo.ntfyEvtFlagMask.negativeAction ? " Double-click Button 1: ": ""),
                    (g_NtfyInfo.ntfyEvtFlagMask.negativeAction ? (char *)g_NtfyInfo.negativeAction: ""));
            APP_UartMsg(s_strBuf);

            g_NtfyInfo.state = APP_ANCS_IND_APP_ATTR;
        }
        break;

        case BLE_ANCS_EVT_ERR_UNSPECIFIED_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_ANCS_EVT_ERR_NO_MEM_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_ANCS_EVT_ERR_IND:
        {
            /* TODO: implement your application code.*/
            /* Require security permission to access characteristics. Inform application and pause the discovery. */
            if ((p_event->eventField.evtErrInd.errCode == ATT_ERR_INSUF_ENC) || (p_event->eventField.evtErrInd.errCode == ATT_ERR_INSUF_AUTHN))
            {
                BLE_DM_ProceedSecurity(p_event->eventField.evtErrInd.connHandle, 0);
            }
        }
        break;

        default:
        break;
    }
}


