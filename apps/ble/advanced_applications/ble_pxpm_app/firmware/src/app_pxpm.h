/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
    app_pxpm.h

  Summary:
    This header file provides functions for the application.

  Description:
    This header file provides functions for the application.
*******************************************************************************/

#ifndef APP_PXPM_H
#define APP_PXPM_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "mba_error_defs.h"
#include "gap_defs.h"
#include "ble_gap.h"
#include "gatt.h"
#include "ble_pxpm/ble_pxpm.h"
#include "app_timer.h"
#include "app_key.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
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

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/* For PXPR, calculating the link budget value as a standard for path loss assessment, especially in cases where power control is not supported. */
typedef struct APP_PXPM_TxPower_T{
    int8_t tx_power;     /**< The pxpr tx power status, which is read from PXPR via the TPS service. */
    uint16_t connhandle; /**< The PXPR connect handle, which is used to monitor the Received Signal Strength Indicator (RSSI) of PXPR for link budget calculation.*/
} APP_PXPM_TxPower_T;



typedef enum APP_PXPM_States_T{
    APP_PXP_IND_STA_IDLE,
    APP_PXP_IND_STA_CONNECTING,
    APP_PXP_IND_STA_CONNECTED,
    APP_PXP_IND_STA_WITH_BOND_SCAN,
    APP_PXP_IND_STA_SCAN,
}APP_PXPM_States_T;

typedef struct APP_PXPM_ConnList_T
{
    uint16_t                          connHandle;
    uint8_t                           connIndex;
    bool                              connstatus;
    uint8_t                           connTxPwr;
    BLE_PXPM_AlertLevel_T             connIasLevel;
    bool                              bSecurityStatus;
} APP_PXPM_ConnList_T;

typedef struct APP_CtrlInfo_T{
    APP_PXPM_States_T state;
    uint8_t peerDevId;                  //Peer Device Id which is the pointer to the pairing data in DM(actually in flash)
    bool bAllowNewPairing;
}APP_CtrlInfo_T;

typedef struct APP_ExtPairedDevInfo_T{
    BLE_GAP_Addr_T localAddr;
    bool bConnectedByResolvedAddr;
}APP_ExtPairedDevInfo_T;

/**@addtogroup APP_PXPM_FUNS Functions
 * @{ */

/**
 * @brief Initialize APP Proximity Client.
 *
 */
bool APP_GetPairedDeviceId(uint8_t *p_devId);
uint16_t APP_ScanEnable(APP_PXPM_States_T scanType);
uint16_t APP_CreateConnection(BLE_GAP_Addr_T *peerAddr);
bool APP_GetPairedDeviceId(uint8_t *p_devId);
void APP_RssiMonitorHandler(APP_TIMER_TmrElem_T *timerElem);
void APP_KeyFunction(APP_KEY_MSG_T msg);
APP_PXPM_ConnList_T *APP_PXPM_GetConnListByHandle(uint16_t connHandle);
APP_PXPM_ConnList_T *APP_PXPM_GetFreeConnList(void);
void APP_PXPM_InitConnCharList(uint8_t connIndex);
uint16_t APP_PXPM_SetPathLossReportingParams(BLE_GAP_PathLossReportingParams_T *params);
void APP_SetResolvingList(bool isSet);
void APP_SetFilterAcceptList(bool isSet);

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
extern APP_CtrlInfo_T g_ctrlInfo;

/**@} */ //APP_PXPM_FUNS


#endif /* APP_PXPM_H */
