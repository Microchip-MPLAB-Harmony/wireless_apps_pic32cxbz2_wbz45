/*******************************************************************************
  BLE Transparent Service Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_trs.h

  Summary:
    This file contains the BLE Transparent Service functions for application user.

  Description:
    This file contains the BLE Transparent Service functions for application user.
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

/**
 * @addtogroup BLE_TRS BLE TRS
 * @{
 * @brief Header file for the BLE Transparent Service.
 * @note Definitions and prototypes for the BLE Transparent Service stack layer application programming interface.
 */
#ifndef BLE_TRS_H
#define BLE_TRS_H

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

/**@defgroup BLE_TRS_ASSIGN_HANDLE BLE_TRS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Transparent Service.
 * @{ */
#define TRS_START_HDL                               0x00A0                                      /**< The start attribute handle of transparent service. */
/** @} */

/**@brief Definition of BLE transparent attribute handle */
enum BLE_TRS_AttributeHandle_T
{
    TRS_HDL_SVC = TRS_START_HDL,           /**< Handle of Primary Service of BLE Transparent Service. */
    TRS_HDL_CHAR_TX,                       /**< Handle of Transparent TX characteristic. */
    TRS_HDL_CHARVAL_TX,                    /**< Handle of Transparent TX characteristic value. */
    TRS_HDL_CCCD_TX,                       /**< Handle of Transparent TX characteristic CCCD */
    TRS_HDL_CHAR_RX,                       /**< Handle of Transparent RX characteristic. */
    TRS_HDL_CHARVAL_RX,                    /**< Handle of Transparent RX characteristic value. */
    TRS_HDL_CHAR_CTRL,                     /**< Handle of Transparent Control characteristic. */
    TRS_HDL_CHARVAL_CTRL,                  /**< Handle of Transparent Control characteristic value. */
    TRS_HDL_CCCD_CTRL                      /**< Handle of Transparent Control characteristic CCCD. */
};

/**@defgroup BLE_TRS_ASSIGN_HANDLE BLE_TRS_ASSIGN_HANDLE
 * @brief Assigned attribute handles of BLE Transparent Service.
 * @{ */

#define TRS_END_HDL                                 TRS_HDL_CCCD_CTRL         /**< The end attribute handle of transparent service. */
/** @} */

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/**@brief The structure contains information about change UUID function parameters. */
typedef struct  BLE_TRS_Uuids_T
{
    uint8_t *p_primaryService;                                    /**< The 128-bit primary service UUID.*/
    uint8_t *p_transTx;                                           /**< The 128-bit trans tx characteristic UUID. */
    uint8_t *p_transRx;                                           /**< The 128-bit trans rx characteristic UUID. */
    uint8_t *p_transCtrl;                                         /**< The 128-bit trans ctrl characteristic UUID. */
} BLE_TRS_Uuids_T;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**
 *@brief Initialize BLE Transparent Service callback function.
 *
 *
 *@return MBA_RES_SUCCESS                    Successfully register BLE transparent service.
 *@return MBA_RES_NO_RESOURCE                Fail to register service.
 *
 */
uint16_t BLE_TRS_Add();


/**@brief Add the permission of the characteristic in transparent service.\n
          Must be called after BLE_TRS_Add().
 *
 *@param[in] attrHdl                         Transparent service attribute handle. See @ref BLE_TRS_AttributeHandle_T.
 *@param[in] permissions                     Attribute permissions. See @ref GATT_ATTRIBUTE_PERMISSIONS.
 *
 *
 *@return MBA_RES_SUCCESS                    Successfully configurate the permission.
 *@return MBA_RES_INVALID_PARA               The assigned attribute handle is not included in transparent service.
 *
 */
uint16_t BLE_TRS_PermissionConfig(uint16_t attrHdl, uint8_t permissions);


#endif

/**
  @}
 */
