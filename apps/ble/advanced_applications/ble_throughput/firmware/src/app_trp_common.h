/*******************************************************************************
  Application Transparent Common Function Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_trp_common.h

  Summary:
    This file contains the Application Transparent common functions for this project.

  Description:
    This file contains the Application Transparent common functions for this project.
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


#ifndef APP_TRP_COMMON_H
#define APP_TRP_COMMON_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_utility.h"
#include "app_timer.h"
#include "system/console/sys_console.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_TRP_SERVER_ROLE                 0x01
#define APP_TRP_CLIENT_ROLE                 0x02
#define APP_TRP_CTRL_CHAN_ENABLE            0x01
#define APP_TRP_CTRL_CHAN_DISABLE           (~APP_TRP_CTRL_CHAN_ENABLE)
#define APP_TRP_DATA_CHAN_ENABLE            0x02
#define APP_TRP_DATA_CHAN_DISABLE           (~APP_TRP_DATA_CHAN_ENABLE)
#define APP_TRCBP_CTRL_CHAN_ENABLE          0x04
#define APP_TRCBP_CTRL_CHAN_DISABLE         (~APP_TRCBP_CTRL_CHAN_ENABLE)
#define APP_TRCBP_DATA_CHAN_ENABLE          0x08
#define APP_TRCBP_DATA_CHAN_DISABLE         (~APP_TRCBP_DATA_CHAN_ENABLE)

#define APP_TRP_MAX_TRANSMIT_NUM            0x0F

/**@defgroup BLE_TRS_ROLE BLE_TRS_ROLE
 * @brief The definition of BLE TRS role.
 * @{ */
#define BLE_TRP_ROLE_SERVER                 0x00    /**< A ble transparent server role in ATT protocol. */
#define BLE_TRP_ROLE_CLIENT                 0x01    /**< A ble transparent client role in ATT protocol. */
/** @} */

#define APP_TRP_VENDOR_OPCODE_BLE_UART      0x80    /**< Opcode for BLE UART */

#define APP_TRP_WMODE_TX_MODE_PL_NUM        0x02
#define APP_TRP_WMODE_CHECK_SUM_DISABLE     0x00
#define APP_TRP_WMODE_CHECK_SUM_ENABLE      0x01
#define APP_TRP_WMODE_CHECK_SUM             0x02
#define APP_TRP_WMODE_CHECK_SUM_PL_NUM      0x03
#define APP_TRP_WMODE_LOOPBACK_DISABLE      0x00
#define APP_TRP_WMODE_LOOPBACK_ENABLE       0x01
#define APP_TRP_WMODE_FIX_PATTERN_DISABLE   0x00
#define APP_TRP_WMODE_FIX_PATTERN_ENABLE    0x01
#define APP_TRP_WMODE_TX_LAST_NUMBER        0x02
#define APP_TRP_WMODE_FIX_PATTERN_PL_NUM    0x04
#define APP_TRP_WMODE_TX_MAX_SIZE           (500 * 0x400) // 500k bytes
#define APP_TRP_WMODE_UART_DISABLE          0x00
#define APP_TRP_WMODE_UART_ENABLE           0x01
#define APP_TRP_WMODE_TX_DATA_END           0x00
#define APP_TRP_WMODE_TX_DATA_START         0x01
#define APP_TRP_WMODE_TX_DATA_LENGTH        0x02
#define APP_TRP_WMODE_TX_LENGTH_PL_NUM      0x06
#define APP_TRP_WMODE_TX_TYPE               0x04
#define APP_TRP_WMODE_TX_TYPE_PL_NUM        0x03
#define APP_TRP_WMODE_TX_DATA_PL_NUM        0x02
#define APP_TRP_WMODE_ERROR_RSP             0x03
#define APP_TRP_WMODE_ERROR_PL_NUM          0x02
#define APP_TRP_WMODE_SNED_UP_CONN_STATUS   0x00
#define APP_TRP_WMODE_UPDATE_CONN_PARA      0x01
#define APP_TRP_WMODE_NOTIFY_PL_NUM         0x03
#define APP_TRP_WMODE_REV_LOOPBACK_DISABLE  0x00
#define APP_TRP_WMODE_REV_LOOPBACK_ENABLE   0x01

#define APP_TRP_SEND_GID_CS_FAIL            0x01
#define APP_TRP_SEND_GID_FP_FAIL            0x02
#define APP_TRP_SEND_GID_LB_FAIL            0x04
#define APP_TRP_SEND_GID_UART_FAIL          0x08
#define APP_TRP_SEND_GID_TX_FAIL            0x10
#define APP_TRP_SEND_ERROR_RSP_FAIL         0x20
#define APP_TRP_SEND_CHECK_SUM_FAIL         0x40
#define APP_TRP_SEND_LAST_NUMBER_FAIL       0x80
#define APP_TRP_SEND_LENGTH_FAIL            0x100
#define APP_TRP_SEND_TYPE_FAIL              0x200
#define APP_TRP_SEND_STATUS_FLAG            0x400
#define APP_TRP_SEND_GID_REV_LB_FAIL        0x800

#define APP_TRP_SERVER_UART                 0x01
#define APP_TRP_CLIENT_UART                 0x02

#define APP_TRP_LE_MAX_QUEUE_NUM            0x02
#define APP_TRP_ML_MAX_QUEUE_NUM            0x04

#define APP_TRP_TMR_ID_INST_MERGE(id, instance) ((((uint16_t)id) << 8) | instance)
#define APP_TRP_SET_QUEUE_NUMBER(clientLinkNum, serverLinkNum) ((clientLinkNum + serverLinkNum) < 2 ? APP_UTILITY_MAX_QUEUE_NUM : APP_TRP_ML_MAX_QUEUE_NUM)
#define APP_TRP_IS_CONN_MAX_DEV ((g_trpClientLinkNum+g_trpServerLinkNum) >= APP_BLE_MAX_LINK_NUMBER ? true : false)
#define APP_TRP_IS_OVER_CONN_MAX_DEV ((g_trpClientLinkNum+g_trpServerLinkNum) > APP_BLE_MAX_LINK_NUMBER ? true : false)

enum APP_TRP_WMODE_T
{
    TRP_WMODE_NULL = 0x00,
    TRP_WMODE_CHECK_SUM,
    TRP_WMODE_LOOPBACK,
    TRP_WMODE_FIX_PATTERN,
    
    TRP_WMODE_END   
};

/**@brief Enumeration type of BLE transparent type. */
typedef enum APP_TRP_TYPE_T
{
    APP_TRP_TYPE_UNKNOWN = 0x00,                     /**< Unknown Type. */
    APP_TRP_TYPE_LEGACY,                             /**< Legacy Transparent Profile. */
    APP_TRP_TYPE_TRCBP,                              /**< Transparent Credit Based Profile. */

    APP_TRP_TYPE_TOTAL
} APP_TRP_TYPE_T;

/**@brief Enumeration type from LE source or UART source. */
typedef enum APP_TRP_SRC_TYPE_T
{
    APP_TRP_SRC_TYPE_LE = 0x01,                     /**< LE source. */
    APP_TRP_SRC_TYPE_UART,                          /**< UART source. */

    APP_TRP_SRC_TYPE_TOTAL
} APP_TRP_SRC_TYPE_T;

enum APP_TRP_GRPID_T
{
    TRP_GRPID_NULL = 0x00,
    TRP_GRPID_CHECK_SUM,
    TRP_GRPID_LOOPBACK,
    TRP_GRPID_FIX_PATTERN,
    TRP_GRPID_UART,
    TRP_GRPID_TRANSMIT,
    TRP_GRPID_UPDATE_CONN_PARA,
    TRP_GRPID_WMODE_SELECTION,
    TRP_GRPID_REV_LOOPBACK,

    TRP_GRPID_END   
};

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/**@brief The structure contains information about circular queue format. */
typedef struct APP_TRP_LeCircQueue_T
{
    uint8_t                     usedNum;                                /**< The number of data list in circular queue. */
    uint8_t                     writeIdx;                               /**< The Index of data, written in circular queue. */
    uint8_t                     readIdx;                                /**< The Index of data, read in circular queue. */
    APP_UTILITY_QueueElem_T     queueElem[APP_TRP_LE_MAX_QUEUE_NUM];   /**< The circular data queue. @ref APP_UTILITY_QueueElem_T.*/
} APP_TRP_LeCircQueue_T;

/**@brief The structure contains information about APP transparent connection parameters for recording connection information. */
typedef struct APP_TRP_ConnList_T
{
    uint16_t                connHandle;         /**< Connection handle associated with this connection. */
    uint8_t                 trpRole;            /**< Transparent Role for APP_TRP_SERVER_ROLE or APP_TRP_CLIENT_ROLE */
    uint8_t                 channelEn;          /**< Channel enable for control channel and data channel. */
    uint16_t                exchangedMTU;       /**< Exchange MTU size */
    uint8_t                 workMode;           /**< Work active mode */
    uint8_t                 workModeEn;         /**< Enable work mode procedure */
    uint8_t                 trpState;           /**< Transparent state */
    uint32_t                checkSum;           /**< Check sum value for check sum mode */
    uint16_t                lastNumber;         /**< The last number value for fix pattern mode */
    uint16_t                rxLastNunber;       /**< The received last number value for fix pattern check */
    uint16_t                txMTU;              /**< The Tx MTU value to transmit data by GATT */
    uint32_t                fixPattMaxSize;     /**< The total pattern length for fix pattern mode */
    uint16_t                fixPattTrcbpMtu;    /**< The fix pattern MTU value for fix pattern mode over L2CAP CoC. */
    APP_TRP_TYPE_T          type;               /**< Transparent type. See @ref APP_TRP_TYPE_T. */
    uint16_t                txCtrRspFg;         /**< Transmit control data response flag */
    uint32_t                txTotalLeng;        /**< The transmission total length */
    APP_TRP_SRC_TYPE_T      srcType;            /**< The data source type. See @ref APP_TRP_SRC_TYPE_T. */
    uint16_t                lePktLeng;          /**< The LE packet length and it could be TRP or TRCBP packet size. */
    uint8_t                 upConnParaStatus;   /**< The status for update connection parameter. */
    uint8_t                 maxAvailTxNumber;   /**< The maximum available number of transmission packets. */
    APP_TRP_LeCircQueue_T   leCircQueue_t;      /**< The circular queue to store LE data */
    APP_TIMER_TmrElem_T     protocolRspTmr;
    APP_TIMER_TmrElem_T     leRxTmr;
    APP_TIMER_TmrElem_T     leTxTmr; 
} APP_TRP_ConnList_T;

/**@brief The structure contains the information about general data format. */
typedef struct APP_TRP_GenData_T
{
    uint16_t    rxLeng;         /**< Received length. */
    uint16_t    srcOffset;      /**< Source data offset. */
    uint8_t     *p_srcData;     /**< Source data pointer. */
    uint8_t     noNewElemFg;    /**< No new element flag. */     
} APP_TRP_GenData_T;


// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

extern uint8_t g_trpClientLinkNum, g_trpServerLinkNum;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_TRP_COMMON_Init(void);
uint16_t APP_TRP_COMMON_SetTimer(uint16_t idInstance, void *p_tmrParam, 
    uint32_t timeout, bool isPeriodicTimer, APP_TIMER_TmrElem_T *p_tmrElem);
uint8_t APP_TRP_COMMON_GetValidLeCircQueueNum(APP_TRP_LeCircQueue_T *p_circQueue_t);
uint16_t APP_TRP_COMMON_InsertDataToLeCircQueue(uint16_t dataLeng, uint8_t *p_data, 
    APP_TRP_LeCircQueue_T *p_circQueue_t);
APP_UTILITY_QueueElem_T * APP_TRP_COMMON_GetElemLeCircQueue(APP_TRP_LeCircQueue_T *p_circQueue_t);
void APP_TRP_COMMON_FreeElemLeCircQueue(APP_TRP_LeCircQueue_T *p_circQueue_t);
uint16_t APP_TRP_COMMON_SendVendorCmd(APP_TRP_ConnList_T *p_connList_t, 
    uint16_t length, uint8_t *p_payload);
uint16_t APP_TRP_COMMON_SendModeCommand(APP_TRP_ConnList_T *p_connList_t, 
    uint8_t grpId, uint8_t commandId);
uint16_t APP_TRP_COMMON_SendLengthCommand(APP_TRP_ConnList_T *p_connList_t, 
    uint32_t length);
uint16_t APP_TRP_COMMON_SendTypeCommand(APP_TRP_ConnList_T *p_connList_t);
uint16_t APP_TRP_COMMON_SendCheckSumCommand(APP_TRP_ConnList_T *p_connList_t);
uint16_t APP_TRP_COMMON_SendLastNumber(APP_TRP_ConnList_T *p_connList_t);
uint16_t APP_TRP_COMMON_SendErrorRsp(APP_TRP_ConnList_T *p_connList_t, 
    uint8_t grpId);
uint16_t APP_TRP_COMMON_SendUpConnParaStatus(APP_TRP_ConnList_T *p_connList_t, uint8_t grpId, uint8_t commandId, uint8_t upParaStatus);
uint16_t APP_TRP_COMMON_GetTrpDataLength(APP_TRP_ConnList_T *p_connList_t, uint16_t *p_dataLeng);
uint16_t APP_TRP_COMMON_GetTrpData(APP_TRP_ConnList_T *p_connList_t, uint8_t *p_data);
uint16_t APP_TRP_COMMON_FreeLeData(APP_TRP_ConnList_T *p_connList_t);
void APP_TRP_COMMON_DelAllCircData(APP_UTILITY_CircQueue_T *p_circQueue_t);
void APP_TRP_COMMON_DelAllLeCircData(APP_TRP_LeCircQueue_T *p_circQueue_t);
uint32_t APP_TRP_COMMON_CalculateCheckSum(uint32_t checkSum, uint32_t *dataLeng,
    APP_TRP_ConnList_T *p_connList_t);
uint8_t * APP_TRP_COMMON_GenFixPattern(uint16_t *startSeqNum, uint16_t *patternLeng,
    uint32_t *pattMaxSize, uint32_t *checkSum);
uint16_t APP_TRP_COMMON_UpdateFixPatternLen(APP_TRP_ConnList_T *p_connList_t);
uint16_t APP_TRP_COMMON_SendLeData(APP_TRP_ConnList_T *p_connList_t, uint16_t len, 
    uint8_t *p_data);
void APP_TRP_COMMON_InitFixPatternParam(APP_TRP_ConnList_T *p_connList_t);
uint16_t APP_TRP_COMMON_SendFixPatternFirstPkt(APP_TRP_ConnList_T *p_connList_t);
uint16_t APP_TRP_COMMON_SendFixPattern(APP_TRP_ConnList_T *p_connList_t);

uint16_t APP_TRP_COMMON_CheckFixPatternData(APP_TRP_ConnList_T *p_connList_t);

uint16_t APP_TRPC_SendLeDataTrpProfile(APP_TRP_ConnList_T *p_connList_t);
uint16_t APP_TRP_COMMON_SendLeDataCircQueue(APP_TRP_ConnList_T *p_connList_t);

#endif

