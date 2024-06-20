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
  BLE Alert Notification Service Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_ans.h

  Summary:
    This file contains the BLE Device Information Service functions for application user.

  Description:
    This file contains the BLE Device Information Service functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_ANS BLE ANS
 * @{
 * @brief Header file for the BLE Alert Notification Service.
 * @note Definitions and prototypes for the BLE Alert Notification Service stack layer application programming interface.
 */
#ifndef BLE_ANS_H
#define BLE_ANS_H

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

/**@defgroup BLE_ANS_ASSIGN_HANDLE BLE_ANS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Alert Notification Service.
 * @{ */
#define BLE_ANS_START_HDL                  (0x0150U)      /**< The start attribute handle of alert notification service. */
/** @} */

/**@brief Definition of BLE Device Information Service attribute handle */
typedef enum BLE_ANS_AttributeHandle_T
{
    ANS_HDL_SVC = BLE_ANS_START_HDL,        /**< Handle of Primary Service of BLE Device Information Service. */

    ANS_HDL_CHAR_SUPP_NEW_ALERT_CAT,        /**< Handle of Supported New Alert Category. */
    ANS_HDL_CHARVAL_SUPP_NEW_ALERT_CAT,     /**< Handle of Supported New Alert Category value. */

    ANS_HDL_CHAR_NEW_ALERT,                 /**< Handle of New Alert. */
    ANS_HDL_CHARVAL_NEW_ALERT,              /**< Handle of New Alert value. */
    ANS_HDL_CHARVAL_NEW_ALERT_CCC,          /**< Handle of New Alert Client Characteristic Configuration value. */

    ANS_HDL_CHAR_SUPP_UNREAD_ALERT_CAT,     /**< Handle of Supported Unread Alert Category. */
    ANS_HDL_CHARVAL_SUPP_UNREAD_ALERT_CAT,  /**< Handle of Supported Unread Alert Category value. */

    ANS_HDL_CHAR_UNREAD_ALERT_STAT,         /**< Handle of Unread Alert. */
    ANS_HDL_CHARVAL_UNREAD_ALERT_STAT,      /**< Handle of Unread Alert value. */
    ANS_HDL_CHARVAL_UNREAD_ALERT_STAT_CCC,  /**< Handle of Unread Alert Status Client Characteristic Configuration value. */

    ANS_HDL_CHAR_ANCP,     /**< Handle of Alert Notification Control Point. */
    ANS_HDL_CHARVAL_ANCP  /**< Handle of Alert Notification Control Point value. */

}BLE_ANS_AttributeHandle_T;

/**@defgroup BLE_ANS_ASSIGN_HANDLE BLE_ANS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Alert Notification Service.
 * @{ */

#define BLE_ANS_END_HDL   ANS_HDL_CHARVAL_ANCP /**< The end attribute handle of alert notification service. */
/** @} */

/**
 *@brief Initialize BLE Alert Notificaiton Service callback function.
 *
 *
 *@return MBA_RES_SUCCESS                    Successfully register BLE Alert Notification service.
 *@return MBA_RES_NO_RESOURCE                Fail to register service.
 *
 */
uint16_t BLE_ANS_Add(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif
/**
  @}
 */
