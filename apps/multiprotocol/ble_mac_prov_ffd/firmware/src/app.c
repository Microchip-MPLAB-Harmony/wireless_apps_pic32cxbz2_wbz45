// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "app.h"
#include "definitions.h"
#include "app_ble.h"
#include "mac_api.h"
#include "app_ffd.h"
#include <time.h>
#include "app_prov/app_prov.h"
#include "app_adv.h"
#include "app_ble_sensor.h"
#include "app_ble_conn_handler.h"
/* === MACROS ============================================================== */

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;
uint32_t wbz451_silicon_revision = 0x00;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
/* Callback for Button Press EIC Interrupt */
//static void APP_Button_Callback(uintptr_t context);
// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/
void SystemReset(void)
{
#ifdef __FPGA__
  NVIC_SystemReset();
#else
  uint32_t temp;
  CFG_REGS->CFG_SYSKEY = 0x00000000; // Write junk to lock it if it is already unlocked
  CFG_REGS->CFG_SYSKEY = 0xAA996655U;
  CFG_REGS->CFG_SYSKEY = 0x556699AA;
 
  // Trigger Software Reset
  RCON_REGS->RCON_RSWRST = 0x1;
 
  // The read will trigger the reset to occur
  temp = RCON_REGS->RCON_RSWRST;
  (void)temp;
  // 4 NOP's to insure no other instructions get executed
  __NOP();
  __NOP();
  __NOP();
  __NOP();
#endif
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;


    appData.appQueue = xQueueCreate( 64, sizeof(APP_Msg_T) );
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
    wbz451_silicon_revision = DSU_REGS->DSU_DID;	
    
    if(wbz451_silicon_revision & (1 << 29)) // A2 Silicon // if((wbz451_silicon_revision >> 28) == (0x02))
    {  
      /* PPS Output Remapping */
      PPS_REGS->PPS_RPB0G1R = 11U;
      PPS_REGS->PPS_RPB3G4R = 12U;
      PPS_REGS->PPS_RPB5G3R = 11U;
    }
    else if((wbz451_silicon_revision >> 28) ==  (0x00)) // A0 silicon
    {
      /* PPS Output Remapping */
      PPS_REGS->PPS_RPB0G1R = 21U;
      PPS_REGS->PPS_RPB3G4R = 21U;
      PPS_REGS->PPS_RPB5G3R = 22U;
    }
    /* Register external button interrupt callback   */    
  //  EIC_CallbackRegister(BUTTON_1, APP_Button_Callback,ZERO);
}



/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    APP_Msg_T    appMsg[1];
    APP_Msg_T   *p_appMsg;
    p_appMsg=appMsg;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            //appData.appQueue = xQueueCreate( 10, sizeof(APP_Msg_T) );
            
            APP_BleStackInit();
            APP_UpdateLocalName(0, NULL);

            APP_TRPS_Sensor_Init();
            APP_Prov_TRPS_Init();
            APP_InitConnList();
            APP_ADV_Init();
           
            /* This routine deletes the Persistent items and this can be modified based on the user application */
            PDS_DeleteAll(true);
            
            if (appInitialized)
            {
                /* Initialize the FTD Demo application*/
                MAC_FFDDemoInit();
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            if (OSAL_QUEUE_Receive(&appData.appQueue, &appMsg, OSAL_WAIT_FOREVER))
            {

                if(p_appMsg->msgId==APP_MSG_BLE_STACK_EVT)
                {
                    // Pass BLE Stack Event Message to User Application for handling
                    APP_BleStackEvtHandler((STACK_Event_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_BLE_STACK_LOG)
                {
                    // Pass BLE LOG Event Message to User Application for handling
                    APP_BleStackLogHandler((BT_SYS_LogEvent_T *)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_MAC_PROV)
                {
                    App_Msg_Handle_Prov((APP_Msg_T*)p_appMsg->msgData);
                }
                else if(p_appMsg->msgId==APP_MSG_MAC_PROV_COMPLTE)
                {
                    //Provisioning complete is handled here. Here we sent the response to the mobile app
                    APP_Prov_TRPS_ProvComplete(p_appMsg->msgData[0]);
                }
                else if(p_appMsg->msgId==APP_MSG_MAC_FACTORY_RESET)
                {
                    //Disable the BLE advertisement  
                    APP_ADV_Ctrl(false);
                    SYS_CONSOLE_MESSAGE("System Reset\r\n");
                    /* This routine deletes the Persistent items and this can be modified based on the user application */
                    PDS_DeleteAll(true);
                    SystemReset(); 
                }
                else if(p_appMsg->msgId==APP_TIMER_ADV_CTRL_MSG)
                {
                    APP_BLE_Adv_TimerHandler();
                }
                else if(p_appMsg->msgId==APP_TIMER_BLE_SENSOR_MSG)
                {
                   APP_TRPS_Sensor_TimerHandler();
                }
                else
                {
                    Handle_MACMessages(appMsg);
                }



            }
            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

/*******************************************************************************
 End of File
 */
