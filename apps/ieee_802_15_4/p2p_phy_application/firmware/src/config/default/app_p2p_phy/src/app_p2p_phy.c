/*******************************************************************************
 P2P PHY Application based on IEEE 802.15.4 Standalone PHY Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_p2p_phy.c

  Summary:
    This file contains the P2P PHY Application functions for this project.

  Description:
   This file contains the P2P PHY Application functions for this project.
 *******************************************************************************/

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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "definitions.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

static APP_STATES_PARAM appStatesParam;
static char consoleBuffer[APP_PAYLOAD_BUFFER_SIZE];
static SYS_CONSOLE_HANDLE uartConsoleHandle;
static ssize_t nUnreadBytes;
static uint8_t readBytesConsole[APP_PAYLOAD_BUFFER_SIZE], inc, nextLoc, len;
static uint8_t *pReadBytes, *pReadBytesConsole;
static bool payloadMaxLimitFlag, payloadFragmentLimit, contDataModeflag;
static OSAL_RESULT osalResult;
extern TaskHandle_t xSYS_CMD_Tasks;
APP_NWK_PARAM appNwkParam;
DEVICE_TABLE deviceTable[NUM_OF_DEVICES]; 
bool dataModeTimerStartFlag;


// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************
//DeviceTable_t deviceTable[NUM_OF_DEVICES];
bool app_P2P_Phy_Init(void){

        SYS_CONSOLE_MESSAGE("\r\n=====P2P PHY APP INIT======\r\n");
        PhyCmdTable_Init();
        uartConsoleHandle = SYS_CONSOLE_HandleGet(SYS_CONSOLE_INDEX_0);
        APP_OP_STATE_T app_op_state;
        APP_NWK_PARAM *pAppnwkParam;
        pAppnwkParam = app_P2P_Phy_AssignNWKParams();
        app_op_state = app_P2P_Phy_SetNWKParams(pAppnwkParam);
        if(app_op_state == APP_PIB_ATTR_SET_FAILURE)
        {
            return false;
        }
        else
        {
            DEVICE_TABLE devtable_1;
            devtable_1.ieeeAddr = appNwkParam.ieeeAddress;
            devtable_1.shortAddr = appNwkParam.srcAddress;
            app_P2P_Phy_SetDeviceTableInfo(SOURCE_DEV_INDEX,&devtable_1);   
            DEVICE_TABLE devtable_2;
            devtable_2.ieeeAddr = appNwkParam.dstIeeeAddress;
            devtable_2.shortAddr = appNwkParam.dstAddress;
            app_P2P_Phy_SetDeviceTableInfo(DEST_DEV_INDEX,&devtable_2);
	        app_P2P_Phy_ConfigDataReception();
            app_P2P_Phy_setAddressModes(FCF_SHORT_ADDR, FCF_SHORT_ADDR);
            app_P2P_Phy_setFrameType(FCF_FRAMETYPE_DATA);
            sysTimerFlag.periodicSysTimer = 10000U;
            sysTimerFlag.singleShotSysTimer = 10000U;
            sysTimerFlag.throughputSysTimer = 1000000U;
            appNwkParam.nPerTestPackets = 100U;
            return true;   
        }
}


void app_P2P_Phy_TaskHandler(APP_Msg_T *appModeSwitch){

    switch((APP_Mode_T)appModeSwitch->msgId)
    {            
        case APP_CMD_TASK_SUSPEND:
        {
            memset(readBytesConsole, 0, sizeof(readBytesConsole));
            memset(consoleBuffer, 0, sizeof(consoleBuffer));
            vTaskSuspend(xSYS_CMD_Tasks);
            nextLoc = inc = len = 0;
            payloadMaxLimitFlag = false;
            payloadFragmentLimit = false;
            pReadBytes = (uint8_t *)consoleBuffer; 
            pReadBytesConsole = readBytesConsole;
            appStatesParam.appOpModes = DATA_MODE;
            appModeSwitch->msgId = (uint8_t)APP_DATA_MODE;
            osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
            osalResult = osalResult;//MISRA C-2012 Rule 17.7
            break;
        }
                                        
        case APP_DATA_MODE:
        {
        uint8_t temp = 0;
        ssize_t sysConsoleRead;
        nUnreadBytes = SYS_CONSOLE_ReadCountGet(uartConsoleHandle);
        if((nUnreadBytes >= 1))
        {
            if( dataModeTimerStartFlag == false)
            {
               dataModeTimerStartFlag = true;
               sysTimerFlag.sysTimerDataModeHandle = SYS_TIME_CallbackRegisterUS(&app_P2P_Phy_systimerCallbackDataMode, (uintptr_t)&temp, sysTimerFlag.singleShotSysTimer, SYS_TIME_SINGLE); 
               if(sysTimerFlag.sysTimerDataModeHandle == SYS_TIME_HANDLE_INVALID)
               {
                   dataModeTimerStartFlag = false;
               }
            }
            sysConsoleRead = SYS_CONSOLE_Read(uartConsoleHandle, consoleBuffer, (size_t)nUnreadBytes);
            sysConsoleRead = sysConsoleRead;
            len = len + ((uint8_t)nUnreadBytes);
            SYS_CONSOLE_PRINT("%s",consoleBuffer);//enable local echo in teraterm
            for(uint8_t i=0; i < ((uint8_t)nUnreadBytes); i++)
            {                
                if((*(pReadBytes + i)) != ((uint8_t)'\n'))
                {
                    
                    if((*(pReadBytes + i)) == ((uint8_t)'&'))
                    {
                        inc = inc + 1U;
                        if(inc > 2U)
                        {
                            appStatesParam.appOpModes = CMD_MODE;
                            inc = 0;
                            vTaskResume(xSYS_CMD_Tasks);
                            SYS_CONSOLE_MESSAGE("\nCMD>\n");
                            app_P2P_Phy_stopActiveModes();
                        }
                    }
                    else
                    {
                        inc = 0;
                    }
                    *(pReadBytesConsole + nextLoc + i) = *(pReadBytes + i);                  
                    contDataModeflag = true;    
                }
                else
                {
                  
                    SYS_TIME_RESULT timerDestroy = SYS_TIME_TimerDestroy(sysTimerFlag.sysTimerDataModeHandle);
                    timerDestroy = timerDestroy;
                    if((len > (uint8_t)(aMaxPHYPacketSize - appNwkParam.frameOverHead)) && (len != 0U))
                    {
//                        SYS_CONSOLE_MESSAGE("\r\n Data Limit reached/not within range\r\n");
                        payloadMaxLimitFlag = true;
                        if(len > 232U)
                        {
                            payloadFragmentLimit = true;
                        }
                    }

                    contDataModeflag = false;
                    break;
                }
            }
            nextLoc += ((uint8_t)nUnreadBytes);
        }
        if(appStatesParam.appOpModes == DATA_MODE)
        {
            if((contDataModeflag == true) || (nUnreadBytes == 0) || (len == 0U))
            {
                appModeSwitch->msgId = (uint8_t)APP_DATA_MODE;//MISRA C-2012 Rule 10.3
                osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
            }
            else if(((contDataModeflag == false) && (payloadMaxLimitFlag == true) && (payloadFragmentLimit == false)))
            {
                SYS_CONSOLE_PRINT("\r\n No of Bytes %d\n",len);
                SYS_CONSOLE_PRINT("\r\n No of Bytes sent in 1st iteration %d\n",(aMaxPHYPacketSize - appNwkParam.frameOverHead));
                app_P2P_Phy_ParseUserdefinedData(readBytesConsole, (aMaxPHYPacketSize - appNwkParam.frameOverHead));    
            }
            else if((payloadMaxLimitFlag == true) && (payloadFragmentLimit == true))
            {
                SYS_CONSOLE_PRINT("\r\n Payload length is beyond Limit, Discarding..\r\n");
                appModeSwitch->msgId = (uint8_t)APP_RESUME_DATA_MODE;
                osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
            }
            else if(((contDataModeflag == false) && (len == 1U) && (readBytesConsole[0] == 0U)))
            {
                appModeSwitch->msgId = (uint8_t)APP_DATA_MODE;
                osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
            }
            else
            { 
                SYS_CONSOLE_PRINT("\r\n No of Bytes %d\n",len);
                app_P2P_Phy_ParseUserdefinedData(readBytesConsole, len);
            }
            osalResult = osalResult;
        }
        break;
        }
                                 
        case APP_RESUME_DATA_MODE:
        {
            memset(readBytesConsole, 0, sizeof(readBytesConsole));
            memset(consoleBuffer, 0, sizeof(consoleBuffer));
            nextLoc = inc = len = 0;
            appStatesParam.appOpModes = DATA_MODE;
            payloadMaxLimitFlag = false;
            payloadFragmentLimit = false;
            dataModeTimerStartFlag = false;
//            sysTimerFlag.sysTimerDataModeHandle = SYS_TIME_TimerCreate(0, SYS_TIME_MSToCount(sysTimerFlag.singleShotSysTimer), &app_P2P_Phy_systimerCallbackDataMode, (uintptr_t)&temp, SYS_TIME_SINGLE);
            appModeSwitch->msgId = (uint8_t)APP_DATA_MODE;
            osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
            osalResult = osalResult;
            break; 
        }
        
        case APP_FRAGMENT_DATA_MODE:
        {
            payloadMaxLimitFlag = false;
            PAL_TimerDelay(10000);
            SYS_CONSOLE_PRINT("\r\n No of Bytes sent in 2nd iteration %d\n",(len - (uint8_t)(aMaxPHYPacketSize - appNwkParam.frameOverHead)));
            app_P2P_Phy_ParseUserdefinedData((readBytesConsole + (aMaxPHYPacketSize - appNwkParam.frameOverHead)), (len - (uint8_t)(aMaxPHYPacketSize - appNwkParam.frameOverHead)));     
            break;
        }
        
        case APP_SYSTIMER_EXPIRY_DATA_ENTRY:
        {
            if(appStatesParam.appOpModes == DATA_MODE)
            {

            if((len > (uint8_t)(aMaxPHYPacketSize - appNwkParam.frameOverHead)) && (len != 0U))
            {
//                SYS_CONSOLE_MESSAGE("\r\n Data Limit reached\r\n");
                payloadMaxLimitFlag = true;
                if(len > 232U)
                {
                    payloadFragmentLimit = true;
                }
            }
            if((contDataModeflag == true) && (payloadMaxLimitFlag == true) && (payloadFragmentLimit == false))
            {
                SYS_CONSOLE_PRINT("\r\n No of Bytes %d\n",len);
                SYS_CONSOLE_PRINT("\r\n No of Bytes sent in 1st iteration %d\n",(aMaxPHYPacketSize - appNwkParam.frameOverHead));
                app_P2P_Phy_ParseUserdefinedData(readBytesConsole, (aMaxPHYPacketSize - appNwkParam.frameOverHead));
            }
            else if((payloadMaxLimitFlag == true) && (payloadFragmentLimit == true))
            {
                SYS_CONSOLE_PRINT("\r\n Payload length is beyond Limit, Discarding..\r\n");
                appModeSwitch->msgId = (uint8_t)APP_RESUME_DATA_MODE;
                osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
                osalResult = osalResult;
            }
            else if((len == 0U) || ((len == 1U) && (readBytesConsole[0] == 0U)))
            {
                appModeSwitch->msgId = (uint8_t)APP_RESUME_DATA_MODE;
                osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
                osalResult = osalResult;
            }
            else
            { 
                SYS_CONSOLE_PRINT("\r\n No of Bytes %d\n",len);
                app_P2P_Phy_ParseUserdefinedData(readBytesConsole, len);
            }
            }
            break;

        }
                                        
        case APP_SYSTIMER_START_PERIODIC_TEST_MODE:
        {
            uint8_t temp = 0;
            memset(appModeSwitch->msgData, 0, sizeof(appModeSwitch->msgData));
            sysTimerFlag.sysTimerPeriodicTestModeHandle = SYS_TIME_CallbackRegisterUS(&app_P2P_Phy_systimerCallbackTestMode, (uintptr_t)&temp, sysTimerFlag.periodicSysTimer, SYS_TIME_SINGLE); 
            if(sysTimerFlag.sysTimerPeriodicTestModeHandle == SYS_TIME_HANDLE_INVALID)
            {
                appModeSwitch->msgId = (uint8_t)APP_SYSTIMER_START_PERIODIC_TEST_MODE;
                osalResult = OSAL_QUEUE_Send(&appData.appQueue, appModeSwitch, 0);
                osalResult = osalResult;
            }
            else
            {
                // SYS_CONSOLE_PRINT("\r\nTimer Started with period %x\r\n",sysTimerFlag.periodicSysTimer);
            }
            break;
        }
                               
        case APP_SYSTIMER_EXPIRY_PERIODIC_TEST_MODE:
        {
            appStatesParam.appOpModes = TEST_MODE;
            app_P2P_Phy_TestMode(PERIOD_TEST_MODE);
            break;
        }
                    
        case APP_CONTINUOUS_DATA_TEST_MODE:
        {
            PAL_TimerDelay(1500000);//Todo: have a macro for timer
            app_P2P_Phy_TestMode(CONT_TEST_MODE);
            break;
        }
                    
        case APP_PER_TEST_MODE:
        {
            app_P2P_Phy_TestMode(PER_TEST_MODE);
            break;
        }
                    
        case APP_PROMISCUOUS_MODE:
        {
            app_P2P_Phy_TestMode(PROM_TEST_MODE);
            break;
        }
    
        case APP_THROUGHPUT_TEST_MODE:
        {
            app_P2P_Phy_TestMode(THROUGHPUT_TEST_MODE);
            break;
        }                     
                    
        default:
        {
            SYS_CONSOLE_MESSAGE("\r\nIDLE MODE\r\n");
            break; 
        }
                    
    }
}

//If device index is 0xff/255 all the valid entries in the table will be displayed
void app_P2P_Phy_GetDeviceTableInfo(uint8_t dev_index)
{  
//    SYS_CONSOLE_PRINT("\r\n Device Index is : %d\n",dev_index);
    if(dev_index != 15U)
    {
            if(deviceTable[dev_index].valid == true)
            {
                SYS_CONSOLE_MESSAGE("\r\nDEVINDX\tSHORTADDR\tLQI\tRSSI\tTXPKTCNT\tRXPKTCNT\tIEEEADDR\r\n");
                SYS_CONSOLE_PRINT("%d\t",dev_index);
                SYS_CONSOLE_PRINT("0x%x\t\t",deviceTable[dev_index].shortAddr); 
                SYS_CONSOLE_PRINT("%d\t",deviceTable[dev_index].lqi);
                SYS_CONSOLE_PRINT("%d\t",deviceTable[dev_index].rssiVal);
                SYS_CONSOLE_PRINT("%8x\t",deviceTable[dev_index].txPacketCnt);
                SYS_CONSOLE_PRINT("%8x\t",deviceTable[dev_index].rxPacketCnt);
                SYS_CONSOLE_PRINT("%8x",deviceTable[dev_index].ieeeAddr);
            }
            else
            {
                SYS_CONSOLE_MESSAGE("\r\nDevice Index is empty\r\n");
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
                SYS_CONSOLE_PRINT("%8x\t",deviceTable[DEST_DEV_INDEX].txPacketCnt);
                SYS_CONSOLE_PRINT("%8x\t",deviceTable[DEST_DEV_INDEX].rxPacketCnt);
                SYS_CONSOLE_PRINT("%8x",deviceTable[i].ieeeAddr);
            }
           
        }
    }
}

void app_P2P_Phy_SetDeviceTableInfo(uint8_t dev_index, DEVICE_TABLE *devTable)
{
    deviceTable[dev_index].ieeeAddr = devTable->ieeeAddr;
    deviceTable[dev_index].shortAddr = devTable->shortAddr; 
    deviceTable[dev_index].valid = true;
}

APP_OP_STATE_T app_P2P_Phy_SetNWKParams(APP_NWK_PARAM *appnwkParam)
{
    PibValue_t pibValue;
    uint8_t tempVar;
    PHY_Retval_t attributeStatus;
    
        pibValue.pib_value_16bit = appnwkParam->srcAddress;
        attributeStatus = PHY_PibSet(macShortAddress, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(macShortAddress,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            appNwkParam.srcAddress = pibValue.pib_value_16bit;
//    SYS_CONSOLE_PRINT("\r\n Source Address : %d \r\n",appNwkParam.srcAddress);
        }
    

        pibValue.pib_value_16bit = appnwkParam->srcPanId;
        attributeStatus = PHY_PibSet(macPANId, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(macPANId,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            appNwkParam.srcPanId = pibValue.pib_value_16bit;
//    SYS_CONSOLE_PRINT("\r\n PAN ID : %x \r\n",appNwkParam.srcPanId);
        }

    
    if(appnwkParam->cca.ccaMode > 0xdU)
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        pibValue.pib_value_8bit = appnwkParam->cca.ccaMode;
        attributeStatus = PHY_PibSet(phyCCAMode, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(phyCCAMode,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            appNwkParam.cca.ccaMode = pibValue.pib_value_8bit;
//    SYS_CONSOLE_PRINT("\r\n CCA mode : %d \r\n",appNwkParam.cca.ccaMode);
        }
    }
    
    if((appnwkParam->channel < 0xbU) || (appnwkParam->channel > 0x1aU))
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        pibValue.pib_value_8bit = appnwkParam->channel;
        attributeStatus = PHY_PibSet(phyCurrentChannel, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(phyCurrentChannel,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
       {
        appNwkParam.channel = pibValue.pib_value_8bit;
//    SYS_CONSOLE_PRINT("\r\n Channel : %d \r\n",appNwkParam.channel);
       }
    }

    if((appnwkParam->channelPage != 0x0U) && (appnwkParam->channelPage != 0x2U) && (appnwkParam->channelPage != 0x10U) && (appnwkParam->channelPage != 0x11U))
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        pibValue.pib_value_8bit = appnwkParam->channelPage;
        attributeStatus = PHY_PibSet(phyCurrentPage, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
       { 
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(phyCurrentPage,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            appNwkParam.channelPage = pibValue.pib_value_8bit;
//        SYS_CONSOLE_PRINT("\r\n Channel pg : %d \r\n",appNwkParam.channelPage);
        }
    }
        pibValue.pib_value_64bit = appnwkParam->ieeeAddress;
        attributeStatus = PHY_PibSet(macIeeeAddress,&pibValue);
    if(attributeStatus != PHY_SUCCESS)
    {
        appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
        appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
        return appStatesParam.appOpStates;
    }
    attributeStatus = PHY_PibGet(macIeeeAddress,(uint8_t *)&pibValue);
    if(attributeStatus == PHY_SUCCESS)
    {
        appNwkParam.extendedSrcAddress = appNwkParam.ieeeAddress = pibValue.pib_value_64bit;
//        SYS_CONSOLE_PRINT("%8x",appNwkParam.ieeeAddress);
//        memcpy (&appNwkParam.ieeeAddress, &pibValue.pib_value_64bit, sizeof(pibValue.pib_value_64bit));
    }
    
    if((appnwkParam->maxBE < 0x3U) || (appnwkParam->maxBE > 0x8U) )
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        pibValue.pib_value_8bit = appnwkParam->maxBE;
        attributeStatus = PHY_PibSet(macMaxBE, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(macMaxBE,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            appNwkParam.maxBE = pibValue.pib_value_8bit;
//    SYS_CONSOLE_PRINT("\r\n MAX BE : %d \r\n",appNwkParam.maxBE);
        }
    }
    
    if(appnwkParam->minBE > 0x3U)
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        pibValue.pib_value_8bit = appnwkParam->minBE;
        attributeStatus = PHY_PibSet(macMinBE, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(macMinBE,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            appNwkParam.minBE = pibValue.pib_value_8bit;
//    SYS_CONSOLE_PRINT("\r\n MIN BE : %d \r\n",appNwkParam.minBE);
        }
    }

    if((appnwkParam->txPowerDBm < -12) || (appnwkParam->txPowerDBm > 14))
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        appNwkParam.txPowerDBm = appnwkParam->txPowerDBm;
        tempVar = (uint8_t)CONV_DBM_TO_phyTransmitPower(appnwkParam->txPowerDBm);
        pibValue.pib_value_8bit = tempVar;
        attributeStatus = PHY_PibSet(phyTransmitPower, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(phyTransmitPower,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
       {
            appNwkParam.txPower = pibValue.pib_value_8bit;	
//    SYS_CONSOLE_PRINT("\r\n TX Power : %d \r\n",appNwkParam.txPowerDBm);
       }        
    }

    appNwkParam.csma.csmaMode = appnwkParam->csma.csmaMode;
    appNwkParam.dstAddress = appnwkParam->dstAddress;
    appNwkParam.edScan.edscanDurationSYMBOLS = appnwkParam->edScan.edscanDurationSYMBOLS;
    appNwkParam.dstIeeeAddress = appnwkParam->dstIeeeAddress;
    if(appnwkParam->frameRetry > 7U)
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        pibValue.pib_value_8bit = appnwkParam->frameRetry;
        attributeStatus = PHY_PibSet(macMaxFrameRetries, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(macMaxFrameRetries,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
       {
            appNwkParam.frameRetry = pibValue.pib_value_8bit;
//    SYS_CONSOLE_PRINT("\r\n MIN BE : %d \r\n",appNwkParam.minBE);
       }       
    }
    
    pibValue.pib_value_bool = appnwkParam->PromiscuousMode;
    attributeStatus = PHY_PibSet(macPromiscuousMode, &pibValue);
    if(attributeStatus != PHY_SUCCESS){
        appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
        appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
        return appStatesParam.appOpStates;
    }
    attributeStatus = PHY_PibGet(macPromiscuousMode,(uint8_t *)&pibValue);
    if(attributeStatus == PHY_SUCCESS){
        appNwkParam.PromiscuousMode = pibValue.pib_value_bool;
//    SYS_CONSOLE_PRINT("\r\n MIN BE : %d \r\n",appNwkParam.minBE);
    }
    
    if(appnwkParam->csma.csmaBackoff > 5U)
    {
        appStatesParam.appOpStates = APP_PIB_ATTR_OUT_OF_RANGE; 
        return appStatesParam.appOpStates;
    }
    else
    {
        pibValue.pib_value_8bit = appnwkParam->csma.csmaBackoff;
        attributeStatus = PHY_PibSet(macMaxCSMABackoffs, &pibValue);
        if(attributeStatus != PHY_SUCCESS)
        {
            appPhyCmdProcessor_PhyStatusPrint(attributeStatus);
            appStatesParam.appOpStates = APP_PIB_ATTR_SET_FAILURE; 
            return appStatesParam.appOpStates;
        }
        attributeStatus = PHY_PibGet(macMaxCSMABackoffs,(uint8_t *)&pibValue);
        if(attributeStatus == PHY_SUCCESS)
        {
            appNwkParam.csma.csmaBackoff = pibValue.pib_value_8bit;
//    SYS_CONSOLE_PRINT("\r\n MIN BE : %d \r\n",appNwkParam.minBE);
        }  
    }

    appNwkParam.ackReq = appnwkParam->ackReq;
    
    appStatesParam.appOpStates = APP_PIB_ATTR_SET_SUCCESS;
    return appStatesParam.appOpStates;
}

void app_P2P_Phy_TestMode(APP_TEST_MODE_T testStates)
{
    SYS_CONSOLE_MESSAGE("\n===============================================\n");
    appStatesParam.appOpModes = TEST_MODE;
    switch(testStates)
    {
        case PER_TEST_MODE:
        {
            app_P2P_Phy_TransmitPredefinedData(true);
            break;
        }
        
        case PERIOD_TEST_MODE:
        {     
            app_P2P_Phy_TransmitPredefinedData(true);           
            break;
        }
        
        case CONT_TEST_MODE:
        {
            app_P2P_Phy_TransmitPredefinedData(true);
            break;
        }
        case PROM_TEST_MODE:
        {
            PibValue_t pibValue;
            PHY_Retval_t retVal;
            PHY_TrxStatus_t rxStatus;
            bool mode = true;
            retVal = PHY_ConfigRxPromiscuousMode(true);
            pibValue.pib_value_bool = mode;
            retVal = PHY_PibSet(macPromiscuousMode, &pibValue);
            retVal = retVal;
            /*RX_AACK_ON Mode is enabled if Promiscuous Mode is not used,else RX is switched on in RX_ON Mode*/
            rxStatus = PHY_RxEnable(PHY_STATE_RX_ON); 
            appPhyCmdProcessor_PhyTrxStatusPrint(rxStatus);
            break;
        }
        case THROUGHPUT_TEST_MODE:
        {
            app_P2P_Phy_TransmitPredefinedData(true);
            break;
        }
        default:
        {
            SYS_CONSOLE_MESSAGE("\r\nNo test mode selected\r\n");
            break;
        }
    }
}

void app_P2P_Phy_TransmitPredefinedData(bool isAckReq)
{
        
    static uint8_t seqNum = 0; 
    uint8_t txData[TX_BUFFER_SIZE];
    memset(txData, 0, 116);
    memset(txData, (int)'a', 116);
    if(isAckReq == true)
    {
        if(deviceTable[DEST_DEV_INDEX].valid == true)
        {
//            SYS_CONSOLE_PRINT("\r Unicast Data to first device in device table\n");
            appNwkParam.unicastFlagPredefData = true;
            app_P2P_Phy_TransmitFrame(txData, MAX_PAYLOAD_SIZE, 1, seqNum++, true, true, true);
        }    
        else
        {
//            SYS_CONSOLE_PRINT("\r Broadcast Data\n");
            appNwkParam.unicastFlagPredefData = false;
            app_P2P_Phy_TransmitFrame(txData, MAX_PAYLOAD_SIZE, 0xf, seqNum++, false, false, false);
        } 
    }
    else
    {
        if(deviceTable[DEST_DEV_INDEX].valid == true)
        {
//            SYS_CONSOLE_PRINT("\r Unicast Data to first device in device table\n");
            appNwkParam.unicastFlagPredefData = true;
            app_P2P_Phy_TransmitFrame(txData, MAX_PAYLOAD_SIZE, 1, seqNum++, true, false, true);
        }    
        else
        {
//            SYS_CONSOLE_PRINT("\r Broadcast Data\n");
            appNwkParam.unicastFlagPredefData = false;
            app_P2P_Phy_TransmitFrame(txData, MAX_PAYLOAD_SIZE, 0xf, seqNum++, false, false, false);
        } 
    }
}

void app_P2P_Phy_ParseUserdefinedData(uint8_t *userData, uint8_t userDataLen){
    
    
    uint8_t txBuffer[TX_BUFFER_SIZE], deviceIndex, txBufferLen=0;//use global buffer better to make that buffer as local
    static uint8_t seqNum = 0;   
    memset(txBuffer, 0, sizeof(txBuffer));   
    if(*(userData+3) == 58U)
    {
        for(uint8_t i = 4; i<userDataLen; i++)
        {
            txBuffer[i-4U] = *(userData+i);   
        }
        txBufferLen = userDataLen - 4U;
        if(((*userData == 102U) && (*(userData+1) == 102U)) || ((*userData == 70U) && (*(userData+1) == 70U)))
        {
//            SYS_CONSOLE_PRINT("\r Broadcast Data\n");
            appNwkParam.unicastFlagUserDefData = false;
            app_P2P_Phy_TransmitFrame(txBuffer, txBufferLen, 0xfU, seqNum++, false, false, false);               
        }
        else
        {
            deviceIndex = (((*userData) - 48U)*10U) + ((*(userData+1)) - 48U);
            if(deviceIndex < NUM_OF_DEVICES)
            {
                if(deviceTable[deviceIndex].valid == true)
                {
//                    SYS_CONSOLE_PRINT("\r Unicast Data to device %d in device table\n",deviceIndex);
                    appNwkParam.unicastFlagUserDefData = true;
                    app_P2P_Phy_TransmitFrame(txBuffer, txBufferLen, deviceIndex, seqNum++, true, true, true);
                }
                else
                {
//                    SYS_CONSOLE_PRINT("\r Broadcast Data\n");
                    appNwkParam.unicastFlagUserDefData = false;
                    app_P2P_Phy_TransmitFrame(txBuffer, txBufferLen, 0xfU, seqNum++, false, false, false);
                }
            }
            else
            {
                SYS_CONSOLE_PRINT("\r Device Index Out of Range\n");
                if(deviceTable[DEST_DEV_INDEX].valid == true)
                {
//                    SYS_CONSOLE_PRINT("\r Unicast Data to first device in device table\n");
                    appNwkParam.unicastFlagUserDefData = true;
                    app_P2P_Phy_TransmitFrame(txBuffer, txBufferLen, DEST_DEV_INDEX, seqNum++, true, true, true);
                }
                else
                {
//                    SYS_CONSOLE_PRINT("\r Broadcast Data\n");
                    appNwkParam.unicastFlagUserDefData = false;
                    app_P2P_Phy_TransmitFrame(txBuffer, txBufferLen, 0xfU, seqNum++, false, false, false);
                } 
            }
        }
    }
    else
    {
        if(deviceTable[DEST_DEV_INDEX].valid == true)
        {
//            SYS_CONSOLE_PRINT("\r Unicast Data to first device in device table\n");
            appNwkParam.unicastFlagUserDefData = true;
            app_P2P_Phy_TransmitFrame(userData, userDataLen, DEST_DEV_INDEX, seqNum++, true, true, true);
        }
        else
        {
//            SYS_CONSOLE_PRINT("\r Broadcast Data\n");
            app_P2P_Phy_TransmitFrame(userData, userDataLen, 0xfU, seqNum++, false, false, false);
        }          
    }
    memset(txBuffer, 0, sizeof(txBuffer));  
}
/*
 * \brief Function to transmit frames as per 802.15.4 std.
 *
 * \param dst_addr_mode     destination address mode - can be 16 or 64 bit
 * \param dst_addr          destination address
 * \param src_addr_mode     source address mode - can be 16 or 64 bit
 * \param msdu_handle       msdu handle for the upper layers to track packets
 * \param payload           data payload pointer
 * \param payload_length    data length
 * \param ack_req           specifies ack requested for frame if set to 1
 * \param csma_mode         specifies the csma mode for transmission
 * \param frame_retry       specifies whether frame retry is required or not
 *
 * \return PHY_SUCCESS      if the PHY has accepted the data for frame
 * transmission
 *         PHY_BUSY         if the PHY is busy servicing the previous tx request
 */
void app_P2P_Phy_TransmitFrame(uint8_t *payload, uint8_t payloadLength, uint8_t devIndex, uint8_t msduHandle, bool unicast, bool ackReq, bool frameRetry)
{     
    PHY_FrameInfo_t txFrameInfo;//naming convention shd be taken care
	uint8_t frameLength = 0;
	uint8_t *framePtr, *tempFramePtr;
	uint16_t tempValue = 0, frameControlField = 0, dstShortAddr = 0, srcShortAddr = 0;
    uint64_t dstLongAddr = 0, srcLongAddr = 0;
    static uint8_t storage_buffer[LARGE_BUFFER_SIZE];
//	txFrameInfo = (PHY_FrameInfo_t *)storage_buffer;
	/* Get length of current frame. */
	frameLength = (appNwkParam.frameOverHead + payloadLength); 
	/* Set payload pointer. */     
	framePtr = tempFramePtr = storage_buffer + LARGE_BUFFER_SIZE - payloadLength - FCS_LEN;
	/*Payload is stored to the end of the buffer avoiding payload copying by TAL.*/
	for (uint8_t i = 0; i < payloadLength; i++) 
    {
	    *tempFramePtr++ = *(payload + i);
	}
	/* Source address */
	if (appNwkParam.srcAddrMode == FCF_SHORT_ADDR)
    {
	    framePtr -= SHORT_ADDR_LEN;
        srcShortAddr = appNwkParam.srcAddress;
	    convert_16_bit_to_byte_array(srcShortAddr, framePtr);
	    frameControlField |= FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR);
	} 
    else 
    {
	   framePtr -= EXT_ADDR_LEN;
	   frameLength += (uint8_t)FCF_2_SOURCE_ADDR_OFFSET;
       srcLongAddr = appNwkParam.extendedSrcAddress;
	   convert_64_bit_to_byte_array(srcLongAddr, framePtr);
	   frameControlField |= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR);
	}
    /* Source PAN-Id */
	#if (DST_PAN_ID == SRC_PAN_ID)
		/* No source PAN-Id included, but FCF updated. */
		frameControlField |= FCF_PAN_ID_COMPRESSION;
	#else
		framePtr -= PAN_ID_LEN;
		tempValue = (SRC_PAN_ID);
		convert_16_bit_to_byte_array(tempValue, framePtr);
	#endif
	/* Destination address */
	if (appNwkParam.dstAddrMode == FCF_SHORT_ADDR) 
    {
		framePtr -= SHORT_ADDR_LEN;
        if(unicast == false)
        {
            dstShortAddr = appNwkParam.dstAddress = (uint16_t)BROADCAST;
        }
        else
        {
            dstShortAddr = appNwkParam.dstAddress = deviceTable[devIndex].shortAddr;    
        }    
		convert_16_bit_to_byte_array(dstShortAddr,framePtr);
		frameControlField |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);
	}
    else 
    {
		framePtr -= EXT_ADDR_LEN;
        if(unicast == false)
        {
            dstLongAddr = appNwkParam.extendedDstAddress = (uint64_t)BROADCAST;
        }
        else
        {
            dstLongAddr = appNwkParam.extendedDstAddress = deviceTable[devIndex].ieeeAddr;    
        }  
		frameLength += (uint8_t)PL_POS_DST_ADDR_START;
		convert_64_bit_to_byte_array(dstLongAddr,framePtr);
		frameControlField |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);
	}
	/* Destination PAN-Id */
	tempValue = DST_PAN_ID;
	framePtr -= PAN_ID_LEN;
	convert_16_bit_to_byte_array(tempValue, framePtr);

	/* Set DSN. */
	framePtr--;
	*framePtr = msduHandle;

	/* Set the FCF. */
	frameControlField |= FCF_FRAMETYPE_DATA;
	if (ackReq) 
    {
        appNwkParam.ackReq = true;
	    frameControlField |= FCF_ACK_REQUEST;
	}
	framePtr -= (uint8_t)FCF_LEN;
	convert_16_bit_to_byte_array(frameControlField, framePtr);
	/* First element shall be length of PHY frame. */
	framePtr--;
	*framePtr = frameLength;
	/* Finished building of frame. */
	txFrameInfo.mpdu = framePtr;
	appNwkParam.txStatusFrameSent = PHY_TxFrame(&txFrameInfo, appNwkParam.csma.csmaMode, frameRetry);
    if(appNwkParam.txStatusFrameSent == PHY_SUCCESS)
    {
        appNwkParam.nPacketsSent += 1U;
        if((modeSwitchFlag.enterPerTestMode == true) && (appNwkParam.nPacketsSent == appNwkParam.nPerTestPackets))
        {
            modeSwitchFlag.perTestMode = true;
        }
        SYS_CONSOLE_PRINT("\r\nTX Done, PKT No. %d\r\n", msduHandle);
    }
    else
    {
        SYS_CONSOLE_PRINT("\r\nPHY Busy, PKT No. %d\r\n", msduHandle);
    } 
    app_P2P_Phy_appModePostDataTxSwitchHandler();
}

void app_P2P_Phy_setAddressModes(uint8_t srcAddressMode, uint8_t dstAddressMode)
{    
    if((srcAddressMode <= 3U) && (dstAddressMode <= 3U))
    {
        appNwkParam.srcAddrMode = srcAddressMode;
        appNwkParam.dstAddrMode = dstAddressMode;  
    }
    else
    {
        SYS_CONSOLE_PRINT("\r\n Address Mode Out of Range\r\n");
    }
    
    if((srcAddressMode == (uint8_t)FCF_SHORT_ADDR) && (dstAddressMode == (uint8_t)FCF_SHORT_ADDR))
    {
        appNwkParam.frameOverHead = 11U;
    }
    else if((srcAddressMode == (uint8_t)FCF_LONG_ADDR) && (dstAddressMode == (uint8_t)FCF_LONG_ADDR))
    {
        appNwkParam.frameOverHead = 23U;
    }
    else
    {
        appNwkParam.frameOverHead = 18U;
    }
}

void app_P2P_Phy_setFrameType(uint8_t frameType)
{
    if(frameType <= 5U)
    {
        appNwkParam.frameType = frameType;  
    }
    else
    {
        SYS_CONSOLE_PRINT("\r\n FrameType Out of Range\r\n");
    }
}


void PHY_TxDoneCallback(PHY_Retval_t status, PHY_FrameInfo_t *frame)
{
    
    appNwkParam.txDoneCbStatus = status;
    appPhyCmdProcessor_PhyStatusPrint(appNwkParam.txDoneCbStatus);
    appNwkParam.nBytesSent += frame->mpdu[0];
    if(appNwkParam.ackReq == true)
    {
        if((appNwkParam.txDoneCbStatus == PHY_SUCCESS) && ((appNwkParam.unicastFlagUserDefData == true) || (appNwkParam.unicastFlagPredefData == true)))
        {
            SYS_CONSOLE_PRINT("\r\nACK RCVD for PKT: %d\r\n", frame->mpdu[3]);
            appNwkParam.nPacketsRcvdAck += 1U;
            deviceTable[SOURCE_DEV_INDEX].txPacketCnt += 1U;
        }
        else
        {
            SYS_CONSOLE_PRINT("\r\nNo ACK for PKT: %d\r\n", frame->mpdu[3]);
        }
    }
    SYS_CONSOLE_PRINT("\r\nTXPKTCNT: ");
    SYS_CONSOLE_PRINT("%8x\n",deviceTable[SOURCE_DEV_INDEX].txPacketCnt);
    SYS_CONSOLE_MESSAGE("\r\n");
    app_P2P_Phy_appModePostDataTxDoneCbSwitchHandler();
}

void PHY_RxFrameCallback(PHY_FrameInfo_t *rxFrame)
{
    uint8_t recBuffer[LARGE_BUFFER_SIZE], seqNumber=0;
    memset(recBuffer, 0, sizeof(recBuffer));
    uint8_t payloadLength = (uint8_t)rxFrame->mpdu[0];
    for(uint8_t i = 0; i<payloadLength; i++)
    {
        recBuffer[i] = rxFrame->mpdu[appNwkParam.frameOverHead-FCS_LEN+1U+i];
    }
    int8_t recRSSI =  (int8_t )(rxFrame->mpdu[payloadLength + LQI_LEN + ED_VAL_LEN]) + PHY_GetRSSIBaseVal();
    uint8_t LQI = rxFrame->mpdu[payloadLength + LQI_LEN];
    seqNumber = rxFrame->mpdu[3];
    // Free-up the buffer which was used for reception once the frame is extracted.
	bmm_buffer_free(rxFrame->buffer_header);
    payloadLength = payloadLength - appNwkParam.frameOverHead;
    SYS_CONSOLE_PRINT("\r\nRXPKT <pktno: %d>\n", seqNumber);
    SYS_CONSOLE_PRINT("\r\nLen %d\r\n",payloadLength);
    SYS_CONSOLE_PRINT("\r\n");
    for(uint8_t i = 0; i<payloadLength; i++)
    {
        SYS_CONSOLE_PRINT("%c",recBuffer[i]);
    }
    SYS_CONSOLE_MESSAGE("\n"); 
    deviceTable[SOURCE_DEV_INDEX].rxPacketCnt  +=  1U;
    deviceTable[SOURCE_DEV_INDEX].lqi = LQI;
    deviceTable[SOURCE_DEV_INDEX].rssiVal = recRSSI;
    SYS_CONSOLE_PRINT("\r\nLQI:%i", LQI);    
    SYS_CONSOLE_PRINT("\r\nRSSI:%i", recRSSI);
    SYS_CONSOLE_PRINT("\r\nPKTCNT:");
    SYS_CONSOLE_PRINT("%8x\n",deviceTable[SOURCE_DEV_INDEX].rxPacketCnt);
    SYS_CONSOLE_MESSAGE("\r\n");
}

/** 
 * \brief The Receiver is switched on Using this function,
  * When PROMISCUOUS_MODE is enabled the receiver is put in RX_ON mode ,else it is switched on in RX_AACK_ON Mode
 */
void app_P2P_Phy_ConfigDataReception(void)
{
   /*Enable Promiscuous Mode pib attribute to put the transceiver in RX_ON mode.*/
   PHY_TrxStatus_t rxStatus;
   #ifdef PROMISCUOUS_MODE
       PibValue_t pibVal;
       PHY_Retval_t retVal;
       appNwkParam.PromiscuousMode = true;
       pibVal.pib_value_bool = true;
       retVal = PHY_ConfigRxPromiscuousMode(true);
       retVal = PHY_PibSet(macPromiscuousMode, &pibVal);
       retVal = retVal;
   #endif
   /*RX_AACK_ON Mode is enabled if Promiscuous Mode is not used,else RX is switched on in RX_ON Mode*/
   rxStatus = PHY_RxEnable(PHY_STATE_RX_ON); 
   rxStatus = rxStatus;
   
}


/**
 *
 * \brief This function starts the ED scan on the current channel
 */
PHY_Retval_t app_P2P_Phy_StartEdScan(void)
{
    PHY_Retval_t status;
    appNwkParam.edScan.edscanDurationSYMBOLS = ED_SCAN_DURATION;
    status = PHY_EdStart(ED_SCAN_DURATION);
    appNwkParam.edScan.edStatus = status;
    return status;
}

void app_P2P_Phy_systimerCallbackTestMode(uintptr_t context)
{
    context = context;
    APP_Msg_T *p_appModes;
    APP_Msg_T appModes;
    p_appModes = &appModes;
    p_appModes->msgId = (uint8_t)APP_SYSTIMER_EXPIRY_PERIODIC_TEST_MODE;
    osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);   
    osalResult = osalResult;
}

void app_P2P_Phy_systimerCallbackDataMode(uintptr_t context)
{
    context = context;
    APP_Msg_T *p_appModes;
    APP_Msg_T appModes;
    p_appModes = &appModes;
    p_appModes->msgId = (uint8_t)APP_SYSTIMER_EXPIRY_DATA_ENTRY;
    osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    osalResult = osalResult;
}

void app_P2P_Phy_systimerCallbackThroughputTestMode(uintptr_t context)
{
    context = context;
    modeSwitchFlag.throughputTimerCallback = true;
}

void app_P2P_Phy_packetErrorRateCal(uint64_t nPacketsSent, uint64_t nBytesSent, uint64_t nPacketsRecAck)
{
    uint64_t nBytes=0;
    double perValIntm = 0.0;
    double per_val;
    SYS_CONSOLE_PRINT("\r\n PKTS SENT 0x%8x\r\n",nPacketsSent);
    nBytes = nBytesSent;
    SYS_CONSOLE_PRINT("\r\n Bytes 0x%8x\r\n",nBytes);
    SYS_CONSOLE_PRINT("\r\n PKTS WITH ACK 0x%8x\r\n",nPacketsRecAck);
    perValIntm = (((double)nPacketsSent - (double)nPacketsRecAck)/(double)(nPacketsSent));
    per_val = perValIntm * (double)100;
    SYS_CONSOLE_PRINT("\r\n PER Value %f percent\n",per_val);
    SYS_CONSOLE_MESSAGE("\r\n");
}

void app_P2P_Phy_appModePostDataTxSwitchHandler(void)
{
    APP_Msg_T *p_appModes;
    APP_Msg_T appModes;
    p_appModes = &appModes;
    if((modeSwitchFlag.enterDataMode == true) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == false))
    {
        if(payloadMaxLimitFlag == false)
        {
            p_appModes->msgId = (uint8_t)APP_RESUME_DATA_MODE;
            osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
        }
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == true) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == false))
    {
        p_appModes->msgId = (uint8_t)APP_SYSTIMER_START_PERIODIC_TEST_MODE;
        osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    }
    else
    {
        //Do nothing
    }
            osalResult = osalResult;
}
void app_P2P_Phy_appModePostDataTxDoneCbSwitchHandler(void)
{
    APP_Msg_T *p_appModes;
    APP_Msg_T appModes;
    p_appModes = &appModes;
    if((modeSwitchFlag.enterDataMode == true) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == false))
    {
        if(payloadMaxLimitFlag == true)
        {
            if(appNwkParam.txDoneCbStatus == PHY_SUCCESS)
            {
                p_appModes->msgId = (uint8_t)APP_FRAGMENT_DATA_MODE;
                osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
            }
        }
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == true) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == false))
    {
        p_appModes->msgId = (uint8_t)APP_CONTINUOUS_DATA_TEST_MODE;
        osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == true) && (modeSwitchFlag.enterPerTestMode == false))
    {
        if(modeSwitchFlag.throughputTimerCallback == true)
        {
            PAL_TimerDelay(1500000);
            appPhyCmdProcessor_calculateDevicePerfParams();
        }
        else
        {
            p_appModes->msgId = (uint8_t)APP_THROUGHPUT_TEST_MODE;
            osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
        } 
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == true))
    {
        if(modeSwitchFlag.perTestMode == true)
        {
            PAL_TimerDelay(1500000);
            appPhyCmdProcessor_calculateDevicePerfParams();
        }
        else
        {
            p_appModes->msgId = (uint8_t)APP_PER_TEST_MODE;
            osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
        }
    }
    else
    {
        //do nothing
    }
            osalResult = osalResult;
}
void app_P2P_Phy_appModeSwitchHandler(void)
{
    APP_Msg_T *p_appModes;
    APP_Msg_T appModes;
    p_appModes = &appModes;
    if((modeSwitchFlag.enterDataMode == true) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == false))
    {
        p_appModes->msgId = (uint8_t)APP_CMD_TASK_SUSPEND;
        osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == true) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == false))
    {
        p_appModes->msgId = (uint8_t)APP_CONTINUOUS_DATA_TEST_MODE;
        osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == true) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == false))
    {
        p_appModes->msgId = (uint8_t)APP_SYSTIMER_START_PERIODIC_TEST_MODE;
        osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == true) && (modeSwitchFlag.enterPerTestMode == false))
    {
        p_appModes->msgId = (uint8_t)APP_THROUGHPUT_TEST_MODE;
        osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0); 
    }
    else if((modeSwitchFlag.enterDataMode == false) && (modeSwitchFlag.enterContTestMode == false) && (modeSwitchFlag.enterPeriodicTestMode == false) && (modeSwitchFlag.enterThroughputTestMode == false) && (modeSwitchFlag.enterPerTestMode == true))
    {
        p_appModes->msgId = (uint8_t)APP_PER_TEST_MODE;
        osalResult = OSAL_QUEUE_Send(&appData.appQueue, p_appModes, 0);
    }
    else
    {
        //handle error
    }
            osalResult = osalResult;
}

void app_P2P_Phy_stopActiveModes(void)
{
    SYS_TIME_RESULT timerDestroy;
    timerDestroy = SYS_TIME_TimerDestroy(sysTimerFlag.sysTimerPeriodicTestModeHandle);
    timerDestroy = SYS_TIME_TimerDestroy(sysTimerFlag.sysTimerDataModeHandle);
    timerDestroy = SYS_TIME_TimerDestroy(sysTimerFlag.sysTimerThroughputHandle);
    appPhyCmdProcessor_SetModeFlags(false,false,false,false,false,false,false);
    PAL_TimerDelay(1700000);
    appPhyCmdProcessor_calculateDevicePerfParams();
    timerDestroy = timerDestroy;
}

APP_NWK_PARAM *app_P2P_Phy_AssignNWKParams(void)
{
        static APP_NWK_PARAM appnwkparam;
        APP_NWK_PARAM *pAppNwkParam;
        pAppNwkParam = &appnwkparam;
        appnwkparam.srcPanId = (uint16_t)SRC_PAN_ID;
        appnwkparam.channel = CHANNEL_TRANSMIT_RECEIVE;
        appnwkparam.channelPage = CHANNEL_PAGE_TRANSMIT_RECEIVE;
        appnwkparam.cca.ccaMode = CCA_MODE;
        appnwkparam.txPowerDBm = TRANSMIT_POWER;
        appnwkparam.srcAddress = (uint16_t)SRC_ADDR;
        appnwkparam.ieeeAddress = (uint64_t)IEEE_ADDR;
        appnwkparam.ackReq = ACK_REQ;
        appnwkparam.frameRetry = FRAME_RETRY;
        appnwkparam.csma.csmaMode = CSMA_MODE;
        appnwkparam.dstAddress = (uint16_t)DST_ADDR;
        appnwkparam.csma.csmaBackoff = CSMA_BACKOFF;
        appnwkparam.edScan.edscanDurationSYMBOLS = ED_SCAN_DURATION;
        appnwkparam.maxBE = MAXBE;
        appnwkparam.minBE = MINBE;
        appnwkparam.PromiscuousMode = false;
        appNwkParam.extendedDstAddress = appnwkparam.dstIeeeAddress = (uint64_t)DST_IEEE_ADDR;
        return pAppNwkParam;
}





