/*
 *  Copyright (c) 2023, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for radio communication.
 *
 */


/*******************************************************************************
* Copyright (C) [2023], Microchip Technology Inc., and its subsidiaries. All rights reserved.
  
* The software and documentation is provided by Microchip and its contributors 
* "as is" and any express, implied or statutory warranties, including, but not 
* limited to, the implied warranties of merchantability, fitness for a particular 
* purpose and non-infringement of third party intellectual property rights are 
* disclaimed to the fullest extent permitted by law. In no event shall Microchip 
* or its contributors be liable for any direct, indirect, incidental, special,
* exemplary, or consequential damages (including, but not limited to, procurement 
* of substitute goods or services; loss of use, data, or profits; or business 
* interruption) however caused and on any theory of liability, whether in contract, 
* strict liability, or tort (including negligence or otherwise) arising in any way 
* out of the use of the software and documentation, even if advised of the 
* possibility of such damage.
* 
* Except as expressly permitted hereunder and subject to the applicable license terms 
* for any third-party software incorporated in the software and any applicable open 
* source software license terms, no license or other rights, whether express or 
* implied, are granted under any patent or other intellectual property rights of 
* Microchip or any third party.
 *******************************************************************************/


#include <string.h>

#include "definitions.h"
#include "phy.h"
#include "stack_config.h"
#include <uart.h>

#include <openthread/config.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/entropy.h>

#include "platform-pic32cx.h"

#include <common/logging.hpp>
#include <utils/code_utils.h>
#include <utils/soft_source_match_table.h>
#include <utils/mac_frame.h>

enum
{
    PHY_SCAN_DURATION = 3 // 3 * symbol duration
};

#define OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN 11
#define OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MAX 26

#define OT_RADIO_RX_BUFFERS_NUM          5U

static otRadioFrame sTransmitFrame;
static uint8_t sTransmitPsdu[OT_RADIO_FRAME_MAX_SIZE + 1];

typedef struct radioReceivedFrame
{
    otRadioFrame RadioFrame;
    buffer_t* phyBuffer;
}radioReceivedFrame_t;

static radioReceivedFrame_t sReceivedFrames[OT_RADIO_RX_BUFFERS_NUM];

static bool __attribute__((used)) sSleep = false;
static bool sRxEnable = false;
static bool sTxDone = false;
static uint8_t sTxStatus = PHY_SUCCESS;
static otRadioState sState = OT_RADIO_STATE_DISABLED;
static bool sPromiscuous = false;
static uint8_t sChannel = 0xFF;

static int8_t sMaxRssi = INT8_MIN;
static int8_t sCurRssi;
static uint32_t sScanStartTime;
static uint16_t sScanDuration;
static bool sStartScan = false;
static bool sScanOver = false;

static uint8_t sTxRxBuffer[LARGE_BUFFER_SIZE] = {0};
static bool sIsSrcMatchEnabled = false;
static bool sAckedWithFramePending = true;

extern OSAL_QUEUE_HANDLE_TYPE OTQueue;

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
static uint32_t         sMacFrameCounter;
static uint8_t          sKeyId;
static otMacKeyMaterial sPrevKey;
static otMacKeyMaterial sCurrKey;
static otMacKeyMaterial sNextKey;
#endif

static otExtAddress sExtAddr;

/*******************************************************************************
 * Static
 ******************************************************************************/
static void __attribute__((used)) radioAssertError(bool condition)
{
    //if (!condition)
    //    RGB_LED_RED_Off();
    //else
    //    RGB_LED_RED_On();
}

static void __attribute__((used)) radioAssertWarn(bool condition)
{
    //if (!condition)
    //    RGB_LED_BLUE_Off();
    //else
    //    RGB_LED_BLUE_On();
}


static void __attribute__((used)) radioSleep() 
{
    PHY_SleepMode_t sleepMode = SLEEP_MODE_1;
    if(!sSleep)
    {
        PHY_TrxSleep(sleepMode);
        sSleep = true;
        sRxEnable = false;
    }
}

static void __attribute__((used)) radioWakeup() 
{
    PHY_TrxWakeup();
}

static void __attribute__((used)) radioRxEnable()
{
    radioAssertWarn(!sRxEnable);
    
    if (sSleep)
    {
        //Wakeup the radio if it is in sleep
        radioWakeup();
    }
    
    if (!sRxEnable)
    {
        if (PHY_GetTrxStatus() != PHY_RX_ON)
        {
          PHY_RxEnable(PHY_STATE_RX_ON);
        }
        
        sRxEnable = true;
        sSleep = false;
    }
}

static void __attribute__((used)) radioTrxOff()
{
    radioAssertWarn(sRxEnable);

    if(sSleep)
    {
        radioWakeup();
    }
    else if (sRxEnable)
    {
        radioAssertError(PHY_RxEnable(PHY_STATE_TRX_OFF) == PHY_TRX_OFF);
    }
}

static void __attribute__((used)) radioRestore()
{
    radioAssertWarn(sRxEnable);

    if (sSleep)
    {
        radioSleep();
    } 
    else if (sRxEnable)
    {
        if (PHY_GetTrxStatus() != PHY_RX_ON)
        {
            PHY_RxEnable(PHY_STATE_RX_ON);
        }
    }
}

static void setChannel(uint8_t aChannel)
{
    if (((aChannel != sChannel) && (aChannel >= OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN) 
            && (aChannel <= OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MAX)))
    {
        otLogDebgPlat("Radio set channel: %d", aChannel);
        
        radioTrxOff();
        
        PHY_PibSet(phyCurrentChannel, (PibValue_t *) & aChannel);
        
        radioRestore();

        sChannel = aChannel;
    }
}

static void handleEnergyScan()
{
    if (sStartScan)
    {
        if (sScanOver)
        {
            if (sCurRssi > sMaxRssi)
            {
                sMaxRssi = sCurRssi;
            }

            if ((otPlatAlarmMilliGetNow() - sScanStartTime) < sScanDuration)
            {
                PHY_EdStart(PHY_SCAN_DURATION);
                sScanOver = false;
            }
            else
            {
                sStartScan = false;
                sScanOver = false;
                otPlatRadioEnergyScanDone(sInstance, sMaxRssi);
                radioRestore();
            }
        }
    }
}

 static void handleRx(void)
 {
    for (uint32_t i = 0; i < OT_RADIO_RX_BUFFERS_NUM; i++)
    {
        if (sReceivedFrames[i].RadioFrame.mPsdu != NULL && sReceivedFrames[i].phyBuffer != NULL )
        {
#if OPENTHREAD_CONFIG_DIAG_ENABLE

            if (otPlatDiagModeGet())
            {
                otPlatDiagRadioReceiveDone(aInstance, &sReceivedFrames[i], OT_ERROR_NONE);
            }
            else
#endif
            {
                otPlatRadioReceiveDone(sInstance, &sReceivedFrames[i].RadioFrame, OT_ERROR_NONE);
            }

            sReceivedFrames[i].RadioFrame.mPsdu = NULL;
            // free the buffer that was used for frame reception 
            bmm_buffer_free((buffer_t *)sReceivedFrames[i].phyBuffer);
            
            sReceivedFrames[i].phyBuffer = NULL;
        }
    }
}

static void handleTx(void)
{
    otError otStatus = OT_ERROR_NONE;

    otRadioFrame ackFrame;
    otRadioFrame *ackFramePtr = &ackFrame;
    uint8_t ackPsdu[ACK_PAYLOAD_LEN];

    if (sTxDone)
    {
        sTxDone = false;
        otLogDebgPlat("Radio transmit done, status: %d", otStatus);

        ackFrame.mPsdu = ackPsdu;
        ackFrame.mLength = ACK_PAYLOAD_LEN;
        ackFrame.mPsdu[0] = FCF_FRAMETYPE_ACK;
        ackFrame.mPsdu[1] = 0;
        ackFrame.mPsdu[2] = sTransmitFrame.mPsdu[PL_POS_SEQ_NUM];

        switch (sTxStatus)
        {
        case PHY_FAILURE:
            otStatus = OT_ERROR_ABORT;
            break;

        case PHY_SUCCESS:
            otStatus = OT_ERROR_NONE;
            break;

        case PHY_CHANNEL_ACCESS_FAILURE:
            otStatus = OT_ERROR_CHANNEL_ACCESS_FAILURE;
            break;

        case PHY_NO_ACK:
            otStatus = OT_ERROR_NO_ACK;
            break;
            
        case PHY_FRAME_PENDING:
            ackFrame.mPsdu[0] |= FCF_FRAME_PENDING;
            otStatus = OT_ERROR_NONE;
            break;

        default:
            otStatus = OT_ERROR_ABORT;
            break;
        }

        if ((~(sTransmitFrame.mPsdu[0] & FCF_ACK_REQUEST) == 0) || (otStatus != OT_ERROR_NONE))
        {
            ackFramePtr = NULL;
        }
        sState = OT_RADIO_STATE_RECEIVE;
        otPlatRadioTxDone(sInstance, &sTransmitFrame, ackFramePtr, otStatus);
        
    }
}

/*******************************************************************************
 * PHY
 ******************************************************************************/

void PHY_EdEndCallback(uint8_t energyLevel)
{
    OT_Msg_T otRadioEDMsg;
    if (sStartScan && !sScanOver)
    {
        sCurRssi = (int8_t) (PHY_GetRSSIBaseVal() + energyLevel);
        sScanOver = true;
        otRadioEDMsg.OTMsgId = OT_MSG_RADIO_SCAN_DONE;
        OSAL_QUEUE_Send(&OTQueue, &otRadioEDMsg,0);
    }
}

void PHY_TxDoneCallback(PHY_Retval_t status, PHY_FrameInfo_t *frame)
{
    OT_Msg_T otRadioTxMsg;
    /* Keep compiler happy. */
    status = status;
    frame = frame;

    sTxStatus = (uint8_t) status;
    sTxDone = true;
    otRadioTxMsg.OTMsgId = OT_MSG_RADIO_TX_DONE;
    OSAL_QUEUE_Send(&OTQueue, &otRadioTxMsg,0);
}

void PHY_RxFrameCallback(PHY_FrameInfo_t *rxFrame)
{
    OT_Msg_T otRadioRxMsg;
    otRadioFrame *receivedFrame = NULL;
    
    for (uint32_t i = 0; i < OT_RADIO_RX_BUFFERS_NUM; i++)
    {
        if (sReceivedFrames[i].RadioFrame.mPsdu == NULL)
        {
            receivedFrame = &sReceivedFrames[i].RadioFrame;

            memset(receivedFrame, 0, sizeof(*receivedFrame));
            sReceivedFrames[i].phyBuffer = rxFrame->buffer_header;
            break;
        }
    }
    
    assert(receivedFrame != NULL);
    

    if(rxFrame != NULL)
    {
        uint8_t frameLen = rxFrame->mpdu[0];
        receivedFrame->mPsdu               = &rxFrame->mpdu[1];
        receivedFrame->mLength             = frameLen;
        receivedFrame->mInfo.mRxInfo.mRssi = (int8_t )(rxFrame->mpdu[frameLen + LQI_LEN + ED_VAL_LEN] + PHY_GetRSSIBaseVal());
        receivedFrame->mInfo.mRxInfo.mLqi  = rxFrame->mpdu[frameLen + LQI_LEN];
        receivedFrame->mChannel            = sChannel;


        receivedFrame->mInfo.mRxInfo.mTimestamp = otPlatAlarmMilliGetNow() * 1000;

        if(receivedFrame->mPsdu[0] & FCF_ACK_REQUEST)
        {
            receivedFrame->mInfo.mRxInfo.mAckedWithFramePending = sAckedWithFramePending;
        }
        else
        {
             receivedFrame->mInfo.mRxInfo.mAckedWithFramePending = false;
        }

        sAckedWithFramePending = false;

        otRadioRxMsg.OTMsgId = OT_MSG_RADIO_RX_DONE;
        OSAL_QUEUE_Send(&OTQueue, &otRadioRxMsg,0);
    }
}

/*******************************************************************************
 * Platform
 ******************************************************************************/



void pic32cxRadioInit(void)
{
    sTransmitFrame.mLength = 0;
    sTransmitFrame.mPsdu = sTransmitPsdu + 1;

    for (uint32_t i = 0; i < OT_RADIO_RX_BUFFERS_NUM; i++)
    {
        sReceivedFrames[i].RadioFrame.mPsdu = NULL;
        sReceivedFrames[i].phyBuffer = NULL;
    }
}

void pic32cxRadioProcess(otInstance *aInstance,OT_MsgId_T otRadioMsgId)
{
    OT_UNUSED_VARIABLE(aInstance);

    if(OT_MSG_RADIO_SCAN_DONE == otRadioMsgId)
        handleEnergyScan();
    else if(OT_MSG_RADIO_RX_DONE == otRadioMsgId)
        handleRx();
    else if(OT_MSG_RADIO_TX_DONE == otRadioMsgId)
        handleTx();
}

uint32_t pic32cxRadioRandomGet(void)
{
    uint32_t result = 0;

    result = TRNG_ReadData();

    return result;
}

void pic32cxRadioRandomGetTrue(uint8_t *aOutput, uint16_t aOutputLength)
{
    otPlatEntropyGet(aOutput, aOutputLength);
}

/*******************************************************************************
 * Radio
 ******************************************************************************/
otRadioState otPlatRadioGetState(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    otRadioState radioState = OT_RADIO_STATE_RECEIVE;

    if (sState == OT_RADIO_STATE_DISABLED)
    {
        return OT_RADIO_STATE_DISABLED;
    } 

    switch (PHY_GetTrxStatus())
    {
        case PHY_TRX_OFF:
        case PHY_TRX_SLEEP:
        case PHY_TRX_DEEP_SLEEP:
            radioState  = OT_RADIO_STATE_SLEEP;           
            break;

        case PHY_RX_ON:
        case PHY_BUSY_RX:
            radioState = OT_RADIO_STATE_RECEIVE;
            break;

        case PHY_TX_ON:
        case PHY_BUSY_TX:
            radioState = OT_RADIO_STATE_TRANSMIT;
            break;

        default:
           radioState = OT_RADIO_STATE_RECEIVE;
            break;
    }

    return radioState; // It is the default state. Return it in case of unknown.
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    pic32cxGetIeeeEui64(aInstance, aIeeeEui64);
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t aPanId)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Set Pan ID: 0x%04X", aPanId);
    
    radioTrxOff();
    
    PHY_PibSet(macPANId, (PibValue_t *) & aPanId);
    
    radioRestore();
    
    utilsSoftSrcMatchSetPanId(aPanId);
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    
    radioTrxOff();
#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
    for (size_t i = 0; i < sizeof(*aAddress); i++)
    {
        sExtAddr.m8[i] = aAddress->m8[sizeof(*aAddress) - 1 - i];
    }
#endif
    
    PHY_PibSet(macIeeeAddress, (PibValue_t *) aAddress);
    
    radioRestore();

}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    
    radioTrxOff();
    
    PHY_PibSet(macShortAddress, (PibValue_t *) & aAddress);
    
    radioRestore();
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return (sState != OT_RADIO_STATE_DISABLED);
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    otLogDebgPlat("Radio enable");

    if (!otPlatRadioIsEnabled(aInstance))
    {
        radioSleep();

        sState = OT_RADIO_STATE_SLEEP;
    }

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    otLogDebgPlat("Radio disable");

    if (otPlatRadioIsEnabled(aInstance))
    {
        radioSleep();

        sState = OT_RADIO_STATE_DISABLED;
    }

    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Radio sleep");

    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(sState == OT_RADIO_STATE_SLEEP || sState == OT_RADIO_STATE_RECEIVE, error = OT_ERROR_INVALID_STATE);

    radioSleep();

    sState = OT_RADIO_STATE_SLEEP;

exit:

    return error;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Radio receive, channel: %d", aChannel);

    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(sState != OT_RADIO_STATE_DISABLED, error = OT_ERROR_INVALID_STATE);
    
    radioWakeup();

    setChannel(aChannel);

    radioRxEnable();
    
    sState = OT_RADIO_STATE_RECEIVE;

exit:

    return error;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    otError error = OT_ERROR_NONE;
    
    PHY_FrameInfo_t txFrame;
    PHY_CSMAMode_t csmaMode;
    bool performRetry = false; 

    if(aFrame->mInfo.mTxInfo.mCsmaCaEnabled)
    {
        csmaMode = CSMA_UNSLOTTED;
    }
    else
    {
        csmaMode = NO_CSMA_WITH_IFS;
    }
    
    if(aFrame->mInfo.mTxInfo.mMaxFrameRetries > 0)
    {
        performRetry = true;
    }

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    if (otMacFrameIsSecurityEnabled(aFrame) && otMacFrameIsKeyIdMode1(aFrame) && !aFrame->mInfo.mTxInfo.mIsARetx && !aFrame->mInfo.mTxInfo.mIsSecurityProcessed)
    {
        otMacFrameSetKeyId(aFrame, sKeyId);
        otMacFrameSetFrameCounter(aFrame, sMacFrameCounter++);
        aFrame->mInfo.mTxInfo.mAesKey = &sCurrKey;
        otMacFrameProcessTransmitAesCcm(aFrame, &sExtAddr);
    }
#endif
     
    radioWakeup();
    
    PHY_PibSet(macMaxCSMABackoffs, (PibValue_t *)&(aFrame->mInfo.mTxInfo.mMaxCsmaBackoffs));
    
    PHY_PibSet(macMaxFrameRetries, (PibValue_t *)&(aFrame->mInfo.mTxInfo.mMaxFrameRetries));

    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Radio transmit");


    otEXPECT_ACTION(sState == OT_RADIO_STATE_RECEIVE , error = OT_ERROR_INVALID_STATE);

    setChannel(aFrame->mChannel);


    sTxRxBuffer[0] = aFrame->mLength;
    memcpy((uint8_t *) & sTxRxBuffer[1], aFrame->mPsdu, aFrame->mLength);

    txFrame.mpdu = &sTxRxBuffer[0];

    if (PHY_SUCCESS == PHY_TxFrame(&txFrame, csmaMode, performRetry))
    {
        //Frame transmitted successfully
        sState = OT_RADIO_STATE_TRANSMIT;
        otPlatRadioTxStarted(aInstance, aFrame);
    }
    else
    {
        error = OT_ERROR_FAILED;
    }

exit:

    return error;
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return &sTransmitFrame;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    uint8_t edLevel;
    int8_t pwrDbm;
    radioTrxOff();
    edLevel = PHY_EdSample();
    pwrDbm = (int8_t)(edLevel + PHY_GetRSSIBaseVal());
    radioRestore();
    
    return pwrDbm;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    return  OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_TRANSMIT_RETRIES |
            OT_RADIO_CAPS_ENERGY_SCAN | OT_RADIO_CAPS_TRANSMIT_SEC | OT_RADIO_CAPS_TRANSMIT_TIMING ;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return sPromiscuous;
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    PibValue_t pibValue;
    
    pibValue.pib_value_bool = aEnable;

    sPromiscuous = aEnable;
    
    PHY_PibSet(macPromiscuousMode, &pibValue);
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

     sIsSrcMatchEnabled = aEnable;
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    OT_UNUSED_VARIABLE(aInstance);

    sScanStartTime = otPlatAlarmMilliGetNow();
    sScanDuration = aScanDuration;

    sMaxRssi = INT8_MIN;
    
    radioWakeup();
    PHY_EdStart(PHY_SCAN_DURATION);

    sScanOver = false;
    sStartScan = true;

    return OT_ERROR_NONE;
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    otError error = OT_ERROR_NONE;
  
    otEXPECT_ACTION(aPower != NULL, error = OT_ERROR_INVALID_ARGS);
    PHY_PibGet(phyTransmitPower, (uint8_t *)aPower);

exit:
    return error;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Radio set default TX power: %d", aPower);

    PHY_PibSet(phyTransmitPower, (PibValue_t *) &aPower);

    return OT_ERROR_NONE;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aThreshold);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aThreshold);

    return OT_ERROR_NOT_IMPLEMENTED;
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    int8_t sensitivity = INT8_MIN;
    
    OT_UNUSED_VARIABLE(aInstance);
    
    //Get the radio base RSSI value
    sensitivity = PHY_GetRSSIBaseVal();

    return sensitivity;
}

#if OPENTHREAD_FTD || OPENTHREAD_RADIO
bool PHY_IsFramePendingFromNextLayer(PHY_Addr_t *addr, uint8_t *addrMode)
{
    //By default framePendingBit should be set as per Spec
    bool framePendingSet = true;
    otExtAddress extAddr;    
    
    uint8_t addrModeType = *addrMode;
    uint16_t shortAddr = 0xFFFF;
    
    if (sIsSrcMatchEnabled)
    {
        if(addr != NULL)
        {
            if (addrModeType == FCF_LONG_ADDR) 
            {    
                memcpy(&extAddr, addr->val, EXT_ADDR_LEN); 
            }
            else if (addrModeType == FCF_SHORT_ADDR) 
            {
                memcpy(&shortAddr, addr->val, SHORT_ADDR_LEN);
            }

            if (addrModeType != FCF_RESERVED_ADDR)
            {
                framePendingSet = ((addrModeType == FCF_LONG_ADDR) 
                    ?  (utilsSoftSrcMatchExtFindEntry( &extAddr) >= 0)
                    : (utilsSoftSrcMatchShortFindEntry( shortAddr) >= 0));

            }
        }
    }
    
    sAckedWithFramePending = framePendingSet;
       

    return framePendingSet;
   
}
#endif
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
void otPlatRadioSetMacKey(otInstance *            aInstance,
                          uint8_t                 aKeyIdMode,
                          uint8_t                 aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType          aKeyType)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeyIdMode);
  
    assert(aKeyType == OT_KEY_TYPE_LITERAL_KEY);
    assert(aPrevKey != NULL && aCurrKey != NULL && aNextKey != NULL);
    

    
    sKeyId   = aKeyId;
    sPrevKey = *aPrevKey;
    sCurrKey = *aCurrKey;
    sNextKey = *aNextKey;
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    OT_UNUSED_VARIABLE(aInstance);

    sMacFrameCounter = aMacFrameCounter;
    
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
