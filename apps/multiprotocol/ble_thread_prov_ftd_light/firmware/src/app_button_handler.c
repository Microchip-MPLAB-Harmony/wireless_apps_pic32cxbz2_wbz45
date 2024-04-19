/*******************************************************************************
  Button Control

  Company:
    Microchip Technology Inc.

  File Name:
    app_button_handler.c

  Summary:
    This file contains functions to control on board button

  Description:
     This file contains functions to control on board button
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "definitions.h"
#include "peripheral/eic/plib_eic.h"
#include "app.h"
#include "app_prov/app_prov.h"
#include "peripheral/gpio/plib_gpio.h"
#include "app_thread.h"
#include "app_thread_udp.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define RESET_TO_FN_BUTTON  EIC_PIN_0
#define BUTTON_PRESSED      0
#define BUTTON_RELEASED     1
#define BUTTON_RELEASE_EVENT 1
#define BUTTON_EVENT_LONG_PRESS_TIME  3000
#define BUTTON_DEFAULT_VALUE BUTTON_PRESSED

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
static bool buttonPress = BUTTON_DEFAULT_VALUE;
static TickType_t xCurrentTime ,xButtonPress,xButtonRelease;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
/**************************************************************************//**
Reset To Factory New Function initialisation based on the button press.
******************************************************************************/
void APP_Button_Handler(uint8_t* event)
{
   if(event[0] == APP_LED_TOGGLE)
   {
      uint8_t cmd = APP_LED_TOGGLE;
      APP_Thread_UdpSend(&cmd,1); 
   }
}

/* Callback from board button interrupt*/
static void APP_Button_Callback(uintptr_t context)
{
    APP_Msg_T appMsg;    
    xCurrentTime = xTaskGetTickCount();
    
    if(buttonPress == BUTTON_RELEASE_EVENT)    
    {
        xButtonRelease = xCurrentTime - xButtonPress;

        if(xButtonRelease < BUTTON_EVENT_LONG_PRESS_TIME)
        {
            SYS_CONSOLE_MESSAGE("[BUTTON]Short Press\r\n");
            if(APP_ThreadGetDeviceRole() >= OT_DEVICE_ROLE_CHILD)
            {
                appMsg.msgId = APP_MSG_BUTTON_INT;
                appMsg.msgData[0] =  APP_LED_TOGGLE;

                OSAL_QUEUE_SendISR(&appData.appQueue, &appMsg);
            }
        }
        else if(xButtonRelease > BUTTON_EVENT_LONG_PRESS_TIME)
        {
            SYS_CONSOLE_MESSAGE("[BUTTON]Long Press\r\n");
        }        
        buttonPress = BUTTON_DEFAULT_VALUE;
        xButtonPress = BUTTON_DEFAULT_VALUE;
        xButtonRelease = BUTTON_DEFAULT_VALUE;
    }
    else if(buttonPress == BUTTON_PRESSED)
    {    
        xButtonPress = xCurrentTime;
        buttonPress = BUTTON_RELEASE_EVENT;
    }
 }


/**************************************************************************//**
Button initialisation  of pic32cx on board 
******************************************************************************/
void App_Button_Init(void)
{
    /* Register external button interrupt callback   */ 
    EIC_CallbackRegister(RESET_TO_FN_BUTTON,APP_Button_Callback,0);
}