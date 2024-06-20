/*******************************************************************************
  Light device Color Control cluster Header File

  Company:
    Microchip Technology Inc.

  File Name:
    lightColorControlCluster.h

  Summary:
    This file contains the Light device Color Control cluster interface.

  Description:
    This file contains the Light device Color Control cluster interface.
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

#ifndef LIGHTCOLORCONTROLCLUSTER_H
#define LIGHTCOLORCONTROLCLUSTER_H

/******************************************************************************
                    Includes
******************************************************************************/
#include <zcl/include/zcl.h>
#include <zcl/include/zclZllColorControlCluster.h>
#include <z3device/common/include/z3Device.h>
#include <zcl/clusters/include/scenes.h>


/******************************************************************************
                    Defines section
******************************************************************************/
//Updating all the MAX_REPORT values in the range(0x003d - 0x00fe)
/* Same period is used for all attributes */
#define COLOR_CONTROL_CUURENT_HUE_MIN_REPORT_PERIOD 6
#define COLOR_CONTROL_CUURENT_HUE_MAX_REPORT_PERIOD 88

#define COLOR_CONTROL_CURRENT_SAT_MIN_REPORT_PERIOD 8
#define COLOR_CONTROL_CURRENT_SAT_MAX_REPORT_PERIOD 92

#define COLOR_CONTROL_CURR_X_MIN_REPORT_PERIOD 10
#define COLOR_CONTROL_CURR_X_MAX_REPORT_PERIOD 96

#define COLOR_CONTROL_CURR_Y_MIN_REPORT_PERIOD 14
#define COLOR_CONTROL_CURR_Y_MAX_REPORT_PERIOD 100

#define COLOR_CONTROL_COLOR_TEMP_MIN_REPORT_PERIOD 18
#define COLOR_CONTROL_COLOR_TEMP_MAX_REPORT_PERIOD 104

/******************************************************************************
                    Externals
******************************************************************************/
extern ZCL_ColorControlClusterServerAttributes_t lightColorControlClusterServerAttributes;
extern PROGMEM_DECLARE(ZCL_ColorControlClusterCommands_t lightColorControlClusterServerCommands);

/******************************************************************************
                    Prototypes
******************************************************************************/
void lightColorControlClusterInit(void);
void colorControlClusterSetExtensionField(Scene_t *scene, uint16_t transitionTime);

/**************************************************************************//**
\brief Show identify effect.

\param[in] enhancedHue - enhanced hue to move to.
******************************************************************************/
void colorControlShowIdentifyEffect(uint16_t enhancedHue);

/**************************************************************************//**
\brief Stop showing identify effect and return to previous state.
******************************************************************************/
void colorControlStopIdentifyEffect(void);

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

/**************************************************************************//**
\brief Update the color temperature on level change if required

\param[in] deviceOn - The on/off status of the device
\param[in] updateColorTemp - Update coupleColorTemp bit overridden in optional fields
\param[in] level    - level the device is moving to
******************************************************************************/
void lightUpdateCoupledColorTemperature(bool deviceOn, bool updateColorTemp, uint8_t level);

#endif /* #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT */
#endif // LIGHTCOLORCONTROLCLUSTER_H

// eof lightColorControlCluster.h
