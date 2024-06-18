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
    app_anps.h

  Summary:
    This header file provides functions for the application.

  Description:
    This header file provides functions for the application.
*******************************************************************************/

#ifndef APP_ANPS_H
#define APP_ANPS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "mba_error_defs.h"
#include "gap_defs.h"
#include "ble_gap.h"
#include "gatt.h"
#include "app_key.h"
#include "ble_anps/ble_anps.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
typedef enum APP_ANPS_States_T
{
    APP_ANPS_STATE_IDLE=0,
    APP_ANPS_STATE_CONNECTING,
    APP_ANPS_STATE_CONNECTED,
    APP_ANPS_STATE_WITH_BOND_SCAN,
    APP_ANPS_STATE_SCAN,
} APP_ANPS_States_T;

typedef struct APP_CtrlInfo_T{
    bool              bAllowNewPairing;
    uint8_t           peerDevId;
    APP_ANPS_States_T state;
}APP_CtrlInfo_T;

typedef enum APP_ANPS_NTFY_StatesT
{
    APP_ANPS_NTFY_NEW_ALERT,
    APP_ANPS_NTFY_UNREAD_ALERT,
} APP_ANPS_NTFY_StatesT;

typedef struct APP_ANPS_ConnList_T
{
    bool                  bConnStatus;
    bool                  bSecurityStatus;
    uint8_t               connIndex;
    uint16_t              connHandle;
    APP_ANPS_NTFY_StatesT ntfyInfo;
} APP_ANPS_ConnList_T;
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
extern APP_CtrlInfo_T  g_anpsCtrlInfo;
// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
bool APP_GetPairedDeviceId(uint8_t *p_devId);
uint16_t APP_CreateConnection(const BLE_GAP_Addr_T *peerAddr);
uint16_t APP_ScanEnable(APP_ANPS_States_T scanType);
APP_ANPS_ConnList_T *APP_AnpsGetConnListByHandle(uint16_t connHandle);
APP_ANPS_ConnList_T *APP_AnpsGetFreeConnList(void);
void APP_SetResolvingList(bool isSet);
void APP_SetFilterAcceptList(bool isSet);
void APP_KeyFunction(APP_KEY_MSG_T msg);
void APP_InitBleConfig(void);
void APP_AnpsInitConnList(uint8_t connIndex);
/*******************************************************************************
 End of File
 */

#endif /* APP_MODULE_H */
