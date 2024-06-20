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
  BLE Alert Notification Profile Server Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_anp.h

  Summary:
    This file contains the BLE Alert Notification Profile Server functions for application user.

  Description:
    This file contains the BLE Alert Notification Profile Server functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_ANPS
 * @{
 * @brief Header file for the BLE Alert Notification Profile Server library.
 * @note Definitions and prototypes for the BLE Alert Notification profile server stack layer application programming interface.
 */
#ifndef BLE_ANPS_H
#define BLE_ANPS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "stack_mgr.h"

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
/**@addtogroup BLE_ANPS_DEFINES Defines
 * @{ */

/**@defgroup BLE_ANPS_MAX_CONN_NBR BLE_ANPS_MAX_CONN_NBR
 * @brief The definition of Memory size.
 * @{ */
#define BLE_ANPS_MAX_CONN_NBR                  BLE_GAP_MAX_LINK_NBR    /**< Maximum allowing Conncetion Numbers. */
/** @} */

/**@defgroup BLE_ANPS_CMD   Command definition
 * @brief The definition of command id.
 * @{ */
#define BLE_ANPS_CMD_ENABLE_NEW_ALERT               (0x00U) /**< Enable new alert notification.*/
#define BLE_ANPS_CMD_ENABLE_UNREAD_ALERT_STAT       (0x01U) /**< Enable unread alert status notification.*/
#define BLE_ANPS_CMD_DISABLE_NEW_ALERT              (0x02U) /**< Disable new alert notification.*/
#define BLE_ANPS_CMD_DISABLE_UNREAD_ALERT_STAT      (0x03U) /**< Disable unread alert status notification.*/
#define BLE_ANPS_CMD_NOTI_NEW_ALERT_IMM             (0x04U) /**< Notify new alert immediately.*/
#define BLE_ANPS_CMD_NOTI_UNREAD_ALERT_STAT_IMM     (0x05U) /**< Notify unread alert status immediately.*/
#define BLE_ANPS_CMD_END                            (BLE_ANPS_CMD_NOTI_UNREAD_ALERT_STAT_IMM) /**< End of command ID.*/
/** @} */

/**@defgroup BLE_ANPS_CAT_ID    Category Id
 * @brief The definition of category id. 
 * @{ */
#define BLE_ANPS_CAT_ID_SIMPLE_ALERT                (0x00U) /**< Simple Alert. The title of the alert.*/
#define BLE_ANPS_CAT_ID_EMAIL                       (0x01U) /**< Email. Sender name.*/
#define BLE_ANPS_CAT_ID_NEWS                        (0x02U) /**< News. Title of the news feed.*/
#define BLE_ANPS_CAT_ID_CALL                        (0x03U) /**< Call. Caller name or caller ID.*/
#define BLE_ANPS_CAT_ID_MISSED_CALL                 (0x04U) /**< Missed Call. Caller name or caller ID*/
#define BLE_ANPS_CAT_ID_SMS_MMS                     (0x05U) /**< SMS. Sender name or caller ID.*/
#define BLE_ANPS_CAT_ID_VOICE_MAIL                  (0x06U) /**< Voice Mail. Sender name or caller ID.*/
#define BLE_ANPS_CAT_ID_SCHEDULE                    (0x07U) /**< Schedule. Title of the schedule.*/
#define BLE_ANPS_CAT_ID_HIGH_PRIOR_ALERT            (0x08U) /**< High Prioritized Alert. Title of alert.*/
#define BLE_ANPS_CAT_ID_INSTANT_MSG                 (0x09U) /**< Instant Messaging. Sender name.*/
#define BLE_ANPS_CAT_ID_ALL_ALERT                   (0xFFU) /**< All alert.*/
/** @} */

/**@defgroup BLE_ANPS_SUPP_CAT_ID_BIT_MASK      Support category mask
 * @brief The definition of category id bit mask.
 * @{ */
#define BLE_ANPS_SUPP_CAT_SIMPLE_ALERT              (1U << 0U)   /**< Simple Alert.*/
#define BLE_ANPS_SUPP_CAT_EMAIL                     (1U << 1U)   /**< Email.*/
#define BLE_ANPS_SUPP_CAT_NEWS                      (1U << 2U)   /**< News.*/
#define BLE_ANPS_SUPP_CAT_CALL                      (1U << 3U)   /**< Call.*/
#define BLE_ANPS_SUPP_CAT_MISSED_CALL               (1U << 4U)   /**< Missed Call.*/
#define BLE_ANPS_SUPP_CAT_SMS_MMS                   (1U << 5U)   /**< SMS, MMS.*/
#define BLE_ANPS_SUPP_CAT_VOICE_MAIL                (1U << 6U)   /**< Voice Mail.*/
#define BLE_ANPS_SUPP_CAT_SCHEDULE                  (1U << 7U)   /**< Schedule.*/
#define BLE_ANPS_SUPP_CAT_HIGH_PRIOR_ALERT          (1U << 8U)   /**< High Prioritized Alert.*/
#define BLE_ANPS_SUPP_CAT_INSTANT_MSG               (1U << 9U)   /**< Instant Message.*/
#define BLE_ANPS_SUPP_CAT_RESERVED                  (0xFC00U) /**< Reserved.*/
/** @} */

/**@} */ //BLE_ANPS_DEFINES
/**@addtogroup BLE_ANPS_ENUMS Enumerations
 * @{ */

/**@brief Enumeration type of BLE Alert Notification callback events. */

typedef enum BLE_ANPS_EventId_T
{
    BLE_ANPS_EVT_ANCP_WRITE_IND = 0x00U,     /**< Indication of alert notification control point is written by client. See @ref BLE_ANPS_EvtAncpWriteInd_T for event details.*/
    BLE_ANPS_EVT_ERR_UNSPECIFIED_IND,       /**< Profile internal unspecified error occurs. */ 
    BLE_ANPS_EVT_ERR_NO_MEM_IND             /**< Profile internal error occurs due to insufficient heap memory. */ 
}BLE_ANPS_EventId_T;
/**@} */ //BLE_ANPS_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types 
// *****************************************************************************
// *****************************************************************************

/**@addtogroup BLE_ANPS_STRUCTS Structures
 * @{ */

/**@brief Data structure for @ref BLE_ANPS_EVT_ANCP_WRITE_IND event. */
typedef struct BLE_ANPS_EvtAncpWriteInd_T
{
    uint16_t        connHandle;                     /**< The connection handle.*/
    uint8_t         cmdId;                          /**< The command ID.  See @ref BLE_ANPS_CMD.*/
    uint8_t         catId;                          /**< The category ID. See @ref BLE_ANPS_CAT_ID.*/
}BLE_ANPS_EvtAncpWriteInd_T;

/**@brief Union of BLE Alert Notification profile callback event data types.*/
typedef union
{
    BLE_ANPS_EvtAncpWriteInd_T  evtAncpWriteInd;    /**< Handle @ref BLE_ANPS_EVT_ANCP_WRITE_IND.*/
}BLE_ANPS_EventField_T;

/**@brief BLE Alert Notification profile callback event.*/
typedef struct  BLE_ANPS_Event_T
{
    BLE_ANPS_EventId_T          eventId;            /**< Event ID. */
    BLE_ANPS_EventField_T       eventField;         /**< Event field. */
}BLE_ANPS_Event_T;

/**@brief BLE Alert Notification profile server callback type. This callback function sends BLE Alert Notification profile server events to the application.*/
typedef void(*BLE_ANPS_EventCb_T)(BLE_ANPS_Event_T *p_event);

/**@} */ //BLE_ANPS_STRUCTS

// ***************************************************************************** 
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_ANP_FUNS Functions
 * @{ */


/**
 *@brief Initialize BLE Alert Notification Profile.
 *
 * @retval MBA_RES_SUCCESS       Successfully init BLE Alert Notification Profile. 
 *
 */
uint16_t BLE_ANPS_Init(void);

/**
 *@brief Register BLE Alert Notification profile callback.\n
 *       Application must call this API before starting BLE Alert Notification procedure.
 *
 *
 *@param[in] bleAnpRoutine       Client callback function.
 *
 */
void BLE_ANPS_EventRegister(BLE_ANPS_EventCb_T bleAnpRoutine);

/**
 *@brief Set new alert category.\n
 *
 *@param[in] catMask             Category of new alert are supported. See @ref BLE_ANPS_SUPP_CAT_ID_BIT_MASK.
 *
 *@retval MBA_RES_SUCCESS        Successfully set supported new category.
 *@retval MBA_RES_FAIL           Supported new alert category shall not change while in a connection.
 *@retval MBA_RES_INVALID_PARA   Parameter does not meet the spec.
 */
uint16_t BLE_ANPS_SetSuppNewCat(uint16_t catMask);

/**
 *@brief Set unread alert category.\n
 *
 *@param[in] catMask             Category of unread alert are supported. See @ref BLE_ANPS_SUPP_CAT_ID_BIT_MASK.
 *
 *@retval MBA_RES_SUCCESS        Successfully set supported unread category.
 *@retval MBA_RES_FAIL           Supported unread alert category shall not change while in a connection.
 *@retval MBA_RES_INVALID_PARA   Parameter does not meet the spec.
 */
uint16_t BLE_ANPS_SetSuppUnreadCat(uint16_t catMask);

/**
 *@brief Send new alert.\n
 *
 *
 *@param[in] connHandle           The connection handle.
 *@param[in] catId                Category Id. See @ref BLE_ANPS_CAT_ID.
 *@param[in] numAlert             Number of new alert.
 *@param[in] txtStrLen            Text string information length. Length must be less than or equal to (MTU - 5).
 *@param[in] p_txtStr             Text string infomation.
 *
 *@retval MBA_RES_SUCCESS         Successfully send new alert.
 *@retval MBA_RES_OOM             Internal memory allocation failure.
 *@retval MBA_RES_NO_RESOURCE     No available buffer for transmitting new alert.
 *@retval MBA_RES_INVALID_PARA    Invalid parameters.
 */
uint16_t BLE_ANPS_SendNewAlert(uint16_t connHandle, uint8_t catId, uint8_t numAlert, uint16_t txtStrLen, const uint8_t *p_txtStr);

/**
 *@brief Send unread alert status.\n
 *
 *
 *@param[in] connHandle           The connection handle.
 *@param[in] catId                Category Id. See @ref BLE_ANPS_CAT_ID.
 *@param[in] unreadCnt            Unread count number. 
 *
 *@retval MBA_RES_SUCCESS         Successfully send unread alert status.
 *@retval MBA_RES_OOM             Internal memory allocation failure.
 *@retval MBA_RES_NO_RESOURCE     No available buffer for transmitting unread alert status.
 *@retval MBA_RES_INVALID_PARA    Invalid parameters.
 */
uint16_t BLE_ANPS_SendUnreadAlertStat(uint16_t connHandle, uint8_t catId, uint8_t unreadCnt);


/**@brief Handle BLE_Stack events.
 *       This API should be called in the application while caching BLE_Stack events.
 *
 * @param[in] p_stackEvent        Pointer to BLE_Stack events buffer.
 *
 */
void BLE_ANPS_BleEventHandler(STACK_Event_T *p_stackEvent);


/**@} */ //BLE_ANP_FUNS

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
*/


