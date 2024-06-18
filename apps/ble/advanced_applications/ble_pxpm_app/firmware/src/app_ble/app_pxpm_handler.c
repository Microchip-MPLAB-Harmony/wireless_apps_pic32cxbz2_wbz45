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
    app_pxpm_handler.c

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
#include "app_pxpm_handler.h"
#include "bsp/bsp.h"
#include "app_timer.h"
#include "app_pxpm.h"
#include "app_error_defs.h"

//#define APP_PWR_CTRL_ENABLE 
#define APP_PXPM_ENABLE
#define BLE_PXPM_EVT_NUM  (0x5U)
#define APP_PXPM_TMR_ID_INST_MERGE(id, instance) ((((uint16_t)(id)) << 8) | (uint16_t)(instance))
static APP_PXPM_TxPower_T s_monitorHandle ;
static APP_TIMER_TmrElem_T s_pxpmPathLossTmr;        /**< Key scan timer of 50 ms unit. */
typedef struct APP_PXPM_Reissue_T
{
    reissue                 reissue; 
    uint16_t                connHandle;
    BLE_PXPM_AlertLevel_T   alert_level;
}APP_PXPM_Reissue_T ;
static APP_PXPM_Reissue_T s_pxpmReissue = {
    .reissue = NULL , 

};


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
void APP_PXPM_BackupService(reissue func,uint16_t connHandle,BLE_PXPM_AlertLevel_T level)
{
    if(connHandle) {
        s_pxpmReissue.reissue = func;
        s_pxpmReissue.alert_level = level;
        s_pxpmReissue.connHandle = connHandle;
    }
}
void APP_PXPM_ReissueService(void)
{
    if(s_pxpmReissue.connHandle && NULL != s_pxpmReissue.reissue)
    {
        s_pxpmReissue.reissue(s_pxpmReissue.connHandle,s_pxpmReissue.alert_level);
        s_pxpmReissue.reissue = NULL;
        s_pxpmReissue.connHandle = 0x0;
    }
}

void APP_PXPM_Recovery(void){
    
    RGB_LED_RED_Off();
    RGB_LED_GREEN_Off();
    APP_TIMER_StopTimer(&s_pxpmPathLossTmr.tmrHandle);
} 



void APP_PXPM_RssiCheck(uint16_t connHandle,uint8_t link_budget)
{
    APP_PXPM_ConnList_T *connlist = APP_PXPM_GetConnListByHandle(connHandle);

    switch(connlist->connIasLevel)
    {
        case BLE_PXPM_ALERT_LEVEL_MILD:
        case BLE_PXPM_ALERT_LEVEL_HIGH:
        {
            if(link_budget <= TH_MID_ALERT - LOW_HYSTERESIS)
            {
                connlist->connIasLevel = BLE_PXPM_ALERT_LEVEL_NO;
                BLE_PXPM_WriteIasAlertLevel(connHandle,connlist->connIasLevel);
                RGB_LED_RED_Off();
                RGB_LED_GREEN_On();


            }
        }
        break;
        case BLE_PXPM_ALERT_LEVEL_NO:
        {
            if(link_budget > TH_MID_ALERT + LOW_HYSTERESIS)
            {
                connlist->connIasLevel = (link_budget >= TH_HIGH_ALERT + HIGH_HYSTERESIS) ?
                                                                    BLE_PXPM_ALERT_LEVEL_HIGH : BLE_PXPM_ALERT_LEVEL_MILD;
                BLE_PXPM_WriteIasAlertLevel(connHandle,connlist->connIasLevel);
                RGB_LED_RED_On();
                RGB_LED_GREEN_Off();
            }
        }
        break;
        default:
        {
            connlist->connIasLevel = BLE_PXPM_ALERT_LEVEL_NO;
        }
        break;
    }
}


static uint16_t app_pxpm_SetTimer(uint16_t idInstance, void *p_tmrParam, uint32_t timeout, bool isPeriodicTimer,
APP_TIMER_TmrElem_T *p_tmrElem)
{
    uint8_t tmrId;
    uint16_t result;

    tmrId = (uint8_t)(idInstance >> 8);
    APP_TIMER_SetTimerElem(tmrId, (uint8_t)idInstance, (void *)p_tmrParam, p_tmrElem);
    result = APP_TIMER_SetTimer(p_tmrElem, timeout, isPeriodicTimer);
    return result;
}



void APP_PxpmEvtHandler(BLE_PXPM_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_PXPM_EVT_DISC_COMPLETE_IND:
        {
            /* TODO: implement your application code.*/
            uint16_t connHdl = p_event->eventField.evtDiscComplete.connHandle;
            APP_PXPM_ConnList_T *connlist = APP_PXPM_GetConnListByHandle(connHdl);
            connlist->connTxPwr = 0U;

            if(NULL == memset(&s_monitorHandle , 0U, sizeof(APP_PXPM_TxPower_T)))
            {
                return ;
            }
            if(NULL == memset(&s_pxpmPathLossTmr , 0U, sizeof(APP_TIMER_TmrElem_T)))
            {
                return ;
            }
            
            RGB_LED_RED_Off();
            RGB_LED_GREEN_Off();
            APP_PXPM_BackupService(BLE_PXPM_WriteLlsAlertLevel,connHdl,BLE_PXPM_ALERT_LEVEL_HIGH);
            BLE_PXPM_WriteLlsAlertLevel(connHdl,BLE_PXPM_ALERT_LEVEL_HIGH);
        }
        break;
        
        case BLE_PXPM_EVT_LLS_ALERT_LEVEL_WRITE_RSP_IND:
        {
            /* TODO: implement your application code.*/
            RGB_LED_RED_Off();
            RGB_LED_GREEN_Off();
#ifdef APP_PWR_CTRL_ENABLE
            uint16_t connHdl = p_event->eventField.evtLlsAlertLvInd.connHandle;
            APP_PXPM_ConnList_T *connlist = APP_PXPM_GetConnListByHandle(connHdl);
            BLE_GAP_PathLossReportingParams_T params;
            params.connHandle =  p_event->eventField.evtLlsAlertLvInd.connHandle/* Set the connection handle */;
            params.highThreshold = PWR_CTRL_TH_HIGH /* Set the high threshold */;
            params.highHysteresis = HIGH_HYSTERESIS/* Set the high hysteresis */;
            params.lowThreshold = PWR_CTRL_TH_MILD/* Set the low threshold */;
            params.lowHysteresis = LOW_HYSTERESIS/* Set the low hysteresis */;
            params.minTimeSpent = 1/* Set the minimum time spent */; 
            if (MBA_RES_SUCCESS != APP_PXPM_SetPathLossReportingParams(&params) && !connlist->connTxPwr)
#endif
            {
                BLE_PXPM_ReadTpsTxPowerLevel(p_event->eventField.evtLlsAlertLvInd.connHandle);
                
            }
        }
        break;
   
        case BLE_PXPM_EVT_TPS_TX_POWER_LEVEL_IND:
        {
             /* TODO: implement your application code.*/
            RGB_LED_RED_Off();
            RGB_LED_GREEN_On();
            uint16_t connHdl = p_event->eventField.evtTpsTxPwrLvInd.connHandle;
            APP_PXPM_ConnList_T *connlist = APP_PXPM_GetConnListByHandle(connHdl);
            connlist->connTxPwr = p_event->eventField.evtTpsTxPwrLvInd.txPowerLevel;

            if(connlist->connTxPwr == 0U)
            {
                RGB_LED_GREEN_Off();
                return ;
            }

            s_monitorHandle = (APP_PXPM_TxPower_T){.connhandle = connHdl,
                                                    .tx_power = p_event->eventField.evtTpsTxPwrLvInd.txPowerLevel};
                                                    
            if(APP_RES_SUCCESS != app_pxpm_SetTimer(APP_PXPM_TMR_ID_INST_MERGE(APP_TIMER_RSSI_MONI_03, 0U), &s_monitorHandle, APP_TIMER_1S, true, &s_pxpmPathLossTmr))
            {
                return ; 
            }
        }
        break;

        case BLE_PXPM_EVT_ERR_UNSPECIFIED_IND:
        {
            /* TODO: implement your application code.*/
            RGB_LED_RED_Off();
            RGB_LED_GREEN_Off();
            uint16_t connHdl = p_event->eventField.evtTpsTxPwrLvInd.connHandle;
            APP_PXPM_ConnList_T *connlist = APP_PXPM_GetConnListByHandle(connHdl);
            if(connlist->connTxPwr != 0U)
            {
                APP_TIMER_StopTimer(&s_pxpmPathLossTmr.tmrHandle);
            }
        }
        break;
        default:
        {
        }
        break;
    }

}
