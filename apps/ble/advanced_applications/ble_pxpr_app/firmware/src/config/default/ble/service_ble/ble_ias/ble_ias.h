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
  BLE Immediate Alert Service Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_ias.h

  Summary:
    This file contains the BLE Immediate Alert Service functions for application user.

  Description:
    This file contains the BLE Immediate Alert Service functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_IAS BLE IAS
 * @{
 * @brief Header file for the BLE Immediate Alert Service.
 * @note Definitions and prototypes for the BLE Immediate Alert Service stack layer application programming interface.
 */
#ifndef BLE_IAS_H
#define BLE_IAS_H

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

/**@defgroup BLE_IAS_UUID_DEF BLE_IAS_UUID_DEF
 * @brief The definition of Immediate Alert Service UUID.
 * @{ */
#define UUID_IMMEDIATE_ALERT_SERVICE                    (0x1802U)  /**< Immediate Alert Service UUID. */
/** @} */

/**@defgroup BLE_ALERT_LEVEL_UUID_DEF BLE_ALERT_LEVEL_UUID_DEF
 * @brief The definition of Alert Level UUID.
 * @{ */
#define UUID_ALERT_LEVEL                                (0x2A06U)  /**< Alert Level UUID. */
/** @} */

/**@defgroup BLE_IAS_ASSIGN_HANDLE BLE_IAS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Immediate Alert Service.
 * @{ */
#define IAS_START_HDL                                   (0x0FAU)  /**< The start attribute handle of transparent service. */
/** @} */

/**@brief Definition of BLE Immediate Alert Service attribute handle */
typedef enum BLE_IAS_AttributeHandle_T
{
    IAS_HDL_SVC = IAS_START_HDL,           /**< Handle of Primary Service of BLE Immediate Alert Service. */
    IAS_HDL_CHAR_ALERT_LEVEL,               /**< Handle of Immediate Alert Level characteristic. */
    IAS_HDL_CHARVAL_ALERT_LEVEL,            /**< Handle of Immediate Alert Level characteristic value. */
}BLE_IAS_AttributeHandle_T;

/**@defgroup BLE_IAS_ASSIGN_HANDLE BLE_IAS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Immediate Alert Service.
 * @{ */
#define IAS_END_HDL                                 (IAS_HDL_CHARVAL_ALERT_LEVEL)         /**< The end attribute handle of Immediate Alert service. */
/** @} */


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**
 *@brief This API is used to register the BLE IAS Service to GATT server.
 *
 *
 *@retval MBA_RES_SUCCESS       Successfully registering the service
 *@retval MBA_RES_FAIL          Fail to register the service. The assigned attribute handles in the service conflict or \n
 *                              the start handle of the service is smaller than @ref GATTS_APP_SVC_START_HDL.
 */
uint16_t BLE_IAS_Add(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
 */
