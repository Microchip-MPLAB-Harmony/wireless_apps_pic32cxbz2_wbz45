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
#include "app_coap/app_coap_server.h"
#include "app_led.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
uint8_t databuffer = 'G';

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void APP_CoapServerCmdChangeState(uint8_t* command)
{
    switch(command[0])
    {
        case 'R':
        {
            databuffer = command[0];
            APP_LED_Toggle(APP_LED_TYPE_RED);
        }
        break;
        case 'G':
        {
           databuffer = command[0];
           APP_LED_Toggle(APP_LED_TYPE_GREEN);
        }
        break;
        case 'B':
        {
           databuffer = command[0];
           APP_LED_Toggle(APP_LED_TYPE_BLUE);
        }
        break;
        default:
            //unknown command
        break;
    }
}

static void APP_CoapServerGetDataToSend(uint8_t resourceID, uint8_t** dataBuffer,uint16_t* dataSize)
{
   switch(resourceID)
   {
        case APP_LED_RESOURCE:
        {
            *dataBuffer = &databuffer;
            *dataSize = 1;
        }
        break;

        default:
        break;
   } 
}

static void APP_CoapServerGetByteToRead(uint8_t resourceID, uint16_t* bytesToRead)
{
   switch(resourceID)
   {
        case APP_LED_RESOURCE:
        {
            *bytesToRead = 1;
        }
        break;

        default:
        break;
   } 
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************


void APP_CoapServerDataReceiveHandler(uint8_t resourceID, uint8_t* dataBuff)
{
    switch(resourceID)
    {
        case APP_LED_RESOURCE:
        {
            SYS_CONSOLE_PRINT("AppLog: Received Data = %c\r\n",dataBuff[0]);
            APP_CoapServerCmdChangeState(dataBuff);
        }
        break;
        
        default:
        break;
    }
}

void APP_CoapAppInit()
{
    APP_CoapStart();
    APP_CoapServerAddResource();
    APP_CoapServerRegisterCallback(APP_CoapServerGetDataToSend,APP_CoapServerGetByteToRead);
}

/* *****************************************************************************
 End of File
 */
