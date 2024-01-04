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
  Application CoAP Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_coap.c

  Summary:
    This file contains the Application CoAP for this project.

  Description:
    This file contains the Application CoAP for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_coap/app_coap_common.h"
#include "app_coap/app_coap_client.h"
#include "app_led.h"
#include "app_thread.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_CoapClientCmdSetStatus(uint8_t* command)
{
    switch(command[0])
    {
        case 'R':
        {
            APP_LED_Toggle(APP_LED_TYPE_RED);            
        }
        break;
        case 'G':
        {       
           APP_LED_Toggle(APP_LED_TYPE_GREEN);
        }
        break;
        case 'B':
        {            
           APP_LED_Toggle(APP_LED_TYPE_BLUE);        
        }
        break;
        default:
            //unknown command
        break;
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_CoapClientAckReceiveHandler(uint8_t resourceID)
{
    switch(resourceID)
    {
        case APP_LED_RESOURCE:
        {
            SYS_CONSOLE_MESSAGE("AppLog:ACK Received\r\n");
            DEVICE_DeepSleepWakeSrc_T wakeupSrc;
            DEVICE_GetDeepSleepWakeUpSrc(&wakeupSrc);
            if(wakeupSrc == DEVICE_DEEP_SLEEP_WAKE_INT0)
            {
               SYS_CONSOLE_MESSAGE("AppLog:Send Get Request\r\n");
               APP_CoapClientGetReq(APP_LED_RESOURCE,1);
               APP_ThreadResetActiveTimer();       
            }
        }
        break;
        
        default:
        break;
    }
}

void APP_CoapClientDataReceiveHandler(uint8_t resourceID, uint8_t* dataBuff)
{
    switch(resourceID)
    {
        case APP_LED_RESOURCE:
        {
            SYS_CONSOLE_PRINT("AppLog:Response Received = %c\r\n",dataBuff[0]);
            APP_CoapClientCmdSetStatus(dataBuff);
        }
        break;
        
        default:
        break;
    }
}


void APP_CoapAppInit()
{
    APP_CoapStart();
}

/* *****************************************************************************
 End of File
 */
