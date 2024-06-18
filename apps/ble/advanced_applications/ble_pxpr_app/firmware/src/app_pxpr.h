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
    app_pxpr.h

  Summary:
    This header file provides functions for the application.

  Description:
    This header file provides functions for the application.
*******************************************************************************/

#ifndef APP_PXPR_H
#define APP_PXPR_H

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
#include "app_led.h"
#include "app_key.h"
#include "ble/profile_ble/ble_pxpr/ble_pxpr.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_ADV_TYPE_ADV            (0x00U)
#define APP_ADV_TYPE_WITH_BOND_ADV  (0x01U)
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************




/** @} */

/**@addtogroup BLE_PXPR_ENUMS Enumerations
 * @{ */

typedef enum
{
    APP_PXPR_STATE_IDLE = 0,
    APP_PXPR_STATE_ADV,
    APP_PXPR_STATE_WITH_BOND_ADV,
    APP_PXPR_STATE_CONN,
} APP_PXPR_States_T;


typedef struct APP_CtrlInfo_T{
    bool bAllowNewPairing;
    uint8_t           peerDevId;
    APP_PXPR_States_T state;
}APP_CtrlInfo_T;


typedef struct APP_PXPR_ConnList_T
{
    uint16_t                connHandle;
    uint8_t                 connIndex;
    bool                    connStatus;
    int               connTxp;
    BLE_PXPR_AlertLevel_T currAlert;
    BLE_PXPR_AlertLevel_T llsAlert;
} APP_PXPR_ConnList_T;

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
extern APP_CtrlInfo_T g_ctrlInfo;
// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_EnableAdv(uint8_t advType);
void APP_ConfigAdv(uint8_t advType);
void APP_SetResolvingList(bool isSet);
void APP_SetFilterAcceptList(bool isSet);
void APP_KeyFunction(APP_KEY_MSG_T msg);
bool APP_GetPairedDeviceId(uint8_t *p_devId);
void APP_InitBleConfig(void);
void APP_PXPR_Init(void);
APP_PXPR_ConnList_T *APP_PXPR_GetConnListByHandle(uint16_t connHandle);
APP_PXPR_ConnList_T *APP_PXPR_GetFreeConnList(void);
void APP_PXPR_InitConnCharList(uint8_t connIndex);


/*******************************************************************************
 End of File
 */

#endif /* APP_MODULE_H */
