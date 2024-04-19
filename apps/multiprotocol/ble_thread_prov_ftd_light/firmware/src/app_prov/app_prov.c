/*******************************************************************************
  Application BLE Provisioning Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_bleprov.c

  Summary:
    This file contains the BLE Provisioning Application functions 
    in this project.

  Description:
    This file contains the Application functions for commissioning a 
    device via BLE link in this project.
    The implementation of demo mode is included in this file.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include "definitions.h"
#include "app_prov.h"
#include "app_trps/app_trps.h"
#include "../app_ble_conn_handler.h"
#include "driver/device_support/include/info_block.h"


// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
char commission_device_name[PDS_APP_DEVICE_NAME_ITEM_SIZE];
bool isDeviceCommissioned = false;
bool isBleConnected = false;
APP_ProvData_T provData;
bool startComm = false;
uint8_t provStatus = 0;

PDS_DECLARE_ITEM(PDS_APP_DEVICE_NAME_ID, PDS_APP_DEVICE_NAME_ITEM_SIZE, commission_device_name, NULL, SIZE_MODIFICATION_ALLOWED);
PDS_DECLARE_ITEM(PDS_APP_COMMISION_ID, sizeof(isDeviceCommissioned), &isDeviceCommissioned, NULL, NO_ITEM_FLAGS);
PDS_DECLARE_ITEM(PDS_APP_COMMISSION_DATA_ID, sizeof(APP_ProvData_T), &provData, NULL, NO_ITEM_FLAGS);
 
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

static void APP_TRPS_Prov_Default_Value(void)
{
    provData.provNwData.channel                    = CURRENT_CHANNEL;

    uint8_t channel_Mask[CHANNEL_MASK_LEN]            = CHANNEL_MASK;
    memcpy((uint8_t*)&provData.channelMaskDummy, channel_Mask, CHANNEL_MASK_LEN);
    provData.provNwData.channelMask = (provData.channelMaskDummy & 0x000000ff) << 24u| \
                                      (provData.channelMaskDummy & 0x0000ff00) << 8u | \
                                      (provData.channelMaskDummy & 0x00ff0000) >> 8u | \
                                      (provData.channelMaskDummy & 0xff000000) >> 24u;

    uint8_t pan_ID[PANID_LEN]            = PANID;
    memcpy((uint8_t*)&provData.panIdDummy, pan_ID, PANID_LEN);
    provData.provNwData.panId = (provData.panIdDummy & 0x00ff) << 8u | \
                                (provData.panIdDummy & 0xff00) >> 8u;

    uint8_t ex_panid[EXT_PANID_LEN] = EXD_PANID;
    memcpy(provData.provNwData.extPanId, ex_panid, EXT_PANID_LEN);

    uint8_t nw_key[NETWORK_KEY_LEN] = NW_KEY;
    memcpy(provData.provNwData.networkKey,nw_key,NETWORK_KEY_LEN);

    char nwk_name[] = NWK_NAME;
    size_t length = strlen(nwk_name);
    memset(provData.provNwData.networkName,0,NETWORK_NAME_LEN+1);
    if(length <= NETWORK_NAME_LEN)
    {
       memcpy(provData.provNwData.networkName, nwk_name, length);
    }
    else
    {
       memcpy(provData.provNwData.networkName, nwk_name, NETWORK_NAME_LEN); 
    }

    uint8_t ml_prefix[MESHLOCAL_PREFIX_LEN] = ML_PREFIX;
    memcpy(provData.provNwData.meshLocalPrefix,ml_prefix,MESHLOCAL_PREFIX_LEN);
}

/* Prov Get command handling */
static uint8_t APP_TRPS_Prov_Get_Cmd(uint8_t *p_cmd)
{
    switch(p_cmd[2])
    {
		case TECH_STACK_GET_CMD:
        {
            provData.devTechStack = TECH_STACK_TYPE;         
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
        
        case DEVICE_STATE_GET_CMD:
        {
            provData.deviceState = isDeviceCommissioned;
        }            
        break;
        
        case MAC_ID_GET_CMD:
        {
            if(!IB_GetMACAddr(provData.macAddr))
            {
              memset(provData.macAddr,0,8);  
            }
        }            
        break;
            
    }

    return SUCCESS;
}

/* Prov Set command handling */
static uint8_t APP_TRPS_Prov_Set_Cmd(uint8_t *p_cmd)
{
    uint8_t status = SUCCESS;
    switch(p_cmd[2])
    {
        case DEVICE_NAME_SET_CMD:
        {
            if( (p_cmd[1] <= DEVICE_NAME_LEN+1) &&  (p_cmd[1] > 0))
            {
                memset(commission_device_name, 0, PDS_APP_DEVICE_NAME_ITEM_SIZE);
                memcpy(commission_device_name, &p_cmd[3], (p_cmd[1]-1));
                PDS_Store(PDS_APP_DEVICE_NAME_ID);
                
                BLE_GAP_SetDeviceName(strlen(commission_device_name), (uint8_t*)commission_device_name);
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
                SYS_CONSOLE_MESSAGE("[BLE] Start Commissioning\r\n");
                startComm = true;
                provStatus = 0;
                APP_Prov_StartCommissioning(&provData.provNwData);
            }    
            else
                status = OPERATION_FAILED;
        }            
        break;
        
        case FACTORY_NEW_SET_CMD:
        {
            isDeviceCommissioned = false;
            PDS_Store(PDS_APP_COMMISSION_DATA_ID);
            APP_Prov_ResetToFactoryNew();
        }            
        break;
        case CURRENT_CHANNEL_SET_CMD:
        {
            if( (p_cmd[1] == CURRENT_CHANNEL_LEN + 1) && \
                ((p_cmd[3] >= 11) &&  (p_cmd[3] <= 26)))
            {
                provData.provNwData.channel = p_cmd[3];
                
            }
            else
            {
                status = INVALID_PARAMETER;
            }
        }
        break;
        case CHANNEL_MASK_SET_CMD:
        {
            if( p_cmd[1] == CHANNEL_MASK_LEN + 1 )
            {
                memcpy(&provData.channelMaskDummy,&p_cmd[3],CHANNEL_MASK_LEN);
                provData.provNwData.channelMask = (provData.channelMaskDummy & 0x000000ff) << 24u| \
                                                  (provData.channelMaskDummy & 0x0000ff00) << 8u | \
                                                  (provData.channelMaskDummy & 0x00ff0000) >> 8u | \
                                                  (provData.channelMaskDummy & 0xff000000) >> 24u;
            }
            else
            {
                status = INVALID_PARAMETER;
            }
        }
        break;
        case PANID_SET_CMD:
        {
            if( p_cmd[1] == PANID_LEN + 1 )
            {
                memcpy(&provData.panIdDummy,&p_cmd[3],PANID_LEN);
                provData.provNwData.panId = (provData.panIdDummy & 0x00ff) << 8u | \
                                            (provData.panIdDummy & 0xff00) >> 8u;
            }
            else
            {
                status = INVALID_PARAMETER;
            }
        }
        break;
		case EXT_PANID_SET_CMD:
		{
		    if( p_cmd[1] == EXT_PANID_LEN + 1 )
            {
                memcpy(provData.provNwData.extPanId,&p_cmd[3],EXT_PANID_LEN);
            }
            else
            {
                status = INVALID_PARAMETER;
            }
		}
		break;
        case NETWORK_NAME_SET_CMD:
        {
            if( (p_cmd[1] <= NETWORK_NAME_LEN + 1) &&  (p_cmd[1] > 0))
            {
                memset(provData.provNwData.networkName, 0, NETWORK_NAME_LEN);
                memcpy(provData.provNwData.networkName, &p_cmd[3], (p_cmd[1]-1));
            }
            else
            {
                status = INVALID_PARAMETER;
            }           
        }
        break;
		case NETWORK_KEY_SET_CMD:
        {
            if( p_cmd[1] == NETWORK_KEY_LEN + 1 )
            {
                memcpy(provData.provNwData.networkKey, &p_cmd[3], NETWORK_KEY_LEN);
            }
            else
            {
                status = INVALID_PARAMETER;
            }           
        }
        break;
		case MESHLOCAL_PREFIX_SET_CMD:
        {
            if( p_cmd[1] == MESHLOCAL_PREFIX_LEN + 1 )
            {
                memcpy(provData.provNwData.meshLocalPrefix, &p_cmd[3], MESHLOCAL_PREFIX_LEN);
            }
            else
            {
                status = INVALID_PARAMETER;
            }           
        }
        break;
		case USER_PARAMS_SET_CMD:
        {
            if( p_cmd[1] == USER_PARAMS_LEN + 1 )
            {
                memcpy(provData.provNwData.userParam, &p_cmd[3], USER_PARAMS_LEN);
            }
            else
            {
                status = INVALID_PARAMETER;
            }           
        }
        break;
        default:
        break;
    }
    return status;
}

void APP_Prov_TRPS_ProvComplete(bool commStatus)
{ 
  if(commStatus)
  {
  
	provStatus = (provStatus & 0xFD) | 0x02;
    provData.deviceState = true;

	if(startComm)
	{
        isDeviceCommissioned = commStatus;
        PDS_Store(PDS_APP_COMMISION_ID);
        PDS_Store(PDS_APP_COMMISSION_DATA_ID);
        
		if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
		{
			APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_PROV,PROV_STATUS_NFY);
		}
	}
  }
  else
  {
    provStatus = 0;
	
	if(startComm)
	{
		if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
		{
			APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_PROV,PROV_STATUS_NFY);
		}
	}
  }
}

/* Init Prov Specific */
void APP_Prov_TRPS_Init(void)
{
    uint8_t localNameLen;
    /* Init TRPS profile with Prov specific command structure*/
    APP_TRPS_Init(APP_TRP_VENDOR_OPCODE_PROV,appTrpsProvCmdResp,appTrpsProvNotify,PROV_CMD_RESP_LST_SIZE,PROV_NOTIFY_LST_SIZE);
   
   
    // Configure device name
    if (PDS_IsAbleToRestore(PDS_APP_DEVICE_NAME_ID))
    {
      PDS_Restore(PDS_APP_DEVICE_NAME_ID);
      BLE_GAP_SetDeviceName(strlen(commission_device_name), (uint8_t*)commission_device_name);
    }
    else
    {
      strcpy(commission_device_name, "MICROCHIP");
      BLE_GAP_GetDeviceName(&localNameLen, (uint8_t*)&commission_device_name[0]);
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
    
    provData.deviceState = false;
    
    APP_TRPS_Prov_Default_Value();
}

void APP_Prov_TRPS_CommissionOnReset(void)
{
    if(isDeviceCommissioned)
    {
        if (PDS_IsAbleToRestore(PDS_APP_COMMISSION_DATA_ID))
        {
            SYS_CONSOLE_MESSAGE("[THREAD] Start Commissioning\r\n");
            PDS_Restore(PDS_APP_COMMISSION_DATA_ID);
            APP_Prov_StartCommissioning(&provData.provNwData);
        }
        else
        {
            //Debug print if required
        }
    }
    else
    {
        //Debug print if required
    }
}

/* Do the BLE Prov specific on disconnection  */
void APP_Prov_TRPS_DiscEvtProc(void)
{
  
}
