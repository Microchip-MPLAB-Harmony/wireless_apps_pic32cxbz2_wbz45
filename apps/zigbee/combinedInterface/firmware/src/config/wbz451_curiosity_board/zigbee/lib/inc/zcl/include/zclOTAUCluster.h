// DOM-IGNORE-BEGIN
/*******************************************************************************
  Zigbee Cluster Library OTAU Cluster Header File

  Company:
    Microchip Technology Inc.

  File Name:
    zclOTAUCluster.h

  Summary:
    The header file describes the OTAU cluster interface.

  Description:
    The header file describes the OTAU cluster interface.
 *******************************************************************************/


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

// DOM-IGNORE-BEGIN
#ifndef _ZCLOTAUCLUSTER_H
#define _ZCLOTAUCLUSTER_H
// DOM-IGNORE-END

/******************************************************************************
                        Includes section
 ******************************************************************************/

#include <zcl/include/zclParser.h>
#include <zcl/include/zcl.h>
#include <zcl/include/clusters.h>

/******************************************************************************
                        Defines section
 ******************************************************************************/
 
//#define _OTAU_DEBUG_

/** ZCL OTAU Cluster attributes */
#define ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT      0
#define ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT      6


/** ZCL OTAU Cluster commands amount */
#define ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT               8

/** ZCL OTAU Cluster client side attributes id */
#define ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_SERVER_ID                   0x0000
#define ZCL_OTAU_CLUSTER_CLIENT_IMAGE_UPGRADE_STATUS_ID             0x0006
#define ZCL_OTAU_CLUSTER_CLIENT_MINIMUM_BLOCK_REQUEST_DELAY_ID      0x0009
#define ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_ACTIVATION_POLICY_ID        0x000B
#define ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_TIMEOUT_POLICY_ID           0x000C

/** Global Attribute */
#define ZCL_OTAU_CLUSTER_CLIENT_CLUSTER_REVISION_ID                 0xfffd


#define DEFINE_OTAU_CLIENT_ATTRIBUTES \
        DEFINE_ATTRIBUTE(upgradeServerID, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_SERVER_ID, ZCL_IEEE_ADDRESS_DATA_TYPE_ID), \
        DEFINE_ATTRIBUTE(imageUpgradeStatus, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_IMAGE_UPGRADE_STATUS_ID, ZCL_8BIT_ENUM_DATA_TYPE_ID), \
        DEFINE_ATTRIBUTE_WITH_BOUNDARY(minimumBlockPeriod, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_MINIMUM_BLOCK_REQUEST_DELAY_ID, \
                                                  ZCL_U16BIT_DATA_TYPE_ID, 0x0000, 0x0258), \
        DEFINE_ATTRIBUTE_WITH_BOUNDARY(clusterRevision, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_CLUSTER_REVISION_ID, \
                                                  ZCL_U16BIT_DATA_TYPE_ID, 0x0001, 0xfffe), \
        DEFINE_ATTRIBUTE_WITH_BOUNDARY(upgradeActivationPolicy, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_ACTIVATION_POLICY_ID, \
                                                  ZCL_8BIT_ENUM_DATA_TYPE_ID, 0x00, 0x01), \
        DEFINE_ATTRIBUTE_WITH_BOUNDARY(upgradeTimeoutPolicy, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_UPGRADE_TIMEOUT_POLICY_ID, \
                                                  ZCL_8BIT_ENUM_DATA_TYPE_ID, 0x00, 0x01)

#define DEFINE_OTAU_SERVER_ATTRIBUTES \
        DEFINE_ATTRIBUTE_WITH_BOUNDARY(clusterRevision, ZCL_READONLY_ATTRIBUTE, ZCL_OTAU_CLUSTER_CLIENT_CLUSTER_REVISION_ID, \
                                                  ZCL_U16BIT_DATA_TYPE_ID, 0x0001, 0xfffe)

#define OTAU_HEADER_STRING_SIZE                                      32
//#define OTAU_MAX_REQ_BLOCK_SIZE \
//          (APS_MAX_NON_SECURITY_ASDU_SIZE \
//          - ZCL_IMAGE_BLOCK_RESP_HEADER_SIZE - ZCL_FRAME_STANDARD_HEADER_SIZE)

#define OTAU_MAX_REQ_BLOCK_SIZE 48

#define OTAU_MAX_REQ_PAGE_SIZE 256

// The page size - the number of bytes sent for a single image page request
// 
//  The parameter sets the number of bytes to be sent by the server for an image
// page request sent by the OTAU client. A page is transmitted in several data
// frames, one data frame for each block of data. The client, upon receiving
// individual blocks, does not send an APS acknowledgement until all blocks for the
// current page are not sent. Blocks that have not been delivered are requested by
// the client via image block requests after loading of the current page finishes.
// 
//  The page size should be greater than or equal to 64 bytes.
// 
//  The parameter is valid only for OTAU clients.
// 
//  Value range: equal to or greater than 64
//  C-type: uint16_t
//  Can be set: at compile time only
//  Persistent: No
#if defined _PIC32CX_BZ3_ 
#define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE    DRV_SST26_PAGE_SIZE
#else
#define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_PAGE_SIZE    (OTAU_MAX_REQ_BLOCK_SIZE * 5)
#endif

// OTAU commands
// list of commands id
#define IMAGE_NOTIFY_ID                             0x00
#define QUERY_NEXT_IMAGE_REQUEST_ID                 0x01
#define QUERY_NEXT_IMAGE_RESPONSE_ID                0x02
#define IMAGE_BLOCK_REQUEST_ID                      0x03
#define IMAGE_PAGE_REQUEST_ID                       0x04
#define IMAGE_BLOCK_RESPONSE_ID                     0x05
#define UPGRADE_END_REQUEST_ID                      0x06
#define UPGRADE_END_RESPONSE_ID                     0x07
#define DEFAULT_RESPONSE_ID                         0x08
#define DEFAULT_RESPONSE_IND_ID                     0x09


#define OTAU_IMAGE_NOTIFY_MAX_QUERY_JITTER_VALUE    100

#define DEFINE_IMAGE_NOTIFY(otauImageNotifyInd) \
        DEFINE_COMMAND(imageNotify, IMAGE_NOTIFY_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauImageNotifyInd)

#define DEFINE_QUERY_NEXT_IMAGE_REQUEST(otauQueryNextImageReqInd) \
        DEFINE_COMMAND(queryNextImageReq, QUERY_NEXT_IMAGE_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauQueryNextImageReqInd)

#define DEFINE_QUERY_NEXT_IMAGE_RESPONSE(otauQueryNextImageRespInd) \
        DEFINE_COMMAND(queryNextImageResp, QUERY_NEXT_IMAGE_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauQueryNextImageRespInd)

#define DEFINE_IMAGE_BLOCK_REQUEST(otauImageBlockReqInd)  \
        DEFINE_COMMAND(imageBlockReq, IMAGE_BLOCK_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauImageBlockReqInd)

#define DEFINE_IMAGE_PAGE_REQUEST(otauImagePageReqInd)  \
        DEFINE_COMMAND(imagePageReq, IMAGE_PAGE_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauImagePageReqInd)

#define DEFINE_IMAGE_BLOCK_RESPONSE(otauImageBlockRespInd) \
        DEFINE_COMMAND(imageBlockResp, IMAGE_BLOCK_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauImageBlockRespInd)

#define DEFINE_UPGRADE_END_REQUEST(otauUpgradeEndReqInd) \
        DEFINE_COMMAND(upgradeEndReq, UPGRADE_END_REQUEST_ID, \
        COMMAND_OPTIONS(CLIENT_TO_SERVER, ZCL_THERE_IS_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauUpgradeEndReqInd)

#define DEFINE_UPGRADE_END_RESPONSE(otauUpgradeEndRespInd) \
        DEFINE_COMMAND(upgradeEndResp, UPGRADE_END_RESPONSE_ID, \
        COMMAND_OPTIONS(SERVER_TO_CLIENT, ZCL_THERE_IS_NO_RELEVANT_RESPONSE, ZCL_COMMAND_ACK), \
        otauUpgradeEndRespInd)

#define OTAU_COMMANDS(otauImageNotifyInd, otauQueryNextImageReqInd, otauQueryNextImageRespInd, otauImageBlockReqInd,     \
        otauImagePageReqInd, otauImageBlockRespInd, otauUpgradeEndReqInd, otauUpgradeEndRespInd)                         \
        DEFINE_IMAGE_NOTIFY(otauImageNotifyInd),                                                             \
        DEFINE_QUERY_NEXT_IMAGE_REQUEST(otauQueryNextImageReqInd),                                           \
        DEFINE_QUERY_NEXT_IMAGE_RESPONSE(otauQueryNextImageRespInd),                                         \
        DEFINE_IMAGE_BLOCK_REQUEST(otauImageBlockReqInd),                                                    \
        DEFINE_IMAGE_PAGE_REQUEST(otauImagePageReqInd),                                                      \
        DEFINE_IMAGE_BLOCK_RESPONSE(otauImageBlockRespInd),                                                  \
        DEFINE_UPGRADE_END_REQUEST(otauUpgradeEndReqInd),                                                    \
        DEFINE_UPGRADE_END_RESPONSE(otauUpgradeEndRespInd)

/***************************************************************************//**
  \brief ZCL OTAU Cluster server side defining macros

  \param attributes - pointer to cluster server attributes (ZCL_OtauClusterClientAttributes_t)
         or NULL if there are no attributes
  clcommands - pointer to cluster server commands
         or NULL if there are no commands

  \return None
 *******************************************************************************/
#define OTAU_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands)        \
  {                                                                           \
    .id = OTAU_CLUSTER_ID,                                                    \
    .options = {                                                              \
      .type = ZCL_SERVER_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0},                                                         \
    .attributesAmount = ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,            \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                       \
    .commands = (uint8_t *) clcommands                                        \
  }

#define OTAU_CLUSTER_ZCL_SERVER_CLUSTER_TYPE_FLASH(clattributes, clcommands)  \
  {                                                                           \
    .id = OTAU_CLUSTER_ID,                                                    \
    .options = {                                                              \
      .type = ZCL_SERVER_CLUSTER_TYPE,                                        \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                           \
      .reserved = 0},                                                         \
    .attributesAmount = ZCL_OTAU_CLUSTER_SERVER_ATTRIBUTES_AMOUNT,            \
    .attributes = (uint8_t *) clattributes,                                   \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                       \
    .commands = (FLASH_PTR uint8_t *) clcommands                              \
  }

/***************************************************************************//**
  \brief ZCL OTAU Cluster client side defining macros

  \param attributes - pointer to cluster client attributes (ZCL_OtauClusterClientAttributes_t)
          or NULL if there are no attributes
  clcommands - pointer to cluster client commands
          or NULL if there are no commands

  \return None
 *******************************************************************************/
#define OTAU_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands) \
  {                                                                    \
    .id = OTAU_CLUSTER_ID,                                             \
    .options = {                                                       \
      .type = ZCL_CLIENT_CLUSTER_TYPE,                                 \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                    \
      .reserved = 0},                                                  \
    .attributesAmount = ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT,     \
    .attributes = (uint8_t *) clattributes,                            \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                \
    .commands = (uint8_t *) clcommands                                 \
  }

#define OTAU_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE_FLASH(clattributes, clcommands) \
  {                                                                          \
    .id = OTAU_CLUSTER_ID,                                                   \
    .options = {                                                             \
      .type = ZCL_CLIENT_CLUSTER_TYPE,                                       \
      .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY,                          \
      .reserved = 0},                                                        \
    .attributesAmount = ZCL_OTAU_CLUSTER_CLIENT_ATTRIBUTES_AMOUNT,           \
    .attributes = (uint8_t *) clattributes,                                  \
    .commandsAmount = ZCL_OTAU_CLUSTER_COMMANDS_AMOUNT,                      \
    .commands = (FLASH_PTR uint8_t *) clcommands                             \
  }

#define DEFINE_OTAU_CLUSTER(cltype, clattributes, clcommands) OTAU_CLUSTER_##cltype(clattributes, clcommands)
#define DEFINE_OTAU_CLUSTER_FLASH(cltype, clattributes, clcommands) OTAU_CLUSTER_##cltype##_FLASH(clattributes, clcommands)

#define ZCL_IMAGE_BLOCK_RESP_HEADER_SIZE \
            ( sizeof(ZCL_Status_t) + sizeof(uint16_t) + sizeof(ZCL_OtauImageType_t) \
              + sizeof(ZCL_OtauFirmwareVersion_t) + sizeof(uint32_t) + sizeof(uint8_t) )

/*******************************************************************************
                   Types section
 *******************************************************************************/
 
typedef uint8_t OtauHeaderString_t[OTAU_HEADER_STRING_SIZE];

typedef enum
{
  OTAU_SERVER_DISCOVERY_INITIATED,
  OTAU_SERVER_HAS_BEEN_FOUND,
  OTAU_SERVER_HAS_NOT_BEEN_FOUND,
  OTAU_SERVER_HAS_BEEN_LOST,
  OTAU_OFD_DRIVER_ERROR,
  OTAU_SERVER_RECEIVED_MALFORMED_COMMAND,
  OTAU_SERVER_NO_IMAGE_AVAILABLE,
  OTAU_SERVER_RECEIVED_UNSUPPORTED_COMMAND,
  OTAU_SERVER_RECEIVED_UNKNOWN_STATUS,
  OTAU_SERVER_IS_NOT_AUTHORIZED_TO_UPDATE,
  OTAU_DOWNLOAD_STARTED,
  OTAU_DOWNLOAD_ABORTED,
  OTAU_QUERY_NEXT_IMAGE_INITIATED,
  OTAU_IMAGE_BLOCK_REQ_INITIATED,
  OTAU_DOWNLOAD_FINISHED,
  OTAU_UPGRADE_REQUEST_INITIATED,
  OTAU_DEVICE_SHALL_CHANGE_IMAGE,
  OTAU_ISD_IMAGE_CACHE_IS_FOUND,
  OTAU_ISD_HARDWARE_FAULT,
  OTAU_ISD_NO_COMMUNICATION,
  OTAU_IMAGE_NOTIFICATION_RECEIVED,
  OTAU_DOUBLE_START,
  OTAU_MAX_MESSAGE_NUMBER,
} ZCL_OtauAction_t;

typedef void (* ZCL_OtauStatInd_t)(ZCL_OtauAction_t action);

typedef enum
{
  OTAU_ZB_STACK_VERSION_2006 = 0x0000,
  OTAU_ZB_STACK_VERSION_2007 = 0x0001,
  OTAU_ZB_STACK_VERSION_PRO  = 0x0002,
  OTAU_ZB_STACK_VERSION_IP   = 0x0003,
  OTAU_FAKE_VERSION          = 0xFFFF
} ZCL_OtauZigbeeStackVersion_t;

typedef enum
{
  /*Manufacturer Specific Values*/
  OTAU_IMAGE_TYPE_MANU_SPEC_NULL_UPGRADE_FILE         = 0x000F,
#ifdef OTAU_CLIENT       
  OTAU_IMAGE_TYPE_APPLICATION                         = OTAU_APP_IMAGE_TYPE,
#endif
  /*Spec defined values*/
  OTAU_IMAGE_TYPE_CLIENT_SECURITY_CREDENTIAL          = 0xFFC0,
  OTAU_IMAGE_TYPE_CLIENT_CONFIGURATION                = 0xFFC1,
  OTAU_IMAGE_TYPE_SERVER_LOG                          = 0xFFC2,
  OTAU_IMAGE_TYPE_PICTURE                             = 0xFFC3,
  OTAU_IMAGE_TYPE_WILD_CARD                           = 0xFFFF
  
} ZCL_OtauImageType_t;

typedef enum
{
  OTAU_UPGRADE_IMAGE             = 0x0000,
  OTAU_ECDSA_SIGNATURE           = 0x0001,
  OTAU_ECDSA_SIGNING_CERTIFICATE = 0x0002,
  OTAU_FAKE_NUMBER               = 0xFFFF
} ZCL_OtauTagID_t;

typedef enum
{
  OTAU_NORMAL,
  OTAU_DOWNLOAD_IN_PROGRESS,
  OTAU_DOWNLOAD_COMPLETE,
  OTAU_WAITING_TO_UPGRADE,
  OTAU_COUNT_DOWN,
  OTAU_WAIT_FOR_MORE        //  Not Supported/Required for BC 4.x
} ZCL_ImageUpdateStatus_t;

/** Payload type field for Image Notify command. */
typedef enum
{
  OTAU_PAYLOAD_TYPE_QUERY_JITTER      = 0x00, //!< Query jitter
  OTAU_PAYLOAD_TYPE_MANUFACTURER_CODE = 0x01, //!< Query jitter and manufacturer code
  OTAU_PAYLOAD_TYPE_IMAGE_TYPE        = 0x02, //!< Query jitter, manufacturer code and image type
  OTAU_PAYLOAD_TYPE_NEW_FILE_VERSION  = 0x03, //!< Query jitter, manufacturer code, image type, and new file version
  OTAU_PAYLOAD_TYPE_RESERVED          = 0x04, //!< Values 0x04 - 9xFF are reserved
} ZCL_ImageNotifyPayloadType_t;

/** upgradeActivationPolicy attribute values */
typedef enum
{
  OTA_SERVER_ACTIVATION_ALLOWED = 0,
  OTA_OOUTOFBAND_ACTIVATION     = 1,
}upgradeActivationPolicy_t;

/** UpgradeTimeoutPolicy attribute values */
typedef enum
{
  OTA_APPLY_UPGRADE_AFTER_TIMEOUT = 0,
  OTA_NO_UPGRADE_AFTER_TIMEOUT    = 1,
}UpgradeTimeoutPolicy_T;

BEGIN_PACK
typedef union PACK
{
  uint16_t memAlloc;
  struct PACK
  {
    uint16_t securityCredentialVersionPresent : 1;
    uint16_t deviceSpecificFile               : 1;
    uint16_t hardwareVersionPresent           : 1;
    uint16_t reserved                         : 13;
  };
} ZCL_OtauHeaderFieldControl_t;

typedef struct PACK
{
  ZCL_OtauTagID_t tagID;
  uint32_t        lengthField;
} ZCL_OtauSubElementHeader_t;

typedef union PACK
{
  uint32_t versionId;
  struct PACK
  {
    uint8_t appRelease;
    uint8_t appBuild;
    uint8_t stackRelease;
    uint8_t stackBuild;
  };
} ZCL_OtauFirmwareVersion_t;

/** ZCL OTAU upgrade image header */
typedef struct PACK
{
  uint32_t                     upgradeFileID;
  uint16_t                     headerVersion;
  uint16_t                     headerLength;
  ZCL_OtauHeaderFieldControl_t fieldControl;
  uint16_t                     manufacturerId;
  uint16_t                     imageType;
  ZCL_OtauFirmwareVersion_t    firmwareVersion;
  ZCL_OtauZigbeeStackVersion_t zigbeeStackVersion;
  OtauHeaderString_t           headerString;
  uint32_t                     totalImageSize;
} ZCL_OtauUpgradeImageHeader_t;

/** ZCL OTAU Cluster attributes.
    Currently, all attributes are client side attributes (only stored on the client).
*/
typedef struct PACK
{
  /*!
  The attribute is used to store the IEEE address of the upgrade server
  resulted from the discovery of the upgrade server's identity. If the value is set
  to a non-zero value and corresponds to an IEEE address of a device that is no longer
  accessible, a device may choose to discover a new Upgrade Server depending on its own
  security policies. The attribute is mandatory because it serves as a placeholder in a case
  where the client is programmed, during manufacturing time, its upgrade server ID.
  In addition, the attribute is used to identify the current upgrade server the client is using
  in a case where there are multiple upgrade servers in the network.  The attribute is also helpful
  in a case when a client has temporarily lost connection to the network (for example, via a reset
  or a rejoin), it shall try to rediscover the upgrade server via network address discovery using
  the IEEE address stored in the attribute. By default the value is 0xffffffffffffffff, which is
  an invalid IEEE address.  The attribute is a client-side attribute and stored on the client.
  */
  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0000)
    uint8_t                 type;         //!<Attribute type (ExtAddr_t - 64-bit) (typeId = 0xf0, #ZCL_IEEE_ADDRESS_DATA_TYPE_ID)
    uint8_t                 properties;   //!<Attribute properties bitmask
    ExtAddr_t               value;        //!<Attribute value (default 0xFFFFFFFFFFFFFFFF)
  } upgradeServerID;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0006)
    uint8_t                 type;         //!<Attribute type (ZCL_ImageUpdateStatus_t - 8-bit enum) (typeId = 0x30)
    uint8_t                 properties;   //!<Attribute properties bitmask
    ZCL_ImageUpdateStatus_t value;        //!<Attribute value (default 0x00)
  } imageUpgradeStatus;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x0009)
    uint8_t                 type;         //!<Attribute type (uint16_t - 16-bit) (typeId = 0x21)
    uint8_t                 properties;   //!<Attribute properties bitmask
    uint16_t                value;        //!<Attribute value (default none)
    uint16_t                minVal;        //!<Attribute min value
    uint16_t                maxVal;        //!<Attribute max value
  } minimumBlockPeriod;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0xfffd)
    uint8_t                 type;         //!<Attribute type (uint16_t - 16-bit) (typeId = 0x21)
    uint8_t                 properties;   //!<Attribute properties bitmask
    uint16_t                value;        //!<Attribute value (default none)
    uint16_t                minVal;        //!<Attribute min value
    uint16_t                maxVal;        //!<Attribute max value
  } clusterRevision;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x000B)
    uint8_t                 type;         //!<Attribute type (ZCL_8BIT_ENUM_DATA_TYPE_ID) (typeId = 0x30)
    uint8_t                 properties;   //!<Attribute properties bitmask
    uint8_t                 value;        //!<Attribute value (default 0)
    uint8_t                 minVal;        //!<Attribute min value
    uint8_t                 maxVal;        //!<Attribute max value
  } upgradeActivationPolicy;

  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0x000C)
    uint8_t                 type;         //!<Attribute type (ZCL_8BIT_ENUM_DATA_TYPE_ID) (typeId = 0x30)
    uint8_t                 properties;   //!<Attribute properties bitmask
    uint8_t                 value;        //!<Attribute value (default 0)
    uint8_t                 minVal;        //!<Attribute min value
    uint8_t                 maxVal;        //!<Attribute max value
  }upgradeTimeoutPolicy;
} ZCL_OtauClusterClientAttributes_t;

/** ZCL OTAU Cluster Server attributes.
    Currently, all attributes are client side attributes (only stored on the client).
*/
typedef struct PACK
{
  struct PACK
  {
    ZCL_AttributeId_t       id;           //!<Attribute Id (0xfffd)
    uint8_t                 type;         //!<Attribute type (uint16_t - 16-bit) (typeId = 0x21)
    uint8_t                 properties;   //!<Attribute properties bitmask
    uint16_t                value;        //!<Attribute value (default none)
    uint16_t                minVal;        //!<Attribute min value
    uint16_t                maxVal;        //!<Attribute max value
  } clusterRevision;

} ZCL_OtauClusterServerAttributes_t;

/** ZCL OTAU Cluster commands. 
*/
/** Image Notify Command. */
typedef struct PACK
{
  ZCL_ImageNotifyPayloadType_t payloadType;
  uint8_t                      queryJitter;
  uint16_t                     manufacturerCode;
  uint16_t                     imageType;
  uint32_t                     newFileVersion;
} ZCL_OtauImageNotify_t;

/** Control field of query next image request. */
typedef struct PACK
{
  uint8_t hardwareVersionPresent  : 1;
  uint8_t reserved                : 7;
} ZCL_QueryNextImageReqControlField_t;


/** Query Next Image Request Command. */
typedef struct PACK
{
  ZCL_QueryNextImageReqControlField_t  controlField;
  uint16_t                             manufacturerId;
  ZCL_OtauImageType_t                  imageType;
  ZCL_OtauFirmwareVersion_t            currentFirmwareVersion;
} ZCL_OtauQueryNextImageReq_t;

/** Query Next Image Response Command. */
typedef struct PACK
{
  ZCL_Status_t              status;
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t currentFirmwareVersion;
  uint32_t                  imageSize;
} ZCL_OtauQueryNextImageResp_t;

/** Control field of image block request. */
typedef struct PACK
{
  uint8_t reqNodeIeeeAddrPresent   : 1;
  uint8_t blockRequestDelayPresent : 1;
  uint8_t reserved                 : 6;
} ZCL_ImageBlockReqControlField_t;

/** Control field of image page request. */
typedef struct PACK
{
  uint8_t reqNodeIeeeAddrPresent   : 1;
  uint8_t reserved                 : 7;
} ZCL_PageReqControlField_t;

/** Image Block Request Command. */
typedef struct PACK
{
  ZCL_ImageBlockReqControlField_t controlField;
  uint16_t                        manufacturerId;
  ZCL_OtauImageType_t             imageType;
  ZCL_OtauFirmwareVersion_t       firmwareVersion;
  uint32_t                        fileOffset;
  uint8_t                         maxDataSize;
  uint16_t                        minimumBlockPeriod;
} ZCL_OtauImageBlockReq_t;

/** Image Block Response Command. */
typedef struct PACK
{
  ZCL_Status_t                 status;
  union PACK
  {
    struct PACK
    {
      uint16_t                  manufacturerId;
      ZCL_OtauImageType_t       imageType;
      ZCL_OtauFirmwareVersion_t firmwareVersion;
      uint32_t                  fileOffset;
      uint8_t                   dataSize;
      uint8_t                   imageData[OTAU_MAX_REQ_BLOCK_SIZE];
    };
    struct PACK
    {
      uint32_t                  currentTime;
      uint32_t                  requestTime;
      uint16_t                  minimumBlockPeriod;
    };
  };
} ZCL_OtauImageBlockResp_t;

/** Upgrade End Request Command. */
typedef struct PACK
{
  ZCL_Status_t              status;
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t firmwareVersion;
} ZCL_OtauUpgradeEndReq_t;

/**  Upgrade End Response Command. */
typedef struct PACK
{
  uint16_t                  manufacturerId;
  ZCL_OtauImageType_t       imageType;
  ZCL_OtauFirmwareVersion_t firmwareVersion;
  uint32_t                  currentTime;
  uint32_t                  upgradeTime;
} ZCL_OtauUpgradeEndResp_t;

/** Image Page Request Command. */
typedef struct PACK
{
  ZCL_PageReqControlField_t      controlField;
  uint16_t                       manufacturerId;
  ZCL_OtauImageType_t            imageType;
  ZCL_OtauFirmwareVersion_t      firmwareVersion;
  uint32_t                       fileOffset;
  uint8_t                        maxDataSize;
  uint16_t                       pageSize;
  uint16_t                       responseSpacing;
} ZCL_OtauImagePageReq_t;
END_PACK

/** ZCL OTAU Cluster Commands */
typedef struct
{
  //! Image Notify Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageNotifyind)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageNotify_t *payload);
  } imageNotify;

  //! Query Next Image Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pQueryNextImageReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageReq_t *payload);
  } queryNextImageReq;

  //! Query Next Image Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pQueryNextImageRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauQueryNextImageResp_t *payload);
  } queryNextImageResp;

  //! Image Block Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageBlockReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockReq_t *payload);
  } imageBlockReq;

  //! Image Page Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImagePageReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImagePageReq_t *payload);
  } imagePageReq;

  //! Image Block Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pImageBlockRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauImageBlockResp_t *payload);
  } imageBlockResp;

  //! Upgrade End Request Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pUpgradeEndReqInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndReq_t *payload);
  } upgradeEndReq;

  //! Upgrade End Response Command descriptor
  struct
  {
    ZCL_CommandId_t       id;
    ZclCommandOptions_t   options;
    ZCL_Status_t (* pUpgradeEndRespInd)(ZCL_Addressing_t *addressing, uint8_t payloadLength, ZCL_OtauUpgradeEndResp_t *payload);
  } upgradeEndResp;
} ZCL_OtauClusterCommands_t;

/** Initialization parameters for OTAU service */
typedef struct
{
/** Cluster side. Must be chosen from: \n
    ZCL_SERVER_CLUSTER_TYPE \n
    ZCL_CLIENT_CLUSTER_TYPE */
  uint8_t                    clusterSide;
/** Endpoint for cluster. */
  Endpoint_t                 otauEndpoint;
/**  Profile ID, that uses otau service. */
  ProfileId_t                profileId;
/** Current firmware version. Parameter is valid only for client. */
  ZCL_OtauFirmwareVersion_t  firmwareVersion;
} ZCL_OtauInitParams_t;

extern ZCL_OtauClusterClientAttributes_t otauClientAttributes;

/******************************************************************************
                   Prototypes section
 ******************************************************************************/
 
/***************************************************************************//**
  \brief Gets Otau's cluster
  
  \param None
  
  \return Otau's client cluster
 ******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauClientCluster(void);

/***************************************************************************//**
  \brief Gets Otau's cluster
  
  \param None
  
  \return Otau's server cluster
 ******************************************************************************/
ZCL_Cluster_t ZCL_GetOtauServerCluster(void);

/***************************************************************************//**
  \brief OTAU parametres initialization. Start server discovery procedure
 on client side or start server.


  \param pInitParam - pointer to init structure.
         ind        - indication for OTAU actions \n
   ("ind" can be NULL if indication is not required.)

  \return Service status
 ******************************************************************************/
ZCL_Status_t ZCL_StartOtauService(ZCL_OtauInitParams_t *pInitParam, ZCL_OtauStatInd_t ind);

/***************************************************************************//**
  \brief Stop OTAU service. (It is not implemented for client)


  \param pInitParam - pointer to init structure.

  \return Service status
 ******************************************************************************/
ZCL_Status_t ZCL_StopOtauService(ZCL_OtauInitParams_t *pInitParam);

/***************************************************************************//**
  \brief The server may send an unsolicited Upgrade End Response command to the client.
  This may be used for example if the server wants to synchronize the upgrade
  on multiple clients simultaneously.


  \param addressing - pointer to structure with addressing information;
         resp - pointer to upgrade end response payload

  \return true - data is sending, false - server is busy.
 ******************************************************************************/
bool ZCL_UnsolicitedUpgradeEndResp(ZCL_Addressing_t *addressing, ZCL_OtauUpgradeEndResp_t *resp);

/***************************************************************************//**
  \brief This is called by the server to send image notify command to client


  \param addressing - pointer to structure with addressing information;
         resp - pointer to image notify command payload

  \return true - data is sending, false - server is busy.
 ******************************************************************************/
bool ZCL_ImageNotifyReq(ZCL_Addressing_t *addressing, ZCL_OtauImageNotify_t *resp);

#endif 
/** _ZCLOTAUCLUSTER_H */
