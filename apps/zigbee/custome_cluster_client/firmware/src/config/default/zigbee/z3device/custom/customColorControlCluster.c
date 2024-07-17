/*******************************************************************************
  Custom Color Control Cluster Source file

  Company:
    Microchip Technology Inc.

  File Name:
    customColorControlCluster.c

  Summary:
    This file contains Custom Color Control Cluster Implementation.

  Description:
    This file contains Custom Color Control Cluster Implementation.
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

#if (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_CUSTOM_DEVICE)

/******************************************************************************
                    Includes
******************************************************************************/
#ifdef BOARD_MEGARF
#include <buttonsExt.h>
#endif

#if defined(BOARD_MEGARF) || defined(BOARD_SAMR21)
#include <ledsExt.h>
#include <lcdExt.h>
#endif

#if (BSP_SUPPORT == BOARD_SAMR21_ZLLEK)
#include <printLcd.h>
#else
#define dbgLcdMsg(...) while (0) {;}
#endif

#include <z3device/common/include/hs2xy.h>

#include <z3device/clusters/include/haClusters.h>
#include <z3device/custom/include/customScenesCluster.h>
#include <z3device/custom/include/customOnOffCluster.h>
#include <z3device/custom/include/customLevelControlCluster.h>
#include <z3device/custom/include/customColorControlCluster.h>
#include <z3device/common/include/z3Device.h>

#include <z3device/custom/include/customColorSchemesConversion.h>

#include <zllplatform/ZLL/N_DeviceInfo/include/N_DeviceInfo_Bindings.h>
#include <zllplatform/ZLL/N_DeviceInfo/include/N_DeviceInfo.h>
#include <pds/include/wlPdsMemIds.h>
#include <zcl/include/zclCommandManager.h>
#include <app_zigbee/zigbee_console/console.h>
#include <app_zigbee/app_zigbee_handler.h>

/******************************************************************************
                    Definitions
******************************************************************************/
#define UPDATE_TIMER_INTERVAL    100 // 1/10th of a second as per specification

#define MIN_HUE_LEVEL             0
#define MAX_HUE_LEVEL             0xfeff
#define MAX_ENHANCED_HUE_LEVEL    0xffff

#define MIN_SATURATION_LEVEL      0
#define MAX_SATURATION_LEVEL      0xfe

#define MIN_COLOR_LEVEL           0
#define MAX_COLOR_LEVEL           0xfeff

#define DEFAULT_SATURATION_LEVEL (MAX_SATURATION_LEVEL / 2)
#define DEFAULT_TRANSITION_TIME  2

#define HUE_SATURATION_SUPPORTED    (1 << 0)
#define ENHANCED_HUE_SUPPORTED      (1 << 1)
#define COLOR_LOOP_SUPPORTED        (1 << 2)
#define X_Y_ATTRIBUTES_SUPPORTED    (1 << 3)
#define COLOR_TEMPERATURE_SUPPORTED (1 << 4)

#define COLOR_LOOP_UPDATE_ACTION    (1 << 0)
#define COLOR_LOOP_UPDATE_DIRECTION (1 << 1)
#define COLOR_LOOP_UPDATE_TIME      (1 << 2)
#define COLOR_LOOP_UPDATE_START_HUE (1 << 3)

#define ABS(x)  ((x > 0) ? (x) : -(x))

#define ABSOLUTE_MAXIMUM_COLOR_TEMP  0xffff
/******************************************************************************
                    Types
******************************************************************************/
typedef enum _TransitionType_t
{
  NONE        = 0x00,
  ALL         = 0xff,
  HUE         = 0x01,
  SATURATION  = 0x02,
  COLOR       = 0x04,
  COLOR_LOOP  = 0x08,
  TEMPERATURE = 0x10,
} TransitionType_t;
/******************************************************************************
                    Prototypes
******************************************************************************/
static ZCL_Status_t moveToHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToHueCommand_t *payload);
static ZCL_Status_t moveHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveHueCommand_t *payload);
static ZCL_Status_t stepHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepHueCommand_t *payload);
static ZCL_Status_t moveToSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToSaturationCommand_t *payload);
static ZCL_Status_t moveSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveSaturationCommand_t *payload);
static ZCL_Status_t stepSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepSaturationCommand_t *payload);
static ZCL_Status_t moveToHueAndSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToHueAndSaturationCommand_t *payload);
static ZCL_Status_t moveToColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToColorCommand_t *payload);
static ZCL_Status_t moveColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveColorCommand_t *payload);
static ZCL_Status_t stepColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepColorCommand_t *payload);
static ZCL_Status_t enhancedMoveToHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveToHueCommand_t *payload);
static ZCL_Status_t enhancedMoveHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveHueCommand_t *payload);
static ZCL_Status_t enhancedStepHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedStepHueCommand_t *payload);
static ZCL_Status_t enhancedMoveToHueAndSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t *payload);
static ZCL_Status_t colorLoopSetInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllColorLoopSetCommand_t *payload);
static ZCL_Status_t stopMoveStepInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload);
static ZCL_Status_t moveToColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToColorTemperatureCommand_t *payload);
static ZCL_Status_t moveColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveColorTemperatureCommand_t *payload);
static ZCL_Status_t stepColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepColorTemperatureCommand_t *payload);

static void setColorMode(uint8_t mode);
static void setHue(uint16_t hue);
static void setSaturation(uint8_t saturation);
static void setColor(uint16_t x, uint16_t y);
static void setColorTemperature(uint16_t temperature);
static void handleColorLoopTransition(void);

static void displayStatus(void);
static void updateTransitionState(void);

static void transitionStart(TransitionType_t type);
static void transitionStop(TransitionType_t type);

static void setColorLoop(uint8_t colorLoopActive, uint8_t colorLoopDirection, uint16_t colorLoopTime);
static void startColorLoop(ZCL_ZllColorLoopAction_t action);
static TransitionType_t prepareMoveToHue(uint16_t hue, uint8_t direction, uint16_t transitionTime,
                                         bool byStep);
static TransitionType_t prepareMoveToSaturation(uint8_t saturation, uint16_t transitionTime,
                                                bool byStep);
static bool prepareMoveToColor(uint16_t x, uint16_t y, uint16_t transitionTime);
static bool prepareMoveToColorTemperature(uint16_t temperature, uint16_t transitionTime, bool byStep);
static void handleColorTemperatureTransition(void);
static bool prepareMoveColorTemperature(uint8_t moveMode, uint16_t rate, uint16_t tempMin, uint16_t tempMax);


static void customUpdateStartupColorTemperature(void);
static void ZCL_ColorControlAttributeEventInd(ZCL_Addressing_t *addressing,
  ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event);
static void customUpdateStartupColorTemperature(void);
static void customColorControlReportInd(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);
/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_ColorControlClusterServerAttributes_t customColorControlClusterServerAttributes =
{
  ZCL_DEFINE_COLOR_CONTROL_SERVER_CLUSTER_ATTRIBUTES(0, 0)
};
ZCL_ColorControlClusterClientAttributes_t customColorControlClusterClientAttributes =
{
  ZCL_DEFINE_COLOR_CONTROL_CLIENT_CLUSTER_ATTRIBUTES()
};
PROGMEM_DECLARE(ZCL_ColorControlClusterCommands_t customColorControlClusterServerCommands) =
{
  ZCL_COLOR_CONTROL_CLUSTER_COMMANDS_FOR_EXTENDED_COLOR_LIGHT(moveToHueInd, moveHueInd, stepHueInd,
      moveToSaturationInd, moveSaturationInd, stepSaturationInd, moveToHueAndSaturationInd, moveToColorInd,
      moveColorInd, stepColorInd, moveToColorTemperatureInd, enhancedMoveToHueInd,
      enhancedMoveHueInd, enhancedStepHueInd, enhancedMoveToHueAndSaturationInd, colorLoopSetInd,
      stopMoveStepInd, moveColorTemperatureInd, stepColorTemperatureInd)
};
PROGMEM_DECLARE(ZCL_ColorControlClusterCommands_t customColorControlClusterClientCommands) =
{
  ZCL_COLOR_CONTROL_CLUSTER_COMMANDS_FOR_COLOR_SCENE_CONTROLLER()
};

/****************************************************************************
*****************************************************************************/

/******************************************************************************
                    Local variables
******************************************************************************/
static HAL_AppTimer_t updateTimer;
static uint8_t bckpSaturation;
static uint16_t bckpEnhacnedHue;

/* gExecuteIfOff:
 * --------------
 * Used with the "Execute If Off" behavior
 *
 * true  => if the device is in OFF state, color control command
 *          WILL BE PROCESSED but there WILL NOT BE change in LED.
 *          i.e., Execute If, the device is, Off
 *
 * false => if the device is in OFF state, color control command
 *           WILL NOT BE PROCESSED
 */
static bool gExecuteIfOff = false;

static TransitionType_t inTransition = NONE;

static struct
{
  uint32_t delta;
  uint32_t current;
  uint16_t target;
  bool     direction;
  bool     byStep;
} hueTransition;

static struct
{
  uint32_t delta;
  uint32_t current;
  uint8_t  target;
  bool     direction;
  bool     byStep;
} saturationTransition;

static struct
{
  uint32_t deltaX;
  uint32_t deltaY;
  uint32_t currentX;
  uint32_t currentY;
  uint16_t targetX;
  uint16_t targetY;
  bool     directionX;
  bool     directionY;
} colorTransition;

static struct
{
  uint32_t delta;
  uint32_t current;
  uint16_t target;
  bool     direction;
  bool     byStep;
  uint16_t minTemp;
  uint16_t maxTemp;
} colorTemperatureTransition;
/******************************************************************************
                    Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initialize Color Control cluster.
******************************************************************************/
void customColorControlClusterInit(void)
{
  ZCL_Cluster_t *cluster =  ZCL_GetCluster(APP_ENDPOINT_CUSTOM, COLOR_CONTROL_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
  if (cluster)
    cluster->ZCL_AttributeEventInd = ZCL_ColorControlAttributeEventInd;

  if (!PDS_IsAbleToRestore(APP_CUSTOM_COLOR_CONTROL_MEM_ID))
  {
    uint8_t colorCapabilities = 1;
    customColorControlClusterServerAttributes.currentHue.value                 = ZCL_ZLL_CLUSTER_CURRENT_HUE_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.currentSaturation.value          = DEFAULT_SATURATION_LEVEL;
    customColorControlClusterServerAttributes.remainingTime.value              = ZCL_ZLL_CLUSTER_REMAINING_TIME_DEFAULT_VALUE;

    customColorControlClusterServerAttributes.currentX.value                   = ZCL_ZLL_CLUSTER_CURRENT_X_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.currentY.value                   = ZCL_ZLL_CLUSTER_CURRENT_Y_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorTemperature.value           = ZCL_ZCL_CLUSTER_COLOR_TEMP_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorMode.value                  = ZCL_ZLL_CLUSTER_COLOR_MODE_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.numberOfPrimaries.value          = 1;

    customColorControlClusterServerAttributes.primary1X.value                  = 41942; // red
    customColorControlClusterServerAttributes.primary1Y.value                  = 20971; // red
    customColorControlClusterServerAttributes.primary1XIntensity.value         = 0;

    customColorControlClusterServerAttributes.primary2X.value                  = 0;
    customColorControlClusterServerAttributes.primary2Y.value                  = 0;
    customColorControlClusterServerAttributes.primary2XIntensity.value         = 0;

    customColorControlClusterServerAttributes.primary3X.value                  = 0;
    customColorControlClusterServerAttributes.primary3Y.value                  = 0;
    customColorControlClusterServerAttributes.primary3XIntensity.value         = 0;

    customColorControlClusterServerAttributes.primary4X.value                  = 0;
    customColorControlClusterServerAttributes.primary4Y.value                  = 0;
    customColorControlClusterServerAttributes.primary4XIntensity.value         = 0;

    customColorControlClusterServerAttributes.primary5X.value                  = 0;
    customColorControlClusterServerAttributes.primary5Y.value                  = 0;
    customColorControlClusterServerAttributes.primary5XIntensity.value         = 0;

    customColorControlClusterServerAttributes.primary6X.value                  = 0;
    customColorControlClusterServerAttributes.primary6Y.value                  = 0;
    customColorControlClusterServerAttributes.primary6XIntensity.value         = 0;

    customColorControlClusterServerAttributes.enhancedCurrentHue.value         = ZCL_ZCL_CLUSTER_ENHANCED_CURRENT_HUE_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.enhancedColorMode.value          = ZCL_ZCL_CLUSTER_ENHANCED_COLOR_MODE_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorLoopActive.value            = ZCL_ZCL_CLUSTER_COLOR_LOOP_ACTIVE_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorLoopDirection.value         = ZCL_ZCL_CLUSTER_COLOR_LOOP_DIRECTION_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorLoopTime.value              = ZCL_ZCL_CLUSTER_COLOR_LOOP_TIME_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorLoopStartEnhancedHue.value  = ZCL_ZCL_CLUSTER_COLOR_LOOP_START_ENHANCED_HUE_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorLoopStoredEnhancedHue.value = ZCL_ZCL_CLUSTER_COLOR_LOOP_STORED_ENHANCED_HUE_DEFAULT_VALUE;

    customColorControlClusterServerAttributes.colorCapabilities.value.hueSaturationSupported = colorCapabilities;
    customColorControlClusterServerAttributes.colorCapabilities.value.enhancedHueSupported   = colorCapabilities;
    customColorControlClusterServerAttributes.colorCapabilities.value.colorLoopSupported     = colorCapabilities;
    customColorControlClusterServerAttributes.colorCapabilities.value.XYAttributesSupported  = colorCapabilities;

    customColorControlClusterServerAttributes.colorCapabilities.value.colorTemperatureSupported = 1;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
    customColorControlClusterServerAttributes.startUpColorTemperatureMireds.value = ZCL_ZLL_STARTUP_COLOR_TEMPERATURE_PREVIOUS;
    customColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value = ZCL_ZCL_CLUSTER_TEMP_PHYSICAL_MIN_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.options.value = 0x00; // Bit#0: ExecuteIfOff
#endif
    gExecuteIfOff = false;
    customColorControlClusterServerAttributes.colorCapabilities.value.colorTemperatureSupported = 0;
    customColorControlClusterServerAttributes.colorTempPhysicalMin.value = ZCL_ZCL_CLUSTER_TEMP_PHYSICAL_MIN_DEFAULT_VALUE;
    customColorControlClusterServerAttributes.colorTempPhysicalMax.value = ZCL_ZCL_CLUSTER_TEMP_PHYSICAL_MAX_DEFAULT_VALUE;
  }
  else
  {
    PDS_Restore(APP_CUSTOM_COLOR_CONTROL_MEM_ID);
  }

  customColorControlClusterServerAttributes.currentHue.minReportInterval = COLOR_CONTROL_CUURENT_HUE_MIN_REPORT_PERIOD;
  customColorControlClusterServerAttributes.currentHue.maxReportInterval = COLOR_CONTROL_CUURENT_HUE_MAX_REPORT_PERIOD;

  customColorControlClusterServerAttributes.currentSaturation.minReportInterval = COLOR_CONTROL_CURRENT_SAT_MIN_REPORT_PERIOD;
  customColorControlClusterServerAttributes.currentSaturation.maxReportInterval = COLOR_CONTROL_CURRENT_SAT_MAX_REPORT_PERIOD;

  customColorControlClusterServerAttributes.currentX.minReportInterval = COLOR_CONTROL_CURR_X_MIN_REPORT_PERIOD;
  customColorControlClusterServerAttributes.currentX.maxReportInterval = COLOR_CONTROL_CURR_X_MAX_REPORT_PERIOD;

  customColorControlClusterServerAttributes.currentY.minReportInterval = COLOR_CONTROL_CURR_Y_MIN_REPORT_PERIOD;
  customColorControlClusterServerAttributes.currentY.maxReportInterval = COLOR_CONTROL_CURR_Y_MAX_REPORT_PERIOD;

  customColorControlClusterServerAttributes.colorTemperature.minReportInterval = COLOR_CONTROL_COLOR_TEMP_MIN_REPORT_PERIOD;
  customColorControlClusterServerAttributes.colorTemperature.maxReportInterval = COLOR_CONTROL_COLOR_TEMP_MAX_REPORT_PERIOD;

  customColorControlClusterServerAttributes.clusterVersion.value = COLOR_CONTROL_CLUSTER_VERSION;

  updateTimer.mode = TIMER_REPEAT_MODE;
  updateTimer.interval = UPDATE_TIMER_INTERVAL;
  updateTimer.callback = updateTransitionState;

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  customUpdateStartupColorTemperature();
#endif
  customColorControlClusterClientAttributes.clusterVersion.value = COLOR_CONTROL_CLUSTER_VERSION;
  ZCL_Cluster_t *clusterClient = ZCL_GetCluster(APP_ENDPOINT_CUSTOM, COLOR_CONTROL_CLUSTER_ID, ZCL_CLUSTER_SIDE_CLIENT);
  
  if (clusterClient)
  {
    clusterClient->ZCL_ReportInd = customColorControlReportInd;
    clusterClient->ZCL_DefaultRespInd = ZCL_CommandZclDefaultResp;
  }
  displayStatus();
}

/**************************************************************************//**
\brief Set Extension Field Set
******************************************************************************/
void colorControlClusterSetExtensionField(Scene_t *scene, uint16_t transitionTime)
{
  uint8_t proceedWith = 0;
  setColorMode(scene->colorMode);
  if (0xffff == transitionTime || 0 == transitionTime)
  {
    setHue(scene->enhancedCurrentHue);
    setSaturation(scene->currentSaturation);
  }
  else
  {
    if(prepareMoveToHue(scene->enhancedCurrentHue, 1 , transitionTime, false))
      proceedWith = HUE;
    if(prepareMoveToSaturation(scene->currentSaturation, transitionTime, false))
      proceedWith |= SATURATION;
  }
  setColorLoop(scene->colorLoopActive, scene->colorLoopDirection, scene->colorLoopTime);
  if (0xffff == transitionTime || 0 == transitionTime)
    setColor(scene->currentX, scene->currentY);
  else
    if(prepareMoveToColor(scene->currentX, scene->currentY, transitionTime))
      proceedWith |= COLOR;
  if (0xffff == transitionTime || 0 == transitionTime)
    setColorTemperature(scene->colorTemperature);
  else
    if(prepareMoveToColorTemperature(scene->colorTemperature, transitionTime, false))
      proceedWith |= TEMPERATURE;
  if (proceedWith)
    transitionStart((TransitionType_t)proceedWith);
}

INLINE uint8_t levelControlCalculateIntensity(void)
{
  // Wrong in general..
  return customColorControlClusterServerAttributes.currentSaturation.value == UINT8_MAX ?
         UINT8_MAX - 1 : customColorControlClusterServerAttributes.currentSaturation.value;
  // Should be something like..
  //return (uint8_t) pow(10.0, ((currentLevel - 1) * 3 / 253 ) - 1);
}

/**************************************************************************//**
\brief Display current level status.
******************************************************************************/
static void displayStatus(void)
{
  /* if the device is in OFF state, color control command
   * WILL BE PROCESSED but there WILL NOT BE change in LED.
   */
  if (gExecuteIfOff)
    return;

    // add information about color temperature
  LCD_PRINT(0, 1, 
   "%5u  %5u %5u %3u",
            customColorControlClusterServerAttributes.currentX.value,
            customColorControlClusterServerAttributes.currentY.value,
            customColorControlClusterServerAttributes.enhancedCurrentHue.value,
            customColorControlClusterServerAttributes.currentSaturation.value
           );
  switch (customColorControlClusterServerAttributes.colorMode.value)
  {
     uint32_t tmpColorX, tmpColorY;
    case ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION:  
      // Get the X and Y values for the required hue and saturation
      HS2XY((int)customColorControlClusterServerAttributes.enhancedCurrentHue.value >> 8,
        (int)customColorControlClusterServerAttributes.currentSaturation.value,
        (unsigned int *)&tmpColorX,(unsigned int *)&tmpColorY);
      // Set the LED color for the X and Y values
      LEDS_SET_COLOR_XY(tmpColorX,tmpColorY);
      customColorControlClusterServerAttributes.currentX.value = tmpColorX;
      customColorControlClusterServerAttributes.currentY.value = tmpColorY;
      break;
    case ZCL_ZLL_COLOR_TEMPERATURE:
      customConvertColorToXY(customColorControlClusterServerAttributes.colorTemperature.value,
        (uint16_t *)&tmpColorX, (uint16_t *)&tmpColorY
      );
      customColorControlClusterServerAttributes.currentX.value = (uint16_t)tmpColorX;
      customColorControlClusterServerAttributes.currentY.value = (uint16_t)tmpColorY;
    // fall through from ZCL_ZLL_COLOR_TEMPERATURE case
    default:
      LEDS_SET_COLOR_XY(
        customColorControlClusterServerAttributes.currentX.value,
        customColorControlClusterServerAttributes.currentY.value
      );
      break;
  }
#ifdef _ZCL_REPORTING_SUPPORT_
  ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.currentX);
  ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.currentY);
#endif
}

/**************************************************************************//**
\brief Set current mode
******************************************************************************/
static void setColorMode(uint8_t mode)
{
  customColorControlClusterServerAttributes.enhancedColorMode.value  = mode;

  if (ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION == mode)
    customColorControlClusterServerAttributes.colorMode.value = ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION;
  else
    customColorControlClusterServerAttributes.colorMode.value = mode;
}

/**************************************************************************//**
\brief Set hue value
******************************************************************************/
static void setHue(uint16_t hue)
{
  if (customColorControlClusterServerAttributes.enhancedCurrentHue.value != hue)
  {
    customColorControlClusterServerAttributes.enhancedCurrentHue.value = hue;
    customColorControlClusterServerAttributes.currentHue.value = hue >> 8;
    customScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.currentHue);
#endif
  }
}

/**************************************************************************//**
\brief Set saturation value
******************************************************************************/
static void setSaturation(uint8_t saturation)
{
  if (customColorControlClusterServerAttributes.currentSaturation.value != saturation)
  {
    customColorControlClusterServerAttributes.currentSaturation.value = saturation;
    customScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.currentSaturation);
#endif
  }
}

/**************************************************************************//**
\brief Set color value
******************************************************************************/
static void setColor(uint16_t x, uint16_t y)
{
  if (customColorControlClusterServerAttributes.currentX.value != x ||
      customColorControlClusterServerAttributes.currentY.value != y)
  {
    customColorControlClusterServerAttributes.currentX.value = x;
    customColorControlClusterServerAttributes.currentY.value = y;
    customColorControlClusterServerAttributes.primary1X.value = x;
    customColorControlClusterServerAttributes.primary1Y.value = y;
    customColorControlClusterServerAttributes.primary1XIntensity.value =
      levelControlCalculateIntensity();

    customScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.currentX);
    ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.currentY);
#endif
  }
}

/**************************************************************************//**
\brief Set color temperature value
******************************************************************************/
static void setColorTemperature(uint16_t temperature)
{
  if (customColorControlClusterServerAttributes.colorTemperature.value != temperature)
  {
    customColorControlClusterServerAttributes.colorTemperature.value = temperature;
    customScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.colorTemperature);
#endif
  }
}

/**************************************************************************//**
\brief Set Color Loop values
******************************************************************************/
static void setColorLoop(uint8_t colorLoopActive, uint8_t colorLoopDirection, uint16_t colorLoopTime)
{
  customColorControlClusterServerAttributes.colorLoopActive.value    = colorLoopActive;
  customColorControlClusterServerAttributes.colorLoopDirection.value = colorLoopDirection;
  customColorControlClusterServerAttributes.colorLoopTime.value      = colorLoopTime;

  if (colorLoopActive)
    startColorLoop(COLOR_LOOP_ACTIVATE_FROM_ECH);
}

/**************************************************************************//**
\brief Stop Color Loop handler.
******************************************************************************/
static void stopColorLoop(void)
{
  transitionStop(ALL);
  customColorControlClusterServerAttributes.colorLoopActive.value = 0;
  setHue(customColorControlClusterServerAttributes.colorLoopStoredEnhancedHue.value);
}

/**************************************************************************//**
\brief Starts the color loop specified way.

\param[in] action - color loop mode.
******************************************************************************/
static void startColorLoop(ZCL_ZllColorLoopAction_t action)
{
  customColorControlClusterServerAttributes.colorLoopStoredEnhancedHue.value =
    customColorControlClusterServerAttributes.enhancedCurrentHue.value;
  if (COLOR_LOOP_ACTIVATE_FROM_ECH == action)
      customColorControlClusterServerAttributes.colorLoopStartEnhancedHue.value =
      customColorControlClusterServerAttributes.enhancedCurrentHue.value;
  customColorControlClusterServerAttributes.colorLoopActive.value = 1;
  customColorControlClusterServerAttributes.remainingTime.value = 0xffff;
  setColorMode(ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
  transitionStart(COLOR_LOOP);
}

/**************************************************************************//**
\brief Handle hue transition
******************************************************************************/
static void handleHueTransition(void)
{
  if (hueTransition.direction)
  {// Up transition
    hueTransition.current += hueTransition.delta;
  }
  else
  {// Down transition
    hueTransition.current -= hueTransition.delta;
  }

  setHue(hueTransition.current >> 16);
}

/**************************************************************************//**
\brief Handle saturation transition
******************************************************************************/
static void handleSaturationTransition(void)
{
  uint8_t saturation;
  bool finish = false;

  // Up transition
  if (saturationTransition.direction)
  {
    if (((uint32_t){MAX_SATURATION_LEVEL} << 16) - saturationTransition.delta < saturationTransition.current)
    {
      saturation = MAX_SATURATION_LEVEL;
      finish = true;
    }
    else
    {
      saturationTransition.current += saturationTransition.delta;
      saturation = saturationTransition.current >> 16;
    }
  }
  // Down transition
  else
  {
    if (saturationTransition.current - ((uint32_t){MIN_SATURATION_LEVEL} << 16) < saturationTransition.delta)
    {
      saturation = MIN_SATURATION_LEVEL;
      finish = true;
    }
    else
    {
      saturationTransition.current -= saturationTransition.delta;
      saturation = saturationTransition.current >> 16;
    }
  }

  setSaturation(saturation);

  if (finish)
    transitionStop(SATURATION);
}

/**************************************************************************//**
\brief Handle color transition
******************************************************************************/
static void handleColorTransition(void)
{
  uint16_t x, y;
  bool finishX = false;
  bool finishY = false;

  // X Up transition
  if (colorTransition.directionX)
  {
    if (((uint32_t){MAX_COLOR_LEVEL} << 16) - colorTransition.deltaX < colorTransition.currentX)
    {
      x = MAX_COLOR_LEVEL;
      finishX = true;
    }
    else
    {
      colorTransition.currentX += colorTransition.deltaX;
      x = colorTransition.currentX >> 16;
    }
  }
  // X Down transition
  else
  {
    if (colorTransition.currentX - ((uint32_t){MIN_COLOR_LEVEL} << 16) < colorTransition.deltaX)
    {
      x = MIN_COLOR_LEVEL;
      finishX = true;
    }
    else
    {
      colorTransition.currentX -= colorTransition.deltaX;
      x = colorTransition.currentX >> 16;
    }
  }

  // Y Up transition
  if (colorTransition.directionY)
  {
    if (((uint32_t){MAX_COLOR_LEVEL} << 16) - colorTransition.deltaY < colorTransition.currentY)
    {
      y = MAX_COLOR_LEVEL;
      finishY = true;
    }
    else
    {
      colorTransition.currentY += colorTransition.deltaY;
      y = colorTransition.currentY >> 16;
    }
  }
  // Y Down transition
  else
  {
    if (colorTransition.currentY - ((uint32_t){MIN_COLOR_LEVEL} << 16) < colorTransition.deltaY)
    {
      y = MIN_COLOR_LEVEL;
      finishY = true;
    }
    else
    {
      colorTransition.currentY -= colorTransition.deltaY;
      y = colorTransition.currentY >> 16;
    }
  }

  // Common
  setColor(x, y);

  if (finishX && finishY)
    transitionStop(COLOR);
}

/**************************************************************************//**
\brief Handle Color Loop transition
******************************************************************************/
static void handleColorLoopTransition(void)
{
  uint16_t delta = (MAX_HUE_LEVEL - MIN_HUE_LEVEL) /
                   (customColorControlClusterServerAttributes.colorLoopTime.value * 10);
  int32_t enhancedHue = customColorControlClusterServerAttributes.enhancedCurrentHue.value;

  // Up transition
  if (customColorControlClusterServerAttributes.colorLoopDirection.value)
    enhancedHue += delta;
  // Down transition
  else
    enhancedHue -= delta;

  if (enhancedHue < MIN_HUE_LEVEL)
    enhancedHue = MAX_HUE_LEVEL;
  else if (MAX_HUE_LEVEL < enhancedHue)
    enhancedHue = MIN_HUE_LEVEL;

  setHue(enhancedHue);
}

/**************************************************************************//**
\brief Handle color temperature transition
******************************************************************************/
static void handleColorTemperatureTransition(void)
{
  uint16_t temp;
  bool finish = false;

  // Temperature Up transition
  if (colorTemperatureTransition.direction)
  {
    if (((uint32_t){colorTemperatureTransition.maxTemp} << 16) - colorTemperatureTransition.delta < colorTemperatureTransition.current)
    {
      temp = colorTemperatureTransition.maxTemp;
      finish = true;
    }
    else
    {
      colorTemperatureTransition.current += colorTemperatureTransition.delta;
      temp = colorTemperatureTransition.current >> 16;
    }
  }
  // Temperature Down transition
  else
  {
    if (colorTemperatureTransition.current - ((uint32_t){colorTemperatureTransition.minTemp} << 16) < colorTemperatureTransition.delta)
    {
      temp = colorTemperatureTransition.minTemp;
      finish = true;
    }
    else
    {
      colorTemperatureTransition.current -= colorTemperatureTransition.delta;
      temp = colorTemperatureTransition.current >> 16;
    }
  }

  // Common
  setColorTemperature(temp);

  if (finish)
    transitionStop(TEMPERATURE);
}

/**************************************************************************//**
\brief Show identify effect.

\param[in] enhancedHue - enhanced hue to move to.
******************************************************************************/
void colorControlShowIdentifyEffect(uint16_t enhancedHue)
{
  TransitionType_t proceedWith;
  bckpSaturation = customColorControlClusterServerAttributes.currentSaturation.value;
  bckpEnhacnedHue = customColorControlClusterServerAttributes.enhancedCurrentHue.value;
  proceedWith = prepareMoveToHue(enhancedHue,
    ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, DEFAULT_TRANSITION_TIME, false);
  proceedWith |= prepareMoveToSaturation(DEFAULT_SATURATION_LEVEL,
    DEFAULT_TRANSITION_TIME, false);

  if (proceedWith)
  {
    setColorMode(ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(proceedWith);
  }
}

/**************************************************************************//**
\brief Stop showing identify effect and return to previous state.
******************************************************************************/
void colorControlStopIdentifyEffect(void)
{
  TransitionType_t proceedWith;

  proceedWith = prepareMoveToHue(bckpEnhacnedHue,
    ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, DEFAULT_TRANSITION_TIME, false);
  proceedWith |= prepareMoveToSaturation(bckpSaturation, DEFAULT_TRANSITION_TIME, false);

  if (proceedWith)
  {
    setColorMode(ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(proceedWith);
  }
}

/**************************************************************************//**
\brief Set target transition value
******************************************************************************/
static void setTargetValue()
{
  if (inTransition & HUE)
    setHue(hueTransition.target);

  if (inTransition & SATURATION)
    setSaturation(saturationTransition.target);

  if (inTransition & COLOR)
  {
    setColor(colorTransition.targetX, colorTransition.targetY);
    setColorTemperature(customConvertXYToColor(colorTransition.targetX, colorTransition.targetY));
  }

  if (inTransition & TEMPERATURE)
  {
    uint16_t x, y;

    customConvertColorToXY(colorTemperatureTransition.target, &x, &y);
    setColor(x, y);
    setColorTemperature(colorTemperatureTransition.target);
  }
}

/**************************************************************************//**
\brief Update transition state
******************************************************************************/
static void updateTransitionState(void)
{
  if (!deviceIsOn() && !gExecuteIfOff)
  {
    customColorControlClusterServerAttributes.remainingTime.value = 0;
    HAL_StopAppTimer(&updateTimer);
    return;
  }

  if (customColorControlClusterServerAttributes.remainingTime.value < 0xffff)
    customColorControlClusterServerAttributes.remainingTime.value--;

  if (customColorControlClusterServerAttributes.remainingTime.value > 0)
  {
    if (inTransition & HUE)
      handleHueTransition();

    if (inTransition & SATURATION)
      handleSaturationTransition();

    else if (inTransition & COLOR)
      handleColorTransition();

    else if (inTransition & COLOR_LOOP)
      handleColorLoopTransition();

    else if (inTransition & TEMPERATURE)
      handleColorTemperatureTransition();
  }
  else
  {
    setTargetValue();
    transitionStop(ALL);
  }
}

/**************************************************************************//**
\brief Start the transition
******************************************************************************/
static void transitionStart(TransitionType_t type)
{
  inTransition |= type;

  if (customColorControlClusterServerAttributes.remainingTime.value > 0)
  {
    HAL_StopAppTimer(&updateTimer);
    HAL_StartAppTimer(&updateTimer);
  }
  else
  {
    setTargetValue();
  }
}

/**************************************************************************//**
\brief Start current transition
******************************************************************************/
static void transitionStop(TransitionType_t type)
{
  gExecuteIfOff = false;

  inTransition &= ~type;

  if (NONE == inTransition)
  {
    HAL_StopAppTimer(&updateTimer);
    customColorControlClusterServerAttributes.remainingTime.value = 0;
  }

  PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);
}

/**************************************************************************//**
\brief Prepare Move To Hue parameters
******************************************************************************/
static TransitionType_t prepareMoveToHue(uint16_t hue, uint8_t direction, uint16_t transitionTime,
                                         bool byStep)
{
  uint16_t delta;
  bool dir = true;

  if (hue == customColorControlClusterServerAttributes.enhancedCurrentHue.value)
    return NONE;

  hueTransition.target = hue;
  hueTransition.byStep = byStep;

  if (!byStep)
  {
    // Get shortest distance direction and delta
    if (hue > customColorControlClusterServerAttributes.enhancedCurrentHue.value)
    {
      dir = true;
      delta = hue - customColorControlClusterServerAttributes.enhancedCurrentHue.value;
    }
    else
    {
      dir = false;
      delta = customColorControlClusterServerAttributes.enhancedCurrentHue.value - hue;
    }

    // Check if change in direction is needed
    if (ZCL_ZLL_MOVE_TO_HUE_DIRECTION_LONGEST_DISTANCE == direction ||
        (ZCL_ZLL_MOVE_TO_HUE_DIRECTION_UP == direction && false == dir) ||
        (ZCL_ZLL_MOVE_TO_HUE_DIRECTION_DOWN == direction && true == dir))
    {
      dir = !dir;
      delta = (MAX_HUE_LEVEL - MIN_HUE_LEVEL) - delta;
    }

    hueTransition.delta = ((uint32_t)delta << 16) / transitionTime;
    hueTransition.current = (uint32_t)customColorControlClusterServerAttributes.enhancedCurrentHue.value << 16;
    hueTransition.direction = dir;
  }

  customColorControlClusterServerAttributes.remainingTime.value = transitionTime;

  return HUE;
}

/**************************************************************************//**
\brief Prepare Move Hue parameters
******************************************************************************/
static bool prepareMoveHue(uint8_t mode, uint16_t rate)
{
  if (ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == mode)
    hueTransition.direction = true;
  else
    hueTransition.direction = false;

  hueTransition.current = (uint32_t)customColorControlClusterServerAttributes.enhancedCurrentHue.value << 16;
  hueTransition.target = 0;
  hueTransition.delta = ((uint32_t)rate << 16) / 10;
  hueTransition.byStep = false;
  customColorControlClusterServerAttributes.remainingTime.value = 0xffff;

  return true;
}

/**************************************************************************//**
\brief Prepare Move To Saturation parameters
******************************************************************************/
static TransitionType_t prepareMoveToSaturation(uint8_t saturation, uint16_t transitionTime,
                                                bool byStep)
{
  uint16_t delta;
  bool dir = true;

  if (saturation == customColorControlClusterServerAttributes.currentSaturation.value)
    return NONE;

  saturationTransition.target = saturation;
  saturationTransition.byStep = byStep;

  if (!byStep)
  {
    // Get shortest distance direction and delta
    if (saturation > customColorControlClusterServerAttributes.currentSaturation.value)
    {
      dir = true;
      delta = saturation - customColorControlClusterServerAttributes.currentSaturation.value;
    }
    else
    {
      dir = false;
      delta = customColorControlClusterServerAttributes.currentSaturation.value - saturation;
    }

    saturationTransition.delta = ((uint32_t)delta << 16) / (uint32_t)transitionTime;
    saturationTransition.current = (uint32_t)customColorControlClusterServerAttributes.currentSaturation.value << 16;
    saturationTransition.direction = dir;
  }
  customColorControlClusterServerAttributes.remainingTime.value = transitionTime;

  return SATURATION;
}

/**************************************************************************//**
\brief Prepare Move Saturation parameters
******************************************************************************/
static bool prepareMoveSaturation(uint8_t mode, uint16_t rate)
{
  if (ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == mode)
    saturationTransition.direction = true;
  else
    saturationTransition.direction = false;
  saturationTransition.current = (uint32_t)customColorControlClusterServerAttributes.currentSaturation.value << 16;
  saturationTransition.target = 0;
  saturationTransition.delta = ((uint32_t)rate << 16) / 10;
  saturationTransition.byStep = false;

  customColorControlClusterServerAttributes.remainingTime.value = 0xffff;
  return true;
}


/**************************************************************************//**
\brief Prepare Move To Color parameters
******************************************************************************/
static bool prepareMoveToColor(uint16_t x, uint16_t y, uint16_t transitionTime)
{
  uint16_t deltax, deltay;
  bool dirx, diry;
  if (x == customColorControlClusterServerAttributes.currentX.value &&
      y == customColorControlClusterServerAttributes.currentY.value)
    return false;

  // Get shortest distance direction and delta
  dirx = x > customColorControlClusterServerAttributes.currentX.value;

  if (dirx)
    deltax = x - customColorControlClusterServerAttributes.currentX.value;
  else
    deltax = customColorControlClusterServerAttributes.currentX.value - x;

  diry = y > customColorControlClusterServerAttributes.currentY.value;

  if (diry)
    deltay = y - customColorControlClusterServerAttributes.currentY.value;
  else
    deltay = customColorControlClusterServerAttributes.currentY.value - y;

  colorTransition.deltaX = ((uint32_t)deltax << 16) / transitionTime;
  colorTransition.deltaY = ((uint32_t)deltay << 16) / transitionTime;
  colorTransition.currentX = (uint32_t)customColorControlClusterServerAttributes.currentX.value << 16;
  colorTransition.currentY = (uint32_t)customColorControlClusterServerAttributes.currentY.value << 16;
  colorTransition.directionX = dirx;
  colorTransition.directionY = diry;
  colorTransition.targetX = x;
  colorTransition.targetY = y;
  customColorControlClusterServerAttributes.remainingTime.value = transitionTime;
  return true;
}

/**************************************************************************//**
\brief Prepare Move Color parameters
******************************************************************************/
static bool prepareMoveColor(int16_t ratex, int16_t ratey)
{
  // X
  colorTransition.deltaX = ((uint32_t)ABS(ratex) << 16) / 10;
  colorTransition.currentX = (uint32_t)customColorControlClusterServerAttributes.currentX.value << 16;
  colorTransition.directionX = ratex > 0;
  colorTransition.targetX = 0;
  // Y
  colorTransition.deltaY = ((uint32_t)ABS(ratey) << 16) / 10;
  colorTransition.currentY = (uint32_t)customColorControlClusterServerAttributes.currentY.value << 16;
  colorTransition.directionY = ratey > 0;
  colorTransition.targetY = 0;
  customColorControlClusterServerAttributes.remainingTime.value = 0xffff;
  return true;
}

/**************************************************************************//**
\brief Prepare Move To Color Temperature parameters
******************************************************************************/
static bool prepareMoveToColorTemperature(uint16_t temperature, uint16_t transitionTime, bool byStep)
{
  uint16_t deltaTemp;
  bool dirTemp;

  if (temperature == customColorControlClusterServerAttributes.colorTemperature.value)
    return false;

  customColorControlClusterServerAttributes.remainingTime.value = transitionTime;
  colorTemperatureTransition.target                       = temperature;
  colorTemperatureTransition.byStep                       = byStep;

  // Get shortest distance direction and delta
  dirTemp = temperature > customColorControlClusterServerAttributes.colorTemperature.value;
  if (dirTemp)
    deltaTemp = temperature - customColorControlClusterServerAttributes.colorTemperature.value;
  else
    deltaTemp = customColorControlClusterServerAttributes.colorTemperature.value - temperature;

  colorTemperatureTransition.delta     = ((uint32_t)deltaTemp << 16) / transitionTime;
  colorTemperatureTransition.current   = (uint32_t)customColorControlClusterServerAttributes.colorTemperature.value << 16;
  colorTemperatureTransition.direction = dirTemp;
  colorTemperatureTransition.minTemp   = customColorControlClusterServerAttributes.colorTempPhysicalMin.value;
  colorTemperatureTransition.maxTemp   = customColorControlClusterServerAttributes.colorTempPhysicalMax.value;

  return true;
}

/**************************************************************************//**
\brief Prepare Move Color Temperature parameters
******************************************************************************/
static bool prepareMoveColorTemperature(uint8_t moveMode, uint16_t rate, uint16_t tempMin, uint16_t tempMax)
{
  colorTemperatureTransition.delta     = ((uint32_t)ABS(rate) << 16) / 10;
  colorTemperatureTransition.current   = (uint32_t)customColorControlClusterServerAttributes.colorTemperature.value << 16;
  colorTemperatureTransition.direction = ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_UP == moveMode;
  colorTemperatureTransition.target    = 0;
  colorTemperatureTransition.byStep    = false;
  colorTemperatureTransition.minTemp   = MAX(tempMin, customColorControlClusterServerAttributes.colorTempPhysicalMin.value);
  colorTemperatureTransition.maxTemp   = MIN(tempMax, customColorControlClusterServerAttributes.colorTempPhysicalMax.value);

  customColorControlClusterServerAttributes.remainingTime.value = 0xffff;

  return true;
}

/**************************************************************************//**
\brief Callback on receive of Move To Hue command
******************************************************************************/
static ZCL_Status_t moveToHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToHueCommand_t *payload)
{
  uint8_t on, type;
  uint16_t hue;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  dbgLcdMsg("moveToHue");
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToHueWithOptionsCommand_t *tmp = (ZCL_ZllMoveToHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif

  gExecuteIfOff = executeIfOff;

  hue = ((MAX_HUE_LEVEL >> 8) < payload->hue) ? (MAX_HUE_LEVEL >> 8) : payload->hue;
  if (prepareMoveToHue(hue << 8, payload->direction, payload->transitionTime, false))
  {
    setColorMode(ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(HUE);
  }

  APP_Zigbee_Handler(event);

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move Hue command
******************************************************************************/
static ZCL_Status_t moveHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveHueCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  dbgLcdMsg("moveHue");
  
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveHueWithOptionsCommand_t *tmp = (ZCL_ZllMoveHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif

  gExecuteIfOff = executeIfOff;

  if (ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == payload->moveMode ||
      ZCL_ZLL_MOVE_HUE_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (!payload->rate)
      return ZCL_INVALID_FIELD_STATUS;

    if (prepareMoveHue(payload->moveMode, (uint16_t)payload->rate << 8))
    {
      setColorMode(ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
      transitionStart(HUE);
    }
  }
  else if (ZCL_ZLL_MOVE_HUE_MOVE_MODE_STOP == payload->moveMode)
  {
    transitionStop(ALL);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Step Hue command
******************************************************************************/
static ZCL_Status_t stepHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepHueCommand_t *payload)
{
  uint16_t hue;
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("stepHue");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepHueWithOptionsCommand_t *tmp = (ZCL_ZllStepHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif

  gExecuteIfOff = executeIfOff;

  hue = customColorControlClusterServerAttributes.enhancedCurrentHue.value;

  if (ZCL_ZLL_STEP_HUE_STEP_MODE_UP == payload->stepMode)
    hue += (uint16_t)payload->stepSize << 8;
  else if (ZCL_ZLL_STEP_HUE_STEP_MODE_DOWN == payload->stepMode)
    hue -= (uint16_t)payload->stepSize << 8;
  else
    return ZCL_SUCCESS_STATUS;

  if (prepareMoveToHue(hue, ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, true))
  {
    setColorMode(ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(HUE);
  }
  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move To Saturation command
******************************************************************************/
static ZCL_Status_t moveToSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToSaturationCommand_t *payload)
{
  uint8_t on, type, saturation;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("moveToSat");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToSaturationWithOptionsCommand_t *tmp = (ZCL_ZllMoveToSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  saturation = (MAX_SATURATION_LEVEL < payload->saturation) ? MAX_SATURATION_LEVEL : payload->saturation;
  if (prepareMoveToSaturation(saturation, payload->transitionTime, false))
  {
    setColorMode(ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(SATURATION);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move Saturation command
******************************************************************************/
static ZCL_Status_t moveSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveSaturationCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("moveSat");
 
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveSaturationWithOptionsCommand_t *tmp = (ZCL_ZllMoveSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (!payload->rate)
    return ZCL_INVALID_FIELD_STATUS;

  if (ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_UP == payload->moveMode ||
      ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (prepareMoveSaturation(payload->moveMode, payload->rate))
    {
      setColorMode(ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
      transitionStart(SATURATION);
    }
  }
  else if (ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_STOP == payload->moveMode)
  {
    transitionStop(ALL);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Step Saturation command
******************************************************************************/
static ZCL_Status_t stepSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepSaturationCommand_t *payload)
{
  int16_t saturation;
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("stepSat");
  
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepSaturationWithOptionsCommand_t *tmp = (ZCL_ZllStepSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  saturation = customColorControlClusterServerAttributes.currentSaturation.value;

  if (ZCL_ZLL_STEP_SATURATION_STEP_MODE_UP == payload->stepMode)
    saturation += payload->stepSize;
  else if (ZCL_ZLL_STEP_HUE_STEP_MODE_DOWN == payload->stepMode)
    saturation -= payload->stepSize;
  else
    return ZCL_SUCCESS_STATUS;

  if (prepareMoveToSaturation(saturation, payload->transitionTime, true))
  {
    setColorMode(ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(SATURATION);
  }

  (void)addressing;
  (void)payloadLength;
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move To Hue And Saturation command
******************************************************************************/
static ZCL_Status_t moveToHueAndSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToHueAndSaturationCommand_t *payload)
{
  TransitionType_t proceedWith;
  uint8_t on, type, saturation;
  uint16_t hue;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  dbgLcdMsg("moveToHueAndSat");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_HUE_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToHueAndSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToHueAndSaturationWithOptionsCommand_t *tmp = (ZCL_ZllMoveToHueAndSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  hue = ((MAX_HUE_LEVEL >> 8) < payload->hue) ? (MAX_HUE_LEVEL >> 8) : payload->hue;
  saturation = (MAX_SATURATION_LEVEL < payload->saturation) ? MAX_SATURATION_LEVEL : payload->saturation;

  proceedWith = prepareMoveToHue((uint16_t)hue << 8, ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, false);
  proceedWith |= prepareMoveToSaturation(saturation, payload->transitionTime, false);

  if (proceedWith)
  {
    setColorMode(ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(proceedWith);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move To Color command
******************************************************************************/
static ZCL_Status_t moveToColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToColorCommand_t *payload)
{
  uint8_t on, type;
  uint16_t x, y;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  dbgLcdMsg("moveToColor");
 
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_COLOR;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToColorCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToColorWithOptionsCommand_t *tmp = (ZCL_ZllMoveToColorWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  x = (MAX_COLOR_LEVEL < payload->colorX) ? MAX_COLOR_LEVEL : payload->colorX;
  y = (MAX_COLOR_LEVEL < payload->colorY) ? MAX_COLOR_LEVEL : payload->colorY;

  if (prepareMoveToColor(x, y, payload->transitionTime))
  {
    setColorMode(ZCL_ZLL_CURRENT_X_AND_CURRENT_Y);
    transitionStart(COLOR);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move Color command
******************************************************************************/
static ZCL_Status_t moveColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveColorCommand_t *payload)
{
  uint8_t on, type;

  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  dbgLcdMsg("moveColor");
 
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_COLOR;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveColorCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveColorWithOptionsCommand_t *tmp = (ZCL_ZllMoveColorWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (prepareMoveColor(payload->rateX, payload->rateY))
  {
    setColorMode(ZCL_ZLL_CURRENT_X_AND_CURRENT_Y);
    transitionStart(COLOR);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Step Color command
******************************************************************************/
static ZCL_Status_t stepColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepColorCommand_t *payload)
{
  int32_t x, y;
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("stepColor");
  
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_COLOR;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepColorCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepColorWithOptionsCommand_t *tmp = (ZCL_ZllStepColorWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (0 == payload->stepX && 0 == payload->stepY)
  {
    transitionStop(ALL);
    return ZCL_SUCCESS_STATUS;
  }

  x = (int32_t)customColorControlClusterServerAttributes.currentX.value + payload->stepX;
  y = (int32_t)customColorControlClusterServerAttributes.currentY.value + payload->stepY;

  if (prepareMoveToColor(x, y, payload->transitionTime))
  {
    setColorMode(ZCL_ZLL_CURRENT_X_AND_CURRENT_Y);
    transitionStart(COLOR);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Enhanced Move To Hue command
******************************************************************************/
static ZCL_Status_t enhancedMoveToHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveToHueCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_MOVE_TO_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedMoveToHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedMoveToHueWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedMoveToHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  // Check for direction == 1?
  setColorMode(ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
  if (prepareMoveToHue(payload->enhancedHue, payload->direction, payload->transitionTime, false))
  {
    transitionStart(HUE);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Enhanced Move Hue command
******************************************************************************/
static ZCL_Status_t enhancedMoveHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveHueCommand_t *payload)
{
  uint8_t on, type;

  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  dbgLcdMsg("eMoveHue");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_MOVE_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedMoveHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedMoveHueWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedMoveHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == payload->moveMode ||
      ZCL_ZLL_MOVE_HUE_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (!payload->rate)
      return ZCL_INVALID_FIELD_STATUS;

    if (prepareMoveHue(payload->moveMode, payload->rate))
    {
      setColorMode(ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
      transitionStart(HUE);
    }
  }
  else if (ZCL_ZLL_MOVE_HUE_MOVE_MODE_STOP == payload->moveMode)
  {
    transitionStop(ALL);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Enhanced Step Hue command
******************************************************************************/
static ZCL_Status_t enhancedStepHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedStepHueCommand_t *payload)
{
  uint16_t hue;
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("eStepHue");
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_STEP_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedStepHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedStepHueWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedStepHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;
  hue = customColorControlClusterServerAttributes.enhancedCurrentHue.value;
  if (ZCL_ZLL_STEP_HUE_STEP_MODE_UP == payload->stepMode)
    hue += (uint16_t)payload->stepSize;
  else if (ZCL_ZLL_STEP_HUE_STEP_MODE_DOWN == payload->stepMode)
    hue -= (uint16_t)payload->stepSize;
  else
    return ZCL_SUCCESS_STATUS;

  if (prepareMoveToHue(hue, ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, true))
  {
    setColorMode(ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(HUE);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Enhanced Move To Hue And Saturation command
******************************************************************************/
static ZCL_Status_t enhancedMoveToHueAndSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t *payload)
{
  TransitionType_t proceedWith;
  uint8_t on, type;
  uint16_t saturation;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;

  dbgLcdMsg("eMTHAS");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_MOVED_TO_HUE_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedMoveToHueAndSaturationWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedMoveToHueAndSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  saturation = (MAX_SATURATION_LEVEL < payload->saturation) ? MAX_SATURATION_LEVEL : payload->saturation;

  proceedWith = prepareMoveToHue(payload->enhancedHue, ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, false);
  proceedWith |= prepareMoveToSaturation(saturation, payload->transitionTime, false);

  if (proceedWith)
  {
    setColorMode(ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(proceedWith);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Color Loop Set command
******************************************************************************/
static ZCL_Status_t colorLoopSetInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllColorLoopSetCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("colorLoopSet");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_COLOR_LOOP_SET;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllColorLoopSetCommand_t))
    {
      /* Check for options override */
      ZCL_ZllColorLoopSetWithOptionsCommand_t *tmp = (ZCL_ZllColorLoopSetWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (COLOR_LOOP_UPDATE_DIRECTION & payload->updateFlags)
    customColorControlClusterServerAttributes.colorLoopDirection.value = payload->direction;

  if (COLOR_LOOP_UPDATE_TIME & payload->updateFlags)
    customColorControlClusterServerAttributes.colorLoopTime.value = payload->colorLoopTime;

  if (COLOR_LOOP_UPDATE_START_HUE & payload->updateFlags)
    customColorControlClusterServerAttributes.colorLoopStartEnhancedHue.value = payload->startHue;

  if (COLOR_LOOP_UPDATE_ACTION & payload->updateFlags)
  {
    switch (payload->action)
    {
      case COLOR_LOOP_DEACTIVATE:
        if (customColorControlClusterServerAttributes.colorLoopActive.value)
          stopColorLoop();
        break;

      case COLOR_LOOP_ACTIVATE_FROM_CLSEH:
        startColorLoop(COLOR_LOOP_ACTIVATE_FROM_CLSEH);
        break;

      case COLOR_LOOP_ACTIVATE_FROM_ECH:
        startColorLoop(COLOR_LOOP_ACTIVATE_FROM_ECH);
        break;

      default:
        break;
    }
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Stop Move Step command
******************************************************************************/
static ZCL_Status_t stopMoveStepInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("stopMoveStep");
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STOP_MOVE_STEP;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > 0)
    {
      /* Check for options override */
      ZCL_ZllStopMoveStepWithOptionsCommand_t *tmp = (ZCL_ZllStopMoveStepWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  transitionStop(ALL);

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Callback on receive of MoveToColorTemperature command
******************************************************************************/
static ZCL_Status_t moveToColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToColorTemperatureCommand_t *payload)
{
  uint16_t miredValue;
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);


  dbgLcdMsg("moveToColorTemperature");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_COLOR_TEMPERATURE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToColorTemperatureCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToColorTemperatureWithOptionsCommand_t *tmp = (ZCL_ZllMoveToColorTemperatureWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (customColorControlClusterServerAttributes.colorTempPhysicalMin.value > payload->colorTemperature)
  {
    miredValue = customColorControlClusterServerAttributes.colorTempPhysicalMin.value;
  }
  else if (customColorControlClusterServerAttributes.colorTempPhysicalMax.value < payload->colorTemperature)
  {
    miredValue = customColorControlClusterServerAttributes.colorTempPhysicalMax.value;
  }
  else
  {
    miredValue = payload->colorTemperature;
  }

  if (prepareMoveToColorTemperature(miredValue, payload->transitionTime, false))
  {
    setColorMode(ZCL_ZLL_COLOR_TEMPERATURE);
    transitionStart(TEMPERATURE);
  }

  APP_Zigbee_Handler(event);
  /*clipped status also returns success status, default response check at ZCL layer */
   return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of MoveColorTemperature command
******************************************************************************/
static ZCL_Status_t moveColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveColorTemperatureCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("moveColorTemperature");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_COLOR_TEMPERATURE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveColorTemperatureCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveColorTemperatureWithOptionsCommand_t *tmp = (ZCL_ZllMoveColorTemperatureWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_STOP != payload->moveMode)
    if (!payload->rate)
      return ZCL_INVALID_VALUE_STATUS;

  if (ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_UP == payload->moveMode ||
      ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (prepareMoveColorTemperature(payload->moveMode, payload->rate, payload->colorTemperatureMinimum, payload->colorTemperatureMaximum))
    {
      setColorMode(ZCL_ZLL_COLOR_TEMPERATURE);
      transitionStart(TEMPERATURE);
    }
  }
   else if(ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_STOP == payload->moveMode)
     transitionStop(ALL);

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of StepColorTemperature command
******************************************************************************/
static ZCL_Status_t stepColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepColorTemperatureCommand_t *payload)
{
  uint16_t temperature;
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  dbgLcdMsg("stepColorTemperature");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_COLOR_TEMPERATURE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!status && !on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepColorTemperatureCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepColorTemperatureWithOptionsCommand_t *tmp = (ZCL_ZllStepColorTemperatureWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customColorControlClusterServerAttributes.options.value)
      {
        executeIfOff = true;
      }
      else
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
  }
#endif
  gExecuteIfOff = executeIfOff;

  if (0 == payload->stepSize)
  {
    transitionStop(ALL);
    return ZCL_SUCCESS_STATUS;
  }
  temperature = customColorControlClusterServerAttributes.colorTemperature.value;

  if (ZCL_ZLL_STEP_COLOR_TEMPERATURE_STEP_MODE_UP == payload->stepMode)
  {
    if (ABSOLUTE_MAXIMUM_COLOR_TEMP - temperature > payload->stepSize)
      temperature += payload->stepSize;
    else
      temperature = ABSOLUTE_MAXIMUM_COLOR_TEMP;

    if (temperature > payload->colorTemperatureMaximum)
      temperature = payload->colorTemperatureMaximum;
  }
  else if (ZCL_ZLL_STEP_COLOR_TEMPERATURE_STEP_MODE_DOWN == payload->stepMode)
  {
    if (temperature >= payload->stepSize)
      temperature -= payload->stepSize;
    else
      temperature = 0;

    if (temperature < payload->colorTemperatureMinimum)
      temperature = payload->colorTemperatureMinimum;
  }
  else
    return ZCL_SUCCESS_STATUS;

  if (temperature < customColorControlClusterServerAttributes.colorTempPhysicalMin.value ||
      customColorControlClusterServerAttributes.colorTempPhysicalMax.value < temperature)
    return ZCL_INVALID_VALUE_STATUS;

  if (prepareMoveToColorTemperature(temperature, payload->transitionTime, true))
  {
    setColorMode(ZCL_ZLL_COLOR_TEMPERATURE);
    transitionStart(TEMPERATURE);
  }
  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}

#if ZLO_CLUSTER_ENHANCEMENTS == 1
/**************************************************************************//**
\brief Update device ON state as per StartUpOnOff attribute value
******************************************************************************/
static void customUpdateStartupColorTemperature(void)
{
  if (ZCL_ZLL_STARTUP_COLOR_TEMPERATURE_PREVIOUS !=
      customColorControlClusterServerAttributes.startUpColorTemperatureMireds.value)
  {
    // Set the color temperature to startup value
    customColorControlClusterServerAttributes.colorTemperature.value =
      customColorControlClusterServerAttributes.startUpColorTemperatureMireds.value;
  }
  /* In case of different value i.e., startUpColorTemp != 0xffff, use the value
   * restored from PDS */
  // Set the color mode to colorTemperature
  customColorControlClusterServerAttributes.colorMode.value = ZCL_ZLL_COLOR_TEMPERATURE;
  // Set the enhanced color mode to colorTemperature
  customColorControlClusterServerAttributes.enhancedColorMode.value = ZCL_ZLL_COLOR_TEMPERATURE;

  PDS_Store(APP_CUSTOM_COLOR_CONTROL_MEM_ID);
}
#endif

/**************************************************************************//**
\brief Attribute event (writing/reading) callback.

\param[in] addressing - incoming request addressing information.
\param[in] attributeId - attribute identifier.
\param[in] event - attribute event (read/write).
******************************************************************************/
static void ZCL_ColorControlAttributeEventInd(ZCL_Addressing_t *addressing,
  ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event)
{
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_ATTR_COLOR_CONTROL;
  eventItem.eventData.zclAttributeData.addressing = addressing;
  eventItem.eventData.zclAttributeData.attributeId = attributeId;
  eventItem.eventData.zclAttributeData.event = event;
  APP_Zigbee_Handler(eventItem);

#if ZLO_CLUSTER_ENHANCEMENTS == 1  
  if (((ZCL_WRITE_ATTRIBUTE_EVENT == event) && \
      ((ZCL_ZLL_CLUSTER_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID == attributeId) || \
       (ZCL_ZLL_CLUSTER_STARTUP_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID == attributeId))) || \
       ((ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT == event) && \
         ((ZCL_ZLL_CLUSTER_CURRENT_HUE_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_CURRENT_SATURATION_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_CURRENT_X_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_CURRENT_Y_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID == attributeId)
         )
        )
      )
#else
	if (((ZCL_WRITE_ATTRIBUTE_EVENT == event) && \
      (ZCL_ZLL_CLUSTER_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID == attributeId)) || \
       ((ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT == event) && \
         ((ZCL_ZLL_CLUSTER_CURRENT_HUE_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_CURRENT_SATURATION_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_CURRENT_X_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_CURRENT_Y_SERVER_ATTRIBUTE_ID == attributeId) || \
          (ZCL_ZLL_CLUSTER_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID == attributeId)
         )
        )
      )
#endif
  {
    PDS_Store(APP_CUSTOM_COLOR_CONTROL_MEM_ID);
  }

}

/**************************************************************************//**
\brief Update the color temperature on level change if required

\param[in] deviceOn - The on/off status of the device
\param[in] updateColorTemp - Update coupleColorTemp bit overridden in optional fields
\param[in] level    - level the device is moving to
******************************************************************************/
#if ZLO_CLUSTER_ENHANCEMENTS == 1
void customUpdateCoupledColorTemperature(bool deviceOn, bool updateColorTemp, uint8_t level)
{
  uint16_t tmpColorTempMax, tmpNewColorTemp;

  if (updateColorTemp)
  {
    if ((ZCL_ZLL_COLOR_TEMPERATURE == customColorControlClusterServerAttributes.colorMode.value)
        || (ZCL_ZLL_COLOR_TEMPERATURE == customColorControlClusterServerAttributes.enhancedColorMode.value))
    {
      switch (level)
      {
        case ZCL_LEVEL_CONTROL_MAXIMUM_LEVEL:
          customColorControlClusterServerAttributes.colorTemperature.value =
            customColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value;
          break;

        case ZCL_LEVEL_CONTROL_MINIMUM_LEVEL:
          customColorControlClusterServerAttributes.colorTemperature.value =
            customColorControlClusterServerAttributes.colorTempPhysicalMax.value;
          break;

        default:
        {
          tmpColorTempMax = customColorControlClusterServerAttributes.colorTempPhysicalMax.value -
            customColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value;

          tmpNewColorTemp = tmpColorTempMax - ((tmpColorTempMax / ZCL_LEVEL_CONTROL_MAXIMUM_LEVEL) * level);
          tmpNewColorTemp += customColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value;

          customColorControlClusterServerAttributes.colorTemperature.value = tmpNewColorTemp;
        }
        break;
      }
#ifdef _ZCL_REPORTING_SUPPORT_
      ZCL_ReportOnChangeIfNeeded(&customColorControlClusterServerAttributes.colorTemperature);
#endif
      if (deviceOn)
      {
        displayStatus();
      }
    }
    PDS_Store(APP_CUSTOM_COLOR_CONTROL_MEM_ID);
  }
}
#endif
  void colorControlClusterInit(void) {;}
/**************************************************************************//**
\brief Report attribute indication handler

\param[in] addressing - pointer to addressing information;
\param[in] reportLength - data payload length;
\param[in] reportPayload - data pointer
******************************************************************************/
static void customColorControlReportInd(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload)
{
  ZCL_Report_t *rep = (ZCL_Report_t *)reportPayload;
  uint16_t reportValue;
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_REPORTING_COLOR_CONTROL;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = reportLength;
  event.eventData.zclEventData.payload = (uint8_t*)reportPayload;

  APP_Zigbee_Handler(event);
}

/*********************************************************************************
*********************************************************************************/

#endif // APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_CUSTOM_DEVICE

// eof customColorControlCluster.c
