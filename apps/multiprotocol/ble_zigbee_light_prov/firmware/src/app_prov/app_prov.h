/*******************************************************************************
  Application BLE - Zigbee Provisioning Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_prov.h

  Summary:
    This file contains the BLE Zigbee Provisioning Application functions in 
    this project.

  Description:
    This file contains the Application functions for commissioning a 
    zigbee device via BLE link in this project.
    The implementation of demo mode is included in this file.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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


#ifndef APP_PROV_H
#define APP_PROV_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <pds/include/wlPdsMemIds.h>
#include <bdb/include/bdb.h>
#include <z3device/clusters/include/haClusters.h>
#include <z3device/common/include/z3Device.h>
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define COMBO_PROV

// Define for Prov Ctrl Commands

#define APP_TRP_VENDOR_OPCODE_PROV  0x81
//  Defines Prov Control Command Set APP_TRPS_CTRL_CMD
#define    DEVICE_TYPE_GET_CMD    0x50
#define    SW_VERSION_GET_CMD    0x51    
#define    DEVICE_NAME_SET_CMD    0x52
#define    DEVICE_NAME_GET_CMD    0x53
#define    CHANNEL_MAP_SET_CMD    0x54
#define    CHANNEL_MAP_GET_CMD    0x55
#define    DEVICE_STATE_GET_CMD    0x56
#define    MAC_ID_GET_CMD         0x57
#define    CURRENT_CHANNEL_GET_CMD         0x59
#define    START_COMMISSION_SET_CMD         0x5E
#define    FACTORY_NEW_SET_CMD         0x5F
#define    COMMISION_STATUS_GET_CMD         0x58


//  Defines Prov Response Command Set APP_TRPS_CTRL_RSP
#define    DEVICE_TYPE_GET_RSP    0xA0
#define    SW_VERSION_GET_RSP    0xA1    
#define    DEVICE_NAME_SET_RSP   0xA2
#define    DEVICE_NAME_GET_RSP    0xA3
#define    CHANNEL_MAP_SET_RSP    0xA4
#define    CHANNEL_MAP_GET_RSP    0xA5
#define    DEVICE_STATE_GET_RSP    0xA6
#define    MAC_ID_GET_RSP         0xA7
#define    CURRENT_CHANNEL_GET_RSP         0xA9
#define    START_COMMISSION_SET_RSP         0xAE
#define    FACTORY_NEW_SET_RSP         0xAF
#define    COMMISION_STATUS_GET_RSP         0xA8


//  Defines Prov Response Command length APP_TRPS_CTRL_RSP_LENGTH
#define    DEVICE_TYPE_GET_RSP_LEN    0x2
#define    SW_VERSION_GET_RSP_LEN   0x7    
#define    DEVICE_NAME_SET_RSP_LEN   0x0
#define    DEVICE_NAME_GET_RSP_LEN    16
#define    CHANNEL_MAP_SET_RSP_LEN    0x0
#define    CHANNEL_MAP_GET_RSP_LEN    0x4
#define    DEVICE_STATE_GET_RSP_LEN    0x1
#define    MAC_ID_GET_RSP_LEN         0x8
#define    CURRENT_CHANNEL_GET_RSP_LEN         0x1
#define    START_COMMISSION_SET_RSP_LEN         0x0
#define    FACTORY_NEW_SET_RSP_LEN         0x0
#define    COMMISION_STATUS_GET_RSP_LEN         0x1

//  Defines Prov Notify Command Set APP_TRPS_CTRL_NOTIFY
#define    PROV_STATUS_NFY  0x60


//  Defines Prov Notify Command length APP_TRPS_CTRL_NOTIFY_LENGTH
#define    PROV_STATUS_NFY_LEN   0x1

#define COMBO_STACK_VERSION  "B_0.7.3.5_Z_5.0.0.3"
#define COMBO_APP_VERSION  "1.0.0.0"
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/**@brief The structure contains the information about Prov data */
typedef struct __attribute__ ((packed))
{
    uint8_t deviceType[2];
    uint8_t swVersion[SW_VERSION_GET_RSP_LEN];
    uint8_t deviceName[DEVICE_NAME_GET_RSP_LEN];
    uint8_t macAddr[8];
    uint8_t deviceState;
    uint8_t channelMap[4];
    uint8_t currChannel;
} APP_TRPS_ProvData_T;

extern uint8_t provStatus;
extern bool startComm;
extern bool isDeviceCommissioned;
extern bool isBleConnected;

#define PROV_CMD_RESP_LST_SIZE   12
#define PROV_DEFINE_CTRL_CMD_RESP()                \
        { DEVICE_TYPE_GET_CMD, DEVICE_TYPE_GET_RSP, DEVICE_TYPE_GET_RSP_LEN, (uint8_t *)&provData.deviceType , APP_TRPS_Prov_Get_Cmd},      \
        { SW_VERSION_GET_CMD, SW_VERSION_GET_RSP, SW_VERSION_GET_RSP_LEN, (uint8_t *)&provData.swVersion , APP_TRPS_Prov_Get_Cmd},       \
        { DEVICE_NAME_SET_CMD, DEVICE_NAME_SET_RSP, DEVICE_NAME_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
        { DEVICE_NAME_GET_CMD, DEVICE_NAME_GET_RSP, DEVICE_NAME_GET_RSP_LEN, (uint8_t *)&provData.deviceName , APP_TRPS_Prov_Get_Cmd},   \
        { CHANNEL_MAP_SET_CMD, CHANNEL_MAP_SET_RSP, CHANNEL_MAP_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
        { CHANNEL_MAP_GET_CMD, CHANNEL_MAP_GET_RSP, CHANNEL_MAP_GET_RSP_LEN, (uint8_t *)&provData.channelMap , APP_TRPS_Prov_Get_Cmd},       \
        { DEVICE_STATE_GET_CMD, DEVICE_STATE_GET_RSP, DEVICE_STATE_GET_RSP_LEN, (uint8_t *)&provData.deviceState , APP_TRPS_Prov_Get_Cmd},      \
        { MAC_ID_GET_CMD, MAC_ID_GET_RSP, MAC_ID_GET_RSP_LEN, (uint8_t *)&provData.macAddr , APP_TRPS_Prov_Get_Cmd},     \
        { CURRENT_CHANNEL_GET_CMD, CURRENT_CHANNEL_GET_RSP, CURRENT_CHANNEL_GET_RSP_LEN, (uint8_t *)&provData.currChannel , APP_TRPS_Prov_Get_Cmd},      \
        { START_COMMISSION_SET_CMD, START_COMMISSION_SET_RSP, START_COMMISSION_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},       \
        { FACTORY_NEW_SET_CMD, FACTORY_NEW_SET_RSP, FACTORY_NEW_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
        { COMMISION_STATUS_GET_CMD, COMMISION_STATUS_GET_RSP, COMMISION_STATUS_GET_RSP_LEN, (uint8_t *)&provStatus , NULL}      

#define PROV_NOTIFY_LST_SIZE   1
#define PROV_DEFINE_CTRL_NOTIFY()                   \
        { PROV_STATUS_NFY, PROV_STATUS_NFY_LEN, (uint8_t *)&provStatus},      \


/**@brief Define PDS mem ID needed for Provisioning */
enum{
    PDS_PROV_APP_ITEM_ID_1 = (PDS_MODULE_APP_OFFSET),
    PDS_PROV_APP_ITEM_ID_2,
};    

#define PDS_APP_COMMISION_ID PDS_PROV_APP_ITEM_ID_1
#define PDS_APP_DEVICE_NAME_ID PDS_PROV_APP_ITEM_ID_2
#define PDS_APP_DEVICE_NAME_ITEM_SIZE     17

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_Prov_TRPS_Init(void);

void APP_Prov_TRPS_DiscEvtProc(void);

void BZ_Prov_StartCommissioning(BDB_CommissioningMode_t mode);

bool update_find_n_bind(void);
#endif
