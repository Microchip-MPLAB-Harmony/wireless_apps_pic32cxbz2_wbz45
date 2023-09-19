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
    ble_ancs.c

  Summary:
    This file contains the BLE ANCS functions for application user.

  Description:
    This file contains the BLE ANCS functions for application user.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "osal/osal_freertos.h"
#include "mba_error_defs.h"
#include "ble_gap.h"
#include "gatt.h"
#include "ble_util/mw_assert.h"
#include "ble_util/byte_stream.h"
#include "ble_ancs.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define BLE_ANCS_NTFY_DATA_LENGTH               (8U)
#define BLE_ANCS_NTFY_EVT_ID_INDEX              (0U)           /**< Index of the Event ID field when parsing notifications. */
#define BLE_ANCS_NTFY_FLAGS_INDEX               (1U)           /**< Index of the Flags field when parsing notifications. */
#define BLE_ANCS_NTFY_CATEGORY_ID_INDEX         (2U)           /**< Index of the Category ID field when parsing notifications. */
#define BLE_ANCS_NTFY_CATEGORY_CNT_INDEX        (3U)           /**< Index of the Category Count field when parsing notifications. */
#define BLE_ANCS_NTFY_UID_INDEX                 (4U)           /**< Index of the Notification UID field when parsing notifications. */

#define UUID_ANCS_SVC                           0xD0,0x00,0x2D,0x12,0x1E,0x4B,0x0F,0xA4,0x99,0x4E,0xCE,0xB5,0x31,0xF4,0x05,0x79         /**< Definition of ANCS proprietary service UUID in little endian. */
#define UUID_ANCS_CHAR_NOTIFICATION_SOURCE      0xBD,0x1D,0xA2,0x99,0xE6,0x25,0x58,0x8C,0xD9,0x42,0x01,0x63,0x0D,0x12,0xBF,0x9F         /**< Definition of ANCS Notification Source characteristic UUID in little endian. */
#define UUID_ANCS_CHAR_CONTROL_POINT            0xD9,0xD9,0xAA,0xFD,0xBD,0x9B,0x21,0x98,0xA8,0x49,0xE1,0x45,0xF3,0xD8,0xD1,0x69         /**< Definition of ANCS Control Point characteristic UUID in little endian. */
#define UUID_ANCS_CHAR_DATA_SOURCE              0xFB,0x7B,0x7C,0xCE,0x6A,0xB3,0x44,0xBE,0xB5,0x4B,0xD6,0x24,0xE9,0xC6,0xEA,0x22         /**< Definition of ANCS Data Source characteristic UUID in little endian. */

#define BLE_ANCS_MAX_CONN_NBR                   BLE_GAP_MAX_LINK_NBR

#define BLE_ANCS_EVENT_FLAG_SILENT              0           //!< 0b.......1 Silent: First (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_IMPORTANT           1           //!< 0b......1. Important: Second (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_PREEXISTING         2           //!< 0b.....1.. Pre-existing: Third (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_POSITIVE_ACTION     3           //!< 0b....1... Positive action: Fourth (LSB) bit is set. All flags can be active at the same time.
#define BLE_ANCS_EVENT_FLAG_NEGATIVE_ACTION     4           //!< 0b...1.... Negative action: Fifth (LSB) bit is set. All flags can be active at the same time.

/**@defgroup BLE_ANCS_RETRY_TYPE Retrying type
 * @brief The definition of BLE apple notification client service retry type
 * @{ */
#define BLE_ANCS_RETRY_TYPE_ENABLE_NTFY        0x01    /**< Definition of response retry type enable notification. */
/** @} */

/**@brief Enumeration type of BLE ANCS profile characteristics. */
typedef enum BLE_ANCS_CharIndex_T
{
    ANCS_INDEX_CHAR_CONTROL_POINT = 0,
    ANCS_INDEX_CHAR_NTFY,
    ANCS_INDEX_CHAR_NTFY_CCCD,
    ANCS_INDEX_CHAR_DATA,
    ANCS_INDEX_CHAR_DATA_CCCD,
    ANCS_INDEX_CHAR_MAX_NUM
}BLE_ANCS_CharIndex_T;
 
/**@brief Event for iOS Notification. */
typedef enum BLE_ANCS_NtfyEvt_T
{
    BLE_ANCS_NTFY_EVT_ADDED,                                /**< The iOS notification was added. */
    BLE_ANCS_NTFY_EVT_MODIFIED,                             /**< The iOS notification was modified. */
    BLE_ANCS_NTFY_EVT_REMOVED,                              /**< The iOS notification was removed. */
    BLE_ANCS_NTFY_EVT_MAX                                   /**< Undefined notification event. */
} BLE_ANCS_NtfyEvt_T;


/**@brief AppAttributeID for iOS Get App Attributes. */
typedef enum BLE_ANCS_AppAttrId_T
{
    BLE_ANCS_APP_ATTR_ID_DISPLAY_NAME = 0,                  /**< Command used to get the display name for an app identifier. */
    BLE_ANCS_APP_ATTR_ID_MAX                                /**< Undefined application attribute id. */
} BLE_ANCS_AppAttrId_T;


typedef struct
{
    uint8_t                     attrId;                     //!< Attribute ID: AppIdentifier(0), Title(1), Subtitle(2), Message(3), MessageSize(4), Date(5), PositiveActionLabel(6), NegativeActionLabel(7).
    uint16_t                    attrLen;                    //!< Length of the attribute. If more data is received from the Notification Provider, all the data beyond this length is discarded.
    uint8_t                     *p_attrData;                //!< Pointer to where the memory is allocated for storing incoming attributes.
} BLE_ANCS_AttrList_T;

typedef enum BLE_ANCS_PacketOrder_T
{
    BLE_ANCS_FIRST_PACKET = 0,
    BLE_ANCS_MID_PACKET,
    BLE_ANCS_LAST_PACKET
} BLE_ANCS_PacketOrder_T;

typedef enum BLE_ANCS_CmdId_T
{
    BLE_ANCS_COMMAND_ID_GET_NTFY_ATTR = 0x00U,              /**< Requests attributes to be sent from the NP to the NC for a given notification. */
    BLE_ANCS_COMMAND_ID_GET_APP_ATTR,                       /**< Requests attributes to be sent from the NP to the NC for a given iOS app. */
    BLE_ANCS_COMMAND_ID_SET_PERFORM_NTFY_ACTION,            /**< Requests an action to be performed on a given notification. For example, dismiss an alarm. */
} BLE_ANCS_CmdId_T;

typedef enum BLE_ANCS_State_T
{
    BLE_ANCS_STATE_IDLE = 0x00,
    BLE_ANCS_STATE_CONNECTED
} BLE_ANCS_State_T;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef struct BLE_ANCS_DecodeAttrs_T
{
    BLE_ANCS_DecodeNtfyAttrs_T  attrNtfyData;               /**< Decode Data @ref BLE_ANCS_DecodeNtfyAttrs_T. */
    BLE_ANCS_DecodeAppAttrs_T   attrAppData;                /**< Decode Data @ref BLE_ANCS_DecodeAppAttrs_T. */
} BLE_ANCS_DecodeAttrs_T;

typedef struct BLE_ANCS_ConnList_T
{
    uint8_t                     connIndex;
    BLE_ANCS_State_T            state;
    uint16_t                    connHandle;
    uint16_t                    attMtu;

    BLE_ANCS_CharIndex_T        enableCccd;

    uint8_t                     *p_packet;
    BLE_ANCS_PacketOrder_T      completePkt;
    uint16_t                    pktLen;

    BLE_ANCS_DecodeAttrs_T      attrData;

    BLE_ANCS_AttrList_T         ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_MAX];
    BLE_ANCS_AttrList_T         appAttrList[BLE_ANCS_APP_ATTR_ID_MAX];
    BLE_ANCS_AttrList_T         *p_attrList;
    uint8_t                     retryType;
} BLE_ANCS_ConnList_T;

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static BLE_ANCS_EventCb_T       bleAncsProcess;
static BLE_ANCS_ConnList_T      s_ancsConnList[BLE_ANCS_MAX_CONN_NBR];
static BLE_DD_CharInfo_T        s_ancsCharInfoList[BLE_ANCS_MAX_CONN_NBR][ANCS_INDEX_CHAR_MAX_NUM];

static const uint8_t            discSvcUuid[] =     {UUID_ANCS_SVC};
static const ATT_Uuid_T         discCharCtrl =      {{UUID_ANCS_CHAR_CONTROL_POINT}, ATT_UUID_LENGTH_16 };
static const ATT_Uuid_T         discCharNtfy =      {{UUID_ANCS_CHAR_NOTIFICATION_SOURCE}, ATT_UUID_LENGTH_16 };
static const ATT_Uuid_T         discCharNtfyCccd =  {{UINT16_TO_BYTES(UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG)}, ATT_UUID_LENGTH_2 };
static const ATT_Uuid_T         discCharData =      {{UUID_ANCS_CHAR_DATA_SOURCE}, ATT_UUID_LENGTH_16 };
static const ATT_Uuid_T         discCharDataCccd =  {{UINT16_TO_BYTES(UUID_DESCRIPTOR_CLIENT_CHAR_CONFIG)}, ATT_UUID_LENGTH_2 };

static BLE_DD_DiscChar_T        ancsCtrl =          {&discCharCtrl, 0 };
static BLE_DD_DiscChar_T        ancsNtfy =          {&discCharNtfy,0 };
static BLE_DD_DiscChar_T        ancsNtfyCccd =      {&discCharNtfyCccd, CHAR_SET_DESCRIPTOR };
static BLE_DD_DiscChar_T        ancsData =          {&discCharData, 0 };
static BLE_DD_DiscChar_T        ancsDataCccd =      {&discCharDataCccd, CHAR_SET_DESCRIPTOR };

static BLE_DD_DiscChar_T        *ancsDiscCharList[] =
{
    &ancsCtrl,
    &ancsNtfy,
    &ancsNtfyCccd,
    &ancsData,
    &ancsDataCccd
};

static BLE_DD_CharList_T        s_ancsCharList[BLE_ANCS_MAX_CONN_NBR];

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

static void ble_ancs_ConveyEvent(uint8_t eventId, uint8_t *p_eventField, uint8_t eventFieldLen)
{
    if(bleAncsProcess != NULL)
    {
        BLE_ANCS_Event_T evtPara;

        evtPara.eventId = eventId;
        (void)memcpy((uint8_t *)&evtPara.eventField, p_eventField, eventFieldLen);
        bleAncsProcess(&evtPara);
    }
}

static uint16_t ble_ancs_GattWrite(BLE_ANCS_ConnList_T *p_conn, GATTC_WriteParams_T *p_writeParams, 
                            uint16_t charHandle, uint16_t len)
{
    uint16_t ret;

    p_writeParams->charHandle = charHandle;
    p_writeParams->charLength = len;
    p_writeParams->writeType  = ATT_WRITE_REQ;
    p_writeParams->valueOffset  = 0;
    p_writeParams->flags = 0;

    ret= GATTC_Write(p_conn->connHandle, p_writeParams);

    return ret;
}

static void ble_ancs_InitResource(BLE_ANCS_ConnList_T *p_conn)
{
    BLE_ANCS_DecodeNtfyAttrs_T  *p_attrNtfyData= &p_conn->attrData.attrNtfyData;
    
    if (p_conn->p_packet!= NULL )
    {
        OSAL_Free(p_conn->p_packet);
        p_conn->p_packet    = NULL;
    }

    p_conn->completePkt = BLE_ANCS_FIRST_PACKET;
    p_conn->pktLen      = 0;
    
    if (p_attrNtfyData->p_title != NULL)
    {
        OSAL_Free(p_attrNtfyData->p_title);
        p_attrNtfyData->p_title = NULL;
    }
    
    if (p_attrNtfyData->p_subtitle != NULL)
    {
        OSAL_Free(p_attrNtfyData->p_subtitle);
        p_attrNtfyData->p_subtitle = NULL;
    }
    
    if (p_attrNtfyData->p_msg != NULL)
    {
        OSAL_Free(p_attrNtfyData->p_msg);
        p_attrNtfyData->p_msg = NULL;
    }
}

static bool ble_ancs_VerifyNotification(BLE_ANCS_EvtNtfyInd_T *p_ntfy, BLE_ANCS_NtfyEvt_T ntfyEvtId)
{
    if (  (ntfyEvtId >= BLE_ANCS_NTFY_EVT_MAX)
       || (p_ntfy->categoryId >= BLE_ANCS_CATEGORY_ID_MAX))
    {
        return true;
    }

    return false;
}
 
static void ble_ancs_NotificationRsp(BLE_ANCS_ConnList_T *p_conn, GATT_EvtReceiveHandleValue_T *p_event)
{
    BLE_ANCS_EvtNtfyInd_T *p_ntfy;
    BLE_ANCS_Event_T evtPara;
    uint8_t *p_data  = p_event->receivedValue, ntfyEvtId;
    uint8_t eventId[] = {BLE_ANCS_EVT_NTFY_ADDED_IND, BLE_ANCS_EVT_NTFY_MODIFIED_IND, BLE_ANCS_EVT_NTFY_REMOVED_IND};
    uint16_t  hvx_data_len= p_event->receivedLength;

    if (hvx_data_len != BLE_ANCS_NTFY_DATA_LENGTH)
    {
        ble_ancs_ConveyEvent(BLE_ANCS_EVT_ERR_UNSPECIFIED_IND, NULL, 0);
        return;
    }

    p_ntfy = &evtPara.eventField.evtNtfyInd;

    ntfyEvtId  = p_data[BLE_ANCS_NTFY_EVT_ID_INDEX];

    p_ntfy->ntfyEvtFlagMask.silent          =
            (p_data[BLE_ANCS_NTFY_FLAGS_INDEX] >> BLE_ANCS_EVENT_FLAG_SILENT) & 0x01U;

    p_ntfy->ntfyEvtFlagMask.important       =
            (p_data[BLE_ANCS_NTFY_FLAGS_INDEX] >> BLE_ANCS_EVENT_FLAG_IMPORTANT) & 0x01U;

    p_ntfy->ntfyEvtFlagMask.preExisting    =
            (p_data[BLE_ANCS_NTFY_FLAGS_INDEX] >> BLE_ANCS_EVENT_FLAG_PREEXISTING) & 0x01U;

    p_ntfy->ntfyEvtFlagMask.positiveAction =
            (p_data[BLE_ANCS_NTFY_FLAGS_INDEX] >> BLE_ANCS_EVENT_FLAG_POSITIVE_ACTION) & 0x01U;

    p_ntfy->ntfyEvtFlagMask.negativeAction =
            (p_data[BLE_ANCS_NTFY_FLAGS_INDEX] >> BLE_ANCS_EVENT_FLAG_NEGATIVE_ACTION) & 0x01u;

    p_ntfy->categoryId               =
        (BLE_ANCS_CategoryId_T) p_data[BLE_ANCS_NTFY_CATEGORY_ID_INDEX];

    p_ntfy->categoryCount            = p_data[BLE_ANCS_NTFY_CATEGORY_CNT_INDEX];
    (void)memcpy((uint8_t*)&p_ntfy->ntfyId, (uint8_t*)&p_data[BLE_ANCS_NTFY_UID_INDEX], 4);

    if (ble_ancs_VerifyNotification(p_ntfy, ntfyEvtId)==true)
    {
        ble_ancs_ConveyEvent(BLE_ANCS_EVT_ERR_UNSPECIFIED_IND, NULL, 0);
        return;
    }

    evtPara.eventField.evtNtfyInd.connHandle  = p_conn->connHandle;
    ble_ancs_ConveyEvent(eventId[ntfyEvtId], (uint8_t *)&evtPara.eventField, sizeof(BLE_ANCS_EvtNtfyInd_T));
}

static BLE_ANCS_ConnList_T *ble_ancs_GetConnListByHandle(uint16_t connHandle)
{
    uint8_t i;
    BLE_ANCS_ConnList_T *p_conn = NULL;

    for(i=0; i<BLE_ANCS_MAX_CONN_NBR; i++)
    {
        if ((s_ancsConnList[i].state == BLE_ANCS_STATE_CONNECTED) && (s_ancsConnList[i].connHandle == connHandle))
        {
            p_conn = &s_ancsConnList[i];
            break;
        }
    }

    return p_conn;
}
 
static BLE_ANCS_PacketOrder_T  ble_ancs_ChkAncsComplete(BLE_ANCS_ConnList_T *p_conn)
{
    BLE_ANCS_PacketOrder_T ret;
    uint8_t *p_data = p_conn->p_packet;
    uint16_t len = p_conn->pktLen, index =0;
    BLE_ANCS_CmdId_T  commandId = (BLE_ANCS_CmdId_T)p_data[0];

    index++;        //CommandID

    if (commandId == BLE_ANCS_COMMAND_ID_GET_NTFY_ATTR)
    {
        index += 4U; //NotificationUID
    }
    else
    {
        index += strlen((char *)(p_data +1));
        index ++;   //'\0'
    }


    while(1)
    {
        uint16_t attrLen;

        index++;    //AttributeID
        attrLen = p_data[index++];
        attrLen |= (p_data[index++] << 8);

        if (index + attrLen == len)
        {
            ret = BLE_ANCS_LAST_PACKET;
            break;
        }
        else
        {
            if (index + attrLen < len)
            {
                index += attrLen;  
            }
            else   
            {      
                ret = BLE_ANCS_MID_PACKET; 
                break; 
            }
        }
    }

    p_conn->completePkt = ret;

    return ret;
}

static uint16_t ble_ancs_AttrsRsp(BLE_ANCS_ConnList_T *p_conn)
{
    uint8_t * p_data        = p_conn->p_packet;
    BLE_ANCS_DecodeNtfyAttrs_T  *p_attrNtfyData= &p_conn->attrData.attrNtfyData;
    BLE_ANCS_DecodeAppAttrs_T   *p_attrAppData = &p_conn->attrData.attrAppData;

    uint16_t  hvx_data_len  = p_conn->pktLen, index =0;
    BLE_ANCS_CmdId_T  commandId  = (BLE_ANCS_CmdId_T) p_data[index++];
    uint16_t maxLen[BLE_ANCS_COMMAND_ID_GET_APP_ATTR +1][BLE_ANCS_NTFY_ATTR_ID_MAX] ={
        { BLE_ANCS_MAX_APPID_LEN,   BLE_ANCS_MAX_TITLE_LEN, BLE_ANCS_MAX_SUB_TITLE_LEN,
          BLE_ANCS_MAX_MESSAGE_LEN, BLE_ANCS_MAX_MESSAGE_SIZE_LEN, BLE_ANCS_MAX_DATE_LEN,
          BLE_ANCS_MAX_POS_ACTION_LABEL_LEN, BLE_ANCS_MAX_NEG_ACTION_LABEL_LEN
        },
        { BLE_ANCS_MAX_APP_NAME_LEN,0,0,0,0,0,0,0}
    };

    if (commandId == BLE_ANCS_COMMAND_ID_GET_NTFY_ATTR)
    {
        p_conn->p_attrList    = p_conn->ntfyAttrList;
        (void)memcpy((uint8_t*)&p_attrNtfyData->ntfyId , (uint8_t*)&p_data[index], 4);
        index += 4U;
    }
    else
    {
        p_conn->p_attrList    = p_conn->appAttrList;
        (void)strcpy((char *)p_attrAppData->appId , (char *)(p_data +index));
        index += (strlen((char *)p_attrAppData->appId) +1U);
    }

    do
    {
        uint16_t len, bufferFail, normalAttrID;
        BLE_ANCS_NtfyAttrId_T attrId;

        attrId  = p_data[index++];
        //the len does not include '\0' by ANCS spec
        len     = p_data[index++];
        len     |= (p_data[index++] << 8);

        bufferFail      = 0;
        normalAttrID    = 1;

        if (commandId == BLE_ANCS_COMMAND_ID_GET_NTFY_ATTR)
        {
            if ((attrId == BLE_ANCS_NTFY_ATTR_ID_TITLE)    ||
                (attrId == BLE_ANCS_NTFY_ATTR_ID_SUBTITLE) ||
                (attrId == BLE_ANCS_NTFY_ATTR_ID_MESSAGE))
            {
                uint8_t *p_tmp;

                p_tmp = OSAL_Malloc(maxLen[commandId][attrId] +1U);  //reserved '\0'
                if (p_tmp == NULL)
                {
                   bufferFail = 1;
                }
                else
                {
                    if (attrId == BLE_ANCS_NTFY_ATTR_ID_TITLE)
                    {
                        p_attrNtfyData->p_title      = p_tmp;
                    }
                    else if (attrId == BLE_ANCS_NTFY_ATTR_ID_SUBTITLE)
                    {
                        p_attrNtfyData->p_subtitle   = p_tmp;
                    }
                    else 
                    {
                        p_attrNtfyData->p_msg        = p_tmp;
                    }
                      
                    p_conn->p_attrList[attrId].p_attrData   = p_tmp; 
                }

                normalAttrID = 0;
            }
        }
        
        if (normalAttrID != 0U)
        {            
            if (len >= maxLen[commandId][attrId])
            {
                bufferFail = 1;
            }
        }
         
        if (bufferFail == 0U)
        {
            if (attrId != BLE_ANCS_NTFY_ATTR_ID_MESSAGE_SIZE)
            {
                (void)memcpy(p_conn->p_attrList[attrId].p_attrData, p_data +index, len);
                p_conn->p_attrList[attrId].p_attrData[len] = 0;     //'\0'
            }
            else
            {
                uint8_t str[8];

                (void)memcpy(str, p_data +index, len);
                str[len] = 0;                                       //'\0'

                p_attrNtfyData->msgSize = atoi((char *)str);
            }

            if (commandId == BLE_ANCS_COMMAND_ID_GET_NTFY_ATTR)
            {
                uint8_t *p_bitId = (uint8_t *)&p_attrNtfyData->bitmask;

                *p_bitId |= (1U << attrId);
            }
        }
        else
        {
            BLE_ANCS_Event_T evtPara;

            evtPara.eventField.evtErrAttrBufInd.connHandle  = p_conn->connHandle;
            evtPara.eventField.evtErrAttrBufInd.attrId      = attrId;
            evtPara.eventField.evtErrAttrBufInd.len         = len;
            ble_ancs_ConveyEvent(BLE_ANCS_EVT_ERR_ATTR_BUF_IND, (uint8_t *)&evtPara.eventField, sizeof(BLE_ANCS_EvtErrAttrBufInd_T));
        }

        index += len;

    } while (index <hvx_data_len);

    return MBA_RES_SUCCESS;
}

static uint16_t ble_ancs_QueryNtfyAttr(BLE_ANCS_ConnList_T *p_conn, uint8_t *p_gattValue, uint32_t uid, BLE_ANCS_NtfyAttrsMask_T enabeMask)
{
    uint16_t index  = 0;
    uint8_t bitmask = *(uint8_t *)&enabeMask;
    uint16_t maxEncodeLen[] = {0, BLE_ANCS_MAX_TITLE_LEN, BLE_ANCS_MAX_SUB_TITLE_LEN, BLE_ANCS_MAX_MESSAGE_LEN};

    if (bitmask != 0U)
    {
        BLE_ANCS_NtfyAttrId_T   attr;
        //Encode Command ID.
        p_gattValue[index++] = BLE_ANCS_COMMAND_ID_GET_NTFY_ATTR;

        //Encode Notification UID.
        (void)memcpy((uint8_t*)&p_gattValue[index], (uint8_t*)&uid, 4);
        index += 4U;

        //Encode Attribute ID.
        for (attr = BLE_ANCS_NTFY_ATTR_ID_APP_IDENTIFIER; attr < BLE_ANCS_NTFY_ATTR_ID_MAX; attr++)
        {
            if (bitmask & (1U <<attr))
            {
                p_gattValue[index++] = (uint8_t)attr;

                if ((attr == BLE_ANCS_NTFY_ATTR_ID_TITLE) ||
                    (attr == BLE_ANCS_NTFY_ATTR_ID_SUBTITLE) ||
                    (attr == BLE_ANCS_NTFY_ATTR_ID_MESSAGE))
                {
                    //Encode Length field. Only applicable for Title, Subtitle, and Message.
                    (void)memcpy((uint8_t*)&p_gattValue[index], (uint8_t*)&maxEncodeLen[attr], 2);

                    index += 2U;
                }
            }
        }
    }
    return index ;
}

static uint16_t ble_ancs_EncodeNotifAction(BLE_ANCS_ConnList_T *p_conn, uint8_t * p_encodedData, uint32_t ntfyId, BLE_ANCS_ActionId_T actId)
{
    uint8_t index = 0;

    p_encodedData[index++] = BLE_ANCS_COMMAND_ID_SET_PERFORM_NTFY_ACTION;
    (void)memcpy((uint8_t*)&p_encodedData[index], (uint8_t*)&ntfyId, 4);
    index += 4U;
    p_encodedData[index++] = (uint8_t)actId;

    return index;
}


uint16_t BLE_ANCS_GetNtfyAttr(uint16_t connHandle, uint32_t ntfyId, BLE_ANCS_NtfyAttrsMask_T bitmask)
{
    uint16_t ret = MBA_RES_INVALID_PARA, len;
    BLE_ANCS_ConnList_T *p_conn;

    p_conn = ble_ancs_GetConnListByHandle(connHandle);
    if (p_conn != NULL)
    {       
        GATTC_WriteParams_T *p_writeParams;
        uint16_t charHandle = s_ancsCharInfoList[p_conn->connIndex][ANCS_INDEX_CHAR_CONTROL_POINT].charHandle;

        p_writeParams = OSAL_Malloc(sizeof(GATTC_WriteParams_T));
        if (p_writeParams == NULL)
        {
            return MBA_RES_OOM;
        }
      
        len = ble_ancs_QueryNtfyAttr(p_conn, p_writeParams->charValue, ntfyId, bitmask);
             
        if (len!= 0U)
        {
            ret = ble_ancs_GattWrite(p_conn, p_writeParams, charHandle, len); 
        }
    
        OSAL_Free(p_writeParams);                                
    }
 
    return ret;
}
 
static uint16_t ble_ancs_QueryAppAttr(BLE_ANCS_ConnList_T *p_conn, uint8_t *p_appId, uint8_t *p_value)
{
    uint32_t  index = 0;

    // Encode Command ID.
    p_value[index++] = BLE_ANCS_COMMAND_ID_GET_APP_ATTR;

    //Encode app identifier.
    (void)strcpy((char *)(p_value +index) , (char *)p_appId);
    index += (strlen((char *)p_appId) +1U);

    //Encode Attribute ID.
    p_value[index++] = BLE_ANCS_APP_ATTR_ID_DISPLAY_NAME;

    return index;
}

uint16_t BLE_ANCS_GetAppAttr(uint16_t connHandle, uint8_t *p_appId, BLE_ANCS_AppAttrsMask_T bitmask)
{
    uint16_t ret = MBA_RES_INVALID_PARA, index;
    BLE_ANCS_ConnList_T *p_conn;

    if (!bitmask.displayName)
    {
        return MBA_RES_INVALID_PARA;
    }
            
    p_conn = ble_ancs_GetConnListByHandle(connHandle);
    if (p_conn != NULL)
    { 
        GATTC_WriteParams_T *p_writeParams;
        uint16_t charHandle = s_ancsCharInfoList[p_conn->connIndex][ANCS_INDEX_CHAR_CONTROL_POINT].charHandle;

        p_writeParams = OSAL_Malloc(sizeof(GATTC_WriteParams_T));
        if (p_writeParams == NULL)
        {
            return MBA_RES_OOM;   
        }
        
        index = ble_ancs_QueryAppAttr(p_conn, p_appId, p_writeParams->charValue);
        
        if (index != 0U)
        {
            ret = ble_ancs_GattWrite(p_conn, p_writeParams, charHandle, index);
        }
         
        OSAL_Free(p_writeParams);
    }

    return ret;
}

uint16_t BLE_ANCS_PerformNtfyAction(uint16_t connHandle, uint32_t ntfyId, BLE_ANCS_ActionId_T actId)
{
    uint16_t len, ret= MBA_RES_INVALID_PARA;
    BLE_ANCS_ConnList_T *p_conn;

    if (actId < BLE_ANCS_ACTION_ID_MAX)
    {
        p_conn = ble_ancs_GetConnListByHandle(connHandle);
        if (p_conn != NULL)
        {
            GATTC_WriteParams_T *p_writeParams;
            uint16_t charHandle = s_ancsCharInfoList[p_conn->connIndex][ANCS_INDEX_CHAR_CONTROL_POINT].charHandle;

            p_writeParams = OSAL_Malloc(sizeof(GATTC_WriteParams_T));
            if (p_writeParams == NULL)
            {
                return MBA_RES_OOM;
            }
            
            len = ble_ancs_EncodeNotifAction(p_conn, p_writeParams->charValue, ntfyId, actId);

            ret = ble_ancs_GattWrite(p_conn, p_writeParams, charHandle, len);

            OSAL_Free(p_writeParams);
        }
    }

    return ret;
}

static void ble_ancs_ProcErr(GATT_Event_T *p_event)
{
    BLE_ANCS_Event_T evtPara;
    evtPara.eventField.evtErrInd.connHandle  = p_event->eventField.onError.connHandle;
    evtPara.eventField.evtErrInd.errCode     = p_event->eventField.onError.errCode;
    ble_ancs_ConveyEvent(BLE_ANCS_EVT_ERR_IND, (uint8_t *)&evtPara.eventField, sizeof(BLE_ANCS_EvtErrInd_T));
}
static void ble_ancs_ProcGattNotification(BLE_ANCS_ConnList_T *p_conn, GATT_Event_T *p_event)
{
    uint16_t ret = MBA_RES_SUCCESS;
    uint16_t charHandle = p_event->eventField.onNotification.charHandle;
    BLE_ANCS_Event_T evtPara;

    if (charHandle == s_ancsCharInfoList[p_conn->connIndex][ANCS_INDEX_CHAR_NTFY].charHandle)
    {
        ble_ancs_NotificationRsp(p_conn, &p_event->eventField.onNotification);
    }
    else if (charHandle == s_ancsCharInfoList[p_conn->connIndex][ANCS_INDEX_CHAR_DATA].charHandle)
    {
        BLE_ANCS_DecodeNtfyAttrs_T  *p_attrNtfyData= &p_conn->attrData.attrNtfyData;
        uint8_t * p_data   = p_event->eventField.onNotification.receivedValue;
        uint16_t  dataLen  = p_event->eventField.onNotification.receivedLength;

        if (p_conn->completePkt == BLE_ANCS_FIRST_PACKET)
        {
            *(uint8_t *)&p_attrNtfyData->bitmask = 0;

            p_conn->p_packet = OSAL_Malloc(BLE_ANCS_MAX_PACKET_BUFFER_SIZE);
            if (p_conn->p_packet == NULL)
			{
               return;
			}
        }
        else
        {
            if (p_conn->pktLen +dataLen > BLE_ANCS_MAX_PACKET_BUFFER_SIZE)
            {
            /*
                Check if BLE_ANCS_MAX_PACKET_BUFFER_SIZE is suitable
            */
                BLE_ANCS_Event_T evtPara;

                ble_ancs_InitResource(p_conn);
                evtPara.eventField.evtErrRecomposeBufInd.connHandle  = p_conn->connHandle;
                ble_ancs_ConveyEvent(BLE_ANCS_EVT_ERR_RECOMPOSE_BUF_IND, (uint8_t *)(uint8_t *)&evtPara.eventField, sizeof(BLE_ANCS_EvtErrRecomposeBufInd_T));

                return;
            }
        }
        (void)memcpy(p_conn->p_packet +p_conn->pktLen , p_data, dataLen);
        p_conn->pktLen  += dataLen;

        /*
            Recompose packet if more notification
            Check if miss any notification, so it can not be recomposed successfully.
        */
        if (ble_ancs_ChkAncsComplete(p_conn) == BLE_ANCS_LAST_PACKET)
        {
            BLE_ANCS_CmdId_T  commandId = (BLE_ANCS_CmdId_T) p_conn->p_packet[0];

            ret = ble_ancs_AttrsRsp(p_conn);
            if (ret == MBA_RES_SUCCESS)
            {
                if (commandId == BLE_ANCS_COMMAND_ID_GET_NTFY_ATTR)
                {
                    evtPara.eventField.evtNtfyAttrInd.connHandle  = p_conn->connHandle;

                    evtPara.eventField.evtNtfyAttrInd.p_data = &p_conn->attrData.attrNtfyData;
                    ble_ancs_ConveyEvent(BLE_ANCS_EVT_NTFY_ATTR_IND, (uint8_t *)&evtPara.eventField, sizeof(BLE_ANCS_EvtNtfyAttrInd_T));
                }
                else
                {
                    evtPara.eventField.evtAppAttrInd.connHandle     = p_conn->connHandle;

                    evtPara.eventField.evtAppAttrInd.p_data = &p_conn->attrData.attrAppData;
                    ble_ancs_ConveyEvent(BLE_ANCS_EVT_APP_ATTR_IND, (uint8_t *)&evtPara.eventField, sizeof(BLE_ANCS_EvtAppAttrInd_T));
                }
            }
            ble_ancs_InitResource(p_conn);
        }
    }
    else
    {
        //Shall not enter here
    }
}
static void ble_ancs_enableCccd(uint16_t connHandle)
{
    BLE_ANCS_ConnList_T *p_conn = ble_ancs_GetConnListByHandle(connHandle);
    uint16_t status;

    if (p_conn == NULL)
    {
        return;
    }

    switch (p_conn->enableCccd)
    {
        case ANCS_INDEX_CHAR_NTFY_CCCD:
        case ANCS_INDEX_CHAR_DATA_CCCD:
        {
            GATTC_WriteParams_T *p_writeParams = OSAL_Malloc(sizeof(GATTC_WriteParams_T));
            if (p_writeParams == NULL)
            {
                ble_ancs_ConveyEvent(BLE_ANCS_EVT_ERR_NO_MEM_IND, NULL, 0);
                return;
            }
            p_writeParams->charHandle   = s_ancsCharInfoList[p_conn->connIndex][p_conn->enableCccd].charHandle;
            U16_TO_BUF_LE(p_writeParams->charValue, NOTIFICATION);
            p_writeParams->charLength   = 2;
            p_writeParams->writeType    = ATT_WRITE_REQ;
            p_writeParams->valueOffset  = 0x0000;
            p_writeParams->flags        = 0;

            status = GATTC_Write(connHandle, p_writeParams);
            if (status == MBA_RES_SUCCESS)
            {
                if (p_conn->enableCccd == ANCS_INDEX_CHAR_DATA_CCCD)
                {
                    p_conn->enableCccd = ANCS_INDEX_CHAR_NTFY_CCCD;
                }
                else
                {
                    p_conn->enableCccd = 0;
                }
                p_conn->retryType = 0;
            }
            else
            {
                p_conn->retryType = BLE_ANCS_RETRY_TYPE_ENABLE_NTFY;
            }
            OSAL_Free(p_writeParams);
        }
        break;
        default:
            break;
    }
}

static void ble_ancs_ProcessQueuedTask(uint16_t connHandle)
{
    BLE_ANCS_ConnList_T *p_conn;

    p_conn = ble_ancs_GetConnListByHandle(connHandle);
    if (p_conn == NULL)
    {
        return;
    }
    switch (p_conn->retryType)
    {
        case BLE_ANCS_RETRY_TYPE_ENABLE_NTFY:
        {
            ble_ancs_enableCccd(connHandle);
        }
        break;
        default:
        break;
    }
}

static void ble_ancs_GattEventProcess(GATT_Event_T *p_event)
{
    BLE_ANCS_ConnList_T *p_conn;
    
    switch (p_event->eventId)
    {
        case ATT_EVT_UPDATE_MTU:
        {
            p_conn = ble_ancs_GetConnListByHandle(p_event->eventField.onUpdateMTU.connHandle);
            if (p_conn != NULL)
            {
                p_conn->attMtu =  p_event->eventField.onUpdateMTU.exchangedMTU;
            }
        }
        break;

        case GATTC_EVT_HV_NOTIFY:
        {
            p_conn = ble_ancs_GetConnListByHandle(p_event->eventField.onNotification.connHandle);
            if (p_conn != NULL)
            {
                ble_ancs_ProcGattNotification(p_conn, p_event);
            }
        }
        break;

        case GATTC_EVT_PROTOCOL_AVAILABLE:
        {
            p_conn = ble_ancs_GetConnListByHandle(p_event->eventField.onUpdateMTU.connHandle);
            if (p_conn == NULL)
			{
                return;
			}

            if ((p_conn->enableCccd == ANCS_INDEX_CHAR_NTFY_CCCD) || (p_conn->enableCccd == ANCS_INDEX_CHAR_DATA_CCCD))
            {
                ble_ancs_enableCccd(p_event->eventField.onClientProtocolAvailable.connHandle);
            }
        }
        break;
        case GATTC_EVT_ERROR_RESP:
        {
            p_conn = ble_ancs_GetConnListByHandle(p_event->eventField.onError.connHandle);
            if (p_conn != NULL)
            {
                ble_ancs_ProcErr(p_event);
            }

        } 
        break;
        default:
            break;
    }
}

static BLE_ANCS_ConnList_T *ble_ancs_GetFreeConnList(void)
{
    uint8_t i;
    BLE_ANCS_ConnList_T *p_conn = NULL;

    for(i=0; i<BLE_ANCS_MAX_CONN_NBR; i++)
    {
        if (s_ancsConnList[i].state == BLE_ANCS_STATE_IDLE)
        {
            s_ancsConnList[i].state = BLE_ANCS_STATE_CONNECTED;
            s_ancsConnList[i].connIndex = i;
            p_conn = &s_ancsConnList[i];

            break;
        }
    }

    return p_conn;
}

static void ble_ancs_InitConnList(BLE_ANCS_ConnList_T *p_conn, uint8_t disconnect)
{
    BLE_ANCS_DecodeNtfyAttrs_T  *p_attrNtfyData = &p_conn->attrData.attrNtfyData;
    BLE_ANCS_DecodeAppAttrs_T   *p_attrAppData= &p_conn->attrData.attrAppData;
    
    if (disconnect != 0U)
    {
        ble_ancs_InitResource(p_conn);
    }

    (void)memset((uint8_t *)p_conn, 0, sizeof(BLE_ANCS_ConnList_T));
    p_conn->attMtu= BLE_ATT_DEFAULT_MTU_LEN;

    p_conn->ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_APP_IDENTIFIER].p_attrData 
                = p_attrNtfyData->appId;
    p_conn->ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_TITLE].p_attrData 
                = p_attrNtfyData->p_title;
    p_conn->ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_SUBTITLE].p_attrData 
                = p_attrNtfyData->p_subtitle;
    p_conn->ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_MESSAGE].p_attrData 
                = p_attrNtfyData->p_msg;
    p_conn->ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_DATE].p_attrData 
                = p_attrNtfyData->date;
    p_conn->ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_POSITIVE_ACTION_LABEL].p_attrData 
                = p_attrNtfyData->positiveAction;
    p_conn->ntfyAttrList[BLE_ANCS_NTFY_ATTR_ID_NEGATIVE_ACTION_LABEL].p_attrData 
                = p_attrNtfyData->negativeAction;

    p_conn->appAttrList[BLE_ANCS_APP_ATTR_ID_DISPLAY_NAME].p_attrData 
                = p_attrAppData->displayName;
}

static void ble_ancs_GapEventProcess(BLE_GAP_Event_T *p_event)
{
    BLE_ANCS_ConnList_T *p_conn = NULL;
    
    switch(p_event->eventId)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            if ((p_event->eventField.evtConnect.status == GAP_STATUS_SUCCESS))
            {
                p_conn = ble_ancs_GetFreeConnList();
                if (p_conn != NULL)
                {
                    p_conn->connHandle = p_event->eventField.evtConnect.connHandle;
                }
            }
        }
        break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            uint16_t connHandle = p_event->eventField.evtDisconnect.connHandle;
            p_conn = ble_ancs_GetConnListByHandle(connHandle);
            if (p_conn != NULL)
			{
                ble_ancs_InitConnList(p_conn, 1);
			}
        }
        break;
        case BLE_GAP_EVT_ENCRYPT_STATUS:
        {
            uint16_t connHandle = p_event->eventField.evtEncryptStatus.connHandle;
            if (p_event->eventField.evtEncryptStatus.status == BLE_GAP_ENCRYPT_SUCCESS)
            {
                p_conn = ble_ancs_GetConnListByHandle(connHandle);
                if (p_conn == NULL)
                {
                    return;
                }
                p_conn->enableCccd = ANCS_INDEX_CHAR_DATA_CCCD;
                ble_ancs_enableCccd(connHandle);
            }

        }
        break;
        case BLE_GAP_EVT_TX_BUF_AVAILABLE:
        {
            ble_ancs_ProcessQueuedTask(p_event->eventField.evtTxBufAvailable.connHandle);
        }
        break;
        default:
        break;
    }
}

void BLE_ANCS_EventRegister(BLE_ANCS_EventCb_T bleAncsHandler)
{
    bleAncsProcess = bleAncsHandler;
}

static void ble_ancs_InitCharList(BLE_DD_CharList_T *p_charList, uint8_t connIndex)
{
    BLE_ANCS_CharIndex_T i;

    p_charList->connHandle = 0;
    p_charList->p_charInfo = (BLE_DD_CharInfo_T *) &(s_ancsCharInfoList[connIndex]);

    for(i=0; i<ANCS_INDEX_CHAR_MAX_NUM; i++)
    {
        s_ancsCharInfoList[connIndex][i].charHandle = 0;
        s_ancsCharInfoList[connIndex][i].property = 0;
    }
}

uint16_t BLE_ANCS_Init(void)
{
    uint16_t i;
    BLE_DD_DiscSvc_T ancsDisc;

    for (i = 0; i < BLE_ANCS_MAX_CONN_NBR; i++)
    {
        ble_ancs_InitConnList(&s_ancsConnList[i], 0);
        ble_ancs_InitCharList(&s_ancsCharList[i], i);
    }

    ancsDisc.svcUuid.uuidLength = ATT_UUID_LENGTH_16;
    (void)memcpy(ancsDisc.svcUuid.uuid, discSvcUuid, ATT_UUID_LENGTH_16);
    ancsDisc.p_discInfo = NULL;
    ancsDisc.p_discChars = ancsDiscCharList;
    ancsDisc.p_charList = s_ancsCharList;
    ancsDisc.discCharsNum = ANCS_INDEX_CHAR_MAX_NUM;

    return BLE_DD_ServiceDiscoveryRegister(&ancsDisc);
}

void BLE_ANCS_BleEventHandler(STACK_Event_T *p_stackEvent)
{
    switch (p_stackEvent->groupId)
    {
        case STACK_GRP_BLE_GAP:
            ble_ancs_GapEventProcess((BLE_GAP_Event_T *)p_stackEvent->p_event);
        break;

        case STACK_GRP_GATT:
            ble_ancs_GattEventProcess((GATT_Event_T *)p_stackEvent->p_event);
        break;

        default:
        break;
    }
}

void BLE_ANCS_BleDdEventHandler(BLE_DD_Event_T *p_event)
{
    switch (p_event->eventId)
    {
        case BLE_DD_EVT_DISC_COMPLETE:
        {
            BLE_ANCS_ConnList_T *p_conn;

            p_conn = ble_ancs_GetConnListByHandle(p_event->eventField.evtDiscResult.connHandle);
            if (p_conn != NULL)
            {
                BLE_ANCS_Event_T evtPara;

                p_conn->enableCccd = ANCS_INDEX_CHAR_DATA_CCCD;
                ble_ancs_enableCccd(p_event->eventField.evtDiscResult.connHandle);

                evtPara.eventField.evtDiscComplete.connHandle = p_conn->connHandle;
                ble_ancs_ConveyEvent(BLE_ANCS_EVT_DISC_COMPLETE_IND, (uint8_t *)&evtPara.eventField, sizeof(BLE_ANCS_EvtDiscComplete_T));
            }
        }
        break;

        default:
        break;
    }
}

