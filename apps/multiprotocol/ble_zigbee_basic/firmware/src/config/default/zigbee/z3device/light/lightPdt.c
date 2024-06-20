/*******************************************************************************
  Light Persistent Data Table Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightPdt.c

  Summary:
    This file contains the Light Persistent Data Table implementation.

  Description:
    This file contains the Light Persistent Data Table implementation.
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
#include <pds/include/wlPdsMemIds.h>
#include <zcl/clusters/include/scenes.h>
#include <z3device/light/include/lightOnOffCluster.h>
#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightColorControlCluster.h>
#include <z3device/light/include/lightScenesCluster.h>
/******************************************************************************
                    PDT definitions
******************************************************************************/
#ifdef _ENABLE_PERSISTENT_SERVER_
/* Dimmable Light application data file descriptors.
   Shall be placed in the PDS_FF code segment. */
PDS_DECLARE_FILE(APP_LIGHT_SCENES_MEM_ID,        MAX_SCENES_AMOUNT * sizeof(Scene_t),                        lightSceneTable,                                           NO_FILE_MARKS);
PDS_DECLARE_FILE(APP_LIGHT_ONOFF_MEM_ID,         (uint16_t)sizeof(lightOnOffClusterServerAttributes),               &lightOnOffClusterServerAttributes,               NO_FILE_MARKS);
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT)
PDS_DECLARE_FILE(APP_LIGHT_LEVEL_CONTROL_MEM_ID, (uint16_t)sizeof(lightLevelControlClusterServerAttributes), &lightLevelControlClusterServerAttributes, NO_FILE_MARKS);
#endif
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT)
PDS_DECLARE_FILE(APP_LIGHT_COLOR_CONTROL_MEM_ID, (uint16_t)sizeof(lightColorControlClusterServerAttributes), &lightColorControlClusterServerAttributes, NO_FILE_MARKS);
#endif


/* Dimmable Light application data file identifiers list.
   Will be placed in flash. */
PROGMEM_DECLARE(PDS_MemId_t appDlMemoryIdsTable[]) =
{
  APP_LIGHT_SCENES_MEM_ID,
  APP_LIGHT_ONOFF_MEM_ID

#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT)
  ,APP_LIGHT_LEVEL_CONTROL_MEM_ID
#endif

#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT)
 ,APP_LIGHT_COLOR_CONTROL_MEM_ID
#endif
};

/* Dimmable Light application directory descriptor.
   Shall be placed in the PDS_FD code segment. */
PDS_DECLARE_DIR(PDS_DirDescr_t appEsiMemoryDirDescr) =
{
  .list       = appDlMemoryIdsTable,
  .filesCount = ARRAY_SIZE(appDlMemoryIdsTable),
  .memoryId   = Z3DEVICE_APP_MEMORY_MEM_ID
};

#endif // _ENABLE_PERSISTENT_SERVER_
#endif // APP_DEVICE_TYPE_DIMMABLE_LIGHT

// eof lightPdt.c
