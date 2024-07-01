/*******************************************************************************
  Application Zigbee Handler Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_zigbee_handler.c

  Summary:
    This file contains the Application Zigbee functions for this project.

  Description:
    This file contains the Application Zigbee functions for this project.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <app_zigbee/app_zigbee_handler.h>
#include <app_zigbee/zigbee_console/console.h>
#include <zcl/include/zclZllIdentifyCluster.h>
#include <zcl/include/zclZllGroupsCluster.h>
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

void BSP_Event_Handler(APP_Zigbee_Event_t event);
void Cluster_Event_Handler(APP_Zigbee_Event_t event);
void Zigbee_Event_Handler(APP_Zigbee_Event_t event);

#define MIN_COLOR_LEVEL           0u      //Added to avoid MISRA_c_2014_10.4 Violation
#define MAX_COLOR_LEVEL           0xfeffu

#define MIN_SATURATION_LEVEL      0u
#define MAX_SATURATION_LEVEL      0xfeu
// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Zigbee_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void APP_Zigbee_Handler(APP_Zigbee_Event_t event)
{
    switch(event.eventGroup)
    {
        case EVENT_BSP:
        {
            /* BSP Events Dispatcher */
            BSP_Event_Handler(event);
        }
        break;

        case EVENT_ZIGBEE:
        {
            /* Zigbee Events Dispatcher */
            Zigbee_Event_Handler(event);
        }
        break;

        case EVENT_CLUSTER:
        {
            /* Cluster Events Dispatcher */
            Cluster_Event_Handler(event);
        }
        break;

        default:
            /*default none handler */
        break;
    }
}

/*******************************************************************************
  Function:
    void BSP_Event_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void BSP_Event_Handler(APP_Zigbee_Event_t event)
{
    // User to handle  board Support package events
    switch(event.eventId)
    {
        case CMD_LED_OPEN:
        {
            /* Init/Open LEDs */
            //appSnprintf("Init /Open LEDs\r\n");
        }
        break;

        case CMD_LED_ON:
        {
            /* Turn on the LED */
            //Access - > event.eventData.value;
            //appSnprintf("On\r\n");
        }
        break;

        case CMD_LED_OFF:
        {
            /* Turn off the LED */
            //Access - > event.eventData.value;
            //appSnprintf("Off\r\n");
        }
        break;

        case CMD_LED_TOGGLE:
        {
            /* Toggle the LED */
            //Access - > event.eventData.value;
            //appSnprintf("Toggle\r\n");
        }
        break;
        case CMD_LED_BRIGHTNESS:
        {
            /* Set the given LED brightness */
            //Access - > event.eventData.value;
            //appSnprintf("Led Brightness \r\n");
        }
        break;

        case CMD_LED_COLOR_HS:
        {
            /* LED Hue , Saturation */
            //Access - > event.eventData.colorHS.hue;
            //Access - > event.eventData.colorHS.saturation;
            //appSnprintf("LED Hue , Saturation \r\n");
        }
        break;

        case CMD_LED_COLOR_XY:
        {
            /* Set the LED Color X Y */
            //Access - > event.eventData.colorXY.x;
            //Access - > event.eventData.colorXY.y;
            //appSnprintf("LED X,Y Color \r\n");
        }
        break;

        case CMD_BUTTON_OPEN:
        {
            /* Button Init */
            //appSnprintf("Button Init/Open \r\n");
        }
        break;

        case CMD_BUTTON_READ:
        {
            /* Button Press */
            //Access - > event.eventData.state;
            //appSnprintf("Button Read \r\n");
        }
        break;

        case CMD_SENSOR_OPEN:
        {
            /* Sensor Data */
            //appSnprintf("Sensor Open /Init Event \r\n");
        }        
        break;

        case CMD_SENSOR_READ:
        {
            /* Sensor Data */
            //Access - > event.eventData.data;
            //appSnprintf("Sensor Read Event \r\n");
        }        
        break;
		
		case CMD_BUTTON_LONG_PRESS:
        {
            /* Button long press */
            BDB_ResetToFactoryNew(true);
        }
		break;

        default: // Added Comments in all Default case to avoid MISRA 16.4 violations
            /*default none handler */
        break;
    }
}

/*******************************************************************************
  Function:
    void Zigbee_Event_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void Zigbee_Event_Handler(APP_Zigbee_Event_t event)
{
    // User to handle all zigbee stack events  
    switch(event.eventId)
    {
        case EVENT_NETWORK_ESTABLISHED:
        {
            (void) appSnprintf("Network Established\r\n"); // adding (void) in each appSnprintf - to avoid MISRA 17.7 Violation
        }
        break;
        case EVENT_DISCONNECTED:
        {
           (void) appSnprintf("Disconnected from the Network\r\n");
        }
        break;
        case EVENT_COMMISSIONING_STARTED:
        {
            (void) appSnprintf("Commissioning Procedure Started - 180 Seconds \r\n");
            (void) appSnprintf("Commissioning Sequence: ");
            (void) appSnprintf("Steering->");
            (void) appSnprintf("Find & Bind->");
            (void) appSnprintf("\r\n");
        }
        break;

        case EVENT_COMMISSIONING_COMPLETE:
        {
            (void) appSnprintf("Commissioning Procedure Complete \r\n");
        }
        break;
        case EVENT_COMMISSIONING_FORMATION_COMPLETE:
        {
            (void) appSnprintf("Nwk Formation: ");
            if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else //BDB_COMMISSIONING_FORMATION_FAILURE
            {   (void) appSnprintf("Failed\r\n");  }
        }
        break;
        case EVENT_COMMISSIONING_STEERING_COMPLETE:
        {
            (void) appSnprintf("Steering: ");
            if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_NO_NETWORK)
            {   (void) appSnprintf("No networks found to join\r\n");   }
            else if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else
            {   (void) appSnprintf("Failed\r\n");  }
        }
        break;
        case EVENT_COMMISSIONING_TOUCHLINK_COMPLETE:
        {
            (void) appSnprintf("Touchlink: Attempt: ");
            if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_NO_SCAN_RESPONSE)
            {   (void) appSnprintf("No scan response\r\n");    }
            else if(event.eventData.value == (uint8_t)BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else
            {    (void) appSnprintf("Failed\r\n"); }
        }
        break;
        case EVENT_COMMISSIONING_FINDBIND_COMPLETE:
        {
            (void) appSnprintf("Finding & Binding: ");
            if(event.eventData.value == (uint8_t) BDB_COMMISSIONING_NO_IDENTIFY_QUERY_RESPONSE)
            {   (void) appSnprintf("No identify Query Response\r\n");  }
            else if(event.eventData.value == (uint8_t) BDB_COMMISSIONING_BINDING_TABLE_FULL)
            {   (void) appSnprintf("Binding table full\r\n");  }
            else if(event.eventData.value ==(uint8_t) BDB_COMMISSIONING_SUCCESS)
            {   (void) appSnprintf("Success\r\n"); }
            else
            {   (void) appSnprintf("Failed\r\n");  }
        }
        break;
        case EVENT_COMMISSIONING_FAILURE:
        {
          switch(event.eventData.value)
          {
              case (uint8_t)BDB_COMMISSIONING_NO_NETWORK:  // since event.eventData.value is APP_Zigbee_Event_t uint8_t variable, 
                                                           // BDB_COMMISSIONING_NO_NETWORK is anonymous enum 
                                                           // without typecast raises the MISRA rule 2012 - 10.3 violations.
              {
                  //(void) appSnprintf("No network found in search\r\n");
              }
              break;
              case (uint8_t)BDB_COMMISSIONING_NOT_SUPPORTED:
              {
                  (void) appSnprintf("Commissioning: One of the BDB commissioning procedure not supported\r\n");
              }
              break;
              case (uint8_t)BDB_COMMISSIONING_NO_SCAN_RESPONSE:
              break;
              case (uint8_t)BDB_COMMISSIONING_NO_IDENTIFY_QUERY_RESPONSE:
              break;
              default:
                /*default none handler */
              break;
          }
        }
        break;

        case EVENT_STARTED_CENTRALIZED_NETWORK:
        {
            (void) appSnprintf("Started Centralized Network\r\n");
        }
        break;
        case EVENT_STARTED_DISTRIBUTED_NETWORK:
        {
            (void) appSnprintf("Started Distributed Network\r\n");
        }
        break;
        case EVENT_JOINED_TO_AN_EXISTING_NETWORK:
        {
            (void) appSnprintf("Network Search: Complete: Joined to a Network \r\n");
            (void) appSnprintf("Joined to: Address 0x%04x  MACID 0x%08x%08x ExtendedPANID 0x%08x%08x\r\n", event.eventData.ParentChildInfo.shortAddress, (uint32_t)(event.eventData.ParentChildInfo.extendedAddress >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedAddress & 0xFFFFFFFF), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId & 0xFFFFFFFF));
        }
        break;

        case EVENT_WAKEUP:
        {
            //(void) appSnprintf("Wake up Indication \r\n");
        }
        break;

        case EVENT_LEFT_FROM_NETWORK:
        {
            //(void) appSnprintf("Left from the Network \r\n");
        }
        break;

        case EVENT_CHILD_JOINED:
        {
            (void) appSnprintf("Device joined: Address 0x%04x  MACID 0x%08x%08x ExtendedPANID 0x%08x%08x\r\n", event.eventData.ParentChildInfo.shortAddress, (uint32_t)(event.eventData.ParentChildInfo.extendedAddress >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedAddress & 0xFFFFFFFF), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedPanId & 0xFFFFFFFF));
        }
        break;

        case EVENT_CHILD_REMOVED:
        {
            (void) appSnprintf("Child Left\r\n");
        }
        break;

        case EVENT_NWK_UPDATE:
        {
            //(void) appSnprintf("Network Information updated \r\n");
        }
        break;

        case EVENT_RESET_TO_FACTORY_DEFAULTS:
        {
            //(void) appSnprintf("Reset To Factory New\r\n");
        }
        break;

        case EVENT_NWK_ADDRESS_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf( "->NwkAddrResponse, status = %d, address = %04x\r\n" ,event.eventData.ParentChildInfo.status, event.eventData.ParentChildInfo.shortAddress);}
            else
            {   (void) appSnprintf( "->NwkAddrResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);    }
        }
        break;

        case EVENT_IEEE_ADDRESS_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf("->IeeeAddrResponse, status = %d, address = 0x%08x%08x \r\n", event.eventData.ParentChildInfo.status, (uint32_t)(event.eventData.ParentChildInfo.extendedAddress >> 32), (uint32_t)(event.eventData.ParentChildInfo.extendedAddress & 0xFFFFFFFF)); }
            else
            {   (void) appSnprintf( "->IeeeAddrResponse, status = %d \r\n", event.eventData.ParentChildInfo.status);}
        }
        break;

        case EVENT_SIMPLE_DESCRIPTOR_RESPONSE:
        {
            (void) appSnprintf( "->SimpleDescResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_MATCH_DESCRIPTOR_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf( "->MatchDescResponse, status = %d, MatchedEpCount = %d\r\n" ,event.eventData.ParentChildInfo.status, event.eventData.ParentChildInfo.ep);}
            else
            {   (void) appSnprintf( "->MatchDescResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);}
        }
        break;

        case EVENT_ACTIVE_EP_RESPONSE:
        {
            if(event.eventData.ParentChildInfo.status == (uint8_t) ZCL_SUCCESS_STATUS)
            {   (void) appSnprintf( "->ActiveEpResponse, status = %d, EpCount = %d\r\n" ,event.eventData.ParentChildInfo.status, event.eventData.ParentChildInfo.ep);}
            else
            {   (void) appSnprintf( "->ActiveEpResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);   }
        }
        break;

        case EVENT_NODE_DESCRIPTOR_RESPONSE:
        {
            (void) appSnprintf( "->NodeDescResponse, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_LEAVE_RESPONSE:
        {
            (void) appSnprintf( "->LeaveRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_MANAGEMENT_BIND_RESPONSE:
        {
            (void) appSnprintf( "MgmtBindRsp %d\r\n", event.eventData.ParentChildInfo.status); 
        }
        break;

        case EVENT_LQI_RESPONSE:
        {
            (void) appSnprintf( "->MgmtLqiRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_BIND_RESPONSE:
        {
            (void) appSnprintf( "->BindRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        case EVENT_UNBIND_RESPONSE:
        {
            (void) appSnprintf( "->UnBindRsp, status = %d \r\n" ,event.eventData.ParentChildInfo.status);
        }
        break;

        default:
			/*default none  */
        break;
    }
}

/*******************************************************************************
  Function:
    void Cluster_Event_Handler(APP_Zigbee_Event_t event)

  Remarks:
    See prototype in app_zigbee_handler.h.
******************************************************************************/

void Cluster_Event_Handler(APP_Zigbee_Event_t event)
{
    switch(event.eventId)
    {
        case CMD_ZCL_RESET_TO_FACTORY_DEFAULTS:
        {
            /* Command ZCL ResetToFactoryDefaults */
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("ZCL ResetToFactoryDefaults\r\n");
        }
        break;
        case CMD_ZCL_IDENTIFY:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            (void) appSnprintf( "->Identify\r\n");
        }
        break;
        case CMD_ZCL_IDENTIFY_QUERY:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("->IdentifyQuery\r\n");
        }
        break;
        case CMD_ZCL_TRIGGER_EFFECT:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_TriggerEffect_t *payload = (ZCL_TriggerEffect_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("->TriggerEffect 0x%x\r\n",  payload->effectIdentifier);
        }
        break;
        case CMD_ZCL_IDENTIFY_QUERY_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            //(void) appSnprintf("->IdentifyQueryResponse, addr = 0x%04x, timeout = 0x%04x\r\n", addressing->addr.shortAddress, payload->timeout);
        }
        break;
        case CMD_ZCL_ADD_GROUP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_AddGroup_t *payload = (ZCL_AddGroup_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("addGroupInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_VIEW_GROUP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_ViewGroup_t *payload = (ZCL_ViewGroup_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("viewGroupInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_GET_GROUP_MEMBERSHIP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload; 
            (void) appSnprintf("getGroupMembershipInd()\r\n");
        }
        break;
        case CMD_ZCL_REMOVE_GROUP:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_RemoveGroup_t *payload = (ZCL_RemoveGroup_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("removeGroupInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_REMOVE_ALL_GROUP:
        {
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
            (void) appSnprintf( "removeAllGroupsInd()\r\n");
        }
        break;
        case CMD_ZCL_ADD_GROUP_IF_IDENTIFYING:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_AddGroupIfIdentifying_t *payload = (ZCL_AddGroupIfIdentifying_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("addGroupIfIdentifyingInd(): 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_ADD_GROUP_RESPONSE:
        {
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
            (void) appSnprintf("addGroupResponseInd()\r\n");
        }
        break;
        case CMD_ZCL_VIEW_GROUP_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_ViewGroupResponse_t *payload = (ZCL_ViewGroupResponse_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("viewGroupResponse(): status = 0x%02x\r\n", payload->status);
            (void) appSnprintf("groupId = 0x%04x\r\n", payload->groupId);
        }
        break;
        case CMD_ZCL_GET_GROUP_MEMBERSHIP_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_GetGroupMembershipResponse_t *payload = (ZCL_GetGroupMembershipResponse_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("getGroupMembershipResponse()\r\n");
            (void) appSnprintf("groupCount = %d\r\n", payload->groupCount);
            for (uint8_t i = 0; i < payload->groupCount; i++)
            {  (void) appSnprintf("groupId = 0x%04x\r\n", payload->groupList[i]);}
        }
        break;
        case CMD_ZCL_REMOVE_GROUP_RESPONSE:
        {
            //Access - > event.eventData.zclEventData.addressing;
            //Access - > event.eventData.zclEventData.payloadLength;
            //Access - > event.eventData.zclEventData.payload;
            ZCL_RemoveGroupResponse_t *payload = (ZCL_RemoveGroupResponse_t *)event.eventData.zclEventData.payload;
            (void) appSnprintf("removeGroupResponseInd()\r\n");
            (void) appSnprintf("groupId = 0x%04x\r\n", payload->groupId);

        }
        break;
        case CMD_ZCL_ATTR_IDENTIFY:
        {
            /* Command ZCL IdentifyAttributeEventInd */ 
            //Access - > event.eventData.zclAttributeData.addressing;
            //Access - > event.eventData.zclAttributeData.attributeId;
            //Access - > event.eventData.zclAttributeData.event;
            //(void) appSnprintf("ZCL IdentifyAttributeEventInd\r\n");
        }
        break;
        
        //Custom Clusters       
                case CMD_ZCL_ATTR_FLOWRATEMONITOR:
        {
   
            //add code for read attribute callback

        }
        break;



        default:
           //Default no process 
        break;
    }
}