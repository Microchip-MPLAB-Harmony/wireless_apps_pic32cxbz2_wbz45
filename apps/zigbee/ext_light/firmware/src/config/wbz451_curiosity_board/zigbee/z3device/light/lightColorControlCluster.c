/*******************************************************************************
  Light device Color Control cluster Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightColorControlCluster.c

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

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT

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
#define dbgLcdMsg(...) while (false) {;}
#endif

#include <z3device/common/include/hs2xy.h>

#include <z3device/clusters/include/haClusters.h>
#include <z3device/light/include/lightScenesCluster.h>
#include <z3device/light/include/lightOnOffCluster.h>
#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightColorControlCluster.h>
#include <z3device/common/include/z3Device.h>

#include <z3device/light/include/lightColorSchemesConversion.h>

#include <zllplatform/ZLL/N_DeviceInfo/include/N_DeviceInfo_Bindings.h>
#include <zllplatform/ZLL/N_DeviceInfo/include/N_DeviceInfo.h>
#include <pds/include/wlPdsMemIds.h>
#include <zcl/include/zclCommandManager.h>
#include <app_zigbee/zigbee_console/console.h>
#include <app_zigbee/app_zigbee_handler.h>
#include <zcl/include/zclAttributes.h>

/******************************************************************************
                    Definitions
******************************************************************************/
#define UPDATE_TIMER_INTERVAL    100U // 1/10th of a second as per specification

#define MIN_HUE_LEVEL             0U
#define MAX_HUE_LEVEL             0xfeffU
#define MAX_ENHANCED_HUE_LEVEL    0xffff

#define MIN_SATURATION_LEVEL      0U
#define MAX_SATURATION_LEVEL      0xfeU

#define MIN_COLOR_LEVEL           0U
#define MAX_COLOR_LEVEL           0xfeffU

#define DEFAULT_SATURATION_LEVEL 110
#define DEFAULT_TRANSITION_TIME  2

#define HUE_SATURATION_SUPPORTED    (1 << 0)
#define ENHANCED_HUE_SUPPORTED      (1 << 1)
#define COLOR_LOOP_SUPPORTED        (1 << 2)
#define X_Y_ATTRIBUTES_SUPPORTED    (1 << 3)
#define COLOR_TEMPERATURE_SUPPORTED (1 << 4)

#define COLOR_LOOP_UPDATE_ACTION    (1U << 0)
#define COLOR_LOOP_UPDATE_DIRECTION (1U << 1)
#define COLOR_LOOP_UPDATE_TIME      (1U << 2)
#define COLOR_LOOP_UPDATE_START_HUE (1U << 3)

#define ABS(x)  (((x) > 0) ? (x) : -(x))

#define ABSOLUTE_MAXIMUM_COLOR_TEMP  0xffffU

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
#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
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
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
static ZCL_Status_t moveToColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToColorTemperatureCommand_t *payload);
static ZCL_Status_t moveColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveColorTemperatureCommand_t *payload);
static ZCL_Status_t stepColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllStepColorTemperatureCommand_t *payload);
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

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

#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
static void setColorLoop(uint8_t colorLoopActive, uint8_t colorLoopDirection, uint16_t colorLoopTime);
static void startColorLoop(ZCL_ZllColorLoopAction_t action);
static TransitionType_t prepareMoveToHue(uint16_t hue, uint8_t direction, uint16_t transitionTime,
                                         bool byStep);
static TransitionType_t prepareMoveToSaturation(uint8_t saturation, uint16_t transitionTime,
                                                bool byStep);
static bool prepareMoveToColor(uint16_t x, uint16_t y, uint16_t transitionTime);
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
static void ZCL_ColorControlAttributeEventInd(ZCL_Addressing_t *addressing,
  ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event);
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
static bool prepareMoveToColorTemperature(uint16_t temperature, uint16_t transitionTime, bool byStep);
static void handleColorTemperatureTransition(void);
static bool prepareMoveColorTemperature(uint8_t moveMode, uint16_t rate, uint16_t tempMin, uint16_t tempMax);

#if ZLO_CLUSTER_ENHANCEMENTS == 1
static void lightUpdateStartupColorTemperature(void);

static void lightColorControlWriteAttributeEventHandler(SYS_EventId_t eventId, SYS_EventData_t data);

static SYS_EventReceiver_t lightColorControlWriteAttributeEvent = { .func = lightColorControlWriteAttributeEventHandler};
#endif
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT


/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_ColorControlClusterServerAttributes_t lightColorControlClusterServerAttributes =
{
  ZCL_DEFINE_COLOR_CONTROL_SERVER_CLUSTER_ATTRIBUTES(0, 0)
};

PROGMEM_DECLARE(ZCL_ColorControlClusterCommands_t lightColorControlClusterServerCommands) =
{
#if APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  ZCL_COLOR_CONTROL_CLUSTER_COMMANDS_FOR_EXTENDED_COLOR_LIGHT(moveToHueInd, moveHueInd, stepHueInd,
      moveToSaturationInd, moveSaturationInd, stepSaturationInd, moveToHueAndSaturationInd, moveToColorInd,
      moveColorInd, stepColorInd, moveToColorTemperatureInd, enhancedMoveToHueInd,
      enhancedMoveHueInd, enhancedStepHueInd, enhancedMoveToHueAndSaturationInd, colorLoopSetInd,
      stopMoveStepInd, moveColorTemperatureInd, stepColorTemperatureInd)
#elif APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_LIGHT
  ZCL_COLOR_CONTROL_CLUSTER_COMMANDS_FOR_COLOR_LIGHT(moveToHueInd, moveHueInd, stepHueInd,
      moveToSaturationInd, moveSaturationInd, stepSaturationInd, moveToHueAndSaturationInd, moveToColorInd,
      moveColorInd, stepColorInd, enhancedMoveToHueInd, enhancedMoveHueInd, enhancedStepHueInd,
      enhancedMoveToHueAndSaturationInd, colorLoopSetInd, stopMoveStepInd)
#else
  ZCL_COLOR_CONTROL_CLUSTER_COMMANDS_FOR_TEMPERATURE_COLOR_LIGHT(moveToColorTemperatureInd,
    moveColorTemperatureInd, stepColorTemperatureInd)
#endif
};

/******************************************************************************
                    Local variables
******************************************************************************/
static HAL_AppTimer_t updateTimer;
#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
static uint8_t bckpSaturation;
static uint16_t bckpEnhacnedHue;
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT

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
void lightColorControlClusterInit(void)
{
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
  ZCL_Cluster_t *cluster =  ZCL_GetCluster(APP_ENDPOINT_LIGHT, COLOR_CONTROL_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
  if (cluster != NULL)
  {
    cluster->ZCL_AttributeEventInd = ZCL_ColorControlAttributeEventInd;
  }
#endif //#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

  if (!PDS_IsAbleToRestore(APP_LIGHT_COLOR_CONTROL_MEM_ID))
  {
#if APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
    uint8_t colorCapabilities = 0;
#else
    uint8_t colorCapabilities = 1;
#endif // APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
    lightColorControlClusterServerAttributes.currentHue.value                 = ZCL_ZLL_CLUSTER_CURRENT_HUE_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.currentSaturation.value          = DEFAULT_SATURATION_LEVEL;
    lightColorControlClusterServerAttributes.remainingTime.value              = ZCL_ZLL_CLUSTER_REMAINING_TIME_DEFAULT_VALUE;

    lightColorControlClusterServerAttributes.currentX.value                   = ZCL_ZLL_CLUSTER_CURRENT_X_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.currentY.value                   = ZCL_ZLL_CLUSTER_CURRENT_Y_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorTemperature.value           = ZCL_ZCL_CLUSTER_COLOR_TEMP_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorMode.value                  = ZCL_ZLL_CLUSTER_COLOR_MODE_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.numberOfPrimaries.value          = 1;

    lightColorControlClusterServerAttributes.primary1X.value                  = 41942; // red
    lightColorControlClusterServerAttributes.primary1Y.value                  = 20971; // red
    lightColorControlClusterServerAttributes.primary1XIntensity.value         = 0;

    lightColorControlClusterServerAttributes.primary2X.value                  = 0;
    lightColorControlClusterServerAttributes.primary2Y.value                  = 0;
    lightColorControlClusterServerAttributes.primary2XIntensity.value         = 0;

    lightColorControlClusterServerAttributes.primary3X.value                  = 0;
    lightColorControlClusterServerAttributes.primary3Y.value                  = 0;
    lightColorControlClusterServerAttributes.primary3XIntensity.value         = 0;

    lightColorControlClusterServerAttributes.primary4X.value                  = 0;
    lightColorControlClusterServerAttributes.primary4Y.value                  = 0;
    lightColorControlClusterServerAttributes.primary4XIntensity.value         = 0;

    lightColorControlClusterServerAttributes.primary5X.value                  = 0;
    lightColorControlClusterServerAttributes.primary5Y.value                  = 0;
    lightColorControlClusterServerAttributes.primary5XIntensity.value         = 0;

    lightColorControlClusterServerAttributes.primary6X.value                  = 0;
    lightColorControlClusterServerAttributes.primary6Y.value                  = 0;
    lightColorControlClusterServerAttributes.primary6XIntensity.value         = 0;

    lightColorControlClusterServerAttributes.enhancedCurrentHue.value         = ZCL_ZCL_CLUSTER_ENHANCED_CURRENT_HUE_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.enhancedColorMode.value          = ZCL_ZCL_CLUSTER_ENHANCED_COLOR_MODE_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorLoopActive.value            = ZCL_ZCL_CLUSTER_COLOR_LOOP_ACTIVE_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorLoopDirection.value         = ZCL_ZCL_CLUSTER_COLOR_LOOP_DIRECTION_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorLoopTime.value              = ZCL_ZCL_CLUSTER_COLOR_LOOP_TIME_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorLoopStartEnhancedHue.value  = ZCL_ZCL_CLUSTER_COLOR_LOOP_START_ENHANCED_HUE_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorLoopStoredEnhancedHue.value = ZCL_ZCL_CLUSTER_COLOR_LOOP_STORED_ENHANCED_HUE_DEFAULT_VALUE;

    lightColorControlClusterServerAttributes.colorCapabilities.value.hueSaturationSupported = colorCapabilities;
    lightColorControlClusterServerAttributes.colorCapabilities.value.enhancedHueSupported   = colorCapabilities;
    lightColorControlClusterServerAttributes.colorCapabilities.value.colorLoopSupported     = colorCapabilities;
    lightColorControlClusterServerAttributes.colorCapabilities.value.XYAttributesSupported  = colorCapabilities;

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
    lightColorControlClusterServerAttributes.colorCapabilities.value.colorTemperatureSupported = 1;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
    lightColorControlClusterServerAttributes.startUpColorTemperatureMireds.value = ZCL_ZLL_STARTUP_COLOR_TEMPERATURE_PREVIOUS;
    lightColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value = ZCL_ZCL_CLUSTER_TEMP_PHYSICAL_MIN_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.options.value = 0x00; // Bit#0: ExecuteIfOff
#endif
    gExecuteIfOff = false;
#else
    lightColorControlClusterServerAttributes.colorCapabilities.value.colorTemperatureSupported = 0;
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
    lightColorControlClusterServerAttributes.colorTempPhysicalMin.value = ZCL_ZCL_CLUSTER_TEMP_PHYSICAL_MIN_DEFAULT_VALUE;
    lightColorControlClusterServerAttributes.colorTempPhysicalMax.value = ZCL_ZCL_CLUSTER_TEMP_PHYSICAL_MAX_DEFAULT_VALUE;
  lightColorControlClusterServerAttributes.currentHue.minReportInterval = COLOR_CONTROL_CUURENT_HUE_MIN_REPORT_PERIOD;
  lightColorControlClusterServerAttributes.currentHue.maxReportInterval = COLOR_CONTROL_CUURENT_HUE_MAX_REPORT_PERIOD;

  lightColorControlClusterServerAttributes.currentSaturation.minReportInterval = COLOR_CONTROL_CURRENT_SAT_MIN_REPORT_PERIOD;
  lightColorControlClusterServerAttributes.currentSaturation.maxReportInterval = COLOR_CONTROL_CURRENT_SAT_MAX_REPORT_PERIOD;

  lightColorControlClusterServerAttributes.currentX.minReportInterval = COLOR_CONTROL_CURR_X_MIN_REPORT_PERIOD;
  lightColorControlClusterServerAttributes.currentX.maxReportInterval = COLOR_CONTROL_CURR_X_MAX_REPORT_PERIOD;

  lightColorControlClusterServerAttributes.currentY.minReportInterval = COLOR_CONTROL_CURR_Y_MIN_REPORT_PERIOD;
  lightColorControlClusterServerAttributes.currentY.maxReportInterval = COLOR_CONTROL_CURR_Y_MAX_REPORT_PERIOD;

  lightColorControlClusterServerAttributes.colorTemperature.minReportInterval = COLOR_CONTROL_COLOR_TEMP_MIN_REPORT_PERIOD;
  lightColorControlClusterServerAttributes.colorTemperature.maxReportInterval = COLOR_CONTROL_COLOR_TEMP_MAX_REPORT_PERIOD;

  lightColorControlClusterServerAttributes.clusterVersion.value = COLOR_CONTROL_CLUSTER_VERSION;
  }
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT)
  else
  {
    (void)PDS_Restore(APP_LIGHT_COLOR_CONTROL_MEM_ID);
  }
#endif //(APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT)

  updateTimer.mode = TIMER_REPEAT_MODE;
  updateTimer.interval = UPDATE_TIMER_INTERVAL;
  updateTimer.callback = updateTransitionState;

#if ZLO_CLUSTER_ENHANCEMENTS == 1
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  lightUpdateStartupColorTemperature();

  SYS_SubscribeToEvent((uint8_t)BC_ZCL_EVENT_ACTION_REQUEST, &lightColorControlWriteAttributeEvent);
#endif //#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
#endif

  displayStatus();

}

/**************************************************************************//**
\brief Set Extension Field Set
******************************************************************************/
void colorControlClusterSetExtensionField(Scene_t *scene, uint16_t transitionTime)
{
  uint8_t proceedWith = 0;
  setColorMode((uint8_t)(scene->colorMode));
#if (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_LIGHT) || (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT)
  if (0xffffU == transitionTime || 0U == transitionTime)
  {
    setHue(scene->enhancedCurrentHue);
    setSaturation(scene->currentSaturation);
  }
  else
  {
    if((uint8_t)(prepareMoveToHue(scene->enhancedCurrentHue, 1 , transitionTime, false)) != 0U)
    {
      proceedWith = (uint8_t)HUE;
    }
    if((uint8_t)(prepareMoveToSaturation(scene->currentSaturation, transitionTime, false)) != 0U)
    {
      proceedWith |= (uint8_t)SATURATION;
    }
  }
  setColorLoop(scene->colorLoopActive, scene->colorLoopDirection, scene->colorLoopTime);
#endif // (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_COLOR_LIGHT) || (APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT)
#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
  if (0xffffU == transitionTime || 0U == transitionTime)
  {
    setColor(scene->currentX, scene->currentY);
  }
  else
  {
    if(prepareMoveToColor(scene->currentX, scene->currentY, transitionTime))
    {
      proceedWith |= (uint8_t)COLOR;
    }
  }
#endif   
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  if (0xffffU == transitionTime || 0U == transitionTime)
  {
    setColorTemperature(scene->colorTemperature);
  }
  else
  {
    if(prepareMoveToColorTemperature(scene->colorTemperature, transitionTime, false))
    {
      proceedWith |= (uint8_t)TEMPERATURE;
    }
  }
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  if (proceedWith != 0U)
  {
    transitionStart((TransitionType_t)proceedWith);
  }
}

INLINE uint8_t levelControlCalculateIntensity(void)
{
  // Wrong in general..
  return (lightColorControlClusterServerAttributes.currentSaturation.value == (uint8_t)UINT8_MAX ?
         (uint8_t)UINT8_MAX - 1U : lightColorControlClusterServerAttributes.currentSaturation.value);
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
  {
    return;
  }
    // add information about color temperature
#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
  LCD_PRINT(0, 1, 
   "%5u  %5u %5u %3u",
            lightColorControlClusterServerAttributes.currentX.value,
            lightColorControlClusterServerAttributes.currentY.value,
            lightColorControlClusterServerAttributes.enhancedCurrentHue.value,
            lightColorControlClusterServerAttributes.currentSaturation.value
           );
  switch (lightColorControlClusterServerAttributes.colorMode.value)
  {

    case (uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION:
    {
      uint32_t tmpColorX, tmpColorY;
      // Get the X and Y values for the required hue and saturation
      HS2XY((int32_t)((uint16_t)(lightColorControlClusterServerAttributes.enhancedCurrentHue.value) >> 8),
        (int)lightColorControlClusterServerAttributes.currentSaturation.value,
        (unsigned int *)&tmpColorX,(unsigned int *)&tmpColorY);        
      // Set the LED color for the X and Y values
      LEDS_SET_COLOR_XY((uint16_t)tmpColorX,(uint16_t)tmpColorY);
      lightColorControlClusterServerAttributes.currentX.value = (uint16_t)tmpColorX;
      lightColorControlClusterServerAttributes.currentY.value = (uint16_t)tmpColorY;
     }
      break;
    case (uint8_t)ZCL_ZLL_COLOR_TEMPERATURE:
    {
      uint32_t tmpColorX, tmpColorY;
      lightConvertColorToXY(lightColorControlClusterServerAttributes.colorTemperature.value,
        (uint16_t *)&tmpColorX, (uint16_t *)&tmpColorY
      );
      lightColorControlClusterServerAttributes.currentX.value = (uint16_t)tmpColorX;
      lightColorControlClusterServerAttributes.currentY.value = (uint16_t)tmpColorY;
    }
      break;
    // fall through from ZCL_ZLL_COLOR_TEMPERATURE case
    default:
      LEDS_SET_COLOR_XY(
        lightColorControlClusterServerAttributes.currentX.value,
        lightColorControlClusterServerAttributes.currentY.value
      );
      break;
  }
#ifdef _ZCL_REPORTING_SUPPORT_
  (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.currentX);
  (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.currentY);
#endif
#else
  LCD_PRINT(0, 1, "%5u", lightColorControlClusterServerAttributes.colorTemperature.value);
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
}


/**************************************************************************//**
\brief Set current mode
******************************************************************************/
static void setColorMode(uint8_t mode)
{
  lightColorControlClusterServerAttributes.enhancedColorMode.value  = mode;

  if ((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION == mode)
  {
    lightColorControlClusterServerAttributes.colorMode.value = (uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION;
  }
  else
  {
    lightColorControlClusterServerAttributes.colorMode.value = mode;
  }
}


/**************************************************************************//**
\brief Set hue value
******************************************************************************/
static void setHue(uint16_t hue)
{
  if (lightColorControlClusterServerAttributes.enhancedCurrentHue.value != hue)
  {
    lightColorControlClusterServerAttributes.enhancedCurrentHue.value = hue;
    lightColorControlClusterServerAttributes.currentHue.value = (uint8_t)(hue >> 8);
    lightScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.currentHue);
#endif
  }
}

/**************************************************************************//**
\brief Set saturation value
******************************************************************************/
static void setSaturation(uint8_t saturation)
{
  if (lightColorControlClusterServerAttributes.currentSaturation.value != saturation)
  {
    lightColorControlClusterServerAttributes.currentSaturation.value = saturation;
    lightScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.currentSaturation);
#endif
  }
}

/**************************************************************************//**
\brief Set color value
******************************************************************************/
static void setColor(uint16_t x, uint16_t y)
{
  if (lightColorControlClusterServerAttributes.currentX.value != x ||
      lightColorControlClusterServerAttributes.currentY.value != y)
  {
    lightColorControlClusterServerAttributes.currentX.value = x;
    lightColorControlClusterServerAttributes.currentY.value = y;
    lightColorControlClusterServerAttributes.primary1X.value = x;
    lightColorControlClusterServerAttributes.primary1Y.value = y;
    lightColorControlClusterServerAttributes.primary1XIntensity.value =
      levelControlCalculateIntensity();

    lightScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.currentX);
    (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.currentY);
#endif
  }
}

/**************************************************************************//**
\brief Set color temperature value
******************************************************************************/
static void setColorTemperature(uint16_t temperature)
{
  if (lightColorControlClusterServerAttributes.colorTemperature.value != temperature)
  {
    lightColorControlClusterServerAttributes.colorTemperature.value = temperature;
    lightScenesInvalidate();
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.colorTemperature);
#endif
  }
}

#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
/**************************************************************************//**
\brief Set Color Loop values
******************************************************************************/
static void setColorLoop(uint8_t colorLoopActive, uint8_t colorLoopDirection, uint16_t colorLoopTime)
{
  lightColorControlClusterServerAttributes.colorLoopActive.value    = colorLoopActive;
  lightColorControlClusterServerAttributes.colorLoopDirection.value = colorLoopDirection;
  lightColorControlClusterServerAttributes.colorLoopTime.value      = colorLoopTime;

  if (colorLoopActive != 0U)
  {
    startColorLoop(COLOR_LOOP_ACTIVATE_FROM_ECH);
  }
}

/**************************************************************************//**
\brief Stop Color Loop handler.
******************************************************************************/
static void stopColorLoop(void)
{
  transitionStop(ALL);
  lightColorControlClusterServerAttributes.colorLoopActive.value = 0;
  setHue(lightColorControlClusterServerAttributes.colorLoopStoredEnhancedHue.value);
}

/**************************************************************************//**
\brief Starts the color loop specified way.

\param[in] action - color loop mode.
******************************************************************************/
static void startColorLoop(ZCL_ZllColorLoopAction_t action)
{
  lightColorControlClusterServerAttributes.colorLoopStoredEnhancedHue.value =
    lightColorControlClusterServerAttributes.enhancedCurrentHue.value;
  if (COLOR_LOOP_ACTIVATE_FROM_ECH == action)
  {
      lightColorControlClusterServerAttributes.colorLoopStartEnhancedHue.value =
      lightColorControlClusterServerAttributes.enhancedCurrentHue.value;
   }
  lightColorControlClusterServerAttributes.colorLoopActive.value = 1;
  lightColorControlClusterServerAttributes.remainingTime.value = 0xffff;
  setColorMode((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
  transitionStart(COLOR_LOOP);
}
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT

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

  setHue((uint16_t)(hueTransition.current >> 16));
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
      saturation = (uint8_t)(saturationTransition.current >> 16);
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
      saturation = (uint8_t)(saturationTransition.current >> 16);
    }
  }

  setSaturation(saturation);

  if (finish)
  {
    transitionStop(SATURATION);
  }
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
      x = (uint16_t)(colorTransition.currentX >> 16);
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
      x = (uint16_t)(colorTransition.currentX >> 16);
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
      y = (uint16_t)(colorTransition.currentY >> 16);
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
      y = (uint16_t)(colorTransition.currentY >> 16);
    }
  }

  // Common
  setColor(x, y);

  if (finishX && finishY)
  {
    transitionStop(COLOR);
  }
}

/**************************************************************************//**
\brief Handle Color Loop transition
******************************************************************************/
static void handleColorLoopTransition(void)
{
  uint16_t delta = (uint16_t)((MAX_HUE_LEVEL - MIN_HUE_LEVEL) /
                   (lightColorControlClusterServerAttributes.colorLoopTime.value * 10U));
  int32_t enhancedHue = (int32_t)(lightColorControlClusterServerAttributes.enhancedCurrentHue.value);

  // Up transition
  if ((lightColorControlClusterServerAttributes.colorLoopDirection.value) != 0U)
  {
    enhancedHue += (int32_t)delta;
  }
  // Down transition
  else
  {
    enhancedHue -= (int32_t)delta;
  }

  if (enhancedHue < (int32_t)MIN_HUE_LEVEL)
  {
    enhancedHue = (int32_t)MAX_HUE_LEVEL;
  }
  else if (MAX_HUE_LEVEL < (uint32_t)enhancedHue)
  {
    enhancedHue = (int32_t)MIN_HUE_LEVEL;
  }
  else
  {
        //add else for avoid misra rule 15.7
  }
  setHue((uint16_t)enhancedHue);
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
      temp = (uint16_t)(colorTemperatureTransition.current >> 16);
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
      temp = (uint16_t)(colorTemperatureTransition.current >> 16);
    }
  }

  // Common
  setColorTemperature(temp);

  if (finish)
  {
    transitionStop(TEMPERATURE);
  }
}

/**************************************************************************//**
\brief Show identify effect.

\param[in] enhancedHue - enhanced hue to move to.
******************************************************************************/
void colorControlShowIdentifyEffect(uint16_t enhancedHue)
{
#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
  uint8_t proceedWith;
  bckpSaturation = lightColorControlClusterServerAttributes.currentSaturation.value;
  bckpEnhacnedHue = lightColorControlClusterServerAttributes.enhancedCurrentHue.value;
  proceedWith = (uint8_t)prepareMoveToHue(enhancedHue,
    (uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, DEFAULT_TRANSITION_TIME, false);
  proceedWith |= (uint8_t)prepareMoveToSaturation(DEFAULT_SATURATION_LEVEL,
    DEFAULT_TRANSITION_TIME, false);

  if (proceedWith != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart((TransitionType_t)proceedWith);
  }
#else
  (void)enhancedHue;
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
}

/**************************************************************************//**
\brief Stop showing identify effect and return to previous state.
******************************************************************************/
void colorControlStopIdentifyEffect(void)
{
#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
  uint8_t proceedWith;

  proceedWith = (uint8_t)prepareMoveToHue(bckpEnhacnedHue,
    (uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, DEFAULT_TRANSITION_TIME, false);
  proceedWith |= (uint8_t)prepareMoveToSaturation(bckpSaturation, DEFAULT_TRANSITION_TIME, false);

  if (proceedWith != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart((TransitionType_t)proceedWith);
  }
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
}

/**************************************************************************//**
\brief Set target transition value
******************************************************************************/
static void setTargetValue(void)
{
  if (((uint8_t)inTransition & (uint8_t)HUE) != 0U)
  {
    setHue(hueTransition.target);
  }
  if (((uint8_t)inTransition & (uint8_t)SATURATION) != 0U)
  {
    setSaturation(saturationTransition.target);
  }
  if (((uint8_t)inTransition & (uint8_t)COLOR) != 0U)
  {
    setColor(colorTransition.targetX, colorTransition.targetY);
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
    setColorTemperature(lightConvertXYToColor(colorTransition.targetX, colorTransition.targetY));
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  }

  if (((uint8_t)inTransition & (uint8_t)TEMPERATURE) != 0U)
  {
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
    uint16_t x, y;

    lightConvertColorToXY(colorTemperatureTransition.target, &x, &y);
    setColor(x, y);
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
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
    lightColorControlClusterServerAttributes.remainingTime.value = 0;
    (void)HAL_StopAppTimer(&updateTimer);
    return;
  }

  if (lightColorControlClusterServerAttributes.remainingTime.value < 0xffffU)
  {
    lightColorControlClusterServerAttributes.remainingTime.value--;
  }
  if ((lightColorControlClusterServerAttributes.remainingTime.value > 0U) ||
          (lightColorControlClusterServerAttributes.colorLoopActive.value ==1U))
  {
    if (((uint8_t)inTransition & (uint8_t)HUE) != 0U)
    {
      handleHueTransition();
    }
    if (((uint8_t)inTransition & (uint8_t)SATURATION) != 0U)
    {
      handleSaturationTransition();
    }
    else if (((uint8_t)inTransition & (uint8_t)COLOR) != 0U)
    {
      handleColorTransition();
    }
    else if (((uint8_t)inTransition & (uint8_t)COLOR_LOOP) != 0U)
    {
      handleColorLoopTransition();
    }

    else if (((uint8_t)inTransition & (uint8_t)TEMPERATURE) != 0U)
    {
      handleColorTemperatureTransition();
    }
    else
    {
         //add else for avoid misra rule 15.7
    }
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

  if (lightColorControlClusterServerAttributes.remainingTime.value > 0U)
  {
    (void)HAL_StopAppTimer(&updateTimer);
    (void)HAL_StartAppTimer(&updateTimer);
  }
  else
  {
    setTargetValue();
    transitionStop(type);
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
    (void)HAL_StopAppTimer(&updateTimer);
    lightColorControlClusterServerAttributes.remainingTime.value = 0;
  }

  (void)PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);
}

#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
/**************************************************************************//**
\brief Prepare Move To Hue parameters
******************************************************************************/
static TransitionType_t prepareMoveToHue(uint16_t hue, uint8_t direction, uint16_t transitionTime,
                                         bool byStep)
{
  uint16_t delta;
  bool dir = true;

  if (hue == lightColorControlClusterServerAttributes.enhancedCurrentHue.value)
  {
    return NONE;
  }

  hueTransition.target = hue;
  hueTransition.byStep = byStep;


  {
    // Get shortest distance direction and delta
    if (hue > lightColorControlClusterServerAttributes.enhancedCurrentHue.value)
    {
      dir = true;
      delta = hue - lightColorControlClusterServerAttributes.enhancedCurrentHue.value;
    }
    else
    {
      dir = false;
      delta = lightColorControlClusterServerAttributes.enhancedCurrentHue.value - hue;
    }

    // Check if change in direction is needed
    if ((uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_LONGEST_DISTANCE == direction ||
        ((uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_UP == direction && false == dir) ||
        ((uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_DOWN == direction && true == dir))
    {
      dir = !dir;
      delta = (uint16_t)((MAX_HUE_LEVEL - MIN_HUE_LEVEL) - delta);
    }
   if(transitionTime != 0U)
   {
     hueTransition.delta = ((uint32_t)delta << 16) / transitionTime;
   } 
    hueTransition.current = (uint32_t)lightColorControlClusterServerAttributes.enhancedCurrentHue.value << 16;
    hueTransition.direction = dir;
  }

  lightColorControlClusterServerAttributes.remainingTime.value = transitionTime;

  return HUE;
}

/**************************************************************************//**
\brief Prepare Move Hue parameters
******************************************************************************/
static bool prepareMoveHue(uint8_t mode, uint16_t rate)
{
  if ((uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == mode)
  {
    hueTransition.direction = true;
  }
  else
  {
    hueTransition.direction = false;
  }
  hueTransition.current = (uint32_t)lightColorControlClusterServerAttributes.enhancedCurrentHue.value << 16;
  hueTransition.target = 0;
  hueTransition.delta = ((uint32_t)rate << 16) / 10U;
  hueTransition.byStep = false;
  lightColorControlClusterServerAttributes.remainingTime.value = 0xffff;

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

  if (saturation == lightColorControlClusterServerAttributes.currentSaturation.value)
  {
    return NONE;
  } 

  saturationTransition.target = saturation;
  saturationTransition.byStep = byStep;


  {
    // Get shortest distance direction and delta
    if (saturation > lightColorControlClusterServerAttributes.currentSaturation.value)
    {
      dir = true;
      delta = (uint8_t)(saturation - lightColorControlClusterServerAttributes.currentSaturation.value);
    }
    else
    {
      dir = false;
      delta = (uint8_t)(lightColorControlClusterServerAttributes.currentSaturation.value - saturation);
    }
    if(transitionTime != 0U)
    {
      saturationTransition.delta = ((uint32_t)delta << 16) / (uint32_t)transitionTime;
    }
    saturationTransition.current = (uint32_t)lightColorControlClusterServerAttributes.currentSaturation.value << 16;
    saturationTransition.direction = dir;
  }
  lightColorControlClusterServerAttributes.remainingTime.value = transitionTime;

  return SATURATION;
}

/**************************************************************************//**
\brief Prepare Move Saturation parameters
******************************************************************************/
static bool prepareMoveSaturation(uint8_t mode, uint16_t rate)
{
  if ((uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == mode)
  {
    saturationTransition.direction = true;
  }
  else
  {
    saturationTransition.direction = false;
  }
  saturationTransition.current = (uint32_t)lightColorControlClusterServerAttributes.currentSaturation.value << 16;
  saturationTransition.target = 0;
  saturationTransition.delta = ((uint32_t)rate << 16) / 10U;
  saturationTransition.byStep = false;

  lightColorControlClusterServerAttributes.remainingTime.value = 0xffff;
  return true;
}


/**************************************************************************//**
\brief Prepare Move To Color parameters
******************************************************************************/
static bool prepareMoveToColor(uint16_t x, uint16_t y, uint16_t transitionTime)
{
  uint16_t deltax, deltay;
  bool dirx, diry;
  if (x == lightColorControlClusterServerAttributes.currentX.value &&
      y == lightColorControlClusterServerAttributes.currentY.value)
  {
    return false;
  }
  // Get shortest distance direction and delta
  dirx = x > lightColorControlClusterServerAttributes.currentX.value;

  if (dirx)
  {
    deltax = x - lightColorControlClusterServerAttributes.currentX.value;
  }
  else
  {
    deltax = lightColorControlClusterServerAttributes.currentX.value - x;
  }
  diry = y > lightColorControlClusterServerAttributes.currentY.value;

  if (diry)
  {
    deltay = y - lightColorControlClusterServerAttributes.currentY.value;
  }
  else
  {
    deltay = lightColorControlClusterServerAttributes.currentY.value - y;
  }
  if(transitionTime != 0U)
  {
    colorTransition.deltaX = ((uint32_t)deltax << 16) / transitionTime;
    colorTransition.deltaY = ((uint32_t)deltay << 16) / transitionTime;
  }
  colorTransition.currentX = (uint32_t)lightColorControlClusterServerAttributes.currentX.value << 16;
  colorTransition.currentY = (uint32_t)lightColorControlClusterServerAttributes.currentY.value << 16;
  colorTransition.directionX = dirx;
  colorTransition.directionY = diry;
  colorTransition.targetX = x;
  colorTransition.targetY = y;
  lightColorControlClusterServerAttributes.remainingTime.value = transitionTime;
  return true;
}

/**************************************************************************//**
\brief Prepare Move Color parameters
******************************************************************************/
static bool prepareMoveColor(int16_t ratex, int16_t ratey)
{
  // X
  colorTransition.deltaX = ((uint32_t)ABS(ratex) << 16U) / 10U;
  colorTransition.currentX = (uint32_t)lightColorControlClusterServerAttributes.currentX.value << 16;
  colorTransition.directionX = ratex > 0;
  colorTransition.targetX = 0;
  // Y
  colorTransition.deltaY = ((uint32_t)ABS(ratey) << 16U) / 10U;
  colorTransition.currentY = (uint32_t)lightColorControlClusterServerAttributes.currentY.value << 16;
  colorTransition.directionY = ratey > 0;
  colorTransition.targetY = 0;
  lightColorControlClusterServerAttributes.remainingTime.value = 0xffff;
  return true;
}
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
/**************************************************************************//**
\brief Prepare Move To Color Temperature parameters
******************************************************************************/
static bool prepareMoveToColorTemperature(uint16_t temperature, uint16_t transitionTime, bool byStep)
{
  uint16_t deltaTemp;
  bool dirTemp;

  if (temperature == lightColorControlClusterServerAttributes.colorTemperature.value)
  {
    return false;
  }
  lightColorControlClusterServerAttributes.remainingTime.value = transitionTime;
  colorTemperatureTransition.target                       = temperature;
  colorTemperatureTransition.byStep                       = byStep;

  // Get shortest distance direction and delta
  dirTemp = temperature > lightColorControlClusterServerAttributes.colorTemperature.value;
  if (dirTemp)
  {
    deltaTemp = temperature - lightColorControlClusterServerAttributes.colorTemperature.value;
  }
  else
  {
    deltaTemp = lightColorControlClusterServerAttributes.colorTemperature.value - temperature;
  }
  if(transitionTime != 0U)
  {
    colorTemperatureTransition.delta     = ((uint32_t)deltaTemp << 16) / transitionTime;
  }
  colorTemperatureTransition.current   = (uint32_t)lightColorControlClusterServerAttributes.colorTemperature.value << 16;
  colorTemperatureTransition.direction = dirTemp;
  colorTemperatureTransition.minTemp   = lightColorControlClusterServerAttributes.colorTempPhysicalMin.value;
  colorTemperatureTransition.maxTemp   = lightColorControlClusterServerAttributes.colorTempPhysicalMax.value;

  return true;
}

/**************************************************************************//**
\brief Prepare Move Color Temperature parameters
******************************************************************************/
static bool prepareMoveColorTemperature(uint8_t moveMode, uint16_t rate, uint16_t tempMin, uint16_t tempMax)
{
  colorTemperatureTransition.delta     = ((uint32_t)ABS((int)rate) << 16) / 10U;
  colorTemperatureTransition.current   = (uint32_t)lightColorControlClusterServerAttributes.colorTemperature.value << 16;
  colorTemperatureTransition.direction = (uint8_t)ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_UP == moveMode;
  colorTemperatureTransition.target    = 0;
  colorTemperatureTransition.byStep    = false;
  colorTemperatureTransition.minTemp   = MAX(tempMin, lightColorControlClusterServerAttributes.colorTempPhysicalMin.value);
  colorTemperatureTransition.maxTemp   = MIN(tempMax, lightColorControlClusterServerAttributes.colorTempPhysicalMax.value);

  lightColorControlClusterServerAttributes.remainingTime.value = 0xffff;

  return true;
}
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT


#if APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT
/**************************************************************************//**
\brief Callback on receive of Move To Hue command
******************************************************************************/
static ZCL_Status_t moveToHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToHueCommand_t *payload)
{
  uint8_t on, type;
  uint16_t hue;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  //appSnprintf("moveToHueInd()\r\n");
  dbgLcdMsg("moveToHue");
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToHueWithOptionsCommand_t *tmp = (ZCL_ZllMoveToHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  hue = (uint16_t)(((MAX_HUE_LEVEL >> 8) < payload->hue) ? (MAX_HUE_LEVEL >> 8) : payload->hue);
  if ((uint8_t)(prepareMoveToHue(hue << 8, payload->direction, payload->transitionTime, false)) != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(HUE);
  } 

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move Hue command
******************************************************************************/
static ZCL_Status_t moveHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveHueCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  //appSnprintf("moveHueInd()\r\n");
  dbgLcdMsg("moveHue");
  
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveHueWithOptionsCommand_t *tmp = (ZCL_ZllMoveHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  if ((uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == payload->moveMode ||
      (uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (!(bool)payload->rate)
    {
      return ZCL_INVALID_FIELD_STATUS;
    }
    if (prepareMoveHue(payload->moveMode, (uint16_t)payload->rate << 8))
    {
      setColorMode((uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
      transitionStart(HUE);
    }
  }
  else if ((uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_STOP == payload->moveMode)
  {
    transitionStop(ALL);
  }
  else
  {
       //add else for avoid misra rule 15.7
  }
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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  //appSnprintf("stepHueInd()\r\n");
  dbgLcdMsg("stepHue");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepHueWithOptionsCommand_t *tmp = (ZCL_ZllStepHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff &(uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  hue = lightColorControlClusterServerAttributes.enhancedCurrentHue.value;

  if ((uint8_t)ZCL_ZLL_STEP_HUE_STEP_MODE_UP == payload->stepMode)
  {
    hue += (uint16_t)payload->stepSize << 8;
  }
  else if ((uint8_t)ZCL_ZLL_STEP_HUE_STEP_MODE_DOWN == payload->stepMode)
  {
    hue -= (uint16_t)payload->stepSize << 8;
  }
  else
  {
    return ZCL_SUCCESS_STATUS;
  }
  if ((uint8_t)(prepareMoveToHue(hue, (uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, true)) != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(HUE);
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move To Saturation command
******************************************************************************/
static ZCL_Status_t moveToSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToSaturationCommand_t *payload)
{
  uint8_t on, type, saturation;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  //appSnprintf("moveToSaturationInd()\r\n");
  dbgLcdMsg("moveToSat");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToSaturationWithOptionsCommand_t *tmp = (ZCL_ZllMoveToSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  saturation = (uint8_t)((MAX_SATURATION_LEVEL < payload->saturation) ? MAX_SATURATION_LEVEL : payload->saturation);
  if ((uint8_t)(prepareMoveToSaturation(saturation, payload->transitionTime, false)) != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(SATURATION);
  }
  
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move Saturation command
******************************************************************************/
static ZCL_Status_t moveSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveSaturationCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  //appSnprintf("moveSaturationInd()\r\n");
  dbgLcdMsg("moveSat");
 
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveSaturationWithOptionsCommand_t *tmp = (ZCL_ZllMoveSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  if (!(bool)(payload->rate))
  {
    return ZCL_INVALID_FIELD_STATUS;
  }
  if ((uint8_t)ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_UP == payload->moveMode ||
      (uint8_t)ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (prepareMoveSaturation(payload->moveMode, payload->rate))
    {
      setColorMode((uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
      transitionStart(SATURATION);
    }
  }
  else if ((uint8_t)ZCL_ZLL_MOVE_SATURATION_MOVE_MODE_STOP == payload->moveMode)
  {
    transitionStop(ALL);
  }
  else
  {
       //add else for avoid misra rule 15.7
  }
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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  //appSnprintf("stepSaturationInd()\r\n");
  dbgLcdMsg("stepSat");
  
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepSaturationWithOptionsCommand_t *tmp = (ZCL_ZllStepSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  saturation = (int16_t)(lightColorControlClusterServerAttributes.currentSaturation.value);

  if ((uint8_t)ZCL_ZLL_STEP_SATURATION_STEP_MODE_UP == payload->stepMode)
  {
    saturation += (int16_t)(payload->stepSize);
  }
  else if ((uint8_t)ZCL_ZLL_STEP_HUE_STEP_MODE_DOWN == payload->stepMode)
  {
    saturation -= (int16_t)(payload->stepSize);
  }
  else
  {
    return ZCL_SUCCESS_STATUS;
  }
  if ((uint8_t)(prepareMoveToSaturation((uint8_t)saturation, payload->transitionTime, true)) !=0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(SATURATION);
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move To Hue And Saturation command
******************************************************************************/
static ZCL_Status_t moveToHueAndSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToHueAndSaturationCommand_t *payload)
{
  uint8_t proceedWith;
  uint8_t on, type, saturation;
  uint16_t hue;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  //appSnprintf("moveToHueAndSaturationInd()\r\n");
  dbgLcdMsg("moveToHueAndSat");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_HUE_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1 
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToHueAndSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToHueAndSaturationWithOptionsCommand_t *tmp = (ZCL_ZllMoveToHueAndSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  hue = (uint16_t)(((MAX_HUE_LEVEL >> 8) < payload->hue) ? (MAX_HUE_LEVEL >> 8) : payload->hue);
  saturation = (uint8_t)((MAX_SATURATION_LEVEL < payload->saturation) ? MAX_SATURATION_LEVEL : payload->saturation);

  proceedWith = (uint8_t)prepareMoveToHue((uint16_t)hue << 8, (uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, false);
  proceedWith |= (uint8_t)prepareMoveToSaturation(saturation, payload->transitionTime, false);

  if (proceedWith != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart((TransitionType_t)proceedWith);
  }
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move To Color command
******************************************************************************/
static ZCL_Status_t moveToColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToColorCommand_t *payload)
{
  uint8_t on, type;
  uint16_t x, y;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  //appSnprintf("moveToColorInd()\r\n");
  dbgLcdMsg("moveToColor");
 
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_COLOR;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToColorCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToColorWithOptionsCommand_t *tmp = (ZCL_ZllMoveToColorWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  x = (uint16_t)((MAX_COLOR_LEVEL < payload->colorX) ? MAX_COLOR_LEVEL : payload->colorX);
  y = (uint16_t)((MAX_COLOR_LEVEL < payload->colorY) ? MAX_COLOR_LEVEL : payload->colorY);

  if (prepareMoveToColor(x, y, payload->transitionTime))
  {
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_X_AND_CURRENT_Y);
    transitionStart(COLOR);
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move Color command
******************************************************************************/
static ZCL_Status_t moveColorInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveColorCommand_t *payload)
{
  uint8_t on, type;

  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;

  //appSnprintf("moveColorInd()\r\n");
  dbgLcdMsg("moveColor");
 
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_COLOR;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveColorCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveColorWithOptionsCommand_t *tmp = (ZCL_ZllMoveColorWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_X_AND_CURRENT_Y);
    transitionStart(COLOR);
  }

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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  //appSnprintf("stepColorInd()\r\n");
  dbgLcdMsg("stepColor");
  
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_COLOR;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepColorCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepColorWithOptionsCommand_t *tmp = (ZCL_ZllStepColorWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  x = (int32_t)lightColorControlClusterServerAttributes.currentX.value + payload->stepX;
  y = (int32_t)lightColorControlClusterServerAttributes.currentY.value + payload->stepY;

  if (prepareMoveToColor((uint16_t)x, (uint16_t)y, payload->transitionTime))
  {
    setColorMode((uint8_t)ZCL_ZLL_CURRENT_X_AND_CURRENT_Y);
    transitionStart(COLOR);
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Enhanced Move To Hue command
******************************************************************************/
static ZCL_Status_t enhancedMoveToHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveToHueCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  //appSnprintf("enhancedMoveToHueInd()\r\n");
  dbgLcdMsg("eMoveToHue");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_MOVE_TO_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedMoveToHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedMoveToHueWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedMoveToHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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
  setColorMode((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
  if ((uint8_t)(prepareMoveToHue(payload->enhancedHue, payload->direction, payload->transitionTime, false)) != 0U)
  {
    transitionStart(HUE);
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Enhanced Move Hue command
******************************************************************************/
static ZCL_Status_t enhancedMoveHueInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveHueCommand_t *payload)
{
  uint8_t on, type;

  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  //appSnprintf("enhancedMoveHueInd()\r\n");
  dbgLcdMsg("eMoveHue");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_MOVE_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedMoveHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedMoveHueWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedMoveHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  if ((uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_UP == payload->moveMode ||
      (uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (!(bool)(payload->rate))
    {
      return ZCL_INVALID_FIELD_STATUS;
    }
    if (prepareMoveHue(payload->moveMode, payload->rate))
    {
      setColorMode((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
      transitionStart(HUE);
    }
  }
  else if ((uint8_t)ZCL_ZLL_MOVE_HUE_MOVE_MODE_STOP == payload->moveMode)
  {
    transitionStop(ALL);
  }
  else
  {
       //add else for avoid misra rule 15.7
  }
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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);

  //appSnprintf("enhancedStepHueInd()\r\n");
  dbgLcdMsg("eStepHue");
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_STEP_HUE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedStepHueCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedStepHueWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedStepHueWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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
  hue = lightColorControlClusterServerAttributes.enhancedCurrentHue.value;
  if ((uint8_t)ZCL_ZLL_STEP_HUE_STEP_MODE_UP == payload->stepMode)
  {
    hue += (uint16_t)payload->stepSize;
  }
  else if ((uint8_t)ZCL_ZLL_STEP_HUE_STEP_MODE_DOWN == payload->stepMode)
  {
    hue -= (uint16_t)payload->stepSize;
  }
  else
  {
    return ZCL_SUCCESS_STATUS;
  }
  if ((uint8_t)(prepareMoveToHue(hue, (uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, true)) != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart(HUE);
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Enhanced Move To Hue And Saturation command
******************************************************************************/
static ZCL_Status_t enhancedMoveToHueAndSaturationInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t *payload)
{
  uint8_t proceedWith;
  uint8_t on, type;
  uint16_t saturation;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;

  //appSnprintf("enhancedMoveToHueAndSaturationInd()\r\n");
  dbgLcdMsg("eMTHAS");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ENHANCED_MOVED_TO_HUE_SATURATION;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1 
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllEnhancedMoveToHueAndSaturationCommand_t))
    {
      /* Check for options override */
      ZCL_ZllEnhancedMoveToHueAndSaturationWithOptionsCommand_t *tmp = (ZCL_ZllEnhancedMoveToHueAndSaturationWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  saturation = (uint16_t)((MAX_SATURATION_LEVEL < payload->saturation) ? MAX_SATURATION_LEVEL : payload->saturation);

  proceedWith = (uint8_t)prepareMoveToHue(payload->enhancedHue, (uint8_t)ZCL_ZLL_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE, payload->transitionTime, false);
  proceedWith |= (uint8_t)prepareMoveToSaturation((uint8_t)saturation, payload->transitionTime, false);

  if (proceedWith != 0U)
  {
    setColorMode((uint8_t)ZCL_ZLL_ENHANCED_CURRENT_HUE_AND_CURRENT_SATURATION);
    transitionStart((TransitionType_t)proceedWith);
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Color Loop Set command
******************************************************************************/
static ZCL_Status_t colorLoopSetInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllColorLoopSetCommand_t *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);


  //appSnprintf("colorLoopSetInd()\r\n");
  dbgLcdMsg("colorLoopSet");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_COLOR_LOOP_SET;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllColorLoopSetCommand_t))
    {
      /* Check for options override */
      ZCL_ZllColorLoopSetWithOptionsCommand_t *tmp = (ZCL_ZllColorLoopSetWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  if ((COLOR_LOOP_UPDATE_DIRECTION & payload->updateFlags) != 0U)
  {
    lightColorControlClusterServerAttributes.colorLoopDirection.value = payload->direction;
  }

  if ((COLOR_LOOP_UPDATE_TIME & payload->updateFlags) != 0U)
  {
    lightColorControlClusterServerAttributes.colorLoopTime.value = payload->colorLoopTime;
  }
  if ((COLOR_LOOP_UPDATE_START_HUE & payload->updateFlags) != 0U)
  {
    lightColorControlClusterServerAttributes.colorLoopStartEnhancedHue.value = payload->startHue;
  }
  if ((COLOR_LOOP_UPDATE_ACTION & payload->updateFlags) != 0U)
  {
    switch (payload->action)
    {
      case COLOR_LOOP_DEACTIVATE:
        if ((lightColorControlClusterServerAttributes.colorLoopActive.value) != 0U)
        {
          stopColorLoop();
        }
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

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Stop Move Step command
******************************************************************************/
static ZCL_Status_t stopMoveStepInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, void *payload)
{
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  dbgLcdMsg("stopMoveStep");
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STOP_MOVE_STEP;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > 0U)
    {
      /* Check for options override */
      ZCL_ZllStopMoveStepWithOptionsCommand_t *tmp = (ZCL_ZllStopMoveStepWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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
  
  return ZCL_SUCCESS_STATUS;
}
#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_TEMPERATURE_COLOR_LIGHT

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
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
    (void)PDS_Store(APP_LIGHT_COLOR_CONTROL_MEM_ID);
  }

}

#endif // APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_COLOR_LIGHT

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
/**************************************************************************//**
\brief Callback on receive of MoveToColorTemperature command
******************************************************************************/
static ZCL_Status_t moveToColorTemperatureInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_ZllMoveToColorTemperatureCommand_t *payload)
{
  uint16_t miredValue;
  uint8_t on, type;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);


  //appSnprintf("moveToColorTemperatureInd()\r\n");
  dbgLcdMsg("moveToColorTemperature");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_COLOR_TEMPERATURE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveToColorTemperatureCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveToColorTemperatureWithOptionsCommand_t *tmp = (ZCL_ZllMoveToColorTemperatureWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  if (lightColorControlClusterServerAttributes.colorTempPhysicalMin.value > payload->colorTemperature)
  {
    miredValue = lightColorControlClusterServerAttributes.colorTempPhysicalMin.value;
  }
  else if (lightColorControlClusterServerAttributes.colorTempPhysicalMax.value < payload->colorTemperature)
  {
    miredValue = lightColorControlClusterServerAttributes.colorTempPhysicalMax.value;
  }
  else
  {
    miredValue = payload->colorTemperature;
  }

  if (prepareMoveToColorTemperature(miredValue, payload->transitionTime, false))
  {
    setColorMode((uint8_t)ZCL_ZLL_COLOR_TEMPERATURE);
    transitionStart(TEMPERATURE);
  }

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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);


  //appSnprintf( "moveColorTemperatureInd()\r\n");
  dbgLcdMsg("moveColorTemperature");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_COLOR_TEMPERATURE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllMoveColorTemperatureCommand_t))
    {
      /* Check for options override */
      ZCL_ZllMoveColorTemperatureWithOptionsCommand_t *tmp = (ZCL_ZllMoveColorTemperatureWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  if ((uint8_t)ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_STOP != payload->moveMode)
  {
    if (!(bool)(payload->rate))
    {
      return ZCL_INVALID_VALUE_STATUS;
    }
  }
  
  if ((uint8_t)ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_UP == payload->moveMode ||
      (uint8_t)ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_DOWN == payload->moveMode)
  {
    if (prepareMoveColorTemperature(payload->moveMode, payload->rate, payload->colorTemperatureMinimum, payload->colorTemperatureMaximum))
    {
      setColorMode((uint8_t)ZCL_ZLL_COLOR_TEMPERATURE);
      transitionStart(TEMPERATURE);
    }
  }
   else if((uint8_t)ZCL_ZLL_MOVE_COLOR_TEMPERATURE_MOVE_MODE_STOP == payload->moveMode)
   {
     transitionStop(ALL);
   }
   else
   {
        //add else for avoid misra rule 15.7
   }
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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);


  //appSnprintf("stepColorTemperatureInd()\r\n");
  dbgLcdMsg("stepColorTemperature");

  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_COLOR_TEMPERATURE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;
  APP_Zigbee_Handler(event);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_ZllStepColorTemperatureCommand_t))
    {
      /* Check for options override */
      ZCL_ZllStepColorTemperatureWithOptionsCommand_t *tmp = (ZCL_ZllStepColorTemperatureWithOptionsCommand_t *)payload;

      executeIfOff = (bool)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_ZLL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightColorControlClusterServerAttributes.options.value) != 0U)
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

  if (0U == payload->stepSize)
  {
    transitionStop(ALL);
    return ZCL_SUCCESS_STATUS;
  }
  temperature = lightColorControlClusterServerAttributes.colorTemperature.value;

  if ((uint8_t)ZCL_ZLL_STEP_COLOR_TEMPERATURE_STEP_MODE_UP == payload->stepMode)
  {
    if (ABSOLUTE_MAXIMUM_COLOR_TEMP - temperature > payload->stepSize)
    {
      temperature += payload->stepSize;
    }
    else
    {
      temperature = ABSOLUTE_MAXIMUM_COLOR_TEMP;
    }
    if (temperature > payload->colorTemperatureMaximum)
    {
      temperature = payload->colorTemperatureMaximum;
    }
  }
  else if ((uint8_t)ZCL_ZLL_STEP_COLOR_TEMPERATURE_STEP_MODE_DOWN == payload->stepMode)
  {
    if (temperature >= payload->stepSize)
    {
      temperature -= payload->stepSize;
    }
    else
    {
      temperature = 0;
    }

    if (temperature < payload->colorTemperatureMinimum)
    {
      temperature = payload->colorTemperatureMinimum;
    }
  }
  else
  {
    return ZCL_SUCCESS_STATUS;
  }
  if (temperature < lightColorControlClusterServerAttributes.colorTempPhysicalMin.value ||
      lightColorControlClusterServerAttributes.colorTempPhysicalMax.value < temperature)
  {
    return ZCL_INVALID_VALUE_STATUS;
  }
  if (prepareMoveToColorTemperature(temperature, payload->transitionTime, true))
  {
    setColorMode((uint8_t)ZCL_ZLL_COLOR_TEMPERATURE);
    transitionStart(TEMPERATURE);
  }

  return ZCL_SUCCESS_STATUS;
}

#if ZLO_CLUSTER_ENHANCEMENTS == 1
/**************************************************************************//**
\brief Update device ON state as per StartUpOnOff attribute value
******************************************************************************/
static void lightUpdateStartupColorTemperature(void)
{
  /* if the color mode is ColorTemptertaure, startupColormireds decide the startup behaviour */
  if ((uint8_t)ZCL_ZLL_COLOR_TEMPERATURE == lightColorControlClusterServerAttributes.colorMode.value)
  {
    if (ZCL_ZLL_STARTUP_COLOR_TEMPERATURE_PREVIOUS !=
        lightColorControlClusterServerAttributes.startUpColorTemperatureMireds.value)
    {
      // Set the color temperature to startup value
      lightColorControlClusterServerAttributes.colorTemperature.value =
        lightColorControlClusterServerAttributes.startUpColorTemperatureMireds.value;
    }
    /* In case of different value i.e., startUpColorTemp != 0xffff, use the value
     * restored from PDS */
    // Set the color mode to colorTemperature
    lightColorControlClusterServerAttributes.colorMode.value = (uint8_t)ZCL_ZLL_COLOR_TEMPERATURE;
    // Set the enhanced color mode to colorTemperature
    lightColorControlClusterServerAttributes.enhancedColorMode.value = (uint8_t)ZCL_ZLL_COLOR_TEMPERATURE;

    (void)PDS_Store(APP_LIGHT_COLOR_CONTROL_MEM_ID);
  }
}
#endif

/**************************************************************************//**
\brief Update the color temperature on level change if required

\param[in] deviceOn - The on/off status of the device
\param[in] updateColorTemp - Update coupleColorTemp bit overridden in optional fields
\param[in] level    - level the device is moving to
******************************************************************************/
#if ZLO_CLUSTER_ENHANCEMENTS == 1
void lightUpdateCoupledColorTemperature(bool deviceOn, bool updateColorTemp, uint8_t level)
{
  uint16_t tmpColorTempMax, tmpNewColorTemp;

  if (updateColorTemp)
  {
    if (((uint8_t)ZCL_ZLL_COLOR_TEMPERATURE == lightColorControlClusterServerAttributes.colorMode.value)
        || ((uint8_t)ZCL_ZLL_COLOR_TEMPERATURE == lightColorControlClusterServerAttributes.enhancedColorMode.value))
    {
      switch (level)
      {        
        case ZCL_LEVEL_CONTROL_MAXIMUM_LEVEL:      
          lightColorControlClusterServerAttributes.colorTemperature.value =
            lightColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value;
          break;

        case ZCL_LEVEL_CONTROL_MINIMUM_LEVEL:
          lightColorControlClusterServerAttributes.colorTemperature.value =
            lightColorControlClusterServerAttributes.colorTempPhysicalMax.value;
          break;

        default:
        {
          tmpColorTempMax = lightColorControlClusterServerAttributes.colorTempPhysicalMax.value -
            lightColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value;

          tmpNewColorTemp = (uint16_t)(tmpColorTempMax - ((tmpColorTempMax / ZCL_LEVEL_CONTROL_MAXIMUM_LEVEL) * level));
          tmpNewColorTemp += lightColorControlClusterServerAttributes.coupleColorTempToLevelMinMireds.value;

          lightColorControlClusterServerAttributes.colorTemperature.value = tmpNewColorTemp;
        }
        break;
      } 
#ifdef _ZCL_REPORTING_SUPPORT_
      (void)ZCL_ReportOnChangeIfNeeded(&lightColorControlClusterServerAttributes.colorTemperature);
#endif
      if (deviceOn)
      {
        displayStatus();
      }
    }
    (void)PDS_Store(APP_LIGHT_COLOR_CONTROL_MEM_ID);
  }
}

/**************************************************************************//**
  \brief Processes BC_ZCL_EVENT_ACTION_REQUEST

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data
******************************************************************************/
static void lightColorControlWriteAttributeEventHandler(SYS_EventId_t eventId, SYS_EventData_t data)
{
  BcZCLActionReq_t *const actionReq = (BcZCLActionReq_t*)data;
  
  if(eventId == (uint8_t)BC_ZCL_EVENT_ACTION_REQUEST) 
  {
    if (ZCL_ACTION_SPL_WRITE_ATTR_REQUEST == actionReq->action)
    {
      uint16_t attrVal = 0;
      ZCLActionWriteAttrReq_t* zclWriteAttrReqInfo  = (ZCLActionWriteAttrReq_t*)actionReq->context;
      (void)memcpy(&attrVal, (uint16_t *)zclWriteAttrReqInfo->attrValue, sizeof(uint16_t));
      if(((uint16_t)COLOR_CONTROL_CLUSTER_ID == zclWriteAttrReqInfo->clusterId) &&
              (ZCL_ZLL_CLUSTER_STARTUP_COLOR_TEMPERATURE_SERVER_ATTRIBUTE_ID == zclWriteAttrReqInfo->attrId) &&
              (ZCL_ZLL_STARTUP_COLOR_TEMPERATURE_PREVIOUS == attrVal))
      {
          zclWriteAttrReqInfo->status = ZCL_SUCCESS_STATUS;
      }
    }
  }
}

#endif
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

#elif APP_Z3_DEVICE_TYPE != APP_DEVICE_TYPE_COLOR_SCENE_CONTROLLER
  void colorControlClusterInit(void) {;}
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT

// eof lightColorControlCluster.c
