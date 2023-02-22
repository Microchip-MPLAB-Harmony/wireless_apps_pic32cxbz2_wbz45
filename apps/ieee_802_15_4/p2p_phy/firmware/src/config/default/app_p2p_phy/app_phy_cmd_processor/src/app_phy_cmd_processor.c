/* ************************************************************************** */
/** Descriptive File Name

  @Company
 Microchip Technology pvt Ltd

  @File Name
    PHY_cmd_processor.c

  @Summary
    Command Interface file for PHY & App layer functions

  @Description
    Command Interface file for PHY & App layer functions
 */
 /*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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
// *****************************************************************************
// *****************************************************************************
#include <errno.h>
#include <limits.h>
#include "definitions.h"

//SYS_TIME_RESULT timerDestroyDataModeStatus, timerDestroyContTestModeStatus, timerDestroyPeriodicTestModeStatus;
SYSTIMER_FLAGS sysTimerFlag;
MODE_SWITCH_FLAGS modeSwitchFlag;
static APPL_DATA_MODE appDataMode;
static APPL_PERIOD_TEST_MODE appPeriodTestMode;
static APPL_CNT_TEST_MODE appContTestMode;
static APPL_PER_TEST_MODE appPerTestMode;
static APPL_THROUGHPUT_TEST_MODE appThroughputTestMode;
//PHY Commands definitions
static void phy_init(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_reset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_edstart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibget(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_rxenable(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_trxsleep(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_ccaperform(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void update_device_table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void read_device_table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_trxwakeup(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void exit_data_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void data_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void periodic_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void continuous_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void PER_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void promiscuous_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void throughput_measure(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetChannel(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetChannel(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetChannelPage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetChannelPage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetPanId(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetPanId(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetCsmaMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetCsmaMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetTxPwr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetTxPwr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetIeeeAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetIeeeAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetFrameRetry(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetFrameRetry(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void stop_active_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetSrcAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetSrcAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void getAddressingModes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void setAddressingModes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void getFrameTypes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void setFrameTypes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibsetExtDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_pibgetExtDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void selectDestDevice(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_StartContinuousTransmit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_StopContinuousTransmit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_ConfigAutoAck(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_GetRSSIBaseVal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_GetTrxStatus(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_ConfigRxSensitivity(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_ConfigTxPwr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_ConfigAntennaDiversity(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void phy_ConfigRxRPCMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void update_PER_test_packets(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
static void throughput_timer_update(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);
//CMD descriptor table definition
static const SYS_CMD_DESCRIPTOR PhyCmdsTbl[] =
{
    {"phyInit", phy_init, ":Initialize PHY: phyInit"},
    {"phyReset", phy_reset, ":Reset the PHY state machine: phyReset,<set default PIB values-0/1 : R,1/0>\r\n"},
    {"phyEdScan", phy_edstart, ":ED start: phyEdScan,<set scan duration ranging from 1-14>\r\n"},
    {"getAttribute", phy_pibget, ":Get PHY PIB attribute: getAttribute,<attribute id>\r\n"},
    {"setAttribute", phy_pibset, ":Set PHY PIB attribute: setAttribute,<attribute id>,<attribute value>\r\n"},
    {"getChannel", phy_pibgetChannel, ":Get PHY PIB attribute: getChannel\r\n"},
    {"setChannel", phy_pibsetChannel, ":Set PHY PIB attribute: setChannel,<channel>\r\n"},
    {"getChannelPg", phy_pibgetChannelPage, ":Get PHY PIB attribute: getChannelPg\r\n"},
    {"setChannelPg", phy_pibsetChannelPage, ":Set PHY PIB attribute: setChannelPg,<channel page>\r\n"},
    {"getPanId", phy_pibgetPanId, ":Get PHY PIB attribute: getPanId\r\n"},
    {"setPanId", phy_pibsetPanId, ":Set PHY PIB attribute: setPanId,<PAN ID>\r\n"},
    {"getDestAddr", phy_pibgetDestAddr, ":Get PHY PIB attribute: getDestAddr\r\n"},
    {"setDestAddr", phy_pibsetDestAddr, ":Set PHY PIB attribute: setDestAddr,<dest address>\r\n"},
    {"getExtDestAddr", phy_pibgetExtDestAddr, ":Get PHY PIB attribute: getExtDestAddr\r\n"},
    {"setExtDestAddr", phy_pibsetExtDestAddr, ":Set PHY PIB attribute: setExtDestAddr,<Ext dest address 64 bit>\r\n"},
    {"getSrcAddr", phy_pibgetSrcAddr, ":Get PHY PIB attribute: getDestAddr\r\n"},
    {"setSrcAddr", phy_pibsetSrcAddr, ":Set PHY PIB attribute: setDestAddr,<src address>\r\n"},
    {"getCsmaMode", phy_pibgetCsmaMode, ":Get PHY PIB attribute: getCsmaMode\r\n"},
    {"setCsmaMode", phy_pibsetCsmaMode, ":Set PHY PIB attribute: setCsmaMode,<mode>\r\n"},
    {"getTxPwr", phy_pibgetTxPwr, ":Get PHY PIB attribute: getTxPwr\r\n"},
    {"setTxPwr", phy_pibsetTxPwr, ":Set PHY PIB attribute: setTxPwr,<tx pwr>\r\n"},
    {"getIeeeAddr", phy_pibgetIeeeAddr, ":Get PHY PIB attribute: getIeeeAddr\r\n"},
    {"setIeeeAddr", phy_pibsetIeeeAddr, ":Set PHY PIB attribute: setIeeeAddr,<Ieee address 64 bit>\r\n"},
    {"getFrameRetry", phy_pibgetFrameRetry, ":Get PHY PIB attribute: getFrameRetry\r\n"},
    {"setFrameRetry", phy_pibsetFrameRetry, ":Set PHY PIB attribute: setFrameRetry,<No of retries>\r\n"},
    {"rxEnable", phy_rxenable, ":RX ENABLE: rxEnable,<Set TRX states PHY_STATE_RX_ON/PHY_STATE_TRX_OFF: 1/0>\r\n"},
    {"trxSleep",phy_trxsleep, ":Set TRX to sleep/deep sleep: trxSleep,<sleep mode - slp mode 1/deep sleep - 0/1>\r\n"},
    {"trxWakeup",phy_trxwakeup, ":Set TRX to Wakeup: trxWakeup\r\n"},
    {"CCA",phy_ccaperform, "Perform CCA on desired channel : CCA,<phychannel: 11-26>\r\n"},    
    {"updateDeviceTable",update_device_table, "update device table : updateDeviceTable,<Parameter: device index,short address(16 bit), Ieee address(64 bit)>\r\n"},   
    {"readDeviceTable",read_device_table, "read device table : readDeviceTable,<Parameter: device index -- F/index>\r\n"},
    {"&&&", exit_data_mode, ":Switch to command mode: &&&\r\n"},
    {"+++", data_mode, ":switch to User Input data mode: +++,<time period in us>\r\n"}, 
    {"periodTestMode", periodic_test_mode, ":switch to periodic test mode: periodTestMode,<timer period in us>\r\n"}, 
    {"stopTestMode",stop_active_test_mode, "Stop all active application test modes: stopPeriodTestMode\r\n "},
    {"contTestMode", continuous_test_mode, ":switch to continuous test mode: contTestMode\r\n"}, 
    {"perTestMode", PER_test_mode, ":switch to PER test mode: perTestMode\r\n"}, 
    {"promTestMode", promiscuous_mode, ":switch to promiscuous test mode: PromTestMode\r\n"}, 
    {"throughputTestMode", throughput_measure, ":switch to throughput test mode: throughputTestMode\r\n"},  
    {"getAddrModes", getAddressingModes, ":get source and destination addressing modes: getAddrModes\r\n"},
    {"setAddrModes", setAddressingModes, ":set source and destination addressing modes: setAddrModes,<src addr mode:0-3>,<dest addr mode:0-3>\r\n"},
    {"getFrameType", getFrameTypes, ":get frame type: getFrameType"},
    {"setFrameType", setFrameTypes, ":set frame type: setFrameType,<Range: 0-5>"},
    {"selectDstDev", selectDestDevice, ":select destination device from device table: selectDstDevice,<Device table Index: 0-NUM_OF_DEVICES>\r\n"},
    {"phyCntTx", phy_StartContinuousTransmit, ":Starts continuous transmission on current channel: phyCntTx,<txMode>\r\n"},
    {"stopPhyCntTx", phy_StopContinuousTransmit, ":Stops continuous transmission on current channel: stopPhyCntTx\r\n"},
    {"ConfigRxRPCMode", phy_ConfigRxRPCMode, "Configures the reduced power consumption mode: ConfigRxRPCMode,<0x01 - To Enable the rxRPC mode, 0x00 - To disable the rx RPC mode>\r\n" },
    {"configAutoAck",phy_ConfigAutoAck,"Configures TRX for auto acknowledging the reserved frame: configAutoAck,<enableAACK-true/false>\r\n"},
    {"getRSSIBaseVal",phy_GetRSSIBaseVal,"get the base RSSI value for respective radios:getRSSIBaseVal\r\n"},
    {"getTrxStatus",phy_GetTrxStatus,"get the status of the transceiver:getTrxStatus\r\n"},
    {"configRxSensitivity",phy_ConfigRxSensitivity,"Configures receiver sensitivity level. This is used to desensitize the receiver:configRxSensitivity,<pdtlevel->0-15>\r\n"},
    {"configTxPwr",phy_ConfigTxPwr,"configure the TX Power\r\n"},
    {"configAntennaDiversity",phy_ConfigAntennaDiversity,"Configure the Antenna Diversity\r\n"},
    {"updateThroughputTime",throughput_timer_update,"Update Throughput Timer\r\n"},
    {"updatePERTestPacketsCnt",update_PER_test_packets,"Update PER test Packets\r\n"},
};


//PHY commands init - registering the command table to sys commands
void PhyCmdTable_Init(void)
{
    if(SYS_CMD_ADDGRP(PhyCmdsTbl, (int)(sizeof(PhyCmdsTbl)/sizeof(*PhyCmdsTbl)), "PhyCmds", ": PHY Layer commands"))
    {
        //SYS_CONSOLE_MESSAGE("\r\nSYS CMD TBL INITIALISED\r\n");
    }   
}

static void exit_data_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    SYS_CONSOLE_PRINT("\r\n CMD>\r\n");
}

static void data_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    if(argc<2)
    {
        SYS_CONSOLE_PRINT("\r\n Missing Parameter \r\n");
    }
    else
    {
        if(appPhyCmdProcessor_StrToUint32(argv[1],&sysTimerFlag.singleShotSysTimer))
        {
            dataModeTimerStartFlag = false;
            app_P2P_Phy_stopActiveModes();
            appPhyCmdProcessor_SetModeFlags(true,false,false,false,false,false,false);
            PAL_TimerDelay(1500000);
            appDataMode.devPerformanceParam.isModeActive = true;
            app_P2P_Phy_appModeSwitchHandler();
        } 
    }
}


static void periodic_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    if(argc < 2)
    {
        SYS_CONSOLE_PRINT("\r\n Missing Parameter \r\n");
    }
    else
    {
        if(appPhyCmdProcessor_StrToUint32(argv[1],&sysTimerFlag.periodicSysTimer))
        {
            SYS_CONSOLE_PRINT("\r\n Timer set to %u\r\n",sysTimerFlag.periodicSysTimer);
            app_P2P_Phy_stopActiveModes();
            appPhyCmdProcessor_SetModeFlags(false,true,false,false,false,false,false);
            PAL_TimerDelay(1500000);
            appPeriodTestMode.devPerformanceParam.isModeActive = true;
            app_P2P_Phy_appModeSwitchHandler();
        }
    }   
}

static void continuous_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    app_P2P_Phy_stopActiveModes();
    appPhyCmdProcessor_SetModeFlags(false,false,true,false,false,false,false);
    PAL_TimerDelay(1500000);
    appContTestMode.devPerformanceParam.isModeActive = true;
    SYS_CONSOLE_PRINT("\r\n CONTINUOUS TEST MODE \r\n");
    app_P2P_Phy_appModeSwitchHandler();

}

static void PER_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    SYS_CONSOLE_PRINT("\r\n PER TEST MODE\r\n");
    app_P2P_Phy_stopActiveModes();
    appPhyCmdProcessor_SetModeFlags(false,false,false,false,true,false,false);
    PAL_TimerDelay(1500000);
    appPerTestMode.devPerformanceParam.isModeActive = true;
    app_P2P_Phy_appModeSwitchHandler();
}

static void update_PER_test_packets(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appPhyCmdProcessor_StrToUint64(argv[1],&appNwkParam.nPerTestPackets))
    {
        SYS_CONSOLE_PRINT("\r\n No. of packets in PER test Mode set to %u\r\n",appNwkParam.nPerTestPackets);
    }
}

static void promiscuous_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    OSAL_RESULT osalResult;
    APP_Msg_T *p_appModes;
    APP_Msg_T appModes;
    p_appModes = &appModes;
    SYS_CONSOLE_PRINT("\r\n PROMISCUOUS TEST MODE\r\n");
    p_appModes->msgId = (uint8_t)APP_PROMISCUOUS_MODE;
    osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    osalResult = osalResult;
}

static void throughput_measure(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t temp = 0;
    app_P2P_Phy_stopActiveModes();
    appPhyCmdProcessor_SetModeFlags(false,false,false,true,false,false,false);
    PAL_TimerDelay(1500000);
    appThroughputTestMode.devPerformanceParam.isModeActive = true;
    // sysTimerFlag.throughputSysTimer = 1000000U;   //1sec   
    SYS_CONSOLE_PRINT("\r\n Throughput Test Mode\r\n");
    sysTimerFlag.sysTimerThroughputHandle = SYS_TIME_CallbackRegisterUS(&app_P2P_Phy_systimerCallbackThroughputTestMode, (uintptr_t)&temp, sysTimerFlag.throughputSysTimer, SYS_TIME_SINGLE);
    if(sysTimerFlag.sysTimerThroughputHandle != SYS_TIME_HANDLE_INVALID)
    {
        app_P2P_Phy_appModeSwitchHandler();
    }
    else
    {
        SYS_CONSOLE_MESSAGE("\r\n Timer Not Started\r\n");
    }

}

static void throughput_timer_update(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appPhyCmdProcessor_StrToUint32(argv[1],&sysTimerFlag.throughputSysTimer))
    {
        SYS_CONSOLE_PRINT("\r\n Throughput Timer set to %u usecs\r\n",sysTimerFlag.throughputSysTimer);
    }
}

static void update_device_table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{ 
    uint8_t retcode, dev_index = 0;
    uint16_t short_addr = 0;
    uint64_t ieeeAddr = 0;
    if(argc<4)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    
    if(appPhyCmdProcessor_StrToUint8(argv[1], &dev_index))
    {
        if((dev_index < 1U) || (dev_index > NUM_OF_DEVICES))
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
        SYS_CONSOLE_PRINT("\r\n Device Index is : %d\n",dev_index);
    }
    
    if(appPhyCmdProcessor_StrToUint16HexIp(argv[2],&short_addr))
    {
        SYS_CONSOLE_PRINT("\r\n Short Address is :");
        deviceTable[dev_index].shortAddr = short_addr;
        SYS_CONSOLE_PRINT("0x%x\r\n",deviceTable[dev_index].shortAddr);
    }
    
    if(appPhyCmdProcessor_StrToUint64(argv[3],&ieeeAddr))
    {
        SYS_CONSOLE_PRINT("\r\n Ieee Address is:");
        deviceTable[dev_index].ieeeAddr = ieeeAddr;
        SYS_CONSOLE_PRINT("0x%x\r\n",deviceTable[dev_index].ieeeAddr);
        deviceTable[dev_index].valid = true;
    }

}

static void read_device_table(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t retcode = 0; 
    uint16_t dev_index = 0;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint16(argv[1], &dev_index))
    {
        if((dev_index > (NUM_OF_DEVICES - 1U) ) && (dev_index != 0xffU))
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
        SYS_CONSOLE_PRINT("\r\n Device Index is : %d\n",dev_index);
        if(dev_index != 0xffU)
        {    
            uint8_t dvIndx = (uint8_t)dev_index;
            if(deviceTable[dvIndx].valid == true)
            {
                SYS_CONSOLE_MESSAGE("\r\nDEVINDX\tSHORTADDR\tLQI\tRSSI\tTXPKTCNT\tRXPKTCNT\tIEEEADDR\r\n");
                SYS_CONSOLE_PRINT("%d\t",dev_index);
                SYS_CONSOLE_PRINT("0x%x\t\t",deviceTable[dvIndx].shortAddr); 
                SYS_CONSOLE_PRINT("%d\t",deviceTable[dvIndx].lqi);
                SYS_CONSOLE_PRINT("%d\t",deviceTable[dvIndx].rssiVal);
                SYS_CONSOLE_PRINT("%8x\t",deviceTable[dvIndx].txPacketCnt);
                SYS_CONSOLE_PRINT("%8x\t",deviceTable[dvIndx].rxPacketCnt);
                SYS_CONSOLE_PRINT("%8x",deviceTable[dvIndx].ieeeAddr);
            }
            else
            {
                SYS_CONSOLE_MESSAGE("\r\nDevice Index is invalid\r\n");
            }
       }
       else
        {
            SYS_CONSOLE_MESSAGE("\r\nDEVINDX\tSHORTADDR\tLQI\tRSSI\tTXPKTCNT\tRXPKTCNT\tIEEEADDR\r\n");
            for(uint8_t i = 0; i<NUM_OF_DEVICES; i++)
            {
                if(deviceTable[i].valid == true)
                {
                    SYS_CONSOLE_PRINT("%d\t",i);
                    SYS_CONSOLE_PRINT("0x%x\t\t",deviceTable[i].shortAddr); 
                    SYS_CONSOLE_PRINT("%d\t",deviceTable[i].lqi);
                    SYS_CONSOLE_PRINT("%d\t",deviceTable[i].rssiVal);
                    SYS_CONSOLE_PRINT("%8x\t",deviceTable[i].txPacketCnt);
                    SYS_CONSOLE_PRINT("%8x\t",deviceTable[i].rxPacketCnt);
                    SYS_CONSOLE_PRINT("%8x",deviceTable[i].ieeeAddr);
                    SYS_CONSOLE_MESSAGE("\n");
                    PAL_TimerDelay(1500000);
                }
            }
        } 
    }
}

static void phy_init(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    PHY_Retval_t status;
    SYS_CONSOLE_PRINT("Initializing PHY...\r\n");
    status = PHY_Init();
    appPhyCmdProcessor_PhyStatusPrint(status);
}

static void phy_reset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    PHY_Retval_t status;
    uint8_t retcode;
    bool set_pib_val_default;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }

    if(appPhyCmdProcessor_StrToBool(argv[1], &set_pib_val_default))
    {
        SYS_CONSOLE_PRINT("Resetting PHY...\r\n");
        status = PHY_Reset(set_pib_val_default);
        appPhyCmdProcessor_PhyStatusPrint(status);
    }
}

static void phy_edstart(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    
    PHY_Retval_t status;
    uint8_t retcode, scan_duration = 0U;
     if(PHY_Init()!=PHY_SUCCESS){
        return;
    }
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
       appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }

    if(appPhyCmdProcessor_StrToUint8(argv[1], &scan_duration))
    {
        if(scan_duration > 0xeU)
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
    }
    SYS_CONSOLE_PRINT("Energy Detection Scanning Start...\r\n");
    appNwkParam.edScan.edscanDurationSYMBOLS = scan_duration;
    status = PHY_EdStart(scan_duration);
    appNwkParam.edScan.edStatus = status;
    appPhyCmdProcessor_PhyStatusPrint(status);
    if(status == PHY_SUCCESS)
    {
        SYS_CONSOLE_PRINT("ED scan duration timer started successfully, scanned channel is %d\r\n",appNwkParam.channel);
    }    
}

static void phy_pibget(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    PHY_Retval_t status;
    uint8_t retcode, attribute_id = 0U;
    PibValue_t pibValue;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8HexIp(argv[1], &attribute_id))
    {
        if(attribute_id > 0xf0U)
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
    }
       SYS_CONSOLE_PRINT("Getting PHY PIB attribute...\r\n");
    if((attribute_id == 0x0U) || (attribute_id == 0x2U) || (attribute_id == 0x3U) || (attribute_id == 0x4U) ||(attribute_id == 0x4fU) || (attribute_id == 0x57U) || (attribute_id == 0x4eU) || (attribute_id == 0x59U) || (attribute_id == 0x51U))
    {    
        status = PHY_PibGet(attribute_id,(uint8_t *)&pibValue);
        appPhyCmdProcessor_PhyStatusPrint(status);
        if(status == PHY_SUCCESS)
        {
            SYS_CONSOLE_PRINT("\r\n PHY PIB Parameter  - 0x%x\n ",pibValue.pib_value_8bit); 
        }
    }
    
    if((attribute_id == 0x53U) || (attribute_id == 0x50U)) 
    {
        status = PHY_PibGet(attribute_id,(uint8_t *)&pibValue);
        appPhyCmdProcessor_PhyStatusPrint(status);
        if(status == PHY_SUCCESS)
        {
            SYS_CONSOLE_PRINT("\r\n PHY PIB Parameter  - 0x%x\n ",pibValue.pib_value_16bit); 
        }
    }
   
}
static void phy_pibgetChannel(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv){
    
    PHY_Retval_t status;
    PibValue_t pibValue;
    status = PHY_PibGet(phyCurrentChannel,(uint8_t *)&pibValue);
    appPhyCmdProcessor_PhyStatusPrint(status);
    if(status == PHY_SUCCESS)
    {
      SYS_CONSOLE_PRINT("\r\n Channel  - 0x%x\n ",pibValue.pib_value_8bit); 
      appNwkParam.channel = pibValue.pib_value_8bit;
    }
    else
    {
        appPhyCmdProcessor_PhyStatusPrint(status);
    }
   
}

static void phy_pibsetChannel(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    PHY_Retval_t status;
    uint8_t retcode, attribute_val = 0U;
    PibValue_t pibValue;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &attribute_val))
    {
        if((attribute_val < 11U) || (attribute_val > 26U))
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
    }
    pibValue.pib_value_8bit = attribute_val;
    status = PHY_PibSet(phyCurrentChannel, &pibValue);
    appPhyCmdProcessor_PhyStatusPrint(status);
    if(status == PHY_SUCCESS)
    {
      if(PHY_PibGet(phyCurrentChannel,&attribute_val) == PHY_SUCCESS)
      {
          SYS_CONSOLE_PRINT("\r\n Channel  - 0x%x\n ",attribute_val); 
      }
      appNwkParam.channel = attribute_val;
    }
   
}


static void phy_pibgetChannelPage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    PHY_Retval_t status;
    uint8_t channelPage;
    status = PHY_PibGet(phyCurrentPage,&channelPage);
    appPhyCmdProcessor_PhyStatusPrint(status);
    if(status == PHY_SUCCESS)
    {
      SYS_CONSOLE_PRINT("\r\n Channel Page - %x\n ",channelPage); 
      appNwkParam.channelPage = channelPage;
    }
    else
    {
        appPhyCmdProcessor_PhyStatusPrint(status);
    }
}

static void phy_pibsetChannelPage(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    PHY_Retval_t status;
    uint8_t retcode, attribute_val = 0U;
    PibValue_t pibValue;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &attribute_val))
    {
        if((attribute_val != 0x0U) && (attribute_val != 0x2U) && (attribute_val != 0x10U) && (attribute_val != 0x11U))
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
    }
    pibValue.pib_value_8bit = attribute_val;
    status = PHY_PibSet(phyCurrentPage, &pibValue);
    appPhyCmdProcessor_PhyStatusPrint(status);
    if(status == PHY_SUCCESS)
    {
        if(PHY_PibGet(phyCurrentPage,&attribute_val) == PHY_SUCCESS)
        {
            SYS_CONSOLE_PRINT("\r\n Channel Page - %x\n ",attribute_val); 
        }
      appNwkParam.channelPage = attribute_val;
    }
   
}

static void phy_pibgetPanId(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{  
    PHY_Retval_t status;
    PibValue_t pibValue;
    status = PHY_PibGet(macPANId,(uint8_t *)&pibValue);
    appPhyCmdProcessor_PhyStatusPrint(status);
    if(status == PHY_SUCCESS)
    {
      SYS_CONSOLE_PRINT("\r\n PAN ID - 0x%x\n ",pibValue.pib_value_16bit); 
      appNwkParam.srcPanId = pibValue.pib_value_16bit;
    }
    else
    {
        appPhyCmdProcessor_PhyStatusPrint(status);
    }
}
static void phy_pibsetPanId(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PHY_Retval_t status;
    uint8_t retcode;
    uint16_t attr_val_16 = 0U;
    PibValue_t pibValue;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint16HexIp(argv[1],&attr_val_16))
    {
        pibValue.pib_value_16bit = attr_val_16;
        status = PHY_PibSet(macPANId , &pibValue);
        appPhyCmdProcessor_PhyStatusPrint(status);
        if(status == PHY_SUCCESS)
        {
            if(PHY_PibGet(macPANId, (uint8_t*)&attr_val_16) == PHY_SUCCESS)
            {
                SYS_CONSOLE_PRINT("\r\n PAN ID : 0x%x\n ",attr_val_16); 
            }
          appNwkParam.srcPanId = attr_val_16;
        }
    }
 

}
static void phy_pibgetDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
   
    SYS_CONSOLE_PRINT("\r\n Destination Address : 0x%x\n ",appNwkParam.dstAddress); 

}
static void phy_pibsetDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t retcode;
    uint16_t attr_val_16 = 0U;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint16HexIp(argv[1],&attr_val_16))
    {
        SYS_CONSOLE_PRINT("\r\n Existing Destination Address - 0x%x\n ",appNwkParam.dstAddress); 
        appNwkParam.dstAddress = attr_val_16;
        SYS_CONSOLE_PRINT("\r\n New Destination Address - 0x%x\n ",appNwkParam.dstAddress);
        deviceTable[DEST_DEV_INDEX].shortAddr = appNwkParam.dstAddress;
        deviceTable[DEST_DEV_INDEX].valid = true;
    }
}

static void selectDestDevice(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t retcode;
    uint8_t devIndx = 0U;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1],&devIndx))
    {
        SYS_CONSOLE_PRINT("\r\n Selected Destination Device is in Index - %d\n ",devIndx); 
        if(deviceTable[devIndx].valid == true)
        {
            appNwkParam.dstAddress = deviceTable[devIndx].shortAddr;
            appNwkParam.extendedDstAddress = appNwkParam.dstIeeeAddress = deviceTable[devIndx].ieeeAddr;
        }
        else
        {
            SYS_CONSOLE_PRINT("\r\nIndex in device table is empty, update the device table\r\n");
        }
    }
}

static void phy_pibgetExtDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
   
    SYS_CONSOLE_PRINT("\r\n Destination Address : 0x%x\n ",appNwkParam.extendedDstAddress); 

}
static void phy_pibsetExtDestAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t retcode;
    uint64_t attr_val_64 = 0;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint64(argv[1],&attr_val_64))
    {
        SYS_CONSOLE_PRINT("\r\n Existing Destination Address - 0x%x\n ",appNwkParam.extendedDstAddress); 
        appNwkParam.extendedDstAddress = appNwkParam.dstIeeeAddress = attr_val_64;
        SYS_CONSOLE_PRINT("\r\n New Destination Address - 0x%x\n ",appNwkParam.extendedDstAddress);
        deviceTable[DEST_DEV_INDEX].ieeeAddr = appNwkParam.extendedDstAddress;
        deviceTable[DEST_DEV_INDEX].valid = true;
    }
}

static void phy_pibgetSrcAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PibValue_t pibValue;
    PHY_Retval_t attr_stat;
    attr_stat = PHY_PibGet(macShortAddress,(uint8_t *)&pibValue);
    if(attr_stat == PHY_SUCCESS)
    {
        appNwkParam.srcAddress = pibValue.pib_value_16bit;
        SYS_CONSOLE_PRINT("\r\n Source Address : 0x%x\r\n",appNwkParam.srcAddress);
    }
    else
    {
        appPhyCmdProcessor_PhyStatusPrint(attr_stat);
    }
}
static void phy_pibsetSrcAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    
    PibValue_t pibValue;
    uint8_t retcode;
    PHY_Retval_t attr_stat;
    uint16_t attr_val_16 = 0;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint16HexIp(argv[1],&attr_val_16))
    {
        pibValue.pib_value_16bit = attr_val_16;
        SYS_CONSOLE_PRINT("\r\n Existing Source Address - 0x%x\n ",appNwkParam.srcAddress); 
        attr_stat = PHY_PibSet(macShortAddress, &pibValue);
        if(attr_stat != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attr_stat);
            return;
        }
        attr_stat = PHY_PibGet(macShortAddress,(uint8_t *)&attr_val_16);
        if(attr_stat == PHY_SUCCESS)
        {
            appNwkParam.srcAddress = attr_val_16;
            deviceTable[SOURCE_DEV_INDEX].shortAddr = attr_val_16;
            deviceTable[SOURCE_DEV_INDEX].valid = true;
            SYS_CONSOLE_PRINT("\r\n New Source Address : %d \r\n",appNwkParam.srcAddress);
        }
    }
}

static void phy_pibgetCsmaMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    if(appNwkParam.csma.csmaMode == NO_CSMA_NO_IFS)
    {
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - NO_CSMA_NO_IFS\r\n"); 
    }
    else if(appNwkParam.csma.csmaMode == NO_CSMA_WITH_IFS)
    {
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - NO_CSMA_WITH_IFS\r\n"); 
    }
    else if(appNwkParam.csma.csmaMode == CSMA_UNSLOTTED)
    {
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - CSMA_UNSLOTTED\r\n"); 
    }
    else
    {
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - CSMA_SLOTTED\r\n"); 
    }
}
static void phy_pibsetCsmaMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{

    uint8_t retcode, attribute_val = 0U;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &attribute_val))
    {
        if(attribute_val > 0x3U)
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
    }
    if(attribute_val == 0x0U)
    {
        appNwkParam.csma.csmaMode = NO_CSMA_NO_IFS;
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - NO_CSMA_NO_IFS\r\n"); 
    }
    else if(attribute_val == 0x1U)
    {
        appNwkParam.csma.csmaMode = NO_CSMA_WITH_IFS;
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - NO_CSMA_WITH_IFS\r\n"); 
    }
    else if(attribute_val == 0x2U)
    {
        appNwkParam.csma.csmaMode = CSMA_UNSLOTTED;
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - CSMA_UNSLOTTED\r\n"); 
    }
    else
    {
        appNwkParam.csma.csmaMode = CSMA_SLOTTED;
        SYS_CONSOLE_PRINT("\r\n CSMA Mode - CSMA_SLOTTED\r\n"); 
    }
     
}
static void phy_pibgetTxPwr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    
    PHY_Retval_t status;
    PibValue_t pibValue;
    status = PHY_PibGet(phyTransmitPower,(uint8_t *)&pibValue);	
    if(status == PHY_SUCCESS)
    {
        SYS_CONSOLE_PRINT("\r\n Tx Power(in DBm) - %d DBm\r\n ",appNwkParam.txPowerDBm); 
        SYS_CONSOLE_PRINT("\r\n Tx Power - %d\r\n ",pibValue.pib_value_8bit); 
        appNwkParam.txPower = pibValue.pib_value_8bit;
    }
    else
    {
        appPhyCmdProcessor_PhyStatusPrint(status);
    }
}
static void phy_pibsetTxPwr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PHY_Retval_t status;
    uint8_t retcode = 0U;
    int8_t attribute_val = 0;
    PibValue_t pibValue;
    if(argc<2){
        retcode = ERR_MISSING_PARAM;
       appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToInt8(argv[1], &attribute_val))
    {
        if((attribute_val < -12) || (attribute_val > 14))
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
            appNwkParam.txPowerDBm = attribute_val;
            SYS_CONSOLE_PRINT("\r\n Tx Power - %d DBm\r\n ",appNwkParam.txPowerDBm); 
            pibValue.pib_value_8bit = (uint8_t)CONV_DBM_TO_phyTransmitPower(appNwkParam.txPowerDBm);
            status = PHY_PibSet(phyTransmitPower, &pibValue);	
            if(status == PHY_SUCCESS)
            {
                if(PHY_PibGet(phyTransmitPower,&pibValue.pib_value_8bit) == PHY_SUCCESS)
                {
                    appNwkParam.txPower = pibValue.pib_value_8bit;
                    SYS_CONSOLE_PRINT("\r\nSet Tx Power - %d DBm\r\n ",appNwkParam.txPower); 
                }
            }
            else
            {
               appPhyCmdProcessor_PhyStatusPrint(status);
            }
    }

}

static void phy_pibgetIeeeAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
      
    PHY_Retval_t attributeStatus;
    PibValue_t pibValue;
    attributeStatus = PHY_PibGet(macIeeeAddress,(uint8_t *)&pibValue);
    if(attributeStatus == PHY_SUCCESS)
    {
        appNwkParam.ieeeAddress = pibValue.pib_value_64bit;
        SYS_CONSOLE_PRINT("\r\n IEEE Address : 0x%x\r\n",appNwkParam.ieeeAddress);
    }
}

static void phy_pibsetIeeeAddr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PHY_Retval_t attributeStatus;
    PibValue_t pibValue;
    uint64_t ieeeaddr = 0U;
    if(appPhyCmdProcessor_StrToUint64(argv[1], &ieeeaddr))
    {
        pibValue.pib_value_64bit = ieeeaddr;
        attributeStatus = PHY_PibSet(macIeeeAddress,&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            attributeStatus = PHY_PibGet(macIeeeAddress,(uint8_t *)&ieeeaddr);
            appNwkParam.extendedSrcAddress = appNwkParam.ieeeAddress = ieeeaddr;
            deviceTable[SOURCE_DEV_INDEX].ieeeAddr = appNwkParam.ieeeAddress;
            SYS_CONSOLE_PRINT("\r\n IEEE Address : 0x%x\r\n",appNwkParam.ieeeAddress);
        }
    }
    
    
}

static void phy_pibgetFrameRetry(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{   
    uint8_t frameRetry = 0U;
    PHY_Retval_t attr_stat;
    attr_stat = PHY_PibGet(macMaxFrameRetries,&frameRetry);
    if(attr_stat == PHY_SUCCESS)
    {
        appNwkParam.frameRetry = frameRetry;
        SYS_CONSOLE_PRINT("\r\n Frame retry - %x\n ",appNwkParam.frameRetry); 
    }
    else
    {
        appPhyCmdProcessor_PhyStatusPrint(attr_stat);
    }
}

static void phy_pibsetFrameRetry(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    uint8_t retcode;
    uint8_t attribute_val = 0U;
    PibValue_t pibValue;
    PHY_Retval_t attr_stat;
    
    if(appPhyCmdProcessor_StrToUint8(argv[1], &attribute_val))
    {
        if(attribute_val > 0x7U)
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
    }
    pibValue.pib_value_8bit = attribute_val;
    attr_stat = PHY_PibSet(macMaxFrameRetries, &pibValue);
    if(attr_stat != PHY_SUCCESS)
    {
        appPhyCmdProcessor_PhyStatusPrint(attr_stat);
        return;
    }
    attr_stat = PHY_PibGet(macMaxFrameRetries,&attribute_val);
    if(attr_stat == PHY_SUCCESS)
    {
        appNwkParam.frameRetry = attribute_val;
        SYS_CONSOLE_PRINT("\r\n Frame retry set to- %x\n ",appNwkParam.frameRetry); 
    }
}

static void phy_pibset(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    
    PHY_Retval_t status;
    uint8_t retcode, attribute_id, attr_val_8,tempVar = 0U;
    int8_t txPwrDbm = 0;
    PibValue_t pibValue;
    uint16_t attr_val_16;
    
    if(argc<3)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8HexIp(argv[1], &attribute_id))
    {
        if((attribute_id > 0xf0U))
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
        SYS_CONSOLE_PRINT("\r\n Attribute ID is : %d ",attribute_id);
        if((attribute_id == 0x0U) || (attribute_id == 0x3U) || (attribute_id == 0x4U) ||(attribute_id == 0x4fU) || (attribute_id == 0x57U) || (attribute_id == 0x4EU) || (attribute_id == 0x59U) || (attribute_id == 0x51U))
        {
            if(appPhyCmdProcessor_StrToUint8HexIp(argv[2], &attr_val_8))
            {
                if((attr_val_8 > 0x32U))
                {
                    retcode = ERR_PARAM_OUT_OF_RANGE;
                    appPhyCmdProcessor_PrintReturnCode(retcode);
                    return;
                }
                pibValue.pib_value_8bit = attr_val_8;
                status = PHY_PibSet(attribute_id, &pibValue);
                appPhyCmdProcessor_PhyStatusPrint(status);
                if(status == PHY_SUCCESS)
                {
                    status = PHY_PibGet(attribute_id,&attr_val_8);
                    if(status == PHY_SUCCESS)
                    {
                        SYS_CONSOLE_PRINT("\nAttribute value set to: 0x%x", attr_val_8);
                    }
                }
            }
        } 
        if((attribute_id == 0x2U))
        {
            if(appPhyCmdProcessor_StrToInt8(argv[2], &txPwrDbm))
            {
                if((txPwrDbm < -12) || (txPwrDbm > 14))
                {
                    retcode = ERR_PARAM_OUT_OF_RANGE;
                    appPhyCmdProcessor_PrintReturnCode(retcode);
                    return;
                }
                SYS_CONSOLE_PRINT("\r\n Tx Power(in DBm) - %d\r\n ",appNwkParam.txPowerDBm); 
                tempVar = (uint8_t)CONV_DBM_TO_phyTransmitPower(txPwrDbm);
                pibValue.pib_value_8bit = tempVar;
                status = PHY_PibSet(attribute_id, &pibValue);
                appPhyCmdProcessor_PhyStatusPrint(status);
                if(status == PHY_SUCCESS)
                {
                    status = PHY_PibGet(attribute_id,&tempVar);
                    if(status == PHY_SUCCESS)
                    {
                        SYS_CONSOLE_PRINT("\nTx Power set to: %d\r\n", tempVar);
                    }
                }
            }
        }               
        if((attribute_id == 0x53U) || (attribute_id == 0x50U)) 
        {
            if(appPhyCmdProcessor_StrToUint16(argv[2],&attr_val_16))
            {
                SYS_CONSOLE_PRINT("\r\n 16 bit Attribute value is  : %u ",attr_val_16);
                SYS_CONSOLE_PRINT("\r\n Setting PHY PIB attribute...\r\n");
                pibValue.pib_value_16bit = attr_val_16;
                status = PHY_PibSet(attribute_id, &pibValue);
                appPhyCmdProcessor_PhyStatusPrint(status);
                if(status == PHY_SUCCESS)
                {
//        SYS_CONSOLE_PRINT("\r\n PHY PIB attribute Set Successfully\r\n"); 
                    status = PHY_PibGet(attribute_id,(uint8_t *)&attr_val_16);
                    if(status == PHY_SUCCESS)
                    {
                        SYS_CONSOLE_PRINT("\nAttribute value set to: 0x%4x", attr_val_16);
                    } 
                }
            }
        }
    }  
}

static void phy_rxenable(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    
    uint8_t retcode;
    bool trx_state_val;
    PHY_TrxStatus_t trx_status;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToBool(argv[1], &trx_state_val))
    {
        SYS_CONSOLE_PRINT("Enabling Receiver from PHY_STATE_TRX_OFF to PHY_STATE_RX_ON...\r\n");
        trx_status = PHY_RxEnable((PHY_TrxState_t)trx_state_val);
        appPhyCmdProcessor_PhyTrxStatusPrint(trx_status);
    }
}

static void phy_trxsleep(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv){
    
    uint8_t retcode;
    bool slp;
    PHY_SleepMode_t sleep_mode;
    PHY_Retval_t trx_sleep_status;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToBool(argv[1], &slp))
    {
        sleep_mode = (PHY_SleepMode_t)slp;
        SYS_CONSOLE_PRINT("Setting Transceiver to Sleep...\r\n");
        trx_sleep_status = PHY_TrxSleep(sleep_mode);
        appPhyCmdProcessor_PhyStatusPrint(trx_sleep_status);
    }
    
}

static void phy_trxwakeup(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PHY_Retval_t trx_wake_status;
    SYS_CONSOLE_PRINT("Wakeup Transceiver...\r\n");
    trx_wake_status = PHY_TrxWakeup();
    appPhyCmdProcessor_PhyStatusPrint(trx_wake_status);
}

static void phy_ccaperform(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{    
    PHY_Retval_t cca_res, retVal = PHY_FAILURE;
    uint8_t retcode, phyChannel = 0U;
    PibValue_t pibValue;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &phyChannel))
    {
        if((phyChannel < 11U) || (phyChannel > 26U))
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
        else
        {
            // Setting Current channel
            SYS_CONSOLE_PRINT("Setting Current Channel to %d ",phyChannel);
            pibValue.pib_value_8bit = phyChannel;
            retVal = PHY_PibSet(phyCurrentChannel, &pibValue);
            if(PHY_SUCCESS == retVal)
            {
                SYS_CONSOLE_PRINT("\r\n Performing CCA...\r\n");
                //Take the Ed sample
                cca_res = PHY_CCAPerform(); 
                appPhyCmdProcessor_PhyStatusPrint(cca_res);
            } 
        }
    }
}

static void stop_active_test_mode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    SYS_CONSOLE_PRINT("\r\n STOP ALL ACTIVE TEST MODES\r\n");
    app_P2P_Phy_stopActiveModes();   
}

static void phy_StartContinuousTransmit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t retcode = 0, txModetemp = 0;
    PHY_ContinuousTxMode_t txMode = CW_MODE;
    bool useRandomContent = true;   
    if(argc<3)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &txModetemp))
    {
        if(txModetemp > 1U)
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
        txMode = (PHY_ContinuousTxMode_t)txModetemp;
        if(appPhyCmdProcessor_StrToBool(argv[2], &useRandomContent))
        {
        //Start continuous tx in CW mode
        PHY_StartContinuousTransmit(txMode, useRandomContent);   
        }   
    }
}

static void phy_StopContinuousTransmit(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    bool appInit;
    //Stop continuous tx
    PHY_StopContinuousTransmit();
    appInit = app_P2P_Phy_Init();
    if(appInit == true)
    {
        SYS_CONSOLE_MESSAGE("\r\n App Re-initialized\r\n");
    }
}

static void phy_ConfigAutoAck(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
   PHY_Retval_t retVal = PHY_FAILURE;
   bool isEnableAACK = true;
   uint8_t retcode = 0U;
   if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToBool(argv[1], &isEnableAACK))
    {
        retVal = PHY_ConfigAutoAck(isEnableAACK);
    }
   
   if(PHY_SUCCESS == retVal)
   {
    //Trx is configured to auto acknowledge for the received packet
       SYS_CONSOLE_MESSAGE("\r\nTrx is configured to auto acknowledge for the received packet\r\n");
   }
}
static void phy_GetRSSIBaseVal(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    int8_t trxBaseRSSI;
    // Get RSSI base value of TRX
    trxBaseRSSI = PHY_GetRSSIBaseVal();
    SYS_CONSOLE_PRINT("\r\n RSSI Base Value %d\r\n",trxBaseRSSI);
}
static void phy_GetTrxStatus(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PHY_TrxStatus_t trxStatus;
    //Gets the current status of trx
    trxStatus = PHY_GetTrxStatus();
    appPhyCmdProcessor_PhyTrxStatusPrint(trxStatus); 
}
static void phy_ConfigRxSensitivity(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t pdtLevel = 0x03U, retcode = 0U;
    PHY_Retval_t retVal = PHY_FAILURE;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &pdtLevel))
    {
        if(pdtLevel <= 15U){
         // Reduce the PDT level
        retVal = PHY_ConfigRxSensitivity(pdtLevel);
        if(retVal == PHY_SUCCESS)
        {
                 // To get the PDT level configured
            retVal = PHY_GetTrxConfig(RX_SENS, &pdtLevel);   
            appPhyCmdProcessor_PhyStatusPrint(retVal);
        }
        }else{
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
        }
    }
}
static void phy_ConfigTxPwr(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    //To be implemented
}
static void phy_ConfigAntennaDiversity(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    //To be implemented
}

static void phy_ConfigRxRPCMode(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    PHY_Retval_t retVal = PHY_FAILURE;
    uint8_t rxRPCEnable = 0x01U, retcode = 0U;
    
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &rxRPCEnable))
    {
        retVal = PHY_ConfigRxRPCMode(rxRPCEnable);
    }
    
    if(PHY_SUCCESS == retVal)
    {
        SYS_CONSOLE_MESSAGE("\r\nTrx is configured to reduced power consumption mode\r\n");
    }
}

static void getAddressingModes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // //To be implemented
}
static void setAddressingModes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    // //To be implemented
}
static void getFrameTypes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    SYS_CONSOLE_PRINT("\r\n Frame Type : ");
    switch(appNwkParam.frameType)
        {
            case 0:
            {
                SYS_CONSOLE_MESSAGE("Beacon frame\r\n");
                break;
            }
            case 1:
            {
                SYS_CONSOLE_MESSAGE("Data frame\r\n");
                break;
            }
            case 2:
            {
                SYS_CONSOLE_MESSAGE("Ack frame\r\n");
                break;
            }
            case 3:
            {
                SYS_CONSOLE_MESSAGE("CMD frame\r\n");
                break;
            }
            case 4:
            {
                SYS_CONSOLE_MESSAGE("LLDN frame\r\n");
                break;
            }
            case 5:
            {
                SYS_CONSOLE_MESSAGE("Multipurpose frame\r\n");
                break;
            }
            default:
            {
                SYS_CONSOLE_MESSAGE("Invalid frame\r\n");
                break;
            }
               
        }
}
static void setFrameTypes(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv)
{
    uint8_t retcode;
    uint8_t frameType=0;
    if(argc<2)
    {
        retcode = ERR_MISSING_PARAM;
        appPhyCmdProcessor_PrintReturnCode(retcode);
        return;
    }
    if(appPhyCmdProcessor_StrToUint8(argv[1], &frameType))
    {
        if(frameType > 5U)
        {
            retcode = ERR_PARAM_OUT_OF_RANGE;
            appPhyCmdProcessor_PrintReturnCode(retcode);
            return;
        }
        else
        {
        appNwkParam.frameType = frameType;  
        SYS_CONSOLE_PRINT("\r\n FrameType is :");
        switch(frameType)
        {
            case 0:
            {
                SYS_CONSOLE_MESSAGE("Beacon frame\r\n");
                break;
            }
            case 1:
            {
                SYS_CONSOLE_MESSAGE("Data frame\r\n");
                break;
            }
            case 2:
            {
                SYS_CONSOLE_MESSAGE("Ack frame\r\n");
                break;
            }
            case 3:
            {
                SYS_CONSOLE_MESSAGE("CMD frame\r\n");
                break;
            }
            case 4:
            {
                SYS_CONSOLE_MESSAGE("LLDN frame\r\n");
                break;
            }
            case 5:
            {
                SYS_CONSOLE_MESSAGE("Multipurpose frame\r\n");
                break;
            }
            default:
            {
                SYS_CONSOLE_MESSAGE("Invalid frame\r\n");
                break;
            }
               
        }
    }
    }
}
/**************************************************************************************************/

bool appPhyCmdProcessor_StrToBool(const char *str, bool *res)
{
    if (strcmp(str, "true") == 0 || strcmp(str, "True") == 0)
    {
        *res = true;
        return true;
    }
    else if (strcmp(str, "false") == 0 || strcmp(str, "False") == 0)
    {
        *res = false;
        return true;
    }
    else
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input. Please enter either 'true' or 'false'\r\n");
        return false;
    }
}

bool appPhyCmdProcessor_StrToUint8HexIp(const char *str, uint8_t *res)
{
    char *end;
    errno = 0;
    uint16_t val1 = (uint16_t)strtol(str, &end, 16);
    if (errno != 0 || (*end != '\0') || (val1 > 0xffU)) 
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    errno = 0;
    uint8_t val = (uint8_t)strtol(str, &end, 16);
    if(errno != 0)
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = val;
    return true;
}

bool appPhyCmdProcessor_StrToUint8(const char *str, uint8_t *res)
{
    char *end;
    errno = 0;  
    uint16_t val1 = (uint16_t)strtol(str, &end, 10);
    if (errno != 0 || (*end != '\0') || (val1 > 255U)) 
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    errno = 0;
    uint8_t val = (uint8_t)strtol(str, &end, 10);
    if(errno != 0)
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = val;
    return true;
}

bool appPhyCmdProcessor_StrToInt8(const char *str, int8_t *res)
{
    char *end;
    errno = 0;  
    int8_t val = (int8_t)strtol(str, &end, 10);
    if (errno != 0 || (*end != '\0')) 
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = val;
    return true;
}

bool appPhyCmdProcessor_StrToUint16(const char *str, uint16_t *res)
{
    char *end;
    errno = 0;
    uint32_t val1 = (uint32_t)strtoul(str, &end, 10);
    if (errno != 0 || (*end != '\0') || (val1 > 65535U)) 
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    errno = 0;
    uint16_t val = (uint16_t)strtoul(str, &end, 10);
    if(errno != 0)
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = val;
    return true;
}

bool appPhyCmdProcessor_StrToUint16HexIp(const char *str, uint16_t *res)
{
    char *end;
    errno = 0;
    uint32_t val1 = (uint32_t)strtoul(str, &end, 16);
    if (errno != 0 || (*end != '\0') || (val1 > 0xffffU)) 
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    errno = 0;
    uint16_t val = (uint16_t)strtoul(str, &end, 16);
    if(errno != 0)
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = val;
    return true;
}

bool appPhyCmdProcessor_StrToUint32(const char *str, uint32_t *res) 
{    
    char *end;
    errno = 0;
    uint64_t val1 = (uint64_t)strtoul(str, &end, 10);
    if (errno != 0 || (*end != '\0') || (val1 > 4294967295U))
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    errno = 0;
    uint32_t val = (uint32_t)strtoul(str, &end, 10);
    if(errno != 0)
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = val;
    return true;
}

bool appPhyCmdProcessor_StrToUint64(const char *str, uint64_t *res) 
{
    char *endptr;
    errno = 0;
    uint64_t result = (uint64_t)strtoull(str, &endptr, 16);
    if (errno != 0 || (*endptr != '\0')) //|| (result > 18446744073709551615U) 
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = result;
    return true;
}

bool appPhyCmdProcessor_StrToUint64DecIp(const char *str, uint64_t *res) 
{
    char *endptr;
    errno = 0;
    uint64_t result = (uint64_t)strtoull(str, &endptr, 10);
    if (errno != 0 || (*endptr != '\0')) //|| (result > 18446744073709551615U) 
    {
        SYS_CONSOLE_MESSAGE("\r\nInvalid input\r\n");
        return false;
    }
    *res = result;
    return true;
}

void appPhyCmdProcessor_PhyStatusPrint(PHY_Retval_t status){

    switch((uint8_t)status)
    {
        case 0x00:
            SYS_CONSOLE_PRINT("\r\nPHY_SUCCESS\r\n");
            break;
        case 0x81:
            SYS_CONSOLE_PRINT("\r\nPHY_TRX_ASLEEP\r\n");
            break;
        case 0x82:
            SYS_CONSOLE_PRINT("\r\nPHY_TRX_AWAKE\r\n");
            break;
        case 0x83:
            SYS_CONSOLE_PRINT("\r\nPHY_RF_REQ_ABORTED\r\n");
            break;
        case 0x84:
            SYS_CONSOLE_PRINT("\r\nPHY_RF_UNAVAILABLE\r\n");
            break;
        case 0x85:
            SYS_CONSOLE_PRINT("\r\nPHY_FAILURE\r\n");
            break;
        case 0x86:
            SYS_CONSOLE_PRINT("PHY_BUSY\r\n");
            break;
        case 0x87:
            SYS_CONSOLE_PRINT("\r\nPHY_FRAME_PENDING\r\n");
            break;
        case 0x88:
            SYS_CONSOLE_PRINT("\r\nPHY_INVALID_PARAMETER\r\n");
            break;
        case 0x89:
            SYS_CONSOLE_PRINT("\r\nPHY_UNSUPPORTED_ATTRIBUTE\r\n");
            break;
        case 0x8A:
            SYS_CONSOLE_PRINT("\r\nPHY_CHANNEL_BUSY\r\n");
            break;
        case 0x8B:
            SYS_CONSOLE_PRINT("\r\nPHY_CHANNEL_IDLE\r\n");
            break;
        case 0x8C:
            SYS_CONSOLE_PRINT("\r\nPHY_NO_ACK\r\n");
            break;
        case 0x8D:
            SYS_CONSOLE_PRINT("\r\nPHY_CHANNEL_ACCESS_FAILURE\r\n");
            break;
        default:
            SYS_CONSOLE_PRINT("\r\nPHY_UNKNOWN_STATE\r\n");
            break;
            
    }
} 

void appPhyCmdProcessor_PhyTrxStatusPrint(PHY_TrxStatus_t status){
    
    switch((uint8_t)status){
        case 0x08:
            SYS_CONSOLE_PRINT("PHY_TRX_OFF\r\n");
            break;
        case 0x16:
            SYS_CONSOLE_PRINT("PHY_RX_ON\r\n");
            break;
        case 0x19:
            SYS_CONSOLE_PRINT("PHY_TX_ON\r\n");
            break;
        case 0x11:
            SYS_CONSOLE_PRINT("PHY_BUSY_RX\r\n");
            break;
        case 0x12:
            SYS_CONSOLE_PRINT("PHY_BUSY_TX\r\n");
            break;
        case 0x0F:
            SYS_CONSOLE_PRINT("PHY_TRX_SLEEP\r\n");
            break;
        case 0x20:
            SYS_CONSOLE_PRINT("PHY_TRX_DEEP_SLEEP\r\n");
            break;
        default:
            SYS_CONSOLE_PRINT("PHY_UNKNOWN_STATE\r\n");
            break;
            
    }
} 
void appPhyCmdProcessor_CmdDoneOk(void)
{
  SYS_CONSOLE_PRINT("\r\nSuccess\r\n");
}

void appPhyCmdProcessor_CmdDoneFail(uint8_t err_code)
{
  SYS_CONSOLE_PRINT("Err%d: ",err_code);
  switch(err_code)
  {
    case ERR_UNKNOWN_CMD:
      SYS_CONSOLE_PRINT("Unknown Command\r\n");
      break;
    case ERR_INVALID_PARAM:
      SYS_CONSOLE_PRINT("Invalid Parameter\r\n");
      break;
    case ERR_MISSING_PARAM:
      SYS_CONSOLE_PRINT("Missing Parameter\r\n");
      break;
    case ERR_INVALID_PARAM_TYPE:
      SYS_CONSOLE_PRINT("Invalid Parameter Type\r\n");
      break;
    case ERR_PARAM_OUT_OF_RANGE:
      SYS_CONSOLE_PRINT("Parameter Out of Range\r\n");
      break;
    case ERR_INVALID_FORMAT:
      SYS_CONSOLE_PRINT("Invalid Format\r\n");
      break;
    case ERR_SCRIPT_PARSING:
      SYS_CONSOLE_PRINT("Script Parsing Error\r\n");
      break;
    case ERR_HEAP_FULL:
      SYS_CONSOLE_PRINT("Heap Full\r\n");
      break;
    case ERR_INVALID_STATE:
      SYS_CONSOLE_PRINT("Invalid State\r\n");
      break;
    case ERR_NO_RESOURCES:
      SYS_CONSOLE_PRINT("No Resources\r\n");
      break;
    case ERR_BLE_ERR:
      SYS_CONSOLE_PRINT("BLE Err\r\n");
      break;
    case ERR_HAL_ERR:
      SYS_CONSOLE_PRINT("HAL Err\r\n");
      break;  
    case ERR_UNKNOWN_ERR:
      SYS_CONSOLE_PRINT("Unknown Err\r\n");
        break;
    default:
      SYS_CONSOLE_PRINT("Unknown Err\r\n");
      break;
  }
  //SYS_CONSOLE_PRINT("\r\n");
  //send_prompt();
}

void appPhyCmdProcessor_PrintReturnCode(uint8_t status)
{
  switch(status)
  {
    case AOK:
      appPhyCmdProcessor_CmdDoneOk();
      break;
      
    case NO_PROMPT:
      break;
        
    case PENDING:
      break;

    default:
      appPhyCmdProcessor_CmdDoneFail(status);
      break;
  }
  //time_to_process_cmd = 0;
}

void appPhyCmdProcessor_SetModeFlags(bool enterDM, bool enterPTM, bool enterCTM, bool enterThruputTM, bool enterPerTM, bool thruputCb, bool perTM)
{
    modeSwitchFlag.enterDataMode = enterDM;
    modeSwitchFlag.enterPeriodicTestMode = enterPTM;
    modeSwitchFlag.enterContTestMode = enterCTM;
    modeSwitchFlag.enterThroughputTestMode = enterThruputTM; 
    modeSwitchFlag.enterPerTestMode = enterPerTM;
    modeSwitchFlag.throughputTimerCallback = thruputCb;
    modeSwitchFlag.perTestMode = perTM;
}

void appPhyCmdProcessor_calculateDevicePerfParams(void)
{
    if(appDataMode.devPerformanceParam.isModeActive == true)
    {
        appDataMode.devPerformanceParam.isModeActive = false;
        SYS_CONSOLE_MESSAGE("\r\n Device Performance Data Mode\r\n");
        appDataMode.devPerformanceParam.nBytesSent = appNwkParam.nBytesSent;
        appDataMode.devPerformanceParam.nPacketsSent = appNwkParam.nPacketsSent;
        appDataMode.devPerformanceParam.nPacketsRcvdAck = appNwkParam.nPacketsRcvdAck;
        appNwkParam.nBytesSent = appNwkParam.nPacketsSent = appNwkParam.nPacketsRcvdAck = 0;
        PAL_TimerDelay(1000000);
        app_P2P_Phy_packetErrorRateCal(appDataMode.devPerformanceParam.nPacketsSent,appDataMode.devPerformanceParam.nBytesSent, appDataMode.devPerformanceParam.nPacketsRcvdAck);
    }
    else if(appPeriodTestMode.devPerformanceParam.isModeActive == true)
    {
        appPeriodTestMode.devPerformanceParam.isModeActive = false;
        SYS_CONSOLE_MESSAGE("\r\n Device Performance Period Test Mode\r\n");
        appPeriodTestMode.devPerformanceParam.nBytesSent = appNwkParam.nBytesSent;
        appPeriodTestMode.devPerformanceParam.nPacketsSent = appNwkParam.nPacketsSent;
        appPeriodTestMode.devPerformanceParam.nPacketsRcvdAck = appNwkParam.nPacketsRcvdAck;
        appNwkParam.nBytesSent = appNwkParam.nPacketsSent = appNwkParam.nPacketsRcvdAck = 0;
        PAL_TimerDelay(1000000);
        app_P2P_Phy_packetErrorRateCal(appPeriodTestMode.devPerformanceParam.nPacketsSent,appPeriodTestMode.devPerformanceParam.nBytesSent, appPeriodTestMode.devPerformanceParam.nPacketsRcvdAck);
    }
    else if(appContTestMode.devPerformanceParam.isModeActive == true)
    {
        appContTestMode.devPerformanceParam.isModeActive = false;
        SYS_CONSOLE_MESSAGE("\r\n Device Performance Continuous Test Mode\r\n");
        appContTestMode.devPerformanceParam.nBytesSent = appNwkParam.nBytesSent;
        appContTestMode.devPerformanceParam.nPacketsSent = appNwkParam.nPacketsSent;
        appContTestMode.devPerformanceParam.nPacketsRcvdAck = appNwkParam.nPacketsRcvdAck;
        appNwkParam.nBytesSent = appNwkParam.nPacketsSent = appNwkParam.nPacketsRcvdAck = 0;
        PAL_TimerDelay(1000000);
        app_P2P_Phy_packetErrorRateCal(appContTestMode.devPerformanceParam.nPacketsSent,appContTestMode.devPerformanceParam.nBytesSent, appContTestMode.devPerformanceParam.nPacketsRcvdAck);
    }
    else if(appPerTestMode.devPerformanceParam.isModeActive == true)
    {
        appPerTestMode.devPerformanceParam.isModeActive = false;
        SYS_CONSOLE_MESSAGE("\r\n Device Performance PER Test Mode\r\n");
        appPerTestMode.devPerformanceParam.nBytesSent = appNwkParam.nBytesSent;
        appPerTestMode.devPerformanceParam.nPacketsSent = appNwkParam.nPacketsSent;
        appPerTestMode.devPerformanceParam.nPacketsRcvdAck = appNwkParam.nPacketsRcvdAck;
        appNwkParam.nBytesSent = appNwkParam.nPacketsSent = appNwkParam.nPacketsRcvdAck = 0;
        PAL_TimerDelay(1000000);
        app_P2P_Phy_packetErrorRateCal(appPerTestMode.devPerformanceParam.nPacketsSent,appPerTestMode.devPerformanceParam.nBytesSent, appPerTestMode.devPerformanceParam.nPacketsRcvdAck);
    }
    else if(appThroughputTestMode.devPerformanceParam.isModeActive == true)
    {
        appThroughputTestMode.devPerformanceParam.isModeActive = false;
        SYS_CONSOLE_PRINT("\r\n Device Performance in Throughput Test Mode for %u usecs\r\n",sysTimerFlag.throughputSysTimer);
        appThroughputTestMode.devPerformanceParam.nBytesSent = appNwkParam.nBytesSent;
        appThroughputTestMode.devPerformanceParam.nPacketsSent = appNwkParam.nPacketsSent;
        appThroughputTestMode.devPerformanceParam.nPacketsRcvdAck = appNwkParam.nPacketsRcvdAck;
        appNwkParam.nBytesSent = appNwkParam.nPacketsSent = appNwkParam.nPacketsRcvdAck = 0;
        PAL_TimerDelay(1000000);
        app_P2P_Phy_packetErrorRateCal(appThroughputTestMode.devPerformanceParam.nPacketsSent,appThroughputTestMode.devPerformanceParam.nBytesSent, appThroughputTestMode.devPerformanceParam.nPacketsRcvdAck);
    }
    else
    {
        //do nothing
    }
}