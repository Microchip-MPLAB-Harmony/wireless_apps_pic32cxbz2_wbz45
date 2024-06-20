/*******************************************************************************
  Light On/Off cluster Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightOnOffCluster.c

  Summary:
    This file contains the Light On/Off cluster interface.

  Description:
    This file contains the Light On/Off cluster interface.
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
#if defined(BOARD_MEGARF) || defined(BOARD_SAMR21)
#include <ledsExt.h>
#include <lcdExt.h>
#endif
#include <z3device/clusters/include/haClusters.h>
#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightScenesCluster.h>
#include <z3device/common/include/z3Device.h>
#include <pds/include/wlPdsMemIds.h>
#include <z3device/clusters/include/scenesCluster.h>
#include <z3device/light/include/lightOnOffCluster.h>
#include <z3device/light/include/light.h>
#include <zcl/include/zclCommandManager.h>
#include <app_zigbee/zigbee_console/console.h>
#include <app_zigbee/app_zigbee_handler.h>

/******************************************************************************
                    Definitions section
******************************************************************************/
#define UPDATE_TIMER_INTERVAL               100U // 1/10th of a second as per specification
#define FADE_TIMER_INTERVAL                 UPDATE_TIMER_INTERVAL
#define MAX_BRIGHTNESS_LEVEL                255
#define MIN_BRIGHTNESS_LEVEL                (0)
                    
/******************************************************************************
                    Types section
******************************************************************************/
typedef struct
{

  uint8_t        firstInterval;
  uint8_t        secondInterval;
  uint8_t        currentLevel;
  uint8_t        transitionTime;
  int16_t        delta;
  HAL_AppTimer_t timer;
} FadeMem_t;

/******************************************************************************
                    Prototypes section
******************************************************************************/
static ZCL_Status_t onInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t offInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t toggleInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t offWithEffectInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OffWithEffect_t *payload);
static ZCL_Status_t onWithRecallGlobalSceneInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t onWithTimedOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OnWithTimedOff_t *payload);

static void startDeviceUpdate(void);
static void stopDeviceUpdate(void);
static void displayStatus(void);
static void updateOnOffState(void);
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
static void fadeTimerFired(void);
static void processOffEffect(uint8_t effectId, uint8_t effectVariant);
#endif

static void ZCL_OnOffAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event);

/******************************************************************************
                    Global variables
******************************************************************************/
PROGMEM_DECLARE(ZCL_OnOffClusterCommands_t lightOnOffCommands) =
{
  ZCL_DEFINE_ONOFF_CLUSTER_COMMANDS(onInd, offInd, toggleInd, offWithEffectInd, onWithRecallGlobalSceneInd, onWithTimedOffInd)
};

ZCL_OnOffClusterServerAttributes_t lightOnOffClusterServerAttributes =
{
  ZCL_DEFINE_ONOFF_CLUSTER_SERVER_ATTRIBUTES(ONOFF_VAL_MIN_REPORT_PERIOD, ONOFF_VAL_MAX_REPORT_PERIOD)
};

/******************************************************************************
                    Local variables
******************************************************************************/
static HAL_AppTimer_t updateTimer;
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
static FadeMem_t fadeMem;
#endif

/******************************************************************************
                    Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initialize On/Off cluster.
******************************************************************************/
void lightOnOffClusterInit(void)
{
  ZCL_Cluster_t *cluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT, ONOFF_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);

  if (cluster != NULL)
  {
    cluster->ZCL_AttributeEventInd = ZCL_OnOffAttributeEventInd;
  }
  if (!PDS_IsAbleToRestore(APP_LIGHT_ONOFF_MEM_ID))
  {
    lightOnOffClusterServerAttributes.onOff.value = false;
    lightOnOffClusterServerAttributes.globalSceneControl.value = true;
    lightOnOffClusterServerAttributes.onTime.value = 0;
    lightOnOffClusterServerAttributes.offWaitTime.value = 0;
    lightOnOffClusterServerAttributes.clusterVersion.value = ONOFF_CLUSTER_VERSION;
#if ZLO_CLUSTER_ENHANCEMENTS == 1
    lightOnOffClusterServerAttributes.startUpOnOff.value = ZCL_ON_OFF_CLUSTER_STARTUP_PREVIOUS;
#endif
  }
  else
  {
    (void)PDS_Restore(APP_LIGHT_ONOFF_MEM_ID);
  }

  updateTimer.mode = TIMER_REPEAT_MODE;
  updateTimer.interval = UPDATE_TIMER_INTERVAL;
  updateTimer.callback = updateOnOffState;

#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  fadeMem.timer.mode     = TIMER_REPEAT_MODE;
  fadeMem.timer.interval = FADE_TIMER_INTERVAL;
  fadeMem.timer.callback = fadeTimerFired;
#endif
#if ZLO_CLUSTER_ENHANCEMENTS == 1
  lightUpdateStartUpOnOffState();
#endif
  displayStatus();
}

/**************************************************************************//**
\brief Set Extension Field Set
******************************************************************************/
void onOffClusterSetExtensionField(bool onOff)
{
  if (lightOnOffClusterServerAttributes.onOff.value != onOff)
  {
    if (!onOff)
    {
      lightOnOffClusterServerAttributes.onTime.value = 0;
    }
    else
    {
      lightOnOffClusterServerAttributes.offWaitTime.value = 0;
      stopDeviceUpdate();
    }
    lightOnOffClusterServerAttributes.onOff.value = onOff;
    displayStatus();
#ifdef _ZCL_REPORTING_SUPPORT_
    (void)ZCL_ReportOnChangeIfNeeded(&lightOnOffClusterServerAttributes.onOff);
#endif
    (void)PDS_Store(APP_LIGHT_ONOFF_MEM_ID);
  }
}

/**************************************************************************//**
\brief Set Global Scene Control attribute to 'true'
******************************************************************************/
void onOffClusterSetGlobalSceneControl(void)
{
  lightOnOffClusterServerAttributes.globalSceneControl.value = true;
}

/**************************************************************************//**
\brief Checks, if device is in On state

\return True, if device is in On state; false - otherwise.
******************************************************************************/
bool deviceIsOn(void)
{
  return lightOnOffClusterServerAttributes.onOff.value;
}

/**************************************************************************//**
\brief Start device update procedure
******************************************************************************/
static void startDeviceUpdate(void)
{
  (void)HAL_StopAppTimer(&updateTimer);
  (void)HAL_StartAppTimer(&updateTimer);
}

/**************************************************************************//**
\brief Stop device update procedure
******************************************************************************/
static void stopDeviceUpdate(void)
{
  (void)HAL_StopAppTimer(&updateTimer);
}

/**************************************************************************//**
\brief Set Extension Field Set
******************************************************************************/
void setOnOff(bool onOff)
{
  lightOnOffClusterServerAttributes.onOff.value = onOff;
  lightScenesInvalidate();
  displayStatus();

  if (onOff)
  {
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
    LEDS_SET_BRIGHTNESS(lightLevelControlClusterServerAttributes.currentLevel.value);
#if APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_DIMMABLE_LIGHT
    LEDS_SET_COLOR_HS(DIMMABLE_LIGHT_HUE_LEVEL,DIMMABLE_LIGHT_SATURATION_LEVEL);
#endif
#else
    LEDS_SET_BRIGHTNESS(MAX_BRIGHTNESS_LEVEL);
#endif
  }
  else
  {
    LEDS_SET_BRIGHTNESS(MIN_BRIGHTNESS_LEVEL);
  }
#ifdef _ZCL_REPORTING_SUPPORT_
  (void)ZCL_ReportOnChangeIfNeeded(&lightOnOffClusterServerAttributes.onOff);
#endif
  (void)PDS_Store(APP_LIGHT_ONOFF_MEM_ID);
}

/**************************************************************************//**
\brief Display current On/Off status.
******************************************************************************/
static void displayStatus(void)
{
  LCD_PRINT(0, 0, lightOnOffClusterServerAttributes.onOff.value ? "I" : "O");
}

/**************************************************************************//**
\brief On/Off cluster update timer callback
******************************************************************************/
static void updateOnOffState(void)
{
  if (lightOnOffClusterServerAttributes.onOff.value)
  {
    if ((lightOnOffClusterServerAttributes.onTime.value) != 0U)
    {
      lightOnOffClusterServerAttributes.onTime.value--;
    }
    else
    {
      lightOnOffClusterServerAttributes.offWaitTime.value = 0;
      setOnOff(false);
    }
  }
  else
  {
    if ((lightOnOffClusterServerAttributes.offWaitTime.value) != 0U)
    {
      lightOnOffClusterServerAttributes.offWaitTime.value--;
    }
    else
    {
      stopDeviceUpdate();
    }
  }
}

#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
/**************************************************************************//**
\brief Fade timer has fired
******************************************************************************/
static void fadeTimerFired(void)
{
  if ((fadeMem.firstInterval) == 0U) //Refactoring this logic for misra rule 17.2 avoid nested recursion
  {
    if((fadeMem.secondInterval) != 0U)
      {
        fadeMem.firstInterval  = fadeMem.secondInterval;
        fadeMem.transitionTime = fadeMem.secondInterval;
        fadeMem.secondInterval = 0;
        fadeMem.currentLevel   = MIN((uint16_t)fadeMem.currentLevel + fadeMem.delta, UINT8_MAX);
        fadeMem.delta          = -fadeMem.currentLevel;
      }
  }
  if ((fadeMem.firstInterval) != 0U) 
  {
    fadeMem.firstInterval--;

    int32_t remainingTime = fadeMem.firstInterval;
    int16_t targetLevel = MIN((uint16_t)fadeMem.currentLevel + fadeMem.delta, UINT8_MAX);
    int16_t newLevel    = targetLevel - (fadeMem.delta * remainingTime) / (int32_t)fadeMem.transitionTime;

    LEDS_SET_BRIGHTNESS((uint8_t)newLevel);
    (void)newLevel;
  }
  else
  {
    (void)HAL_StopAppTimer(&fadeMem.timer);
  }
}

/**************************************************************************//**
\brief Processes efefect of OffWithEffect command

\param[in] effectId      - effect identifier;
\param[in] effectVariant - effect variant
******************************************************************************/
static void processOffEffect(uint8_t effectId, uint8_t effectVariant)
{
  fadeMem.currentLevel = lightLevelControlClusterServerAttributes.currentLevel.value;

  if (0U == effectId)
  {
    if (0U == effectVariant)
    {
      fadeMem.firstInterval  = 8;
      fadeMem.secondInterval = 0;
      fadeMem.delta          = (-(int16_t)(fadeMem.currentLevel));
    }
    if (1U == effectVariant)
    {
      LEDS_SET_BRIGHTNESS(0);
      return;
    }
    if (2U == effectVariant)
    {
      fadeMem.firstInterval  = 8;
      fadeMem.secondInterval = 120;
      fadeMem.delta          = ((-(int16_t)(fadeMem.currentLevel)) / 2);
    }
  }
  if (1U == effectId)
  {
    if (0U == effectVariant)
    {
      fadeMem.firstInterval  = 5;
      fadeMem.secondInterval = 10;
      fadeMem.delta          = (int16_t)((uint8_t)((fadeMem.currentLevel * 2U) / 10U));
    }
  }
  if ((uint8_t)UINT8_MAX < (uint8_t)fadeMem.delta + fadeMem.currentLevel)
  {
    fadeMem.delta = ((int16_t)UINT8_MAX - (int16_t)fadeMem.currentLevel);
  }
  fadeMem.transitionTime = fadeMem.firstInterval;
  (void)HAL_StopAppTimer(&fadeMem.timer);
  (void)HAL_StartAppTimer(&fadeMem.timer);
}
#endif

/**************************************************************************//**
\brief Callback on receive of On command
******************************************************************************/
static ZCL_Status_t onInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ON;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  if (0U == lightOnOffClusterServerAttributes.onTime.value)
  {
    lightOnOffClusterServerAttributes.offWaitTime.value = 0;
    stopDeviceUpdate();
  }
  onOffClusterSetGlobalSceneControl();
  setOnOff(true);
  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Off command
******************************************************************************/
static ZCL_Status_t offInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_OFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  lightOnOffClusterServerAttributes.onTime.value = 0;
  setOnOff(false);
  lightOnOffClusterServerAttributes.onTime.value = 0;

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Toggle command
******************************************************************************/
static ZCL_Status_t toggleInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_TOGGLE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  if (false == lightOnOffClusterServerAttributes.onOff.value)
  {
    if (0U == lightOnOffClusterServerAttributes.onTime.value)
    {
      lightOnOffClusterServerAttributes.offWaitTime.value = 0;
    }

    setOnOff(true);
    onOffClusterSetGlobalSceneControl();
  }
  else
  {
    lightOnOffClusterServerAttributes.onTime.value = 0;
    setOnOff(false);
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Off With Effect command
******************************************************************************/
static ZCL_Status_t offWithEffectInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OffWithEffect_t *payload)
{
  uint8_t previousOnOffState = (uint8_t)(lightOnOffClusterServerAttributes.onOff.value);
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_OFF_WITH_EFFECT;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;

  if (lightOnOffClusterServerAttributes.globalSceneControl.value)
  {
    lightOnOffClusterServerAttributes.globalSceneControl.value = false;
    storeGlobalScene(APP_ENDPOINT_LIGHT, lightSceneTable, &lightScenesClusterServerAttributes);
  }
#if APP_Z3_DEVICE_TYPE > APP_DEVICE_TYPE_ON_OFF_LIGHT
  lightOnOffClusterServerAttributes.onTime.value = 0;
  lightOnOffClusterServerAttributes.onOff.value = false;
  lightScenesInvalidate();
  (void)PDS_Store(Z3DEVICE_APP_MEMORY_MEM_ID);
  if (previousOnOffState != 0U)
  {/* Device was ON when receiving this command */
    displayStatus();
    processOffEffect(payload->effectIdentifier, payload->effectVariant);
  }
#ifdef _ZCL_REPORTING_SUPPORT_
  (void)ZCL_ReportOnChangeIfNeeded(&lightOnOffClusterServerAttributes.onOff);
#endif
#else
  setOnOff(false);
  (void)payload;
  (void)previousOnOffState;
#endif

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of On With Recall Global Scene command
******************************************************************************/
static ZCL_Status_t onWithRecallGlobalSceneInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ON_WITH_RECALL_GLOBALSCENE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;
  setOnOff(true);
  lightOnOffClusterServerAttributes.onTime.value = 0;
  lightOnOffClusterServerAttributes.offWaitTime.value = 0;
  stopDeviceUpdate();
  if (false == lightOnOffClusterServerAttributes.globalSceneControl.value)
  {
    recallGlobalScene(APP_ENDPOINT_LIGHT, lightSceneTable, &lightScenesClusterServerAttributes);
    lightOnOffClusterServerAttributes.globalSceneControl.value = true;
    if (0U == lightOnOffClusterServerAttributes.onTime.value)
    {
      lightOnOffClusterServerAttributes.offWaitTime.value = 0;
    }
  }
  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Toggle command
******************************************************************************/
static ZCL_Status_t onWithTimedOffInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OnWithTimedOff_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_ON_WITH_TIMED_OFF;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t *)payload;
  APP_Zigbee_Handler(event);

  if ((payload->onOffControl & ZCL_ON_OFF_CLUSTER_ACCEPT_ONLY_WHEN_ON) != 0U)
  {
    if (false == lightOnOffClusterServerAttributes.onOff.value)
    {
      return ZCL_SUCCESS_STATUS;
    }
  }

  if (lightOnOffClusterServerAttributes.offWaitTime.value > 0U &&
      false == lightOnOffClusterServerAttributes.onOff.value)
  {
    lightOnOffClusterServerAttributes.offWaitTime.value = MIN(payload->offWaitTime,
        lightOnOffClusterServerAttributes.offWaitTime.value);
  }
  else
  {
    lightOnOffClusterServerAttributes.onTime.value = MAX(payload->onTime,
        lightOnOffClusterServerAttributes.onTime.value);

    lightOnOffClusterServerAttributes.offWaitTime.value = payload->offWaitTime;

    setOnOff(true);
  }

  if (lightOnOffClusterServerAttributes.onTime.value < 0xffffU &&
      lightOnOffClusterServerAttributes.offWaitTime.value < 0xffffU)
  {
    startDeviceUpdate();
  }

  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief gets on/off state

\return on/off state
******************************************************************************/
bool onOffState(void)
{
  return lightOnOffClusterServerAttributes.onOff.value;
}

#if ZLO_CLUSTER_ENHANCEMENTS == 1
/**************************************************************************//**
\brief Update device ON state as per StartUpOnOff attribute value
******************************************************************************/
void lightUpdateStartUpOnOffState(void)
{
  switch (lightOnOffClusterServerAttributes.startUpOnOff.value)
  {
    case ZCL_ON_OFF_CLUSTER_STARTUP_OFF:
      lightOnOffClusterServerAttributes.onOff.value = false;
      break;

    case ZCL_ON_OFF_CLUSTER_STARTUP_ON:
      lightOnOffClusterServerAttributes.onOff.value = true;
      break;

    case ZCL_ON_OFF_CLUSTER_STARTUP_TOGGLE:
      lightOnOffClusterServerAttributes.onOff.value = !lightOnOffClusterServerAttributes.onOff.value;
      break;

    case ZCL_ON_OFF_CLUSTER_STARTUP_PREVIOUS:
      ; // lightOnOffClusterServerAttributes.onOff.value restored from PDS is used
      break;

    default:
      break;
  }

  (void)PDS_Store(APP_LIGHT_ONOFF_MEM_ID);
}
#endif

/**************************************************************************//**
\brief Attribute event (writing/reading) callback.

\param[in] addressing - incoming request addressing information.
\param[in] attributeId - attribute identifier.
\param[in] event - attribute event (read/write).
******************************************************************************/
static void ZCL_OnOffAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event)
{
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_ATTR_ONOFF;
  eventItem.eventData.zclAttributeData.addressing = addressing;
  eventItem.eventData.zclAttributeData.attributeId = attributeId;
  eventItem.eventData.zclAttributeData.event = event;
  APP_Zigbee_Handler(eventItem);

#if ZLO_CLUSTER_ENHANCEMENTS == 1
  if (((ZCL_WRITE_ATTRIBUTE_EVENT == event) && \
      ((ZCL_ONOFF_CLUSTER_STARTUP_ONOFF_SERVER_ATTRIBUTE_ID == attributeId) || \
       (ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID == attributeId))) || \
       (((ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT == event) || \
       (ZCL_CONFIGURE_DEFAULT_ATTRIBUTE_REPORTING_EVENT == event)) && \
       (ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID == attributeId))
      )
#else
  if (((ZCL_WRITE_ATTRIBUTE_EVENT == event) && \
       (ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID == attributeId)) || \
       (((ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT == event) || \
       (ZCL_CONFIGURE_DEFAULT_ATTRIBUTE_REPORTING_EVENT == event)) && \
       (ZCL_ONOFF_CLUSTER_ONOFF_SERVER_ATTRIBUTE_ID == attributeId))
      )	  
#endif	  
  {
    (void)PDS_Store(APP_LIGHT_ONOFF_MEM_ID);
  }
}


#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT

// eof lightOnOffCluster.c
