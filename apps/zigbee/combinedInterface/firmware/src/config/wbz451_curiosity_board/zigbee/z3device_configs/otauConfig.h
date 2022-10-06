/*******************************************************************************
  OTAU Configuration File

  Company:
    Microchip Technology Inc.

  File Name:
    otauConfig.h

  Summary:
    This file contains Zigbee OTAU Configuration.

  Description:
    This file contains Zigbee OTAU Configuration.
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

#ifndef _OTAU_CONFIG_H_
#define _OTAU_CONFIG_H_

#include <zigbee/z3device_configs/zigbeeAppConfig.h>
//-----------------------------------------------
// Includes OTAU specific Configurations.
//-----------------------------------------------

//-----------------------------------------------
//APP_USE_OTAU == 1
//-----------------------------------------------
#if (APP_USE_OTAU == 1)

#define APP_DEVICE_OTAU_LOGGING 1
// #define APP_DEVICE_OTAU_LOGGING 0

#define ZCL_SUPPORT_ENABLE

// This is the default minimum delay that the server wants the client to wait
// between subsequent block requests in milliseconds.
// 
//  The parameter is valid only for OTAU .
// 
//  Value range: any 16-bit value
//  C-type: uint16_t
//  Can be set: By Server
//  Persistent: Yes
#define ZCL_OTAU_DEFAULT_BLOCK_REQ_PERIOD 1000

#if (APP_Z3_DEVICE_TYPE ==  APP_DEVICE_TYPE_COMBINED_INTERFACE)

    #define OTAU_SERVER     // OTAU device role
#else
    // OTAU device role
    #define OTAU_CLIENT

#endif

#ifdef OTAU_SERVER

    // Enable or disable console tunneling support in image store driver. Application
    // can send date through tunnel in parallel to ISD driver
    #define APP_USE_ISD_CONSOLE_TUNNELING     1
    //#define APP_USE_ISD_CONSOLE_TUNNELING   0
    
#else 

    // Use fake OFD driver instead of real one. This option is useful for evaluation of OTA driver
    // OTA Upgrade feature on boards without external flash.
    #define APP_USE_FAKE_OFD_DRIVER 0
    // #define APP_USE_FAKE_OFD_DRIVER 1

    //#define OTA_IMAGE_CBC_KEY {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99}
    #define OTA_IMAGE_CBC_KEY {0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89, 0x90, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44}

    #define OTA_IMAGE_CBC_IV {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    
    // Support OTAU image page request features
    #define APP_SUPPORT_OTAU_PAGE_REQUEST 0

    // The recovery pds writes happens for every OTAU_RECOVERY_BLOCK_SIZE_MIN to avoid frequent writing of flash.
    // This increases the endurance of the flash
    #define OTAU_RECOVERY_BLOCK_SIZE_MIN    (NVM_PAGE_SIZE * 4) // 16 KB

    // The amount of servers the OTAU client can listen to during upgrade server
    // discovery
    // 
    //  The OTAU client can store information about several discovered OTAU servers.
    // However, the client device tries to connect to discovered servers one by one
    // until the first successful attempt and then communicates and uploads the whole
    // image from the first suitable server.
    // 
    //  The parameter is valid for OTAU clients only.
    // 
    //  Value range: at least 1
    //  C-type: uint8_t
    //  Can be set: at compile time only
    //  Persistent: No
    #define CS_ZCL_OTAU_DISCOVERED_SERVER_AMOUNT 1
  
    // The interval in milliseconds between two attempts to find an upgrade server
    // 
    //  The parameter is valid for OTAU clients only.
    // 
    //  Value range: any 32-bit value
    //  C-type: uint32_t
    //  Can be set: at any time before an OTAU start
    //  Persistent: No
    #define CS_ZCL_OTAU_SERVER_DISCOVERY_PERIOD 60000

    // The interval in milliseconds between two successful attempts to query the server
    // 
    //  The parameter is valid only for OTAU clients.
    // 
    //  Value range: any 32-bit value
    //  C-type: uint32_t
    //  Can be set: at any time before an OTAU start
    //  Persistent: No
    #define CS_ZCL_OTAU_QUERY_INTERVAL 10000

    // Max retry count for commands (OTAU cluster, ZDO and APS) used for OTAU
    // 
    //  The parameter is valid only for OTAU clients.
    // 
    //  Value range: any 8-bit value
    //  C-type: uint8_t
    //  Can be set: at any time before an OTAU start
    //  Persistent: No
    #define CS_ZCL_OTAU_MAX_RETRY_COUNT 3

    // The file version indicates the image version that the client is required to
    // install.
    // 
    //  The file version represents the release and build number of the image
    // application and stack.The application release and build numbers are manufacturer
    // specific.
    // 
    //  The parameter is valid only for OTAU clients.
    // 
    //  Value range: any 32-bit value
    //  C-type: uint32_t
    //  Can be set: at any time before an OTAU start
    //  Persistent: No
    #define CS_ZCL_OTAU_FILE_VERSION CS_STACK_VERSION_VALUE

    // The default address of an upgrade server
    // 
    //  The parameter indicates how the OTAU client will search for OTAU servers in the
    // network. If one of broadcast addresses is specified, the client will attempt to
    // find all devices supporting the OTAU server cluster and will request new images
    // from the first server that will respond. Otherwise, the client will try to
    // connect to a particular device with the specified extended address.
    // 
    //  The parameter is valid for OTAU clients only.
    // 
    //  Value range: any 64-bit value:
    //  0x0000000000000000ull, 0xFFFFFFFFFFFFFFFFull - a server discovery request is
    // broadcast
    //  otherwise, the client tries to connect to a particular node
    // 
    // 
    //  C-type: ExtAddr_t
    //  Can be set: at any time before an OTAU start
    //  Persistent: No
    #define CS_ZCL_OTAU_DEFAULT_UPGRADE_SERVER_IEEE_ADDRESS 0xFFFFFFFFFFFFFFFFull
    
  // The default Encryption method to use for Image upgrade
  // 
  //  The parameter indicates
  // 
  //  The parameter is valid for OTAU clients only.
  // 

  // 
  //  C-type: ZCL_OtauImageType_t
  //  Can be set: at any time before an OTAU start
  //  Persistent: Yes

    /********************************************************/
    /*! On/Off Light */
#define OTA_ON_OFF_LIGHT_IMAGE_TYPE                            0x0100
    /*! Dimmable Light */
#define OTA_DIMMABLE_LIGHT_IMAGE_TYPE                          0x0101
    /*! Color Dimmable Light */
#define OTA_COLOR_DIMMABLE_LIGHT_IMAGE_TYPE                    0x0102
    /*! On/Off Light Switch */
#define OTA_ON_OFF_LIGHT_SWITCH_IMAGE_TYPE                     0x0103
    /*! Dimmer Switch */
#define OTA_DIMMER_SWITCH_IMAGE_TYPE                           0x0104
    /*! Color Dimmer Switch */
#define OTA_COLOR_DIMMER_SWITCH_IMAGE_TYPE                     0x0105
    /*! Light Sensor */
#define OTA_LIGHT_SENSOR_IMAGE_TYPE                            0x0106
    /*! Occupancy Sensor */
#define OTA_OCCUPANCY_SENSOR_IMAGE_TYPE                        0x0107
    /**! On/Off Ballast */
#define OTA_ON_OFF_BALLAST_IMAGE_TYPE                          0x0108
    /**! Dimmable Ballast */
#define OTA_DIMMABLE_BALLAST_IMAGE_TYPE                        0x0109
    /*! On/off Plug-in unit */
#define OTA_ON_OFF_PLUG_IN_UNIT_IMAGE_TYPE                     0x010A
    /*! Dimmable Plug-in unit */
#define OTA_DIMMABLE_PLUG_IN_UNIT_IMAGE_TYPE                   0x010B
    /*! Color Temperature light */
#define OTA_COLOR_TEMPERATURE_LIGHT_IMAGE_TYPE                 0x010C
    /*! Extended Color light */
#define OTA_EXTENDED_COLOR_LIGHT_IMAGE_TYPE                    0x010D
    /*! Light level Sensor */
#define OTA_LIGHT_LEVEL_SENSOR_IMAGE_TYPE                      0x010E
    /*! Color Controller */
#define OTA_COLOR_CONTROLLER_IMAGE_TYPE                        0x0800
    /*! Color Scene Controller */
#define OTA_COLOR_SCENE_CONTROLLER_IMAGE_TYPE                  0x0810
    /*! Non Color Controller */
#define OTA_NON_COLOR_CONTROLLER_IMAGE_TYPE                    0x0820
    /*! Non Color Scene Controller */
#define OTA_NON_COLOR_SCENE_CONTROLLER_IMAGE_TYPE              0x0830
    /*! Control Bridge */
#define OTA_CONTROL_BRIDGE_IMAGE_TYPE                          0x0840
    /*! On/off Sensor */
#define OTA_ON_OFF_SENSOR_IMAGE_TYPE                           0x0850
      /*! ias_ace device id  */
#define OTA_IAS_ACE_IMAGE_TYPE                                 0x0401
      /*! custom device id  */
#define OTA_CUSTOM_IMAGE_TYPE                                  0x0F00

    /********************************************************/

    #define OTAU_APP_IMAGE_TYPE    OTA_EXTENDED_COLOR_LIGHT_IMAGE_TYPE

#if (APP_SUPPORT_OTAU_PAGE_REQUEST == 1)
    // Indicates that image page request are used to load an image
    // 
    //  If the parameter is set to 1 the OTAU client will use image page requests
    // (rather than image block requests) to load an application image from the OTAU
    // server. In this case the server will send a whole page (consisting of multiple
    // blocks) without requesting an APS acknowledgement for each block. The client
    // marks not-delivered blocks and sends an image block request for each of missed
    // blocks after the transmission on the page finishes.
    // 
    //  If the parameter is set to 0 image block requests will be used. The APS
    // acknowledgement is sent for each of received blocks. If any of the blocks is
    // missing loading of the image is restarted.
    // 
    //  The parameter is valid only for OTAU clients.
    // 
    //  Value range: 1 or 0
    //  C-type: uint8_t
    //  Can be set: at compile time only
    //  Persistent: No
    #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE 1
    // Period in milliseconds between two image block responses sent from the OTAU
    // server
    // 
    //  The parameter indicates how fast the OTAU server shall send data (via the image
    // block response command) to the OTAU client. The value is set on the client. The
    // server receives the value from the client. The server shall wait for, at
    // minimum, the period specified in the parameter before sending another block to
    // the client.
    // 
    //  The parameter is valid only for OTAU clients.
    // 
    //  Value range: not less than 200ms (according to the OTAU specification)
    //  C-type: uint16_t
    //  Can be set: at compile time only
    //  Persistent: No
    #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_RESPONSE_SPACING 200

#else
    #define CS_ZCL_OTAU_IMAGE_PAGE_REQUEST_ENABLE 0

#endif //#if (APP_SUPPORT_OTAU_PAGE_REQUEST == 1)

#endif //#if (APP_Z3_DEVICE_TYPE ==  APP_DEVICE_TYPE_COMBINED_INTERFACE)
#endif //#if (APP_USE_OTAU == 1)

#endif // _OTAU_CONFIG_H_
