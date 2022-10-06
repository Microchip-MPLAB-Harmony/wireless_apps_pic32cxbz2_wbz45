/*******************************************************************************
  Application BLE - Zigbee Provisioning Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_prov.c

  Summary:
    This file contains the BLE Zigbee Provisioning Application functions 
    in this project.

  Description:
    This file contains the Application functions for commissioning a 
    zigbee device via BLE link in this project.
    The implementation of demo mode is included in this file.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include "app_prov.h"
#include "app_trps/app_trps.h"
#include <configserver/include/configserver.h>
#include <z3device/stack_interface/bdb/include/bdb_api.h>
#include <app_zigbee/zigbee_console/console.h>
#include "../app_ble_conn_handler.h"


// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
char commission_device_name[PDS_APP_DEVICE_NAME_ITEM_SIZE];
bool isDeviceCommissioned = false;
bool isBleConnected = false;
static BDB_InitiatorReq_t BZ_Prov_InitiatorReq;
BDB_InvokeCommissioningReq_t BZ_Prov_bdbCommissioningreq;
PDS_DECLARE_ITEM(PDS_APP_DEVICE_NAME_ID, PDS_APP_DEVICE_NAME_ITEM_SIZE, commission_device_name, NULL, SIZE_MODIFICATION_ALLOWED);
PDS_DECLARE_ITEM(PDS_APP_COMMISION_ID, sizeof(isDeviceCommissioned), &isDeviceCommissioned, NULL, NO_ITEM_FLAGS);
static void NetworkFormationCompleted(BDB_CommissioningStatus_t status);
static void NetworkSteeringCompleted(BDB_CommissioningStatus_t status);
BdbEventSubscriber_t s_bdb_EventsCallback =
{
  .networkFormationCompleted = NetworkFormationCompleted,
  .SteeringCompleted = NetworkSteeringCompleted,  
};

void App_SetCommissioningStatus(bool val);


// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
APP_TRPS_ProvData_T provData;
bool startComm = false;
uint8_t provStatus = 0; 
static uint8_t bIndex;
BDB_CommissioningMode_t commissioningMode;

static uint8_t joinInterval = APP_JOIN_ATTEMPT_INTERVAL;
static uint8_t joinAttempts = APP_JOIN_ATTEMPTS_AMOUNT;
static void joinRetryTimerFired(void);

static HAL_AppTimer_t joinRetryTimer =
{
  .interval = APP_JOIN_ATTEMPT_INTERVAL* 1000,
  .mode     = TIMER_ONE_SHOT_MODE,
  .callback = joinRetryTimerFired,
};

static void bzProvNetworkEventsHandler(SYS_EventId_t eventId, SYS_EventData_t data);
static SYS_EventReceiver_t bzProvNetworkEventsListener  = { .func = bzProvNetworkEventsHandler};  
// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
 static uint8_t APP_TRPS_Prov_Get_Cmd(uint8_t *p_cmd);
 static uint8_t APP_TRPS_Prov_Set_Cmd(uint8_t *p_cmd);


APP_TRPS_CmdResp_T appTrpsProvCmdResp[] = 
{
    PROV_DEFINE_CTRL_CMD_RESP()
};

APP_TRPS_NotifyData_T appTrpsProvNotify[] = 
{
    PROV_DEFINE_CTRL_NOTIFY()
};

static void NetworkFormationCompleted(BDB_CommissioningStatus_t status)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_COMMISSIONING_FORMATION_COMPLETE;
  event.eventData.value = status;
  APP_Zigbee_Handler(event);
}

static void NetworkSteeringCompleted(BDB_CommissioningStatus_t status)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_ZIGBEE;
  event.eventId = EVENT_COMMISSIONING_STEERING_COMPLETE;
  event.eventData.value = status;
  APP_Zigbee_Handler(event);
}

static void bzProvNetworkEventsHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
    switch(eventId)
    {
        case BC_EVENT_NETWORK_LEFT:
        {
            APP_Zigbee_Event_t event;
            startComm = true;
            provStatus = 0;
            
            commissioningMode = BDB_COMMISSIONING_NWK_STEERING;
            BZ_Prov_StartCommissioning(commissioningMode);

            event.eventGroup = EVENT_ZIGBEE;
            event.eventId = EVENT_LEFT_FROM_NETWORK;
            APP_Zigbee_Handler(event);
        }
        break;
    }
}
/**************************************************************************
\brief To Handle steeringfailure from application
***************************************************************************/

static void joinRetryTimerFired(void)
{
    if ( joinAttempts && (joinAttempts !=  0xff))
    {
        joinAttempts--;
    }
    BZ_Prov_StartCommissioning(commissioningMode);
}

static void handleSteeringFailure(void)
{
    APP_Zigbee_Event_t event;
    if (commissioningMode  == BDB_COMMISSIONING_NWK_STEERING)    
    {
        if (joinInterval && joinAttempts)
        {     
            HAL_StartAppTimer(&joinRetryTimer);
        }
        else
        {
            HAL_StopAppTimer(&joinRetryTimer);
            joinAttempts = APP_JOIN_ATTEMPTS_AMOUNT;
            
#if (APP_DEVICE_TYPE != DEV_TYPE_ENDDEVICE)           
            commissioningMode = BDB_COMMISSIONING_NWK_FORMATION;
            BZ_Prov_StartCommissioning(commissioningMode);
#else
            event.eventGroup = EVENT_ZIGBEE;
            event.eventId = EVENT_COMMISSIONING_FAILURE;
            event.eventData.value = BDB_COMMISSIONING_NO_NETWORK;
            APP_Zigbee_Handler(event);
#endif            
        }
    }
}

static void BZ_Prov_Callback_Commissioning(BDB_InvokeCommissioningConf_t *conf)
{
    if (conf->status == BDB_COMMISSIONING_NO_NETWORK )
    {
        handleSteeringFailure();
    }
    else if (conf->status == BDB_COMMISSIONING_SUCCESS && (commissioningMode != BDB_COMMISSIONING_FINDING_BINDING) )//&& commissioningMode.nwkSteering)
    {
        update_find_n_bind();
    }
    else if (commissioningMode == BDB_COMMISSIONING_FINDING_BINDING)
    {
        APP_Zigbee_Event_t event;
        event.eventGroup = EVENT_ZIGBEE;
        event.eventId = EVENT_COMMISSIONING_FINDBIND_COMPLETE;
        event.eventData.value = conf->status; //BDB_COMMISSIONING_IN_PROGRESS;
        App_SetCommissioningStatus(false);
        APP_Zigbee_Handler(event);
    }
}


bool update_find_n_bind(void)
{
    if (bIndex < APP_ENDPOINTS_AMOUNT)
    {
        commissioningMode = BDB_COMMISSIONING_FINDING_BINDING;
        BZ_Prov_StartCommissioning(commissioningMode);
        return true;
    }
    else
    {
        return false;
    }
}

void BZ_Prov_StartCommissioning(BDB_CommissioningMode_t mode)
{
    memset(&BZ_Prov_bdbCommissioningreq,0,sizeof(BDB_InvokeCommissioningReq_t));
    BZ_Prov_bdbCommissioningreq.mode = mode;
    if(BZ_Prov_bdbCommissioningreq.mode == BDB_COMMISSIONING_FINDING_BINDING)
    {
        AppBindReq_t *currBindReq = deviceBindReqs[bIndex++];
        
        BZ_Prov_InitiatorReq.initiatorEndpoint = currBindReq->srcEndpoint;
        BZ_Prov_InitiatorReq.clientClustersCount = currBindReq->remoteServersCnt;
        BZ_Prov_InitiatorReq.serverClustersCount = currBindReq->remoteClientsCnt;

        BZ_Prov_InitiatorReq.clientClustersList = currBindReq->remoteServers;
        BZ_Prov_InitiatorReq.serverClustersList = currBindReq->remoteClients;

        BZ_Prov_InitiatorReq.callback = currBindReq->callback;
        BZ_Prov_InitiatorReq.groupId = currBindReq->groupId;

        BZ_Prov_bdbCommissioningreq.initiatorReq = &BZ_Prov_InitiatorReq;
    }
    BZ_Prov_bdbCommissioningreq.BDB_InvokeCommissioningConf = BZ_Prov_Callback_Commissioning;
    BDB_InvokeCommissioning(&BZ_Prov_bdbCommissioningreq);
}

/* Prov Get command handling */
 uint8_t APP_TRPS_Prov_Get_Cmd(uint8_t *p_cmd)
{
    switch(p_cmd[2])
    {
        case DEVICE_TYPE_GET_CMD:
        {
            provData.deviceType[0] = (APP_Z3_DEVICE_TYPE>>8)&0xFF;
            provData.deviceType[1] = APP_Z3_DEVICE_TYPE&0xFF;            
        }
        break;
            
        case SW_VERSION_GET_CMD:
        {
            memcpy(provData.swVersion,COMBO_APP_VERSION,sizeof(COMBO_APP_VERSION));
        }            
        break;
            
        case DEVICE_NAME_GET_CMD:
        {
            memset(provData.deviceName, 0, DEVICE_NAME_GET_RSP_LEN);            
            memcpy(provData.deviceName, commission_device_name, strlen(commission_device_name));
        }            
        break;
        
        case CHANNEL_MAP_GET_CMD:
        {
            CS_ReadParameter(CS_BDB_PRIMARY_CHANNELS_MASK_ID, &provData.channelMap);
        }            
        break;
        
        case DEVICE_STATE_GET_CMD:
        {
            provData.deviceState = BDB_GetBdbNodeIsOnANetwork();
        }            
        break;
        
        case MAC_ID_GET_CMD:
        {
            CS_ReadParameter(CS_UID_ID, &provData.macAddr);
        }            
        break;
        
        case CURRENT_CHANNEL_GET_CMD:
        {
            CS_ReadParameter(CS_RF_CURRENT_CHANNEL_ID, &provData.currChannel);
        }            
        break;         
            
    }

    return SUCCESS;
}

/* Prov Set command handling */
 uint8_t APP_TRPS_Prov_Set_Cmd(uint8_t *p_cmd)
{
    uint8_t status = SUCCESS;
    uint32_t channel_mask;
    switch(p_cmd[2])
    {
        case DEVICE_NAME_SET_CMD:
        {
            if( p_cmd[1] <= 17 )
            {
                memset(commission_device_name, 0, PDS_APP_DEVICE_NAME_ITEM_SIZE);
                memcpy(commission_device_name, &p_cmd[3], (p_cmd[1]-1));
                PDS_Store(PDS_APP_DEVICE_NAME_ID);
                
                BLE_GAP_SetDeviceName(strlen(commission_device_name), commission_device_name);
            }
            else
            {
                status = INVALID_PARAMETER;
            }           
        }
        break;
            
        case CHANNEL_MAP_SET_CMD:
        {
            if( p_cmd[1] == 5 )
            {
                memcpy(&channel_mask,&p_cmd[3],4);
                BDB_SetChannelMask(PRIMARY_CHANNEL_MASK, channel_mask); 
            }
            else
            {
                status = INVALID_PARAMETER;
            }
        }            
        break;
            
        case START_COMMISSION_SET_CMD:
        {
            if(!isDeviceCommissioned)
            {
                //ZB_SysTaskInit();
                startComm = true;
                provStatus = 0;
                commissioningMode = BDB_COMMISSIONING_NWK_STEERING;
				BZ_Prov_StartCommissioning(commissioningMode);
                App_SetCommissioningStatus(true);
            }    
            else
                status = OPERATION_FAILED;
        }            
        break;
        
        case FACTORY_NEW_SET_CMD:
        {
            ZB_BDB_ResetToFactoryNew(true);
        }            
        break;
        default:
        break;
    }
    return status;
}

/* Init Prov Specific */
void APP_Prov_TRPS_Init(void)
{
    uint8_t localNameLen;
    /* Init TRPS profile with Prov specific command structure*/
    APP_TRPS_Init(APP_TRP_VENDOR_OPCODE_PROV,appTrpsProvCmdResp,appTrpsProvNotify,PROV_CMD_RESP_LST_SIZE,PROV_NOTIFY_LST_SIZE);
  
    SYS_SubscribeToEvent(BC_EVENT_NETWORK_LEFT, &bzProvNetworkEventsListener);
    BDB_EnableAutoCommissioning();
    BDB_EventsSubscribe(&s_bdb_EventsCallback);
   
    // Configure device name
    if (PDS_IsAbleToRestore(PDS_APP_DEVICE_NAME_ID))
    {
      PDS_Restore(PDS_APP_DEVICE_NAME_ID);
      BLE_GAP_SetDeviceName(strlen(commission_device_name), commission_device_name);
    }
    else
    {
      strcpy(commission_device_name, "MICROCHIP");
      BLE_GAP_GetDeviceName(&localNameLen, &commission_device_name[0]);
    } 
    
    if (PDS_IsAbleToRestore(PDS_APP_COMMISION_ID))
    {
        PDS_Restore(PDS_APP_COMMISION_ID);
    }
    else
    {
        isDeviceCommissioned = false;
    }
    APP_InitConnList();
    //BLE_GAP_SetAdvEnable(0x01, 0);
}

/* Do the BLE Prov specific on disconnection  */
void APP_Prov_TRPS_DiscEvtProc(void)
{
  
}
