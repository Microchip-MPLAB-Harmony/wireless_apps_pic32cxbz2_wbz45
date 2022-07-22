/*******************************************************************************
  BLE Button_led Service Source File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_button_led_svc.c

  Summary:
    This file contains the BLE Button_led Service functions for application user.

  Description:
    This file contains the BLE Button_led Service functions for application user.
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
#include <stddef.h>
#include "gatt.h"
#include "ble_util/byte_stream.h"
#include "ble_cms/ble_button_led_svc.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
/* Little Endian. */
#define UUID_BUTTON_LED_PRIMARY_SVC_LE         0xea, 0xce, 0xf4, 0xac, 0x1f, 0x88, 0xf8, 0xae, 0xd0, 0x42, 0x55, 0x52, 0x50, 0x48, 0x43, 0x4d

#define UUID_BUTTON_LED_CHARACTERISTIC_0_LE         0xea, 0xce, 0xf4, 0xbc, 0x1f, 0x88, 0xf8, 0xae, 0xd0, 0x42, 0x55, 0x52, 0x50, 0x48, 0x43, 0x4d
#define UUID_BUTTON_LED_CHARACTERISTIC_1_LE         0xea, 0xce, 0xf4, 0xcc, 0x1f, 0x88, 0xf8, 0xae, 0xd0, 0x42, 0x55, 0x52, 0x50, 0x48, 0x43, 0x4d

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

/* Primary Service Declaration */
static const uint8_t s_button_ledSvcUuid[] = {UUID_BUTTON_LED_PRIMARY_SVC_LE};
static const uint16_t s_button_ledSvcUuidLen = sizeof(s_button_ledSvcUuid);

/* Button_led Characteristic 0 Characteristic */
static const uint8_t s_button_ledChar0[] = {ATT_PROP_READ|ATT_PROP_NOTIFY, UINT16_TO_BYTES(BUTTON_LED_HDL_CHARVAL_0), UUID_BUTTON_LED_CHARACTERISTIC_0_LE};
static const uint16_t s_button_ledChar0Len = sizeof(s_button_ledChar0);

/* Button_led Characteristic 0 Characteristic Value */
static const uint8_t s_button_ledUuidChar0[] = {UUID_BUTTON_LED_CHARACTERISTIC_0_LE};
static uint8_t s_button_ledChar0Val[] = {};
static uint16_t s_button_ledChar0ValLen = sizeof(s_button_ledChar0Val);

/* Button_led Characteristic 0 Client Characteristic Configuration Descriptor */
static uint8_t s_button_ledCccChar0[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_button_ledCccChar0Len = sizeof(s_button_ledCccChar0);

/* Button_led Characteristic 1 Characteristic */
static const uint8_t s_button_ledChar1[] = {ATT_PROP_READ|ATT_PROP_WRITE_REQ, UINT16_TO_BYTES(BUTTON_LED_HDL_CHARVAL_1), UUID_BUTTON_LED_CHARACTERISTIC_1_LE};
static const uint16_t s_button_ledChar1Len = sizeof(s_button_ledChar1);

/* Button_led Characteristic 1 Characteristic Value */
static const uint8_t s_button_ledUuidChar1[] = {UUID_BUTTON_LED_CHARACTERISTIC_1_LE};
static uint8_t s_button_ledChar1Val[3] = {0x00, 0xFF, 0x00};
static uint16_t s_button_ledChar1ValLen = sizeof(s_button_ledChar1Val);

/* Attribute list for Button_led service */
static GATTS_Attribute_T s_button_ledList[] = {
    /* Service Declaration */
    {
        (uint8_t *) g_gattUuidPrimSvc,
        (uint8_t *) s_button_ledSvcUuid,
        (uint16_t *) & s_button_ledSvcUuidLen,
        sizeof (s_button_ledSvcUuid),
        0,
        PERMISSION_READ
    },
    /* Characteristic 0 Declaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_button_ledChar0,
        (uint16_t *) & s_button_ledChar0Len,
        sizeof (s_button_ledChar0),
        0,
        PERMISSION_READ
    },
    /* Characteristic 0 Value */
    {
        (uint8_t *) s_button_ledUuidChar0,
        (uint8_t *) s_button_ledChar0Val,
        (uint16_t *) & s_button_ledChar0ValLen,
        sizeof(s_button_ledChar0Val),
        SETTING_MANUAL_READ_RSP|SETTING_UUID_16,
        PERMISSION_READ
    },
    /* Client Characteristic Configuration Descriptor */
    {
        (uint8_t *) g_descUuidCcc,
        (uint8_t *) s_button_ledCccChar0,
        (uint16_t *) & s_button_ledCccChar0Len,
        sizeof (s_button_ledCccChar0),
        SETTING_CCCD,
        PERMISSION_READ|PERMISSION_WRITE
    },
    /* Characteristic 1 Declaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_button_ledChar1,
        (uint16_t *) & s_button_ledChar1Len,
        sizeof (s_button_ledChar1),
        0,
        PERMISSION_READ
    },
    /* Characteristic 1 Value */
    {
        (uint8_t *) s_button_ledUuidChar1,
        (uint8_t *) s_button_ledChar1Val,
        (uint16_t *) & s_button_ledChar1ValLen,
        sizeof(s_button_ledChar1Val),
        SETTING_UUID_16,
        PERMISSION_READ|PERMISSION_WRITE
    },
};

static const GATTS_CccdSetting_T s_button_ledCccdSetting[] = 
{
    {BUTTON_LED_HDL_CCCD_0, NOTIFICATION},
};

/* Button_led Service structure */
static GATTS_Service_T s_button_ledSvc = 
{
    NULL,
    (GATTS_Attribute_T *) s_button_ledList,
    (GATTS_CccdSetting_T const *)s_button_ledCccdSetting,
    BUTTON_LED_START_HDL,
    BUTTON_LED_END_HDL,
    1
};

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

uint16_t BLE_BUTTON_LED_Add()
{
    return GATTS_AddService(&s_button_ledSvc, (BUTTON_LED_END_HDL - BUTTON_LED_START_HDL + 1));
}
