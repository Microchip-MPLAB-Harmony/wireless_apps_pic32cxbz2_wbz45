/*******************************************************************************
  Application Transparent Common Function Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_led.c

  Summary:
    This file contains the Application LED functions for this project.

  Description:
    This file contains the Application LED functions for this project.
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
#include "app_timer.h"
#include "app_led.h"
#include "definitions.h"
#include "bsp/bsp.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
static APP_TIMER_TmrElem_T  s_scanTmr[APP_LED_TYPE_END];


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

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

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
void APP_LED_BlinkLed(uint8_t ledType)
{
    uint16_t idInstance = (((uint16_t)APP_TIMER_LED) << 8) | (uint16_t)ledType;
    app_led_SetTimer(idInstance,NULL, APP_TIMER_500MS, true, &s_scanTmr[ledType]);
}

void APP_LED_StatusLed(bool state)
{
    uint16_t idInstance = (((uint16_t)APP_TIMER_LED) << 8) | (uint16_t)APP_LED_TYPE_STATUS;
    if(state)
    {
        (void)APP_TIMER_StopTimer(&s_scanTmr[APP_LED_TYPE_STATUS].tmrHandle);
        USER_LED_Off();    
    }
    else
    {
        app_led_SetTimer(idInstance, NULL, APP_TIMER_500MS, true, &s_scanTmr[APP_LED_TYPE_STATUS]);        
    }
}

void APP_LED_Toggle(uint8_t ledType)
{
    switch(ledType)
    {
        case APP_LED_TYPE_RED:
        {
            RGB_LED_RED_Toggle();
        }
        break;

        case APP_LED_TYPE_GREEN:
        {
            RGB_LED_GREEN_Toggle();
        }
        break;

        case APP_LED_TYPE_BLUE:
        {
            RGB_LED_BLUE_Toggle();
        }
        break;
        
        case APP_LED_TYPE_STATUS:
        {
            USER_LED_Toggle();
        }
        break;

        default:
        break;        
    }
}


/* *****************************************************************************
 End of File
 */
