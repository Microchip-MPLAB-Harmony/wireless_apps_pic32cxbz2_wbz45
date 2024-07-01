/*******************************************************************************
  ZCL ZCC CUSTOM Cluster Header File

  Company:
    Microchip Technology Inc.

  File Name:
    zclFlowratemonitorCluster.h

  Summary:
    The header file describes the ZCL CUSTOM Cluster and its interface.

  Description:
    The file describes the types and interface of the ZCL CUSTOM Cluster.
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

#ifndef ZCLFLOWRATEMONITOR_H
#define ZCLFLOWRATEMONITOR_H

/*******************************************************************************
                   Includes section
*******************************************************************************/

#include <zcl/include/zcl.h>
#include <zcl/include/clusters.h>
/******************************************************************************
                    Define(s) section
******************************************************************************/
    
//ATTRIBUTE DEFINITION SECTION


#define FLOWRATEMONITOR_CLUSTER_ID                                                     0xfc00

#define ZCL_FLOWRATEMONITOR_SERVER_ATTRIBUTES_AMOUNT                      7
#define ZCL_FLOWRATEMONITOR_SERVER_COMMANDS_AMOUNT                        0 

#define ZCL_FLOWRATEMONITOR_CLIENT_ATTRIBUTES_AMOUNT                      1
#define ZCL_FLOWRATEMONITOR_CLIENT_COMMANDS_AMOUNT                        0

//Attribute IDs
#define ZCL_FLOWRATEMONITOR_VOLUMETRICRATE_SERVER_ATTRIBUTE_ID                   CCPU_TO_LE16(0X0)
#define ZCL_FLOWRATEMONITOR_TEMPERATURE_SERVER_ATTRIBUTE_ID                      CCPU_TO_LE16(0X1)
#define ZCL_FLOWRATEMONITOR_PRESSURE_SERVER_ATTRIBUTE_ID                         CCPU_TO_LE16(0X2)
#define ZCL_FLOWRATEMONITOR_VELOCITY_SERVER_ATTRIBUTE_ID                         CCPU_TO_LE16(0X3)
#define ZCL_FLOWRATEMONITOR_DENSITY_SERVER_ATTRIBUTE_ID                          CCPU_TO_LE16(0X4)
#define ZCL_FLOWRATEMONITOR_VIBRATION_SERVER_ATTRIBUTE_ID                        CCPU_TO_LE16(0X5)
#define ZCL_FLOWRATEMONITOR_CLUSTERREVISION_SERVER_ATTRIBUTE_ID                  CCPU_TO_LE16(0Xfffd)

#define ZCL_FLOWRATEMONITOR_CLUSTERREVISION_CLIENT_ATTRIBUTE_ID                  CCPU_TO_LE16(0X0)

#define ZCL_DEFINE_FLOWRATEMONITOR_CLUSTER_SERVER_ATTRIBUTES(min,max) \
        DEFINE_REPORTABLE_ATTRIBUTE(Volumetricrate, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_VOLUMETRICRATE_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID, min, max) , \
        DEFINE_ATTRIBUTE(Temperature, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_TEMPERATURE_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID) , \
        DEFINE_ATTRIBUTE(Pressure, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_PRESSURE_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID) , \
        DEFINE_ATTRIBUTE(Velocity, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_VELOCITY_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID) , \
        DEFINE_ATTRIBUTE(Density, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_DENSITY_SERVER_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID) , \
        DEFINE_ATTRIBUTE(Vibration, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_VIBRATION_SERVER_ATTRIBUTE_ID, ZCL_S16BIT_DATA_TYPE_ID) , \
        DEFINE_ATTRIBUTE(Clusterrevision, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_CLUSTERREVISION_SERVER_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID) 

#define ZCL_DEFINE_FLOWRATEMONITOR_CLUSTER_CLIENT_ATTRIBUTES() \
        DEFINE_ATTRIBUTE(Clusterrevision, ZCL_READONLY_ATTRIBUTE, ZCL_FLOWRATEMONITOR_CLUSTERREVISION_CLIENT_ATTRIBUTE_ID, ZCL_U16BIT_DATA_TYPE_ID) 



#define FLOWRATEMONITOR_CLUSTER_ZCL_SERVER_CLUSTER_TYPE(clattributes, clcommands)                                                \
  {                                                                                                              \
    .id = FLOWRATEMONITOR_CLUSTER_ID,                                                                                      \
    .options = {.type = ZCL_SERVER_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY},                  \
    .attributesAmount = ZCL_FLOWRATEMONITOR_SERVER_ATTRIBUTES_AMOUNT,                                              \
    .attributes = (uint8_t *) clattributes,                                                                      \
    .commandsAmount = ZCL_FLOWRATEMONITOR_SERVER_COMMANDS_AMOUNT,                                                  \
    .commands = (uint8_t *)clcommands                                                                            \
  }

#define FLOWRATEMONITOR_CLUSTER_ZCL_CLIENT_CLUSTER_TYPE(clattributes, clcommands)                                                \
  {                                                                                                              \
    .id = FLOWRATEMONITOR_CLUSTER_ID,                                                                                      \
    .options = {.type = ZCL_CLIENT_CLUSTER_TYPE, .security = ZCL_NETWORK_KEY_CLUSTER_SECURITY},                  \
    .attributesAmount = ZCL_FLOWRATEMONITOR_CLIENT_ATTRIBUTES_AMOUNT,                                              \
    .attributes = (uint8_t *) clattributes,                                                                      \
    .commandsAmount = ZCL_FLOWRATEMONITOR_CLIENT_COMMANDS_AMOUNT,                                                  \
    .commands = (uint8_t *)clcommands                                                                            \
  }


  #define DEFINE_FLOWRATEMONITOR_CLUSTER(cltype, clattributes, clcommands) \
    FLOWRATEMONITOR_CLUSTER_##cltype(clattributes, clcommands)


// ATTRIBUTE DEFINITION END
//COMMAND DEFINITION SECTION

//Server Commands

//Client Commands

#define ZCL_DEFINE_FLOWRATEMONITOR_CLUSTER_COMMANDS()
    


/******************************************************************************
                    Types section
******************************************************************************/
//ATTRIBUTE's TYPES SECTION


typedef struct PACK
{
struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    int16_t             value;
    ZCL_ReportTime_t    reportCounter;     //!<For internal use only
    ZCL_ReportTime_t    minReportInterval; //!<Minimum reporting interval field value
    ZCL_ReportTime_t    maxReportInterval; //!<Maximum reporting interval field value
    int16_t              reportableChange;  //!<Reporting change field value
    ZCL_ReportTime_t    timeoutPeriod;     //!<Timeout period field value
    int16_t              lastReportedValue; //!<Last reported value
   } Volumetricrate;

struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    int16_t             value;
   } Temperature;

struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    int16_t             value;
   } Pressure;

struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    int16_t             value;
   } Velocity;

struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint16_t            value;
   } Density;

struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    int16_t             value;
   } Vibration;

struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint16_t            value;
   } Clusterrevision;

}ZCL_FlowratemonitorClusterServerAttributes_t;

typedef struct PACK
{
struct PACK
  {
    ZCL_AttributeId_t   id;
    uint8_t             type;
    uint8_t             properties;
    uint16_t            value;
   } Clusterrevision;

}ZCL_FlowratemonitorClusterClientAttributes_t;


// COMMANDS TYPE DEFINITION




typedef struct
{


} ZCL_FlowratemonitorClusterCommands_t;

#endif
