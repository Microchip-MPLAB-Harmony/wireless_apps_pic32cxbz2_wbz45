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
#ifndef APP_P2P_PHY_H
#define	APP_P2P_PHY_H

#include <string.h>
#include <ctype.h>
#include "definitions.h"
#include "pal.h"
#include "../../stack_config.h"

#ifdef	__cplusplus
extern "C" {
#endif

/*Macros*/

//Transeiver Configuration
#define TRANSMITTER_ENABLED
#define DST_PAN_ID      0xCAFEU
#define CHANNEL_PAGE_TRANSMIT_RECEIVE	0U
#define ACK_REQ			1
#define ENABLE_ANTENNA_1                          (1U)
#define ENABLE_ANTENNA_2                          (2U)
#define ANT_SELECT					ENABLE_ANTENNA_2
#define ENABLE_ANTENNA_DIVERSITY	false
#define FRAME_OVERHEAD          (11U)
#define FRAME_OVERHEAD_EXTENDED_ADDR_MODE  (23U)
#define FRAME_OVERHEAD_EXTENDED_SRC_ADDR_MODE (18U)
#define FRAME_OVERHEAD_EXTENDED_DST_ADDR_MODE (18U)
#define TX_BUFFER_SIZE   120U
#define MAX_PAYLOAD_SIZE 116U
#define CSMA_MODE		CSMA_UNSLOTTED
#define SOURCE_DEV_INDEX 0
#define DEST_DEV_INDEX 1
/*Application Operating Modes*/
#define CMD_MODE                         (0U)                         
#define DATA_MODE                        (1U)
#define TEST_MODE                        (2U)


#define LINE_TERM                         "\r\n" 
#define PHY_COMMANDS_IN_CONSOLE   
//#define PROMISCUOUS_MODE 

typedef enum
{
    APP_STATE_TX_SUCCESS,
    APP_STATE_TX_IN_PROGRESS,
    APP_STATE_TX_FAILURE,
    APP_STATE_TX_ACK,
    APP_STATE_TX_NO_ACK,
    APP_STATE_RX_SUCCESS,
    APP_CCA_CHANNEL_ACCESS_SUCCESS,
    APP_CCA_CHANNEL_ACCESS_FAILURE,
    APP_ED_SCAN_INIT,
    APP_ED_SCAN_IN_PROGRESS,
    APP_PIB_ATTR_SET_SUCCESS,
    APP_PIB_ATTR_SET_FAILURE,
    APP_PIB_ATTR_OUT_OF_RANGE,
    APP_TIMER_START,
    APP_TIMER_IN_PROGRESS,
    APP_TIMER_EXPIRED,
    APP_TIMER_STOPPED,
            
}APP_OP_STATE_T;

/*APP Structures*/    
typedef struct
{
    APP_OP_STATE_T appOpStates;
    uint8_t appOpModes;
}APP_STATES_PARAM;


typedef struct
{
    SYS_TIME_RESULT timerStartPeriodicTestMode, timerStartDataMode, timerStopPeriodicTestMode, timerStopDataMode;
    SYS_TIME_HANDLE sysTimerPeriodicTestModeHandle, sysTimerDataModeHandle,sysTimerThroughputHandle;
    uint32_t periodicSysTimer;
    uint32_t singleShotSysTimer;
    uint32_t throughputSysTimer;
}SYSTIMER_FLAGS;

extern SYSTIMER_FLAGS sysTimerFlag;

typedef struct
{
    uint8_t ccaMode;
    PHY_Retval_t ccaStatus;
}CCA;

typedef struct
{
    uint8_t edscanDurationSYMBOLS;
    PHY_Retval_t edStatus;
}ED_SCAN;

typedef struct
{
    PHY_CSMAMode_t csmaMode;
    uint8_t csmaBackoff;
}CSMA;

typedef struct
{
    uint8_t channel;
    uint8_t channelPage;
    uint16_t srcPanId;
    uint16_t dstPanId;
    uint16_t srcAddress;
    uint64_t extendedSrcAddress;
    uint8_t srcAddrMode;
    uint16_t dstAddress;
    uint64_t extendedDstAddress;
    uint8_t dstAddrMode;
    int8_t txPowerDBm;
    uint8_t txPower;
    uint64_t ieeeAddress;
    uint64_t dstIeeeAddress;
    bool unicastFlagPredefData;
    bool unicastFlagUserDefData;
    PHY_Retval_t pibAttributeStatus;
    ED_SCAN edScan;
    CCA cca;
    CSMA csma;
    bool PromiscuousMode;
    uint8_t frameRetry;
    uint8_t maxBE;
    uint8_t minBE;
    bool ackReq;
    uint8_t frameType;
    uint8_t frameOverHead;
    uint64_t nPacketsSent;
    uint64_t nBytesSent;
    uint64_t nPacketsRcvdAck;
    PHY_Retval_t txStatusFrameSent;
    PHY_Retval_t txDoneCbStatus;
    uint64_t nPerTestPackets;
} APP_NWK_PARAM;

extern APP_NWK_PARAM appNwkParam;

typedef struct 
{
    uint64_t ieeeAddr; 
    uint16_t shortAddr;
    int8_t rssiVal; 
    uint8_t lqi;
    uint64_t txPacketCnt; 
    uint64_t rxPacketCnt; 
    bool valid; 
}DEVICE_TABLE;

extern DEVICE_TABLE deviceTable[NUM_OF_DEVICES]; 

typedef struct
{
    bool enterDataMode;
    bool enterPeriodicTestMode;
    bool enterContTestMode;
    bool enterThroughputTestMode;
    bool throughputTimerCallback;
    bool enterPerTestMode;
    bool perTestMode;
}MODE_SWITCH_FLAGS;

extern MODE_SWITCH_FLAGS modeSwitchFlag;

/*Enum definitions*/
typedef enum APP_Mode_T
{
    APP_DATA_MODE,
    APP_CONTINUOUS_DATA_TEST_MODE,
    APP_PER_TEST_MODE,
    APP_PROMISCUOUS_MODE,
    APP_THROUGHPUT_TEST_MODE,
    APP_LOOP_BACK_MODE,
    APP_CMD_TASK_SUSPEND,
    APP_RESUME_DATA_MODE,
    APP_FRAGMENT_DATA_MODE,
    APP_SYSTIMER_START_PERIODIC_TEST_MODE,
    APP_SYSTIMER_EXPIRY_PERIODIC_TEST_MODE,
    APP_SYSTIMER_EXPIRY_DATA_ENTRY,
            
}APP_Mode_T;

typedef enum APP_TEST_MODE_T
{
    PER_TEST_MODE,
    PERIOD_TEST_MODE,
    CONT_TEST_MODE,
    PROM_TEST_MODE,
    THROUGHPUT_TEST_MODE,
}APP_TEST_MODE_T;

extern bool dataModeTimerStartFlag;
/*API Prototypes*/
bool app_P2P_Phy_Init(void);
void app_P2P_Phy_GetDeviceTableInfo(uint8_t dev_index);
void app_P2P_Phy_SetDeviceTableInfo(uint8_t dev_index,  DEVICE_TABLE *devTable);
APP_OP_STATE_T app_P2P_Phy_SetNWKParams(APP_NWK_PARAM *appnwkParam);
void app_P2P_Phy_TestMode(APP_TEST_MODE_T testStates);
void app_P2P_Phy_TransmitPredefinedData(bool isAckReq);
void app_P2P_Phy_ParseUserdefinedData(uint8_t *userData, uint8_t userDataLen);
void app_P2P_Phy_TransmitFrame(uint8_t *payload, uint8_t payloadLength, uint8_t devIndex, uint8_t msduHandle, bool unicast, bool ackReq, bool frameRetry);
void app_P2P_Phy_setAddressModes(uint8_t srcAddressMode, uint8_t dstAddressMode);
void app_P2P_Phy_setFrameType(uint8_t frameType);
void app_P2P_Phy_ConfigDataReception(void);
PHY_Retval_t app_P2P_Phy_StartEdScan(void);
void app_P2P_Phy_systimerCallbackTestMode(uintptr_t context);
void app_P2P_Phy_systimerCallbackDataMode(uintptr_t context);
void app_P2P_Phy_systimerCallbackThroughputTestMode(uintptr_t context);
void app_P2P_Phy_TaskHandler(APP_Msg_T *appModeSwitch);
void app_P2P_Phy_packetErrorRateCal(uint64_t nPacketsSent, uint64_t nBytesSent, uint64_t nPacketsRecAck);
void app_P2P_Phy_appModeSwitchHandler(void);
void app_P2P_Phy_stopActiveModes(void);
void app_P2P_Phy_appModePostDataTxDoneCbSwitchHandler(void);
void app_P2P_Phy_appModePostDataTxSwitchHandler(void);
APP_NWK_PARAM *app_P2P_Phy_AssignNWKParams(void);
/**
* @brief Converts a 16-Bit value into  a 2 Byte array
*
* @param[in] value 16-Bit value
* @param[out] data Pointer to the 2 Byte array to be updated with 16-Bit value
* @ingroup apiPalApi
*/
static inline void convert_16_bit_to_byte_array(uint16_t value, uint8_t *data)
{
    data[0] = (uint8_t)(value & ((uint16_t)0xFF));
    data[1] = (uint8_t)((value >> 8) & ((uint16_t)0xFF));
}



/**
* @brief Converts a 64-Bit value into  a 8 Byte array
*
* @param[in] value 64-Bit value
* @param[out] data Pointer to the 8 Byte array to be updated with 64-Bit value
* @ingroup apiPalApi
*/
static inline void convert_64_bit_to_byte_array(uint64_t value, uint8_t *data)
{
    uint8_t index = 0;

   while (index < 8U)
    {
        data[index++] = (uint8_t) (value & ((uint64_t)0xFFULL));
        value = (uint64_t)(value >> 8);
    }
}
#ifdef	__cplusplus
}
#endif

#endif	/* APP_P2P_PHY_H */

