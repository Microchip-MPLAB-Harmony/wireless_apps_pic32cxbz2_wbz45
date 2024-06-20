/*******************************************************************************
  Light clusters Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightClusters.c

  Summary:
    This file contains the Light clusters interface.

  Description:
    This file contains the Light clusters interface.
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
#include <zcl/clusters/include/identifyCluster.h>
#include <z3device/light/include/lightClusters.h>
#include <z3device/light/include/lightBasicCluster.h>
#include <z3device/light/include/lightIdentifyCluster.h>
#include <z3device/light/include/lightOnOffCluster.h>
#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightColorControlCluster.h>
#include <z3device/light/include/lightGroupsCluster.h>
#include <z3device/light/include/lightScenesCluster.h>
#include <z3device/light/include/lightCommissioningCluster.h>


/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_Cluster_t lightServerClusters[LIGHT_SERVER_CLUSTERS_COUNT] =
{
  ZCL_DEFINE_BASIC_CLUSTER_SERVER(&lightBasicClusterServerAttributes, &lightBasicClusterServerCommands),
  DEFINE_IDENTIFY_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &lightIdentifyClusterServerAttributes, &lightIdentifyCommands),
  DEFINE_GROUPS_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &lightGroupsClusterServerAttributes, &lightGroupsCommands),
  DEFINE_SCENES_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &lightScenesClusterServerAttributes, &lightScenesCommands),
  DEFINE_ONOFF_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &lightOnOffClusterServerAttributes, &lightOnOffCommands)
#if APP_Z3_DEVICE_TYPE >=APP_DEVICE_TYPE_DIMMABLE_LIGHT
  , DEFINE_LEVEL_CONTROL_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &lightLevelControlClusterServerAttributes, &lightLevelControlCommands)
#endif
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
  , DEFINE_COLOR_CONTROL_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &lightColorControlClusterServerAttributes, &lightColorControlClusterServerCommands)
#endif
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT
  , DEFINE_COMMISSIONING_CLUSTER(ZCL_SERVER_CLUSTER_TYPE, &lightCommissioningClusterServerAttributes, &lightCommissioningClusterServerCommands)
#endif
};

void (*lightServerClusterInitFunctions[LIGHT_SERVER_CLUSTER_INIT_COUNT])(void) =
{
  lightBasicClusterInit,
  lightIdentifyClusterInit,
  lightGroupsClusterInit,
  lightScenesClusterInit,
  lightOnOffClusterInit
#if APP_Z3_DEVICE_TYPE >=APP_DEVICE_TYPE_DIMMABLE_LIGHT
  , lightLevelControlClusterInit
#endif
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
  , lightColorControlClusterInit
#endif
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT
  , lightCommissioningClusterInit
#endif
};


ClusterId_t lightServerClusterIds[LIGHT_SERVER_CLUSTERS_COUNT] =
{
  BASIC_CLUSTER_ID,
  IDENTIFY_CLUSTER_ID,
  GROUPS_CLUSTER_ID,
  SCENES_CLUSTER_ID,
  ONOFF_CLUSTER_ID
#if APP_Z3_DEVICE_TYPE >=APP_DEVICE_TYPE_DIMMABLE_LIGHT
  , LEVEL_CONTROL_CLUSTER_ID
#endif
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
  , COLOR_CONTROL_CLUSTER_ID
#endif
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT
  , ZLL_COMMISSIONING_CLUSTER_ID
#endif
};

ZCL_Cluster_t lightClientClusters[LIGHT_CLIENT_CLUSTERS_COUNT] =
{
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
  ZCL_DEFINE_BASIC_CLUSTER_CLIENT(&lightBasicClusterClientAttributes),
#endif
  DEFINE_IDENTIFY_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, &lightIdentifyClusterClientAttributes, &lightIdentifyCommands),
  DEFINE_GROUPS_CLUSTER(ZCL_CLIENT_CLUSTER_TYPE, &lightGroupsClusterClientAttributes, &lightGroupsCommands)  
};


ClusterId_t lightClientClusterIds[LIGHT_CLIENT_CLUSTERS_COUNT] =
{
  IDENTIFY_CLUSTER_ID,
  GROUPS_CLUSTER_ID
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
  ,BASIC_CLUSTER_ID
#endif
#ifdef OTAU_CLIENT
  ,OTAU_CLUSTER_ID // Always should be on last position in list of clusters ID.
#endif
};

void (*lightClientClusterInitFunctions[LIGHT_CLIENT_CLUSTER_INIT_COUNT])(void) =
{
  lightIdentifyClusterInit,
  lightGroupsClusterInit
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
  ,lightBasicClusterInit
#endif
#ifdef OTAU_CLIENT
  ,NULL // oatu client init fucntion
#endif
};


#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT

// eof lightClusters.c
