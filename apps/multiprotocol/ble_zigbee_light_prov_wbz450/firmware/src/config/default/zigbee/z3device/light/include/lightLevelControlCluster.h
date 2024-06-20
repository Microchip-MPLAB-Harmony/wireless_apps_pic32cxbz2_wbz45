/*******************************************************************************
  Dimmable Light Level Control cluster Header File

  Company:
    Microchip Technology Inc.

  File Name:
   lightLevelControlCluster.h

  Summary:
    This file contains the Dimmable Light Level Control cluster interface.

  Description:
    This file contains the Dimmable Light Level Control cluster interface.
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

#ifndef LIGHTLEVELCONTROLCLUSTER_H
#define LIGHTLEVELCONTROLCLUSTER_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include <zcl/include/zcl.h>
#include <zcl/include/zclZllLevelControlCluster.h>

/******************************************************************************
                    Defines section
******************************************************************************/
#define UPDATE_TIMER_INTERVAL              100U // 1/10th of a second as per specification
#define MIN_LIGHT_LEVEL                      1U
#define MAX_LIGHT_LEVEL                    254U
#define MIN_TRANSITION_TIME                  1U
#define DIMMABLE_LIGHT_HUE_LEVEL            35U
#define DIMMABLE_LIGHT_SATURATION_LEVEL     35U

#define LEVEL_CONTROL_VAL_MIN_REPORT_PERIOD 10U
#define LEVEL_CONTROL_VAL_MAX_REPORT_PERIOD 80U

/******************************************************************************
                    Externals
******************************************************************************/
extern ZCL_LevelControlClusterServerAttributes_t lightLevelControlClusterServerAttributes;
extern PROGMEM_DECLARE(ZCL_LevelControlClusterCommands_t         lightLevelControlCommands);

/******************************************************************************
                    Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Level Control cluster
******************************************************************************/
void lightLevelControlClusterInit(void);

/**************************************************************************//**
\brief Stops Level Control cluster
******************************************************************************/
void levelControlClusterStop(void);

/**************************************************************************//**
\brief Gets current color level

\returns current coloe level
******************************************************************************/
uint8_t levelControlCurrentLevel(void);


/**************************************************************************//**
\brief Set Current Level attribute to a specified value
******************************************************************************/

void setLevel(bool wOnOff, bool execIfOff, uint8_t level);

/**************************************************************************//**
\brief Display current level value.
******************************************************************************/


/**************************************************************************//**
\brief Set Extension Field Set
******************************************************************************/
void levelControlClusterSetExtensionField(uint16_t currentLevel, uint16_t transitionTime);

/**************************************************************************//**
\brief Update the current level value to startup current level value.
******************************************************************************/
void lightUpdateStartupLevel(void);

#endif // LIGHTLEVELCONTROLCLUSTER_H

// eof lightLevelControlCluster.h

