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
  BLE Link Loss Service Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_lls.h

  Summary:
    This file contains the BLE Link Loss Service functions for application user.

  Description:
    This file contains the BLE Link Loss Service functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_LLS BLE LLS
 * @{
 * @brief Header file for the BLE Link Loss Service.
 * @note Definitions and prototypes for the BLE Link Loss Service stack layer application programming interface.
 */
#ifndef BLE_LLS_H
#define BLE_LLS_H

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

/**@defgroup BLE_LLS_UUID_DEF BLE_LLS_UUID_DEF
 * @brief The definition of Link Loss Service UUID.
 * @{ */
#define UUID_LINKLOSS_SERVICE                           (0x1803U)  /**< Link Loss Service UUID. */
/** @} */

/**@defgroup BLE_ALERT_LEVEL_UUID_DEF BLE_ALERT_LEVEL_UUID_DEF
 * @brief The definition of Alert Level UUID.
 * @{ */
#define UUID_ALERT_LEVEL                                (0x2A06U)  /**< Alert Level UUID. */
/** @} */


/**@defgroup BLE_LLS_ASSIGN_HANDLE BLE_LLS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Link Loss Service.
 * @{ */
#define LLS_START_HDL                                   (0x00F0U)  /**< The start attribute handle of transparent service. */
/** @} */

/**@brief Definition of BLE Link Loss Service attribute handle */
typedef enum BLE_LLS_AttributeHandle_T
{
    LLS_HDL_SVC = LLS_START_HDL,            /**< Handle of Primary Service of BLE Link Loss Service. */
    LLS_HDL_CHAR_ALERT_LEVEL,               /**< Handle of Link Loss Level characteristic. */
    LLS_HDL_CHARVAL_ALERT_LEVEL,            /**< Handle of Link Loss Level characteristic value. */
}BLE_LLS_AttributeHandle_T;

/**@defgroup BLE_LLS_ASSIGN_HANDLE BLE_LLS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Link Loss Service.
 * @{ */
#define LLS_END_HDL                                 (LLS_HDL_CHARVAL_ALERT_LEVEL)         /**< The end attribute handle of Link Loss service. */
/** @} */


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**
 *@brief This API is used to register the BLE LLS Service to GATT server.
 *
 *
 *@retval MBA_RES_SUCCESS       Successfully registering the service
 *@retval MBA_RES_FAIL          Fail to register the service. The assigned attribute handles in the service conflict or \n
 *                              the start handle of the service is smaller than @ref GATTS_APP_SVC_START_HDL.
 */
uint16_t BLE_LLS_Add(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
 */
