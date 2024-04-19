/*******************************************************************************
  Application BLE Sensor Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_sensor.h

  Summary:
    This file contains the Application Transparent Server Role functions for this project.

  Description:
    This file contains the Application Transparent Server Role functions for this project.
    The implementation of demo mode is included in this file.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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


#ifndef APP_BLE_SENSOR_H
#define APP_BLE_SENSOR_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_trps/app_trps.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

// Define for BLE Sensor Ctrl Commands

#define APP_TRP_VENDOR_OPCODE_BLE_SENSOR  0x8A

#define    RGB_ONOFF_STATUS_NFY  0x40
#define    TEMP_SENSOR_NFY       0x41

#define    TEMP_SENSOR_NFY_LEN        0x2

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@brief The structure contains the information about BLE sensor data */
typedef struct
{
    struct __attribute__ ((packed))
    {
        uint8_t    msb;
        uint8_t    lsb;
    }tempSens;
} APP_TRPS_SensorData_T;

#define BLE_SENSOR_NOTIFY_LST_SIZE   1
#define BLE_SENSOR_DEFINE_CTRL_NOTIFY()                   \
        { TEMP_SENSOR_NFY, TEMP_SENSOR_NFY_LEN, (uint8_t *)&bleSensorData.tempSens},       \

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_TRPS_Sensor_Init(void);

void APP_TRPS_Sensor_DiscEvtProc(void);

void APP_TRPS_Sensor_TimerHandler(void);

void APP_TRPS_Sensor_Beacon(uint8_t* ptr_data);

void APP_TRPS_Sensor_Thread_Temp_Sync(void);

#endif
