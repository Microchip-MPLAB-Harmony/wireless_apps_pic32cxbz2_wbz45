/*******************************************************************************
  Application BLE Custom Service Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_custom_service.c

  Summary:
    This file contains the Application Transparent Server Role functions for this project.

  Description:
    This file contains the Application Transparent Server Role functions for this project.
    The implementation of demo mode is included in this file.
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
#include <stdint.h>
#include "peripheral/eic/plib_eic.h"
#include "app.h"
#include "app_ble_handler.h"
#include "app_ble_custom_service.h"
#include "ble/service_ble/ble_cms/ble_button_led_svc.h"
#include "bsp/bsp.h"
#include "system/console/sys_console.h"

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

/**@brief BLE sensor read periodic timer */
APP_CS_Data_T bleCSdata = {LED_ON,{LED_ON,LED_ON,LED_ON}};
uint16_t G_ConnHandle;
//float lastNotifiedTemp = -50.0, lastAdvTemp = -50.0;
//bool b_button_debounce = false;

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

/* BLE Custom Service RGB LED color control through Mobile app */
uint8_t APP_CustomService_RGB_Handler(uint8_t *p_cmd)
{
    
    bleCSdata.RGB_LED.Red = p_cmd[0];
    bleCSdata.RGB_LED.Green = p_cmd[1];
    bleCSdata.RGB_LED.Blue = p_cmd[2];
    
    SYS_CONSOLE_PRINT ("[BLE] RGB LED data 0x%X 0x%X 0x%X \r\n",p_cmd[0],p_cmd[1],p_cmd[2]);
    if(bleCSdata.rgbOnOffStatus == LED_ON)
    {    
        if(bleCSdata.RGB_LED.Red)
            RGB_LED_RED_On();
        else
            RGB_LED_RED_Off();
        if(bleCSdata.RGB_LED.Green)
            RGB_LED_GREEN_On();
        else
            RGB_LED_GREEN_Off(); 
        if(bleCSdata.RGB_LED.Blue)
            RGB_LED_BLUE_On();
        else
            RGB_LED_BLUE_Off();        
        
        SYS_CONSOLE_MESSAGE("[BLE] RGB SET\n\r");        
    }
    return SUCCESS;
}

/* Callback for BLE Custom Service RGB LED on/off */
void APP_CustomService_RGB_Callback(uint8_t * context)
{
    APP_Msg_T appMsg;
    appMsg.msgId = APP_MSG_BLE_CS_LED_EVT;
    memcpy(appMsg.msgData, context, 3);
    OSAL_QUEUE_Send(&appData.appQueue, &appMsg,0);
}

/* Callback for BLE Custom Service LED on/off through on board button interrupt */
static void APP_CustomService_Button_Callback(uintptr_t context)
{

    APP_Msg_T appMsg;
    appMsg.msgId = APP_MSG_BLE_CS_BUTTON_EVT;
    OSAL_QUEUE_SendISR(&appData.appQueue, &appMsg);
  
}

void APP_CustomService_SendNotification()
{
    GATTS_HandleValueParams_T *p_hvParams;
    
    if(G_ConnHandle)
    {
        p_hvParams = OSAL_Malloc(sizeof(GATTS_HandleValueParams_T));
        if (p_hvParams != NULL)
        {
            p_hvParams->charHandle = BUTTON_LED_HDL_CHARVAL_0;
            p_hvParams->charLength = 1;
            p_hvParams->charValue[0] = bleCSdata.rgbOnOffStatus;
            //memcpy(&p_hvParams->charValue[1], p_commandPayload, commandLength);
            p_hvParams->sendType = ATT_HANDLE_VALUE_NTF;
            GATTS_SendHandleValue(G_ConnHandle, p_hvParams);
            OSAL_Free(p_hvParams);
        }
        else
        {
            // Do nothing
        }
    }
}

/* BLE Custom Service LED on/off control event through on board button */
void APP_CustomService_Button_Handler(void)
{
    SYS_CONSOLE_PRINT("[BLE] Custom Service Button Event : ");
    if(bleCSdata.rgbOnOffStatus == LED_OFF)
    {
        bleCSdata.rgbOnOffStatus = LED_ON;
        if(bleCSdata.RGB_LED.Red)
            RGB_LED_RED_On();
        else
            RGB_LED_RED_Off();
        if(bleCSdata.RGB_LED.Green)
            RGB_LED_GREEN_On();
        else
            RGB_LED_GREEN_Off(); 
        if(bleCSdata.RGB_LED.Blue)
            RGB_LED_BLUE_On();
        else
            RGB_LED_BLUE_Off(); 
        USER_LED_On();
        SYS_CONSOLE_PRINT(" RGB LED ON\n\r");
    }
    else
    {
        bleCSdata.rgbOnOffStatus = LED_OFF;
        RGB_LED_RED_Off();
        RGB_LED_GREEN_Off(); 
        RGB_LED_BLUE_Off(); 
        USER_LED_Off();
        SYS_CONSOLE_PRINT(" RGB LED OFF\n\r");
    }
        
    APP_CustomService_SendNotification();

}


/* Init BLE Custom Service Specific */
void APP_Button_Init(void)
{
    /* Register external button interrupt callback   */    
    EIC_CallbackRegister(BUTTON_1,APP_CustomService_Button_Callback,0);
}
