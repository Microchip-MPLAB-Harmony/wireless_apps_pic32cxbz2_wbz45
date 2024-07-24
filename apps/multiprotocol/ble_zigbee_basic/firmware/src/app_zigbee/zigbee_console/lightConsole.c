/*******************************************************************************
  Light console Source File

  Company:
    Microchip Technology Inc.

  File Name:
    lightConsole.c

  Summary:
    This file contains the Light console implementation.
	
  Description:
    This file contains the Light console implementation.
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
#if APP_ENABLE_CONSOLE == 1

/******************************************************************************
                    Includes section
******************************************************************************/
#include <app_zigbee/zigbee_console/console.h>
#include <app_zigbee/zigbee_console/consoleCmds.h>
#include <z3device/clusters/include/haClusters.h>
#include <z3device/common/include/z3Device.h>
#include <systemenvironment/include/sysUtils.h>
#include <pds/include/wlPdsMemIds.h>
#include <zdo/include/zdo.h>
#include <z3device/light/include/light.h>
#include <zcl/include/zcl.h>
#include <bdb/include/bdb.h>
#include <zllplatform/ZLL/N_LinkTarget/include/N_LinkTarget_Bindings.h>
#include <zllplatform/ZLL/N_LinkTarget/include/N_LinkTarget.h>
#include <bdb/include/bdbInstallCode.h>
#include <z3device/clusters/include/basicCluster.h>
#include <z3device/light/include/lightBasicCluster.h>
#include <z3device/light/include/lightGroupsCluster.h>
#include <z3device/light/include/lightIdentifyCluster.h>
#include <z3device/light/include/lightOnOffCluster.h>
#include <z3device/light/include/lightLevelControlCluster.h>
#include <z3device/light/include/lightCommissioningCluster.h>
#include <zcl/clusters/include/groupsCluster.h>
#include <zcl/clusters/include/identifyCluster.h>
#include <z3device/clusters/include/onOffCluster.h>
#include <z3device/clusters/include/commissioningCluster.h>


/******************************************************************************
                    Defines section
******************************************************************************/
extern BDB_InvokeCommissioningReq_t AppbdbCommissioningreq;
/******************************************************************************
                    Prototypes section
******************************************************************************/
static void processGetAppDeviceTypeCmd(const ScanValue_t *args);
static void processGetDeviceTypeCmd(const ScanValue_t *args);

#if BDB_COMMANDS_IN_CONSOLE == 1
static void processSetInstallCodeCmd(const ScanValue_t *args);
static void processSetInstallCodeDeviceCmd(const ScanValue_t *args);
#endif // #if BDB_COMMANDS_IN_CONSOLE == 1

#if ZCL_COMMANDS_IN_CONSOLE == 1
// groups cluster commands
static void processAddGroupCmd(const ScanValue_t *args);
static void processAddGroupIfIdentifyingCmd(const ScanValue_t *args);
static void processGetGroupMembershipCmd(const ScanValue_t *args);
static void processRemoveAllGroupsCmd(const ScanValue_t *args);
static void processRemoveGroupCmd(const ScanValue_t *args);
static void processViewGroupCmd(const ScanValue_t *args);
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
static void processResetToFactoryDefaultsCmd(const ScanValue_t *args);
#endif // ZLO_EXTRA_CLUSTERS_SUPPORT == 1
static void processReadOnOffAttrVal(const ScanValue_t *args);
static void processReadCurrentLevelAttrVal(const ScanValue_t *args);
static void processIdentifyCmd(const ScanValue_t *args);
static void processIdentifyQueryCmd(const ScanValue_t *args);
static void processTriggerEffectCmd(const ScanValue_t *args);
static void processReadOnOffAttrVal(const ScanValue_t *args);
static void processSendEndpointInfoCmd(const ScanValue_t *args);



#endif // #if ZCL_COMMANDS_IN_CONSOLE == 1

/******************************************************************************
                    Local variables section
******************************************************************************/
const ConsoleCommand_t helpCmds[] =
{
  {"help", "",processHelpCmd, ""},
#if ZDO_COMMANDS_IN_CONSOLE == 1  
  {"zdoHelp", "",processZdoHelpCmd, ""},
#endif
#if COMMISSIONING_COMMANDS_IN_CONSOLE == 1
  {"commissioningHelp", "",processCommissioningHelpCmd, ""},
#endif  
#if ZCL_COMMANDS_IN_CONSOLE == 1  
  {"zclHelp", "",processZclHelpCmd, ""},
#endif
#ifdef OTAU_CLIENT
  {"otauHelp", "",processOtauHelpCmd, ""},
#endif
#if APP_ZGP_DEVICE_TYPE >= APP_ZGP_DEVICE_TYPE_PROXY_BASIC
  {"zgpHelp", "",processZgpHelpCmd, ""},
#endif
  {0,0,0,0},
};

PROGMEM_DECLARE(ConsoleCommand_t commissioningHelpCmds)[]=
{
#if COMMISSIONING_COMMANDS_IN_CONSOLE == 1
  {"invokeCommissioning", "dd", processInvokeCommissioningCmd, "-> invokes commissioning [mode: 1-Touchlink: 2-Steering: 4-Formation: 8-F&B][gid]Invoke one Commissioning method at a time\r\n"},
  {"getAppDeviceType", "", processGetAppDeviceTypeCmd, ""},
  {"getDeviceType", "", processGetDeviceTypeCmd, ""},
  {"getExtAddr", "", processGetExtAddrCmd, ""},
  {"getNetworkAddress", "", processGetNetworkAddressCmd, ""},
  {"getChannel", "", processGetChannelCmd, ""},
  {"powerOff", "", processPseudoPowerOffCmd, ""},
  {"reset", "", processResetCmd, ""},
  {"resetToFN", "", processResetToFactoryFreshCmd, ""},
  {"setExtAddr", "dd",processSetExtAddr, "[upper][lower]\r\n"},
  {"setPrimaryChannelMask", "d", processSetPrimaryChannelMaskCmd, "[mask]\r\n"},
  {"setSecondaryChannelMask", "d", processSetSecondaryChannelMaskCmd, "[mask]\r\n"},
  {"setcca", "dd", processSetCCAModeAndThresholdCmd, "-> Sets CCA Mode and Threshold [cca mode] [threshold]\r\n"},
  {"getcca", "", processGetCCAModeAndThresholdCmd, "-> Gets CCA Mode and Threshold\r\n"},
#if BDB_COMMANDS_IN_CONSOLE == 1
  {"formAndSteer", "", processFormAndSteerCmd, ""},
  {"formSteerAndFB", "", processFormSteerAndFBCmd, ""},
  {"setAllowSteal", "d", processSetAllowStealCmd, "-> Set 0 or 1 <type>\r\n"},
  {"SetInstallCodeDevice", "s", processSetInstallCodeDeviceCmd, "[code]\r\n"},
  {"SetInstallCode", "dds", processSetInstallCodeCmd, "[extAddr][code]\r\n"},
#if BDB_TOUCHLINK_SUPPORT == 1
  {"setAllowTLResetToFN", "d", processSetAllowTLResetToFNCmd, ""},
#if BDB_TOUCHLINK_INITIATOR_SUPPORT == 1
  {"SetTLRole", "d", processSetTLRole, "Touchlink Role:1-Initiator,0-Target[initiator]"},
#endif
#endif
#if ZB_COMMISSIONING_ON_STARTUP == 0
  {"SetFBRole", "d", processSetFBRole, "FindingAndBinding Role 1-Initiator,0-Target[initiator]\r\n"},
#endif
  {"setTCLKExchangeMethod", "d", processSetTCLKExchangeMethodCmd, "[method]\r\n"},
  {"setTCLKMaxRetryAttempts", "d", processSetTCLKMaxRetryAttemptsCmd, "[attempt]\r\n"},
  {"setGlobalKey", "d", processsetGlobalKeyCmd, "[Option]\r\n"},
  {"setPermitJoin", "d", processSetPermitJoinCmd, "[dur]\r\n"},
#endif
#endif
   {0,0,0,0},
};

PROGMEM_DECLARE(ConsoleCommand_t zclHelpCmds)[]=
{
#if ZCL_COMMANDS_IN_CONSOLE == 1
  {"readAttribute", "sdddd", processReadAttrCmd, "[addrMode][addr][ep][clusterId][attrId]\r\n"},
  {"writeAttribute", "sddddddd", processWriteAttrCmd, "[addrMode][addr][ep][clusterId][attrId][type][attrValue][attrSize]\r\n"},
  {"writeAttributeNoResp", "sddddddd", processWriteAttrNoRespCmd, "[addrMode][addr][ep][clusterId][attrId][type][attrValue][attrSize]\r\n"},
  {"configureReporting", "sddddddd", processConfigureReportingCmd, "[addrMode][addr][ep][clusterId][attrId][type][min][max]\r\n"},
  {"startReporting", "", processStartReportingCmd, "Triggers Reporting\r\n"},
  {"configureReportingWRC", "sdddddddd", processConfigureReportingWithReportableChangeCmd, "With Reportable Change [addrMode][addr][ep][clusterId][attrId][type][min][max][repChange]\r\n"},
  {"addGroup", "sddd", processAddGroupCmd, "[addrMode][addr][ep][gid]\r\n"},
  {"addGroupIfIdentifying", "sddd", processAddGroupIfIdentifyingCmd,  "[addrMode][addr][ep][gid]\r\n"},
  {"getGroupMembership", "sdddddddd", processGetGroupMembershipCmd, "[addrMode][addr][ep][count][gid]*5\r\n"},
  {"removeAllGroups", "sdd", processRemoveAllGroupsCmd,"[addrMode][addr][ep]\r\n"},
  {"removeGroup", "sddd", processRemoveGroupCmd, "[addrMode][addr][ep][gid]\r\n"},
  {"viewGroup", "sddd", processViewGroupCmd, "[addrMode][addr][ep][gid]\r\n"},

#if BDB_COMMANDS_IN_CONSOLE == 1
#if BDB_TOUCHLINK_SUPPORT == 1
#if BDB_TOUCHLINK_INITIATOR_SUPPORT == 1
  {"resetTargetToFN", "", processTargetToFnCmd, ""},
#endif
  {"setTargetType", "d", processSetTargetTypeCmd, ""},
#endif
#endif
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
  {"resetToFactoryDefaults", "sdd", processResetToFactoryDefaultsCmd, "[addrMode][addr][ep]\r\n"},
#endif // #if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
  {"onOffQ", "", processReadOnOffAttrVal, ""},
  {"currentLevelQ", "", processReadCurrentLevelAttrVal, ""},
  {"identify", "sddd", processIdentifyCmd, "[addrMode][addr][ep][idTime]\r\n"},
  {"identifyQuery", "sdd", processIdentifyQueryCmd, "[addrMode][addr][ep]\r\n"},
  {"triggerEffect", "sdddd", processTriggerEffectCmd, "->Send TriggerEffect command: triggerEffect [addrMode][addr][ep][effectId][effectVariant]"},
  {"sendEndpointInfo", "dd", processSendEndpointInfoCmd, "[shortAddr][dstEp]\r\n"},

    
  #endif // #if ZCL_COMMANDS_IN_CONSOLE == 1
  {0,0,0,0},
};

/******************************************************************************
                   Global variables section
******************************************************************************/
bool fbRole = false;
ScanValue_t local;
uint8_t srcEp = APP_ENDPOINT_LIGHT;

/******************************************************************************
                    Implementation section
******************************************************************************/

/**************************************************************************//**
\brief Processes request for device type obtaining

\param[in] args - array of command arguments
******************************************************************************/
static void processGetDeviceTypeCmd(const ScanValue_t *args)
{
#if (APP_ZGP_DEVICE_TYPE == APP_ZGP_DEVICE_TYPE_COMBO_BASIC)
#if (APP_ENABLE_ZGP_CERTIFICATION_EXTENSION == 1)
  (void)appSnprintf("DeviceType = %d\r\n", TEST_DEVICE_TYPE_ZGP_TH);
#else
  (void)appSnprintf("DeviceType = %d\r\n", TEST_DEVICE_TYPE_ZGP_COMBO_BASIC);
#endif
#else
  (void)appSnprintf("DeviceType = %d\r\n", TEST_DEVICE_TYPE_ZIGBEE_ROUTER);
#endif
  (void)args;
}

/**************************************************************************//**
\brief Processes request for HA device type obtaining

\param[in] args - array of command arguments
+******************************************************************************/
static void processGetAppDeviceTypeCmd(const ScanValue_t *args)
{
  (void)appSnprintf("Z3DeviceType = 0x%04x\r\n", APP_Z3DEVICE_ID);
  (void)args;
}

#if ZCL_COMMANDS_IN_CONSOLE == 1
/**************************************************************************//**
\brief Processes Add group command

\param[in] args - array of command arguments
******************************************************************************/
static void processAddGroupCmd(const ScanValue_t *args)
{
  groupsSendAddGroup(determineAddressMode(args), args[1].uint16, args[2].uint8,srcEp,
    args[3].uint16);
}

/**************************************************************************//**
\brief Processes View group command

\param[in] args - array of command arguments
******************************************************************************/
static void processViewGroupCmd(const ScanValue_t *args)
{
  groupsSendViewGroup(determineAddressMode(args), args[1].uint16, args[2].uint8,
   srcEp,args[3].uint16);
}

/**************************************************************************//**
\brief Processes Get group membership command

\param[in] args - array of command arguments
******************************************************************************/
static void processGetGroupMembershipCmd(const ScanValue_t *args)
{
  uint16_t groupList[5U];

  for (uint8_t i = 0U; i < 5U; i++)
  {
    groupList[i] = args[i + 4U].uint16;
  }

  groupsSendGetGroupMembership(determineAddressMode(args), args[1].uint16, args[2].uint8,
   srcEp, args[3].uint8, groupList);
}

/**************************************************************************//**
\brief Processes Remove group command

\param[in] args - array of command arguments
******************************************************************************/
static void processRemoveGroupCmd(const ScanValue_t *args)
{
  groupsSendRemoveGroup(determineAddressMode(args), args[1].uint16, args[2].uint8,srcEp,
    args[3].uint16);
}

/**************************************************************************//**
\brief Processes Remove all groups command

\param[in] args - array of command arguments
******************************************************************************/
static void processRemoveAllGroupsCmd(const ScanValue_t *args)
{
  (void)args;
  groupsSendRemoveAllGroups(determineAddressMode(args), args[1].uint16, args[2].uint8, srcEp);
}

/**************************************************************************//**
\brief Processes Add group if identifying command

\param[in] args - array of command arguments
******************************************************************************/
static void processAddGroupIfIdentifyingCmd(const ScanValue_t *args)
{
  groupsSendAddGroupIfIdentifying(determineAddressMode(args), args[1].uint16, args[2].uint8, srcEp,
    args[3].uint16);
}
#endif //#if ZCL_COMMANDS_IN_CONSOLE == 1

#if COMMISSIONING_COMMANDS_IN_CONSOLE == 1
/*****************************************************************************/
//                      BASIC CLUSTER COMMANDS
/*****************************************************************************/
#if BDB_COMMANDS_IN_CONSOLE == 1

/**************************************************************************//**
\brief InstallCode callback

\param[in] status - status of action
******************************************************************************/
void myICCallback(InstallCode_Configuration_Status_t status)
{
  (void)appSnprintf("Status = %d\r\n", status);
}

/**************************************************************************//**
\brief Processes InstallCode command

\param[in] args - array of command arguments
******************************************************************************/
static void processSetInstallCodeCmd(const ScanValue_t *args)
{
  ExtAddr_t devAddr = 0xFFFFFFFFFFFFFFFFULL;
  uint8_t icode[18];
  (void)hexStrTouint8array(args[0].str, icode, 18U);
  BDB_ConfigureInstallCode(devAddr, icode, myICCallback);
  (void)args;
}

/**************************************************************************//**
\brief Processes InstallCode command

\param[in] args - array of command arguments
******************************************************************************/
static void processSetInstallCodeDeviceCmd(const ScanValue_t *args)
{
  ExtAddr_t devAddr = 0xFFFFFFFFFFFFFFFFULL;
  uint8_t icode[18];
  (void)hexStrTouint8array(args[0].str, icode, 18U);
  BDB_ConfigureInstallCode(devAddr, icode, myICCallback);
  (void)args;
}

#endif // #if BDB_COMMANDS_IN_CONSOLE == 1
#endif //#if COMMISSIONING_COMMANDS_IN_CONSOLE == 1

#if ZCL_COMMANDS_IN_CONSOLE == 1
#if ZLO_EXTRA_CLUSTERS_SUPPORT == 1

/**************************************************************************//**
\brief Processes reset To factory defaults command

\param[in] args - array of command arguments
******************************************************************************/
static void processResetToFactoryDefaultsCmd(const ScanValue_t *args)
{
  basicResetToFactoryDefaultsCommand(determineAddressMode(args), args[1].uint16,
    args[2].uint8, srcEp);
}

#endif // #if ZLO_EXTRA_CLUSTERS_SUPPORT == 1
/**************************************************************************//**
\brief Processes Identify command

\param[in] args - array of command arguments
******************************************************************************/
static void processIdentifyCmd(const ScanValue_t *args)
{
  identifySendIdentify(determineAddressMode(args), args[1].uint16, args[2].uint8, srcEp,
    args[3].uint16);
}

/**************************************************************************//**
\brief Processes Identify Query command

\param[in] args - array of command arguments
******************************************************************************/
static void processIdentifyQueryCmd(const ScanValue_t *args)
{
  identifySendIdentifyQuery(determineAddressMode(args), args[1].uint16,
    args[2].uint8, srcEp);
}
/**************************************************************************//**
\brief Processes Trigger Effect command

\param[in] args - array of command arguments
******************************************************************************/
static void processTriggerEffectCmd(const ScanValue_t *args)
{
  identifySendTriggerEffect(determineAddressMode(args), args[1].uint16, args[2].uint8, srcEp,
    args[3].uint8, args[4].uint8);
}

/**************************************************************************//**
\brief Processes Read OnOff Attr Val command

\param[in] args - array of command arguments
******************************************************************************/
static void processReadOnOffAttrVal(const ScanValue_t *args)
{
  (void)appSnprintf("%d\r\n", lightOnOffClusterServerAttributes.onOff.value);
}
/**************************************************************************//**
\brief Processes Read Current Level Attr Val command

\param[in] args - array of command arguments
******************************************************************************/
static void processReadCurrentLevelAttrVal(const ScanValue_t *args)
{
  (void)appSnprintf("%d\r\n", lightLevelControlClusterServerAttributes.currentLevel.value);
}

/**************************************************************************//**
\brief Processes Send Endpoint Info command

\param[in] args - array of command arguments
******************************************************************************/
static void processSendEndpointInfoCmd(const ScanValue_t *args)
{
  commissioningSendEndpointInformation(args[0].uint16, args[1].uint8, srcEp);
}


#endif // #if ZCL_COMMANDS_IN_CONSOLE == 1

#endif // APP_ENABLE_CONSOLE == 1

#endif // APP_DEVICE_TYPE_DIMMABLE_LIGHT

// eof dlConsole.c
