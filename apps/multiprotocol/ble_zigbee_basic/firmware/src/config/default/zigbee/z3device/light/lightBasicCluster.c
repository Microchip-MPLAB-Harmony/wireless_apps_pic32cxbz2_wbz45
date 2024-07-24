/*******************************************************************************
  Light Basic cluster Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightBasicCluster.c

  Summary:
    This file contains the Light Basic cluster interface.

  Description:
    This file contains the Light Basic cluster interface.
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
#include <z3device/light/include/lightBasicCluster.h>
#include <z3device/clusters/include/basicCluster.h>
#include <z3device/light/include/lightClusters.h>
#include <pds/include/wlPdsMemIds.h>
#include <zcl/include/zclCommandManager.h>
#include <app_zigbee/app_zigbee_handler.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
static ZCL_Status_t resetToFactoryDefaultsInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
/******************************************************************************
                    Global variables section
******************************************************************************/
ZCL_BasicClusterServerAttributes_t lightBasicClusterServerAttributes =
{
  ZCL_DEFINE_BASIC_CLUSTER_SERVER_ATTRIBUTES()
};

#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
ZCL_BasicClusterClientAttributes_t lightBasicClusterClientAttributes =
{
  ZCL_DEFINE_BASIC_CLUSTER_CLIENT_ATTRIBUTES()
};
#endif

PROGMEM_DECLARE(ZCL_BasicClusterServerCommands_t lightBasicClusterServerCommands) =
{
  ZCL_DEFINE_BASIC_CLUSTER_COMMANDS(resetToFactoryDefaultsInd)
};

static uint8_t swBuild[] = {15, 97, 98, 99, 100, 101, 102, 103, 104} ;//'a','b','c','d','e','f','g','h'};
static uint8_t productURL[] = {31, 11, 119, 119, 46, 109, 105, 99, 114, 111, 99, 104, 105, 112}; //www.microchip.com
static uint8_t productCode[]= {31, 88, 89, 90, 49, 50, 51,}; //XYZ123
static uint8_t dateCode[]= {15, 50, 48, 49, 53, 48, 57, 48, 49 }; //20150901
static uint8_t modelIdentifier[] = {31, 90, 76, 79, 68, 101, 118, 105, 99, 101}; //ZLODevice
static uint8_t manufacturerName[] = {31, 77, 105, 99 ,114 ,111 ,99 ,104 ,105, 112, 32}; //Microchip

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Basic cluster
******************************************************************************/
void lightBasicClusterInit(void)
{
  ZCL_Cluster_t *cluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT, BASIC_CLUSTER_ID, ZCL_CLUSTER_SIDE_CLIENT);

  lightBasicClusterServerAttributes.clusterVersion.value      = BASIC_CLUSTER_VERSION;
  lightBasicClusterServerAttributes.zclVersion.value          = ZCL_VERSION;
  lightBasicClusterServerAttributes.powerSource.value         = POWER_SOURCE;
  lightBasicClusterServerAttributes.physicalEnvironment.value = UNKNOWN_ENVIRONMENT;
  lightBasicClusterServerAttributes.applicationVersion.value  = APP_VERSION;
  lightBasicClusterServerAttributes.stackVersion.value        = STACK_VERSION;
  lightBasicClusterServerAttributes.hwVersion.value           = HW_VERSION;
  (void)memcpy(lightBasicClusterServerAttributes.manufacturerName.value, manufacturerName, sizeof(manufacturerName));
  (void)memcpy(lightBasicClusterServerAttributes.modelIdentifier.value, modelIdentifier, sizeof(modelIdentifier));
  (void)memcpy(lightBasicClusterServerAttributes.dateCode.value, dateCode, sizeof(dateCode));

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  lightBasicClusterServerAttributes.genericDeviceClass.value  = LIGHTING_DEVICE_CLASS;
  lightBasicClusterServerAttributes.genericDeviceType.value   = LED_BULB_DEVICE_TYPE;
  (void)memcpy(lightBasicClusterServerAttributes.productCode.value, productCode, sizeof(productCode));
  (void)memcpy(lightBasicClusterServerAttributes.productURL.value, productURL, sizeof(productURL));
#endif

  (void)memcpy(lightBasicClusterServerAttributes.swBuildId.value, swBuild, sizeof(swBuild));
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
  lightBasicClusterClientAttributes.clusterVersion.value      = BASIC_CLUSTER_VERSION;
#endif
  //Yog Workaround needs to check :::::  cluster->ZCL_DefaultRespInd = commandZclDefaultResp;
}

/**************************************************************************//**
\brief Callback on receive of resetToFactoryDefaults command
******************************************************************************/
static ZCL_Status_t resetToFactoryDefaultsInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_RESET_TO_FACTORY_DEFAULTS;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = payload;
  
  for (uint8_t i=0; i<LIGHT_SERVER_CLUSTER_INIT_COUNT; i++)
  {
    if ((lightServerClusterInitFunctions[i]) != NULL)
    {
      (lightServerClusterInitFunctions[i])();
    }
  }

  for (uint8_t i=0; i < LIGHT_CLIENT_CLUSTER_INIT_COUNT; i++)
  {
    if ((lightClientClusterInitFunctions[i]) != NULL)
    {
      (lightClientClusterInitFunctions[i])();
    }
  }
   (void)PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
/**************************************************************************//**
\brief Sends resetToFactoryDefaults command unicastly

\param[in] mode - address mode;
\param[in] addr - short address of destination node;
\param[in] ep   - destination endpoint;
******************************************************************************/
void basicResetToFactoryDefaultsCommand(APS_AddrMode_t mode,ShortAddr_t addr, Endpoint_t ep, Endpoint_t srcEp)
{
  ZCL_Request_t *req;
  req = getFreeCommand();
  if (req == NULL)
  {
    return;
  }
  fillCommandRequest(req, ZCL_BASIC_CLUSTER_SERVER_RESET_TO_FACTORY_DEFAULTS_COMMAND_ID, 0, srcEp);
  fillDstAddressing(&req->dstAddressing, mode, addr, ep, BASIC_CLUSTER_ID);

  ZCL_CommandManagerSendCommand(req);
}

#endif //#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1

#endif // APP_DEVICE_TYPE_DIMMABLE_LIGHT

// eof lightBasicCluster.c
