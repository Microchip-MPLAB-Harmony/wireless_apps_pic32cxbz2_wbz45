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
    app_conn.c

  Summary:
    This file contains connection management for this project.

  Description:
    This file contains connection management for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "app_conn.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

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

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_TIMER_TmrElem_T  s_connTmr;
static APP_ConnTimeOutActionCb_T   s_appConnTimeOutActionCb;
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
#define APP_CONN_TMR_ID_INST_MERGE(id, instance) ((((uint16_t)id) << 8) | instance)
static uint16_t app_conn_SetTimer(uint16_t idInstance, void *p_tmrParam, uint32_t timeout, bool isPeriodicTimer,
APP_TIMER_TmrElem_T *p_tmrElem)
{
    uint8_t tmrId;
    uint16_t result;

    tmrId = (uint8_t)(idInstance >> 8);
    APP_TIMER_SetTimerElem(tmrId, (uint8_t)idInstance, (void *)p_tmrParam, p_tmrElem);
    result = APP_TIMER_SetTimer(p_tmrElem, timeout, isPeriodicTimer);

    return result;
}

void APP_CONN_TimeOutActionRegister(APP_ConnTimeOutActionCb_T ActCb)
{
    s_appConnTimeOutActionCb  = ActCb;
}

void APP_CONN_Init(void)
{
    memset((uint8_t *)&s_connTmr, 0, sizeof(APP_TIMER_TmrElem_T));
    s_appConnTimeOutActionCb=NULL;
}

void APP_CONN_StartTimeoutTimer(void)
{
    app_conn_SetTimer(APP_CONN_TMR_ID_INST_MERGE(APP_TIMER_CONN_TIMEOUT_03, 0), NULL, APP_TIMER_3MIN,
            false, &s_connTmr);
}

void APP_CONN_StopTimeoutTimer(void)
{
    if (s_connTmr.tmrHandle != NULL)
    {
        APP_TIMER_StopTimer(&s_connTmr.tmrHandle);
    }
}

void APP_CONN_ResetTimeoutTimer(void)
{
    if (s_connTmr.tmrHandle != NULL)
    {
        APP_TIMER_ResetTimer(s_connTmr.tmrHandle);
    }
}

void APP_CONN_TimeoutAction(void)
{
    if (s_appConnTimeOutActionCb != NULL)
    {
        s_appConnTimeOutActionCb();
    }
}
