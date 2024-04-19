/*******************************************************************************
  Application BLE Sensor Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_sensor.c

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
#include "definitions.h"
#include "app_trps/app_trps.h"
#include "sensors/inc/rgb_led.h"
#include "sensors/inc/temp_sensor.h"
#include "app_timer.h"
#include "app.h"
#include "app_adv.h"
#include "app_ble_conn_handler.h"
#include "app_ble_sensor.h"
#include "app_error_defs.h"
#include "app_thread.h"
#include "sensors/inc/hs2xy.h"
#include "app_thread_udp.h"
// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

/**@brief BLE sensor read periodic timer */
uint32_t bleSensTimer = 0;
APP_TRPS_SensorData_T bleSensorData = {LED_OFF,{0,0x7F,0x7F},{0,0}};
float lastNotifiedTemp = -50.0, lastAdvTemp = -50.0;
static bool updateAdvData = false;

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
 static uint8_t APP_TRPS_Sensor_LED_Ctrl(uint8_t *p_cmd);
 static uint8_t APP_TRPS_Sensor_Color_Ctrl(uint8_t *p_cmd);
 static void APP_TRPS_Sensor_Thread_Light_onOff_Sync(bool on);
 static void APP_TRPS_Sensor_Thread_Light_Color_Sync(uint8_t hue, uint8_t saturation, uint8_t level);

APP_TRPS_CmdResp_T appTrpsSensorCmdResp[] = 
{
    BLE_SENSOR_DEFINE_CTRL_CMD_RESP()
};

APP_TRPS_NotifyData_T appTrpsSensorNotify[] = 
{
    BLE_SENSOR_DEFINE_CTRL_NOTIFY()
};

void APP_TRPS_Sensor_HSV2XYL(uint8_t hue,uint8_t saturation,uint8_t level)
{
    uint32_t tmpColorX, tmpColorY;
 
    // Get the X and Y values for the required hue and saturation
    HS2XY((int)hue,(int)saturation,(unsigned int *)&tmpColorX,(unsigned int *)&tmpColorY);
    RGB_LED_SetLedColorXY(tmpColorX,tmpColorY);
    RGB_LED_SetBrightnessLevel(level);
}    

/* BLE Sensor RGB LED On/Off control through Mobile app */
 uint8_t APP_TRPS_Sensor_LED_Ctrl(uint8_t *p_cmd)
{
    if (p_cmd[3] == LED_OFF)
    {
        if(APP_ThreadGetDeviceRole() >= OT_DEVICE_ROLE_CHILD)
            APP_TRPS_Sensor_Thread_Light_onOff_Sync(false);
        else
            RGB_LED_Off();
        bleSensorData.rgbOnOffStatus = LED_OFF;
        SYS_CONSOLE_MESSAGE("[BLE] LED OFF\n\r");              
    }    
    else if (p_cmd[3] == LED_ON)
    {
        if(bleSensorData.rgbOnOffStatus == LED_OFF)
        {
            if(APP_ThreadGetDeviceRole() >= OT_DEVICE_ROLE_CHILD)
                APP_TRPS_Sensor_Thread_Light_onOff_Sync(true);
            else
                APP_TRPS_Sensor_HSV2XYL(bleSensorData.RGB_color.Hue,bleSensorData.RGB_color.Saturation,bleSensorData.RGB_color.Value);

        }    
        bleSensorData.rgbOnOffStatus = LED_ON;

        SYS_CONSOLE_MESSAGE("[BLE] LED ON\n\r");                
    }
    
    return SUCCESS;
}

/* Update Zigbee light On/Off specific variable when received from BLE */
 static void APP_TRPS_Sensor_Thread_Light_onOff_Sync(bool on)
{
    if(on)
    {
        APP_TRPS_Sensor_HSV2XYL(bleSensorData.RGB_color.Hue,bleSensorData.RGB_color.Saturation,bleSensorData.RGB_color.Value);        
        APP_TRPS_Sensor_Thread_Light_Color_Sync(bleSensorData.RGB_color.Hue,bleSensorData.RGB_color.Saturation,bleSensorData.RGB_color.Value);
    }
    else
    {
        APP_TRPS_Sensor_Thread_Light_Color_Sync(0,0,0);
        RGB_LED_Off();
    }
}
 
 /* Update Zigbee light colour specific variable when received from BLE */
 void APP_TRPS_Sensor_Thread_Light_Color_Sync(uint8_t hue, uint8_t saturation, uint8_t level)
{
    uint8_t udpData[3];
    udpData[0] = hue;
    udpData[1] = saturation;
    udpData[2] = level;
    APP_Thread_UdpSend(udpData,RGB_COLOR_NFY_LEN);
}

/* BLE Sensor RGB LED color control through Mobile app */
 uint8_t APP_TRPS_Sensor_Color_Ctrl(uint8_t *p_cmd)
{
    bleSensorData.RGB_color.Hue =  p_cmd[3];
    bleSensorData.RGB_color.Saturation =  p_cmd[4];
    bleSensorData.RGB_color.Value =  p_cmd[5];
    if (bleSensorData.RGB_color.Value > MAX_LIGHT_LEVEL)
        bleSensorData.RGB_color.Value = MAX_LIGHT_LEVEL;
    if (bleSensorData.RGB_color.Hue > 0xFE)
        bleSensorData.RGB_color.Hue = 0xFE;
    if (bleSensorData.RGB_color.Saturation > 0xFE)
        bleSensorData.RGB_color.Saturation = 0xFE;    
    
    if(bleSensorData.rgbOnOffStatus == LED_ON)
    {
        APP_TRPS_Sensor_HSV2XYL(bleSensorData.RGB_color.Hue,bleSensorData.RGB_color.Saturation,bleSensorData.RGB_color.Value);
        if(APP_ThreadGetDeviceRole() >= OT_DEVICE_ROLE_CHILD)
            APP_TRPS_Sensor_Thread_Light_Color_Sync(bleSensorData.RGB_color.Hue,bleSensorData.RGB_color.Saturation,bleSensorData.RGB_color.Value); 
        SYS_CONSOLE_MESSAGE("[BLE] COLOR SET\n\r");      
    }
    return SUCCESS;
}

  /* Update BLE light on/off specific variable when received from Thread */
void APP_TRPS_Sensor_BLE_light_onOff_Sync(bool lightState)
{
    bleSensorData.rgbOnOffStatus = lightState;
    
     if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
    {
         APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,RGB_ONOFF_STATUS_NFY);
    }
    else
    {  
        updateAdvData = true;
    }  
}

  /* Update BLE light colour specific variable when received from Thread */
void APP_TRPS_Sensor_BLE_light_Color_Sync(uint8_t hue, uint8_t sat)
{
    bleSensorData.RGB_color.Hue =  hue;
    bleSensorData.RGB_color.Saturation =  sat;
    
    if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
    {
        if(bleSensorData.rgbOnOffStatus)
           APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,RGB_COLOR_NFY);
    }
}

  /* Update BLE light brightness specific variable when received from Zigbee */
void APP_TRPS_Sensor_BLE_light_Brightness_Sync(uint8_t value)
{
    bleSensorData.RGB_color.Value =  value;
    
     if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
    {
        if(bleSensorData.rgbOnOffStatus)
            APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,RGB_COLOR_NFY);
    } 
}

/* Init BLE Sensor Specific */
void APP_TRPS_Sensor_Init(void)
{
    /* Init TRPS profile with BLE sensor specific command structure*/
    APP_TRPS_Init(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,appTrpsSensorCmdResp,appTrpsSensorNotify,BLE_SENSOR_CMD_RESP_LST_SIZE,BLE_SENSOR_NOTIFY_LST_SIZE);
    
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
            if( (tempBack > (lastNotifiedTemp + 2)) || (tempBack < (lastNotifiedTemp-2)) )  //+/- 2°C above, only then do the notification
            {
                if(APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,TEMP_SENSOR_NFY)== APP_RES_SUCCESS)
                {
                    lastNotifiedTemp = tempBack;
                    SYS_CONSOLE_PRINT("[BLE] Temperature Notified: %.2f DegC\n\r",lastNotifiedTemp);
                }    
            }
        }
        else
        {
            if( (tempBack > (lastAdvTemp + 2)) || (tempBack < (lastAdvTemp-2)) )  //+/- 2°C above, only then update advertisement payload
            {       
                APP_ADV_Init();
                updateAdvData = false;
                lastAdvTemp = tempBack;                
            }
        }
    }
    if(updateAdvData)
    {
       APP_ADV_Init();
       updateAdvData = false;
    }
}

/* Do the BLE Sensor specific on disconnection  */
void APP_TRPS_Sensor_DiscEvtProc(void)
{
    lastNotifiedTemp = -50.0;    
}

/* Fill Adv Beacon with BLE Sensor specific */
void APP_TRPS_Sensor_Beacon(uint8_t* ptr_data)
{ 
    uint8_t idx=0;
    //Service Data
    ptr_data[idx++] = APP_ADV_ADD_DATA_CLASS_BYTE;
    ptr_data[idx++] = APP_ADV_PROD_TYPE_COMBO;
    ptr_data[idx++] = RGB_ONOFF_STATUS_NFY;
    ptr_data[idx++] =  bleSensorData.rgbOnOffStatus;
    ptr_data[idx++] = TEMP_SENSOR_NFY;
    ptr_data[idx++] =  bleSensorData.tempSens.msb; // MSB
    ptr_data[idx++] =  bleSensorData.tempSens.lsb;  // LSB     
}

/* Toggle BLE light on/off */
void APP_TRPS_Sensor_BLE_light_toggle(void)
{
    if(1 == bleSensorData.rgbOnOffStatus)
    {
       bleSensorData.rgbOnOffStatus = 0;
       RGB_LED_Off();
       SYS_CONSOLE_MESSAGE("LED OFF\r\n");
    }
    else
    {
        bleSensorData.rgbOnOffStatus = 1;
        SYS_CONSOLE_MESSAGE("LED ON\r\n");
        APP_TRPS_Sensor_HSV2XYL(bleSensorData.RGB_color.Hue,bleSensorData.RGB_color.Saturation,bleSensorData.RGB_color.Value);
        APP_TRPS_Sensor_BLE_light_Color_Sync(bleSensorData.RGB_color.Hue,bleSensorData.RGB_color.Saturation);
        APP_TRPS_Sensor_BLE_light_Brightness_Sync(bleSensorData.RGB_color.Value);
    }
    
    if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
    {
         APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_BLE_SENSOR,RGB_ONOFF_STATUS_NFY);
    }
    else
    {  
        updateAdvData = true;
    }
}