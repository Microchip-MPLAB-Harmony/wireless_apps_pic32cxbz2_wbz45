/*******************************************************************************
  BLE Transparent Server Profile Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_trsps.h

  Summary:
    This file contains the BLE Transparent Server functions for application user.

  Description:
    This file contains the BLE Transparent Server functions for application user.
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
 * @addtogroup BLE_TRPS
 * @{
 * @brief Header file for the BLE Transparent Profile library.
 * @note Definitions and prototypes for the BLE Transparent profile stack layer application programming interface.
 */
#ifndef BLE_TRSPS_H
#define BLE_TRSPS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "stack_mgr.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_TRPS_DEFINES Defines
 * @{ */

/**@defgroup BLE_TRS_MAX_CONN_NBR BLE_TRSPS_MAX_CONN_NBR
 * @brief The definition of Memory size.
 * @{ */
#define BLE_TRSPS_MAX_CONN_NBR                  0x04    /**< Maximum allowing Conncetion Numbers for MBADK. */
/** @} */

/**@defgroup BLE_TRSPS_STATUS BLE_TRSPS_STATUS
 * @brief The definition of BLE transparent service status.
 * @{ */
#define BLE_TRSPS_STATUS_CTRL_DISABLED          0x00    /**< Local ble transparent service control characteristic CCCD is closed. */
#define BLE_TRSPS_STATUS_CTRL_OPENED            0x01    /**< Local ble transparent service control characteristic CCCD is enable */
#define BLE_TRSPS_STATUS_TX_DISABLED            0x00    /**< Local ble transparent service TX characteristic CCCD is closed. */
#define BLE_TRSPS_STATUS_TX_OPENED              0x01    /**< Local ble transparent service TX characteristic CCCD is enable */
/** @} */

/**@} */ //BLE_TRPS_DEFINES


/**@addtogroup BLE_TRPS_ENUMS Enumerations
 * @{ */

/**@brief Enumeration type of BLE transparent profile callback events. */
enum BLE_TRSPS_EventId_T
{
    BLE_TRSPS_EVT_NULL = 0x00,
    BLE_TRSPS_EVT_CTRL_STATUS,                          /**< Transparent Profile Control Channel status update event. See @ref BLE_TRSPS_EvtCtrlStatus_T for event details. */
    BLE_TRSPS_EVT_TX_STATUS,                            /**< Transparent Profile Data Channel transmit status event. See @ref BLE_TRSPS_EvtTxStatus_T for event details. */
    BLE_TRSPS_EVT_CBFC_ENABLED,                         /**< Transparent Profile Credit based flow control enable notification event. See @ref BLE_TRSPS_EvtCbfcEnabled_T for event details. */
    BLE_TRSPS_EVT_CBFC_CREDIT,                          /**< Transparent Profile Credit based flow control credit update event. See @ref BLE_TRSPS_EvtCbfcEnabled_T for event details. */
    BLE_TRSPS_EVT_RECEIVE_DATA,                         /**< Transparent Profile Data Channel received notification event. See @ref BLE_TRSPS_EvtReceiveData_T for event details. */
    BLE_TRSPS_EVT_VENDOR_CMD,                           /**< Transparent Profile vendor command received notification event. See @ref BLE_TRSPS_EvtVendorCmd_T for event details. */
    BLE_TRSPS_EVT_END
};

/**@} */ //BLE_TRPS_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_TRPS_STRUCTS Structures
 * @{ */

/**@brief The structure contains information about BLE transparent profile control channel state event parameters. */
typedef struct BLE_TRSPS_EvtCtrlStatus_T
{
    uint16_t         connHandle;                                            /**< Connection handle associated with this connection. */
    uint8_t          status;                                                /**< Connection status. See @ref BLE_TRSPS_STATUS.*/
}   BLE_TRSPS_EvtCtrlStatus_T;

/**@brief The structure contains information about BLE transparent profile connected state event parameters. */
typedef struct BLE_TRSPS_EvtTxStatus_T
{
    uint16_t         connHandle;                                            /**< Connection handle associated with this connection. */
    uint8_t          status;                                                /**< Connection status. See @ref BLE_TRSPS_STATUS.*/
}   BLE_TRSPS_EvtTxStatus_T;

/**@brief The structure contains information about BLE transparent profile credit based flow control enabled event parameters. */
typedef struct BLE_TRSPS_EvtCbfcEnabled_T
{
    uint16_t         connHandle;                                            /**< Connection handle associated with this connection. */
}BLE_TRSPS_EvtCbfcEnabled_T;

/**@brief The structure contains information about BLE transparent profile write request event parameters. */
typedef struct BLE_TRSPS_EvtReceiveData_T
{
    uint16_t         connHandle;                                            /**< Connection handle associated with this connection. */
}BLE_TRSPS_EvtReceiveData_T;

/**@brief The structure contains information about BLE transparent profile vendor command event parameters. */
typedef struct BLE_TRSPS_EvtVendorCmd_T
{
    uint16_t         connHandle;                                            /**< Connection handle associated with this connection. */
    uint16_t         length;																								/**< Vendor command payload length. */
    uint8_t          *p_payLoad;                                            /**< Vendor command payload pointer. */
}BLE_TRSPS_EvtVendorCmd_T;

/**@brief The union of BLE transparent profile event types associated with BLE transparent profile callback events to the application layer. */
typedef union
{
    BLE_TRSPS_EvtCtrlStatus_T        onCtrlStatus;            /**< see @ref BLE_TRSPS_EvtCtrlStatus_T. */
    BLE_TRSPS_EvtTxStatus_T          onTxStatus;              /**< see @ref BLE_TRSPS_EvtTxStatus_T. */
    BLE_TRSPS_EvtCbfcEnabled_T       onCbfcEnabled;           /**< see @ref BLE_TRSPS_EvtCbfcEnabled_T. */
    BLE_TRSPS_EvtReceiveData_T       onReceiveData;           /**< see @ref BLE_TRSPS_EvtReceiveData_T. */
    BLE_TRSPS_EvtVendorCmd_T         onVendorCmd;             /**< see @ref BLE_TRSPS_EvtVendorCmd_T. */
} BLE_TRSPS_EventField_T;

/**@brief The structure contains information about BT SPP callback function parameters. */
typedef struct  BLE_TRSPS_Event_T
{
    uint8_t                   eventId;                                 /**< Event ID.*/
    BLE_TRSPS_EventField_T    eventField;                              /**< see @ref BLE_TRSPS_EventField_T. */
} BLE_TRSPS_Event_T;

/**@brief The structure contains information about change UUID function parameters. */
typedef struct  BLE_TRSPS_Uuids_T
{
    uint8_t *p_primaryService;                      /**< The user-defined 128-bit primary service UUID.*/
    uint8_t *p_transTx;                             /**< The user-defined 128-bit trans tx characteristic UUID. */
    uint8_t *p_transRx;                             /**< The user-defined 128-bit trans rx characteristic UUID. */
    uint8_t *p_transCtrl;                           /**< The user-defined 128-bit trans ctrl characteristic UUID. */
} BLE_TRSPS_Uuids_T;

/**@} */ //BLE_TRPS_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_TRPS_FUNS Functions
 * @{ */

/**@brief BLE transparent profile callback function prototype. */
typedef void(*BleTrspsProcessRoutine)(BLE_TRSPS_Event_T *p_event);

/**
 *@brief Initialize BLE Transparent Profile callback function.
 *
 * @param[in] bleTranServHandler          	Pointer to function callback for BLE transparent profile events.
 *
 */
void BLE_TRSPS_EventRegistration(BleTrspsProcessRoutine bleTranServHandler);

/**
 *@brief Initialize BLE Transparent Profile.
 *
 * @return MBA_RES_SUCCESS                 	Success to add a service to the service table. 
 * @return MBA_RES_FAIL                  		Fail to add a service to the service table.
 * @return MBA_RES_OOM                  		No available memory. 
 *
 */
uint16_t BLE_TRSPS_Init();


/**@brief Change UUIDs of transparent service and characteristics.
 *
 * @param[in] p_bleTranServUuids            Pointer to the UUIDs see @ref BLE_TRSPS_Uuids_T.
 *
 */
void BLE_TRSPS_ChangeUuids(BLE_TRSPS_Uuids_T *p_bleTranServUuids);


/**@brief The API is used to send vendor command.
 *
 * @param[in] connHandle                    Connection handle associated with this connection.
 * @param[in] commandID                     Command id of the vendor command
 * @param[in] commandLength                 Length of payload in vendor commnad
 * @param[in] p_commandPayload              Pointer to the payload of vendor command
 *
 * @return MBA_RES_SUCCESS                  Successfully issue a send vendor command.
 * @return MBA_RES_FAIL                     Invalid connection.
 * @return MBA_RES_OOM                      No available memory.
 * @return MBA_RES_INVALID_PARA             Error commandID usage or commandLength invalid or the CCCD of TCP is not enabled.
 *
 */
uint16_t BLE_TRSPS_SendVendorCommand(uint16_t connHandle, uint8_t commandID, uint8_t commandLength, uint8_t *p_commandPayload);


/**@brief The API is used to send data.
 *
 * @param[in] connHandle                    Connection handle associated with this connection.
 * @param[in] len                           Data length.
 * @param[in] p_data                        Pointer to the transparent data.
 *
 * @return MBA_RES_SUCCESS                  Successfully issue a send data.
 * @return MBA_RES_OOM                      No available memory.
 * @return MBA_RES_INVALID_PARA             Parameter does not meet the spec.
 *
 */
uint16_t BLE_TRSPS_SendData(uint16_t connHandle, uint16_t len, uint8_t *p_data);

/**@brief The API gets queued data length.
 *
 * @param[in]    connHandle                 Connection handle associated with the queued data
 * @param[out]   p_dataLength               Pointer to the data length
 *
 * @return void
 *
 */
void BLE_TRSPS_GetDataLength(uint16_t connHandle, uint16_t *p_dataLength);


/**@brief The API gets queued data.
 *
 * @param[in] connHandle                    Connection handle associated with the queued data
 * @param[out] p_data                       Pointer to the data buffer
 *
 * @return MBA_RES_SUCCESS                  Successfully issue a flow ctrl stop.
 * @return MBA_RES_FAIL                     No data in the input queue or can not find the link.
 *
 */
uint16_t BLE_TRSPS_GetData(uint16_t connHandle, uint8_t *p_data);

/**@brief Function for handling BLE events.
 *       This API should be called in the application while caching BLE events
 *
 * @param[in] p_stackEvent        					Pointer to BLE events buffer.
 *
*/
void BLE_TRSPS_BleEventHandler(STACK_Event_T *p_stackEvent);

/**@} */ //BLE_TRPS_FUNS

#endif

/**
  @}
 */
