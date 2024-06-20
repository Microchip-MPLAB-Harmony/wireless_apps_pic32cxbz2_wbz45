/*******************************************************************************
  Light clusters Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lightClusters.h

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

#ifndef LIGHTCLUSTER_H
#define LIGHTCLUSTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl/include/zcl.h>

/******************************************************************************
                    Definitions section
******************************************************************************/



#if (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_ON_OFF_LIGHT)
#define LIGHT_SERVER_CLUSTERS_COUNT     6U 
#elif APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_DIMMABLE_LIGHT
#define LIGHT_SERVER_CLUSTERS_COUNT     7U 
#elif (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_LIGHT) || (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT) || (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT)
#define LIGHT_SERVER_CLUSTERS_COUNT     8U 
#endif

#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
  #ifdef OTAU_CLIENT
    #define LIGHT_CLIENT_CLUSTERS_COUNT   4U 
  #else
    #define LIGHT_CLIENT_CLUSTERS_COUNT   3U 
  #endif
#else // no extra clusters
  #ifdef OTAU_CLIENT
    #define LIGHT_CLIENT_CLUSTERS_COUNT   3U 
  #else
    #define LIGHT_CLIENT_CLUSTERS_COUNT   2U 
  #endif
#endif // ZLO_EXTRA_CLUSTERS_SUPPORT == 1

#define LIGHT_SERVER_CLUSTER_INIT_COUNT    LIGHT_SERVER_CLUSTERS_COUNT

#define LIGHT_CLIENT_CLUSTER_INIT_COUNT    LIGHT_CLIENT_CLUSTERS_COUNT

/******************************************************************************
                    Externals
******************************************************************************/
extern ZCL_Cluster_t lightServerClusters[LIGHT_SERVER_CLUSTERS_COUNT];
extern ClusterId_t lightServerClusterIds[LIGHT_SERVER_CLUSTERS_COUNT];
extern void (*lightServerClusterInitFunctions[LIGHT_SERVER_CLUSTER_INIT_COUNT])(void);

extern ZCL_Cluster_t lightClientClusters[LIGHT_CLIENT_CLUSTERS_COUNT];
extern ClusterId_t lightClientClusterIds[LIGHT_CLIENT_CLUSTERS_COUNT];
extern void (*lightClientClusterInitFunctions[LIGHT_CLIENT_CLUSTER_INIT_COUNT])(void);


#endif // LIGHTCLUSTER_H

// eof lightClusters.h
