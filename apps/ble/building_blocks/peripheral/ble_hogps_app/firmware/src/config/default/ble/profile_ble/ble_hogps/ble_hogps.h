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
  BLE HID over GATT profile server role Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_hogps.h

  Summary:
    This file contains the BLE HID over GATT profile server role functions for application user.

  Description:
    This file contains the BLE HID over GATT profile server role functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_HOGPS
 * @{
 * @brief Header file for the BLE HID over GATT profile server role library.
 * @note Definitions and prototypes for the BLE HID over GATT profile server role stack layer application programming interface.
 */
#ifndef BLE_HOGPS_H
#define BLE_HOGPS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "stack_mgr.h"
#include "ble_hids/ble_hids.h"

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
/**@addtogroup BLE_HOGPS_DEFINES Defines
 * @{ */

/**@} */ //BLE_HOGPS_DEFINES

/**@addtogroup BLE_HOGPS_ENUMS Enumerations
 * @{ */

/**@brief Enumeration type of BLE HOGPS callback events. */
typedef enum BLE_HOGPS_EventId_T
{
    BLE_HOGPS_EVT_BOOT_MODE_ENTER_IND,                          /**< Indicate HID Boot Mode entered. See @ref BLE_HOGPS_EvtBootModeEnter_T for event details.*/
    BLE_HOGPS_EVT_REPORT_MODE_ENTER_IND,                        /**< Indicate HID Report Mode entered. See @ref BLE_HOGPS_EvtReportModeEnter_T for event details.*/
    BLE_HOGPS_EVT_HOST_SUSPEND_ENTER_IND,                       /**< Indicate HID Host entering suspend. See @ref BLE_HOGPS_EvtHostSuspendEnter_T for event details.*/
    BLE_HOGPS_EVT_HOST_SUSPEND_EXIT_IND,                        /**< Indicate HID Host exiting suspend. See @ref BLE_HOGPS_EvtHostSuspendExit_T for event details.*/
    BLE_HOGPS_EVT_INPUT_REPORT_WRITE_IND,                       /**< Indicate HID Input report written. See @ref BLE_HOGPS_EvtReportWrite_T for event details.*/
    BLE_HOGPS_EVT_OUTPUT_REPORT_WRITE_IND,                      /**< Indicate HID Output report written. See @ref BLE_HOGPS_EvtReportWrite_T for event details.*/
    BLE_HOGPS_EVT_FEATURE_REPORT_WRITE_IND,                     /**< Indicate HID Feature report written. See @ref BLE_HOGPS_EvtReportWrite_T for event details.*/
    BLE_HOGPS_EVT_BOOT_KB_INPUT_REPORT_WRITE_IND,               /**< Indicate HID Boot Keyboard Input report written. See @ref BLE_HOGPS_EvtBootReportWrite_T for event details.*/
    BLE_HOGPS_EVT_BOOT_KB_OUTPUT_REPORT_WRITE_IND,              /**< Indicate HID Boot Keyboard Output report written. See @ref BLE_HOGPS_EvtBootReportWrite_T for event details.*/
    BLE_HOGPS_EVT_BOOT_M_INPUT_REPORT_WRITE_IND,                /**< Indicate HID Boot Mouse Input report written. See @ref BLE_HOGPS_EvtBootReportWrite_T for event details.*/
    BLE_HOGPS_EVT_END
} BLE_HOGPS_EventId_T;


/**@} */ //BLE_HOGPS_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_HOGPS_STRUCTS Structures
 * @{ */

/**@brief Data structure for @ref BLE_HOGPS_EVT_BOOT_MODE_ENTER_IND event. */
typedef struct BLE_HOGPS_EvtBootModeEnter_T
{
    uint16_t                            connHandle;             /**< The connection handle associated with the connection. */
} BLE_HOGPS_EvtBootModeEnter_T;

/**@brief Data structure for @ref BLE_HOGPS_EVT_REPORT_MODE_ENTER_IND event. */
typedef struct BLE_HOGPS_EvtReportModeEnter_T
{
    uint16_t                            connHandle;             /**< The connection handle associated with the connection. */
} BLE_HOGPS_EvtReportModeEnter_T;

/**@brief Data structure for @ref BLE_HOGPS_EVT_HOST_SUSPEND_ENTER_IND event. */
typedef struct BLE_HOGPS_EvtHostSuspendEnter_T
{
    uint16_t                            connHandle;             /**< The connection handle associated with the connection. */
} BLE_HOGPS_EvtHostSuspendEnter_T;

/**@brief Data structure for @ref BLE_HOGPS_EVT_HOST_SUSPEND_EXIT_IND event. */
typedef struct BLE_HOGPS_EvtHostSuspendExit_T
{
    uint16_t                            connHandle;             /**< The connection handle associated with the connection. */
} BLE_HOGPS_EvtHostSuspendExit_T;

/**@brief Data structure for @ref BLE_HOGPS_EVT_INPUT_REPORT_WRITE_IND, @ref BLE_HOGPS_EVT_OUTPUT_REPORT_WRITE_IND, @ref BLE_HOGPS_EVT_FEATURE_REPORT_WRITE_IND event. */
typedef struct BLE_HOGPS_EvtReportWrite_T
{
    uint16_t                            connHandle;             /**< The connection handle associated with the connection. */
    uint16_t                            length;                 /**< Length of the written value. */
    uint16_t                            offset;                 /**< Offset of the written value. */
    uint8_t                             reportId;               /**< Report id of the written report. */
    uint8_t                             *p_data;                /**< Data of the written value. */
} BLE_HOGPS_EvtReportWrite_T;

/**@brief Data structure for @ref BLE_HOGPS_EVT_BOOT_KB_INPUT_REPORT_WRITE_IND, @ref BLE_HOGPS_EVT_BOOT_KB_OUTPUT_REPORT_WRITE_IND, @ref BLE_HOGPS_EVT_BOOT_M_INPUT_REPORT_WRITE_IND event. */
typedef struct BLE_HOGPS_EvtBootReportWrite_T
{
    uint16_t                            connHandle;             /**< The connection handle associated with the connection. */
    uint16_t                            length;                 /**< Length of the written value. */
    uint16_t                            offset;                 /**< Offset of the written value. */
    uint8_t                             *p_data;                /**< Data of the written value. */
} BLE_HOGPS_EvtBootReportWrite_T;

/**@brief Union of BLE HID over GATT profile server role callback event data types. */
typedef union
{
    BLE_HOGPS_EvtBootModeEnter_T        evtBootModeEnter;               /**< Handle @ref BLE_HOGPS_EVT_BOOT_MODE_ENTER_IND. */
    BLE_HOGPS_EvtReportModeEnter_T      evtReportModeEnter;             /**< Handle @ref BLE_HOGPS_EVT_REPORT_MODE_ENTER_IND. */
    BLE_HOGPS_EvtHostSuspendEnter_T     evtHostSuspendEnter;            /**< Handle @ref BLE_HOGPS_EVT_HOST_SUSPEND_ENTER_IND. */
    BLE_HOGPS_EvtHostSuspendExit_T      evtHostSuspendExit;             /**< Handle @ref BLE_HOGPS_EVT_HOST_SUSPEND_EXIT_IND. */
    BLE_HOGPS_EvtReportWrite_T          evtInputReportWrite;            /**< Handle @ref BLE_HOGPS_EVT_INPUT_REPORT_WRITE_IND.*/
    BLE_HOGPS_EvtReportWrite_T          evtOutputReportWrite;           /**< Handle @ref BLE_HOGPS_EVT_OUTPUT_REPORT_WRITE_IND. */
    BLE_HOGPS_EvtReportWrite_T          evtFeatureReportWrite;          /**< Handle @ref BLE_HOGPS_EVT_FEATURE_REPORT_WRITE_IND. */
    BLE_HOGPS_EvtBootReportWrite_T      evtBootKbOutputReportWrite;     /**< Handle @ref BLE_HOGPS_EVT_BOOT_KB_INPUT_REPORT_WRITE_IND. */
    BLE_HOGPS_EvtBootReportWrite_T      evtBootKbInputReportWrite;      /**< Handle @ref BLE_HOGPS_EVT_BOOT_KB_OUTPUT_REPORT_WRITE_IND. */
    BLE_HOGPS_EvtBootReportWrite_T      evtBootMouseInputReportWrite;   /**< Handle @ref BLE_HOGPS_EVT_BOOT_M_INPUT_REPORT_WRITE_IND. */
} BLE_HOGPS_EventField_T;

/**@brief BLE HID over GATT profile server role callback event.*/
typedef struct  BLE_HOGPS_Event_T
{
    BLE_HOGPS_EventId_T                 eventId;            /**< Event ID. See @ref BLE_HOGPS_EventId_T.  */
    BLE_HOGPS_EventField_T              eventField;         /**< Event field. */
} BLE_HOGPS_Event_T;



/**@brief BLE HID over GATT profile server role callback type. This callback function sends BLE HID over GATT profile server role events to the application.*/
typedef void(*BLE_HOGPS_EventCb_T)(BLE_HOGPS_Event_T *p_event);

/**@} */ //BLE_HOGPS_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_HOGPS_FUNS Functions
 * @{ */


/**@brief Initialize BLE HID over GATT profile server role.
 *
 * @retval MBA_RES_SUCCESS          Success to initialize BLE HID over GATT profile server role.
 * @retval MBA_RES_FAIL             Fail to initialize BLE HID over GATT profile server role.
 *
 */
uint16_t BLE_HOGPS_Init(void);

/**
 * @brief Register BLE HID over GATT profile server role callback.\n
 *        Application must call this API after starting BLE HID over GATT profile server role procedure.
 *
 *
 * @param[in] routine               Client callback function.
 *
 */
void BLE_HOGPS_EventRegister(BLE_HOGPS_EventCb_T routine);

/**@brief Handle BLE_Stack events.
 *        This API should be called in the application while caching BLE_Stack events.
 *
 * @param[in] p_stackEvent          Pointer to BLE_Stack events buffer.
 *
*/
void BLE_HOGPS_BleEventHandler(STACK_Event_T *p_stackEvent);

/**@brief Set HID keyboard input report.
 *
 * @param[in] p_keyCodeArray        8 bytes of key codes array. From byte 0 to byte 7: |Modifier Keys|Reserved|Keycode 1|Keycode 2|Keycode 3|Keycode 4|Keycode 5|Keycode 6|.
 *
 * @retval MBA_RES_SUCCESS          Successfully set the input report.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SetKeyboardInputReport(uint8_t *p_keyCodeArray);

/**@brief Set HID mouse (Mouse Button) input report.
 *
 * @param[in] buttons               Bit fields (bit 0 - bit 4) represent button pressed or not.
 * @param[in] wheel                 Value of mouse wheel. Range from -127 to 127.
 * @param[in] acPan                 Value of mouse AC pan. Range from -127 to 127.
 *
 * @retval MBA_RES_SUCCESS          Successfully set the input report.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SetMouseButtonInputReport(uint8_t buttons, int8_t wheel, int8_t acPan);

/**@brief Set HID mouse (Mouse Motion) input report.
 *
 * @param[in] xAxis                 X-Axis position. Range from -2047 to 2047.
 * @param[in] yAxis                 Y-Axis position. Range from -2047 to 2047.
 *
 * @retval MBA_RES_SUCCESS          Successfully set the input report.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SetMouseMotionInputReport(int16_t xAxis, int16_t yAxis);

/**@brief Set HID keyboard output report.
 *
 * @param[in] ledValue              Bit fields (bit 0 - bit 4) represent LED values.
 *
 * @retval MBA_RES_SUCCESS          Successfully set the output report.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SetKeyboardOutputReport(uint8_t ledValue);

/**@brief Set HID boot keyboard input report.
 *
 * @param[in] p_keyCodeArray        8 bytes of key codes array. From byte 0 to byte 7: |Modifier Keys|Reserved|Keycode 1|Keycode 2|Keycode 3|Keycode 4|Keycode 5|Keycode 6|.
 *
 * @retval MBA_RES_SUCCESS          Successfully set the input report.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SetBootKeyboardInputReport(uint8_t *p_keyCodeArray);

/**@brief Set HID boot keyboard output report.
 *
 * @param[in] ledValue              Bit fields (bit 0 - bit 4) represent LED values.
 *
 * @retval MBA_RES_SUCCESS          Successfully set the output report.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SetBootKeyboardOutputReport(uint8_t ledValue);

/**@brief Set HID boot mouse input report.
 *
 * @param[in] buttons               Bit fields (bit 0 - bit 2) represent left/right/middle button pressed or not.
 * @param[in] xAxis                 X-Axis position.
 * @param[in] yAxis                 Y-Axis position.
 *
 * @retval MBA_RES_SUCCESS          Successfully set the input report.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SetBootMouseInputReport(uint8_t buttons, int8_t xAxis, int8_t yAxis);

/**@brief Send HID keyboard input report.
 *
 * @param[in] connHandle            The connection handle associated with the connection.
 * @param[in] p_keyCodeArray        8 bytes of key codes array. From byte 0 to byte 7: |Modifier Keys|Reserved|Keycode 1|Keycode 2|Keycode 3|Keycode 4|Keycode 5|Keycode 6|.
 *
 * @retval MBA_RES_SUCCESS          Successfully send the input report.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SendKeyboardInputReport(uint16_t connHandle, uint8_t *p_keyCodeArray);

/**@brief Send HID mouse (Mouse Button) input report.
 *
 * @param[in] connHandle            The connection handle associated with the connection.
 * @param[in] buttons               Bit fields (bit 0 - bit 4) represent button pressed or not.
 * @param[in] wheel                 Value of mouse wheel. Range from -127 to 127.
 * @param[in] acPan                 Value of mouse AC pan. Range from -127 to 127.
 *
 * @retval MBA_RES_SUCCESS          Successfully send the input report.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SendMouseButtonInputReport(uint16_t connHandle, uint8_t buttons, int8_t wheel, int8_t acPan);

/**@brief Send HID mouse (Mouse Motion) input report.
 *
 * @param[in] connHandle            The connection handle associated with the connection.
 * @param[in] xAxis                 X-Axis position. Range from -2047 to 2047.
 * @param[in] yAxis                 Y-Axis position. Range from -2047 to 2047.
 *
 * @retval MBA_RES_SUCCESS          Successfully send the input report.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SendMouseMotionInputReport(uint16_t connHandle, int16_t xAxis, int16_t yAxis);

/**@brief Send HID boot keyboard input report.
 *
 * @param[in] connHandle            The connection handle associated with the connection.
 * @param[in] p_keyCodeArray        8 bytes of key codes array. From byte 0 to byte 7: |Modifier Keys|Reserved|Keycode 1|Keycode 2|Keycode 3|Keycode 4|Keycode 5|Keycode 6|.
 *
 * @retval MBA_RES_SUCCESS          Successfully send the input report.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SendBootKeyboardInputReport(uint16_t connHandle, uint8_t *p_keyCodeArray);

/**@brief Send HID boot mouse input report.
 *
 * @param[in] connHandle            The connection handle associated with the connection.
 * @param[in] buttons               Bit fields (bit 0 - bit 2) represent left/right/middle button pressed or not.
 * @param[in] xAxis                 X-Axis position.
 * @param[in] yAxis                 Y-Axis position.
 *
 * @retval MBA_RES_SUCCESS          Successfully send the input report.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SendBootMouseInputReport(uint16_t connHandle, uint8_t buttons, int8_t xAxis, int8_t yAxis);

/**@brief Send battery level of HID device by Battery service.
 *
 * @param[in] connHandle            The connection handle associated with the connection.
 *
 * @retval MBA_RES_SUCCESS          Successfully send the input report.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
*/
uint16_t BLE_HOGPS_SendBatteryLevel(uint16_t connHandle);

/**@} */ //BLE_HOGPS_FUNS


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif

/**
  @}
*/

