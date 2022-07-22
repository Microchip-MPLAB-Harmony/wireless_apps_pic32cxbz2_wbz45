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
uint16_t ret;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

void GPIO_LED_Initialize ( void )
{
    CFG_REGS->CFG_CFGCON0CLR = CFG_CFGCON0_JTAGEN_Msk;
          /* PORTA Initialization */
    /* PORTB Initialization */


    /* PPS Input Remapping */

    /* PPS Output Remapping */
 /*  PA  */
    GPIOA_REGS->GPIO_TRISSET = 0xFFFF; //Set all pins as input

    /*  PB  */
    GPIOB_REGS->GPIO_ANSELSET = 0x0040; //PB6 ANSEL for Temp sensor
    GPIOB_REGS->GPIO_TRISSET = 0xFFFF;  //Set all pins as input
    GPIOB_REGS->GPIO_CNPUSET = 0xF886;  //Pull up: PRB 1, 2, 7, 11, 12 , 13 ,14 , 15
    GPIOB_REGS->GPIO_CNPDSET = 0x0029;  //Pull down RB0,3,5 for LED

}

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
            GPIO_LED_Initialize();
            APP_BleStackInit();
            RTC_Timer32Start();
            
            // Enable Ext Adv
            BLE_GAP_ExtAdvEnableParams_T extAdvEnableParam;
            extAdvEnableParam.advHandle = 0x01; // Adv Set - 0x01
            extAdvEnableParam.duration = 0;
            extAdvEnableParam.maxExtAdvEvts = 0; // Enable Cont ADV 
            ret = BLE_GAP_SetExtAdvEnable(true, 0x01,  &extAdvEnableParam);
            if (ret == MBA_RES_SUCCESS)
            SERCOM0_USART_Write((uint8_t *)"Ext Adv Enabled\r\n", 17);  
            if (appInitialized)
            {

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
