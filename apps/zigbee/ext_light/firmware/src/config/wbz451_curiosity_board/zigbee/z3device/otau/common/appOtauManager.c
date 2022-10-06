/*******************************************************************************
  Zigbee Cluster Library OTAU Manager Source File

  Company:
    Microchip Technology Inc.

  File Name:
    zclOtauManager.c

  Summary:
    The file implements the common OTAU stuffs (task handler etc).

  Description:
    The file implements the common OTAU stuffs (task handler etc).
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

#if APP_USE_OTAU == 1 && defined(_ZCL_SUPPORT_) 

/******************************************************************************
                   Includes section
******************************************************************************/
#include <zcl/include/zclOtauManager.h>
#if APP_DEVICE_OTAU_LOGGING == 1
#include <app_zigbee/zigbee_console/console.h>
#endif
#include <systemenvironment/include/sysAssert.h>
#include <systemenvironment/include/sysIdleHandler.h>
#include <z3device/common/include/appConsts.h>
#ifdef OTAU_CLIENT
#include <mac_phy/mac_hwd_phy/RF231_RF212/PHY/include/phyPic32cx.h> // For system reset
#endif // OTAU_CLIENT
/******************************************************************************
                   Static functions prototype section
******************************************************************************/
#ifndef ZAPPSI_HOST
static void isZclOtauBusyOrPollRequest(SYS_EventId_t eventId, SYS_EventData_t data);
#endif

/******************************************************************************
                   Global variables section
******************************************************************************/
ZclOtauMem_t zclOtauMem;
bool genericTimerArmed = false;

#ifndef ZAPPSI_HOST
static SYS_EventReceiver_t zclOtauBusyPollCheck = { .func = isZclOtauBusyOrPollRequest};
#endif

/******************************************************************************
                   Implementation section
******************************************************************************/
#ifdef OTAU_CLIENT
static void otauClusterIndication(ZCL_OtauAction_t action);
#endif // OTAU_CLIENT

/*******************************************************************************
                    Implementation section
*******************************************************************************/
#ifdef OTAU_SERVER
/**************************************************************************//**
\brief Starts Over-the-air update server service.

\param[out] - pointer to otau cluster
******************************************************************************/
void startOtauServer(void)
{

  ZCL_OtauInitParams_t initParam;
  uint32_t fileVersion;
  initParam.clusterSide = ZCL_SERVER_CLUSTER_TYPE;
  initParam.firmwareVersion.versionId = fileVersion;
  initParam.otauEndpoint = APP_SRC_ENDPOINT_ID;
  initParam.profileId = APP_PROFILE_ID;
  if (ZCL_SUCCESS_STATUS != ZCL_StartOtauService(&initParam, NULL))
  {
    /* failed to start otau service. Check for param validity */
  }

}
#endif // OTAU_SERVER
#ifdef OTAU_CLIENT

/**************************************************************************//**
\brief Starts Over-the-air update client service.

\param[out] - pointer to otau cluster
******************************************************************************/
void startOtauClient(void)
{
  uint32_t fileVersion;
  ZCL_OtauInitParams_t initParam;
  CS_ReadParameter(CS_ZCL_OTAU_FILE_VERSION_ID, &fileVersion);
  initParam.clusterSide = ZCL_CLIENT_CLUSTER_TYPE;
  initParam.firmwareVersion.versionId = fileVersion;
  initParam.otauEndpoint = APP_SRC_ENDPOINT_ID;
  initParam.profileId = APP_PROFILE_ID;
  if (ZCL_SUCCESS_STATUS != ZCL_StartOtauService(&initParam, otauClusterIndication))
  {
    /* failed to start otau service. Check for param validity */
  }
}

/***************************************************************************//**
\brief Get indication about all otau cluster actions.

\param[in] action - current action
*******************************************************************************/
static void otauClusterIndication(ZCL_OtauAction_t action)
{
  if (OTAU_DEVICE_SHALL_CHANGE_IMAGE == action) // client is ready to change image
  {
    /*Context switch to 2 sec for any pending tasks( PDS updates )??*/
    otauStartGenericTimer(OTAU_CONTEXT_BREAK_QUICK_TIMEOUT * 100,SystemReset);
  }
}
#endif // OTAU_CLIENT
 
/***************************************************************************//**
\brief Checks whether ZCL OTAU is busy or not.

\return true - if ZCL OTAU is busy, false - otherwise.
******************************************************************************/
bool ZCL_IsOtauBusy(void)
{
#ifdef _SLEEP_WHEN_IDLE_
  if (SYS_IsSleepWhenIdleEnabled())
    return false;
  else
#endif
    return isOtauInProgress(); 
}

#ifndef ZAPPSI_HOST
/**************************************************************************//**
  \brief Processes BC_EVENT_BUSY_REQUEST request

  \param[in] eventId - id of raised event;
  \param[in] data    - event's data
 ******************************************************************************/
static void isZclOtauBusyOrPollRequest(SYS_EventId_t eventId, SYS_EventData_t data)
{
  bool *check = (bool *)data;

  if (BC_EVENT_BUSY_REQUEST == eventId)
    *check &= !ZCL_IsOtauBusy();
#if defined _ENDDEVICE_ && defined _SLEEP_WHEN_IDLE_
  else if (BC_EVENT_POLL_REQUEST == eventId)
    *check |= isOtauInProgress();
#endif
}

#endif  /* ZAPPSI_HOST */

/***************************************************************************//**
\brief OTAU parametres initialization. Start server discovery procedure
 on client side or start server.

\param[in] pInitParam - pointer to init structure.
\param[in] ind        - indication for OTAU actions \n
("ind" can be NULL if indication is not required.)
******************************************************************************/
ZCL_Status_t ZCL_StartOtauService(ZCL_OtauInitParams_t *pInitParam, ZCL_OtauStatInd_t ind)
{

#ifndef ZAPPSI_HOST
  SYS_SubscribeToEvent(BC_EVENT_BUSY_REQUEST, &zclOtauBusyPollCheck);
  SYS_SubscribeToEvent(BC_EVENT_POLL_REQUEST, &zclOtauBusyPollCheck);
#endif

  if (NULL == pInitParam)
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_NULL_POINTER_TO_INIT_STRUCTURE);
    return ZCL_INVALID_PARAMETER_STATUS;
  }

  zclOtauMem.initParam = *pInitParam;
  zclOtauMem.otauInd = ind;

  if (ZCL_CLIENT_CLUSTER_TYPE == pInitParam->clusterSide)
  {
#ifdef OTAU_CLIENT
    zclStartOtauClient();
#endif
  }
  else
  {
#ifdef OTAU_SERVER
    zclStartOtauServer();
#endif
  }
  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Stop OTAU service. If image uploading was in progress, than process is finished.

\param[in] pInitParam - pointer to init structure.

\return Service status
******************************************************************************/
ZCL_Status_t ZCL_StopOtauService(ZCL_OtauInitParams_t *pInitParam)
{
  if (NULL == pInitParam)
  {
    SYS_E_ASSERT_WARN(false, ZCL_OTAU_NULL_POINTER_TO_INIT_STRUCTURE);
    return (ZCL_INVALID_PARAMETER_STATUS);
  }

  #ifdef OTAU_SERVER
    if (ZCL_SERVER_CLUSTER_TYPE == pInitParam->clusterSide) 
    {
        zclStopOtauServer();
    }
  #else
    if (ZCL_CLIENT_CLUSTER_TYPE == pInitParam->clusterSide)
        zclStopOtauClient();
  #endif
  return ZCL_SUCCESS_STATUS;
}

/***************************************************************************//**
\brief Lift otau cluster for customer.

\param[in] action - the OTAU action for a customer.
******************************************************************************/
void zclRaiseCustomMessage(const ZCL_OtauAction_t action)
{
  SYS_E_ASSERT_FATAL((action < OTAU_MAX_MESSAGE_NUMBER), ZCL_UNKNOWN_CUSTOM_MESSAGE);

  if (zclOtauMem.otauInd)
    zclOtauMem.otauInd(action);
}

/***************************************************************************//**
\brief Clear otau client memory.
******************************************************************************/
#ifdef OTAU_CLIENT
void zclClearOtauClientMemory(void)
{
  ZCL_OtauClientMem_t *clientMem = zclGetOtauClientMem();

  CS_ReadParameter(CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT_ID, &clientMem->discoveredServerAmount);
  CS_GetMemory(CS_ZCL_OTAU_DISCOVERED_SERVER_RESULT_ID, (void *)&clientMem->discoveredServerMem);
  memset(clientMem->discoveredServerMem, 0x00, sizeof(ZclOtauDiscoveryResult_t) * clientMem->discoveredServerAmount);

}
#endif
/***************************************************************************//**
\brief Restart poll timer

\param[in] period - timer fired time
\param[in] cb     - pointer to fired method
******************************************************************************/
void otauStartGenericTimer(uint32_t period, void (* cb)(void))
{
  ZclOtauMem_t *otauMem = zclGetOtauMem();
  HAL_AppTimer_t *tmpTimer = &otauMem->genericTimer;

  HAL_StopAppTimer(tmpTimer);
  tmpTimer->interval  = period;
  tmpTimer->mode      = TIMER_ONE_SHOT_MODE;
  tmpTimer->callback  = cb;

  HAL_StartAppTimer(tmpTimer);
}

/***************************************************************************//**
\brief Stop poll timer
******************************************************************************/
void otauStopGenericTimer(void)
{
  ZclOtauMem_t *otauMem = zclGetOtauMem();
  HAL_AppTimer_t *tmpTimer = &otauMem->genericTimer;

  HAL_StopAppTimer(tmpTimer);
}
#endif // (_ZCL_SUPPORT_ == 1) 

// eof zclOtauManager.c
