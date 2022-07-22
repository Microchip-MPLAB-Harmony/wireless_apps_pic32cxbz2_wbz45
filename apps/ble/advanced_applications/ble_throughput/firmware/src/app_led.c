/*******************************************************************************
  Application Transparent Common Function Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_led.c

  Summary:
    This file contains the Application Transparent common functions for this project.

  Description:
    This file contains the Application Transparent common functions for this project.
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
#include "app_led.h"
#include "app_trp_common.h"
#include "app_error_defs.h"
#include "system/console/sys_console.h"
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
uint8_t g_appLedRedHandler, g_appLedGreenHandler;

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_LED_Elem_T s_appLedElement_t[APP_LED_ELEM_NUMBER];

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
const APP_LED_Param_T appLedMode[APP_LED_MODE_NUMBER] = {
    {APP_LED_TYPE_GREEN, 1, 0, 250, 250, 500},
    {APP_LED_TYPE_GREEN, 1, 0, 300, 100, 2000},
    {APP_LED_TYPE_GREEN, 2, 0, 300, 100, 4000},
    {APP_LED_TYPE_GREEN, 3, 0, 300, 100, 4000},
    {APP_LED_TYPE_GREEN, 4, 0, 300, 100, 4000},
    {APP_LED_TYPE_GREEN, 5, 0, 300, 100, 5000},
    {APP_LED_TYPE_RED, 1, 0, 250, 250, 500},
    {APP_LED_TYPE_RED, 1, 0, 300, 100, 2000},
    {APP_LED_TYPE_RED, 2, 0, 300, 100, 3000},
    {APP_LED_TYPE_RED, 3, 0, 300, 100, 4000},
    {APP_LED_TYPE_RED, 4, 0, 300, 100, 4000},
    {APP_LED_TYPE_RED, 5, 0, 300, 100, 5000}
};
static void APP_LED_Off(uint8_t ledType)
{
    switch(ledType)
    {
        case APP_LED_TYPE_RED:
            APP_LED_RED_OFF;
            break;

        case APP_LED_TYPE_GREEN:
            APP_LED_GREEN_OFF;
            break;

        default:
            break;
    }
}

static void APP_LED_On(uint8_t ledType)
{
    switch(ledType)
    {
        case APP_LED_TYPE_RED:
            APP_LED_RED_ON;
            break;

        case APP_LED_TYPE_GREEN:
            APP_LED_GREEN_ON;
            break;

        default:
            break;
    }
}

static void APP_LED_Reload(APP_LED_Elem_T *p_ledElem)
{
    p_ledElem->state = APP_LED_STATE_OFF;
    p_ledElem->tmpFlashCnt = p_ledElem->flashCnt;
    p_ledElem->tmpFlashCnt--;
    // Set timer.
    APP_TRP_COMMON_SetTimer(APP_TRP_TMR_ID_INST_MERGE(APP_TIMER_LED_04, APP_LED_EVENT_INTERVAL), 
        (void *)p_ledElem, p_ledElem->onInterval, false, &(p_ledElem->intervalTmr));

    APP_LED_On(p_ledElem->type);
}

static void APP_LED_CycleProc(APP_LED_Elem_T *p_ledElem)
{
    if (p_ledElem->cycleCnt > 0)
    {
        if (p_ledElem->tmpCycleCnt > 0)
            p_ledElem->tmpCycleCnt--;
        else
        {
            APP_LED_Stop(p_ledElem->handler);
            return;
        }
    }
    APP_LED_Reload(p_ledElem);
}

void APP_LED_Init(void)
{
    uint8_t i;
    
    // GPIO register initialization for LED.
    GPIOB_REGS->GPIO_ANSEL = GPIOB_REGS->GPIO_ANSEL & 0xFFFFFFD6;
    GPIOB_REGS->GPIO_TRIS = GPIOB_REGS->GPIO_TRIS & 0xFFFFFFD6;
    GPIOB_REGS->GPIO_PORTCLR = 0x29;
    memset(s_appLedElement_t, 0, APP_LED_ELEM_NUMBER * sizeof(APP_LED_Elem_T));
    for (i = 0; i < APP_LED_ELEM_NUMBER; i++)
        s_appLedElement_t[i].handler = APP_LED_HANDLER_NULL;
    g_appLedRedHandler = APP_LED_HANDLER_NULL;
    g_appLedGreenHandler = APP_LED_HANDLER_NULL;
}

void APP_LED_StateMachine(uint8_t event, APP_LED_Elem_T *p_ledElem)
{
    uint16_t deltaInterval;
    
    switch(p_ledElem->state)
    {
        case APP_LED_STATE_NULL:
        {
            if (event == APP_LED_EVENT_START)
            {
                
                if (p_ledElem->cycleCnt > 0)
                {
                    p_ledElem->tmpCycleCnt = p_ledElem->cycleCnt;
                    p_ledElem->tmpCycleCnt--;
                }
                APP_LED_Reload(p_ledElem);
            }
        }
            break;

        case APP_LED_STATE_ON:
        {
            if (event == APP_LED_EVENT_INTERVAL)
            {
                if (p_ledElem->tmpFlashCnt > 0)
                    p_ledElem->tmpFlashCnt--;
                else
                {
                    p_ledElem->state = APP_LED_STATE_REMAIN;
                    if (p_ledElem->duration <= ((p_ledElem->offInterval + p_ledElem->onInterval) 
                        * p_ledElem->flashCnt))
                        deltaInterval = 0;
                    else
                        deltaInterval = p_ledElem->duration - (p_ledElem->offInterval + p_ledElem->onInterval) 
                            * p_ledElem->flashCnt;
                    if (deltaInterval > 0)
                        APP_TRP_COMMON_SetTimer(APP_TRP_TMR_ID_INST_MERGE(APP_TIMER_LED_04, APP_LED_EVENT_INTERVAL), 
                            (void *)p_ledElem, deltaInterval, false, &(p_ledElem->intervalTmr));
                    else
                        APP_TRP_COMMON_SetTimer(APP_TRP_TMR_ID_INST_MERGE(APP_TIMER_LED_04, APP_LED_EVENT_INTERVAL), 
                            (void *)p_ledElem, 1, false, &(p_ledElem->intervalTmr));
                    break;
                }
                p_ledElem->state = APP_LED_STATE_OFF;
                APP_TRP_COMMON_SetTimer(APP_TRP_TMR_ID_INST_MERGE(APP_TIMER_LED_04, APP_LED_EVENT_INTERVAL), 
                    (void *)p_ledElem, p_ledElem->onInterval, false, &(p_ledElem->intervalTmr));

                APP_LED_On(p_ledElem->type);
            }
        }
            break;

        case APP_LED_STATE_OFF:
        {
            if (event == APP_LED_EVENT_INTERVAL)
            {
                p_ledElem->state = APP_LED_STATE_ON;
                APP_TRP_COMMON_SetTimer(APP_TRP_TMR_ID_INST_MERGE(APP_TIMER_LED_04, APP_LED_EVENT_INTERVAL), 
                    (void *)p_ledElem, p_ledElem->offInterval, false, &(p_ledElem->intervalTmr));

                APP_LED_Off(p_ledElem->type);
            }
        }
            break;

        case APP_LED_STATE_REMAIN:
        {
            APP_LED_CycleProc(p_ledElem);
        }
            break;

         default:
            break;
    }

}

uint16_t APP_LED_Stop(uint8_t ledHandler)
{
    APP_LED_Elem_T *p_ledElem;
    uint16_t status = APP_RES_FAIL;

    if (ledHandler < APP_LED_ELEM_NUMBER)
    {
        p_ledElem = &(s_appLedElement_t[ledHandler]);
        if (p_ledElem->state != APP_LED_STATE_NULL)
        {
            APP_LED_Off(p_ledElem->type);
            p_ledElem->state = APP_LED_STATE_NULL;
            status = APP_TIMER_StopTimer(&(p_ledElem->intervalTmr.tmrHandle));
            if (status != APP_RES_SUCCESS)
            {
                SYS_CONSOLE_PRINT("Stop LED interval timer error ! status=%d\r\n", status);
            }
            p_ledElem->handler = APP_LED_HANDLER_NULL;
            p_ledElem->type = APP_LED_TYPE_NULL;
            p_ledElem->flashCnt = 0;
            p_ledElem->cycleCnt = 0;
            p_ledElem->offInterval = 0;
            p_ledElem->onInterval = 0;
            p_ledElem->duration = 0;
        }
        else
            return APP_RES_SUCCESS;
    }
    else
        return APP_RES_INVALID_PARA;

    return status;
}

uint8_t APP_LED_Start(APP_LED_Param_T *p_ledParam)
{
    uint8_t i = 0;
    APP_LED_Elem_T *p_ledElem;
    
    if (p_ledParam != NULL)
    {
        if (p_ledParam->duration < ((p_ledParam->offInterval + p_ledParam->onInterval) 
            * p_ledParam->flashCnt))
        {
            SYS_CONSOLE_PRINT("LED duration is too small !\r\n");
            return APP_LED_HANDLER_NULL;
        }
        // Stop the type LED
        for (i = 0; i < APP_LED_ELEM_NUMBER; i++)
        {
            if((s_appLedElement_t[i].state != APP_LED_STATE_NULL) 
                && (s_appLedElement_t[i].type == p_ledParam->type))
            {
                APP_LED_Stop(s_appLedElement_t[i].handler);
            }
        }
        // Get LED element
        for (i = 0; i < APP_LED_ELEM_NUMBER; i++)
        {
            if (s_appLedElement_t[i].state == APP_LED_STATE_NULL)
                break;
        }
        if (i >= APP_LED_ELEM_NUMBER)
            return APP_LED_HANDLER_NULL;
        p_ledElem = &(s_appLedElement_t[i]);
        p_ledElem->handler = i;
        p_ledElem->type = p_ledParam->type;
        p_ledElem->flashCnt = p_ledParam->flashCnt;
        p_ledElem->cycleCnt= p_ledParam->cycleCnt;
        p_ledElem->offInterval= p_ledParam->offInterval;
        p_ledElem->onInterval= p_ledParam->offInterval;
        p_ledElem->duration= p_ledParam->duration;
        APP_LED_StateMachine(APP_LED_EVENT_START, p_ledElem);

        return p_ledElem->handler;
    }
    else
        return APP_LED_HANDLER_NULL;
}

uint8_t APP_LED_StartByMode(uint8_t mode)
{
    APP_LED_Param_T *p_ledMode;
    uint8_t ledHandler;

    p_ledMode = (APP_LED_Param_T *)&(appLedMode[mode]);
    ledHandler = APP_LED_Start(p_ledMode);
    if (ledHandler == APP_LED_HANDLER_NULL)
        SYS_CONSOLE_PRINT("Start LED error ! Mode=%d\r\n", mode);

    return ledHandler;
}

void APP_LED_AlwaysOn(uint8_t ledType1, uint8_t ledType2)
{
    APP_LED_Off(APP_LED_TYPE_GREEN);
    APP_LED_Off(APP_LED_TYPE_RED);
    if (g_appLedGreenHandler != APP_LED_HANDLER_NULL)
        APP_LED_Stop(g_appLedGreenHandler);
    if (g_appLedRedHandler != APP_LED_HANDLER_NULL)
        APP_LED_Stop(g_appLedRedHandler);
    
    APP_LED_On(ledType1);
    APP_LED_On(ledType2);
}