/*******************************************************************************
  Light Source File

  Company:
    Microchip Technology Inc.

  File Name:
    light.c

  Summary:
    This file contains the Light interface.

  Description:
    This file contains the Light interface.
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

#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT)

/******************************************************************************
                             Includes section
******************************************************************************/
#include <z3device/clusters/include/haClusters.h>
#include <z3device/light/include/lightClusters.h>
#include <zcl/clusters/include/identifyCluster.h>
#include <z3device/light/include/lightBasicCluster.h>
#include <z3device/light/include/lightIdentifyCluster.h>
#include <z3device/light/include/lightOnOffCluster.h>
#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightColorControlCluster.h>
#include <z3device/light/include/lightGroupsCluster.h>
#include <z3device/light/include/lightScenesCluster.h>
#include <z3device/light/include/lightCommissioningCluster.h>
#include <z3device/common/include/z3Device.h>
#include <zcl/include/zclSecurityManager.h>
#include <app_zigbee/zigbee_console/console.h>
#include <pds/include/wlPdsMemIds.h>
#include <z3device/common/include/otauService.h>
#include <zcl/include/zclCommandManager.h>
#include <z3device/light/include/light.h>
#include <z3device/clusters/include/scenesCluster.h>
#include <z3device/clusters/include/commissioningCluster.h>
#ifdef OTAU_CLIENT
#include <zcl/include/zclOtauManager.h>
#endif


/******************************************************************************
                   type(s) section
******************************************************************************/

/*******************************************************************************
                    Static functions section
*******************************************************************************/
static void lightFindingBindingFinishedForACluster(Endpoint_t ResponentEp, ClusterId_t clusterId);
#if (ZB_COMMISSIONING_ON_STARTUP == 1)
#ifdef _ZCL_REPORTING_SUPPORT_
static void lightConfigureReportingResp(ZCL_Notify_t *ntfy);
#endif
#endif

#ifdef OTAU_CLIENT 
static void lightAddOTAUClientCluster(void);
static void configureImageKeyDone(void);
#endif
/******************************************************************************
                    Local variables section
******************************************************************************/
static ZCL_DeviceEndpoint_t lightEndpoint =
{
  .simpleDescriptor =
  {
    .endpoint            = APP_ENDPOINT_LIGHT,
    .AppProfileId        = PROFILE_ID_HOME_AUTOMATION,
    .AppDeviceId         = (uint16_t)APP_Z3DEVICE_ID,
    .AppDeviceVersion    = 0x1,
    .AppInClustersCount  = ARRAY_SIZE(lightServerClusterIds),
    .AppInClustersList   = lightServerClusterIds,
    .AppOutClustersCount = ARRAY_SIZE(lightClientClusterIds),
    .AppOutClustersList  = lightClientClusterIds,
  },
  .serverCluster = lightServerClusters,
  .clientCluster = lightClientClusters,
};

static ClusterId_t lightClientClusterToBindIds[] =
{
  IDENTIFY_CLUSTER_ID,
  GROUPS_CLUSTER_ID,  
};

static ClusterId_t lightServerClusterToBindIds[] =
{
  BASIC_CLUSTER_ID,
  IDENTIFY_CLUSTER_ID,
  GROUPS_CLUSTER_ID,
  SCENES_CLUSTER_ID,
  ONOFF_CLUSTER_ID,
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT)
  LEVEL_CONTROL_CLUSTER_ID,
#endif
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT)
  COLOR_CONTROL_CLUSTER_ID,
#endif  
};

static AppBindReq_t lightBindReq =
{
  .srcEndpoint       = APP_ENDPOINT_LIGHT,
  .remoteServersCnt  = ARRAY_SIZE(lightClientClusterToBindIds),
  .remoteClientsCnt  = ARRAY_SIZE(lightServerClusterToBindIds),
  .groupId           = 0xffff,
  .remoteServers     = lightClientClusterToBindIds,
  .remoteClients     = lightServerClusterToBindIds,
  .callback          = lightFindingBindingFinishedForACluster,
  .startIdentifyingFn= lightIdetifyStartIdentifyingCb
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Device initialization routine
******************************************************************************/
void appDeviceInit(void)
{

#if APP_ENABLE_CONSOLE == 1
  initConsole();
#endif

  LCD_INIT();
  LEDS_OPEN();  

#if (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_ON_OFF_LIGHT)
      LCD_PRINT(0, 1, "On Off Light");
#elif (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_DIMMABLE_LIGHT)
      LCD_PRINT(0, 1, "Dimmable Light");
#elif (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_LIGHT)
      LCD_PRINT(0, 1, "Color Light");
#elif (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT)
      LCD_PRINT(0, 1, "Extended Color Light");
#elif (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT)
      LCD_PRINT(0, 1, "Temperature Color Light"); 
#endif
  if (!APP_RegisterEndpoint(&lightEndpoint, &lightBindReq))
  {
    return;
  }
  ZCL_CommandManagerInit();
  lightBasicClusterInit();
  lightIdentifyClusterInit();
  lightGroupsClusterInit();
  lightScenesClusterInit();
  lightOnOffClusterInit();

#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT)
 lightLevelControlClusterInit();
#endif

#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT)
  lightColorControlClusterInit();
#endif
  
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT)
  lightCommissioningClusterInit();
#endif

#ifdef OTAU_CLIENT
  lightAddOTAUClientCluster();
#endif //OTAU_CLIENT

#ifdef _ENABLE_PERSISTENT_SERVER_
  if (PDS_IsAbleToRestore(APP_LIGHT_SCENES_MEM_ID))
  {
    (void)PDS_Restore(APP_LIGHT_SCENES_MEM_ID);
  }
#endif
  /**CHANGE* - cluster version need to be reinitilized here after PDS Restore same for all devices
    implement a common function to reinitilized */
}

/**************************************************************************//**
\brief Device common task handler
******************************************************************************/
void appDeviceTaskHandler(void)
{
  switch (appDeviceState) // Actual device state when one joined network
  {
    case DEVICE_INITIAL_STATE:
      {
        appDeviceState = DEVICE_ACTIVE_IDLE_STATE;
      }
#ifdef OTAU_CLIENT
      startOtauClient();
#endif
      break;
    case DEVICE_ACTIVE_IDLE_STATE:
    default:
      break;
  }
}

/*******************************************************************************
\brief callback called on the finishing of binding of one cluster
********************************************************************************/
static void lightFindingBindingFinishedForACluster(Endpoint_t ResponentEp, ClusterId_t clusterId)
{
  ZCL_Cluster_t *serverCluster;
  switch(clusterId)
  {
    case ONOFF_CLUSTER_ID:
      serverCluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
      if (serverCluster != NULL)
      {
#ifdef _ZCL_REPORTING_SUPPORT_
#if (ZB_COMMISSIONING_ON_STARTUP == 1)
        sendConfigureReportingToNotify(APP_ENDPOINT_LIGHT, 0, ONOFF_CLUSTER_ID,
                                     ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID, ONOFF_VAL_MAX_REPORT_PERIOD,
                                     lightConfigureReportingResp);
#else
        ZCL_StartReporting();
#endif
#endif
      }
      break;
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT
    case LEVEL_CONTROL_CLUSTER_ID:
       serverCluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT, LEVEL_CONTROL_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
       if (serverCluster != NULL)
       {
#if (ZB_COMMISSIONING_ON_STARTUP == 1)
#ifdef _ZCL_REPORTING_SUPPORT_
         sendConfigureReportingToNotify(APP_ENDPOINT_LIGHT, 0,
                                      LEVEL_CONTROL_CLUSTER_ID,ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID,
                                      LEVEL_CONTROL_VAL_MAX_REPORT_PERIOD, lightConfigureReportingResp);
#endif
#else
       ZCL_StartReporting();
#endif
       }
      break;
#endif
  }

}

#if (ZB_COMMISSIONING_ON_STARTUP == 1)
#ifdef _ZCL_REPORTING_SUPPORT_
/**************************************************************************//**
\brief Indication of configure reporting response

\param[in] ntfy - pointer to response
******************************************************************************/
static void lightConfigureReportingResp(ZCL_Notify_t *ntfy)
{
  ZCL_StartReporting();
  (void)ntfy;
}
#endif
#endif
/**************************************************************************//**
\brief Stops identifying on endpoints
******************************************************************************/
void appIdentifyStart(uint16_t identifyTime, bool colorEffect, uint16_t enhancedHue)
{
  lightIdentifyStart(identifyTime, colorEffect, enhancedHue);
}

/**************************************************************************//**
\brief Stops identifying on endpoints
******************************************************************************/
void appIdentifyStop(void)
{
  lightIdentifyStop();
}

#ifdef OTAU_CLIENT 
/**************************************************************************//**
\brief Adds OTAU client cluster to list of clients clusters of light devices
******************************************************************************/
static void lightAddOTAUClientCluster(void)
{
  lightClientClusters[LIGHT_CLIENT_CLUSTERS_COUNT - 1U] = ZCL_GetOtauClientCluster();
}
#endif // OTAU_CLIENT

#endif // APP_DEVICE_TYPE_DIMMABLE_LIGHT
// eof dimmableLight.c