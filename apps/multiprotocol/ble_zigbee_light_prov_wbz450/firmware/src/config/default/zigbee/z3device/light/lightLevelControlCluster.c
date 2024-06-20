/*******************************************************************************
  Light Level Control Cluster Source file

  Company:
    Microchip Technology Inc.

  File Name:
   lightLevelControlCluster.c

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

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT

/******************************************************************************
                    Includes section
******************************************************************************/
#include <z3device/clusters/include/haClusters.h>
#if defined(BOARD_MEGARF) || defined(BOARD_SAMR21)
#include <ledsExt.h>
#include <lcdExt.h>
#endif
#include <zcl/include/zclCommandManager.h>
#include <z3device/light/include/lightOnOffCluster.h>
#include <z3device/light/include/lightScenesCluster.h>
#include <z3device/common/include/z3Device.h>
#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightColorControlCluster.h>
#include <pds/include/wlPdsMemIds.h>
#include <z3device/light/include/light.h>
#include <z3device/clusters/include/onOffCluster.h>
#include <app_zigbee/zigbee_console/console.h>
#include <app_zigbee/app_zigbee_handler.h>
#include <z3device/clusters/include/levelControlCluster.h>

/******************************************************************************
                    Prototypes section
******************************************************************************/
static ZCL_Status_t moveToLevelInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t moveInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t stepInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t stopInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t moveToLevelWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t moveWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t stepWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t stopWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);

static void moveToLevel(bool wOnOff, bool execIfOff, uint8_t newLevel, uint16_t transitionTime);
static void stopMoving(void);
static void updateTransitionState(void);
static void adjustOnOffState(uint8_t level);

static void ZCL_LevelControlAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event);

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_LevelControlClusterServerAttributes_t lightLevelControlClusterServerAttributes =
{
  ZCL_DEFINE_LEVEL_CONTROL_CLUSTER_SERVER_MANDATORY_ATTRIBUTES(0, 0)
};

PROGMEM_DECLARE(ZCL_LevelControlClusterCommands_t lightLevelControlCommands) =
{
  ZCL_DEFINE_LEVEL_CONTROL_CLUSTER_COMMANDS(moveToLevelInd, moveInd, stepInd, stopInd,
      moveToLevelWOnOffInd, moveWOnOffInd, stepWOnOffInd, stopWOnOffInd)
};

/******************************************************************************
                    Local variables
******************************************************************************/
static HAL_AppTimer_t updateTimer;
static uint8_t gTargetLevel;
static uint16_t gTransitionTime;
static int16_t gDelta;
static bool gWithOnOff;
/* gExecuteIfOff:
 * --------------
 * Used with the "Execute If Off" behavior for Level Control commands
 * that are received <<< WithOUT On/Off >>>
 *
 * true  => if the device is in OFF state, level control command
 *          WILL BE PROCESSED but there WILL NOT BE change in LED.
 *          i.e., Execute If, the device is, Off
 *
 * false => if the device is in OFF state, level control command
 *           WILL NOT BE PROCESSED
 */
static bool gExecuteIfOff = false;

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
/* Level change on startup does not affect the color temperature of light
 because, that is a function of StartUpColorTemperature attribute */
static bool gUpdateCoupledColorTemp = false;
#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

/******************************************************************************
                    Implementations
******************************************************************************/

/**************************************************************************//**
\brief Initialize Level Control cluster.
******************************************************************************/
void lightLevelControlClusterInit(void)
{
  ZCL_Cluster_t *cluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT, LEVEL_CONTROL_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT)
  uint8_t level = 0;
  (void)level;
#endif
  if (cluster != NULL)
  {
    cluster->ZCL_AttributeEventInd = ZCL_LevelControlAttributeEventInd;
  }
  if (!PDS_IsAbleToRestore(APP_LIGHT_LEVEL_CONTROL_MEM_ID))
  {
    lightLevelControlClusterServerAttributes.currentLevel.value = MAX_LIGHT_LEVEL / 2U;
    lightLevelControlClusterServerAttributes.remainingTime.value = 0;
    lightLevelControlClusterServerAttributes.clusterVersion.value = LEVEL_CONTROL_CLUSTER_VERSION;
    lightLevelControlClusterServerAttributes.currentLevel.minReportInterval = LEVEL_CONTROL_VAL_MIN_REPORT_PERIOD;
    lightLevelControlClusterServerAttributes.currentLevel.maxReportInterval = LEVEL_CONTROL_VAL_MAX_REPORT_PERIOD;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
    lightLevelControlClusterServerAttributes.startUpCurrentLevel.value = ZCL_LEVEL_CONTROL_STARTUP_LEVEL_PREVIOUS;
    lightLevelControlClusterServerAttributes.options.value = 0x00; /* Bit_0 - ExecuteIfOff; Bit_1 - CoupleColorTempToLevelMinMireds */
#endif
  }
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT)
  else
  {
      (void)PDS_Restore(APP_LIGHT_LEVEL_CONTROL_MEM_ID);
  }
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  lightUpdateStartupLevel();
#endif
  /* To display the LED with the restore current level value afetr reset */
  if (deviceIsOn())
  {
    level = lightLevelControlClusterServerAttributes.currentLevel.value;
  }
  LEDS_SET_BRIGHTNESS(level);
#endif //(APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT)

}

/**************************************************************************//**
\brief Set Extension Field Set
******************************************************************************/
void levelControlClusterSetExtensionField(uint16_t currentLevel, uint16_t transitionTime)
{
  stopMoving();
  if (0xffffU != transitionTime && 0U != transitionTime)
  {
    moveToLevel(1, 0, (uint8_t)currentLevel, transitionTime);
  }
  else
  {
    setLevel(false, false, (uint8_t)currentLevel);
  }
}

/**************************************************************************//**
\brief Set Current Level attribute to a specified value
******************************************************************************/
void setLevel(bool wOnOff, bool execIfOff, uint8_t level)
{
  if (wOnOff)
  {
    adjustOnOffState(level);
  }
  lightLevelControlClusterServerAttributes.currentLevel.value = level;
  lightScenesInvalidate();

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT
  if (!execIfOff)
  {
    levelControlDisplayLevel(lightLevelControlClusterServerAttributes.currentLevel.value);
  }
#endif

#if ZLO_CLUSTER_ENHANCEMENTS == 1
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  lightUpdateCoupledColorTemperature((!execIfOff && deviceIsOn()), gUpdateCoupledColorTemp,
    lightLevelControlClusterServerAttributes.currentLevel.value);
#endif /* #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT */
#endif

#ifdef _ZCL_REPORTING_SUPPORT_
  (void)ZCL_ReportOnChangeIfNeeded(&lightLevelControlClusterServerAttributes.currentLevel);
#endif
}

/**************************************************************************//**
\brief On/off value adjustment regarding to current level

\param[in] level - current level value
******************************************************************************/
static void adjustOnOffState(uint8_t level)
{
  bool turnOn = (level == MIN_LIGHT_LEVEL) ? false : true;

  onOffClusterSetExtensionField(turnOn);

#if APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_ON_OFF_LIGHT
  if(level > MIN_LIGHT_LEVEL)
  {
    appSnprintf("On\r\n");
  }
  else
  {
    appSnprintf("Off\r\n");
  }
#endif
}

/**************************************************************************//**
\brief Stat moving to the specified level
******************************************************************************/
static void moveToLevel(bool wOnOff, bool execIfOff, uint8_t newLevel, uint16_t transitionTime)
{
  int16_t level = (int16_t)newLevel;
  stopMoving();

  gTransitionTime = transitionTime;
  gTargetLevel = newLevel;
  gDelta = (int16_t)(level - (int16_t)lightLevelControlClusterServerAttributes.currentLevel.value);
  gWithOnOff = wOnOff;
  gExecuteIfOff = execIfOff;

  updateTimer.mode = TIMER_REPEAT_MODE;
  updateTimer.interval = UPDATE_TIMER_INTERVAL;
  updateTimer.callback = updateTransitionState;
  (void)HAL_StopAppTimer(&updateTimer);
  (void)HAL_StartAppTimer(&updateTimer);
  lightLevelControlClusterServerAttributes.remainingTime.value = transitionTime;
}

/**************************************************************************//**
\brief Update transition state
******************************************************************************/
static void updateTransitionState(void)
{
  if (!deviceIsOn() && !gWithOnOff && !gExecuteIfOff)
  {/* If the LC command is WITH on/off, then stop transition when the light is
    * turned off while the transition in progress. */
    lightLevelControlClusterServerAttributes.remainingTime.value = 0;
    (void)HAL_StopAppTimer(&updateTimer);
    return;
  }

  if (lightLevelControlClusterServerAttributes.remainingTime.value > 0U)
  {
    int32_t remainingTime = (int32_t)(lightLevelControlClusterServerAttributes.remainingTime.value);
    int16_t newLevel = (int16_t)((int16_t)gTargetLevel - ((remainingTime * gDelta) / (int32_t)gTransitionTime));
	lightLevelControlClusterServerAttributes.remainingTime.value--;
    setLevel(gWithOnOff, gExecuteIfOff, (uint8_t)newLevel);
  }
  else
  {
    (void)HAL_StopAppTimer(&updateTimer);
    setLevel(gWithOnOff, gExecuteIfOff, gTargetLevel);
    gWithOnOff = false;
    gExecuteIfOff = false;

    appSnprintf(" + Level transition is finished. Current level is %u\r\n", gTargetLevel);

    (void)PDS_Store(APP_LIGHT_LEVEL_CONTROL_MEM_ID);
  }
}

/**************************************************************************//**
\brief Stop moving
******************************************************************************/
static void stopMoving(void)
{
  (void)HAL_StopAppTimer(&updateTimer);
  gWithOnOff = false;
  lightLevelControlClusterServerAttributes.remainingTime.value = 0;
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT
  if (!gExecuteIfOff)
  {
    levelControlDisplayLevel(lightLevelControlClusterServerAttributes.currentLevel.value);
  }
#endif
}

/**************************************************************************//**
\brief Process Move To Level command
******************************************************************************/
static ZCL_Status_t processMoveToLevel(bool wOnOff, ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_MoveToLevel_t *req = (ZCL_MoveToLevel_t *)payload;
  uint8_t type, on;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
#if ZLO_CLUSTER_ENHANCEMENTS == 1					  
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                   lightLevelControlClusterServerAttributes.options.value);
#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

  if (!wOnOff && !(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_MoveToLevel_t))
    {
      /* Check for options override */
      ZCL_MoveToLevelWithOptions_t *tmp = (ZCL_MoveToLevelWithOptions_t *)payload;
      executeIfOff = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
      if ((ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK & tmp->optionsMask) != 0U)
      {
        gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                         tmp->optionsOverride);
      }
#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightLevelControlClusterServerAttributes.options.value) != 0U)
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

  if (req->level > MAX_LIGHT_LEVEL)
  {
    req->level = MAX_LIGHT_LEVEL;
  }
  if (req->level < MIN_LIGHT_LEVEL)
  {
    req->level = MIN_LIGHT_LEVEL;
  }
  if (0xffffU != req->transitionTime && 0U != req->transitionTime)
  {
    moveToLevel(wOnOff, executeIfOff, req->level, req->transitionTime);
  }
  else
  {
    setLevel(wOnOff, executeIfOff, req->level);
    (void)PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);
  }

  (void)addressing;
  (void)payloadLength;
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Process Move command
******************************************************************************/
static ZCL_Status_t processMove(bool wOnOff, ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_Move_t *req = (ZCL_Move_t *)payload;
  uint16_t transitionTime = MIN_TRANSITION_TIME;
  uint8_t level = MIN_LIGHT_LEVEL;
  uint8_t currentLevel = 0;
  bool executeIfOff = false;
  uint8_t type, on;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                   lightLevelControlClusterServerAttributes.options.value);
#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

  if (!wOnOff && !(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_Move_t))
    {
      /* Check for options override */
      ZCL_MoveWithOptions_t *tmp = (ZCL_MoveWithOptions_t *)payload;
      executeIfOff = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
      if ((ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK & tmp->optionsMask) != 0U)
      {
        gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                         tmp->optionsOverride);
      }
#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightLevelControlClusterServerAttributes.options.value) != 0U)
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

  currentLevel = lightLevelControlClusterServerAttributes.currentLevel.value;

  if (ZLL_LEVEL_CONTROL_UP_DIRECTION == req->moveMode)
  {
    level = MAX_LIGHT_LEVEL;
	if((req->rate) != 0U)
    {
      transitionTime =(uint8_t)(((MAX_LIGHT_LEVEL - currentLevel) * 10U /* 1/10 sec */) / req->rate);
    }
  }
  else if (ZLL_LEVEL_CONTROL_DOWN_DIRECTION == req->moveMode)
  {
    level = MIN_LIGHT_LEVEL;
	if((req->rate) != 0U)
    {
      transitionTime = (uint8_t)(((currentLevel - MIN_LIGHT_LEVEL) * 10U /* 1/10 sec */) / req->rate);
    }
  }
  else
  {
       //add else for avoid misra rule 15.7
  }

  moveToLevel(wOnOff, executeIfOff, level, transitionTime);

  (void)addressing;
  (void)payloadLength;
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Process Step command
******************************************************************************/
static ZCL_Status_t processStep(bool wOnOff, ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_Step_t *req = (ZCL_Step_t *)payload;
  int16_t level;
  uint8_t type, on;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
  gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                   lightLevelControlClusterServerAttributes.options.value);
#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT

  if (!wOnOff && !(bool)status && !(bool)on)
  {
    if (payloadLength > sizeof(ZCL_Step_t))
    {
      /* Check for options override */
      ZCL_StepWithOptions_t *tmp = (ZCL_StepWithOptions_t *)payload;
      executeIfOff = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
      if ((ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK & tmp->optionsMask) != 0U)
      {
        gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                         tmp->optionsOverride);
      }
#endif // #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_EXTENDED_COLOR_LIGHT
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightLevelControlClusterServerAttributes.options.value) != 0U)
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
  level = (int16_t)lightLevelControlClusterServerAttributes.currentLevel.value;

  if (ZLL_LEVEL_CONTROL_UP_DIRECTION == req->stepMode)
  {
    level += (int16_t)(req->stepSize);

    if (level > (int16_t)MAX_LIGHT_LEVEL)
    {
      level = (int16_t)MAX_LIGHT_LEVEL;
    }
  }

  else if (ZLL_LEVEL_CONTROL_DOWN_DIRECTION == req->stepMode)
  {
    level -= (int16_t)req->stepSize;

    if (level < (int16_t)MIN_LIGHT_LEVEL)
    {
      level = (int16_t)MIN_LIGHT_LEVEL;
    }
  }
  else
  {
       //add else for avoid misra rule 15.7
  }

  if (0xffffU != req->transitionTime && 0U != req->transitionTime)
  {
    moveToLevel(wOnOff, executeIfOff, (uint8_t)level, req->transitionTime);
  }

  if (!wOnOff && !deviceIsOn())
  {
    return ZCL_SUCCESS_STATUS;
  }
  stopMoving();
  setLevel(wOnOff, executeIfOff, (uint8_t)level);
  (void)PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);

  (void)addressing;
  (void)payloadLength;
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Process Stop command
******************************************************************************/
static ZCL_Status_t processStop(bool wOnOff, ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  uint8_t type, on;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!wOnOff && !(bool)status && !(bool)on)
  {
    if (payloadLength > 0U)
    {
      /* Check for options override */
      ZCL_StopWithOptions_t *tmp = (ZCL_StopWithOptions_t *)payload;
      executeIfOff = (bool)((uint8_t)ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &(uint8_t)(tmp->optionsMask));
      executeIfOff = (bool)((uint8_t)executeIfOff & (uint8_t)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride));
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if ((ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          lightLevelControlClusterServerAttributes.options.value) != 0U)
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
  stopMoving();
  (void)PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);

  (void)wOnOff;
  (void)addressing;
  (void)payloadLength;
  (void)payload;
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Move To Level command
******************************************************************************/
static ZCL_Status_t moveToLevelInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_MoveToLevelWithOptions_t *req = (ZCL_MoveToLevelWithOptions_t *)payload;
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_LEVEL;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  if((req->optionsMask == 0U) && (req->optionsOverride ==0U))
  {
    return processMoveToLevel(false, addressing, (uint8_t)(payloadLength-sizeof(uint16_t)), payload);
  }
  else
  {
    return processMoveToLevel(false, addressing, payloadLength, payload);
  }
}
/**************************************************************************//**
\brief Callback on receive of Move command
******************************************************************************/
static ZCL_Status_t moveInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  return processMove(false, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Step command
******************************************************************************/
static ZCL_Status_t stepInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  return processStep(false, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Stop command
******************************************************************************/
static ZCL_Status_t stopInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STOP;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  return processStop(false, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Move To Level With On/Off command
******************************************************************************/
static ZCL_Status_t moveToLevelWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_LEVEL_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  onOffClusterSetGlobalSceneControl();
  return processMoveToLevel(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Move With On/Off command
******************************************************************************/
static ZCL_Status_t moveWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  return processMove(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Step With On/Off command
******************************************************************************/
static ZCL_Status_t stepWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  return processStep(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Stop With On/Off command
******************************************************************************/
static ZCL_Status_t stopWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STOP_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
#endif
  return processStop(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Gets current color level

\returns current coloe level
******************************************************************************/
uint8_t levelControlCurrentLevel(void)
{
  return lightLevelControlClusterServerAttributes.currentLevel.value;
}

//#else // // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT
//void levelControlClusterInit(void) {;}
//void levelControlDisplayLevel(void) {;}

#if ZLO_CLUSTER_ENHANCEMENTS == 1
/**************************************************************************//**
\brief Update the current level value to startup current level value.
******************************************************************************/
void lightUpdateStartupLevel(void)
{
  if (ZCL_LEVEL_CONTROL_STARTUP_LEVEL_PREVIOUS == lightLevelControlClusterServerAttributes.startUpCurrentLevel.value)
  {
    // Use the previous currentLevel value restored from the PDS
    return;
  }
  else if (ZCL_LEVEL_CONTROL_STARTUP_LEVEL_MINIMUM == lightLevelControlClusterServerAttributes.startUpCurrentLevel.value)
  {
    lightLevelControlClusterServerAttributes.currentLevel.value = ZCL_LEVEL_CONTROL_MINIMUM_LEVEL;
  }
  else
  {
    lightLevelControlClusterServerAttributes.currentLevel.value =
      lightLevelControlClusterServerAttributes.startUpCurrentLevel.value;
  }

  (void)PDS_Store(APP_LIGHT_LEVEL_CONTROL_MEM_ID);
}
#endif
/**************************************************************************//**
\brief Attribute event (writing/reading) callback.

\param[in] addressing - incoming request addressing information.
\param[in] attributeId - attribute identifier.
\param[in] event - attribute event (read/write).
******************************************************************************/
static void ZCL_LevelControlAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event)
{
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_ATTR_LEVEL_CONTROL;
  eventItem.eventData.zclAttributeData.addressing = addressing;
  eventItem.eventData.zclAttributeData.attributeId = attributeId;
  eventItem.eventData.zclAttributeData.event = event;
  APP_Zigbee_Handler(eventItem);
#endif

#if ZLO_CLUSTER_ENHANCEMENTS == 1  
  if (((ZCL_WRITE_ATTRIBUTE_EVENT == event) && \
      ((ZCL_LEVEL_CONTROL_CLUSTER_STARTUP_CURRENT_LEVEL_ATTRIBUTE_ID == attributeId) || \
       (ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID == attributeId))) || \
       (((ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT == event) || \
        (ZCL_CONFIGURE_DEFAULT_ATTRIBUTE_REPORTING_EVENT == event)) && \
       (ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID == attributeId))
     )
#else
  if ((ZCL_WRITE_ATTRIBUTE_EVENT == event) || \
       (ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID == attributeId) || \
       (((ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT == event) || \
        (ZCL_CONFIGURE_DEFAULT_ATTRIBUTE_REPORTING_EVENT == event)) && \
       (ZCL_LEVEL_CONTROL_CLUSTER_CURRENT_LEVEL_ATTRIBUTE_ID == attributeId))
     )
#endif
  {
    (void)PDS_Store(APP_LIGHT_LEVEL_CONTROL_MEM_ID);
  }
}
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT

// eof lightLevelControlCluster.c
