/*******************************************************************************
  Application Zigbee Handler Header File

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

#ifndef _APPZIGBEEHANDLER_H
#define _APPZIGBEEHANDLER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <definitions.h>
#include <z3device/common/include/z3Device.h>
// *****************************************************************************
// *****************************************************************************
// Section: Macro definitions
// *****************************************************************************

// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************

/*******************************************************************************
 Application Zigbee Major Types of Events Trigger
******************************************************************************/
typedef enum
{
    /* BSP Events */
    EVENT_BSP,

    /* Cluster Events */
    EVENT_CLUSTER,

    /* Zigbee Events */
    EVENT_ZIGBEE,
} APP_Zigbee_EventGroup_t;

/*******************************************************************************
 Application Zigbee Events
******************************************************************************/
typedef enum
{
    /* Events in the BSP Group Event */
    /* LEDs Init/Open */
    CMD_LED_OPEN = 0,
    /* LED On Event */
    CMD_LED_ON,
    /* LED On Event */
    CMD_LED_OFF,
    /* LED Toggle Event */
    CMD_LED_TOGGLE,
    /* LED BRIGHTNESS Event */
    CMD_LED_BRIGHTNESS,
    /* LED Set Color HS Event */
    CMD_LED_COLOR_HS,
    /* LED Set Color XY Event */
    CMD_LED_COLOR_XY,
    /* Button Event */
    CMD_BUTTON_OPEN,
    /* Button Event */
    CMD_BUTTON_READ,
     /* Sensor Event */
    CMD_SENSOR_OPEN,
    /* Sensor Event */
    CMD_SENSOR_READ,
    /* Button Event */        
    CMD_BUTTON_LONG_PRESS,

    /* Events in the Zigbee Group Event */
    /* Network joining/rejoining done*/
    EVENT_NETWORK_ESTABLISHED,
    /* Disconnected Event */
    EVENT_DISCONNECTED,
    /* Reset To Factory Defaults Event */
    EVENT_RESET_TO_FACTORY_DEFAULTS,
    /* Commissioning Started Event */
    EVENT_COMMISSIONING_STARTED,
    /* Commissioning Complete Event */
    EVENT_COMMISSIONING_COMPLETE,
    /* Commissioning Failure Event */
    EVENT_COMMISSIONING_FAILURE,
    /* Touchlink completed Event */
    EVENT_COMMISSIONING_TOUCHLINK_COMPLETE,
    /*Formation completed event*/
    EVENT_COMMISSIONING_FORMATION_COMPLETE,
    /*Steering completed event*/
    EVENT_COMMISSIONING_STEERING_COMPLETE,
    /*Finding and Binding completed event*/
    EVENT_COMMISSIONING_FINDBIND_COMPLETE,
    /* Started Centralized Network Event */
    EVENT_STARTED_CENTRALIZED_NETWORK,
    /* Started Distributed Network Event */
    EVENT_STARTED_DISTRIBUTED_NETWORK,
    /* Joined to an existing Network Event */
    EVENT_JOINED_TO_AN_EXISTING_NETWORK,
    /* Wake up Indication Event */
    EVENT_WAKEUP,
    /* Network Left Event */
    EVENT_LEFT_FROM_NETWORK,
    /* Child Joined Event */
    EVENT_CHILD_JOINED,
    /* Child Removed Event */
    EVENT_CHILD_REMOVED,
    /* Network Update Event - Nwk Parameters Updated */
    EVENT_NWK_UPDATE,
    /* Network Address Response */
    EVENT_NWK_ADDRESS_RESPONSE,
    /* IEEE Address Response */
    EVENT_IEEE_ADDRESS_RESPONSE,
    /* Simple Descriptor Response */
    EVENT_SIMPLE_DESCRIPTOR_RESPONSE,
    /* Match Descriptor Response */
    EVENT_MATCH_DESCRIPTOR_RESPONSE,
    /* Active Endpoint Response */
    EVENT_ACTIVE_EP_RESPONSE,
    /* Node Descriptor Response */
    EVENT_NODE_DESCRIPTOR_RESPONSE,
    /* Leave response */
    EVENT_LEAVE_RESPONSE,
    /* Management bind response */
    EVENT_MANAGEMENT_BIND_RESPONSE,
    /* Lqi response */
    EVENT_LQI_RESPONSE,
    /* Bind Response */
    EVENT_BIND_RESPONSE,
    /* Unbind reponse */
    EVENT_UNBIND_RESPONSE,

    /* Events in the Cluster Group Event */
    /* Command ZCL ON */
    CMD_ZCL_ON,
    /* Command ZCL OFF */
    CMD_ZCL_OFF,
    /* Command ZCL Toggle */
    CMD_ZCL_TOGGLE,
    /* Command ZCL Toggle */
    CMD_ZCL_OFF_WITH_EFFECT,
    /* Command ZCL Toggle */
    CMD_ZCL_ON_WITH_RECALL_GLOBALSCENE,
    /* Command ZCL Toggle */
    CMD_ZCL_ON_WITH_TIMED_OFF,
    /* Command ZCL MOVE TO HUE */
    CMD_ZCL_MOVE_TO_HUE,
    /* Command ZCL MOVE HUE */
    CMD_ZCL_MOVE_HUE,
    /* Command ZCL STEP HUE */
    CMD_ZCL_STEP_HUE,
    /* Command ZCL MOVE TO SATURATION */
    CMD_ZCL_MOVE_TO_SATURATION,
    /* Command ZCL MOVE SATURATION */
    CMD_ZCL_MOVE_SATURATION,
    /* Command ZCL STEP SATURATION */
    CMD_ZCL_STEP_SATURATION,
    /* Command ZCL MOVE TO HUE SATURATION */
    CMD_ZCL_MOVE_TO_HUE_SATURATION,
    /* Command ZCL MOVE TO COLOR */
    CMD_ZCL_MOVE_TO_COLOR,
    /* Command ZCL MOVE COLOR */
    CMD_ZCL_MOVE_COLOR,
    /* Command ZCL STEP COLOR */
    CMD_ZCL_STEP_COLOR,
    /* Command ZCL MOVE TO COLOR TEMPERATURE*/
    CMD_ZCL_MOVE_TO_COLOR_TEMPERATURE,
    /* Command ZCL ENHANCED MOVE TO HUE*/
    CMD_ZCL_ENHANCED_MOVE_TO_HUE,
    /* Command ZCL ENHANCED MOVE HUE*/
    CMD_ZCL_ENHANCED_MOVE_HUE,
    /* Command ZCL ENHANCED STEP HUE*/
    CMD_ZCL_ENHANCED_STEP_HUE,
    /* Command ZCL ENHANCED MOVE TO HUE SATURATION*/
    CMD_ZCL_ENHANCED_MOVED_TO_HUE_SATURATION,
    /* Command ZCL COLOR LOOP SET*/
    CMD_ZCL_COLOR_LOOP_SET,
    /* Command ZCL COLOR LOOP SET*/
    CMD_ZCL_STOP_MOVE_STEP,
    /* Command ZCL MOVE COLOR TEMPERATURE*/
    CMD_ZCL_MOVE_COLOR_TEMPERATURE,
    /* Command ZCL STEP COLOR TEMPERATURE*/
    CMD_ZCL_STEP_COLOR_TEMPERATURE,
    /* Command ZCL ResetToFactoryDefaults */
    CMD_ZCL_RESET_TO_FACTORY_DEFAULTS,
    /* Command ZCL Identify */
    CMD_ZCL_IDENTIFY,
    /* Command ZCL Identify Query */
    CMD_ZCL_IDENTIFY_QUERY,
    /* Command ZCL Trigger Effect */
    CMD_ZCL_TRIGGER_EFFECT,
    /* Command ZCL Identify Query Response*/
    CMD_ZCL_IDENTIFY_QUERY_RESPONSE,
    /* Command ZCL Add group */
    CMD_ZCL_ADD_GROUP,
    /* Command ZCL View group */
    CMD_ZCL_VIEW_GROUP,
    /* Command ZCL Get Group Membership */
    CMD_ZCL_GET_GROUP_MEMBERSHIP,
    /* Command ZCL Remove Group */
    CMD_ZCL_REMOVE_GROUP,
    /* Command ZCL Remove All Group */
    CMD_ZCL_REMOVE_ALL_GROUP,
    /* Command ZCL Add group if identifying */
    CMD_ZCL_ADD_GROUP_IF_IDENTIFYING,
    /* Command ZCL Add group response */
    CMD_ZCL_ADD_GROUP_RESPONSE,
    /* Command ZCL View group response */
    CMD_ZCL_VIEW_GROUP_RESPONSE,
    /* Command ZCL Group Membership response */
    CMD_ZCL_GET_GROUP_MEMBERSHIP_RESPONSE,
    /* Command ZCL Remove Group Response */
    CMD_ZCL_REMOVE_GROUP_RESPONSE,
    /* Command Move To Level */
    CMD_ZCL_MOVE_TO_LEVEL,
    /* Command Move To Level */
    CMD_ZCL_MOVE,
    /* Command Step */
    CMD_ZCL_STEP,
    /* Command Stop */
    CMD_ZCL_STOP,
    /* Command Move To Level With Onoff */
    CMD_ZCL_MOVE_TO_LEVEL_WITH_ONOFF,
    /* Command Move With Onoff */
    CMD_ZCL_MOVE_WITH_ONOFF,
    /* Command Step With Onoff */
    CMD_ZCL_STEP_WITH_ONOFF,
    /* Command Stop With Onoff */
    CMD_ZCL_STOP_WITH_ONOFF,
    /* Command ZCL ADD SCENE*/
    CMD_ZCL_ADD_SCENE,
    /* Command ZCL VIEW SCENE*/
    CMD_ZCL_VIEW_SCENE,
    /* Command ZCL ENHANCED ADD SCENE*/
    CMD_ZCL_ENHANCED_ADD_SCENE,
    /* Command ZCL ENHANCED VIEW SCENE*/
    CMD_ZCL_ENHANCED_VIEW_SCENE,
    /* Command ZCL REMOVE SCENE*/
    CMD_ZCL_REMOVE_SCENE,
    /* Command ZCL REMOVE ALL SCENES*/
    CMD_ZCL_REMOVE_ALL_SCENES,
    /* Command ZCL STORE SCENES*/
    CMD_ZCL_STORE_SCENE,
    /* Command ZCL RECALL SCENE*/
    CMD_ZCL_RECALL_SCENE,
    /* Command ZCL GET SCENE MEMBERSHIP*/
    CMD_ZCL_GET_SCENE_MEMBERSHIP,
    /* Command ZCL COPY SCENE*/
    CMD_ZCL_COPY_SCENE,
    /* Command ZCL ADD SCENE RESPONSE*/
    CMD_ZCL_ADD_SCENE_RESP,
    /* Command ZCL VIEW SCENE RESPONSE*/
    CMD_ZCL_VIEW_SCENE_RESP,
    /* Command ZCL REMOVE SCENE RESPONSE*/
    CMD_ZCL_REMOVE_SCENE_RESP,
    /* Command ZCL REMOVE ALL SCENE RESPONSE*/
    CMD_ZCL_REMOVE_ALL_SCENES_RESP,
    /* Command ZCL STORE SCENE RESPONSE*/
    CMD_ZCL_STORE_SCENE_RESP,
    /* Command ZCL GET SCENE MEMBERSHIP RESPONSE*/
    CMD_ZCL_GET_SCENE_MEMBERSHIP_RESP,
    /* Command ZCL enhance add scene response*/
    CMD_ZCL_ENHANCED_ADD_SCENE_RESP,
    /* Command ZCL ENHANCED VIEW SCENE RESPONSE*/
    CMD_ZCL_ENHANCED_VIEW_SCENE_RESP,
    /* Command ZCL COPY SCENE RESPONSE*/
    CMD_ZCL_COPY_SCENE_RESP,
    /* Command ZCL ColorControlAttributeEventInd */
    CMD_ZCL_ATTR_COLOR_CONTROL,
    /* Command ZCL IdentifyAttributeEventInd */
    CMD_ZCL_ATTR_IDENTIFY,
    /* Command ZCL LevelControlAttributeEventInd */
    CMD_ZCL_ATTR_LEVEL_CONTROL,
    /* Command ZCL OnOffAttributeEventInd */
    CMD_ZCL_ATTR_ONOFF,
    /* Command ZCL OccupancyAttributeEventInd */
    CMD_ZCL_ATTR_OCCUPANCY,
    /* Command ZCL OnOffReportInd */
    CMD_ZCL_REPORTING_ONOFF,
    /* Command ZCL LevelReportInd */
    CMD_ZCL_REPORTING_LEVEL,
    /* Command ZCL OccupancySensorReportInd */
    CMD_ZCL_REPORTING_OCCUPANCY,
    /* Command ZCL Report attribute Indication*/
     CMD_ZCL_REPORTING_COLOR_CONTROL,



} APP_Zigbee_EventId_t;

/*******************************************************************************
 Application Zigbee Data
******************************************************************************/
typedef union
{
    bool state;
    uint8_t id;
    uint8_t value;
    uint16_t data;
    struct
    {
        uint8_t status;
        uint8_t ep;
        uint16_t shortAddress;
        uint64_t extendedAddress;
        uint64_t extendedPanId;
    }ParentChildInfo;
    struct
    {
        uint16_t hue;
        uint8_t saturation;
    }colorHS;
    struct
    {
        uint16_t x;
        uint16_t y;
    }colorXY;
    struct
    {
        ZCL_Addressing_t *addressing;
        uint8_t payloadLength;
        uint8_t *payload;
    }zclEventData;
    struct
    {
        ZCL_Addressing_t *addressing;
        ZCL_AttributeId_t attributeId;
        ZCL_AttributeEvent_t event;
    }zclAttributeData;    
}APP_Zigbee_EventData;

/*******************************************************************************
 Application Zigbee Event Structure
******************************************************************************/
typedef struct
{
    APP_Zigbee_EventGroup_t eventGroup;
    APP_Zigbee_EventId_t eventId;
    APP_Zigbee_EventData eventData;    
}APP_Zigbee_Event_t;

/*******************************************************************************
  Function:
    void APP_Zigbee_Handler(APP_Zigbee_Event_t event)
******************************************************************************/
void APP_Zigbee_Handler(APP_Zigbee_Event_t event);

#endif