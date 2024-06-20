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
  BLE Alert Notification Profile Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_anpc.h

  Summary:
    This file contains the BLE Alert Notification Profile Client functions for application user.

  Description:
    This file contains the BLE Alert Notification Profile Client functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_ANPC
 * @{
 * @brief Header file for the BLE Alert Notification profile Client library.
 * @note Definitions and prototypes for the BLE Alert Notification profile client stack layer application programming interface.
 */
#ifndef BLE_ANPC_H
#define BLE_ANPC_H

// *****************************************************************************
// *****************************************************************************
// Section: Macros for Included Files
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "stack_mgr.h"
#include "ble_gcm/ble_dd.h"
#include "gatt.h"

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
/**@addtogroup BLE_ANPC_DEFINES Defines
 * @{ */

/**@defgroup BLE_ANPC_ERRCODE    Error Code definition
 * @brief The definition of command id.
 * @{ */
#define BLE_ANPC_ERRCODE_COMMAND_NOT_SUPPORTTED     (0xA0U) /**< Command not supportted.*/
/** @} */

/**@defgroup BLE_ANPC_CMD   Command definition
 * @brief The definition of command id.
 * @{ */
#define BLE_ANPC_CMD_ENABLE_NEW_ALERT               (0x00U) /**< Enable new alert notification.*/
#define BLE_ANPC_CMD_ENABLE_UNREAD_ALERT_STAT       (0x01U) /**< Enable unread alert status notification.*/
#define BLE_ANPC_CMD_DISABLE_NEW_ALERT              (0x02U) /**< Disable new alert notification.*/
#define BLE_ANPC_CMD_DISABLE_UNREAD_ALERT_STAT      (0x03U) /**< Disable unread alert status notification.*/
#define BLE_ANPC_CMD_NOTI_NEW_ALERT_IMM             (0x04U) /**< Notify new alert immediately.*/
#define BLE_ANPC_CMD_NOTI_UNREAD_ALERT_STAT_IMM     (0x05U) /**< Notify unread alert status immediately.*/
#define BLE_ANPC_CMD_END                            (BLE_ANPC_CMD_NOTI_UNREAD_ALERT_STAT_IMM) /**< End of command ID.*/
/** @} */

/**@defgroup BLE_ANPC_DESC_MAX_NUM   Maximum number of descriptor
 * @brief The definition of the max number of descriptor.
 * @{ */
#define BLE_ANPC_DESC_MAX_NUM 						(2U)	   /**< Maximum number of descriptor.*/
/** @} */

/**@defgroup BLE_ANPC_CAT_ID    Category Id
 * @brief The definition of category id. 
 * @{ */
#define BLE_ANPC_CAT_ID_SIMPLE_ALERT                (0x00U) /**< Simple Alert. The title of the alert.*/
#define BLE_ANPC_CAT_ID_EMAIL                       (0x01U) /**< Email. Sender name.*/
#define BLE_ANPC_CAT_ID_NEWS                        (0x02U) /**< News. Title of the news feed.*/
#define BLE_ANPC_CAT_ID_CALL                        (0x03U) /**< Call. Caller name or caller ID.*/
#define BLE_ANPC_CAT_ID_MISSED_CALL                 (0x04U) /**< Missed Call. Caller name or caller ID*/
#define BLE_ANPC_CAT_ID_SMS_MMS                     (0x05U) /**< SMS. Sender name or caller ID.*/
#define BLE_ANPC_CAT_ID_VOICE_MAIL                  (0x06U) /**< Voice Mail. Sender name or caller ID.*/
#define BLE_ANPC_CAT_ID_SCHEDULE                    (0x07U) /**< Schedule. Title of the schedule.*/
#define BLE_ANPC_CAT_ID_HIGH_PRIOR_ALERT            (0x08U) /**< High Prioritized Alert. Title of alert.*/
#define BLE_ANPC_CAT_ID_INSTANT_MSG                 (0x09U) /**< Instant Messaging. Sender name.*/
#define BLE_ANPC_CAT_ID_ALL_ALERT                   (0xFFU) /**< All alert.*/
/** @} */

/**@defgroup BLE_ANPC_SUPP_CAT_ID_BIT_MASK          Supported category mask
 * @brief The definition of category id bit mask.
 * @{ */
#define BLE_ANPC_SUPP_CAT_SIMPLE_ALERT              (1U << 0U) /**< Simple Alert.*/
#define BLE_ANPC_SUPP_CAT_EMAIL                     (1U << 1U) /**< Email.*/
#define BLE_ANPC_SUPP_CAT_NEWS                      (1U << 2U) /**< News.*/
#define BLE_ANPC_SUPP_CAT_CALL                      (1U << 3U) /**< Call.*/
#define BLE_ANPC_SUPP_CAT_MISSED_CALL               (1U << 4U) /**< Missed Call.*/
#define BLE_ANPC_SUPP_CAT_SMS_MMS                   (1U << 5U) /**< SMS, MMS.*/
#define BLE_ANPC_SUPP_CAT_VOICE_MAIL                (1U << 6U) /**< Voice Mail.*/
#define BLE_ANPC_SUPP_CAT_SCHEDULE                  (1U << 7U) /**< Schedule.*/
#define BLE_ANPC_SUPP_CAT_HIGH_PRIOR_ALERT          (1U << 8U) /**< High Prioritized Alert.*/
#define BLE_ANPC_SUPP_CAT_INSTANT_MSG               (1U << 9U) /**< Instant Message.*/
/** @} */

/**@defgroup BLE_ANPC_UUID    UUID of characteristic in Alert Notification profile
 * @brief The definition of UUID of characteristics are used in Alert Notification profile.
 * @{ */
#define BLE_ANPC_UUID_ANCP                          (0x2A44U) /**< Alert Notification Control Point UUID .*/
#define BLE_ANPC_UUID_UNREAD_ALERT_STAT             (0x2A45U) /**< Unread Alert Status UUID.*/
#define BLE_ANPC_UUID_NEW_ALERT                     (0x2A46U) /**< New Alert UUID.*/
#define BLE_ANPC_UUID_SUPP_NEW_ALERT_CAT            (0x2A47U) /**< Supported New Alert Category UUID.*/
#define BLE_ANPC_UUID_SUPP_UNREAD_ALERT_CAT         (0x2A48U) /**< Support Unread Alert Category UUID.*/
/** @} */
/**@} */ //BLE_ANPC_DEFINES

/**@addtogroup BLE_ANPC_ENUMS Enumerations
 * @{ */

/**@brief Enumeration type of BLE ANPC callback events. */

typedef enum BLE_ANPC_EventId_T
{
    BLE_ANPC_EVT_DISC_COMPLETE_IND = 0x00U,         /**< Event for discovery completion. See @ref BLE_ANPC_EvtDiscComplete_T for event details.*/
    BLE_ANPC_EVT_SUPP_NEW_ALERT_CAT_IND,           /**< Event for receiving read supported new alert category response. See @ref BLE_ANPC_EvtSuppNewAlertCatInd_T for event details.*/
    BLE_ANPC_EVT_SUPP_UNREAD_ALERT_STAT_CAT_IND,   /**< Event for receiving read supported unread alert status category response. See @ref BLE_ANPC_EvtSuppUnreadAlertCatInd_T for event details.*/
    BLE_ANPC_EVT_WRITE_NEW_ALERT_NTFY_RSP_IND,     /**< Event for receiving write new alert response. See @ref BLE_ANPC_EvtWriteNewAlertRspInd_T for event details.*/
    BLE_ANPC_EVT_WRITE_UNREAD_ALERT_NTFY_RSP_IND,  /**< Event for receiving write unread alert response. See @ref BLE_ANPC_EvtWriteUnreadAlertRspInd_T for event details.*/
    BLE_ANPC_EVT_WRITE_CP_RSP_IND,                 /**< Event for receiving write cp response. See @ref BLE_ANPC_EvtWriteCpRspInd_T for event details.*/
    BLE_ANPC_EVT_NEW_ALERT_IND,                    /**< Event for receiving new alert notification. See @ref BLE_ANPC_EvtNewAlertInd_T for event details.*/
    BLE_ANPC_EVT_UNREAD_ALERT_STAT_IND,            /**< Event for receiving unread alert status notification. See @ref BLE_ANPC_EvtUnreadAlertStatInd_T for event details.*/
    BLE_ANPC_EVT_ERR_UNSPECIFIED_IND,              /**< Profile internal unspecified error occurs.*/
    BLE_ANPC_EVT_ERR_NO_MEM_IND,                   /**< Profile internal error occurs due to insufficient heap memory.*/
}BLE_ANPC_EventId_T;

/**@} */ //BLE_ANPC_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_ANPC_STRUCTS Structures
 * @{ */

/**@brief Data structure for @ref BLE_ANPC_EVT_DISC_COMPLETE_IND event. */
typedef struct BLE_ANPC_EvtDiscComplete_T
{
    uint16_t        connHandle;         /**< The connection handle of discovery completion. */
    uint16_t        ansStartHandle;     /**< The start handle of ANS. */
    uint16_t        ansEndHandle;       /**< The end handle of ANS. */
}BLE_ANPC_EvtDiscComplete_T;

/**@brief Data structure for @ref BLE_ANPC_EVT_SUPP_NEW_ALERT_CAT_IND event. */
typedef struct BLE_ANPC_EvtSuppNewAlertCatInd_T
{
    uint16_t        connHandle;         /**< The connection handle. */
    uint16_t        category;           /**< Supported New Alert Category. See @ref BLE_ANPC_SUPP_CAT_ID_BIT_MASK.*/
}BLE_ANPC_EvtSuppNewAlertCatInd_T;

/**@brief Data structure for @ref BLE_ANPC_EVT_SUPP_UNREAD_ALERT_STAT_CAT_IND event. */
typedef struct BLE_ANPC_EvtSuppUnreadAlertCatInd_T
{
    uint16_t        connHandle;         /**< The connection handle. */
    uint16_t        category;           /**< Supported Unread Alert Category. See @ref BLE_ANPC_SUPP_CAT_ID_BIT_MASK.*/
}BLE_ANPC_EvtSuppUnreadAlertCatInd_T;

/**@brief Data structure for @ref BLE_ANPC_EVT_WRITE_NEW_ALERT_NTFY_RSP_IND event. */
typedef struct BLE_ANPC_EvtWriteNewAlertRspInd_T
{
    uint16_t        connHandle;         /**< The connection handle.*/
    uint16_t        errCode;            /**< Error code. See @ref ATT_ERROR_CODES.*/
}BLE_ANPC_EvtWriteNewAlertRspInd_T;

/**@brief Data structure for @ref BLE_ANPC_EVT_WRITE_UNREAD_ALERT_NTFY_RSP_IND event. */
typedef struct BLE_ANPC_EvtWriteUnreadAlertRspInd_T
{
    uint16_t        connHandle;         /**< The connection handle.*/
    uint16_t        errCode;            /**< Error code. See @ref ATT_ERROR_CODES.*/
}BLE_ANPC_EvtWriteUnreadAlertRspInd_T;

/**@brief Data structure for @ref BLE_ANPC_EVT_WRITE_CP_RSP_IND event. */
typedef struct BLE_ANPC_EvtWriteCpRspInd_T
{
    uint16_t        connHandle;         /**< The connection handle.*/
    uint16_t        errCode;            /**< The error code of config control point response. See @ref BLE_ANPC_ERRCODE. */
}BLE_ANPC_EvtWriteCpRspInd_T;

/**@brief Data structure for @ref BLE_ANPC_EVT_NEW_ALERT_IND event. */
typedef struct BLE_ANPC_EvtNewAlertInd_T
{
    uint16_t        connHandle;         /**< The connection handle.*/
    uint8_t         categoryId;         /**< The category Id. See @ref BLE_ANPC_CAT_ID.*/
    uint8_t         numOfNewAlert;      /**< Number of new alert.*/
    uint16_t        receivedLength;     /**< Receive txt string length.*/
    uint8_t         *p_receivedValue;   /**< Point of txt string data.*/
}BLE_ANPC_EvtNewAlertInd_T;

/**@brief Data structure for @ref BLE_ANPC_EVT_UNREAD_ALERT_STAT_IND event. */
typedef struct BLE_ANPC_EvtUnreadAlertStatInd_T
{
    uint16_t        connHandle;         /**< The connection handle.*/
    uint8_t         categoryId;         /**< The category Id. See @ref BLE_ANPC_CAT_ID.*/
    uint8_t         unreadCnt;          /**< Unread Count.*/
}BLE_ANPC_EvtUnreadAlertStatInd_T;

/**@brief Data structure for BLE_ANPC_GetCharList function. */
typedef struct BLE_ANPC_CharList_T
{
    uint16_t   		attrHandle;         /**< Attribute handle.*/
    uint8_t    		property;           /**< Property.*/
    uint16_t   		charHandle;         /**< Characteristic handle.*/
}BLE_ANPC_CharList_T;

/**@brief Information structure for @ref BLE_ANPC_GetDescList function. */
typedef struct BLE_ANPC_DescInfo_T
{
    uint16_t  		attrHandle;   		/**< Attribute handle of the descriptor.*/
    uint16_t  		uuid;         		/**< UUID of the descriptor.*/
}BLE_ANPC_DescInfo_T;

/**@brief Data structure for @ref BLE_ANPC_GetDescList function. */
typedef struct BLE_ANPC_DescList_T
{
    uint8_t 		    totalNum;  				             /**< Total number of the descriptor.*/
    BLE_ANPC_DescInfo_T descInfo[BLE_ANPC_DESC_MAX_NUM]; /**< Discovered informations.*/
}BLE_ANPC_DescList_T;

/**@brief Union of BLE Alert Notification Client callback event data types. */
typedef union
{
    BLE_ANPC_EvtDiscComplete_T           evtDiscComplete;          /**< Handle @ref BLE_ANPC_EVT_DISC_COMPLETE_IND. */
    BLE_ANPC_EvtSuppNewAlertCatInd_T     evtSuppNewAlertCatInd;    /**< Handle @ref BLE_ANPC_EVT_SUPP_NEW_ALERT_CAT_IND. */
    BLE_ANPC_EvtSuppUnreadAlertCatInd_T  evtSuppUnreadAlertCatInd; /**< Handle @ref BLE_ANPC_EVT_SUPP_UNREAD_ALERT_STAT_CAT_IND. */
    BLE_ANPC_EvtWriteNewAlertRspInd_T    evtWriteNewAlertRspInd;   /**< Handle @ref BLE_ANPC_EVT_WRITE_NEW_ALERT_NTFY_RSP_IND. */
    BLE_ANPC_EvtWriteUnreadAlertRspInd_T evtWriteUnreadAlertRspInd;/**< Handle @ref BLE_ANPC_EVT_WRITE_UNREAD_ALERT_NTFY_RSP_IND. */
    BLE_ANPC_EvtWriteCpRspInd_T          evtWriteCpRspInd;         /**< Handle @ref BLE_ANPC_EVT_WRITE_CP_RSP_IND. */
    BLE_ANPC_EvtNewAlertInd_T            evtNewAlertInd;           /**< Handle @ref BLE_ANPC_EVT_NEW_ALERT_IND. */
    BLE_ANPC_EvtUnreadAlertStatInd_T     evtUnreadAlertStatInd;    /**< Handle @ref BLE_ANPC_EVT_UNREAD_ALERT_STAT_IND. */
}BLE_ANPC_EventField_T;

/**@brief BLE Alert Notification Client callback event. */
typedef struct  BLE_ANPC_Event_T
{
    BLE_ANPC_EventId_T                   eventId;                  /**< Event ID. See @ref BLE_ANPC_EventId_T.  */
    BLE_ANPC_EventField_T                eventField;               /**< Event field. */
}BLE_ANPC_Event_T;

/**@brief BLE Alert Notification profile Client callback type. This callback function sends BLE Alert Notification profile client events to the application.*/
typedef void(*BLE_ANPC_EventCb_T)(BLE_ANPC_Event_T *p_event);

/**@} */ //BLE_ANPC_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_ANPC_FUNS Functions
 * @{ */

/**
 * @brief Initialize BLE Alert Notification Client.
 *
 * @retval MBA_RES_SUCCESS          Success to initalize BLE Alert Notification Client.
 * @retval MBA_RES_FAIL             Fail to initalize BLE Alert Notification Client.
 *
 */
uint16_t BLE_ANPC_Init(void);

/**
 * @brief Register BLE Alert Notification Client callback.\n
 *       Application must call this API after starting BLE Alert Notification Client procedure.
 *
 * @param[in] routine        Client callback function.
 *
 */
void BLE_ANPC_EventRegister(BLE_ANPC_EventCb_T routine);

/**
 * @brief Read the value of Supported New Alert Category.\n
 *       Application must call this API after starting BLE Alert Notification Client procedure.
 *
 * @param[in] connHandle            Handle of the connection.
 *
 * @retval MBA_RES_SUCCESS          Successfully issue a read Supported New Alert Category packet.
 * @retval MBA_RES_INVALID_PARA     Invalid connection handle.
 * @retval MBA_RES_FAIL             Fail to issue a read Supported New Alert Category packet.
 */
uint16_t BLE_ANPC_ReadSuppNewAlertCat(uint16_t connHandle);

/**
 * @brief Read the value of Supported Unread Alert Category.\n
 *       Application must call this API after starting BLE Alert Notification Client procedure.
 *
 * @param[in] connHandle            Handle of the connection.
 *
 * @retval MBA_RES_SUCCESS          Successfully issue a read Supported Unread Alert Category packet.
 * @retval MBA_RES_INVALID_PARA     Invalid connection handle.
 * @retval MBA_RES_FAIL             Fail to issue a read Supported Unread Alert Category packet.
 */
uint16_t BLE_ANPC_ReadSuppUnreadAlertCat(uint16_t connHandle);

/**
 * @brief Enable New Alert Notification.\n
 *       Application must call this API after starting BLE Alert Notification Client procedure.
 *
 * @param[in] connHandle            Handle of the connection.
 * @param[in] enable                Set true to enable notification of New Alert. Otherwise set false.
 *
 * @retval MBA_RES_SUCCESS          Successfully enable New Alert notification.
 * @retval MBA_RES_INVALID_PARA     Invalid connection handle.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 */
uint16_t BLE_ANPC_EnableNewAlertNtfy(uint16_t connHandle, bool enable);

/**
 * @brief Enable Unread Alert Notificaiton.\n
 *       Application must call this API after starting BLE Alert Notification Client procedure.
 *
 * @param[in] connHandle            Handle of the connection.
 * @param[in] enable                Set true to enable notification of Unread Alert. Otherwise set false.
 *
 * @retval MBA_RES_SUCCESS          Successfully enable Unread Alert notification.
 * @retval MBA_RES_INVALID_PARA     Invalid connection handle.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 */
uint16_t BLE_ANPC_EnableUnreadAlertNtfy(uint16_t connHandle, bool enable);

/**
 * @brief Write Alert Notification Control Point.\n
 *       Application must call this API after starting BLE Alert Notification Client procedure.
 *
 * @param[in] connHandle            Handle of the connection.
 * @param[in] cmdId                 The command Id. See @ref BLE_ANPC_CMD.
 * @param[in] catId                 The category Id. See @ref BLE_ANPC_CAT_ID.
 *
 * @retval MBA_RES_SUCCESS          Successfully issue a wirte packet to Alert Notification Control Point.
 * @retval MBA_RES_INVALID_PARA     Invalid connection handle.
 * @retval MBA_RES_OOM              Internal memory allocation failure.
 */
uint16_t BLE_ANPC_WriteAncp(uint16_t connHandle, uint8_t cmdId, uint8_t catId);

/**
 * @brief Get information about characteristic UUID of the Alert Notification services that has been discovered.
 *       This API could be called only after @ref BLE_ANPC_EVT_DISC_COMPLETE_IND event is issued.
 *
 * @param[in]  connHandle           Handle of the connection.
 * @param[in]  charUuid             Characteristic uuid. See @ref BLE_ANPC_UUID.
 * @param[out] p_charList           Characteristic information of discovered service.  When the characteristic UUID is not found then characteristic list will be 0. 
 *
 * @retval MBA_RES_SUCCESS          Successfully get the characteristic list.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters.
 *                                  - Connection handle is not valid. 
 *                                  - Characteristic Uuid is not valid. 
 */
uint16_t BLE_ANPC_GetCharList(uint16_t connHandle, uint16_t charUuid, BLE_ANPC_CharList_T *p_charList);

/**
 * @brief Get information about descriptor list within start handle and end handle of the Alert Notification Service that has been discovered.
 *       This API could be called only after @ref BLE_ANPC_EVT_DISC_COMPLETE_IND event is issued.
 *
 * @param[in]  connHandle           Handle of the connection.
 * @param[out] p_descList           Descriptor information of the discovered service. When the descriptor is not found then descriptor list will be 0. 
 *
 * @retval MBA_RES_SUCCESS          Successfully get the descriptor list.
 * @retval MBA_RES_INVALID_PARA     Invalid parameters. Connection handle is not valid.
 */
uint16_t BLE_ANPC_GetDescList(uint16_t connHandle, BLE_ANPC_DescList_T *p_descList);

/**
 * @brief Handle BLE_Stack related events.
 *       This API should be called in the application while caching BLE_Stack events.
 *
*/
void BLE_ANPC_BleEventHandler(STACK_Event_T *p_stackEvent);

/**
 * @brief Handle BLE DD (Database Discovery middleware) events.
 *
 * @param[in] p_event          Pointer to BLE DD events buffer.
 *
*/
void BLE_ANPC_BleDdEventHandler(BLE_DD_Event_T *p_event);

/**@} */ //BLE_ANPC_FUNS

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
*/


