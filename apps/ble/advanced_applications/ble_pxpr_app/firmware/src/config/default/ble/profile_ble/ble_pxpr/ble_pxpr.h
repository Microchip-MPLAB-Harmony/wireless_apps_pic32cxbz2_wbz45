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
  BLE Proximity Profile Reporter Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_pxpr.h

  Summary:
    This file contains the BLE Proximity Profile Reporter functions for application user.

  Description:
    This file contains the BLE Proximity Profile Reporter functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_PXPR
 * @{
 * @brief Header file for the BLE Proximity profile reporter library.
 * @note Definitions and prototypes for the BLE Proximity profile reporter stack layer application programming interface.
 */
#ifndef BLE_PXPR_H
#define BLE_PXPR_H

// *****************************************************************************
// *****************************************************************************
// Section: Macros for support feature
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_PXPR_SUPP_SVC_DEFINES Defines
 * @{ */

/**@defgroup BLE_PXPR_SUPP_SVC Supported service
 * @brief The definition of supported service.
 * @{ */
#define BLE_PXPR_IAS_ENABLE        /* Enable Immediate Alert Service */    /**< Feature of PXP Reporter supporting optional Immediate Alert Service. */
#define BLE_PXPR_TPS_ENABLE        /* Enable TX Power Service */           /**< Feature of PXP Reporter supporting optional Tx Power Service. */

/**@} */ //BLE_PXPR_SUPP_SVC_DEFINES



// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "stack_mgr.h"
#include "ble_lls/ble_lls.h"
#ifdef BLE_PXPR_IAS_ENABLE
#include "ble_ias/ble_ias.h"
#endif
#ifdef BLE_PXPR_TPS_ENABLE
#include "ble_tps/ble_tps.h"
#endif

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
/**@addtogroup BLE_PXPR_DEFINES Defines
 * @{ */

/**@} */ //BLE_PXPR_DEFINES

/**@addtogroup BLE_PXPR_ENUMS Enumerations
 * @{ */

/**@brief Enumeration type of BLE PXP Reporter alert levels. */

typedef enum BLE_PXPR_AlertLevel_T
{
    BLE_PXPR_ALERT_LEVEL_NO = 0x00U,                 /**< Indicate alert level for No Alert. */
    BLE_PXPR_ALERT_LEVEL_MILD = 0x01U,               /**< Indicate alert level for Mild Alert. */
    BLE_PXPR_ALERT_LEVEL_HIGH = 0x02U,               /**< Indicate alert level for High Alert. */
} BLE_PXPR_AlertLevel_T;

/**@brief Enumeration type of BLE PXP Reporter callback events. */

typedef enum BLE_PXPR_EventId_T
{
    BLE_PXPR_EVT_LLS_ALERT_LEVEL_WRITE_IND = 0x00U, /**< Event for Alert Level in Link Loss Service is written. See @ref BLE_PXPR_EvtAlertLevelWriteInd_T for event details.*/
    BLE_PXPR_EVT_IAS_ALERT_LEVEL_WRITE_IND,         /**< Event for Alert Level in Immediate Alert service is written. See @ref BLE_PXPR_EvtAlertLevelWriteInd_T for event details.*/
    BLE_PXPR_EVT_ERR_UNSPECIFIED_IND                /**< Profile internal unspecified error occurs. */
}BLE_PXPR_EventId_T;


/**@} */ //BLE_PXPR_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_PXPR_STRUCTS Structures
 * @{ */

/**@brief Data structure for @ref BLE_PXPR_EVT_LLS_ALERT_LEVEL_WRITE_IND, @ref BLE_PXPR_EVT_IAS_ALERT_LEVEL_WRITE_IND event. */
typedef struct BLE_PXPR_EvtAlertLevelWriteInd_T
{
    uint16_t                            connHandle;       /**< The connection handle to request writing alert level. */
    BLE_PXPR_AlertLevel_T               alertLevel;       /**< The level that the request wants to write. See @ref BLE_PXPR_AlertLevel_T for the detail. */
} BLE_PXPR_EvtAlertLevelWriteInd_T;


/**@brief Union of BLE PXP Reporter callback event data types.*/
typedef union
{
    BLE_PXPR_EvtAlertLevelWriteInd_T    evtLlsAlertLevelWriteInd;       /**< Handle @ref BLE_PXPR_EVT_LLS_ALERT_LEVEL_WRITE_IND. */
    BLE_PXPR_EvtAlertLevelWriteInd_T    evtIasAlertLevelWriteInd;       /**< Handle @ref BLE_PXPR_EVT_IAS_ALERT_LEVEL_WRITE_IND. */
} BLE_PXPR_EventField_T;


/**@brief BLE PXP Reporter callback event.*/
typedef struct  BLE_PXPR_Event_T
{
    BLE_PXPR_EventId_T                  eventId;            /**< Event ID. See @ref BLE_PXPR_EventId_T.  */
    BLE_PXPR_EventField_T               eventField;         /**< Event field. */
} BLE_PXPR_Event_T;


/**@brief BLE PXP Reporter callback type. This callback function sends BLE PXP Reporter events to the application. */
typedef void(*BLE_PXPR_EventCb_T)(BLE_PXPR_Event_T *p_event);

/**@} */ //BLE_PXPR_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_PXPR_FUNS Functions
 * @{ */

/**@brief Initialize BLE PXP Reporter.
 *
 * @retval MBA_RES_SUCCESS          Success to initialize PXP reporter.
 * @retval MBA_RES_FAIL             Fail to initialize PXP reporter.
 *
 */
uint16_t BLE_PXPR_Init(void);


/**
 * @brief Register BLE PXP Reporter callback.\n
 *       Application must call this API after starting BLE PXP Reporter procedure.
 *
 *
 * @param[in] routine        Client callback function.
 *
 */
void BLE_PXPR_EventRegister(BLE_PXPR_EventCb_T routine);


/**
 * @brief Set Alert Level value of Link Loss Service (LLS) table.
 *
 *
 * @param[in] level                         Alert level.
 *
 * @retval MBA_RES_SUCCESS                  Successfully set alert level.
 * @retval MBA_RES_INVALID_PARA             Invalid parameters.
 */
uint16_t BLE_PXPR_SetLlsAlertLevel(BLE_PXPR_AlertLevel_T level);


/**
 * @brief Set Tx Power Level value of Tx Power Service table.
 *
 *
 * @param[in] level                         Successfully set tx power level.
 *
 */
void BLE_PXPR_SetTxPowerLevel(int8_t level);


/**@brief Handle BLE_Stack events.
 *       This API should be called in the application while caching BLE_Stack events.
 *
 * @param[in] p_stackEvent          Pointer to BLE_Stack events buffer.
 *
*/
void BLE_PXPR_BleEventHandler(STACK_Event_T *p_stackEvent);


/**@} */ //BLE_PXPR_FUNS

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
*/


