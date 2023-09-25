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
  BLE Human Interface Device Service Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_hids.h

  Summary:
    This file contains the BLE Human Interface Device Service functions for application user.

  Description:
    This file contains the BLE Human Interface Device Service functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_HIDS BLE HIDS
 * @{
 * @brief Header file for the BLE Human Interface Device Service.
 * @note Definitions and prototypes for the BLE Human Interface Device Service stack layer application programming interface.
 */
#ifndef BLE_HIDS_H
#define BLE_HIDS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "ble_bas/ble_bas.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

/**@defgroup BLE_HIDS_MACRO BLE_HIDS_MACRO
 * @brief Preprocessing macros defining supported features of BLE Human Interface Device Service.
 * @{ */

#define HIDS_BOOT_PROTOCOL_MODE_SUPPORT     /* Boot Mode Support*/    /**< Define to support Boot Protocol mode. */

#define HIDS_KEYBOARD_SUPPORT               /* Keyboard Support */     /**< Define to support Keyboard. */

//#define HIDS_MOUSE_SUPPORT                  /* Mouse Support */

/** @} */

/**@defgroup BLE_HIDS_UUID_DEF BLE_HIDS_UUID_DEF
 * @brief SIG-defined UUIDs of BLE Human Interface Device Service.
 * @{ */
#define UUID_HUMAN_INTERFACE_DEVICE_SERVICE                 0x1812          /**< Service UUID: Human Interface Device Service. */
#define UUID_HID_BOOT_KB_INPUT_REPORT                       0x2A22          /**< Characteristic UUID: Boot Keyboard Input Report. */
#define UUID_HID_BOOT_KB_OUTPUT_REPORT                      0x2A32          /**< Characteristic UUID: Boot Keyboard Output Report. */
#define UUID_HID_BOOT_MOUSE_INPUT_REPORT                    0x2A33          /**< Characteristic UUID: Boot Mouse Input Report. */
#define UUID_HID_INFORMATION                                0x2A4A          /**< Characteristic UUID: HID Information. */
#define UUID_HID_REPORT_MAP                                 0x2A4B          /**< Characteristic UUID: HID Report Map. */
#define UUID_HID_CONTROL_POINT                              0x2A4C          /**< Characteristic UUID: HID Control Point. */
#define UUID_HID_REPORT                                     0x2A4D          /**< Characteristic UUID: HID Report. */
#define UUID_HID_PROTOCOL_MODE                              0x2A4E          /**< Characteristic UUID: HID Protocol Mode. */
/** @} */

/**@defgroup BLE_HIDS_REP_TYPE BLE_HIDS_REP_TYPE
 * @brief Report type definitions of BLE Human Interface Device Service.
 * @{ */
#define HID_REPORT_TYPE_INPUT                               0x01            /**< Input report type. */
#define HID_REPORT_TYPE_OUTPUT                              0x02            /**< Output report type. */
#define HID_REPORT_TYPE_FEATURE                             0x03            /**< Feature report type. */
/** @} */

/**@defgroup BLE_HIDS_REP_ID BLE_HIDS_REP_ID
 * @brief Report ID definitions of BLE Human Interface Device Service.
 * @{ */
#define HID_REPORT_ID_KB                                    0x00            /**< HID Report ID: Keyboard. */
#define HID_REPORT_ID_MB                                    0x02            /**< HID Report ID: Mouse buttons. */
#define HID_REPORT_ID_MM                                    0x03            /**< HID Report ID: Mouse motion. */
/** @} */

/**@defgroup BLE_HIDS_REP_LEN BLE_HIDS_REP_LEN
 * @brief Report length definitions of BLE Human Interface Device Service.
 * @{ */
#define HID_REPORT_LENGTH_KB_INPUT                          0x08            /**< Report length of Keyboard Input report. */
#define HID_REPORT_LENGTH_KB_OUTPUT                         0x01            /**< Report length of Keyboard Output report (LED). */
#define HID_REPORT_LENGTH_MB_INPUT                          0x03            /**< Report length of Mouse Button Input report. */
#define HID_REPORT_LENGTH_MM_INPUT                          0x03            /**< Report length of Mouse Motion Input report. */
#define HID_REPORT_LENGTH_BOOT_KB_INPUT                     0x08            /**< Report length of Boot Keyboard Input report. */
#define HID_REPORT_LENGTH_BOOT_KB_OUTPUT                    0x01            /**< Report length of Boot Keyboard Output report. */
#define HID_REPORT_LENGTH_BOOT_M_INPUT                      0x03            /**< Report length of Boot Mouse Input report. */
/** @} */

/**@defgroup BLE_HIDS_MODE BLE_HIDS_MODE
 * @brief HID mode definitions of BLE Human Interface Device Service.
 * @{ */
#define HID_MODE_BOOT_PROTOCOL                              0x00            /**< A HID Service shall only enter Boot Protocol Mode after this value has been written. */
#define HID_MODE_REPORT_PROTOCOL                            0x01            /**< Default Protocol Mode of all HID Devices. */
/** @} */

/**@defgroup BLE_HIDS_SUP_STATUS BLE_HIDS_SUP_STATUS
 * @brief HID host suspend status definitions of BLE Human Interface Device Service.
 * @{ */
#define HID_HOST_SUSPEND_ENTER                              0x00            /**< HID host is entering the suspend state. */
#define HID_HOST_SUSPEND_EXIT                               0x01            /**< HID host is exiting the suspend state. */
/** @} */

/**@defgroup BLE_HIDS_ASSIGN_HANDLE BLE_HIDS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Human Interface Device Service.
 * @{ */
#define HIDS_START_HDL                                      0x0050          /**< The start attribute handle of Human Interface Device service. */
/** @} */

/**@brief Definition of BLE Human Interface Device Service attribute handle */
typedef enum BLE_HIDS_AttributeHandle_T
{
    HIDS_HDL_SVC = HIDS_START_HDL,                                          /**< Handle of Primary Service of BLE Human Interface Device Service. */
    HIDS_HDL_INCLUDE,                                                       /**< Handle of Included Service of BLE Human Interface Device Service. */

#ifdef HIDS_BOOT_PROTOCOL_MODE_SUPPORT
    HIDS_HDL_CHAR_HID_PROTOCOL_MODE,                                        /**< Handle of Protocol Mode Characteristic of BLE Human Interface Device Service. */
    HIDS_HDL_CHARVAL_HID_PROTOCOL_MODE,                                     /**< Handle of Protocol Mode Characteristic Value of BLE Human Interface Device Service. */
#endif

#ifdef HIDS_KEYBOARD_SUPPORT
    HIDS_HDL_CHAR_KB_INPUT_REPORT,                                          /**< Handle of Report Characteristic (Keyboard Input Report). */
    HIDS_HDL_CHARVAL_KB_INPUT_REPORT,                                       /**< Handle of Report Characteristic Value (Keyboard Input Report). */
    HIDS_HDL_CCCD_KB_INPUT_REPORT,                                          /**< Handle of Client Characteristic Configuration Descriptor (Keyboard Input Report). */
    HIDS_HDL_DESC_REPORT_REF_KB_INPUT_REPORT,                               /**< Handle of Report Reference Descriptor (Keyboard Input Report). */
#endif

#ifdef HIDS_MOUSE_SUPPORT
    HIDS_HDL_CHAR_MB_INPUT_REPORT,                                          /**< Handle of Report Characteristic (Mouse Button Input Report). */
    HIDS_HDL_CHARVAL_MB_INPUT_REPORT,                                       /**< Handle of Report Characteristic Value (Mouse Button Input Report). */
    HIDS_HDL_CCCD_MB_INPUT_REPORT,                                          /**< Handle of Client Characteristic Configuration Descriptor (Mouse Button Input Report). */
    HIDS_HDL_DESC_REPORT_REF_MB_INPUT_REPORT,                               /**< Handle of Report Reference Descriptor (Mouse Button Input Report). */
    HIDS_HDL_CHAR_MM_INPUT_REPORT,                                          /**< Handle of Report Characteristic (Mouse Motion Input Report). */
    HIDS_HDL_CHARVAL_MM_INPUT_REPORT,                                       /**< Handle of Report Characteristic Value (Mouse Motion Input Report). */
    HIDS_HDL_CCCD_MM_INPUT_REPORT,                                          /**< Handle of Client Characteristic Configuration Descriptor (Mouse Motion Input Report). */
    HIDS_HDL_DESC_REPORT_REF_MM_INPUT_REPORT,                               /**< Handle of Report Reference Descriptor (Mouse Motion Input Report). */
#endif

#ifdef HIDS_KEYBOARD_SUPPORT
    HIDS_HDL_CHAR_KB_OUTPUT_REPORT,                                         /**< Handle of Report Characteristic (Keyboard Output Report). */
    HIDS_HDL_CHARVAL_KB_OUTPUT_REPORT,                                      /**< Handle of Report Characteristic Value (Keyboard Output Report). */
    HIDS_HDL_DESC_REPORT_REF_KB_OUTPUT_REPORT,                              /**< Handle of Report Reference Descriptor (Keyboard Output Report). */
#endif

    HIDS_HDL_CHAR_REPORT_MAP,                                               /**< Handle of Report Map Characteristic. */
    HIDS_HDL_CHARVAL_REPORT_MAP,                                            /**< Handle of Report Map Characteristic Value. */
    HIDS_HDL_DESC_EXT_REPORT_REF,                                           /**< Handle of External Report Reference Descriptor. */

#if defined(HIDS_KEYBOARD_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
    HIDS_HDL_CHAR_BOOT_KB_INPUT_REPORT,                                     /**< Handle of Boot Keyboard Input Report Characteristic. */
    HIDS_HDL_CHARVAL_BOOT_KB_INPUT_REPORT,                                  /**< Handle of Boot Keyboard Input Report Characteristic Value. */
    HIDS_HDL_CCCD_BOOT_KB_INPUT_REPORT,                                     /**< Handle of Client Characteristic Configuration Descriptor (Boot Keyboard Input Report Characteristic). */
    HIDS_HDL_CHAR_BOOT_KB_OUTPUT_REPORT,                                    /**< Handle of Boot Keyboard Output Report Characteristic. */
    HIDS_HDL_CHARVAL_BOOT_KB_OUTPUT_REPORT,                                 /**< Handle of Boot Keyboard Output Report Characteristic Value. */
#endif

#if defined(HIDS_MOUSE_SUPPORT) && defined (HIDS_BOOT_PROTOCOL_MODE_SUPPORT)
    HIDS_HDL_CHAR_BOOT_M_INPUT_REPORT,                                      /**< Handle of Boot Mouse Input Report Characteristic. */
    HIDS_HDL_CHARVAL_BOOT_M_INPUT_REPORT,                                   /**< Handle of Boot Mouse Input Report Characteristic Value. */
    HIDS_HDL_CCCD_BOOT_M_INPUT_REPORT,                                      /**< Handle of Client Characteristic Configuration Descriptor (Boot Mouse Input Report Characteristic). */
#endif

    HIDS_HDL_CHAR_HID_INFO,                                                 /**< Handle of HID Information Characteristic. */
    HIDS_HDL_CHARVAL_HID_INFO,                                              /**< Handle of HID Information Characteristic Value. */
    HIDS_HDL_CHAR_HID_CTRL,                                                 /**< Handle of HID Control Point Characteristic. */
    HIDS_HDL_CHARVAL_HID_CTRL,                                              /**< Handle of HID Control Point Characteristic Value. */

    HIDS_HDL_END
}BLE_HIDS_AttributeHandle_T;

/**@defgroup BLE_HIDS_ASSIGN_HANDLE BLE_HIDS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Human Interface Device Service.
 * @{ */
#define HIDS_END_HDL                                (HIDS_HDL_END-1)        /**< The end attribute handle of HID service. */
/** @} */

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**
 *@brief Initialize BLE Human Interface Device Service callback function.
 *
 *
 *@return MBA_RES_SUCCESS                    Successfully register BLE Human Interface Device service.
 *@return MBA_RES_NO_RESOURCE                Fail to register service.
 *
 */
uint16_t BLE_HIDS_Add(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
 */

