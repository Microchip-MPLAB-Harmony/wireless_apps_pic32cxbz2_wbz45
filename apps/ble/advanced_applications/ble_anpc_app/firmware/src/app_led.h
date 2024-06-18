/*******************************************************************************
  Application Transparent Client Role Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_led.h

  Summary:
    This file contains the Application Transparent Client Role functions for this project.

  Description:
    This file contains the Application Transparent Client Role functions for this project.
    The implementation of demo mode is included in this file.
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


#ifndef APP_LED_H
#define APP_LED_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_timer.h"
#include "peripheral/gpio/plib_gpio.h"
#include "app_error_defs.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_LED_RED_ON          GPIO_PinSet(GPIO_PIN_RB0);
#define APP_LED_RED_OFF         GPIO_PinClear(GPIO_PIN_RB0);
#define APP_LED_GREEN_ON        GPIO_PinSet(GPIO_PIN_RB3);
#define APP_LED_GREEN_OFF       GPIO_PinClear(GPIO_PIN_RB3);
#define APP_LED_BLUE_ON         GPIO_PinSet(GPIO_PIN_RB5);
#define APP_LED_BLUE_OFF        GPIO_PinClear(GPIO_PIN_RB5);

#define APP_LED_ELEM_NUMBER     (3U)
#define APP_LED_HANDLER_NULL    (0xFFU)
#define APP_LED_EVENT_START     (0x01U)
#define APP_LED_EVENT_INTERVAL  (0x02U)

typedef enum APP_LED_STATE_T
{
    APP_LED_STATE_NULL = 0x00,
    APP_LED_STATE_OFF,
    APP_LED_STATE_ON,
    APP_LED_STATE_REMAIN,
    
    APP_LED_STATE_END   
}APP_LED_STATE_T;

enum APP_LED_TYPE_T
{
    APP_LED_TYPE_NULL = 0x00,
    APP_LED_TYPE_RED,
    APP_LED_TYPE_GREEN,
    APP_LED_TYPE_BLUE,

    APP_LED_TYPE_END   
};

enum APP_LED_MODE_T
{
    APP_LED_MODE_ADV = 0x00,
    APP_LED_MODE_ADV_DIR,
    APP_LED_MODE_CONN,
    
    APP_LED_MODE_END   
};
#define APP_LED_MODE_NUMBER     APP_LED_MODE_END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@brief The structure contains the information about LED element format. */
typedef struct APP_LED_Elem_T
{
    uint8_t                 handler;        /**< LED handler. */
    uint8_t                 type;           /**< LED type. */
    uint8_t                 flashCnt;       /**< Flash counter. */
    uint8_t                 tmpFlashCnt;    /**< Temp flash counter. */
    uint8_t                 cycleCnt;       /**< Cycle counter. */
    uint8_t                 tmpCycleCnt;    /**< Temp cycle counter. */
    uint16_t                offInterval;    /**< Off interval. */
    uint16_t                onInterval;     /**< On interval. */ 
    uint32_t                duration;       /**< Duration. */  
    APP_TIMER_TmrElem_T     intervalTmr;    /**< Interval timer. */
    APP_LED_STATE_T         state;          /**<.LED state. */
} APP_LED_Elem_T;

/**@brief The structure contains the information about LED parameter format. */
typedef struct APP_LED_Param_T
{
    uint8_t                 type;           /**< LED type. */
    uint8_t                 flashCnt;       /**< Flash counter. */
    uint8_t                 cycleCnt;       /**< Cycle counter. */
    uint16_t                offInterval;    /**< Off interval. */
    uint16_t                onInterval;     /**< On interval. */ 
    uint32_t                duration;       /**< Duration. */  
} APP_LED_Param_T;

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
extern uint8_t g_appLedHandler;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_LED_Init(void);
void APP_LED_StateMachine(uint8_t event, APP_LED_Elem_T *p_ledElem);
uint16_t APP_LED_Stop(uint8_t ledHandler);
uint8_t APP_LED_Start(const APP_LED_Param_T *p_ledParam);
uint8_t APP_LED_StartByMode(uint8_t mode);

#endif
/*******************************************************************************
 End of File
 */
