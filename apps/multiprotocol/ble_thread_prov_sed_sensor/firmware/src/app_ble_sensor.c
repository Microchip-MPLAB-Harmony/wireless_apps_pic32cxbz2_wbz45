/*******************************************************************************
  Application BLE Sensor Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_sensor.c

  Summary:
    This file contains the Application Transparent Server Role functions for this project.

  Description:
    This file contains the Application Transparent Server Role functions for this project.
    The implementation of demo mode is included in this file.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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
#include "definitions.h"
#include "app_trps/app_trps.h"
#include "sensors/inc/temp_sensor.h"
#include "app_timer.h"
#include "app.h"
#include "app_adv.h"
#include "app_ble_conn_handler.h"
#include "app_ble_sensor.h"
#include "app_error_defs.h"
#include "app_thread.h"
#include "app_thread_udp.h"
// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

/**@brief BLE sensor read periodic timer */
uint32_t bleSensTimer = 0;
APP_TRPS_SensorData_T bleSensorData = {{0,0}};
float lastNotifiedTemp = -50.0, lastAdvTemp = -50.0;
static bool waitToSleep = false;

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
APP_TRPS_NotifyData_T appTrpsSensorNotify[] = 
{
    BLE_SENSOR_DEFINE_CTRL_NOTIFY()
};

/* Init BLE Sensor Specific */
void APP_TRPS_Sensor_Init(void)
{
    /* Init TRPS profile with BLE sensor specific command structure*/
    APP_TRPS_Init(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,NULL,appTrpsSensorNotify,0,BLE_SENSOR_NOTIFY_LST_SIZE);
    
    /* Init Periodic application timer to do BLE sensor specific measurement like read temp sensor handled in APP_TRPS_Sensor_TimerHandler() */
    APP_TIMER_SetTimer(APP_TIMER_BLE_SENSOR, APP_TIMER_100MS, true);
}

/* Periodic 100ms once timer handler to read sensor data*/
void APP_TRPS_Sensor_TimerHandler(void)
{
    volatile float temperature=0, tempBack;
    uint16_t tempS = 0;
	bleSensTimer++;

    if( bleSensTimer%10)  // temp sensor read every 1sec once
	{
        tempBack = temperature = MCP9700_Temp_Celsius();
        
        if(temperature < 0)
        {
            temperature = temperature * (-1.0);
        
            tempS = (uint16_t) (temperature * 10);  // 1 decimal place
            
            tempS = tempS | 0x8000;  // Set the MSB to indicate negative temperature          
        }
        else
        {       
            tempS = (uint16_t) (temperature * 10);  // 1 decimal place         
        }
        
        bleSensorData.tempSens.lsb = (uint8_t) tempS;
        bleSensorData.tempSens.msb = (uint8_t) (tempS>>8);  

        if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED){
            if( (tempBack > (lastNotifiedTemp + 2)) || (tempBack < (lastNotifiedTemp-2)) )  //+/- 2�C above, only then do the notification
            {
                if(APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,TEMP_SENSOR_NFY)== APP_RES_SUCCESS)
                {
                    lastNotifiedTemp = tempBack;
                }    
            }
        }
        else
        {
            if( (tempBack > (lastAdvTemp + 2)) || (tempBack < (lastAdvTemp-2)) )  //+/- 2�C above, only then update advertisement payload
            {       
                APP_ADV_Init();
                lastAdvTemp = tempBack;                
            }
        }
    }
}

/* Do the BLE Sensor specific on disconnection  */
void APP_TRPS_Sensor_DiscEvtProc(void)
{
    lastNotifiedTemp = -50.0;
    if(waitToSleep)
    {
       /* start sleep timer */
       APP_TIMER_SetTimer(APP_TIMER_SED_TIMEOUT, APP_TIMER_2S, false);
       waitToSleep = false;
    }     
}

/* Fill Adv Beacon with BLE Sensor specific */
void APP_TRPS_Sensor_Beacon(uint8_t* ptr_data)
{ 
    uint8_t idx=0;
    //Service Data
    ptr_data[idx++] = APP_ADV_ADD_DATA_CLASS_BYTE;
    ptr_data[idx++] = APP_ADV_PROD_TYPE_COMBO;
    ptr_data[idx++] = RGB_ONOFF_STATUS_NFY;
    ptr_data[idx++] =  0;
    ptr_data[idx++] = TEMP_SENSOR_NFY;
    ptr_data[idx++] =  bleSensorData.tempSens.msb; // MSB
    ptr_data[idx++] =  bleSensorData.tempSens.lsb;  // LSB     
}

void APP_TRPS_Sensor_Thread_Temp_Sync(void)
{    
    DEVICE_DeepSleepWakeSrc_T wakeupSrc;
    uint8_t tempData[2] = {0};
    SYS_CONSOLE_MESSAGE("[UDP] Temperature Notified to FTD\r\n");
    tempData[0] = bleSensorData.tempSens.msb; // MSB
    tempData[1] = bleSensorData.tempSens.lsb; // LSB
    APP_Thread_UdpSend(tempData,2);
    DEVICE_GetDeepSleepWakeUpSrc(&wakeupSrc);
    /* start sleep timer */
    if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
    {
        waitToSleep = true;
    }
    else
    {
       /* start sleep timer */
       APP_TIMER_SetTimer(APP_TIMER_SED_TIMEOUT, APP_TIMER_2S, false); 
    }
}