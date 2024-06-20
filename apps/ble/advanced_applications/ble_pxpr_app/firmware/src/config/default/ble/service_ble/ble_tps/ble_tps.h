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
  BLE Tx Power Service Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_tps.h

  Summary:
    This file contains the BLE Tx Power Service functions for application user.

  Description:
    This file contains the BLE Tx Power Service functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_TPS BLE TPS
 * @{
 * @brief Header file for the BLE Tx Power Service.
 * @note Definitions and prototypes for the BLE Tx Power Service stack layer application programming interface.
 */
#ifndef BLE_TPS_H
#define BLE_TPS_H

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

/**@defgroup BLE_TPS_UUID_DEF BLE_TPS_UUID_DEF
 * @brief The definition of Tx Power Service UUID.
 * @{ */
#define UUID_TXPOWER_SERVICE                              (0x1804U)  /**< Tx Power Service UUID. */
/** @} */

/**@defgroup BLE_TX_POWER_LEVEL_UUID_DEF BLE_TX_POWER_LEVEL_UUID_DEF
 * @brief The definition of Tx Power Level UUID.
 * @{ */
#define UUID_TXPOWER_LEVEL                                (0x2A07U)  /**< Tx Power Level UUID. */
/** @} */

/**@defgroup BLE_TPS_ASSIGN_HANDLE BLE_TPS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Tx Power Service.
 * @{ */
#define TPS_START_HDL                                     (0x139U)    /**< The start attribute handle of transparent service. */
/** @} */

/**@brief Definition of BLE Tx Power Service attribute handle */
typedef enum BLE_TPS_AttributeHandle_T
{
    TPS_HDL_SVC = TPS_START_HDL,           /**< Handle of Primary Service of BLE Tx Power Service. */
    TPS_HDL_CHAR_TXPOWER_LEVEL,               /**< Handle of Tx Power Level characteristic. */
    TPS_HDL_CHARVAL_TXPOWER_LEVEL,            /**< Handle of Tx Power Level characteristic value. */
}BLE_TPS_AttributeHandle_T;

/**@defgroup BLE_TPS_ASSIGN_HANDLE BLE_TPS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Tx Power Service.
 * @{ */
#define TPS_END_HDL                                 (TPS_HDL_CHARVAL_TXPOWER_LEVEL)         /**< The end attribute handle of Tx Power service. */
/** @} */


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**
 *@brief This API is used to register the BLE TPS Service to GATT server.
 *
 *
 *@retval MBA_RES_SUCCESS       Successfully registering the service
 *@retval MBA_RES_FAIL          Fail to register the service. The assigned attribute handles in the service conflict or \n
 *                              the start handle of the service is smaller than @ref GATTS_APP_SVC_START_HDL.
 */
uint16_t BLE_TPS_Add(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
 */
