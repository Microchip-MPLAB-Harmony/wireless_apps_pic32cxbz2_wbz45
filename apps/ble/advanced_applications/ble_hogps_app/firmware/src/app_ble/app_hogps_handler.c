/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
  Application BLE Profile Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_hogps_handler.c

  Summary:
    This file contains the Application BLE functions for this project.

  Description:
    This file contains the Application BLE functions for this project.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_hogps_handler.h"
#include "app_module.h"

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

void APP_HogpsEvtHandler(BLE_HOGPS_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_HOGPS_EVT_BOOT_MODE_ENTER_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_HOGPS_EVT_REPORT_MODE_ENTER_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_HOGPS_EVT_HOST_SUSPEND_ENTER_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_HOGPS_EVT_HOST_SUSPEND_EXIT_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

	    case BLE_HOGPS_EVT_INPUT_REPORT_WRITE_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_HOGPS_EVT_OUTPUT_REPORT_WRITE_IND:
        {
            static bool bCapsOn=false;
            /* TODO: implement your application code.*/
            if (p_event->eventField.evtOutputReportWrite.length == 1)
            {
                if (p_event->eventField.evtOutputReportWrite.p_data[0] & 0x02)//Caps lock
                {
                    APP_PressFewKeys(g_userKeysCAPON, APP_USER_KEYS_NUM);
                    bCapsOn=true;
                }
                else
                {
                    if (bCapsOn)
                    {
                        APP_PressFewKeys(g_userKeysCAPOF, APP_USER_KEYS_NUM);
                        bCapsOn=false;
                    }
                }
            }
        }
        break;

        case BLE_HOGPS_EVT_FEATURE_REPORT_WRITE_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_HOGPS_EVT_BOOT_KB_INPUT_REPORT_WRITE_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_HOGPS_EVT_BOOT_KB_OUTPUT_REPORT_WRITE_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_HOGPS_EVT_BOOT_M_INPUT_REPORT_WRITE_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;
    }
}
