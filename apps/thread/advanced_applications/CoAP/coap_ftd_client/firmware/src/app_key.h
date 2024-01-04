/*******************************************************************************
  Application Transparent Client Role Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_key.h

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


#ifndef APP_KEY_H
#define APP_KEY_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_timer.h"
#include "peripheral/gpio/plib_gpio.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
enum APP_KEY_STATE_T
{
    APP_KEY_STATE_RELEASE = 0x00,
    APP_KEY_STATE_SHORT_PRESS,
    APP_KEY_STATE_LONG_PRESS,
    APP_KEY_STATE_SHORT_PRESS_RELEASE,
    APP_KEY_STATE_DOUBLE_CLICK_PRESS,
    
    APP_KEY_STATE_END 
};

typedef enum APP_KEY_MSG_T
{
    APP_KEY_MSG_NULL = 0x00,
    APP_KEY_MSG_SHORT_PRESS,
    APP_KEY_MSG_LONG_PRESS,
    APP_KEY_MSG_DOUBLE_CLICK,
    
    APP_KEY_MSG_END 
}APP_KEY_MSG_T;
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@brief The structure contains the information about key element format. */
typedef struct APP_KEY_Elem_T
{
    uint8_t                 state;          /**< Key state. */
    uint8_t                 counter;        /**< Key counter for short press or long press. */
}APP_KEY_Elem_T;

/**@brief app_key callback function prototype. */
typedef void (*APP_KeyCb_T)(APP_KEY_MSG_T msg);
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_KEY_Init(void);
void APP_KEY_Scan(void);
void APP_KEY_MsgRegister(APP_KeyCb_T keyCb);
#endif
/*******************************************************************************
 End of File
 */
