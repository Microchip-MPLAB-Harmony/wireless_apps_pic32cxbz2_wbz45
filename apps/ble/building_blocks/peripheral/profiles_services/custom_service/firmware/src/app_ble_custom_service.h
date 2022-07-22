/*******************************************************************************
  Application BLE Sensor Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_custom_service.h

  Summary:
    This file contains the Application ble custom service functions for this project.

  Description:
    This file contains the Application Transparent Server Role functions for this project.
    The implementation of demo mode is included in this file.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END


#ifndef APP_BLE_CS_H
#define APP_BLE_CS_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
//#include "app_trps.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define GREEN_H  85  // 240deg
#define GREEN_S  255  // 100%
#define GREEN_V  255  //100%

#define BUTTON_1  EIC_PIN_0

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
enum APP_CUSTOM_SERVICE_STATUS_CODE_T
{
    SUCCESS = 0x00,
    //OPCODE_NOT_SUPPORTED = 0x01,
    INVALID_PARAMETER = 0x02,
    OPERATION_FAILED = 0x03
};

enum APP_RGB_LED_STATUS_T
{
    LED_OFF = 0x00,
    LED_ON = 0x01,
};

/**@brief The structure contains the information about BLE custom service rgb button data */
typedef struct
{
    uint8_t    rgbOnOffStatus;        /**< RGB LED On/Off Status */
    struct __attribute__ ((packed))
    {
        uint8_t    Red;         /**The array contains the information about RGB colour value in HSV format. */
        uint8_t    Green;
        uint8_t    Blue;
    }RGB_LED;
} APP_CS_Data_T;

extern APP_CS_Data_T bleCSdata;
extern uint16_t G_ConnHandle;
// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_Button_Init(void);

void APP_CustomService_Button_Handler(void);

uint8_t APP_CustomService_RGB_Handler(uint8_t *);

void APP_CustomService_RGB_Callback(uint8_t *);

#endif
