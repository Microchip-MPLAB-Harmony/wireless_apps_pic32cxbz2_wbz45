/*******************************************************************************
  Application BLE Provisioning Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_prov.h

  Summary:
    This file contains the BLE Zigbee Provisioning Application functions in 
    this project.

  Description:
    This file contains the Application functions for commissioning the
    device via BLE link in this project.
    The implementation of demo mode is included in this file.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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
//#include <pds/include/wlPdsMemIds.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pds.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define TECH_STACK_TYPE  1

#define    DEVICE_TYPE_LEN         2
#define    SW_VERSION_LEN          7
#define    DEVICE_NAME_LEN         16
#define    DEVICE_STATE_LEN        1
#define    MAC_ID_LEN              8
#define    COMMISION_STATUS_LEN    1
#define    TECH_STACK_LEN          1

/*network parameters*/
#define  CURRENT_CHANNEL_LEN    1
#define  CHANNEL_MASK_LEN       4
#define  PANID_LEN              2
#define  EXT_PANID_LEN          8
#define  NETWORK_NAME_LEN       16
#define  NETWORK_KEY_LEN        16
#define  MESHLOCAL_PREFIX_LEN   8
#define  USER_PARAMS_LEN        16

// Define for Prov Ctrl Commands

#define APP_TRP_VENDOR_OPCODE_PROV  0x81
//  Defines Prov Control Command Set APP_TRPS_CTRL_CMD
#define    DEVICE_TYPE_GET_CMD        0x50//not used
#define    SW_VERSION_GET_CMD         0x51    
#define    DEVICE_NAME_SET_CMD        0x52
#define    DEVICE_NAME_GET_CMD        0x53
#define    CHANNEL_MAP_SET_CMD        0x54//not used
#define    CHANNEL_MAP_GET_CMD        0x55//not used
#define    DEVICE_STATE_GET_CMD       0x56
#define    MAC_ID_GET_CMD             0x57
#define    START_COMMISSION_SET_CMD   0x5E
#define    FACTORY_NEW_SET_CMD        0x5F
#define    COMMISION_STATUS_GET_CMD   0x58
#define    TECH_STACK_GET_CMD         0x70

/* Network parameters*/
#define  CURRENT_CHANNEL_GET_CMD    0x59
#define  CURRENT_CHANNEL_SET_CMD    0x5A
#define  CHANNEL_MASK_GET_CMD       0x71
#define  CHANNEL_MASK_SET_CMD       0x72
#define  PANID_GET_CMD              0x73
#define  PANID_SET_CMD              0x74
#define  EXT_PANID_GET_CMD          0x75
#define  EXT_PANID_SET_CMD          0x76
#define  NETWORK_NAME_GET_CMD       0x77
#define  NETWORK_NAME_SET_CMD       0x78
#define  NETWORK_KEY_GET_CMD        0x79
#define  NETWORK_KEY_SET_CMD        0x7A
#define  MESHLOCAL_PREFIX_GET_CMD   0x7B
#define  MESHLOCAL_PREFIX_SET_CMD   0x7C
#define  USER_PARAMS_GET_CMD        0x7D
#define  USER_PARAMS_SET_CMD        0x7E



//  Defines Prov Response Command Set APP_TRPS_CTRL_RSP
#define    DEVICE_TYPE_GET_RSP        0xA0 //Not used
#define    SW_VERSION_GET_RSP         0xA1    
#define    DEVICE_NAME_SET_RSP        0xA2
#define    DEVICE_NAME_GET_RSP        0xA3
#define    CHANNEL_MAP_SET_RSP        0xA4 //Not used
#define    CHANNEL_MAP_GET_RSP        0xA5 //Not used
#define    DEVICE_STATE_GET_RSP       0xA6
#define    MAC_ID_GET_RSP             0xA7
#define    START_COMMISSION_SET_RSP   0xAE
#define    FACTORY_NEW_SET_RSP        0xAF
#define    COMMISION_STATUS_GET_RSP   0xA8
#define    TECH_STACK_GET_RSP         0xC0

/* Network parameters*/
#define  CURRENT_CHANNEL_GET_RSP    0xA9
#define  CURRENT_CHANNEL_SET_RSP    0xAA
#define  CHANNEL_MASK_GET_RSP       0xC1
#define  CHANNEL_MASK_SET_RSP       0xC2
#define  PANID_GET_RSP              0xC3
#define  PANID_SET_RSP              0xC4
#define  EXT_PANID_GET_RSP          0xC5
#define  EXT_PANID_SET_RSP          0xC6
#define  NETWORK_NAME_GET_RSP       0xC7
#define  NETWORK_NAME_SET_RSP       0xC8
#define  NETWORK_KEY_GET_RSP        0xC9
#define  NETWORK_KEY_SET_RSP        0xCA
#define  MESHLOCAL_PREFIX_GET_RSP   0xCB
#define  MESHLOCAL_PREFIX_SET_RSP   0xCC
#define  USER_PARAMS_GET_RSP        0xCD
#define  USER_PARAMS_SET_RSP        0xCE


//  Defines Prov Response Command length APP_TRPS_CTRL_RSP_LENGTH
#define    DEVICE_TYPE_GET_RSP_LEN         DEVICE_TYPE_LEN
#define    SW_VERSION_GET_RSP_LEN          SW_VERSION_LEN    
#define    DEVICE_NAME_SET_RSP_LEN         0
#define    DEVICE_NAME_GET_RSP_LEN         DEVICE_NAME_LEN
#define    DEVICE_STATE_GET_RSP_LEN        DEVICE_STATE_LEN
#define    MAC_ID_GET_RSP_LEN              MAC_ID_LEN
#define    START_COMMISSION_SET_RSP_LEN    0
#define    FACTORY_NEW_SET_RSP_LEN         0
#define    COMMISION_STATUS_GET_RSP_LEN    COMMISION_STATUS_LEN
#define    TECH_STACK_GET_RSP_LEN          TECH_STACK_LEN

/*network parameters*/
#define  CURRENT_CHANNEL_GET_RSP_LEN    CURRENT_CHANNEL_LEN
#define  CURRENT_CHANNEL_SET_RSP_LEN    0
#define  CHANNEL_MASK_GET_RSP_LEN       CHANNEL_MASK_LEN
#define  CHANNEL_MASK_SET_RSP_LEN       0
#define  PANID_GET_RSP_LEN              PANID_LEN
#define  PANID_SET_RSP_LEN              0
#define  EXT_PANID_GET_RSP_LEN          EXT_PANID_LEN
#define  EXT_PANID_SET_RSP_LEN          0
#define  NETWORK_NAME_GET_RSP_LEN       NETWORK_NAME_LEN
#define  NETWORK_NAME_SET_RSP_LEN       0
#define  NETWORK_KEY_GET_RSP_LEN        NETWORK_KEY_LEN
#define  NETWORK_KEY_SET_RSP_LEN        0
#define  MESHLOCAL_PREFIX_GET_RSP_LEN   MESHLOCAL_PREFIX_LEN
#define  MESHLOCAL_PREFIX_SET_RSP_LEN   0
#define  USER_PARAMS_GET_RSP_LEN        USER_PARAMS_LEN
#define  USER_PARAMS_SET_RSP_LEN        0

//  Defines Prov Notify Command Set APP_TRPS_CTRL_NOTIFY
#define    PROV_STATUS_NFY  0x60


//  Defines Prov Notify Command length APP_TRPS_CTRL_NOTIFY_LENGTH
#define    PROV_STATUS_NFY_LEN   0x1

#define COMBO_APP_VERSION  "1.0.0.0"

#define CURRENT_CHANNEL    24
#define PANID              {0x12,0x34}
#define CHANNEL_MASK       {0x07,0xff,0xf8,0x00}
#define EXD_PANID          {0x00,0x01,0x02,0x03,0x4,0x05,0x06,0x07}
#define NWK_NAME           "MCHP_THREAD" // max charin nwk name <= 16
#define NW_KEY             {0x11,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff}
#define ML_PREFIX          {0xfd,0x00,0x00,0x00,0xfb,0x01,0x00,0x01}
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/**@brief The structure contains the information about Prov data */

typedef struct __attribute__ ((packed))
{
    uint8_t channel;
    uint32_t channelMask;
    uint16_t panId;
    uint8_t extPanId[EXT_PANID_LEN];
    int8_t networkName[NETWORK_NAME_LEN+1];
    uint8_t networkKey[NETWORK_KEY_LEN];
    uint8_t meshLocalPrefix[MESHLOCAL_PREFIX_LEN];
    uint8_t userParam[USER_PARAMS_LEN];
} APP_ProvNwData_T;

typedef struct __attribute__ ((packed))
{
    uint8_t deviceType[DEVICE_TYPE_LEN];
    uint8_t swVersion[SW_VERSION_LEN];
    uint8_t deviceName[DEVICE_NAME_LEN];
    uint8_t macAddr[MAC_ID_LEN];
    uint8_t deviceState;
    uint8_t devTechStack;
    uint32_t channelMaskDummy;
    uint16_t panIdDummy;
    APP_ProvNwData_T provNwData;
} APP_ProvData_T;

extern bool isDeviceCommissioned;

#define PROV_CMD_RESP_LST_SIZE   25
#define PROV_DEFINE_CTRL_CMD_RESP()                \
        { TECH_STACK_GET_CMD, TECH_STACK_GET_RSP, TECH_STACK_GET_RSP_LEN, (uint8_t *)&provData.devTechStack , APP_TRPS_Prov_Get_Cmd},      \
        { SW_VERSION_GET_CMD, SW_VERSION_GET_RSP, SW_VERSION_GET_RSP_LEN, (uint8_t *)&provData.swVersion , APP_TRPS_Prov_Get_Cmd},       \
        { DEVICE_NAME_SET_CMD, DEVICE_NAME_SET_RSP, DEVICE_NAME_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
        { DEVICE_NAME_GET_CMD, DEVICE_NAME_GET_RSP, DEVICE_NAME_GET_RSP_LEN, (uint8_t *)&provData.deviceName , APP_TRPS_Prov_Get_Cmd},   \
        { DEVICE_STATE_GET_CMD, DEVICE_STATE_GET_RSP, DEVICE_STATE_GET_RSP_LEN, (uint8_t *)&provData.deviceState , APP_TRPS_Prov_Get_Cmd},      \
        { MAC_ID_GET_CMD, MAC_ID_GET_RSP, MAC_ID_GET_RSP_LEN, (uint8_t *)&provData.macAddr , APP_TRPS_Prov_Get_Cmd},     \
        { START_COMMISSION_SET_CMD, START_COMMISSION_SET_RSP, START_COMMISSION_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},       \
        { FACTORY_NEW_SET_CMD, FACTORY_NEW_SET_RSP, FACTORY_NEW_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
        { COMMISION_STATUS_GET_CMD, COMMISION_STATUS_GET_RSP, COMMISION_STATUS_GET_RSP_LEN, (uint8_t *)&provStatus , NULL},        \
        { CURRENT_CHANNEL_GET_CMD, CURRENT_CHANNEL_GET_RSP, CURRENT_CHANNEL_GET_RSP_LEN, (uint8_t *)&provData.provNwData.channel , APP_TRPS_Prov_Get_Cmd},      \
		{ CURRENT_CHANNEL_SET_CMD, CURRENT_CHANNEL_SET_RSP, CURRENT_CHANNEL_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
		{ CHANNEL_MASK_GET_CMD, CHANNEL_MASK_GET_RSP, CHANNEL_MASK_GET_RSP_LEN, (uint8_t *)&provData.channelMaskDummy , APP_TRPS_Prov_Get_Cmd},      \
	    { CHANNEL_MASK_SET_CMD, CHANNEL_MASK_SET_RSP, CHANNEL_MASK_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
		{ PANID_GET_CMD, PANID_GET_RSP, PANID_GET_RSP_LEN, (uint8_t *)&provData.panIdDummy , APP_TRPS_Prov_Get_Cmd},      \
	    { PANID_SET_CMD, PANID_SET_RSP, PANID_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
		{ EXT_PANID_GET_CMD, EXT_PANID_GET_RSP, EXT_PANID_GET_RSP_LEN, (uint8_t *)&provData.provNwData.extPanId , APP_TRPS_Prov_Get_Cmd},      \
	    { EXT_PANID_SET_CMD, EXT_PANID_SET_RSP, EXT_PANID_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
		{ NETWORK_NAME_GET_CMD, NETWORK_NAME_GET_RSP, NETWORK_NAME_GET_RSP_LEN, (uint8_t *)&provData.provNwData.networkName , APP_TRPS_Prov_Get_Cmd},      \
	    { NETWORK_NAME_SET_CMD, NETWORK_NAME_SET_RSP, NETWORK_NAME_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
		{ NETWORK_KEY_GET_CMD, NETWORK_KEY_GET_RSP, NETWORK_KEY_GET_RSP_LEN, (uint8_t *)&provData.provNwData.networkKey , APP_TRPS_Prov_Get_Cmd},      \
	    { NETWORK_KEY_SET_CMD, NETWORK_KEY_SET_RSP, NETWORK_KEY_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
		{ MESHLOCAL_PREFIX_GET_CMD, MESHLOCAL_PREFIX_GET_RSP, MESHLOCAL_PREFIX_GET_RSP_LEN, (uint8_t *)&provData.provNwData.meshLocalPrefix , APP_TRPS_Prov_Get_Cmd},      \
	    { MESHLOCAL_PREFIX_SET_CMD, MESHLOCAL_PREFIX_SET_RSP, MESHLOCAL_PREFIX_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \
		{ USER_PARAMS_GET_CMD, USER_PARAMS_GET_RSP, USER_PARAMS_GET_RSP_LEN, (uint8_t *)&provData.provNwData.userParam , APP_TRPS_Prov_Get_Cmd},      \
	    { USER_PARAMS_SET_CMD, USER_PARAMS_SET_RSP, USER_PARAMS_SET_RSP_LEN, NULL , APP_TRPS_Prov_Set_Cmd},      \

#define PROV_NOTIFY_LST_SIZE   1
#define PROV_DEFINE_CTRL_NOTIFY()                   \
        { PROV_STATUS_NFY, PROV_STATUS_NFY_LEN, (uint8_t *)&provStatus},      \


/**@brief Define PDS mem ID needed for Provisioning */
enum{
    PDS_PROV_APP_ITEM_ID_1 = (PDS_MODULE_APP_OFFSET),
    PDS_PROV_APP_ITEM_ID_2,
	PDS_PROV_APP_ITEM_ID_3
};    

#define PDS_APP_COMMISION_ID              PDS_PROV_APP_ITEM_ID_1
#define PDS_APP_DEVICE_NAME_ID            PDS_PROV_APP_ITEM_ID_2
#define PDS_APP_COMMISSION_DATA_ID        PDS_PROV_APP_ITEM_ID_3
#define PDS_APP_DEVICE_NAME_ITEM_SIZE     17

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
void APP_Prov_TRPS_Init(void);

void APP_Prov_TRPS_DiscEvtProc(void);

void APP_Prov_TRPS_CommissionOnReset(void);

void APP_Prov_TRPS_ProvComplete(bool commStatus);

void APP_Prov_StartCommissioning(APP_ProvNwData_T *provNwData);

void APP_Prov_ResetToFactoryNew(void);

#endif
