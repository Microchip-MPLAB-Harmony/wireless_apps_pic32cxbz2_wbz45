/*******************************************************************************
  Application Transparent Common Function Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_key.c

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
#include "app_key.h"

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
#define APP_KEY_PB4                 0x10
#define APP_KEY_LONG_PRESS_TIME     10      // 50 ms * 10 = 500 ms
#define APP_KEY_DOUBLE_CLICK_TIME   8       // 50 ms * 8 = 400 ms

enum APP_KEY_DEFINE_T
{
    APP_KEY_DEFINED_RELEASED = 0x00,
    APP_KEY_DEFINED_PRESSED,

    APP_KEY_DEFINE_END
};

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
static APP_KEY_Elem_T       s_appKeyElement;
static APP_TIMER_TmrElem_T  s_scanTmr;        /**< Key scan timer of 50 ms unit. */
static APP_KeyCb_T   s_appKeyCb;
// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
#define APP_KEY_TMR_ID_INST_MERGE(id, instance) ((((uint16_t)id) << 8) | instance)
static uint16_t app_key_SetTimer(uint16_t idInstance, void *p_tmrParam, uint32_t timeout, bool isPeriodicTimer,
APP_TIMER_TmrElem_T *p_tmrElem)
{
    uint8_t tmrId;
    uint16_t result;

    tmrId = (uint8_t)(idInstance >> 8);
    APP_TIMER_SetTimerElem(tmrId, (uint8_t)idInstance, (void *)p_tmrParam, p_tmrElem);
    result = APP_TIMER_SetTimer(p_tmrElem, timeout, isPeriodicTimer);

    return result;
}


void APP_KEY_MsgRegister(APP_KeyCb_T keyCb)
{
    s_appKeyCb  = keyCb;
}

void APP_KEY_Init(void)
{
    // GPIO register initialization for the user button.
    GPIOB_REGS->GPIO_ANSELCLR = APP_KEY_PB4;
    GPIOB_REGS->GPIO_TRISSET =  APP_KEY_PB4;

    memset((uint8_t *)&s_appKeyElement, 0, sizeof(APP_KEY_Elem_T));
    app_key_SetTimer(APP_KEY_TMR_ID_INST_MERGE(APP_TIMER_KEY_SCAN, 0), NULL, APP_TIMER_50MS,
        true, &s_scanTmr);

    s_appKeyCb=NULL;
}

void APP_KEY_Scan(void)
{
    uint8_t keyState, keyMesg = APP_KEY_MSG_NULL;

    if (GPIO_PinRead(GPIO_PIN_RB4) == false)
        keyState = APP_KEY_DEFINED_PRESSED;
    else
        keyState = APP_KEY_DEFINED_RELEASED;

    if (keyState == APP_KEY_DEFINED_PRESSED)
    {
        s_appKeyElement.counter++;
        if (s_appKeyElement.state == APP_KEY_STATE_RELEASE)
        {
            s_appKeyElement.state = APP_KEY_STATE_SHORT_PRESS;
        }
        else if(s_appKeyElement.state == APP_KEY_STATE_SHORT_PRESS)
        {
            if (s_appKeyElement.counter > APP_KEY_LONG_PRESS_TIME)
            {
                s_appKeyElement.state = APP_KEY_STATE_LONG_PRESS;
                keyMesg = APP_KEY_MSG_LONG_PRESS;
            }
        }
        else if (s_appKeyElement.state == APP_KEY_STATE_SHORT_PRESS_RELEASE)
        {
            s_appKeyElement.state = APP_KEY_STATE_DOUBLE_CLICK_PRESS;
            keyMesg = APP_KEY_MSG_DOUBLE_CLICK;
        }
    }
    else
    {
        if (s_appKeyElement.state != APP_KEY_STATE_RELEASE)
            s_appKeyElement.counter++;
        if ((s_appKeyElement.state == APP_KEY_STATE_SHORT_PRESS) &&
            (s_appKeyElement.counter <= APP_KEY_DOUBLE_CLICK_TIME))
            s_appKeyElement.state = APP_KEY_STATE_SHORT_PRESS_RELEASE;
        else if ((s_appKeyElement.state == APP_KEY_STATE_SHORT_PRESS) || ((s_appKeyElement.counter
            > APP_KEY_DOUBLE_CLICK_TIME) && (s_appKeyElement.state == APP_KEY_STATE_SHORT_PRESS_RELEASE)))
            keyMesg = APP_KEY_MSG_SHORT_PRESS;
        if ((keyMesg != APP_KEY_MSG_NULL) || (s_appKeyElement.state == APP_KEY_STATE_LONG_PRESS)
            || (s_appKeyElement.state == APP_KEY_STATE_DOUBLE_CLICK_PRESS))
        {
            s_appKeyElement.state = APP_KEY_STATE_RELEASE;
            s_appKeyElement.counter = 0;
        }
    }
    
    if (keyMesg != APP_KEY_MSG_NULL)
    {
        if (s_appKeyCb)
        {
            s_appKeyCb(keyMesg);
        }
    }
}
