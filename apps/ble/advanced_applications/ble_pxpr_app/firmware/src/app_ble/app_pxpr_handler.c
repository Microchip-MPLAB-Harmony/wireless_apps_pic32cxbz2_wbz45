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
    app_pxpr_handler.c

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
#include "app_pxpr.h"
#include "app_pxpr_handler.h"
#include "app_timer.h"
#include "app_error_defs.h"
#include "bsp/bsp.h"

#define APP_PXPR_TMR_ID_INST_MERGE(id, instance) ((((uint16_t)(id)) << 8) | (uint16_t)(instance))
static APP_TIMER_TmrElem_T  s_pathLedAlertTmr;

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
static uint16_t app_pxpr_SetTimer(uint16_t idInstance, void *p_tmrParam, bool isPeriodicTimer,
APP_TIMER_TmrElem_T *p_tmrElem)
{
    uint8_t tmrId;
    uint16_t result;
    uint32_t timeout = 0x0;
    BLE_PXPR_AlertLevel_T alert_level = *((BLE_PXPR_AlertLevel_T*)p_tmrParam);

    tmrId = (uint8_t)(idInstance >> 8);
    APP_TIMER_SetTimerElem(tmrId, (uint8_t)idInstance, (void *)p_tmrParam, p_tmrElem);
    switch (alert_level)
    {
        case BLE_PXPR_ALERT_LEVEL_MILD:
        {
            timeout = APP_TIMER_500MS;
        }
        break;
        case BLE_PXPR_ALERT_LEVEL_HIGH:
        {
            timeout = APP_TIMER_100MS;
        }
        break;
        default:
        break;
    }

    result = APP_TIMER_SetTimer(p_tmrElem, timeout, isPeriodicTimer);
    return result;
}


static void app_SetAlertTimer(APP_PXPR_ConnList_T *p_conn){

    if(p_conn->currAlert > BLE_PXPR_ALERT_LEVEL_NO)
    {
        if(APP_RES_SUCCESS != app_pxpr_SetTimer(APP_PXPR_TMR_ID_INST_MERGE(APP_TIMER_LED_ALERT_03,0U), &p_conn->currAlert, true, &s_pathLedAlertTmr))
        {
            return ; 
        }
    }
    else
    {
        APP_TIMER_StopTimer(&s_pathLedAlertTmr.tmrHandle);
        USER_LED_Off();
    }
    
}
void APP_PXPR_Linkloss(uint16_t connHandle)
{
    APP_PXPR_ConnList_T *p_conn = APP_PXPR_GetConnListByHandle(connHandle);
    APP_TIMER_StopTimer(&s_pathLedAlertTmr.tmrHandle);
    if(APP_RES_SUCCESS != app_pxpr_SetTimer(APP_PXPR_TMR_ID_INST_MERGE(APP_TIMER_LED_ALERT_03, 0U), &p_conn->llsAlert, true, &s_pathLedAlertTmr))
    {
        return ;
    }
}


void APP_PXPR_Connected(void)
{
    APP_TIMER_StopTimer(&s_pathLedAlertTmr.tmrHandle);
    USER_LED_Off();
}

void APP_PxprEvtHandler(BLE_PXPR_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_PXPR_EVT_LLS_ALERT_LEVEL_WRITE_IND:
        {
            APP_PXPR_ConnList_T *p_conn = APP_PXPR_GetConnListByHandle(p_event->eventField.evtLlsAlertLevelWriteInd.connHandle);
            p_conn ->llsAlert = p_event->eventField.evtLlsAlertLevelWriteInd.alertLevel;
            BLE_PXPR_SetLlsAlertLevel(p_event->eventField.evtLlsAlertLevelWriteInd.alertLevel);
        }
        break;
        
        case BLE_PXPR_EVT_IAS_ALERT_LEVEL_WRITE_IND:
        {
            APP_PXPR_ConnList_T *p_conn = APP_PXPR_GetConnListByHandle(p_event->eventField.evtIasAlertLevelWriteInd.connHandle);
            if(p_conn->currAlert != p_event->eventField.evtLlsAlertLevelWriteInd.alertLevel)
            {
                p_conn->currAlert = p_event->eventField.evtLlsAlertLevelWriteInd.alertLevel;
                app_SetAlertTimer(p_conn);
            }
        }
        break;

        case BLE_PXPR_EVT_ERR_UNSPECIFIED_IND:
        {
            APP_TIMER_StopTimer(&s_pathLedAlertTmr.tmrHandle);
        }
        break;
        default:
        break;
    }
}

