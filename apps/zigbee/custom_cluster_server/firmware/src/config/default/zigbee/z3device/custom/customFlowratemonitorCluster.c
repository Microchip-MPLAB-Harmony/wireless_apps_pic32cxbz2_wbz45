/*******************************************************************************
  ZCC0 Custom Cluster Source File

  Company:
    Microchip Technology Inc.

  File Name:
   customFlowratemonitorCluster.c

  Summary:
    This file contains the Custom Cluster interface.

  Description:
    This file contains the Custom Cluster interface.
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


/******************************************************************************
                    Includes section
******************************************************************************/

#include <z3device/custom/include/customFlowratemonitorCluster.h>
#include <zcl/include/zclFlowratemonitorCluster.h>

#include <z3device/clusters/include/haClusters.h>
#include <zcl/include/zclCommandManager.h>
#include <app_zigbee/app_zigbee_handler.h>
#include <app_zigbee/zigbee_console/console.h>


/*******************************************************************************
                    Prototypes section
*******************************************************************************/

static void customFlowratemonitorAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event);

/******************************************************************************
                    Global variables
******************************************************************************/


//Custom Cluster Attributes
ZCL_FlowratemonitorClusterServerAttributes_t flowratemonitorServerClusterAttributes =
{
    ZCL_DEFINE_FLOWRATEMONITOR_CLUSTER_SERVER_ATTRIBUTES(FLOWRATEMONITOR_MIN_REPORT_PERIOD,FLOWRATEMONITOR_MAX_REPORT_PERIOD)
};

//backup attributes
ZCL_FlowratemonitorClusterServerAttributes_t __attribute__((persistent)) backupFlowratemonitorServerAttribute;

//Custom Cluster Commands
PROGMEM_DECLARE (ZCL_FlowratemonitorClusterCommands_t flowratemonitorClusterCommands) =
{
 
ZCL_DEFINE_FLOWRATEMONITOR_CLUSTER_COMMANDS( )



};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initialization function for the cluster
******************************************************************************/
void customFlowratemonitorInit(void)
{    
    /* Executes only for Server. */ 
    ZCL_Cluster_t *serverCluster =  ZCL_GetCluster(APP_ENDPOINT_CUSTOM, FLOWRATEMONITOR_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);

    if (serverCluster)
        serverCluster->ZCL_AttributeEventInd = customFlowratemonitorAttributeEventInd;


    // Fill definition here
}

/**************************************************************************//**
\brief Attribute event (writing/reading) callback.

\param[in] addressing - incoming request addressing information.
\param[in] attributeId - attribute identifier.
\param[in] event - attribute event (read/write).
******************************************************************************/
static void customFlowratemonitorAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event)
{
    APP_Zigbee_Event_t eventItem;
    eventItem.eventGroup = EVENT_CLUSTER;
    eventItem.eventId = CMD_ZCL_ATTR_FLOWRATEMONITOR; //defined in app_zigbee_handler
    eventItem.eventData.zclAttributeData.addressing = addressing;
    eventItem.eventData.zclAttributeData.attributeId = attributeId;
    eventItem.eventData.zclAttributeData.event = event;

  APP_Zigbee_Handler(eventItem);
}






void customccZCC0BackupAttribute(void)
{
    memcpy4ByteAligned(&backupFlowratemonitorServerAttribute, &flowratemonitorServerClusterAttributes, sizeof(ZCL_FlowratemonitorClusterServerAttributes_t));
}

void customccZCC0RestoreAttribute(void)
{
    memcpy4ByteAligned(&flowratemonitorServerClusterAttributes, &backupFlowratemonitorServerAttribute, sizeof(ZCL_FlowratemonitorClusterServerAttributes_t));
}
