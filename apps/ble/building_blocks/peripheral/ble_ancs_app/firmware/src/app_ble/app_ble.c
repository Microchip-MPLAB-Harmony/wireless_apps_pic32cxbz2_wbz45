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
  Application BLE Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble.c

  Summary:
    This file contains the Application BLE implementation for this project.

  Description:
    This file contains the Application BLE implementation for this project.
 *******************************************************************************/


#include "app.h"
#include "osal/osal_freertos_extend.h"
#include "app_ble.h"
#include "app_ble_handler.h"

#include "ble_dm/ble_dm.h"
#include "peripheral/gpio/plib_gpio.h"
#include "app_led.h"











#include "app_ancs_handler.h"
#include "driver/pds/include/pds.h"
#include "driver/pds/include/pds_config.h"
#include "app_module.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define GAP_DEV_NAME_VALUE          "M-dev"

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
static BLE_DD_Config_T         ddConfig;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

static void APP_BleStackCb(STACK_Event_T *p_stack)
{
    STACK_Event_T stackEvent;
    APP_Msg_T   appMsg;
    APP_Msg_T   *p_appMsg;

    (void)memcpy((uint8_t *)&stackEvent, (uint8_t *)p_stack, sizeof(STACK_Event_T));
    stackEvent.p_event=OSAL_Malloc(p_stack->evtLen);
    if(stackEvent.p_event==NULL)
    {
        return;
    }
    (void)memcpy(stackEvent.p_event, p_stack->p_event, p_stack->evtLen);
    stackEvent.p_event=stackEvent.p_event;

    if (p_stack->groupId==STACK_GRP_GATT)
    {
        GATT_Event_T *p_evtGatt = (GATT_Event_T *)stackEvent.p_event;

        if (p_evtGatt->eventId == GATTS_EVT_CLIENT_CCCDLIST_CHANGE)
        {
            uint8_t *p_payload;

            p_payload = (uint8_t *)OSAL_Malloc((p_evtGatt->eventField.onClientCccdListChange.numOfCccd*4));
            if (p_payload != NULL)
            {
                (void)memcpy(p_payload, (uint8_t *)p_evtGatt->eventField.onClientCccdListChange.p_cccdList, (p_evtGatt->eventField.onClientCccdListChange.numOfCccd*4));
                p_evtGatt->eventField.onClientCccdListChange.p_cccdList = (GATTS_CccdList_T *)p_payload;
            }
        }
    }

    appMsg.msgId=APP_MSG_BLE_STACK_EVT;

    ((STACK_Event_T *)appMsg.msgData)->groupId=p_stack->groupId;
    ((STACK_Event_T *)appMsg.msgData)->evtLen=p_stack->evtLen;
    ((STACK_Event_T *)appMsg.msgData)->p_event=stackEvent.p_event;

    p_appMsg = &appMsg;
    OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0);
}

void APP_BleStackEvtHandler(STACK_Event_T *p_stackEvt)
{
    switch(p_stackEvt->groupId)
    {
        case STACK_GRP_BLE_GAP:
        {
            APP_BleGapEvtHandler((BLE_GAP_Event_T *)p_stackEvt->p_event);
        }
        break;
        
        case STACK_GRP_BLE_L2CAP:
        {
            APP_BleL2capEvtHandler((BLE_L2CAP_Event_T *)p_stackEvt->p_event);
         }
        break;
        case STACK_GRP_BLE_SMP:
        {
            APP_BleSmpEvtHandler((BLE_SMP_Event_T *)p_stackEvt->p_event);
         }
        break;

        case STACK_GRP_GATT:
        {
            APP_GattEvtHandler((GATT_Event_T *)p_stackEvt->p_event);
        }
        break;
        
        default:
        break;

    }

    //Direct event to BLE middleware
    BLE_DM_BleEventHandler(p_stackEvt);

    BLE_DD_BleEventHandler(&ddConfig, p_stackEvt);

    //Direct event to BLE profiles

    








    BLE_ANCS_BleEventHandler(p_stackEvt);

    OSAL_Free(p_stackEvt->p_event);
}


void APP_BleStackLogHandler(BT_SYS_LogEvent_T *p_logEvt)
{
}

static void APP_DdEvtHandler(BLE_DD_Event_T *p_event)
{



    BLE_ANCS_BleDdEventHandler(p_event);
}



static void APP_BleConfigBasic(void)
{
    int8_t                          connTxPower;
    int8_t                          advTxPower;
    BLE_GAP_AdvParams_T             advParam;
    uint8_t advData[]={
            //bit1: LE General Discoverable Mode, bit 2: BR/EDR Not Supported.
            0x02, 0x01, 0x06,
            //local name: "M-dev"
            0x06, 0x09, 0x4D, 0x2D, 0x64, 0x65, 0x76,
            //Length(0x11: 17), 0x15:<<List of 128-bit Service Solicitation UUIDs>>, 128-bit ANCS UUID
            0x11, 0x15, 0xD0, 0x00, 0x2D, 0x12, 0x1E, 0x4B, 0x0F, 0xA4, 0x99, 0x4E, 0xCE, 0xB5, 0x31, 0xF4, 0x05, 0x79};
    BLE_GAP_AdvDataParams_T         appAdvData;
    uint8_t scanRspData[]={
            //Length(0x02: 2), 0x0A:adv TxPower, 0x09
            0x02, 0x0A, 0x09};
    BLE_GAP_AdvDataParams_T         appScanRspData;

    //Check if paired device exists, load the info of paired device from pds.
    memset(&g_extPairedDevInfo, 0, sizeof(APP_ExtPairedDevInfo_T));
    APP_GetExtPairedDevInfoFromFlash(&g_extPairedDevInfo);
    memset(&g_pairedDevInfo.addr, 0, sizeof(BLE_GAP_Addr_T));
    g_pairedDevInfo.bPeerDevIdExist=APP_GetPairedDeviceId(&g_pairedDevInfo.peerDevId);
    if (g_pairedDevInfo.bPeerDevIdExist)
    {
        g_pairedDevInfo.bAddrLoaded =APP_GetPairedDeviceAddr(&g_pairedDevInfo.addr);
    }
    else
    {
        g_pairedDevInfo.bAddrLoaded =false;
    }
    g_pairedDevInfo.bPaired=g_pairedDevInfo.bAddrLoaded;

    // Configure advertising parameters
    BLE_GAP_SetAdvTxPowerLevel(9,&advTxPower);      /* Advertising TX Power */
    
    (void)memset(&advParam, 0, sizeof(BLE_GAP_AdvParams_T));
    advParam.intervalMin = 32;     /* Advertising Interval Min */
    advParam.intervalMax = 32;     /* Advertising Interval Max */
    //Windows/ Android/ iOS support the reconnection using ADV_IND. So using ADV_IND for pairing and reconnection.
    advParam.type = BLE_GAP_ADV_TYPE_ADV_IND;        /* Advertising Type */
    advParam.advChannelMap = BLE_GAP_ADV_CHANNEL_ALL;        /* Advertising Channel Map */
    if (g_pairedDevInfo.bPaired)//Paired already
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_SCAN_CONNECT;     /* Advertising Filter Policy */
    }
    else
    {
        advParam.filterPolicy = BLE_GAP_ADV_FILTER_DEFAULT;     /* Advertising Filter Policy */
    }
    BLE_GAP_SetAdvParams(&advParam);

    // Configure advertising data
    appAdvData.advLen=sizeof(advData);
    (void)memcpy(appAdvData.advData, advData, appAdvData.advLen);     /* Advertising Data */
    BLE_GAP_SetAdvData(&appAdvData);

    //Configure advertising scan response data
    appScanRspData.advLen=sizeof(scanRspData);
    (void)memcpy(appScanRspData.advData, scanRspData, appScanRspData.advLen);     /* Scan Response Data */
    BLE_GAP_SetScanRspData(&appScanRspData);

    BLE_GAP_SetConnTxPowerLevel(15, &connTxPower);      /* Connection TX Power */
}
static void APP_BleConfigAdvance(void)
{
    uint8_t devName[]={GAP_DEV_NAME_VALUE};

    BLE_SMP_Config_T                smpParam;


    BLE_DM_Config_T                 dmConfig;
    BLE_GAP_ServiceOption_T         gapServiceOptions;
    

    // Configure Device Name
    BLE_GAP_SetDeviceName(sizeof(devName), devName);    /* Device Name */

    //Configure Device Address-Random Static Address and local IRK
    if (!g_pairedDevInfo.bPaired)//Not paired yet
    {
        //Set a new IRK
        APP_SetLocalIRK();

        APP_GenerateRandomStaticAddress(&g_extPairedDevInfo.localAddr);
    }
    BLE_GAP_SetDeviceAddr(&g_extPairedDevInfo.localAddr);

    // GAP Service option
    gapServiceOptions.charDeviceName.enableWriteProperty = false;             /* Enable Device Name Write Property */
    gapServiceOptions.charAppearance.appearance = 0x0;                          /* Appearance */
    gapServiceOptions.charPeriPreferConnParam.enable = false;                    /* Enable Peripheral Preferred Connection Parameters */

    BLE_GAP_ConfigureBuildInService(&gapServiceOptions);




    // Configure SMP parameters
    (void)memset(&smpParam, 0, sizeof(BLE_SMP_Config_T));
    smpParam.ioCapability = BLE_SMP_IO_NOINPUTNOOUTPUT;                  /* IO Capability */
    smpParam.authReqFlag |= BLE_SMP_OPTION_BONDING;             /* Authentication Setting: Bonding */
    smpParam.authReqFlag |= BLE_SMP_OPTION_SECURE_CONNECTION;   /* Authentication Setting: Secure Connections */
    BLE_SMP_Config(&smpParam);

    // Configure BLE_DM middleware parameters
    dmConfig.secAutoAccept = true;                          /* Auto Accept Security Request */
    dmConfig.connConfig.autoReplyUpdateRequest = true;      /* Auto Accept Connection Parameter Update Request */
    dmConfig.connConfig.minAcceptConnInterval = 6;    /* Minimum Connection Interval */
    dmConfig.connConfig.maxAcceptConnInterval = 3200;    /* Maximum Connection Interval */
    dmConfig.connConfig.minAcceptPeripheralLatency = 0;    /* Minimum Connection Latency */
    dmConfig.connConfig.maxAcceptPeripheralLatency = 499;    /* Maximum Connection Latency */
    BLE_DM_Config(&dmConfig);


    // Configure BLE_DD middleware parameters
    ddConfig.waitForSecurity = false;
    ddConfig.initDiscInCentral = false;
    ddConfig.initDiscInPeripheral = true;
    //ddConfig.disableConnectedDisc = false; //not generated automatically by MCC, manually added

    //If paired device exists, set resolving list
    if (g_pairedDevInfo.bPaired)//Paired already
    {
        APP_SetFilterAcceptList(true);
        if (g_extPairedDevInfo.bConnectedByResolvedAddr)
        {
            APP_SetResolvingList(true);
        }
    }
    APP_RegisterPdsCb();
}

void APP_BleStackInitBasic(void)
{
    BLE_GAP_Init();

    BLE_GAP_AdvInit();  /* Advertising */

    BLE_GAP_ConnPeripheralInit();   /* Peripheral */
}

void APP_BleStackInitAdvance(void)
{
    uint16_t gattsInitParam=GATTS_CONFIG_NONE;

    uint16_t gattcInitParam=GATTC_CONFIG_NONE;

    STACK_EventRegister(APP_BleStackCb);





    BLE_L2CAP_Init();

    GATTS_Init(gattsInitParam);

    GATTC_Init(gattcInitParam);     /* Enable Client Role */

    BLE_SMP_Init();


    //Initialize BLE middleware
    BLE_DM_Init();
    BLE_DM_EventRegister(APP_DmEvtHandler);

    BLE_DD_Init();
    BLE_DD_EventRegister(APP_DdEvtHandler);
    


    //Initialize BLE services

    //Initialize BLE profiles










    BLE_ANCS_Init();
    BLE_ANCS_EventRegister(APP_AncsEvtHandler);


    APP_BleConfigAdvance();
}

void APP_BleStackInit(void)
{
    APP_BleStackInitBasic();
    APP_BleConfigBasic();
    APP_BleStackInitAdvance();
}
