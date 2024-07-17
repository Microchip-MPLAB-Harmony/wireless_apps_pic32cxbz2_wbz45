/*******************************************************************************
  Custom Occupancy Sensing Cluster Source file

  Company:
    Microchip Technology Inc.

  File Name:
    customOccupancySensingCluster.c

  Summary:
    This file contains Custom OccupancySensing Cluster Implementation.

  Description:
    This file contains Custom OccupancySensing Cluster Implementation.
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
#include <z3device/custom/include/customOccupancySensingCluster.h>
#include <z3device/custom/include/customFanControlCluster.h>
#include <zcl/include/zclCommandManager.h>
#include <app_zigbee/zigbee_console/console.h>
#include <pds/include/wlPdsMemIds.h>
#include <z3device/common/include/z3Device.h>
#include <app_zigbee/app_zigbee_handler.h>
/******************************************************************************
                             Defines section
******************************************************************************/
#define MOVEMENT_DETECTION_PERIOD                       2000UL
#define AMOUNT_MSEC_IN_SEC                              1000UL
#define NO_OF_MOVEMENT_DETECTION_EVENTS_ALLOWED         7U
/******************************************************************************
                    Global variables
******************************************************************************/
ZCL_OccupancySensingClusterServerAttributes_t customOccupancySensingClusterServerAttributes =
{
  ZCL_DEFINE_OCCUPANCY_SENSING_CLUSTER_SERVER_ATTRIBUTES(OCCUPANCY_SENSING_VAL_MIN_REPORT_PERIOD, OCCUPANCY_SENSING_VAL_MAX_REPORT_PERIOD)
};
ZCL_OccupancySensingClusterClientAttributes_t customOccupancySensingClusterClientAttributes =
{
  ZCL_DEFINE_OCCUPANCY_SENSING_CLUSTER_CLIENT_ATTRIBUTES()
};
/*******************************************************************************
                   Types section
*******************************************************************************/
typedef enum
{
  OCCUPANCY_CHANGE_STATE_IDLE,
  OCCUPANCY_CHANGE_STATE_OCCUPIED_TO_UNOCCUPIED_IN_PROGRESS,
  OCCUPANCY_CHANGE_STATE_UNOCCUPIED_TO_OCCUPIED_IN_PROGRESS,
} OccupancyChangeState_t;

/******************************************************************************
                    Static functions
******************************************************************************/
static void occupancySensingSetOccupancyState(void);
static void occupiedToUnoccupiedHandler(void);
static void unoccupiedToOccupiedHandler(void);
static void movementDetected(void);
static bool verifyOccupancySensorDetection(bool state);
static void customOccupancyAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event);
static void customOccupancySensorReportInd(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload);

/******************************************************************************
                    Static variables
******************************************************************************/
static HAL_AppTimer_t sensorAttributeUpdateTimer;
static uint8_t occupancyChangeState = OCCUPANCY_CHANGE_STATE_IDLE;
static uint64_t delayStartTime = 0;
static uint8_t eventCount = 0;
static uint8_t threshold;
static uint16_t delay;

static HAL_AppTimer_t movementEventTimer =
{
  .interval = MOVEMENT_DETECTION_PERIOD,
  .mode     = TIMER_REPEAT_MODE,
  .callback = movementDetected,
};

/******************************************************************************
                    Implementation section
******************************************************************************/
/**************************************************************************//**
\brief Initializes Occupancy Sensing cluster
******************************************************************************/
void customOccupancySensingClusterInit(void)
{
  ZCL_Cluster_t *cluster = ZCL_GetCluster(APP_ENDPOINT_CUSTOM, OCCUPANCY_SENSING_CLUSTER_ID, ZCL_CLUSTER_SIDE_CLIENT);

  if (cluster)
  {
    customOccupancySensingClusterClientAttributes.clusterVersion.value = OCCUPANCY_CLUSTER_VERSION;
    cluster->ZCL_ReportInd = customOccupancySensorReportInd;
  }
  cluster->ZCL_DefaultRespInd = ZCL_CommandZclDefaultResp;
  cluster = ZCL_GetCluster(APP_ENDPOINT_CUSTOM, OCCUPANCY_SENSING_CLUSTER_ID, ZCL_CLUSTER_SIDE_SERVER);
  if (cluster)
  {
    customOccupancySensingClusterServerAttributes.clusterVersion.value = OCCUPANCY_CLUSTER_VERSION;
    customOccupancySensingClusterServerAttributes.occupancySensorType.value = OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_PIR;
    customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value = OCCUPANYC_SENSOR_TYPE_BITMAP_ATTRIBUTE_VALUE_PIR;
    customOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED;

    ZCL_ReportOnChangeIfNeeded(&customOccupancySensingClusterServerAttributes.occupancy);
    customOccupancySensingClusterServerAttributes.PIROccupiedToUnoccupiedDelay.value = ZCL_OCCUPANCY_SENSING_CL_PIR_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.PIRUnoccupiedToOccupiedDelay.value = ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.PIRUnoccupiedToOccupiedThreshold.value = ZCL_OCCUPANCY_SENSING_CL_PIR_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.UltrasonicOccupiedToUnoccupiedDelay.value = ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.UltrasonicUnoccupiedToOccupiedDelay.value = ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.UltrasonicUnoccupiedToOccupiedThreshold.value = ZCL_OCCUPANCY_SENSING_CL_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.PhysicalContactOccupiedToUnoccupiedDelay.value = ZCL_OCCUPANCY_SENSING_CL_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.PhysicalContactUnoccupiedToOccupiedDelay.value = ZCL_OCCUPANCY_SENSING_CL_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY_SER_ATTR_DEFAULT_VAL;
    customOccupancySensingClusterServerAttributes.PhysicalContactUnoccupiedToOccupiedThreshold.value = ZCL_OCCUPANCY_SENSING_CL_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_SER_ATTR_DEFAULT_VAL;
    cluster->ZCL_AttributeEventInd = customOccupancyAttributeEventInd;
    
    if (PDS_IsAbleToRestore(APP_CUSTOM_OCCUPANCY_MEM_ID))
      PDS_Restore(APP_CUSTOM_OCCUPANCY_MEM_ID);
  }
}

/**************************************************************************//**
\brief Toggles occupancy
******************************************************************************/
void occupancySensingToggleOccupancy(void)
{
  if (OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED == customOccupancySensingClusterServerAttributes.occupancy.value)
    customOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_OCCUPIED;
  else
    customOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED;
  
  PDS_Store(APP_CUSTOM_OCCUPANCY_MEM_ID);
#ifdef _ZCL_REPORTING_SUPPORT_
  ZCL_ReportOnChangeIfNeeded(&customOccupancySensingClusterServerAttributes.occupancy);
#endif
}

/**************************************************************************//**
\brief Initiates occupancy to Occupied state or Unoccupied state or vice versa
******************************************************************************/
void occupancySensingInitiateSetOccupancyState(bool state)
{
  if (OCCUPANCY_CHANGE_STATE_IDLE == occupancyChangeState)
  {
    if (OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED == state)
      occupiedToUnoccupiedHandler();
    else
      unoccupiedToOccupiedHandler();
  }
  else
    /* Consider this as movement event detetced from sensor */
    verifyOccupancySensorDetection(state);
  
  PDS_Store(APP_CUSTOM_OCCUPANCY_MEM_ID);
}

/***************************************************************************//**
\brief checks the detected movement whether Occupied to Unoccupied or Unoccupied to Occupied
******************************************************************************/
static bool verifyOccupancySensorDetection(bool state)
{
  /* check the occupancy state detected */
  switch(occupancyChangeState)
  {
    case OCCUPANCY_CHANGE_STATE_OCCUPIED_TO_UNOCCUPIED_IN_PROGRESS:
      if (state)
      {
        HAL_StopAppTimer(&sensorAttributeUpdateTimer);
        occupancyChangeState = OCCUPANCY_CHANGE_STATE_IDLE;
      }
      break;
    case OCCUPANCY_CHANGE_STATE_UNOCCUPIED_TO_OCCUPIED_IN_PROGRESS:
      if (!state)
      {
        HAL_StopAppTimer(&movementEventTimer);
        HAL_StopAppTimer(&sensorAttributeUpdateTimer);
        occupancyChangeState = OCCUPANCY_CHANGE_STATE_IDLE;
      }
      else
        return true;
      break;
  }
  return false;
}

/**************************************************************************//**
\brief Handler for changing the occupancy state from occupied to unoccupied
******************************************************************************/
static void occupiedToUnoccupiedHandler(void)
{
  occupancyChangeState = OCCUPANCY_CHANGE_STATE_OCCUPIED_TO_UNOCCUPIED_IN_PROGRESS;

  if (OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_PIR == customOccupancySensingClusterServerAttributes.occupancySensorType.value ||
      OCCUPANYC_SENSOR_TYPE_BITMAP_ATTRIBUTE_VALUE_PIR == customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value)
  {
    if (!customOccupancySensingClusterServerAttributes.PIROccupiedToUnoccupiedDelay.value)
    {
      occupancySensingSetOccupancyState();
      return;
    }
    else
      sensorAttributeUpdateTimer.interval = customOccupancySensingClusterServerAttributes.PIROccupiedToUnoccupiedDelay.value * AMOUNT_MSEC_IN_SEC;//msec
  }
  else 
      if (OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_ULTRASONIC == customOccupancySensingClusterServerAttributes.occupancySensorType.value ||
           OCCUPANYC_SENSOR_TYPE_BITMAP_ATTRIBUTE_VALUE_ULTRASONIC == customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value)
  {
    if (!customOccupancySensingClusterServerAttributes.UltrasonicOccupiedToUnoccupiedDelay.value)
      {
        occupancySensingSetOccupancyState();
        return;
      }
      else
        sensorAttributeUpdateTimer.interval = customOccupancySensingClusterServerAttributes.UltrasonicOccupiedToUnoccupiedDelay.value * AMOUNT_MSEC_IN_SEC;//msec
  }
  else if (OCCUPANYC_SENSOR_TYPE_ATTRIBITE_VALUE_PHYSICAL_CONTACT == customOccupancySensingClusterServerAttributes.occupancySensorType.value ||
           OCCUPANYC_SENSOR_TYPE_BITMAP_ATTRIBUTE_VALUE_PHYSICAL_CONTACT_PIR == customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value)
  {
    if (!customOccupancySensingClusterServerAttributes.PhysicalContactOccupiedToUnoccupiedDelay.value)
      {
        occupancySensingSetOccupancyState();
        return;
      }
      else
        sensorAttributeUpdateTimer.interval = customOccupancySensingClusterServerAttributes.PhysicalContactOccupiedToUnoccupiedDelay.value * AMOUNT_MSEC_IN_SEC;//msec
  }
  sensorAttributeUpdateTimer.mode     = TIMER_ONE_SHOT_MODE,
  sensorAttributeUpdateTimer.callback = occupancySensingSetOccupancyState,
  HAL_StartAppTimer(&sensorAttributeUpdateTimer);
}

/**************************************************************************//**
\brief Handler for changing the occupancy state from unoccupied to occupied 
******************************************************************************/
static void unoccupiedToOccupiedHandler(void)
{
  occupancyChangeState = OCCUPANCY_CHANGE_STATE_UNOCCUPIED_TO_OCCUPIED_IN_PROGRESS;

  if (OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_PIR == customOccupancySensingClusterServerAttributes.occupancySensorType.value ||
      OCCUPANYC_SENSOR_TYPE_BITMAP_ATTRIBUTE_VALUE_PIR == customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value)
  {
    if (!customOccupancySensingClusterServerAttributes.PIRUnoccupiedToOccupiedDelay.value && !customOccupancySensingClusterServerAttributes.PIRUnoccupiedToOccupiedThreshold.value)
    {
      occupancySensingSetOccupancyState();
      return;
    }
    else
    {
      delay = customOccupancySensingClusterServerAttributes.PIRUnoccupiedToOccupiedDelay.value;
      threshold = customOccupancySensingClusterServerAttributes.PIRUnoccupiedToOccupiedThreshold.value;
    }
  }
  else if (OCCUPANYC_SENSOR_TYPE_ATTRIBUTE_VALUE_ULTRASONIC == customOccupancySensingClusterServerAttributes.occupancySensorType.value ||
           OCCUPANYC_SENSOR_TYPE_BITMAP_ATTRIBUTE_VALUE_ULTRASONIC == customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value)
  {
    if (!customOccupancySensingClusterServerAttributes.UltrasonicUnoccupiedToOccupiedDelay.value)
    {
      occupancySensingSetOccupancyState();
      return;
    }
    else
    {
      delay = customOccupancySensingClusterServerAttributes.UltrasonicUnoccupiedToOccupiedDelay.value;
      threshold = customOccupancySensingClusterServerAttributes.UltrasonicUnoccupiedToOccupiedThreshold.value;
    }
  }
  else if (OCCUPANYC_SENSOR_TYPE_ATTRIBITE_VALUE_PHYSICAL_CONTACT == customOccupancySensingClusterServerAttributes.occupancySensorType.value ||
           OCCUPANYC_SENSOR_TYPE_BITMAP_ATTRIBUTE_VALUE_PHYSICAL_CONTACT_PIR == customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value)
  {
    if (!customOccupancySensingClusterServerAttributes.PhysicalContactUnoccupiedToOccupiedDelay.value)
    {
      occupancySensingSetOccupancyState();
      return;
    }
    else
    {
      delay = customOccupancySensingClusterServerAttributes.PhysicalContactUnoccupiedToOccupiedDelay.value;
      threshold = customOccupancySensingClusterServerAttributes.PhysicalContactUnoccupiedToOccupiedThreshold.value;
    }
  }
  eventCount++; //this is considered as first movement detetcted
  delayStartTime = HAL_GetSystemTime();
  HAL_StartAppTimer(&movementEventTimer);
}

/**************************************************************************//**
\brief Simulation of occupied movement detection events (for every 2secs )

\param[in] resp - pointer to response
******************************************************************************/
static void movementDetected(void)
{
  uint64_t currentTime = 0;
  /* In general , this should be called on any kind of movement unoccupied to occupied
     or occcupied to unoccupied , but here only called on unoccipied to occupied movement 
     detection event only */
  /* API can added here to read sensor detection event - 0 to 1 or 1 to 0 */
  if (!verifyOccupancySensorDetection(1))
    return;

  if (++eventCount > (threshold & NO_OF_MOVEMENT_DETECTION_EVENTS_ALLOWED) - 1 )
  {
    HAL_StopAppTimer(&movementEventTimer);
    HAL_StopAppTimer(&sensorAttributeUpdateTimer);
    eventCount = 0;
    currentTime = HAL_GetSystemTime();
    if ((currentTime - delayStartTime)/AMOUNT_MSEC_IN_SEC >= delay )
    {
      occupancySensingSetOccupancyState();
    }
    else
    {
      /* remaining time before occupancy delay expires */
      sensorAttributeUpdateTimer.interval = delay * AMOUNT_MSEC_IN_SEC - (currentTime - delayStartTime);
      sensorAttributeUpdateTimer.mode     = TIMER_ONE_SHOT_MODE;
      sensorAttributeUpdateTimer.callback = occupancySensingSetOccupancyState;

      HAL_StartAppTimer(&sensorAttributeUpdateTimer);
    }
  }
}
/**************************************************************************//**
\brief Sets occupancy to Occupied state or Unoccupied state
******************************************************************************/
static void occupancySensingSetOccupancyState(void)
{
  customOccupancySensingClusterServerAttributes.occupancy.value = !(customOccupancySensingClusterServerAttributes.occupancy.value & 0x01);
  ZCL_ReportOnChangeIfNeeded(&customOccupancySensingClusterServerAttributes.occupancy);
  occupancyChangeState = OCCUPANCY_CHANGE_STATE_IDLE;
  eventCount = 0;
}
/**************************************************************************//**
\brief Sets occupancy to Occupied state or Unoccupied state
******************************************************************************/
void occupancySensingSetSensorType(uint8_t sensorType)
{
  if (customOccupancySensingClusterServerAttributes.occupancySensorType.value != sensorType)
  {
    customOccupancySensingClusterServerAttributes.occupancySensorType.value = sensorType;
    customOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED;
    HAL_StopAppTimer(&movementEventTimer);
    HAL_StopAppTimer(&sensorAttributeUpdateTimer);
    eventCount = 0;
  }
}
/**************************************************************************//**
\brief Sets occupancysensingTypeBitmap attribute
******************************************************************************/
void occupancySensingSetSensorTypeBitmap(uint8_t sensorType)
{
  if (customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value != sensorType)
  {
    customOccupancySensingClusterServerAttributes.occupancySensorTypeBitmap.value = sensorType;
    customOccupancySensingClusterServerAttributes.occupancy.value = OCCUPANCY_ATTRIBUTE_VALUE_UNOCCUPIED;
    HAL_StopAppTimer(&movementEventTimer);
    HAL_StopAppTimer(&sensorAttributeUpdateTimer);
    eventCount = 0;
  }
}
/**************************************************************************//**
\brief Attribute event (writing/reading) callback.

\param[in] addressing - incoming request addressing information.
\param[in] attributeId - attribute identifier.
\param[in] event - attribute event (read/write).
******************************************************************************/
static void customOccupancyAttributeEventInd(ZCL_Addressing_t *addressing, ZCL_AttributeId_t attributeId, ZCL_AttributeEvent_t event)
{
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_ATTR_OCCUPANCY;
  eventItem.eventData.zclAttributeData.addressing = addressing;
  eventItem.eventData.zclAttributeData.attributeId = attributeId;
  eventItem.eventData.zclAttributeData.event = event;
  APP_Zigbee_Handler(eventItem);

  if (((ZCL_CONFIGURE_ATTRIBUTE_REPORTING_EVENT == event) || \
      (ZCL_CONFIGURE_DEFAULT_ATTRIBUTE_REPORTING_EVENT == event)) && \
      ((ZCL_OCCUPANCY_SENSING_CLUSTER_OCCUPANCY_SERVER_ATTRIBUTE_ID == attributeId))
      )
  {
    PDS_Store(APP_CUSTOM_OCCUPANCY_MEM_ID);
  }
}
/**************************************************************************//**
\brief Report attribute indication handler

\param[in] addressing - pointer to addressing information;
\param[in] reportLength - data payload length;
\param[in] reportPayload - data pointer
******************************************************************************/
static void customOccupancySensorReportInd(ZCL_Addressing_t *addressing, uint8_t reportLength, uint8_t *reportPayload)
{
  APP_Zigbee_Event_t eventItem;
  eventItem.eventGroup = EVENT_CLUSTER;
  eventItem.eventId = CMD_ZCL_REPORTING_OCCUPANCY;
  eventItem.eventData.zclEventData.addressing = addressing;
  eventItem.eventData.zclEventData.payloadLength = reportLength;
  eventItem.eventData.zclEventData.payload = reportPayload;
  APP_Zigbee_Handler(eventItem);
}

#endif // APP_DEVICE_TYPE_CUSTOM_DEVICE

// eof customOccupancySensingCluster.c 
