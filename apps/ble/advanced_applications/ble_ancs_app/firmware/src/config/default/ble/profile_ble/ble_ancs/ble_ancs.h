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
  BLE ANCS Profile Source File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_ancs.h

  Summary:
    This file contains the BLE ANCS functions for application user.

  Description:
    This file contains the BLE ANCS for application user.
 *******************************************************************************/
/** @addtogroup BLE_PROFILE BLE Profile
 *  @{ */

/** 
 * @defgroup BLE_ANCS BLE ANCS
 * @{
 * @brief Header file for the BLE ANCS.
 * @note Definitions and prototypes for the BLE ANCS stack layer application programming interface.
 */
#ifndef BLE_ANCS_H
#define BLE_ANCS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

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
/**@addtogroup BLE_ANCS_DEFINES Defines
 * @{ */
 
/**@defgroup BLE_ANCS_DECODE_LEN    ANCS length definition
 * @brief The definition of maximum length for decoded attribute. 
 * @{ */
#define BLE_ANCS_MAX_PACKET_BUFFER_SIZE     (1024U)        /**< Maximum length of buffer for reassembled packet. */
#define BLE_ANCS_MAX_APPID_LEN              (32U)          /**< Maximum length of App Identifier. */      
#define BLE_ANCS_MAX_TITLE_LEN              (32U)          /**< Maximum length of Title. */
#define BLE_ANCS_MAX_SUB_TITLE_LEN          (32U)          /**< Maximum length of Subtitle. */
#define BLE_ANCS_MAX_MESSAGE_LEN            (512U)         /**< Maximum length of Message. */
#define BLE_ANCS_MAX_MESSAGE_SIZE_LEN       (5U)           /**< Maximum length of Message Size. */      
#define BLE_ANCS_MAX_DATE_LEN               (18U)          /**< Maximum length of Date Size. */   
#define BLE_ANCS_MAX_POS_ACTION_LABEL_LEN   (8U)           /**< Maximum length of Positive Action content. */      
#define BLE_ANCS_MAX_NEG_ACTION_LABEL_LEN   (8U)           /**< Maximum length of Negative action content. */  
#define BLE_ANCS_MAX_APP_NAME_LEN           (16U)          /**< Maximum length of Application Name. */  
/** @} */

/**@defgroup ANCS_ERROR_CODES  ANCS error code
 * @brief The definition of ANCS Error Codes
 * @{ */
#define ANCS_ERRCODE_UNKNOWN_COMMAND        0xA0       /**< The commandID was not recognized by the NP.*/
#define ANCS_ERRCODE_INVALID_COMMAND        0xA1       /**< The command was improperly formatted.*/
#define ANCS_ERRCODE_INVALID_PARAMETER      0xA2       /**< One of the parameters does not refer to an existing object on the NP.*/
#define ANCS_ERRCODE_ACTION_FAILED          0xA3       /**< The action was not performed.*/
/** @} */
       
/**@} */ //BLE_ANCS_DEFINES     


/**@addtogroup BLE_ANCS_ENUMS Enumerations
 * @{ */

/**@brief ActionID for iOS Perform Notification Action. */
typedef enum BLE_ANCS_ActionId_T
{
    BLE_ANCS_ACTION_ID_POSITIVE = 0,                    /**< Positive action. */
    BLE_ANCS_ACTION_ID_NEGATIVE,                        /**< Negative action. */
    BLE_ANCS_ACTION_ID_MAX                              /**< Undefined action. */
} BLE_ANCS_ActionId_T;

/**@brief  A numerical value providing a category in which the iOS notification can be classified. */
typedef enum BLE_ANCS_CategoryId_T
{
    BLE_ANCS_CATEGORY_ID_OTHER,                         /**< The iOS notification belongs to the "Other" category.  */
    BLE_ANCS_CATEGORY_ID_INCOMING_CALL,                 /**< The iOS notification belongs to the "Incoming Call" category. */
    BLE_ANCS_CATEGORY_ID_MISSED_CALL,                   /**< The iOS notification belongs to the "Missed Call" category. */
    BLE_ANCS_CATEGORY_ID_VOICE_MAIL,                    /**< The iOS notification belongs to the "Voice Mail" category. */
    BLE_ANCS_CATEGORY_ID_SOCIAL,                        /**< The iOS notification belongs to the "Social" category. */
    BLE_ANCS_CATEGORY_ID_SCHEDULE,                      /**< The iOS notification belongs to the "Schedule" category. */
    BLE_ANCS_CATEGORY_ID_EMAIL,                         /**< The iOS notification belongs to the "Email" category. */
    BLE_ANCS_CATEGORY_ID_NEWS,                          /**< The iOS notification belongs to the "News" category. */
    BLE_ANCS_CATEGORY_ID_HEALTH_AND_FITNESS,            /**< The iOS notification belongs to the "Health and Fitness" category. */
    BLE_ANCS_CATEGORY_ID_BUSINESS_AND_FINANCE,          /**< The iOS notification belongs to the "Business and Finance" category. */
    BLE_ANCS_CATEGORY_ID_LOCATION,                      /**< The iOS notification belongs to the "Location" category. */
    BLE_ANCS_CATEGORY_ID_ENTERTAINMENT,                 /**< The iOS notification belongs to the "Entertainment" category. */
    BLE_ANCS_CATEGORY_ID_MAX                          	/**< Undefined category id. */
} BLE_ANCS_CategoryId_T;

/**@brief BLE ANCS event received by the application. */
typedef enum BLE_ANCS_EventId_T
{
    BLE_ANCS_EVT_DISC_COMPLETE_IND,                     /**< Event for discovery completion. See @ref BLE_ANCS_EvtDiscComplete_T for event details.*/
    BLE_ANCS_EVT_ERR_ATTR_BUF_IND,                      /**< The size of received data is larger than the attribute buffer. See @ref BLE_ANCS_EvtErrAttrBufInd_T for event details.*/
    BLE_ANCS_EVT_ERR_RECOMPOSE_BUF_IND,                 /**< The size of received data is larger than the recomposed buffer. See @ref BLE_ANCS_EvtErrRecomposeBufInd_T for event details.*/
    BLE_ANCS_EVT_NTFY_ADDED_IND,                        /**< The arrival of a new iOS notification on the NP. See @ref BLE_ANCS_EvtNtfyInd_T for event details.*/
    BLE_ANCS_EVT_NTFY_MODIFIED_IND,                     /**< The modification of an iOS notification on the NP. See @ref BLE_ANCS_EvtNtfyInd_T for event details.*/
    BLE_ANCS_EVT_NTFY_REMOVED_IND,                      /**< The removal of an iOS notification on the NP. See @ref BLE_ANCS_EvtNtfyInd_T for event details.*/
    BLE_ANCS_EVT_NTFY_ATTR_IND,                         /**< Notification attribute received. See @ref BLE_ANCS_EvtNtfyAttrInd_T for event details.*/
    BLE_ANCS_EVT_APP_ATTR_IND,                          /**< App attribute received. See @ref BLE_ANCS_EvtAppAttrInd_T for event details.*/
    BLE_ANCS_EVT_ERR_UNSPECIFIED_IND,                   /**< Profile internal unspecified error occurs. */
    BLE_ANCS_EVT_ERR_NO_MEM_IND,                        /**< Profile internal error occurs due to insufficient heap memory. */
    BLE_ANCS_EVT_ERR_IND,                               /**< Profile writing to the Control Point characteristic, an NC may receive the following ANCS-specific error codes. */
} BLE_ANCS_EventId_T;

/**@brief NotificationAttributeID for iOS Get Notification Attributes. */
typedef enum BLE_ANCS_NtfyAttrId_T
{
    BLE_ANCS_NTFY_ATTR_ID_APP_IDENTIFIER = 0x00U,           /**< Identifies that the attribute data is of an "App Identifier" type. */
    BLE_ANCS_NTFY_ATTR_ID_TITLE,                        /**< Identifies that the attribute data is a "Title". */
    BLE_ANCS_NTFY_ATTR_ID_SUBTITLE,                     /**< Identifies that the attribute data is a "Subtitle". */
    BLE_ANCS_NTFY_ATTR_ID_MESSAGE,                      /**< Identifies that the attribute data is a "Message". */
    BLE_ANCS_NTFY_ATTR_ID_MESSAGE_SIZE,                 /**< Identifies that the attribute data is a "Message Size". */
    BLE_ANCS_NTFY_ATTR_ID_DATE,                         /**< Identifies that the attribute data is a "Date". */
    BLE_ANCS_NTFY_ATTR_ID_POSITIVE_ACTION_LABEL,        /**< The notification has a "Positive action" that can be executed associated with it. */
    BLE_ANCS_NTFY_ATTR_ID_NEGATIVE_ACTION_LABEL,        /**< The notification has a "Negative action" that can be executed associated with it. */
    BLE_ANCS_NTFY_ATTR_ID_MAX                           /**< Undefined notification attribute id. */
} BLE_ANCS_NtfyAttrId_T;

/**@} */ //BLE_ANPC_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_ANCS_STRUCTS Structures
 * @{ */

/**@brief The structure contains iOS notification attributes id. */
typedef struct {
    uint8_t                     appId           : 1;    /**< Set true if App Identifier is enabled. */
    uint8_t                     title           : 1;    /**< Set true if Title is enabled. */
    uint8_t                     subtitle        : 1;    /**< Set true if Sub Title is enabled. */
    uint8_t                     msg             : 1;    /**< Set true if Message is enabled. */
    uint8_t                     msgSize         : 1;    /**< Set true if Message Size is enabled. */
    uint8_t                     date            : 1;    /**< Set true if Date is enabled. */
    uint8_t                     positiveAction  : 1;    /**< Set true if Positive Action is enabled. */
    uint8_t                     negativeAction  : 1;    /**< Set true if Negative Action is enabled. */
} BLE_ANCS_NtfyAttrsMask_T;

/**@brief The structure contains iOS app attributes id. */
typedef struct {
    uint8_t                     displayName     : 1;    /**< Set true if Display name is enable. */
    uint8_t                     reserved        : 7;    /**< Reserved. */
} BLE_ANCS_AppAttrsMask_T;


/**@brief A bitmask of Event Flag whose set bits inform an NC of specificities with the iOS notification. */
typedef struct
{
    uint8_t                     silent          : 1;    /**< Set true if the notification is low priority. */
    uint8_t                     important       : 1;    /**< Set true if the notification is high priority. */
    uint8_t                     preExisting     : 1;    /**< Set true if the notification is pre-existing. */
    uint8_t                     positiveAction  : 1;    /**< Set true if the notification has a positive action that can be taken. */
    uint8_t                     negativeAction  : 1;    /**< Set true if the notification has a negative action that can be taken. */
} BLE_ANCS_NtfyEvtFlagMask_T; 

/**@brief The structure contains iOS notification attributes decoded information. */
typedef struct
{
    BLE_ANCS_NtfyAttrsMask_T    bitmask;                        /**< Notification attribute bitmask. It's used to indicate whether the following filed is enabled or not. */
    uint32_t                    ntfyId;                         /**< Notification UID. */
    uint8_t                     appId[BLE_ANCS_MAX_APPID_LEN];  /**< Valid if App Identifier is enabled. */
    uint8_t                     *p_title;                       /**< Valid if Title is enabled. */
    uint8_t                     *p_subtitle;                    /**< Valid if Sub Title is enabled. */
    uint8_t                     *p_msg;                         /**< Valid if Message is enabled. */
    uint16_t                    msgSize;                        /**< Valid if Message Size is enabled. */
    uint8_t                     date[BLE_ANCS_MAX_DATE_LEN];    /**< Valid if Date is enabled. */
    uint8_t                     positiveAction[BLE_ANCS_MAX_POS_ACTION_LABEL_LEN]; /**< Valid if Positive Action is enabled. */
    uint8_t                     negativeAction[BLE_ANCS_MAX_NEG_ACTION_LABEL_LEN]; /**< Valid if Negative Action is enabled. */

} BLE_ANCS_DecodeNtfyAttrs_T;

/**@brief The structure contains iOS app attributes decoded information. */
typedef struct
{
    uint8_t                     appId[BLE_ANCS_MAX_APPID_LEN];          /**< App Identifier. */
    uint8_t                     displayName[BLE_ANCS_MAX_APP_NAME_LEN]; /**< Display Name. */
} BLE_ANCS_DecodeAppAttrs_T;

/**@brief Data structure for @ref BLE_ANCS_EVT_NTFY_ADDED_IND, @ref BLE_ANCS_EVT_NTFY_MODIFIED_IND, @ref BLE_ANCS_EVT_NTFY_REMOVED_IND event. */
typedef struct
{
    uint16_t                    connHandle;             /**< Connection handle associated with this connection. */
    uint32_t                    ntfyId;                 /**< Notification UID. A 32-bit numerical value that is the unique identifier (UID) for the iOS notification. */
    BLE_ANCS_NtfyEvtFlagMask_T  ntfyEvtFlagMask;        /**< Bitmask to signal whether a special condition applies to the notification. For example, "Silent" or "Important". */
    BLE_ANCS_CategoryId_T       categoryId;             /**< Classification of the notification type. For example, email or location. */
    uint8_t                     categoryCount;          /**< Current number of active iOS notifications in the given category. */
} BLE_ANCS_EvtNtfyInd_T;

/**@brief Data structure for @ref BLE_ANCS_EVT_NTFY_ATTR_IND event. */
typedef struct
{
    uint16_t                    connHandle;             /**< Connection handle associated with this connection. */
    BLE_ANCS_DecodeNtfyAttrs_T  *p_data;                /**< Decoded iOS notification attribute information. See @ref BLE_ANCS_DecodeNtfyAttrs_T.*/
} BLE_ANCS_EvtNtfyAttrInd_T;

/**@brief Data structure for @ref BLE_ANCS_EVT_APP_ATTR_IND event. */
typedef struct           
{
    uint16_t                    connHandle;             /**< Connection handle associated with this connection. */
    BLE_ANCS_DecodeAppAttrs_T   *p_data;                /**< Decoded app attribute information. See @ref BLE_ANCS_DecodeAppAttrs_T.*/
} BLE_ANCS_EvtAppAttrInd_T;

/**@brief Data structure for @ref BLE_ANCS_EVT_DISC_COMPLETE_IND event. */
typedef struct BLE_ANCS_EvtDiscComplete_T
{
    uint16_t                    connHandle;             /**< The connection handle of discovery completion. */
}BLE_ANCS_EvtDiscComplete_T;

/**@brief Data structure for @ref BLE_ANCS_EVT_ERR_ATTR_BUF_IND event. */
typedef struct
{
    uint16_t                    connHandle;             /**< Connection handle associated with this connection. */
    uint8_t                     attrId;                 /**< Attribute ID. See @ref BLE_ANCS_NtfyAttrId_T. */
    uint16_t                    len;                    /**< length of the attribute. */
} BLE_ANCS_EvtErrAttrBufInd_T;

/**@brief Data structure for @ref BLE_ANCS_EVT_ERR_RECOMPOSE_BUF_IND event. */
typedef struct
{
    uint16_t                    connHandle;             /**< Connection handle associated with this connection. */
} BLE_ANCS_EvtErrRecomposeBufInd_T;

/**@brief Data structure for @ref BLE_ANCS_EVT_ERR_IND event. */
typedef struct
{
    uint16_t    				connHandle;        		/**< Connection handle associated with this connection. */
    uint8_t     				errCode;           		/**< Reason why the request has generated an error response. See @ref ANCS_ERROR_CODES.*/
} BLE_ANCS_EvtErrInd_T;

/**@brief Union of BLE ANCS callback event data types. */
typedef union
{
    BLE_ANCS_EvtDiscComplete_T         evtDiscComplete;             /**< Handle @ref BLE_ANCS_EVT_DISC_COMPLETE_IND */
    BLE_ANCS_EvtErrAttrBufInd_T        evtErrAttrBufInd;            /**< Handle @ref BLE_ANCS_EVT_ERR_ATTR_BUF_IND. */
    BLE_ANCS_EvtErrRecomposeBufInd_T   evtErrRecomposeBufInd;       /**< Handle @ref BLE_ANCS_EVT_ERR_RECOMPOSE_BUF_IND. */
    BLE_ANCS_EvtNtfyInd_T              evtNtfyInd;                  /**< Handle @ref BLE_ANCS_EVT_NTFY_ADDED_IND, @ref BLE_ANCS_EVT_NTFY_MODIFIED_IND, @ref BLE_ANCS_EVT_NTFY_REMOVED_IND. */
    BLE_ANCS_EvtNtfyAttrInd_T          evtNtfyAttrInd;              /**< Handle @ref BLE_ANCS_EVT_NTFY_ATTR_IND. */
    BLE_ANCS_EvtAppAttrInd_T           evtAppAttrInd;               /**< Handle @ref BLE_ANCS_EVT_APP_ATTR_IND. */
    BLE_ANCS_EvtErrInd_T               evtErrInd;               	/**< Handle @ref BLE_ANCS_EVT_ERR_IND. */
} BLE_ANCS_EventField_T;

/**@brief BLE ANCS callback event. */
typedef struct
{
    BLE_ANCS_EventId_T          eventId;                /**< Event ID.*/
    BLE_ANCS_EventField_T       eventField;             /**< Event field. */
} BLE_ANCS_Event_T;

/**@brief BLE ANCS callback type. This callback function sends BLE ANCS events to the application. */
typedef void(*BLE_ANCS_EventCb_T)(BLE_ANCS_Event_T *p_event);

/**@} */ //BLE_ANCS_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_ANCS_FUNS Functions
 * @{ */ 

/**@brief Initialize ANCS profile.
 *
 * @retval MBA_RES_SUCCESS                  Initialize BLE ANCS profile successfully.
 * @retval MBA_RES_FAIL                     Fail to initialize BLE ANCS profile.
 *
 */
uint16_t BLE_ANCS_Init(void);

/**@brief Register BLE ANCS callback.
 *
 * @param[in] bleAncsHandler                Client callback function.
 *
 */
void BLE_ANCS_EventRegister(BLE_ANCS_EventCb_T bleAncsHandler);


/**@brief Handle BLE_Stack events.
 *        This API should be called in the application while caching BLE_Stack events.
 *
 * @param[in] p_stackEvent                  Pointer to BLE_Stack events buffer.
 *
 */
void BLE_ANCS_BleEventHandler(STACK_Event_T *p_stackEvent);

/**@brief Handle BLE_DD (Database Discovery middleware) events.
 *        This API should be called in the application while caching BLE_DD events.
 *
 * @param[in] p_event                       Pointer to BLE_DD events buffer.
 *
 */
void BLE_ANCS_BleDdEventHandler(BLE_DD_Event_T *p_event);

/**@brief This API triggers ANCS client role (Notification Consumer) to retrieve a specific iOS notification attributes from iOS device (Notification Provider).
 *
 * @param[in] connHandle                    The connection handle.
 * @param[in] ntfyId                        Notification UID.  The ntfyId could be retrieved when receiving @ref BLE_ANCS_EVT_NTFY_ADDED_IND or @ref BLE_ANCS_EVT_NTFY_MODIFIED_IND or @ref BLE_ANCS_EVT_NTFY_REMOVED_IND. events.
 * @param[in] bitmask                       Bitmask value of Notification Attribute. Set true to enable retrieving, otherwise set to false. See @ref BLE_ANCS_NtfyAttrsMask_T.
 *
 * @retval MBA_RES_SUCCESS                  Successfully issue a Get Notification Attributes command.
 * @retval MBA_RES_FAIL                     Operation is not permitted.
 * @retval MBA_RES_OOM                      Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA             Invalid parameters. One of the following reasons:\n
 *                                          - Connection handle is not valid\n
 *                                          - Invalid write parameters. See @ref GATTC_WriteParams_T for valid values.\n
 * @retval MBA_RES_NO_RESOURCE              No available resource to send write operation.
 * @retval MBA_RES_BUSY                     GATT Client is busy. Another request is ongoing.
 */
uint16_t BLE_ANCS_GetNtfyAttr(uint16_t connHandle, uint32_t ntfyId, BLE_ANCS_NtfyAttrsMask_T bitmask);

/**@brief This API triggers ANCS client role (Notification Consumer) to retrieve a specific app attributes installed on the iOS device (Notification Provider).
 *
 * @param[in] connHandle                    The connection handle.
 * @param[in] p_appId                       Pointer to App Identifier(string with NULL-terminated) would like to get. App Identifier could be retrieved when receiving @ref BLE_ANCS_EVT_NTFY_ATTR_IND event.
 * @param[in] bitmask                       Bitmask value of App Attribute . Set true to enable retrieving, otherwise set to false.  See @ref BLE_ANCS_AppAttrsMask_T.
 *
 * @retval MBA_RES_SUCCESS                  Successfully issue a Get App Attributes command.
 * @retval MBA_RES_FAIL                     Operation is not permitted.
 * @retval MBA_RES_OOM                      Internal memory allocation failure.
 * @retval MBA_RES_INVALID_PARA             Invalid parameters. One of the following reasons:\n
 *                                          - Connection handle is not valid\n
 *                                          - Invalid write parameters. See @ref GATTC_WriteParams_T for valid values.\n
 * @retval MBA_RES_NO_RESOURCE              No available resource to send write operation.
 * @retval MBA_RES_BUSY                     GATT Client is busy. Another request is ongoing.
 */
uint16_t BLE_ANCS_GetAppAttr(uint16_t connHandle, uint8_t *p_appId, BLE_ANCS_AppAttrsMask_T bitmask);

/**@brief Perform a predetermined action on a specific iOS notification.
 *
 * @param[in] connHandle                    The connection handle.
 * @param[in] ntfyId                        A 32-bit numerical value representing the UID of the iOS notification on which the client wants to perform an action.
 *                                          The ntfyId could be retrieved when receiving @ref BLE_ANCS_EVT_NTFY_ADDED_IND or @ref BLE_ANCS_EVT_NTFY_MODIFIED_IND or @ref BLE_ANCS_EVT_NTFY_REMOVED_IND events.
 * @param[in] actId                         The desired action the NC wants to be performed on the iOS notification. See @ref BLE_ANCS_ActionId_T.
 *
 * @retval MBA_RES_SUCCESS                  Successfully issue a Perform Notification Action command.
 * @retval MBA_RES_FAIL                     Operation is not permitted.
 * @retval MBA_RES_OOM                      No available buffer.
 * @retval MBA_RES_INVALID_PARA             Invalid parameters. One of the following reasons:\n
 *                                          - Connection handle is not valid\n
 *                                          - Invalid write parameters. See @ref GATTC_WriteParams_T for valid values.\n
 * @retval MBA_RES_NO_RESOURCE              No available resource to send write operation.
 * @retval MBA_RES_BUSY                     GATT Client is busy. Another request is ongoing.
 */
uint16_t BLE_ANCS_PerformNtfyAction(uint16_t connHandle, uint32_t ntfyId, BLE_ANCS_ActionId_T actId);
/**@} */ //BLE_ANCS_FUNS


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/** @} */

/**
  @}
 */
