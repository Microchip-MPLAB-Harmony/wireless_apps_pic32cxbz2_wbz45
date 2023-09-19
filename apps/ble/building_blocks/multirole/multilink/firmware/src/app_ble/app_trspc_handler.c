/*******************************************************************************
  Application BLE Profile Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_trspc_handler.c

  Summary:
    This file contains the Application BLE functions for this project.

  Description:
    This file contains the Application BLE functions for this project.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
#include <stdint.h>
#include "ble_trspc/ble_trspc.h"
#include "definitions.h"

#include "app_trspc_handler.h"


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

void APP_TrspcEvtHandler(BLE_TRSPC_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_TRSPC_EVT_UL_STATUS:
        {
            /* TODO: implement your application code.*/
        }
        break;
        
        case BLE_TRSPC_EVT_DL_STATUS:
        {
            /* TODO: implement your application code.*/
        }            
        break;

        case BLE_TRSPC_EVT_RECEIVE_DATA:
        {
            /* TODO: implement your application code.*/
            uint16_t data_len;
            uint8_t *data;
            // Retrieve received data length
            BLE_TRSPC_GetDataLength(p_event->eventField.onReceiveData.connHandle, &data_len);
            // Allocate memory according to data length
            data = OSAL_Malloc(data_len);
            if(data == NULL)
            break;
            // Retrieve received data
            BLE_TRSPC_GetData(p_event->eventField.onReceiveData.connHandle, data);
            // Output received data to UART
            SERCOM0_USART_Write((uint8_t *)"\r\nClient Data :", 15);
            SERCOM0_USART_Write(data, data_len);
            // Free memory
            OSAL_Free(data);
        }            
        break;

        case BLE_TRSPC_EVT_VENDOR_CMD:
        {
            /* TODO: implement your application code.*/
        }            
        break;

        case BLE_TRSPC_EVT_VENDOR_CMD_RSP:
        {
            /* TODO: implement your application code.*/
        }            
        break;

        case BLE_TRSPC_EVT_DISC_COMPLETE:
        {
            /* TODO: implement your application code.*/
        }            
        break;

        case BLE_TRSPC_EVT_ERR_NO_MEM:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;
    }
}