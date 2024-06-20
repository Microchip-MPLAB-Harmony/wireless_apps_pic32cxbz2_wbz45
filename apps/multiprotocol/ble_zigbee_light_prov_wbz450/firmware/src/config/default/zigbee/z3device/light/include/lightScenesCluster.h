/*******************************************************************************
  Light Scenes cluster Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lightScenesCluster.h

  Summary:
    This file contains the Light Scenes cluster interface.

  Description:
    This file contains the Light Scenes cluster interface.
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

#ifndef LIGHTSCENESCLUSTER_H
#define LIGHTSCENESCLUSTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl/include/zclZllScenesCluster.h>
#include <zcl/clusters/include/scenes.h>

/******************************************************************************
                    Externals
******************************************************************************/
extern ZCL_SceneClusterServerAttributes_t lightScenesClusterServerAttributes;
extern PROGMEM_DECLARE(ZCL_ScenesClusterCommands_t        lightScenesCommands);


extern Scene_t lightSceneTable[];

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Scenes cluster
******************************************************************************/
void lightScenesClusterInit(void);

/**************************************************************************//**
\brief Invalidates scene
******************************************************************************/
void lightScenesInvalidate(void);

/****************************************************************************
\brief Recalls scene from scene table

\param[in] recallScene - the pointer to Recall Scene request
******************************************************************************/
//void recallScene(ZCL_RecallScene_t *recallScene);
//
/****************************************************************************
\brief Stores scene to scene table

\param[in] storeScene - the pointer to Store Scene request

\returns status of scene storing
******************************************************************************/
//ZCL_Status_t storeScene(ZCL_StoreScene_t *storeScene);
#endif // LIGHTSCENESCLUSTER_H

// eof lightScenesCluster.h

