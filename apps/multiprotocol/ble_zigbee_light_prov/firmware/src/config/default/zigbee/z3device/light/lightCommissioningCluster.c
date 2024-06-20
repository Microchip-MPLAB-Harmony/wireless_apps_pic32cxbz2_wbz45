/*******************************************************************************
  Light device touchlink commissioning cluster Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightCommissioningCluster.c

  Summary:
    This file contains the Light device touchlink commissioning cluster interface.

  Description:
    This file contains the Light device touchlink commissioning cluster interface.
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

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT

/******************************************************************************
                    Includes section
******************************************************************************/
#include <z3device/clusters/include/haClusters.h>
#include <z3device/common/include/appConsts.h>
#include <zcl/include/zclCommandManager.h>
#include <z3device/clusters/include/commissioningCluster.h>
#include <z3device/light/include/lightCommissioningCluster.h>
#include <app_zigbee/zigbee_console/console.h>

/******************************************************************************
                    Prototypes
******************************************************************************/
static ZCL_Status_t getGroupIdentifiersInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetGroupIdentifiers_t *payload);
static ZCL_Status_t getEndpointListInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetEndpointList_t *payload);

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_CommissioningClusterServerAttributes_t lightCommissioningClusterServerAttributes =
{
  ZCL_DEFINE_COMMISSIONING_CLUSTER_SERVER_ATTRIBUTES()
};

ZCL_CommissioningClusterCommands_t lightCommissioningClusterServerCommands =
{
  ZCL_DEFINE_COMMISSIONING_CLUSTER_COMMANDS(getGroupIdentifiersInd,
                                            getEndpointListInd,
                                            NULL,
                                            NULL,
                                            NULL)
};

/******************************************************************************
                    Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initialize Commissioning cluster.
******************************************************************************/
void lightCommissioningClusterInit(void)
{
  ZCL_Cluster_t *commissioningCluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT,
                                                       ZLL_COMMISSIONING_CLUSTER_ID,
                                                       ZCL_CLUSTER_SIDE_SERVER);

  lightCommissioningClusterServerAttributes.clusterVersion.value = COMMISSIONING_CLUSTER_VERSION;
  if (commissioningCluster != NULL)
  {
    commissioningCluster->ZCL_DefaultRespInd = ZCL_CommandZclDefaultResp;
  }
}

/**************************************************************************//**
\brief Callback on receive of Get Group Identifiers command
******************************************************************************/
static ZCL_Status_t getGroupIdentifiersInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetGroupIdentifiers_t *payload)
{
  ZCL_Request_t *req;
  uint8_t size;

  appSnprintf("getGroupIdentifiersInd(), startIndex=%02d\r\n", payload->startIndex);
  req = getFreeCommand();
  if (req == NULL)
  {
   return ZCL_INSUFFICIENT_SPACE_STATUS;
  }
  
  size = fillGetGroupIdentifiersResponsePayload((ZCL_GetGroupIdentifiersResponse_t *)req->requestPayload,  payload->startIndex);
  fillCommandRequest(req, ZCL_COMMISSIONING_CLUSTER_GET_GROUP_IDENTIFIERS_RESPONSE_COMMAND_ID, size, APP_SRC_ENDPOINT_ID);
  fillDstAddressingViaSourceAddressing(&req->dstAddressing, addressing, ZCL_CLUSTER_SIDE_CLIENT);
  ZCL_CommandManagerSendCommand(req);

  (void) payloadLength;
  return ZCL_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Callback on receive of Get Endpoint List command
******************************************************************************/
static ZCL_Status_t getEndpointListInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_GetEndpointList_t *payload)
{
  ZCL_Request_t *req;
  uint8_t size;

  appSnprintf("getEndpointListInd(), startIndex=%02d\r\n", payload->startIndex);
  req = getFreeCommand();
  if (req == NULL)
  {
   return ZCL_INSUFFICIENT_SPACE_STATUS;
  }
  
  size = fillGetEndpointListResponsePayload((ZCL_GetEndpointListResponse_t *)req->requestPayload,  payload->startIndex);
  fillCommandRequest(req, ZCL_COMMISSIONING_CLUSTER_GET_ENDPOINT_LIST_RESPONSE_COMMAND_ID, size, APP_SRC_ENDPOINT_ID);
  fillDstAddressingViaSourceAddressing(&req->dstAddressing, addressing, ZCL_CLUSTER_SIDE_CLIENT);
  ZCL_CommandManagerSendCommand(req);

  (void)payloadLength;
  return ZCL_SUCCESS_STATUS;
}

#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT

// eof lightCommissioningCluster.c
