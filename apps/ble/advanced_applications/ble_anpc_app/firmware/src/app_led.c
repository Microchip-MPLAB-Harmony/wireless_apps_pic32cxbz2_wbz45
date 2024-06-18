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
uint8_t g_appLedHandler;

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_LED_Elem_T s_appLedElement[APP_LED_ELEM_NUMBER];

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
static const APP_LED_Param_T s_appLedMode[APP_LED_MODE_NUMBER] = {
    {APP_LED_TYPE_BLUE, 1, 0, 50,  50,  3000},
    {APP_LED_TYPE_BLUE, 2, 0, 50,  50,  3000},
    {APP_LED_TYPE_BLUE, 2, 0, 150, 50,  1500}
};

#define APP_LED_TMR_ID_INST_MERGE(id, instance) ((((uint16_t)(id)) << 8) | (uint16_t)(instance))
static uint16_t app_led_SetTimer(uint16_t idInstance, void *p_tmrParam, uint32_t timeout, bool isPeriodicTimer,
APP_TIMER_TmrElem_T *p_tmrElem)
{
    uint8_t tmrId;
    uint16_t result;

    tmrId = (uint8_t)(idInstance >> 8);
    APP_TIMER_SetTimerElem(tmrId, (uint8_t)idInstance, (void *)p_tmrParam, p_tmrElem);
    result = APP_TIMER_SetTimer(p_tmrElem, timeout, isPeriodicTimer);

    return result;
}

static void app_led_Off(uint8_t ledType)
{
    switch(ledType)
    {
        case APP_LED_TYPE_RED:
        {
            APP_LED_RED_OFF;
        }
        break;

        case APP_LED_TYPE_GREEN:
        {
            APP_LED_GREEN_OFF;
        }
        break;

        case APP_LED_TYPE_BLUE:
        {
            APP_LED_BLUE_OFF;
        }
        break;

        default:
        break;
    }
}

static void app_led_On(uint8_t ledType)
{
    switch(ledType)
    {
        case APP_LED_TYPE_RED:
        {
            APP_LED_RED_ON;
        }
        break;

        case APP_LED_TYPE_GREEN:
        {
            APP_LED_GREEN_ON;
        }
        break;

        case APP_LED_TYPE_BLUE:
        {
            APP_LED_BLUE_ON;
        }
        break;

        default:
        break;
    }
}

static void app_led_Reload(APP_LED_Elem_T *p_ledElem)
{
    uint16_t ret;

    p_ledElem->state = APP_LED_STATE_OFF;
    p_ledElem->tmpFlashCnt = p_ledElem->flashCnt;
    p_ledElem->tmpFlashCnt--;
    // Set timer.
    ret = app_led_SetTimer(APP_LED_TMR_ID_INST_MERGE(APP_TIMER_LED_01, APP_LED_EVENT_INTERVAL),
        (void *)p_ledElem, p_ledElem->onInterval, false, &(p_ledElem->intervalTmr));
    if (ret != APP_RES_SUCCESS)
    {
        //if error occurs
    }

    app_led_On(p_ledElem->type);
}

static void app_led_CycleProc(APP_LED_Elem_T *p_ledElem)
{
    if (p_ledElem->cycleCnt > 0U)
    {
        if (p_ledElem->tmpCycleCnt > 0U)
        {
            p_ledElem->tmpCycleCnt--;
        }
        else
        {
            uint16_t ret;
            ret = APP_LED_Stop(p_ledElem->handler);
            if (ret != APP_RES_SUCCESS)
            {
                //if error occurs
            }
            return;
        }
    }
    app_led_Reload(p_ledElem);
}

void APP_LED_Init(void)
{
    uint8_t i;

    // GPIO register initialization for LED.
    GPIOB_REGS->GPIO_ANSEL = GPIOB_REGS->GPIO_ANSEL & 0xFFFFFFD6U;
    GPIOB_REGS->GPIO_TRIS = GPIOB_REGS->GPIO_TRIS & 0xFFFFFFD6U;
    GPIOB_REGS->GPIO_PORTCLR = 0x29;
    (void)memset(s_appLedElement, 0, APP_LED_ELEM_NUMBER * sizeof(APP_LED_Elem_T));
    for (i = 0; i < APP_LED_ELEM_NUMBER; i++)
    {
        s_appLedElement[i].handler = APP_LED_HANDLER_NULL;
    }
    g_appLedHandler = APP_LED_HANDLER_NULL;
}

void APP_LED_StateMachine(uint8_t event, APP_LED_Elem_T *p_ledElem)
{
    uint16_t ret;

    switch(p_ledElem->state)
    {
        case APP_LED_STATE_NULL:
        {
            if (event == APP_LED_EVENT_START)
            {

                if (p_ledElem->cycleCnt > 0U)
                {
                    p_ledElem->tmpCycleCnt = p_ledElem->cycleCnt;
                    p_ledElem->tmpCycleCnt--;
                }
                app_led_Reload(p_ledElem);
            }
        }
        break;

        case APP_LED_STATE_ON:
        {
            if (event == APP_LED_EVENT_INTERVAL)
            {
                if (p_ledElem->tmpFlashCnt > 0U)
                {
                    p_ledElem->tmpFlashCnt--;
                }
                else
                {
                    uint16_t deltaInterval;
                    p_ledElem->state = APP_LED_STATE_REMAIN;
                    if (p_ledElem->duration <= ((p_ledElem->offInterval + p_ledElem->onInterval)
                        * p_ledElem->flashCnt))
                    {
                        deltaInterval = 0;
                    }
                    else
                    {
                        deltaInterval = p_ledElem->duration - (p_ledElem->offInterval + p_ledElem->onInterval)
                            * p_ledElem->flashCnt;
                    }
                    if (deltaInterval > 0U)
                    {
                        ret = app_led_SetTimer(APP_LED_TMR_ID_INST_MERGE(APP_TIMER_LED_01, APP_LED_EVENT_INTERVAL),
                            (void *)p_ledElem, deltaInterval, false, &(p_ledElem->intervalTmr));
                        if (ret != APP_RES_SUCCESS)
                        {
                            //if error occurs
                        }
                    }
                    else
                    {
                        ret = app_led_SetTimer(APP_LED_TMR_ID_INST_MERGE(APP_TIMER_LED_01, APP_LED_EVENT_INTERVAL),
                            (void *)p_ledElem, 1, false, &(p_ledElem->intervalTmr));
                        if (ret != APP_RES_SUCCESS)
                        {
                            //if error occurs
                        }
                    }
                    break;
                }
                p_ledElem->state = APP_LED_STATE_OFF;
                ret = app_led_SetTimer(APP_LED_TMR_ID_INST_MERGE(APP_TIMER_LED_01, APP_LED_EVENT_INTERVAL),
                    (void *)p_ledElem, p_ledElem->onInterval, false, &(p_ledElem->intervalTmr));
                if (ret != APP_RES_SUCCESS)
                {
                    //if error occurs
                }

                app_led_On(p_ledElem->type);
            }
        }
        break;

        case APP_LED_STATE_OFF:
        {
            if (event == APP_LED_EVENT_INTERVAL)
            {
                p_ledElem->state = APP_LED_STATE_ON;
                ret = app_led_SetTimer(APP_LED_TMR_ID_INST_MERGE(APP_TIMER_LED_01, APP_LED_EVENT_INTERVAL),
                    (void *)p_ledElem, p_ledElem->offInterval, false, &(p_ledElem->intervalTmr));
                if (ret != APP_RES_SUCCESS)
                {
                    //if error occurs
                }

                app_led_Off(p_ledElem->type);
            }
        }
        break;

        case APP_LED_STATE_REMAIN:
        {
            app_led_CycleProc(p_ledElem);
        }
        break;

        default:
        break;
    }

}

uint16_t APP_LED_Stop(uint8_t ledHandler)
{
    uint16_t status = APP_RES_FAIL;

    if (ledHandler < APP_LED_ELEM_NUMBER)
    {
        APP_LED_Elem_T *p_ledElem;
        p_ledElem = &(s_appLedElement[ledHandler]);
        if (p_ledElem->state != APP_LED_STATE_NULL)
        {
            app_led_Off(p_ledElem->type);
            p_ledElem->state = APP_LED_STATE_NULL;
            status = APP_TIMER_StopTimer(&(p_ledElem->intervalTmr.tmrHandle));

            p_ledElem->handler = APP_LED_HANDLER_NULL;
            p_ledElem->type = APP_LED_TYPE_NULL;
            p_ledElem->flashCnt = 0;
            p_ledElem->cycleCnt = 0;
            p_ledElem->offInterval = 0;
            p_ledElem->onInterval = 0;
            p_ledElem->duration = 0;
        }
        else
        {
            return APP_RES_SUCCESS;
        }
    }
    else
    {
        return APP_RES_INVALID_PARA;
    }

    return status;
}

uint8_t APP_LED_Start(const APP_LED_Param_T *p_ledParam)
{
    if (p_ledParam != NULL)
    {
        APP_LED_Elem_T *p_ledElem;
        uint8_t i = 0;
        if (p_ledParam->duration < ((p_ledParam->offInterval + p_ledParam->onInterval)
            * p_ledParam->flashCnt))
        {
            return APP_LED_HANDLER_NULL;
        }
        // Stop the type LED
        for (i = 0; i < APP_LED_ELEM_NUMBER; i++)
        {
            if((s_appLedElement[i].state != APP_LED_STATE_NULL)
                && (s_appLedElement[i].type == p_ledParam->type))
            {
                uint16_t ret = APP_LED_Stop(s_appLedElement[i].handler);
                if (ret != APP_RES_SUCCESS)
                {
                    //if error occurs
                }
            }
        }
        // Get LED element
        for (i = 0; i < APP_LED_ELEM_NUMBER; i++)
        {
            if (s_appLedElement[i].state == APP_LED_STATE_NULL)
            {
                break;
            }
        }
        if (i >= APP_LED_ELEM_NUMBER)
        {
            return APP_LED_HANDLER_NULL;
        }
        p_ledElem = &(s_appLedElement[i]);
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
    {
        return APP_LED_HANDLER_NULL;
    }
}

uint8_t APP_LED_StartByMode(uint8_t mode)
{
    APP_LED_Param_T *p_ledMode;
    uint8_t ledHandler;

    p_ledMode = (APP_LED_Param_T * volatile)&(s_appLedMode[mode]);
    ledHandler = APP_LED_Start(p_ledMode);

    return ledHandler;
}

