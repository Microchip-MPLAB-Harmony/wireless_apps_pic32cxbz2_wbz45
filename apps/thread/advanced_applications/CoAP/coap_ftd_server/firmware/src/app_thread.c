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
  Application Thread Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_thread.c

  Summary:
    This file contains the Application Thread Source code for this project.

  Description:
    This file contains the Application Thread Source code for this project.
 *******************************************************************************/
 
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"
#include "openthread/thread.h"
#include "app_thread/app_thread_common.h"
#include "app_thread.h"
#include "app_led.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

static otDeviceRole s_appthreadstate = OT_DEVICE_ROLE_DISABLED;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_ThreadRoleChangeHandler(otChangedFlags aFlags)
{
    static bool deviceattached = false;
    switch(aFlags)
    {
        case OT_DEVICE_ROLE_CHILD:
        {
            SYS_CONSOLE_MESSAGE("AppLog: Device enabled as Child\r\n");
            deviceattached = true;
            APP_LED_StatusLed(true);
        }
        break;

        case OT_DEVICE_ROLE_ROUTER:
        {
            SYS_CONSOLE_MESSAGE("AppLog: Device enabled as Router\r\n");
            deviceattached = true;
            APP_LED_StatusLed(true);
        }
        break;

        case OT_DEVICE_ROLE_LEADER:
        {
            SYS_CONSOLE_MESSAGE("AppLog: Device enabled as Leader\r\n");
            deviceattached = true;
            APP_LED_StatusLed(true);
        }
        break;

        case OT_DEVICE_ROLE_DETACHED:
        {
            if(deviceattached)
            {
               SYS_CONSOLE_MESSAGE("AppLog: Device is Detached\r\n");
               deviceattached = false;
               APP_LED_StatusLed(false);
            }
        }
        break;

        case OT_DEVICE_ROLE_DISABLED:
        {
            SYS_CONSOLE_MESSAGE("AppErr: Device state is disabled. Reset the device!!!!\r\n");
            deviceattached = false;
            APP_LED_StatusLed(false);
        }
        break;

        default:
        break;
    }  
}


static void APP_ThreadHandler(otChangedFlags aFlags, void *aContext)
{
    //Check if Device role is changed
    if(aFlags & OT_CHANGED_THREAD_ROLE)
    {
        otInstance * pinstance = APP_ThreadGetInstance();
        s_appthreadstate = otThreadGetDeviceRole(pinstance);
        APP_ThreadRoleChangeHandler(s_appthreadstate);
    }
    /* TODO: Not handling other flags as of now, add if required*/
}


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

otChangedFlags APP_ThreadGetDeviceRole(void)
{
    return s_appthreadstate;
}

void APP_ThreadAppInit(void)
{
  APP_ThreadInit(APP_ThreadHandler);
  APP_ThreadSetNwParameters();
  APP_ThreadNwStart();
  APP_LED_StatusLed(false);
}
