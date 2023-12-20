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
#include "peripheral/eic/plib_eic.h"
#include "app.h"
#include <bdb/include/bdb.h>
#include "app_prov/app_prov.h"
#include <app_zigbee/zigbee_console/console.h>
#include "peripheral/gpio/plib_gpio.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define RESET_TO_FN_BUTTON  EIC_PIN_0
#define BUTTON_PRESSED      0
#define BUTTON_RELEASED     1
#define BUTTON_RELEASE_EVENT 1
#define BUTTON_EVENT_3_SEC_MIN  3000
#define BUTTON_EVENT_5_SEC_MIN 5000
#define BUTTON_EVENT_7_SEC_MIN  7000
#define BUTTON_EVENT_10_SEC_MIN  10000
#define BUTTON_EVENT_15_SEC_MIN  15000
#define BUTTON_DEFAULT_VALUE BUTTON_PRESSED
#define RESET_TO_FN_EVENT 1
#define START_PROV  2

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
static bool buttonPress = BUTTON_DEFAULT_VALUE;
extern BDB_CommissioningMode_t commissioningMode;
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
   if(event[0] == RESET_TO_FN_EVENT)
   {
       // PPS_REGS->PPS_RPB3G4R = 0U; //JK!$$
       // GPIO_PortOutputEnable(GPIO_PORT_B, (1<<3));
       // GPIO_PortSet(GPIO_PORT_B, (1<<3));  // GREEN LED indication
        PPS_REGS->PPS_RPB7G4R = 0U; //JK!$$
       GPIO_PortOutputEnable(GPIO_PORT_B, (1<<7));
       GPIO_PortSet(GPIO_PORT_B, (1<<7));  // GREEN LED indication
        BDB_ResetToFactoryNew(true);       
   }
   if(event[0] == START_PROV)
   {   
       if(!BDB_GetBdbNodeIsOnANetwork())
       {
            commissioningMode = BDB_COMMISSIONING_NWK_STEERING;
			BZ_Prov_StartCommissioning(commissioningMode);
            appSnprintf("Zigbee Device Initialized: Type Help for Commands\r\n");
            startComm = true;
       }    
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

        if(xButtonRelease > BUTTON_EVENT_10_SEC_MIN)
        {
            appMsg.msgId = APP_MSG_BUTTON_INT;
            appMsg.msgData[0] =  RESET_TO_FN_EVENT;

            OSAL_QUEUE_SendISR(&appData.appQueue, &appMsg);
        }
        else if(xButtonRelease > BUTTON_EVENT_5_SEC_MIN)
        {
            appMsg.msgId = APP_MSG_BUTTON_INT;
            appMsg.msgData[0] =  START_PROV;

            OSAL_QUEUE_SendISR(&appData.appQueue, &appMsg);
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