/*******************************************************************************
  Custom Level Control Cluster Source file

  Company:
    Microchip Technology Inc.

  File Name:
    customLevelControlCluster.c

  Summary:
    This file contains Custom Level Control Cluster Implementation.

  Description:
    This file contains Custom Level Control Cluster Implementation.
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
                    Includes section
******************************************************************************/
#include <z3device/clusters/include/haClusters.h>
#if defined(BOARD_MEGARF) || defined(BOARD_SAMR21)
#include <ledsExt.h>
#include <lcdExt.h>
#endif
#include <zcl/include/zclCommandManager.h>
#include <z3device/custom/include/customOnOffCluster.h>
#include <z3device/custom/include/customScenesCluster.h>
#include <z3device/common/include/z3Device.h>
#include <z3device/custom/include/customLevelControlCluster.h>
#include <z3device/custom/include/customColorControlCluster.h>
#include <pds/include/wlPdsMemIds.h>
#include <z3device/custom/include/custom.h>
#include <z3device/clusters/include/onOffCluster.h>
#include <app_zigbee/zigbee_console/console.h>
#include <app_zigbee/app_zigbee_handler.h>

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
static void customLevelReportInd(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_LevelControlClusterServerAttributes_t customLevelControlClusterServerAttributes =
{
  ZCL_DEFINE_LEVEL_CONTROL_CLUSTER_SERVER_MANDATORY_ATTRIBUTES(0, 0)
};

PROGMEM_DECLARE(ZCL_LevelControlClusterCommands_t customLevelControlCommands) =
{
  ZCL_DEFINE_LEVEL_CONTROL_CLUSTER_COMMANDS(moveToLevelInd, moveInd, stepInd, stopInd,
      moveToLevelWOnOffInd, moveWOnOffInd, stepWOnOffInd, stopWOnOffInd)
};

ZCL_LevelControlClusterClientAttributes_t customLevelControlClusterClientAttributes =
{
  ZCL_DEFINE_LEVEL_CONTROL_CLUSTER_CLIENT_ATTRIBUTES()
};

//###########################################################################

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

/* Level change on startup does not affect the color temperature of custom
 because, that is a function of StartUpColorTemperature attribute */
static bool gUpdateCoupledColorTemp = false;

/******************************************************************************
                    Implementations
******************************************************************************/

/**************************************************************************//**
\brief Initialize Level Control cluster.
******************************************************************************/
void customLevelControlClusterInit(void)
{
  ZCL_Cluster_t *cluster = ZCL_GetCluster(APP_ENDPOINT_CUSTOM, LEVEL_CONTROL_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
  ZCL_Cluster_t *clusterClient = ZCL_GetCluster(APP_ENDPOINT_CUSTOM, LEVEL_CONTROL_CLUSTER_ID, ZCL_CLUSTER_SIDE_CLIENT);
#if (APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_CUSTOM)
  uint8_t level = 0;
  (void)level;
#endif
  if (cluster)
    cluster->ZCL_AttributeEventInd = ZCL_LevelControlAttributeEventInd;
	cluster->ZCL_DefaultRespInd = ZCL_CommandZclDefaultResp;

  if (!PDS_IsAbleToRestore(APP_CUSTOM_LEVEL_CONTROL_MEM_ID))
  {
    customLevelControlClusterServerAttributes.currentLevel.value = MAX_CUSTOM_LEVEL / 2;
    customLevelControlClusterServerAttributes.remainingTime.value = 0;
    customLevelControlClusterServerAttributes.clusterVersion.value = LEVEL_CONTROL_CLUSTER_VERSION;
    customLevelControlClusterServerAttributes.currentLevel.minReportInterval = LEVEL_CONTROL_VAL_MIN_REPORT_PERIOD;
    customLevelControlClusterServerAttributes.currentLevel.maxReportInterval = LEVEL_CONTROL_VAL_MAX_REPORT_PERIOD;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
    customLevelControlClusterServerAttributes.startUpCurrentLevel.value = ZCL_LEVEL_CONTROL_STARTUP_LEVEL_PREVIOUS;
    customLevelControlClusterServerAttributes.options.value = 0x00; /* Bit_0 - ExecuteIfOff; Bit_1 - CoupleColorTempToLevelMinMireds */
#endif
  }
  else
  {
      PDS_Restore(APP_CUSTOM_LEVEL_CONTROL_MEM_ID);
  }
#if ZLO_CLUSTER_ENHANCEMENTS == 1  
  customUpdateStartupLevel();
#endif
  /* To display the LED with the restore current level value afetr reset */
  if (deviceIsOn())
    level = customLevelControlClusterServerAttributes.currentLevel.value;
  LEDS_SET_BRIGHTNESS(level);
  customLevelControlClusterClientAttributes.clusterVersion.value = LEVEL_CONTROL_CLUSTER_VERSION;
  if (clusterClient)
    clusterClient->ZCL_ReportInd = customLevelReportInd;
  clusterClient->ZCL_DefaultRespInd = ZCL_CommandZclDefaultResp;
}

/**************************************************************************//**
\brief Set Extension Field Set
******************************************************************************/
void levelControlClusterSetExtensionField(uint16_t currentLevel, uint16_t transitionTime)
{
  stopMoving();
  if (0xffff != transitionTime && 0 != transitionTime) 
    moveToLevel(1, 0, currentLevel, transitionTime);
  else
    setLevel(false, false, currentLevel);    
}

/**************************************************************************//**
\brief Set Current Level attribute to a specified value
******************************************************************************/
void setLevel(bool wOnOff, bool execIfOff, uint8_t level)
{
  if (wOnOff)
    adjustOnOffState(level);

  customLevelControlClusterServerAttributes.currentLevel.value = level;
  customScenesInvalidate();

#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_DIMMABLE_LIGHT
  if (!execIfOff)
    levelControlDisplayLevel(customLevelControlClusterServerAttributes.currentLevel.value);
#endif

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  customUpdateCoupledColorTemperature((!execIfOff && deviceIsOn()), gUpdateCoupledColorTemp,
    customLevelControlClusterServerAttributes.currentLevel.value);
#endif

#ifdef _ZCL_REPORTING_SUPPORT_
  ZCL_ReportOnChangeIfNeeded(&customLevelControlClusterServerAttributes.currentLevel);
#endif
}

/**************************************************************************//**
\brief On/off value adjustment regarding to current level

\param[in] level - current level value
******************************************************************************/
static void adjustOnOffState(uint8_t level)
{
  bool turnOn = (level == MIN_CUSTOM_LEVEL) ? false : true;

  onOffClusterSetExtensionField(turnOn);

  if(level > MIN_CUSTOM_LEVEL)
  {
    appSnprintf("On\r\n");
  }
  else
  {
    appSnprintf("Off\r\n");
  }
}

/**************************************************************************//**
\brief Stat moving to the specified level
******************************************************************************/
static void moveToLevel(bool wOnOff, bool execIfOff, uint8_t newLevel, uint16_t transitionTime)
{
  int16_t level = newLevel;
  stopMoving();

  gTransitionTime = transitionTime;
  gTargetLevel = newLevel;
  gDelta = level - customLevelControlClusterServerAttributes.currentLevel.value;
  gWithOnOff = wOnOff;
  gExecuteIfOff = execIfOff;

  updateTimer.mode = TIMER_REPEAT_MODE;
  updateTimer.interval = UPDATE_TIMER_INTERVAL;
  updateTimer.callback = updateTransitionState;
  HAL_StopAppTimer(&updateTimer);
  HAL_StartAppTimer(&updateTimer);
  customLevelControlClusterServerAttributes.remainingTime.value = transitionTime;
}

/**************************************************************************//**
\brief Update transition state
******************************************************************************/
static void updateTransitionState(void)
{
  if (!deviceIsOn() && !gWithOnOff && !gExecuteIfOff)
  {/* If the LC command is WITH on/off, then stop transition when the custom is
    * turned off while the transition in progress. */
    customLevelControlClusterServerAttributes.remainingTime.value = 0;
    HAL_StopAppTimer(&updateTimer);
    return;
  }

  if (customLevelControlClusterServerAttributes.remainingTime.value > 0)
  {
    int32_t remainingTime = customLevelControlClusterServerAttributes.remainingTime.value;
    int16_t newLevel = (int16_t)gTargetLevel - ((remainingTime * gDelta) / (int32_t)gTransitionTime);
	customLevelControlClusterServerAttributes.remainingTime.value--;
    setLevel(gWithOnOff, gExecuteIfOff, (uint8_t)newLevel);
  }
  else
  {
    HAL_StopAppTimer(&updateTimer);
    setLevel(gWithOnOff, gExecuteIfOff, gTargetLevel);
    gWithOnOff = false;
    gExecuteIfOff = false;

    appSnprintf(" + Level transition is finished. Current level is %u\r\n", gTargetLevel);

    PDS_Store(APP_CUSTOM_LEVEL_CONTROL_MEM_ID);
  }
}

/**************************************************************************//**
\brief Stop moving
******************************************************************************/
static void stopMoving(void)
{
  HAL_StopAppTimer(&updateTimer);
  gWithOnOff = false;
  customLevelControlClusterServerAttributes.remainingTime.value = 0;
  if (!gExecuteIfOff)
  {
    levelControlDisplayLevel(customLevelControlClusterServerAttributes.currentLevel.value);
  }
}

/**************************************************************************//**
\brief Process Move To Level command
******************************************************************************/
static ZCL_Status_t processMoveToLevel(bool wOnOff, ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  ZCL_MoveToLevel_t *req = (ZCL_MoveToLevel_t *)payload;
  uint8_t type, on;
  bool executeIfOff = false;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                   customLevelControlClusterServerAttributes.options.value);
  if (!wOnOff && !status && !on)
  {
    if (payloadLength > sizeof(ZCL_MoveToLevel_t))
    {
      /* Check for options override */
      ZCL_MoveToLevelWithOptions_t *tmp = (ZCL_MoveToLevelWithOptions_t *)payload;
      executeIfOff = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
      if (ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK & tmp->optionsMask)
      {
        gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                         tmp->optionsOverride);
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customLevelControlClusterServerAttributes.options.value)
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
  if (req->level > MAX_CUSTOM_LEVEL)
    req->level = MAX_CUSTOM_LEVEL;

  if (req->level < MIN_CUSTOM_LEVEL)
    req->level = MIN_CUSTOM_LEVEL;

  if (0xffff != req->transitionTime && 0 != req->transitionTime)
  {
    moveToLevel(wOnOff, executeIfOff, req->level, req->transitionTime);
  }
  else
  {
    setLevel(wOnOff, executeIfOff, req->level);
    PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);
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
  uint8_t level = MIN_CUSTOM_LEVEL;
  uint8_t currentLevel = 0;
  bool executeIfOff = false;
  uint8_t type, on;
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                   customLevelControlClusterServerAttributes.options.value);

  if (!wOnOff && !status && !on)
  {
    if (payloadLength > sizeof(ZCL_Move_t))
    {
      /* Check for options override */
      ZCL_MoveWithOptions_t *tmp = (ZCL_MoveWithOptions_t *)payload;
      executeIfOff = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
      if (ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK & tmp->optionsMask)
      {
        gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                         tmp->optionsOverride);
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customLevelControlClusterServerAttributes.options.value)
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

  currentLevel = customLevelControlClusterServerAttributes.currentLevel.value;

  if (ZLL_LEVEL_CONTROL_UP_DIRECTION == req->moveMode)
  {
    level = MAX_CUSTOM_LEVEL;
    transitionTime = ((MAX_CUSTOM_LEVEL - currentLevel) * 10 /* 1/10 sec */) / req->rate;
  }
  else if (ZLL_LEVEL_CONTROL_DOWN_DIRECTION == req->moveMode)
  {
    level = MIN_CUSTOM_LEVEL;
    transitionTime = ((currentLevel - MIN_CUSTOM_LEVEL) * 10 /* 1/10 sec */) / req->rate;
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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
#if ZLO_CLUSTER_ENHANCEMENTS == 1						  
  gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                   customLevelControlClusterServerAttributes.options.value);

  if (!wOnOff && !status && !on)
  {
    if (payloadLength > sizeof(ZCL_Step_t))
    {
      /* Check for options override */
      ZCL_StepWithOptions_t *tmp = (ZCL_StepWithOptions_t *)payload;
      executeIfOff = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
      if (ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK & tmp->optionsMask)
      {
        gUpdateCoupledColorTemp = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_COUPLE_COLOR_TEMP_TO_LEVEL_MASK &
                                         tmp->optionsOverride);
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customLevelControlClusterServerAttributes.options.value)
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

  level = customLevelControlClusterServerAttributes.currentLevel.value;

  if (ZLL_LEVEL_CONTROL_UP_DIRECTION == req->stepMode)
  {
    level += req->stepSize;

    if (level > MAX_CUSTOM_LEVEL)
      level = MAX_CUSTOM_LEVEL;
  }

  else if (ZLL_LEVEL_CONTROL_DOWN_DIRECTION == req->stepMode)
  {
    level -= req->stepSize;

    if (level < MIN_CUSTOM_LEVEL)
      level = MIN_CUSTOM_LEVEL;
  }

  if (0xffff != req->transitionTime && 0 != req->transitionTime)
  {
    moveToLevel(wOnOff, executeIfOff, level, req->transitionTime);
  }

  if (!wOnOff && !deviceIsOn())
    return ZCL_SUCCESS_STATUS;

  stopMoving();
  setLevel(wOnOff, executeIfOff, level);
  PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);

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
  ZCL_Status_t status = ZCL_ReadAttributeValue(APP_ENDPOINT_CUSTOM, ONOFF_CLUSTER_ID,
                          ZCL_CLUSTER_SIDE_SERVER, ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID,
                          &type, &on);
  bool executeIfOff = false;
  
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (!wOnOff && !status && !on)
  {
    if (payloadLength > 0)
    {
      /* Check for options override */
      ZCL_StopWithOptions_t *tmp = (ZCL_StopWithOptions_t *)payload;
      executeIfOff = (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsMask);
      executeIfOff &= (bool)(ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK & tmp->optionsOverride);
      if (!executeIfOff)
      {
        return ZCL_SUCCESS_STATUS;
      }
    }
    else
    {
      /* Check for ExecuteIfOff bit */
      if (ZCL_LEVEL_CONTROL_OPTIONS_ATTR_EXECUTE_IF_OFF_MASK &
          customLevelControlClusterServerAttributes.options.value)
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
  PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);

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
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_LEVEL;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);

  return processMoveToLevel(false, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Move command
******************************************************************************/
static ZCL_Status_t moveInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);

  return processMove(false, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Step command
******************************************************************************/
static ZCL_Status_t stepInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);

  return processStep(false, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Stop command
******************************************************************************/
static ZCL_Status_t stopInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STOP;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);

  return processStop(false, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Move To Level With On/Off command
******************************************************************************/
static ZCL_Status_t moveToLevelWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_TO_LEVEL_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);
  onOffClusterSetGlobalSceneControl();
  return processMoveToLevel(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Move With On/Off command
******************************************************************************/
static ZCL_Status_t moveWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_MOVE_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);

  return processMove(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Step With On/Off command
******************************************************************************/
static ZCL_Status_t stepWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STEP_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);

  return processStep(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Callback on receive of Stop With On/Off command
******************************************************************************/
static ZCL_Status_t stopWOnOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_STOP_WITH_ONOFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  APP_Zigbee_Handler(event);

  return processStop(true, addressing, payloadLength, payload);
}
/**************************************************************************//**
\brief Gets current color level

\returns current coloe level
******************************************************************************/
uint8_t levelControlCurrentLevel(void)
{
  return customLevelControlClusterServerAttributes.currentLevel.value;
}

//void levelControlClusterInit(void) {;}
//void levelControlDisplayLevel(void) {;}


#if ZLO_CLUSTER_ENHANCEMENTS == 1
/**************************************************************************//**
\brief Update the current level value to startup current level value.
******************************************************************************/
void customUpdateStartupLevel(void)
{
  if (ZCL_LEVEL_CONTROL_STARTUP_LEVEL_PREVIOUS == customLevelControlClusterServerAttributes.startUpCurrentLevel.value)
  {
    // Use the previous currentLevel value restored from the PDS
    return;
  }
  else if (ZCL_LEVEL_CONTROL_STARTUP_LEVEL_MINIMUM == customLevelControlClusterServerAttributes.startUpCurrentLevel.value)
  {
    customLevelControlClusterServerAttributes.currentLevel.value = ZCL_LEVEL_CONTROL_MINIMUM_LEVEL;
  }
  else
  {
    customLevelControlClusterServerAttributes.currentLevel.value =
      customLevelControlClusterServerAttributes.startUpCurrentLevel.value;
  }

  PDS_Store(APP_CUSTOM_LEVEL_CONTROL_MEM_ID);
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
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_ATTR_LEVEL_CONTROL;
  eventItem.eventData.zclAttributeData.addressing = addressing;
  eventItem.eventData.zclAttributeData.attributeId = attributeId;
  eventItem.eventData.zclAttributeData.event = event;
  APP_Zigbee_Handler(eventItem);
  
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
    PDS_Store(APP_CUSTOM_LEVEL_CONTROL_MEM_ID);
  }

}

/**************************************************************************//**
\brief Report attribute indication handler

\param[in] addressing - pointer to addressing information;
\param[in] reportLength - data payload length;
\param[in] reportPayload - data pointer
******************************************************************************/
static void customLevelReportInd(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload)
{
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_REPORTING_LEVEL;
  eventItem.eventData.zclEventData.addressing = addressing;
  eventItem.eventData.zclEventData.payloadLength = reportLength;
  eventItem.eventData.zclEventData.payload = reportPayload;

  APP_Zigbee_Handler(eventItem);
}


/*********************************************************************************
*********************************************************************************/

#endif // APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_CUSTOM_DEVICE

// eof customLevelControlCluster.c
