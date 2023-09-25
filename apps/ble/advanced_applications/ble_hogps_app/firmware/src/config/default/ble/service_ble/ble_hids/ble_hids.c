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
  BLE Human Interface Device Service Source File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_hids.c

  Summary:
    This file contains the BLE Human Interface Device Service functions for application user.

  Description:
    This file contains the BLE Human Interface Device Service functions for application user.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdint.h>
#include "mba_error_defs.h"
#include "gatt.h"
#include "ble_util/byte_stream.h"
#include "ble_hids.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define HID_CD_HID                                          0x0111      /**< Version number (HID v1.11) of base USB HID Sepcification. */

#define HID_COUNTRY_CODE_NONE                               0x00        /**< Country code of the localized hardware. Value 0x00 represents not localized. */
#define HID_COUNTRY_CODE_JAPAN                              0x0F        /**< Country code of the localized hardware: Japan. */
#define HID_COUNTRY_CODE_TAIWAN                             0x1E        /**< Country code of the localized hardware: Taiwan. */
#define HID_COUNTRY_CODE_US                                 0x11        /**< Country code of the localized hardware: US. */

#define HID_FLAG_NONE                                       0x00        /**< Default value of HID infomation characteristic. */
#define HID_FLAG_REMOTE_WAKE                                0x01        /**< HID Device is capable of sending a wake-signal to a HID Host or not. */
#define HID_FLAG_NORMALLY_CONNECTABLE                       0x02        /**< HID Device will be advertising when bonded but not connected or not. */

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static const uint8_t s_svcUuidHids[ATT_UUID_LENGTH_2] =                 {UINT16_TO_BYTES(UUID_HUMAN_INTERFACE_DEVICE_SERVICE)};
#if defined(HIDS_KEYBOARD_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
static const uint8_t s_chUuidHidBootKbInReport[ATT_UUID_LENGTH_2] =     {UINT16_TO_BYTES(UUID_HID_BOOT_KB_INPUT_REPORT)};
static const uint8_t s_chUuidHidBootKbOutReport[ATT_UUID_LENGTH_2] =    {UINT16_TO_BYTES(UUID_HID_BOOT_KB_OUTPUT_REPORT)};
#endif
#if defined(HIDS_MOUSE_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
static const uint8_t s_chUuidHidBootMouseInReport[ATT_UUID_LENGTH_2] =  {UINT16_TO_BYTES(UUID_HID_BOOT_MOUSE_INPUT_REPORT)};
#endif
static const uint8_t s_chUuidHidInfo[ATT_UUID_LENGTH_2] =               {UINT16_TO_BYTES(UUID_HID_INFORMATION)};
static const uint8_t s_chUuidHidReportMap[ATT_UUID_LENGTH_2] =          {UINT16_TO_BYTES(UUID_HID_REPORT_MAP)};
static const uint8_t s_chUuidHidCtrl[ATT_UUID_LENGTH_2] =               {UINT16_TO_BYTES(UUID_HID_CONTROL_POINT)};
static const uint8_t s_chUuidHidReport[ATT_UUID_LENGTH_2] =             {UINT16_TO_BYTES(UUID_HID_REPORT)};
#ifdef HIDS_BOOT_PROTOCOL_MODE_SUPPORT
static const uint8_t s_chUuidHidProtocolMode[ATT_UUID_LENGTH_2] =       {UINT16_TO_BYTES(UUID_HID_PROTOCOL_MODE)};
#endif

/* Human Interface Device Service Declaration */
static const uint16_t s_svcUuidHidsLen = sizeof(s_svcUuidHids);

/* HIDS Include declaration */
static const uint8_t s_hidsInclude1Val[] = {UINT16_TO_BYTES(BAS_START_HDL), UINT16_TO_BYTES(BAS_END_HDL), UINT16_TO_BYTES(UUID_BATTERY_SERVICE)};
static uint16_t s_hidsInclude1ValLen = sizeof(s_hidsInclude1Val);

#ifdef HIDS_BOOT_PROTOCOL_MODE_SUPPORT
/* HID Protocol Mode Characteristic */
static const uint8_t s_charHidProtocolMode[] = {(ATT_PROP_READ|ATT_PROP_WRITE_CMD), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_HID_PROTOCOL_MODE), UINT16_TO_BYTES(UUID_HID_PROTOCOL_MODE)};
static const uint16_t s_charHidProtocolModeLen = sizeof(s_charHidProtocolMode);

/* HID Protocol Mode Value */
static uint8_t s_hidProtocolModeVal[] = {HID_MODE_REPORT_PROTOCOL};
static const uint16_t s_hidProtocolModeValLen = sizeof(s_hidProtocolModeVal);
#endif

#ifdef HIDS_KEYBOARD_SUPPORT
/* HID Report Characteristic (Keyboard Input Report) */
static const uint8_t s_charHidReportInputKB[] = {(ATT_PROP_READ|ATT_PROP_WRITE_REQ|ATT_PROP_NOTIFY), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_KB_INPUT_REPORT), UINT16_TO_BYTES(UUID_HID_REPORT)};
static const uint16_t s_charHidReportInputKBLen = sizeof(s_charHidReportInputKB);

/* HID Report Value (Keyboard Input Report) */
static uint8_t s_hidReportInputValKB[HID_REPORT_LENGTH_KB_INPUT]={0x00};
static uint16_t s_hidReportInputValKBLen = sizeof(s_hidReportInputValKB);

/* HID Report Client Characteristic Configuration Descriptor (Keyboard Input Report) */
static uint8_t s_descCccReportInputKB[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccReportInputKBLen = sizeof(s_descCccReportInputKB);

/* HID Report Reference Descriptor (Keyboard Input Report) */
static uint8_t s_descReportRefInputKB[] = {HID_REPORT_ID_KB, HID_REPORT_TYPE_INPUT};
static const uint16_t s_descReportRefInputKBLen = sizeof(s_descReportRefInputKB);
#endif

#ifdef HIDS_MOUSE_SUPPORT
/* HID Report Characteristic (Mouse Button Input Report) */
static const uint8_t s_charHidReportInputMB[] = {(ATT_PROP_READ|ATT_PROP_WRITE_REQ|ATT_PROP_NOTIFY), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_MB_INPUT_REPORT), UINT16_TO_BYTES(UUID_HID_REPORT)};
static const uint16_t s_charHidReportInputMBLen = sizeof(s_charHidReportInputMB);

/* HID Report Value (Mouse Button Input Report) */
static uint8_t s_hidReportInputValMB[HID_REPORT_LENGTH_MB_INPUT]={0x00};
static uint16_t s_hidReportInputValMBLen = sizeof(s_hidReportInputValMB);

/* HID Report Client Characteristic Configuration Descriptor (Mouse Button Input Report) */
static uint8_t s_descCccReportInputMB[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccReportInputMBLen = sizeof(s_descCccReportInputMB);

/* HID Report Reference Descriptor (Mouse Button Input Report) */
static uint8_t s_descReportRefInputMB[] = {HID_REPORT_ID_MB, HID_REPORT_TYPE_INPUT};
static const uint16_t s_descReportRefInputMBLen = sizeof(s_descReportRefInputMB);

/* HID Report Characteristic (Mouse Motion Input Report) */
static const uint8_t s_charHidReportInputMM[] = {(ATT_PROP_READ|ATT_PROP_WRITE_REQ|ATT_PROP_NOTIFY), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_MM_INPUT_REPORT), UINT16_TO_BYTES(UUID_HID_REPORT)};
static const uint16_t s_charHidReportInputMMLen = sizeof(s_charHidReportInputMM);

/* HID Report Value (Mouse Motion Input Report) */
static uint8_t s_hidReportInputValMM[HID_REPORT_LENGTH_MM_INPUT]={0x00};
static uint16_t s_hidReportInputValMMLen = sizeof(s_hidReportInputValMM);

/* HID Report Client Characteristic Configuration Descriptor (Mouse Motion Input Report) */
static uint8_t s_descCccReportInputMM[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccReportInputMMLen = sizeof(s_descCccReportInputMM);

/* HID Report Reference Descriptor (Mouse Motion Input Report) */
static uint8_t s_descReportRefInputMM[] = {HID_REPORT_ID_MM, HID_REPORT_TYPE_INPUT};
static const uint16_t s_descReportRefInputMMLen = sizeof(s_descReportRefInputMM);
#endif

#ifdef HIDS_KEYBOARD_SUPPORT
/* HID Report Characteristic (Output type) */
static const uint8_t s_charHidReportOutputKB[] = {(ATT_PROP_READ|ATT_PROP_WRITE_REQ|ATT_PROP_WRITE_CMD), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_KB_OUTPUT_REPORT), UINT16_TO_BYTES(UUID_HID_REPORT)};
static const uint16_t s_charHidReportOutputKBLen = sizeof(s_charHidReportOutputKB);

/* HID Report Value (Output type) */
static uint8_t s_hidReportOutputValKB[HID_REPORT_LENGTH_KB_OUTPUT]={0x00};
static uint16_t s_hidReportOutputValKBLen = sizeof(s_hidReportOutputValKB);

/* HID Report Reference (Output type) Descriptor */
static uint8_t s_descReportRefOutputKB[] = {HID_REPORT_ID_KB, HID_REPORT_TYPE_OUTPUT};
static const uint16_t s_descReportRefOutputKBLen = sizeof(s_descReportRefOutputKB);
#endif

/* HID Report Map Characteristic */
static const uint8_t s_charHidReportMap[] = {ATT_PROP_READ, UINT16_TO_BYTES(HIDS_HDL_CHARVAL_REPORT_MAP), UINT16_TO_BYTES(UUID_HID_REPORT_MAP)};
static const uint16_t s_charHidReportMapLen = sizeof(s_charHidReportMap);

/* HID Report Map Value */
static const uint8_t s_hidReportMapVal[] = {

#ifdef HIDS_KEYBOARD_SUPPORT
    0x05, 0x01,                 // Usage Page (Generic Desktop) */
    0x09, 0x06,                 // Usage (Keyboard) */
    0xA1, 0x01,                 // Collection (Application) */
    0x75, 0x01,                 //   Report Size (1)
    0x95, 0x08,                 //   Report Count (8)
    0x05, 0x07,                 //   Usage Page (Key Codes)
    0x19, 0xe0,                 //   Usage Minimum (224)
    0x29, 0xe7,                 //   Usage Maximum (231)
    0x15, 0x00,                 //   Logical Minimum (0)
    0x25, 0x01,                 //   Logical Maximum (1)
    0x81, 0x02,                 //   Input (Data, Variable, Absolute)
    0x95, 0x01,                 //   Report Count (1)
    0x75, 0x08,                 //   Report Size (8)
    0x81, 0x01,                 //   Input (Constant)
    0x95, 0x05,                 //   Report Count (5)
    0x75, 0x01,                 //   Report Size (1)
    0x05, 0x08,                 //   Usage Page (Page# for LEDs)
    0x19, 0x01,                 //   Usage Minimum (1)
    0x29, 0x05,                 //   Usage Maximum (5)
    0x91, 0x02,                 //   Output (Data, Variable, Absolute), Led report
    0x95, 0x01,                 //   Report Count (1)
    0x75, 0x03,                 //   Report Size (3)
    0x91, 0x01,                 //   Output (Constant), for padding
    0x95, 0x06,                 //   Report Count (6)
    0x75, 0x08,                 //   Report Size (8)
    0x15, 0x00,                 //   Logical Minimum (0)
    0x25, 0x65,                 //   Logical Maximum (101)
    0x05, 0x07,                 //   Usage Page (Key codes)
    0x19, 0x00,                 //   Usage Minimum (0)
    0x29, 0x65,                 //   Usage Maximum (101)
    0x81, 0x00,                 //   Input (Data, Array) Key array(6 bytes)
    0xC0,                       // End Collection (Application)
#endif

#ifdef HIDS_MOUSE_SUPPORT
    0x05, 0x01,                 // Usage Page (Generic Desktop Controls)
    0x09, 0x02,                 // Usage (Mouse)
    0xA1, 0x01,                 // Collection (Application)
    0x85, HID_REPORT_ID_MB,     //   Report Id HID_REPORT_ID_MB (Mouse buttons + scroll/pan)
    0x09, 0x01,                 //   Usage (Pointer)
    0xA1, 0x00,                 //   Collection (Physical)
    0x95, 0x05,                 //     Report Count (5)
    0x75, 0x01,                 //     Report Size (1)
    0x05, 0x09,                 //     Usage Page (Buttons)
    0x19, 0x01,                 //     Usage Minimum (1)
    0x29, 0x05,                 //     Usage Maximum (5)
    0x15, 0x00,                 //     Logical Minimum (0)
    0x25, 0x01,                 //     Logical Maximum (1)
    0x81, 0x02,                 //     Input (Data, Variable, Absolute)
    0x95, 0x01,                 //     Report Count (1)
    0x75, 0x03,                 //     Report Size (3)
    0x81, 0x01,                 //     Input (Constant) for padding
    0x75, 0x08,                 //     Report Size (8)
    0x95, 0x01,                 //     Report Count (1)
    0x05, 0x01,                 //     Usage Page (Generic Desktop)
    0x09, 0x38,                 //     Usage (Wheel)
    0x15, 0x81,                 //     Logical Minimum (-127)
    0x25, 0x7F,                 //     Logical Maximum (127)
    0x81, 0x06,                 //     Input (Data, Variable, Relative)
    0x05, 0x0C,                 //     Usage Page (Consumer)
    0x0A, 0x38, 0x02,           //     Usage (AC Pan)
    0x95, 0x01,                 //     Report Count (1)
    0x81, 0x06,                 //     Input (Data,Value,Relative,Bit Field)
    0xC0,                       //   End Collection (Physical)
    0x85, HID_REPORT_ID_MM,     //   Report Id HID_REPORT_ID_MM (Mouse motion)
    0x09, 0x01,                 //   Usage (Pointer)
    0xA1, 0x00,                 //   Collection (Physical)
    0x75, 0x0C,                 //     Report Size (12)
    0x95, 0x02,                 //     Report Count (2)
    0x05, 0x01,                 //     Usage Page (Generic Desktop Controls)
    0x09, 0x30,                 //     Usage (X)
    0x09, 0x31,                 //     Usage (Y)
    0x16, 0x01, 0xF8,           //     Logical maximum (2047)
    0x26, 0xFF, 0x07,           //     Logical minimum (-2047)
    0x81, 0x06,                 //     Input (Data, Variable, Relative)
    0xC0,                       //   End Collection (Physical)
    0xC0,                       // End Collection (Application)
#endif

};

static uint16_t s_hidReportMapValLen = sizeof(s_hidReportMapVal);

/* HID Report Map External Report Reference Descriptor */
static uint8_t s_descExtReportRef[] = {UINT16_TO_BYTES(UUID_BATTERY_LEVEL)};
static const uint16_t s_descExtReportRefLen = sizeof(s_descExtReportRef);

#if defined(HIDS_KEYBOARD_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
/* HID Boot Keyboard Input Report Characteristic */
static const uint8_t s_charHidBootKbInReport[] = {(ATT_PROP_READ|ATT_PROP_WRITE_REQ|ATT_PROP_NOTIFY), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_BOOT_KB_INPUT_REPORT), UINT16_TO_BYTES(UUID_HID_BOOT_KB_INPUT_REPORT)};
static const uint16_t s_charHidBootKbInReportLen = sizeof(s_charHidBootKbInReport);

/* HID Boot Keyboard Input Report Value */
static uint8_t s_hidBootKbInReportVal[HID_REPORT_LENGTH_BOOT_KB_INPUT]={0x00};
static uint16_t s_hidBootKbInReportValLen = sizeof(s_hidBootKbInReportVal);

/* HID Boot Keyboard Input Report Client Characteristic Configuration Descriptor */
static uint8_t s_descCccHidBootKbInReport[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccHidBootKbInReportLen = sizeof(s_descCccHidBootKbInReport);

/* HID Boot Keyboard Output Report Characteristic */
static const uint8_t s_charHidBootKbOutReport[] = {(ATT_PROP_READ|ATT_PROP_WRITE_REQ|ATT_PROP_WRITE_CMD), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_BOOT_KB_OUTPUT_REPORT), UINT16_TO_BYTES(UUID_HID_BOOT_KB_OUTPUT_REPORT)};
static const uint16_t s_charHidBootKbOutReportLen = sizeof(s_charHidBootKbOutReport);

/* HID Boot Keyboard Output Report Value */
static uint8_t s_hidBootKbOutReportVal[HID_REPORT_LENGTH_BOOT_KB_OUTPUT]={0x00};
static uint16_t s_hidBootKbOutReportValLen = sizeof(s_hidBootKbOutReportVal);
#endif

#if defined(HIDS_MOUSE_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
/* HID Boot Mouse Input Report Characteristic */
static const uint8_t s_charHidBootMouseInReport[] = {(ATT_PROP_READ|ATT_PROP_WRITE_REQ|ATT_PROP_NOTIFY), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_BOOT_M_INPUT_REPORT), UINT16_TO_BYTES(UUID_HID_BOOT_MOUSE_INPUT_REPORT)};
static const uint16_t s_charHidBootMouseInReportLen = sizeof(s_charHidBootMouseInReport);

/* HID Boot Mouse Input Report Value */
static uint8_t s_hidBootMouseInReportVal[HID_REPORT_LENGTH_BOOT_M_INPUT]={0x00};
static uint16_t s_hidBootMouseInReportValLen = sizeof(s_hidBootMouseInReportVal);

/* HID Boot Mouse Input Report Client Characteristic Configuration Descriptor */
static uint8_t s_descCccHidBootMouseInReport[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccHidBootMouseInReportLen = sizeof(s_descCccHidBootMouseInReport);
#endif

/* HID Information Characteristic */
static const uint8_t s_charHidInfo[] = {(ATT_PROP_READ), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_HID_INFO), UINT16_TO_BYTES(UUID_HID_INFORMATION)};
static const uint16_t s_charHidInfoLen = sizeof(s_charHidInfo);

/* HID Information Value */
static uint8_t s_hidInfoVal[]={UINT16_TO_BYTES(HID_CD_HID), HID_COUNTRY_CODE_NONE, (HID_FLAG_REMOTE_WAKE|HID_FLAG_NORMALLY_CONNECTABLE)};
static const uint16_t s_hidInfoValLen = sizeof(s_hidInfoVal);

/* HID Control Point Characteristic */
static const uint8_t s_charHidCtrl[] = {(ATT_PROP_WRITE_CMD), UINT16_TO_BYTES(HIDS_HDL_CHARVAL_HID_CTRL), UINT16_TO_BYTES(UUID_HID_CONTROL_POINT)};
static const uint16_t s_charHidCtrlLen = sizeof(s_charHidCtrl);

/* HID Control Point Value */
static uint8_t s_hidCtrlVal[] = {0x00};
static const uint16_t s_hidCtrlValLen = sizeof(s_hidCtrlVal);

/* Attribute list for Human Device Information service */
static GATTS_Attribute_T s_hidsList[HIDS_END_HDL-HIDS_START_HDL+1];

#if (defined(HIDS_MOUSE_SUPPORT)) || (defined(HIDS_KEYBOARD_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT))
static const GATTS_CccdSetting_T s_hidsCccdSetting[] = 
{
#ifdef HIDS_KEYBOARD_SUPPORT
    {HIDS_HDL_CCCD_KB_INPUT_REPORT, (NOTIFICATION)},
#endif
#ifdef HIDS_MOUSE_SUPPORT
    {HIDS_HDL_CCCD_MB_INPUT_REPORT, (NOTIFICATION)},
    {HIDS_HDL_CCCD_MM_INPUT_REPORT, (NOTIFICATION)},
#endif
#if defined(HIDS_KEYBOARD_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
    {HIDS_HDL_CCCD_BOOT_KB_INPUT_REPORT, (NOTIFICATION)},
#endif
#if defined(HIDS_MOUSE_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
    {HIDS_HDL_CCCD_BOOT_M_INPUT_REPORT, (NOTIFICATION)}
#endif
};
#endif

/* HID Service structure */
static GATTS_Service_T s_svcHids;
static uint8_t s_hidsAttrIndex;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************

uint16_t BLE_HIDS_Add(void)
{
    uint16_t result;

    s_hidsAttrIndex = 0;

    /* Initialize attribute list */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidPrimSvc;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_svcUuidHids;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_svcUuidHidsLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_svcUuidHids);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    /* Include Battery Service */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidInc;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidsInclude1Val;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidsInclude1ValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidsInclude1Val);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    #ifdef HIDS_BOOT_PROTOCOL_MODE_SUPPORT
    /* HID Protocol Mode */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidProtocolMode;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidProtocolModeLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidProtocolMode);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidProtocolMode;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidProtocolModeVal;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidProtocolModeValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidProtocolModeVal);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_MANUAL_READ_RSP;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;
    #endif

    #ifdef HIDS_KEYBOARD_SUPPORT
    /* HID Report (Keyboard Input Report) */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidReportInputKB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidReportInputKBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidReportInputKB);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidReport;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidReportInputValKB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidReportInputValKBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidReportInputValKB);
    s_hidsList[s_hidsAttrIndex].settings = (SETTING_MANUAL_READ_RSP|SETTING_VARIABLE_LEN);
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidCcc;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descCccReportInputKB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descCccReportInputKBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descCccReportInputKB);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_CCCD;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidReportRef;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descReportRefInputKB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descReportRefInputKBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descReportRefInputKB);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;
    #endif

    #ifdef HIDS_MOUSE_SUPPORT
    /* HID Report (Mouse Button Input Report) */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidReportInputMB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidReportInputMBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidReportInputMB);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidReport;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidReportInputValMB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidReportInputValMBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidReportInputValMB);
    s_hidsList[s_hidsAttrIndex].settings = (SETTING_MANUAL_READ_RSP|SETTING_VARIABLE_LEN);
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidCcc;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descCccReportInputMB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descCccReportInputMBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descCccReportInputMB);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_CCCD;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidReportRef;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descReportRefInputMB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descReportRefInputMBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descReportRefInputMB);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    /* HID Report (Mouse Motion Input Report) */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidReportInputMM;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidReportInputMMLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidReportInputMM);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidReport;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidReportInputValMM;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidReportInputValMMLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidReportInputValMM);
    s_hidsList[s_hidsAttrIndex].settings = (SETTING_MANUAL_READ_RSP|SETTING_VARIABLE_LEN);
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidCcc;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descCccReportInputMM;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descCccReportInputMMLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descCccReportInputMB);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_CCCD;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidReportRef;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descReportRefInputMM;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descReportRefInputMMLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descReportRefInputMM);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;
    #endif

    #ifdef HIDS_KEYBOARD_SUPPORT
    /* HID Report (Output) */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidReportOutputKB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidReportOutputKBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidReportOutputKB);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidReport;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidReportOutputValKB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidReportOutputValKBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidReportOutputValKB);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_MANUAL_READ_RSP;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidReportRef;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descReportRefOutputKB;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descReportRefOutputKBLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descReportRefOutputKB);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;
    #endif

    /* HID Report Map */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidReportMap;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidReportMapLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidReportMap);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidReportMap;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidReportMapVal;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidReportMapValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidReportMapVal);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidExtReportRef;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descExtReportRef;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descExtReportRefLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descExtReportRef);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    #if defined(HIDS_KEYBOARD_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
    /* HID Boot Keyboard Input Report */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidBootKbInReport;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidBootKbInReportLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidBootKbInReport);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidBootKbInReport;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidBootKbInReportVal;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidBootKbInReportValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidBootKbInReportVal);
    s_hidsList[s_hidsAttrIndex].settings = (SETTING_MANUAL_READ_RSP|SETTING_VARIABLE_LEN);
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidCcc;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descCccHidBootKbInReport;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descCccHidBootKbInReportLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descCccHidBootKbInReport);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_CCCD;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    /* HID Boot Keyboard Output Report */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidBootKbOutReport;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidBootKbOutReportLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidBootKbOutReport);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidBootKbOutReport;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidBootKbOutReportVal;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidBootKbOutReportValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidBootKbOutReportVal);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_MANUAL_READ_RSP;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;
    #endif

    #if defined(HIDS_MOUSE_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
    /* HID Boot Mouse Input Report */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidBootMouseInReport;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidBootMouseInReportLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidBootMouseInReport);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidBootMouseInReport;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidBootMouseInReportVal;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidBootMouseInReportValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidBootMouseInReportVal);
    s_hidsList[s_hidsAttrIndex].settings = (SETTING_MANUAL_READ_RSP|SETTING_VARIABLE_LEN);
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_descUuidCcc;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_descCccHidBootMouseInReport;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_descCccHidBootMouseInReportLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_descCccHidBootMouseInReport);
    s_hidsList[s_hidsAttrIndex].settings = SETTING_CCCD;
    s_hidsList[s_hidsAttrIndex].permissions = (PERMISSION_READ|PERMISSION_WRITE);
    s_hidsAttrIndex++;
    #endif

    /* HID Information */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidInfo;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidInfoLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidInfo);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidInfo;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidInfoVal;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidInfoValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidInfoVal);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ | PERMISSION_READ_ENC;
    s_hidsAttrIndex++;

    /* HID Control Point */
    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) g_gattUuidChar;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_charHidCtrl;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_charHidCtrlLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_charHidCtrl);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_READ;
    s_hidsAttrIndex++;

    s_hidsList[s_hidsAttrIndex].p_uuid = (uint8_t *) s_chUuidHidCtrl;
    s_hidsList[s_hidsAttrIndex].p_value = (uint8_t *) s_hidCtrlVal;
    s_hidsList[s_hidsAttrIndex].p_len = (uint16_t *) &s_hidCtrlValLen;
    s_hidsList[s_hidsAttrIndex].maxLen = sizeof(s_hidCtrlVal);
    s_hidsList[s_hidsAttrIndex].settings = 0;
    s_hidsList[s_hidsAttrIndex].permissions = PERMISSION_WRITE;
    s_hidsAttrIndex++;

    /* Initialize Service */
    s_svcHids.p_next = NULL;
    s_svcHids.p_attrList = (GATTS_Attribute_T *) s_hidsList;
    s_svcHids.p_cccdSetting = s_hidsCccdSetting;
    s_svcHids.startHandle = HIDS_START_HDL;
    s_svcHids.endHandle = HIDS_START_HDL+s_hidsAttrIndex-1;
    s_svcHids.cccdNumber = (sizeof(s_hidsCccdSetting)/sizeof(GATTS_CccdSetting_T));
    result = GATTS_AddService(&s_svcHids, s_hidsAttrIndex);
    return result;
}


