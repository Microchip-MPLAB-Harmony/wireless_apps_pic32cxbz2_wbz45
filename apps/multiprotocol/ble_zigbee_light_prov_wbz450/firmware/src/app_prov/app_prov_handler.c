/*******************************************************************************
  Application BLE - Zigbee Provisioning Handler Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_prov_handler.c

  Summary:
    This file contains the BLE Zigbee Provisioning Handler functions for this 
    project.

  Description:
    This file contains the Application Handler functions for commissioning a 
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
#include <string.h>
#include <stdint.h>
#include "app_prov.h"
#include "app_trps/app_trps.h"
#include "../app_ble_conn_handler.h"
#include <app_zigbee/zigbee_console/console.h>
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:
    void Zigbee_Event_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void BZ_Prov_Zigbee_Event_Handler(APP_Zigbee_Event_t event)
{
    // User to handle all zigbee stack events  
    switch(event.eventId)
    {
        case EVENT_COMMISSIONING_FINDBIND_COMPLETE:
	    {
            if (update_find_n_bind() == false)
            {
                appSnprintf("Finding & Binding: ");
                if(event.eventData.value == BDB_COMMISSIONING_NO_IDENTIFY_QUERY_RESPONSE)
                    appSnprintf("No identify Query Response\r\n");
                else if(event.eventData.value == BDB_COMMISSIONING_BINDING_TABLE_FULL)
                    appSnprintf("Binding table full\r\n");
                else if(event.eventData.value == BDB_COMMISSIONING_SUCCESS)
                    appSnprintf("Success\r\n");
                else
                    appSnprintf("Failed\r\n");
                event.eventGroup = EVENT_ZIGBEE;
                event.eventId = EVENT_COMMISSIONING_COMPLETE;
                APP_Zigbee_Handler(event);
            }
	    }
	    break;
        case EVENT_COMMISSIONING_FAILURE:
        {
			if (event.eventData.value ==BDB_COMMISSIONING_NO_NETWORK)
			{
				if(startComm)
				{
					if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
					{
						provStatus = 0;
                        APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_PROV,PROV_STATUS_NFY);
					}
				}				
			}
		}
		break;
        
        case EVENT_COMMISSIONING_FORMATION_COMPLETE:
	    {
            appSnprintf("Nwk Formation: ");

            if(event.eventData.value == BDB_COMMISSIONING_SUCCESS)
            {
                appSnprintf("Success\r\n");
                ShortAddr_t nwkAddr;
                CS_ReadParameter(CS_NWK_ADDR_ID, &nwkAddr);
                       
                if (nwkAddr == 0)
                {
                    appSnprintf("Started Centralized Network\r\n");
                }
                else
                {
                    appSnprintf("Started Distributed Network\r\n");
                }          
                if(startComm)
                {
                    if(!isDeviceCommissioned)
                    {
                        isDeviceCommissioned = true;
                        PDS_Store(PDS_APP_COMMISION_ID);
                    }        
                    if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
                    {
                        provStatus = (provStatus & 0xFB) | 0x04;
                    }
                }                
            }
            else //BDB_COMMISSIONING_FORMATION_FAILURE
                appSnprintf("Failed\r\n");
	    }
	    break;

        case EVENT_COMMISSIONING_STEERING_COMPLETE:
        {
            appSnprintf("Steering: ");
            if(event.eventData.value == BDB_COMMISSIONING_NO_NETWORK)
                appSnprintf("No networks found to join\r\n");
            else if(event.eventData.value == BDB_COMMISSIONING_SUCCESS)
            {
                appSnprintf("Success\r\n");
                ShortAddr_t nwkAddr;
                ExtPanId_t  csExtPanId;
                uint64_t extendedAddress;
                CS_ReadParameter(CS_NWK_PARENT_ADDR_ID, &nwkAddr);
                const ExtAddr_t* parentExtAddr =  NWK_GetExtByShortAddress(nwkAddr);
                CS_ReadParameter(CS_EXT_PANID_ID, &csExtPanId);
                if (parentExtAddr)
                    memcpy(&extendedAddress, parentExtAddr, 8);
                else
                    extendedAddress = 0;            
           
                appSnprintf("Network Search: Complete: Joined to a Network \r\n");
                appSnprintf("Joined to: Address 0x%04x  MACID 0x%08x%08x ExtendedPANID 0x%08x%08x\r\n", nwkAddr, (uint32_t)(extendedAddress >> 32), (uint32_t)(extendedAddress & 0xFFFFFFFF), (uint32_t)(csExtPanId >> 32), (uint32_t)(csExtPanId & 0xFFFFFFFF));

                if(startComm)
                {
                    /* APP_TRPS_Sensor_BLE_light_Init(true);    */           
                    if(!isDeviceCommissioned)
                    {
                        isDeviceCommissioned = true;
                        PDS_Store(PDS_APP_COMMISION_ID);
                    }
                    if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
                    {
                        provStatus = (provStatus & 0xFE) | 0x01; 
                        //APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_PROV,PROV_STATUS_NFY);
                    }
                } 
            }    
            else
                appSnprintf("Failed\r\n");
              
        }
        break;
        case EVENT_COMMISSIONING_COMPLETE:
        {
            appSnprintf("Commissioning Procedure Complete \r\n");
            provStatus = (provStatus & 0xFD);
            if(event.eventData.value == BDB_COMMISSIONING_SUCCESS)
            {
                provStatus = (provStatus & 0xFD) | 0x02;
            }

            if(startComm)
            {
                if ( APP_GetBleState() == APP_BLE_STATE_CONNECTED)
                {
                    APP_TRPS_SendNotification(APP_TRP_VENDOR_OPCODE_PROV,PROV_STATUS_NFY);
                }
            }                       
        }
        break;      
        default:
        break;
    }
}

