/*******************************************************************************
  BLE L2CAP Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_l2cap.h

  Summary:
    This file contains the BLE L2CAP functions for application user.

  Description:
    This file contains the BLE L2CAP functions and event for application user.  The
    "BLE_L2CAP_Init" function shall be called in the "APP_Initialize" function to 
    initialize the this modules in the system
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
 * @addtogroup BLE_L2CAP
 * @{
 * @brief Header file for the BLE Logical Link Control for protocol/service multiplexers library.
 * @note Definitions and prototypes for the BLE L2CAP stack layer application programming interface.
 */

#ifndef BLE_L2CAP_H
#define BLE_L2CAP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

/**@addtogroup BLE_L2CAP_DEFINES Defines
 * @{ */
 
 
/**@defgroup BLE_L2CAP_SPSM_RANGE SPSM range
 * @brief Range for SPSM.
 * @{ */
#define BLE_L2CAP_SPSM_DYNAMIC_MIN                  0x0080          /**< The minimum dynamic SPSM value */
#define BLE_L2CAP_SPSM_DYNAMIC_MAX                  0x00ff          /**< The maximum dynamic SPSM value */
/** @} */

/**@brief The definition of maximum PDU size */
#define BLE_L2CAP_MAX_PDU_SIZE                      1024

/**@brief The definition of minimum MTU size */
#define BLE_L2CAP_MIN_MTU_SIZE                      23

/**@brief The definition of minimum MPS size */
#define BLE_L2CAP_MIN_MPS_SIZE                      23

/**@brief The definition of maximum SDU size */
#define BLE_L2CAP_MAX_SDU_SIZE                      739


/**@brief The definition of maximum SPSM number of credit based channel can be registered */
#define BLE_L2CAP_MAX_CB_SPSM_NUM                   2


/**@brief The definition of maximum L2cap credit based channel number per connection */
#define BLE_L2CAP_MAX_CB_NUM                        0x05


/**@defgroup BLE_L2CAP_PERMISSION Permission definition
*@brief The definition of L2CAP permission.
* @{ */
#define BLE_L2CAP_PERMISSION_AUTH                   0x01            /**< BLE L2CAP premission Auth. */
#define BLE_L2CAP_PERMISSION_ENC                    0x02            /**< BLE L2CAP premission Enc. */
/** @} */

/**@defgroup BLE_L2CAP_CONN_PARAM_RSP_RESULT Connection parameter update result
 *@brief The definition of response result code for connection parameter update.
 * @{ */
#define BLE_L2CAP_CONN_PARAMS_ACCEPT                0x0000          /**< Connection Parameters accepted */
#define BLE_L2CAP_CONN_PARAMS_REJECT                0x0001          /**< Connection Parameters rejected */
/** @} */

/**@defgroup BLE_L2CAP_CB_CONN_RSP_RESULT Connection result
 * @brief The definition of the result for L2CAP credit based connection response packet.
 * @{ */
#define BLE_L2CAP_RES_CONN_SUCCESSFUL               0x0000          /**< Connection successful. */
#define BLE_L2CAP_RES_CONN_SPSM_NOT_SUPPORTED       0x0002          /**< Connection refused - SPSM not supported. */
#define BLE_L2CAP_RES_CONN_NO_RESOURCES             0x0004          /**< Connection refused - no resource available. */
#define BLE_L2CAP_RES_CONN_INSUFF_AUTHENTICATION    0x0005          /**< Connection refused - insufficient authentication. */
#define BLE_L2CAP_RES_CONN_INSUFF_AUTHORIZATION     0x0006          /**< Connection refused - insufficient authorization. */
#define BLE_L2CAP_RES_CONN_INSUFF_ENC_KEY_SIZE      0x0007          /**< Connection refused - insufficient encryption key size. */
#define BLE_L2CAP_RES_CONN_INSUFF_ENC               0x0008          /**< Connection refused - insufficient encryption. */
#define BLE_L2CAP_RES_CONN_INVALID_SOURCE_CID       0x0009          /**< Connection refused - invalid Source CID. */
#define BLE_L2CAP_RES_CONN_SOURCE_CID_ALLOCATED     0x000A          /**< Connection refused - Source CID already allocated. */
#define BLE_L2CAP_RES_CONN_UNACCEPT_PARAMS          0x000B          /**< Connection refused - unacceptable parameters. */
#define BLE_L2CAP_RES_CONN_REJECT                   0xFFFF          /**< Command reject. */
/** @} */

/**@} */ //BLE_L2CAP_DEFINES


/**@addtogroup BLE_L2CAP_ENUMS Enumerations
 * @{ */

/**@brief L2cap events delivered to application from BLE Stack. */
enum BLE_L2CAP_EventId_T
{
    BLE_L2CAP_EVT_CONN_PARA_UPDATE_REQ=0x00,                       /**< Notify connection parameter update request received. See the @ref BLE_L2CAP_EvtConnParamUpdateReq_T for the event content. */
    BLE_L2CAP_EVT_CONN_PARA_UPDATE_RSP,                            /**< Notify connection parameter update response received. See the @ref BLE_L2CAP_EvtConnParamUpdateRsp_T for the event content. */
    BLE_L2CAP_EVT_CB_CONN_IND,                                     /**< Notify a credit based connection is opened. See the @ref BLE_L2CAP_EvtCbConnInd_T for the event content. */
    BLE_L2CAP_EVT_CB_CONN_FAIL_IND,                                /**< Notify a credit based connection is failed. See the @ref BLE_L2CAP_EvtCbConnFailInd_T for the event content. */
    BLE_L2CAP_EVT_CB_SDU_IND,                                      /**< Notify credit based SDU received. See the @ref BLE_L2CAP_EvtCbSduInd_T for the event content. */
    BLE_L2CAP_EVT_CB_ADD_CREDITS_IND,                              /**< Notify credit based credit received. See the @ref BLE_L2CAP_EvtCbAddCreditsInd_T for the event content. */
    BLE_L2CAP_EVT_CB_DISC_IND,                                     /**< Notify credit based disconnect request received. See the @ref BLE_L2CAP_EvtCbDiscInd_T for the event content. */
    
    BLE_L2CAP_EVT_END
};

/**@} */ //BLE_L2CAP_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_L2CAP_STRUCTS Structures
 * @{ */
 
/**@brief Parameters for @ref BLE_L2CAP_EVT_CONN_PARA_UPDATE_REQ event.*/
typedef struct BLE_L2CAP_EvtConnParamUpdateReq_T
{
    uint16_t      connHandle;                                      /**< Connection handle. */
    uint16_t      intervalMin;                                     /**< Minimum value for the connection interval. See @ref BLE_GAP_CP_RANGE. */
    uint16_t      intervalMax;                                     /**< Maximum value for the connection interval. See @ref BLE_GAP_CP_RANGE.*/
    uint16_t      latency;                                         /**< Peripheral latency parameter. See @ref BLE_GAP_CP_RANGE. */
    uint16_t      timeout;                                         /**< Connection timeout. See @ref BLE_GAP_CP_RANGE. */
} BLE_L2CAP_EvtConnParamUpdateReq_T;

/**@brief Parameters for @ref BLE_L2CAP_EVT_CONN_PARA_UPDATE_RSP event.*/
typedef struct BLE_L2CAP_EvtConnParamUpdateRsp_T
{
    uint16_t      connHandle;                                      /**< Connection handle. */
    uint16_t      result;                                          /**< The outcome of the connection parameters updating. See @ref BLE_L2CAP_CONN_PARAM_RSP_RESULT. */
} BLE_L2CAP_EvtConnParamUpdateRsp_T;


/**@brief Parameters for @ref BLE_L2CAP_EVT_CB_CONN_IND event.*/
typedef struct BLE_L2CAP_EvtCbConnInd_T
{
    uint8_t       leL2capId;                                       /**< Instance of l2cap session. */
    uint16_t      connHandle;                                      /**< Connection handle. */
    uint16_t      spsm;                                            /**< SPSM. */
    uint16_t      remoteMtu;                                       /**< Remote maximum transmit unit, in bytes */
    uint16_t      remoteMps;                                       /**< Remote maximum PDU size, in bytes */
    uint16_t      initialCredits;                                  /**< Initial credits. */
} BLE_L2CAP_EvtCbConnInd_T;

/**@brief Parameters for @ref BLE_L2CAP_EVT_CB_CONN_FAIL_IND event.*/
typedef struct BLE_L2CAP_EvtCbConnFailInd_T
{
    uint16_t      connHandle;                                      /**< Connection handle. */
    uint16_t      spsm;                                            /**< SPSM. */
    uint16_t      result;                                          /**< Remote maximum transmit unit. */
} BLE_L2CAP_EvtCbConnFailInd_T;

/**@brief Parameters for @ref BLE_L2CAP_EVT_CB_SDU_IND event.*/
typedef struct BLE_L2CAP_EvtCbSduInd_T
{
    uint8_t       leL2capId;                                       /**< Instance of l2cap session. */
    uint16_t      length;                                          /**< Length of SDU payload, in bytes*/
    uint8_t       payload[BLE_L2CAP_MAX_PDU_SIZE];                 /**< SDU payload. */
    uint8_t       frames;                                          /**< Number of frames reassembled for this SDU. */
} BLE_L2CAP_EvtCbSduInd_T;


/**@brief Parameters for @ref BLE_L2CAP_EVT_CB_ADD_CREDITS_IND event.*/
typedef struct BLE_L2CAP_EvtCbAddCreditsInd_T
{
    uint8_t       leL2capId;                                       /**< Instance of l2cap session. */
    uint16_t      credits;                                         /**< Initial credits. */
} BLE_L2CAP_EvtCbAddCreditsInd_T;


/**@brief Parameters for @ref BLE_L2CAP_EVT_CB_DISC_IND event.*/
typedef struct BLE_L2CAP_EvtCbDiscInd_T
{
    uint8_t       leL2capId;                                       /**< Instance of l2cap session. */
} BLE_L2CAP_EvtCbDiscInd_T;


/**@brief Union containing details regarding a BLE l2cap event. */
typedef union
{
    BLE_L2CAP_EvtConnParamUpdateReq_T        evtConnParamUpdateReq;    /**< See @ref BLE_L2CAP_EvtConnParamUpdateReq_T. */
    BLE_L2CAP_EvtConnParamUpdateRsp_T        evtConnParamUpdateRsp;    /**< See @ref BLE_L2CAP_EvtConnParamUpdateRsp_T. */
    BLE_L2CAP_EvtCbConnInd_T                 evtCbConnInd;             /**< See @ref BLE_L2CAP_EvtCbConnInd_T. */
    BLE_L2CAP_EvtCbConnFailInd_T             evtCbConnFailInd;         /**< See @ref BLE_L2CAP_EvtCbConnFailInd_T. */
    BLE_L2CAP_EvtCbSduInd_T                  evtCbSduInd;              /**< See @ref BLE_L2CAP_EvtCbSduInd_T. */
    BLE_L2CAP_EvtCbAddCreditsInd_T           evtCbAddCreditsInd;       /**< See @ref BLE_L2CAP_EvtCbAddCreditsInd_T. */
    BLE_L2CAP_EvtCbDiscInd_T                 evtCbDiscInd;             /**< See @ref BLE_L2CAP_EvtCbDiscInd_T. */
} BLE_L2CAP_EventField_T;


/**@brief Parameters of BLE L2cap event. */
typedef struct  BLE_L2CAP_Event_T
{
    uint8_t                 eventId;                               /**< Event ID. See @ref BLE_L2CAP_EventId_T.*/
    BLE_L2CAP_EventField_T  eventField;                            /**< See @ref BLE_L2CAP_EventField_T. */
} BLE_L2CAP_Event_T;

/**@} */ //BLE_L2CAP_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**@addtogroup BLE_L2CAP_FUNS Functions
 * @{ */
 
/**@brief Initialize BLE L2CAP module.
 *
 * @retval MBA_RES_SUCCESS      Successfully initialize BLE L2cap main module.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
*/
uint16_t BLE_L2CAP_Init();


/**@brief Initialize BLE L2CAP Credit Based module.
 *
 * @retval MBA_RES_SUCCESS      Successfully initialize BLE L2cap credit based module.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
*/
uint16_t BLE_L2CAP_CbInit();


/**@brief Issue a connection parameter update request.
 * @note  This API shall only be called if local is LE slave role.
 *
 * @par Events generated
 * @ref BLE_L2CAP_EVT_CONN_PARA_UPDATE_RSP Generated when connection parameter update response received.
 *
 * @param[in] connHandle        Connection handle associated with this connection.
 * @param[in] intervalMin       Minimum value for the connection interval. See @ref BLE_GAP_CP_RANGE.
 * @param[in] intervalMax       Maximum value for the connection interval. See @ref BLE_GAP_CP_RANGE.
 * @param[in] latency           Peripheral latency parameter. See @ref BLE_GAP_CP_RANGE.
 * @param[in] timeout           Connection timeout parameter. See @ref BLE_GAP_CP_RANGE.
 *
 * @retval MBA_RES_SUCCESS      Successfully send a L2cap connection parameters update request.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
 * @retval MBA_RES_NO_RESOURCE  No Transmit buffers available for sending L2cap connection parameters update request.
 * @retval MBA_RES_FAIL         Fail to send L2cap connection parameters update request by error L2cap instance.
 */
uint16_t BLE_L2CAP_ConnParamUpdateReq(uint16_t connHandle, uint16_t intervalMin, uint16_t intervalMax, uint16_t latency, uint16_t timeout);


/**@brief Reply a connection parameter update request.
 * @note  This function should be called when BLE_L2CAP_EVT_CONN_PARA_UPDATE_REQ received.
 *
 * @param[in] connHandle        Connection handle associated with this connection.
 * @param[in] result            Accept or reject the request. See @ref BLE_L2CAP_CONN_PARAM_RSP_RESULT.
 *
 * @retval MBA_RES_SUCCESS      Successfully issue a L2cap connection parameters update response.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
 * @retval MBA_RES_NO_RESOURCE  No Transmit buffers available for sending L2cap connection parameters update response.
 * @retval MBA_RES_FAIL         Fail to send L2cap connection parameters update response by error L2cap instance.
 */
uint16_t BLE_L2CAP_ConnParamUpdateRsp(uint16_t connHandle, uint16_t result);

/**@brief Issue a L2cap credit based connection request to establish a L2cap channel.
 * @note  @ref BLE_L2CAP_CbRegisterSpsm must be called to register SPSM before calling this API.
 *
 * @par Events generated
 * @ref BLE_L2CAP_EVT_CB_CONN_IND Generated when a credit based connection is established successfully.\n
 * @ref BLE_L2CAP_EVT_CB_CONN_FAIL_IND Generated when a credit based connection is established failure.
 *
 * @param[in] connHandle        Connection handle associated with this connection.
 * @param[in] spsm              SPSM. See @ref BLE_L2CAP_SPSM_RANGE
 *
 * @retval MBA_RES_SUCCESS      Successfully issue a connection request.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
 * @retval MBA_RES_NO_RESOURCE  No Transmit buffers available for sending connection request.
 */
uint16_t BLE_L2CAP_CbConnReq(uint16_t connHandle, uint16_t spsm);


/**@brief Send a L2cap SDU over credit based connection channel.
 *
 * @param[in] leL2capId         L2cap instance.
 * @param[in] length            Length of L2cap SDU. The maximum value is @ref BLE_L2CAP_MAX_SDU_SIZE.
 * @param[in] p_payload         Point to the L2cap SDU buffer.
 *
 * @retval MBA_RES_SUCCESS      Successfully send a L2cap data.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
 * @retval MBA_RES_NO_RESOURCE  No Transmit buffers available for sending L2cap data.
 * @retval MBA_RES_FAIL         Fail to send L2cap data by error L2cap instance. 
 */
uint16_t BLE_L2CAP_CbSendSdu(uint8_t leL2capId, uint16_t length, uint8_t *p_payload);


/**@brief Send a L2cap flow control credit to peer device.
 * @note  Credit should be returned to peer device after receiving data. Otherwise no more data could be sent by peer device.
 *
 * @param[in] leL2capId          L2cap instance.
 * @param[in] credits            Credit number.
 *
 * @retval MBA_RES_SUCCESS      Successfully send credit to peer device.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
 * @retval MBA_RES_NO_RESOURCE  No Transmit buffers available for sending L2cap data.
 * @retval MBA_RES_FAIL         Fail to send credit to peer device by error L2cap instance. 
 *
 */
uint16_t BLE_L2CAP_CbAddCredits(uint8_t leL2capId, uint16_t credits);


/**@brief Issue a disconnect request to credit based connection channel.
 *
 * @par Events generated
 * @ref BLE_L2CAP_EVT_CB_DISC_IND Generated when a credit based connection is disconnected.
 *
 * @param[in] leL2capId         L2cap instance.
 *
 * @retval MBA_RES_SUCCESS      Successfully issue a L2cap disconnect request.
 * @retval MBA_RES_OOM          Internal memory allocation failure.
 * @retval MBA_RES_NO_RESOURCE  No Transmit buffers available for sending L2cap disconnect request.
 * @retval MBA_RES_FAIL         Fail to send L2cap disconnect request by error L2cap instance.
 *
 */
uint16_t BLE_L2CAP_CbDiscReq(uint8_t leL2capId);


/**@brief Register a SPSM.
 * @note  This function should be called before establishing credit based channel.
 *
 * @param[in] spsm              Simplified Protocol/Service Multiplexer.
 * @param[in] mtu               Maximum Transmission Unit. Range is @ref BLE_L2CAP_MIN_MTU_SIZE ~ @ref BLE_L2CAP_MAX_PDU_SIZE.
 * @param[in] mps               Maximum PDU Payload Size. The minimum value is @ref BLE_L2CAP_MIN_MPS_SIZE.
 * @param[in] initCredits       Initial Credits.
 * @param[in] permission        Permission of the SPSM. See @ref BLE_L2CAP_PERMISSION
 *
 * @retval MBA_RES_SUCCESS      Successfully register SPSM.
 * @retval MBA_RES_INVALID_PARA Invalid MTU parameter usage.
 * @retval MBA_RES_FAIL         Fail to register SPSM.
 */
uint16_t BLE_L2CAP_CbRegisterSpsm(uint16_t spsm, uint16_t mtu, uint16_t mps, uint16_t initCredits, uint8_t permission);


/**@brief Deregister a SPSM.
 * @note This function should be called to free unused registered SPSM.
 *
 * @param[in] spsm              Simplified Protocol/Service Multiplexer.
 *
 * @retval MBA_RES_SUCCESS      Successfully deregister SPSM.
 * @retval MBA_RES_FAIL         Fail to deregister SPSM.
 */
uint16_t BLE_L2CAP_CbDeregisterSpsm(uint16_t spsm);

/**@} */ //BLE_L2CAP_FUNS
#endif

/**
  @}
*/
