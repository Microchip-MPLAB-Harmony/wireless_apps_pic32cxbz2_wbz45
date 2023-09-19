/*******************************************************************************
  Zigbee Cluster Library OTAU Manager Header File

  Company:
    Microchip Technology Inc.

  File Name:
    zclOtauManager.h

  Summary:
    Declaration of the OTAU manager interface.

  Description:
    Declaration of the OTAU manager interface.
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

#if APP_USE_OTAU == 1

#ifndef _ZCLOTAUMANAGER_H
#define _ZCLOTAUMANAGER_H
/******************************************************************************
                        Includes section
******************************************************************************/

#include <zigbee/z3device_configs/otauConfig.h>

#ifdef OTAU_CLIENT
#include <z3device/otau/ofd/include/ofdMemoryDriver.h>
#endif

#include <zcl/include/zclOTAUCluster.h>
#include <zcl/include/zclDbg.h>
#include <zdo/include/zdo.h>
#include <app_zigbee/zigbee_console/console.h>

/******************************************************************************
                        Defines section
******************************************************************************/
#define KEY_LENGTH            (16U)
#define IV_LENGTH             (16U)

#if APP_DEVICE_OTAU_LOGGING == 1 && APP_USE_OTAU == 1
  #define appOtauPrintf(...) appSnprintf(__VA_ARGS__)
#else
  #define appOtauPrintf(...) do{}while(0)
#endif

#define OTAU_CONTEXT_BREAK_QUICK_TIMEOUT     20

/******************************************************************************
                           Types section
******************************************************************************/
typedef enum
{
  OTAU_STOPPED_STATE,
  OTAU_INIT_OFD_STATE,
  OTAU_WAIT_TO_DISCOVER_STATE,
  OTAU_GET_MATCH_DESC_BROADCAST_STATE,
  OTAU_GET_MATCH_DESC_UNICAST_STATE,
  OTAU_GET_IEEE_ADDR_STATE,
  OTAU_GET_NWK_ADDR_STATE,
  OTAU_GET_LINK_KEY_STATE,
  OTAU_WAIT_TO_QUERY_STATE,
  OTAU_QUERY_FOR_IMAGE_STATE,
  OTAU_START_DOWNLOAD_STATE,
  OTAU_GET_IMAGE_DOWNLOADING_STATE,
  OTAU_WAIT_TO_UPGRADE_STATE
} ZclOtauClientStateMachine_t;

typedef union
{
  ZDO_ZdpReq_t          zdpOtauReq;
#if (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  APS_RequestKeyReq_t   apsKeyReq;
#endif // (defined _LINK_SECURITY_) && (!defined _LIGHT_LINK_PROFILE_)
  ZCL_Request_t         zclCommandReq;
} OtauReqMemory_t;

typedef union
{
  ZCL_OtauQueryNextImageReq_t   uQueryNextImageReq;
  ZCL_OtauImageBlockReq_t       uImageBlockReq;
  ZCL_OtauImagePageReq_t        uImagePageReq;
  ZCL_OtauUpgradeEndReq_t       uUpgradeEndReq;
} OtauZclReqMemory_t;

/* This structure always should consist actual values to
 * continue broken uploading. */

typedef struct
{
  /* Total Image Size to be downloaded/Currently Getting Downloaded */
  uint32_t  totalImageLength;
  /* Request(ed) File Offset. */
  uint32_t  requestFileOffset;

  /* Size of block request(ed) */
  uint8_t   requestBlockSize;
} OtauImageAuxVar_t;

typedef struct
{
  uint32_t        receivedImgBlockSize;

  uint8_t         receivedImgBlock[OTAU_MAX_REQ_BLOCK_SIZE];
} ZclOtauClientImageBuffer_t;

typedef struct
{ /* memory for storage of server discovery result */
  struct
  {
    void *next;
  } service;

  bool        busy;
  Endpoint_t  serverEndpoint;
  ShortAddr_t serverShortAddress;
  ExtAddr_t   serverExtAddress;
} ZclOtauDiscoveryResult_t;

typedef struct
{
  struct
  {
    void *next;
  } service;

  bool                           busy;
  uint8_t                        id;
  uint16_t                       pageReminderSize;
  ZCL_Addressing_t               addressing;
  ZCL_Request_t                  zclCommandReq;

  union
  {
    ZCL_OtauQueryNextImageReq_t  queryNextImageReq;
    ZCL_OtauImageBlockReq_t      imageBlockReq;
    ZCL_OtauUpgradeEndReq_t      upgradeEndReq;
    ZCL_OtauImagePageReq_t       imagePageReq;
    ZCL_DefaultResp_t            defaultResp;
  };
  union
  {
    ZCL_OtauQueryNextImageResp_t queryNextImageResp;
    ZCL_OtauImageBlockResp_t     imageBlockResp;
    ZCL_OtauUpgradeEndResp_t     upgradeEndResp;
  };

  
} ZclOtauServerTransac_t;

#ifdef OTAU_CLIENT
typedef struct
{
  // reqMem contains the memory structure for ZDO, APS and ZCL commands
  OtauReqMemory_t              reqMem;

  // Memory to store the valid block response from the server to process further
  union
  {
    ZCL_OtauImageBlockResp_t      blockResp;
    ZCL_OtauQueryNextImageResp_t  queryNextImageResp;
  };

  // zclReqMem contains the memory structure for request commands supported by the client side OTAU cluster
  OtauZclReqMemory_t           zclReqMem;
  /// blockResponseWaitTimer is used for timing the first block resp for the page request
  // also i plan to use this for timing the image page req completion turnaround
  HAL_AppTimer_t                blockResponseWaitTimer;

  // otauParam holds the image blocks/page and size received over the air
  ZclOtauClientImageBuffer_t   otauParam;

  // imageAuxParam holds the offset tracking, image breakage and current data size
  OtauImageAuxVar_t            imageAuxParam;

  // ofdParam holds the memory structure to facilitate operation of OTAU flash drive
  OFD_MemoryAccessParam_t      ofdParam;

  // newFirmwareVersion holds the version information of currently downloading image
  ZCL_OtauFirmwareVersion_t    newFirmwareVersion;

  // information related to the discovered servers
  ZclOtauDiscoveryResult_t    *discoveredServerMem;

  ZCL_OtauImageType_t          downloadingImgType;

  // numbers of servers that responded during the discovery procedure
  uint8_t                      discoveredServerAmount;
  
  // Otau Transcation retry count
  uint8_t                       retryCount;

  // Upgrade End Status
  ZCL_Status_t otauUpgradeEndStatus;
} ZCL_OtauClientMem_t;

#endif

typedef struct
{
  ZCL_Request_t            unsolicitedReq;
  ZCL_OtauUpgradeEndResp_t unsolicitedUpgradeEndResp;
  ZCL_OtauImageNotify_t    imageNotify;
  ZclOtauServerTransac_t  *serverTransac;
} ZCL_OtauServerMem_t;

typedef struct
{
  ZCL_OtauStatInd_t     otauInd;
  ZCL_OtauInitParams_t  initParam;
  HAL_AppTimer_t        genericTimer;
  bool                  isOtauStopTriggered;
#ifdef OTAU_CLIENT
    ZCL_OtauClientMem_t clientMem;
#else
    ZCL_OtauServerMem_t serverMem;
#endif
} ZclOtauMem_t;

typedef struct
{
  uint8_t keyUpdated;
  uint8_t key[KEY_LENGTH];
  uint8_t iv[IV_LENGTH];
} ZCL_OtauImageKey_t;

typedef struct
{
  ZCL_OtauFirmwareVersion_t version;
  uint32_t totalSize;
  #if defined _PIC32CX_BZ3_ 
  uint8_t   otauHeader[OTAU_MAX_REQ_PAGE_SIZE];
  #else
  uint8_t   metaHeader[OTAU_MAX_REQ_BLOCK_SIZE];
  #endif
}ZclOtauRecoveryImageInfo_t;

typedef uint32_t ZCLOtauRecoveryOffsetInfo_t;

typedef struct
{
  ZclOtauRecoveryImageInfo_t   image;
  ZCLOtauRecoveryOffsetInfo_t  fileOffset;
}ZclOtauRecovery_t;
/******************************************************************************
                           External variables section
******************************************************************************/
/** zclOtauMem is defined in zclOtauManager.c */
extern ZclOtauMem_t zclOtauMem;
extern bool isOtauBusy;

/******************************************************************************
                           Inline functions section
******************************************************************************/
INLINE ZclOtauMem_t* zclGetOtauMem(void)
{
  return &zclOtauMem;
}
#ifdef OTAU_CLIENT
INLINE ZCL_OtauClientMem_t* zclGetOtauClientMem(void)
{
  return &zclOtauMem.clientMem;
}
#endif
#ifdef OTAU_SERVER
INLINE ZCL_OtauServerMem_t* zclGetOtauServerMem(void)
{
  return &zclOtauMem.serverMem;
}
/***************************************************************************//**
\brief Checks whether OTAU cluster is busy or not.

\return true - if OTAU cluster is busy, false - otherwise.
******************************************************************************/
#endif
INLINE bool isOtauStopped(void)
{
  return zclOtauMem.isOtauStopTriggered;
}
INLINE bool isOtauInProgress(void)
{
  return isOtauBusy;
}


/******************************************************************************
                   Prototypes section
******************************************************************************/
/******`*********************************************************************//**
\brief Lift otau cluster action for customer.

\param[in] action - the OTAU action for a customer.
******************************************************************************/
void zclRaiseCustomMessage(const ZCL_OtauAction_t action);

/***************************************************************************//**
\brief Start otau client services
******************************************************************************/
void zclStartOtauClient(void);

/***************************************************************************//**
\brief Re start otau client services
******************************************************************************/
void zclRestartOtauClient(void);

/***************************************************************************//**
\brief Start otau server service
******************************************************************************/
void zclStartOtauServer(void);

/***************************************************************************//**
\brief Stop otau server service
******************************************************************************/
void zclStopOtauServer(void);

/***************************************************************************//**
\brief Stop otau client service
******************************************************************************/
void zclStopOtauClient(void);

/***************************************************************************//**
\brief Clear otau client memory.
******************************************************************************/
void zclClearOtauClientMemory(void);

/***************************************************************************//**
\brief Restart poll timer

\param[in] period - timer fired time
\param[in] cb     - pointer to fired method
******************************************************************************/
void otauStartGenericTimer(uint32_t period, void (* cb)(void));

/***************************************************************************//**
\brief Stop poll timer
******************************************************************************/
void otauStopGenericTimer(void);

/***************************************************************************//**
\brief  Returns OTAU recovery parameters
******************************************************************************/
ZclOtauRecovery_t *zclOtauGetRecoveryParams(void);

/***************************************************************************//**
\brief  Resets OTAU recovery parameters to default
******************************************************************************/
void zclOtauResetRecoveryParams(void);
void zclOtauResetAttributesPDS(void);

#endif /* _ZCLOTAUMANAGER_H */
#endif // APP_USE_OTAU
//eof zclOtauManager.h
