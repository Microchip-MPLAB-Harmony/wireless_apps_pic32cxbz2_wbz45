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
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_handler.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef APP_BLE_HANDLER_H
#define APP_BLE_HANDLER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>
#include "gap_defs.h"
#include "ble_gap.h"
#include "ble_l2cap.h"
#include "ble_smp.h"
#include "gatt.h"
#include "ble_dtm.h"
#include "ble_dm/ble_dm.h"
//#include "ble_log/ble_log.h"

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
/**@brief Maximum device in peripheral role. */
#define APP_TRPS_MAX_LINK_NUMBER        BLE_TRSPS_MAX_CONN_NBR

/**@brief Maximum device in central role. */
#define APP_TRPC_MAX_LINK_NUMBER        BLE_TRSPC_MAX_CONN_NBR

/**@defgroup APP_BLE_MAX_LINK_NUMBER APP_BLE_MAX_LINK_NUMBER
 * @brief The definition of maximum BLE links that can exist
 * @{ */
#define APP_BLE_MAX_LINK_NUMBER         BLE_GAP_MAX_LINK_NBR
/** @} */

//#define APP_BLE_L2CAP_MAX_LINK_NUM      0x02   //BLE_TRCBPS_MAX_CONN_NBR

#define APP_BLE_UNKNOWN_ID              0xFF


/**@brief Enumeration type of BLE state. */
typedef enum APP_BLE_LinkState_T
{
    APP_BLE_STATE_STANDBY,                                                /**< Standby state. i.e. not advertising */
    APP_BLE_STATE_ADVERTISING,                                            /**< BLE is advertising */
    APP_BLE_STATE_SCANNING,                                               /**< BLE is scanning (only for BLE_GAP_ROLE_CENTRAL) */
    APP_BLE_STATE_CONNECTED,                                              /**< BLE is connected */
    APP_BLE_STATE_CONNECTED_ADVERTISING,                                  /**< BLE is connected with an advertising (only for multi-link) */
    APP_BLE_STATE_TOTAL
} APP_BLE_LinkState_T;


/**@brief Enumeration type of pair trigger method. */
typedef enum APP_BLE_PAIR_TRIGGER_ID
{
    APP_BLE_PAIR_TRIGGER_CENTRAL = 0x00,
    APP_BLE_PAIR_TRIGGER_PERIPHERAL,
    APP_BLE_PAIR_TRIGGER_End
} APP_BLE_PAIR_TRIGGER_ID;

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/**@brief The structure contains the BLE Connection parameters. */
typedef struct APP_BLE_ConnData_T
{
    uint8_t                role;                                           /**< GAP role, see @ref BLE_GAP_ROLE. */
    uint16_t               handle;                                         /**< Connection handle associated with this connection. */
    BLE_GAP_Addr_T         remoteAddr;                                     /**< See @ref BLE_GAP_Addr_T. */
    uint16_t               connInterval;                                   /**< Connection interval used on this connection. Range should be @ref BLE_GAP_CP_RANGE. */
    uint16_t               connLatency;                                    /**< Slave latency for the connection in terms of number of connection events, see @ref BLE_GAP_CP_RANGE. */
    uint16_t               supervisionTimeout;                             /**< Supervision timeout for the LE Link, see @ref BLE_GAP_CP_RANGE. */
} APP_BLE_ConnData_T;

/**@brief This structure contains the BLE security related information. */
typedef struct APP_BLE_SecuData_T
{
    uint8_t                encryptionStatus;                               /**< BLE encryption status. 1: Enabled; 0: Disabled. */
    BLE_GAP_Addr_T         smpInitiator;                                   /**< Initiator BD_ADDR and address type. See @ref BLE_GAP_Addr_T */
} APP_BLE_SecuData_T;


#if 0
/**@brief This structure contains the BLE Transparent Credit Based Profile connection parameters. */
typedef struct APP_BLE_TrcbpsConnData_T
{
    BLE_TRCBPS_ChanType_T type;                                            /**< Channel type. See @ref BLE_TRCBPS_ChanType_T. */
    uint8_t      leL2capId;                                                /**< Instance of l2cap session. */
    uint8_t      state;                                                    /**< TRCBPS connection state. See @ref BLE_TRCBPS_STATUS. */
    uint16_t     peerMtu;                                                  /**< MTU size of peer device. */
    uint16_t     peerMps;                                                  /**< PDU size of peer device. */
} APP_BLE_TrcbpsConnData_T;
#endif


/**@brief The structure contains the BLE link related information maintained by the application Layer */
typedef struct APP_BLE_ConnList_T
{
    APP_BLE_LinkState_T         linkState;                                              /**< BLE link state. see @ref APP_BLE_LinkState_T */
    APP_BLE_ConnData_T          connData;                                               /**< BLE connection information. See @ref APP_BLE_ConnData_T */
    APP_BLE_SecuData_T          secuData;                                               /**< BLE security information. See @ref APP_BLE_SecuData_T */
    //APP_BLE_TrcbpsConnData_T    trcbpsConnData[APP_BLE_L2CAP_MAX_LINK_NUM];             /**< BLE TRCBP connection parameters. See @ref APP_BLE_TrcbpsConnData_T */
} APP_BLE_ConnList_T;


typedef struct APP_Database_T
{
    uint8_t ioCapability;
    uint16_t gattcReadUuid;             /* Used for GATTC GATTC_ReadUsingUUID() API. */
} APP_Database_T;

typedef struct APP_ConnList
{
    uint8_t                         bdAddr[7];
    uint16_t                        connHandle;
    uint8_t                         state;
} APP_ConnList;

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
extern APP_Database_T         g_appDb;
extern APP_ConnList           g_appConnList[APP_BLE_MAX_LINK_NUMBER];


// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_BleGapEvtHandler( BLE_GAP_Event_T *p_event )

  Summary:
     BLE Stack application GAP event handler.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleGapEvtHandler(BLE_GAP_Event_T *p_event);

/*******************************************************************************
  Function:
    void APP_BleL2capEvtHandler( BLE_L2CAP_Event_T *p_event )

  Summary:
     BLE application L2cap event handler.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleL2capEvtHandler(BLE_L2CAP_Event_T *p_event);

/*******************************************************************************
  Function:
    void APP_GattEvtHandler( GATT_Event_T * p_event )

  Summary:
     BLE application GATT event handler.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_GattEvtHandler(GATT_Event_T * p_event);

/*******************************************************************************
  Function:
    void APP_BleSmpEvtHandler( BLE_SMP_Event_T *p_event )

  Summary:
     BLE application SMP event handler.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleSmpEvtHandler(BLE_SMP_Event_T *p_event);

/*******************************************************************************
  Function:
    void APP_DmEvtHandler( BLE_DM_Event_T *p_event )

  Summary:
     BLE application DM event handler.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_DmEvtHandler(BLE_DM_Event_T *p_event);

APP_BLE_ConnList_T *APP_GetFreeConnList(void);
void APP_InitConnList(void);
void APP_UpdateLocalName(uint8_t devNameLen, uint8_t *p_devName);
APP_BLE_LinkState_T APP_GetBleState(void);
void APP_SetBleState(APP_BLE_LinkState_T state);
uint8_t APP_GetBleRole(void);

uint16_t APP_GetCurrentConnHandle(void);
APP_BLE_ConnList_T *APP_GetAdvConnList(void);

#ifdef APP_PAIRING_ENABLE
void APP_SetPairTrigger(uint8_t trigger);
#endif

#endif /* _APP_BLE_HANDLER_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

//#endif /* APP_BLE_HANDLER_H */


/*******************************************************************************
 End of File
 */

