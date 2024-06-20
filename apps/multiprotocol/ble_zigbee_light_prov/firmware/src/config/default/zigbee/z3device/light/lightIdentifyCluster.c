/*******************************************************************************
  Light Identify cluster Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightIdentifyCluster.c

  Summary:
    This file contains the Light Identify cluster interface.

  Description:
    This file contains the Light Identify cluster interface.
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

#include <z3device/light/include/lightIdentifyCluster.h>
#include <zcl/clusters/include/identifyCluster.h>

#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightColorControlCluster.h>

#include <z3device/common/include/z3Device.h>
#include <zcl/include/zclCommandManager.h>
#include <pds/include/wlPdsMemIds.h>
#include <app_zigbee/zigbee_console/console.h>
#include <app_zigbee/app_zigbee_handler.h>
//#include <leds.h>

/******************************************************************************
                    Definition(s) section
******************************************************************************/
#define IDENTIFY_EFFECT_TIMER_PERIOD  500U // in milliseconds
#define DEFAULT_IDENTIFY_TIME         3U

#define BLINK_IDENTIFY_TIME          1U
#define BREATHE_IDENTIFY_TIME        15U
#define OKAY_COLOR_IDENTIFY_TIME     1U
#define OKAY_NONCOLOR_IDENTIFY_TIME  2U
#define CHANNEL_CHANGE_IDENTIFY_TIME 8U

#define ENHANCED_HUE_GREEN           30000U
#define ENHANCED_HUE_ORANGE          5000U
#define LED_NO_BRIGHTNESS            0U

/******************************************************************************
                    Prototypes section
******************************************************************************/

// Commands received by server
static ZCL_Status_t identifyInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_Identify_t *payload);
static ZCL_Status_t identifyQueryInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload);
static ZCL_Status_t triggerEffectInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_TriggerEffect_t *payload);
static ZCL_Status_t identifyQueryResponseInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_IdentifyQueryResponse_t *payload);

// Indication to notify change in Identify cluster attributes
static void ZCL_IdentifyAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event);

// Timer related function
static void effectTimerFired(void);
static void (*identifycb)(void);

// Support functions
static void lightIdentifyFinish(void);

/******************************************************************************
                    Global variables section
******************************************************************************/
ZCL_IdentifyClusterServerAttributes_t lightIdentifyClusterServerAttributes =
{
  ZCL_DEFINE_IDENTIFY_CLUSTER_SERVER_ATTRIBUTES()
};

ZCL_IdentifyClusterClientAttributes_t lightIdentifyClusterClientAttributes =
{
  ZCL_DEFINE_IDENTIFY_CLUSTER_CLIENT_ATTRIBUTES()
};

PROGMEM_DECLARE(ZCL_IdentifyClusterCommands_t   lightIdentifyCommands) =
{
  ZCL_IDENTIFY_CLUSTER_COMMANDS(identifyInd, identifyQueryInd, triggerEffectInd, identifyQueryResponseInd)
};


/******************************************************************************
                    Static variables section
******************************************************************************/
static HAL_AppTimer_t effectTimer;

static struct
{
  bool period      : 1;
  bool finish      : 1;
  bool colorEffect : 1;
} identificationStatus;


/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Identify cluster
******************************************************************************/
void lightIdentifyClusterInit(void)
{
  ZCL_Cluster_t *cluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT, IDENTIFY_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
  ZCL_Cluster_t *identifyCluster = ZCL_GetCluster(APP_ENDPOINT_LIGHT, IDENTIFY_CLUSTER_ID, ZCL_CLUSTER_SIDE_CLIENT);
  
  if (cluster != NULL)
  {
    cluster->ZCL_AttributeEventInd = ZCL_IdentifyAttributeEventInd;
  }

  lightIdentifyStop();

  // Reset attributes
  lightIdentifyClusterServerAttributes.identifyTime.value = 0;
  lightIdentifyClusterServerAttributes.clusterVersion.value = IDENTIFY_CLUSTER_VERSION;
  lightIdentifyClusterClientAttributes.clusterVersion.value = IDENTIFY_CLUSTER_VERSION;
  if (identifyCluster != NULL)
  {
    identifyCluster->ZCL_DefaultRespInd = ZCL_CommandZclDefaultResp;
  }
}

/**************************************************************************//**
\brief Shows identification effect in way specified.

\param[in] identifyTime - identifying period in seconds.
\param[in] colorEffect - flag to set for color effects.
\param[in] enhancedHue - enhanced hue to move for color effects.
******************************************************************************/
void lightIdentifyStart(uint16_t identifyTime, bool colorEffect, uint16_t enhancedHue)
{
  (void)HAL_StopAppTimer(&effectTimer);
  identificationStatus.finish = false;
  identificationStatus.colorEffect = colorEffect;

  if (colorEffect)
  {
    /**CHANGE - identifyTime Shall not be updated for triggerEffcet - need to work on*/
    lightIdentifyClusterServerAttributes.identifyTime.value = (uint16_t)(identifyTime * 2U);
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
    colorControlShowIdentifyEffect(enhancedHue);
#else
    (void)enhancedHue;
#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
  }
  else
  {
    lightIdentifyClusterServerAttributes.identifyTime.value = identifyTime;
  }

  LCD_FILL(true);
  
  if (!(bool)identificationStatus.colorEffect)
  {
  LEDS_SET_BRIGHTNESS(0);
  }
  effectTimer.mode = TIMER_REPEAT_MODE;
  effectTimer.interval = IDENTIFY_EFFECT_TIMER_PERIOD;
  effectTimer.callback = effectTimerFired;

  (void)HAL_StartAppTimer(&effectTimer);
}

/**************************************************************************//**
\brief Finish identification routine.
******************************************************************************/
static void lightIdentifyFinish(void)
{
  identificationStatus.finish = true;
}

/**************************************************************************//**
\brief Stop identification routine.
******************************************************************************/
void lightIdentifyStop(void)
{
#if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
  if (identificationStatus.colorEffect)
  {
    colorControlStopIdentifyEffect();
  }
#elif APP_Z3_DEVICE_TYPE <= APP_DEVICE_TYPE_DIMMABLE_LIGHT
    uint8_t level;
    level=levelControlCurrentLevel();
    LEDS_SET_BRIGHTNESS(level);
    (void)level;
#endif 
  identificationStatus.period = 0;
  (void)HAL_StopAppTimer(&effectTimer);
  lightIdentifyClusterServerAttributes.identifyTime.value = 0;

  if (identifycb != NULL)
  {
    identifycb();
  }
  LCD_FILL(false);
  LEDS_SET_BRIGHTNESS(LED_NO_BRIGHTNESS);
}

/**************************************************************************//**
\brief Effect timer callback
******************************************************************************/
static void effectTimerFired(void)
{
  uint8_t level = 0;
  if (identificationStatus.colorEffect)
  {
    level = MAX_LIGHT_LEVEL;
    lightIdentifyClusterServerAttributes.identifyTime.value--;
#if APP_Z3_DEVICE_TYPE <= APP_DEVICE_TYPE_DIMMABLE_LIGHT
     if(lightIdentifyClusterServerAttributes.identifyTime.value <= 14)
        level = MIN_LIGHT_LEVEL;  
#endif // APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_DIMMABLE_LIGHT
  }
  else
  {    
    identificationStatus.period = (!identificationStatus.period);
    LCD_FILL(identificationStatus.period);

    if (identificationStatus.period)
    {
      level = MAX_LIGHT_LEVEL;
      lightIdentifyClusterServerAttributes.identifyTime.value--;
    }
  }
  LEDS_SET_BRIGHTNESS(level);
#if APP_Z3_DEVICE_TYPE == APP_DEVICE_TYPE_DIMMABLE_LIGHT
  LEDS_SET_COLOR_HS(DIMMABLE_LIGHT_HUE_LEVEL,DIMMABLE_LIGHT_SATURATION_LEVEL);
#endif
  
  (void)level;
  
  if ((0U == lightIdentifyClusterServerAttributes.identifyTime.value) || (bool)(identificationStatus.finish))
  {
    lightIdentifyStop();
  }
}

/**************************************************************************//**
\brief Checks if identification process is in progress

\returns true if it is, false otherwise
******************************************************************************/
bool lightIdentifyIsIdentifying(void)
{
  return lightIdentifyClusterServerAttributes.identifyTime.value > 0U;
}

/**************************************************************************//**
\brief Callback on receiving Identify Query Response command

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer

\return status of indication routine
******************************************************************************/
static ZCL_Status_t identifyQueryResponseInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_IdentifyQueryResponse_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_IDENTIFY_QUERY_RESPONSE;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;

  //appSnprintf("->IdentifyQueryResponse, addr = 0x%04x, timeout = 0x%04x\r\n", addressing->addr.shortAddress, payload->timeout);

  RAISE_CALLBACKS_TO_IDENTIFY_SUBSCIBERS((identifySubscribers), identifyQueryResponse);

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receiving Identify command

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer

\return status of indication routine
******************************************************************************/
static ZCL_Status_t identifyInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_Identify_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_IDENTIFY;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;

  lightIdentifyClusterServerAttributes.identifyTime.value = payload->identifyTime;

  if ((payload->identifyTime) != 0U)
  {
    lightIdentifyStart(payload->identifyTime, IDENTIFY_NON_COLOR_EFFECT, 0);
  }
  else
  {
    lightIdentifyStop();
  }
  RAISE_CALLBACKS_TO_IDENTIFY_SUBSCIBERS((identifySubscribers), identify);

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}
/**************************************************************************//**
\brief Callback on receiving Identify Query command

\param[in] addressing - pointer to addressing information;
\param[in] payloadLength - data payload length;
\param[in] payload - data pointer

\return status of indication routine
******************************************************************************/
static ZCL_Status_t identifyQueryInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, uint8_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_IDENTIFY_QUERY;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;

  RAISE_CALLBACKS_TO_IDENTIFY_SUBSCIBERS((identifySubscribers), identifyQuery);
  APP_Zigbee_Handler(event);

  if ((lightIdentifyClusterServerAttributes.identifyTime.value) != 0U)
  {
    return sendIdentifyQueryResponse(addressing, APP_ENDPOINT_LIGHT, lightIdentifyClusterServerAttributes.identifyTime.value);
  }

  return ZCL_SUCCESS_WITH_DEFAULT_RESPONSE_STATUS;
}
/**************************************************************************//**
\brief Callback on receive of Trigger Effect command
******************************************************************************/
static ZCL_Status_t triggerEffectInd(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_TriggerEffect_t *payload)
{
  APP_Zigbee_Event_t event;
  event.eventGroup = EVENT_CLUSTER;
  event.eventId = CMD_ZCL_TRIGGER_EFFECT;
  event.eventData.zclEventData.addressing = addressing;
  event.eventData.zclEventData.payloadLength = payloadLength;
  event.eventData.zclEventData.payload = (uint8_t*)payload;

  switch (payload->effectIdentifier)
  {
    case ZCL_EFFECT_IDENTIFIER_BLINK:
      lightIdentifyStart(BLINK_IDENTIFY_TIME, IDENTIFY_NON_COLOR_EFFECT, 0);
      break;

    case ZCL_EFFECT_IDENTIFIER_BREATHE:
      lightIdentifyStart(BREATHE_IDENTIFY_TIME, IDENTIFY_NON_COLOR_EFFECT, 0);
      break;

    case ZCL_EFFECT_IDENTIFIER_OKAY:
      #if APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_COLOR_LIGHT
      lightIdentifyStart(OKAY_COLOR_IDENTIFY_TIME, IDENTIFY_COLOR_EFFECT, ENHANCED_HUE_GREEN);
      #else
      lightIdentifyStart(OKAY_NONCOLOR_IDENTIFY_TIME, IDENTIFY_NON_COLOR_EFFECT, 0);
      #endif
      break;

    case ZCL_EFFECT_IDENTIFIER_CHANNEL_CHANGE:
      lightIdentifyStart(CHANNEL_CHANGE_IDENTIFY_TIME, IDENTIFY_COLOR_EFFECT, ENHANCED_HUE_ORANGE);
      break;

    case ZCL_EFFECT_IDENTIFIER_FINISH_EFFECT:
      lightIdentifyFinish();
      break;

    case ZCL_EFFECT_IDENTIFIER_STOP_EFFECT:
      lightIdentifyStop();
      break;
  
    default:
      /* TODO: default case. */
      break;
  }

  APP_Zigbee_Handler(event);
  return ZCL_SUCCESS_STATUS;
}

/**************************************************************************//**
\brief Attribute event (writing/reading) callback.

\param[in] addressing - incoming request addressing information.
\param[in] attributeId - attribute identifier.
\param[in] event - attribute event (read/write).
******************************************************************************/
static void ZCL_IdentifyAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event)
{
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_ATTR_IDENTIFY;
  eventItem.eventData.zclAttributeData.addressing = addressing;
  eventItem.eventData.zclAttributeData.attributeId = attributeId;
  eventItem.eventData.zclAttributeData.event = event;

  if ((ZCL_WRITE_ATTRIBUTE_EVENT == event) && (ZCL_IDENTIFY_CLUSTER_IDENTIFY_TIME_ATTRIBUTE_ID == attributeId))
  {
    if ((lightIdentifyClusterServerAttributes.identifyTime.value) != 0U)
    {
      lightIdentifyStart(lightIdentifyClusterServerAttributes.identifyTime.value, IDENTIFY_NON_COLOR_EFFECT, 0);
    }
    else
    {
      lightIdentifyStop();
    }

  APP_Zigbee_Handler(eventItem);
  }
 
  APP_Zigbee_Handler(eventItem); 
}

/**************************************************************************//**
\brief Makes device to start identify itself

\param[in] identifyTime - identifying time in seconds
******************************************************************************/
void lightIdetifyStartIdentifyingCb(uint16_t identifyTime, void (*cb)(void))
{
  lightIdentifyClusterServerAttributes.identifyTime.value = identifyTime;
  identifycb = cb;
  lightIdentifyStart(lightIdentifyClusterServerAttributes.identifyTime.value,
                IDENTIFY_NON_COLOR_EFFECT, 0);

  (void)*cb;
}

#endif // APP_Z3_DEVICE_TYPE >= APP_DEVICE_TYPE_ON_OFF_LIGHT

// eof lightIdentifyCluster.c
