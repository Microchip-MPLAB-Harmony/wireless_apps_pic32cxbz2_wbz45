/*******************************************************************************
  Application Transparent Server Role Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_trps.h

  Summary:
    This file contains the Application Transparent Server Role functions for this project.

  Description:
    This file contains the Application Transparent Server Role functions for this project.
    The implementation of demo mode is included in this file.
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


#ifndef APP_TRPS_H
#define APP_TRPS_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "ble_gap.h"
#include "ble_trsps/ble_trsps.h"
#include "app_ble_handler.h"

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
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_TRPS_Init(void);
void APP_TRPS_ConnEvtProc(BLE_GAP_Event_T *p_event);
void APP_TRPS_DiscEvtProc(uint16_t connHandle);
void APP_TRPS_TxBufValidEvtProc(void);
#ifdef APP_BLE_MULTI_LINK_ENABLE
void APP_TRPS_LeRxMultiLinkProc(APP_TRP_ConnList_T *p_connList_t);
APP_TRP_ConnList_T *APP_TRPS_ChangeNextLink(APP_TRP_LINK_TYPE_T linkType, 
    APP_TRP_TrafficPriority_T *p_connToken);
#endif
void APP_TRPS_UpdateMtuEvtProc(uint16_t connHandle, uint16_t exchangedMTU);
void APP_TRPS_LeRxProc(APP_TRP_ConnList_T *p_connList_t);
void APP_TRPS_EventHandler(BLE_TRSPS_Event_T *p_event);
void APP_TRPS_TrcbpsEventHandler(BLE_TRCBPS_Event_T *p_event);
// uint8_t APP_TRPS_GetWorkMode(void);
bool APP_TRPS_IsUartMode(void);
APP_TRP_ConnList_T *APP_TRPS_GetConnListByUartMode(void);
void APP_TRPS_SendUpConnParaStatusToClient(uint16_t connHandle, uint8_t upConnParaStatus);
void APP_TRPS_KeyTrigger(uint8_t keyMesg);
#endif
