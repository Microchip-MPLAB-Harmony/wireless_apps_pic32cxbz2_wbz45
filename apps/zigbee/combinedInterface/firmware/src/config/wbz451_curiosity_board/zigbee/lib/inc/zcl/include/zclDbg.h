/*******************************************************************************
  Zigbee Cluster Debug Module Header File

  Company:
    Microchip Technology Inc.

  File Name:
    zclDbg.h

  Summary:
    The header file describes ZCL Debug Module.

  Description:
    The file describes the ZCL Debug Module.
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



#ifndef _ZCLDBG_H
#define _ZCLDBG_H

/******************************************************************************
                   Includes section
******************************************************************************/
#include <systemenvironment/include/dbg.h>

/******************************************************************************
                   Types section
******************************************************************************/

/******************************************************************************
                   Constants section
******************************************************************************/
typedef enum
{
  /***********************************
    ZCL section. Range 0xC000 - 0xCfff
  ************************************/

  //ZCL Task Manager Id       (0xC000 - 0xC0ff)
  ZCLTASKMANAGER_TASKHANDLER_0                      = 0xC000,
  //ZCL Security Manager Id   (0xC100 - 0xC1ff)
  ZCL_STARTSECURITY_NULLREQUEST                     = 0xC100,
  ZCL_STARTSECURITY_NULLCONFIRM                     = 0xC101,
  //ZCL ZCL Id                (0xC200 - 0xC2ff)
  ZCLGETCLUSTER_0                                   = 0xC200,
  PARSEDATAIND_0                                    = 0xC201,
  ZCLREPORTIND_0                                    = 0xC202,
  DBG_ZCL_GET_TRUST_CENTER_ADDRESS                  = 0xC203,
  ZCL_UNDEFINED_CLUSTER_IN_REQUEST                  = 0xC204,
  ZCL_UNEXPECTED_ASDU_LENGTH                        = 0xC205,
  UNKNOWN_DAT_TYPE_DESCR                            = 0xC206,
  ZCL_UNBOUNDED_READ                                = 0xC207,
  //ZCL Memory Manager        (0xC300 - 0xC3ff)
  ZCL_DATAIND_0                                     = 0xC300,
  ZCL_THERE_ARE_NO_BUFFERS                          = 0xC301,
  ZCL_BUFFER_SIZE_IS_TOO_LOW                        = 0xC302,
  ZCL_EP_NOT_FOUND                                  = 0xC303,

#if APP_USE_OTAU == 1
  //OTAU                      (0xC400 - 0xC4ff)
  ZCL_OTAU_NULL_POINTER_TO_INIT_STRUCTURE           = 0xC400,
  ZCL_OTAU_DOUBLE_START                             = 0xC401,
  ZCL_OTAU_UNEXPECTED_SERVER_DISCOVERY              = 0xC402,
  ZCL_UNKNOWN_CUSTOM_MESSAGE                        = 0xC403,
  ZCL_OTAU_UNKNOWN_SERVER_TRANSACTUION_ID           = 0xC404,
  ZCL_UNKNOWN_ISD_MESSAGE                           = 0xC405,
  ZCL_OTAU_DISCOVERY_QUEUE_IS_EMPTY                 = 0xC406,
  ZCL_OTAU_INVALID_OPERATION                        = 0xC407,
  ZCL_OTAU_INVALID_STATE_OFD_OPEN_CALLBACK          = 0xC408,
  ZCL_OTAU_INVALID_STATE_OFD_WRITE                  = 0xC409,
  ZCL_OTAU_INVALID_STATE_OFD_WRITE_CALLBACK         = 0xC40A,
  ZCL_OTAU_INVALID_STATE_OFD_ERASE                  = 0xC40B,
  ZCL_OTAU_INVALID_STATE_OFD_ERASE_CALLBACK         = 0xC40C,
  ZCL_OTAU_INVALID_STATE_BROADCAST_MATCH_DESC_RSP   = 0xC40D,
  ZCL_OTAU_INVALID_STATE_UNICAST_MATCH_DESC_RSP	    = 0xC40E,
  ZCL_OTAU_INVALID_STATE_MATCH_DESC_REQ             = 0xC40F,
  ZCL_OTAU_INVALID_STATE_IEEE_ADDR_RSP              = 0xC410,
  ZCL_OTAU_INVALID_STATE_IEEE_ADDR_REQ              = 0xC411,
  ZCL_OTAU_INVALID_STATE_NWK_ADDR_RSP               = 0xC412,
  ZCL_OTAU_INVALID_STATE_NWK_ADDR_REQ               = 0xC413,
  ZCL_OTAU_INVALID_STATE_REQUEST_KEY_RSP            = 0xC414,
  ZCL_OTAU_INVALID_STATE_REQUEST_KEY_REQ            = 0xC415,
  ZCL_OTAU_INVALID_STATE_START_DISCOVERY            = 0xC416,
  ZCL_OTAU_INVALID_STATE_IMAGE_BLOCK_RSP            = 0xC417,
  ZCL_OTAU_INVALID_STATE_IMAGE_BLOCK_REQ            = 0xC418,
  ZCL_OTAU_INVALID_STATE_START_DOWNLOAD             = 0xC419,
  ZCL_OTAU_INVALID_STATE_START_QUERY                = 0xC41A,
  ZCL_OTAU_INVALID_STATE_QUERY_AFTER_JITTER         = 0xC41B,
  ZCL_OTAU_INVALID_STATE_QUERY_NEXT_IMAGE_RSP       = 0xC41C,
  ZCL_OTAU_INVALID_STATE_UPGRADE_END_RSP            = 0xC41D,
#if APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  ZCL_OTAU_INVALID_STATE_PAGE_REQ_TIMER_FIRED       = 0xC41F,
#endif // APP_SUPPORT_OTAU_PAGE_REQUEST == 1
  ZCL_OTAU_PDS_RESTORE_FAILED                       = 0xC423,
  ZCL_OTAU_INVALID_IMAGE_RECEIVED                   = 0xC424,
  ZCL_OTAU_DOWNLOAD_ABORTED                         = 0xC425,
  ZCL_OTAU_GET_QUEUE_ELEM_NULL_ON_QUERY_CB          = 0xC426,
  ZCL_OTAU_GET_QUEUE_ELEM_NULL_ON_UPGRADE_CB        = 0xC427,
  ZCL_OTAU_GET_QUEUE_ELEM_NULL_ON_PAGE_CB           = 0xC428,
  ZCL_OTAU_GET_QUEUE_ELEM_NULL_ON_BLOCK_CB          = 0xC429,
  ZCL_OTAU_INVALID_OTAURETRYCOUNT                   = 0xC42A,
  ZCL_OTAU_INVALID_NEXT_PDS_WRITE_OFFSET            = 0xC42B,
  ZCL_OTAU_INVALID_MIC_CRC                          = 0xC42C,
#endif // APP_USE_OTAU == 1

  // KE
  KE_WRONG_STATE_0                                  = 0xC500,
  KE_WRONG_STATE_1                                  = 0xC501,
  KE_WRONG_STATE_2                                  = 0xC502,
  KE_WRONG_STATE_3                                  = 0xC503,
  KE_WRONG_STATE_4                                  = 0xC504,
  KE_WRONG_STATE_5                                  = 0xC505,
  KE_WRONG_STATE_6                                  = 0xC506,
  KE_INIT_CLUSTER_0                                 = 0xC507,
  KE_INIT_CLUSTER_1                                 = 0xC508,
  ZCL_MEMORY_CORRUPTION_0                           = 0xC600,
  ZCL_MEMORY_CORRUPTION_1                           = 0xC601,
  ZCL_MEMORY_CORRUPTION_2                           = 0xC602,
  ZCL_MEMORY_CORRUPTION_3                           = 0xC603,

  ZCLZLLSCAN_ZCLZLLSCANREQ0                         = 0xC604,
  ZCLZLLNETWORK_ZCLZLLSTARTNETWORKREQ0              = 0xC605,
  // ZCL attributes operations
  ZCLPARSER_FORMWRITEATTRIBUTERESPONSE0             = 0xC700,
  ZCLPARSER_FORMREADATTRIBUTERESPONSE0              = 0xC701,

  ZCL_STARTSECURITY_NULLCALLBACK0                   = 0xC702,
  ZCL_ZLLRESETTOFN_NULLCALLBACK0                    = 0xC703,
  ZCL_ZLLSTARTNETWORK_NULLCALLBACK0                 = 0xC704,
  ZCL_ZLLSTARTNETWORK_NULLCALLBACK1                 = 0xC705,
  ZCL_ZLLSELECTDEVICE_NULLCALLBACK0                 = 0xC706,
  ZCL_ZLLSCAN_NULLCALLBACK0                         = 0xC707,
  ZCL_ZLLSCAN_NULLCALLBACK1                         = 0xC70C,
  ZCLMEMORYMANAGER_ZCLMMGETNEXTBUSYDESCRIPTOR_0     = 0xC709,
} ZclDbgCodeId_t;
#endif  //#ifndef _ZCLDBG_H

//eof zclDbg.h
