/*
 *  Copyright (c) 2024, The OpenThread Authors.
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
#include <openthread/link.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/entropy.h>
#include <openthread/platform/time.h>

#include "platform-pic32cx.h"

#include <common/logging.hpp>
#include <utils/code_utils.h>
#include <utils/soft_source_match_table.h>
#include <utils/mac_frame.h>
#include <utils/link_metrics.h>

enum
{
    PHY_SCAN_DURATION = 3 // 3 * symbol duration
};

#define OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN 11
#define OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MAX 26

#define OT_RADIO_RX_BUFFERS_NUM			  5U

/* Variable holding the details on frame to be transmitted */
static otRadioFrame sTransmitFrame;

typedef struct radioReceivedFrame
{
    otRadioFrame RadioFrame;
    buffer_t* phyBuffer;
}radioReceivedFrame_t;

/* Buffer holding the received frames details */
static radioReceivedFrame_t sReceivedFrames[OT_RADIO_RX_BUFFERS_NUM];
/* Variable holding the Transceiver's sleep state */
static bool __attribute__((used)) sSleep = false;
/* Variable holding the Transceiver's receive state */
static bool sRxEnable = false;
/* variable holding the Transmission done status*/
static bool sTxDone = false;
/* Variable holding the last transmission status */
static uint8_t sTxStatus = PHY_SUCCESS;
/* Variable holding the radio state information */
static otRadioState sState = OT_RADIO_STATE_DISABLED;
/* Variable holding status on Promiscuous mode (Enable/Disable)*/
static bool sPromiscuous = false;
/* Variable holding Transceiver's current channel being used*/
static uint8_t sChannel = 0xFF;

/* Variable holding Max RSSI measured during ED Scan */
static int8_t sMaxRssi = INT8_MIN;
/* Variable holding recently measured RSSI value */
static int8_t sCurRssi;
/* Variable holding ED Scan start time */
static uint32_t sScanStartTime;
/* Variable holding ED Scan duration*/
static uint16_t sScanDuration;
/* Variable holding status on staring of ED Scan */
static bool sStartScan = false;
/* Variable holding status on ED Scan completion*/
static bool sScanOver = false;

static uint8_t sTxRxBuffer[LARGE_BUFFER_SIZE] = {0};
/* Variable to hold status on Source address match feature*/
static bool sIsSrcMatchEnabled = false;
/* Status on Acknowledgement sent - Acked With Frame Pending Bit Set*/
static bool sAckedWithFramePending = true;

extern OSAL_QUEUE_HANDLE_TYPE OTQueue;

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
/* MAC Layer Security Material - Frame Counter*/
static uint32_t         sMacFrameCounter;
/* MAC Layer Security Material - Key Id*/
static uint8_t          sKeyId;
/* MAC Layer Security Material - Previous Key*/
static otMacKeyMaterial sPrevKey;
/* MAC Layer Security Material - Current Key*/
static otMacKeyMaterial sCurrKey;
/* MAC Layer Security Material - Next Key*/
static otMacKeyMaterial sNextKey;
/* Status on Acknowledgement sent - Acked with Secured Enhanced ACK*/
static bool sAckedWithSecEnhAck = false;
/* ACK Frame counter value */
static uint32_t         sAckFrameCounter;
/* ACK Key Id value */
static uint8_t          sAckKeyId;
#endif

/* Device Extended address */
static otExtAddress sExtAddr;

/* Variable holding information on ACK frame to be transmitted */
static otRadioFrame sTxAckFrame;

/* Buffer holding SW ACK to be transmitted */
static uint8_t sTxAckBuffer[LARGE_BUFFER_SIZE] = {0};

/* Buffer holding received ACK frame*/
static uint8_t sRxAckBuffer[LARGE_BUFFER_SIZE] = {0};

/* Variable holding status of Transmission of v2015 frame */
static bool is2015FrameSent = false;

/* Variable holding status of Transmission of Enhanced ACK */
static bool enhAckSent = false;

/* Time stamp of recently received frame*/
static volatile uint64_t rxPktTimestamp = 0;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
/* CSL Receiver's Period in us*/
static uint32_t      sCslPeriod;
/* CSL Receiver's sample time in us*/
static uint32_t      sCslSampleTime;
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

/* The Uncertainty of the scheduling CSL of transmission by the parent, in ±10 us units.*/
#define CSL_UNCERT            200          

/* Device EUI64 value */
const uint8_t systemUniqueId[OT_EXT_ADDRESS_SIZE] = {0x3e, 0x51, 0xf0, 0x0b, 0x47, 0x3e, 0x51, 0x00};
/*******************************************************************************
 * Extern
 ******************************************************************************/
extern void PHY_EnableEnhancedFramepending(bool enable);
extern PHY_Retval_t PHY_Tx2015Frame(PHY_FrameInfo_t *txFrame, PHY_CSMAMode_t csmaMode, bool performFrameRetry);
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
    /* Uploading the received frames to higher layer
     * 1. Check the received frame buffer sReceivedFrames[5]
     * 2. If RadioFrame and phyBuffer having valid pointer, signal the reception with frame details 
     *    to subMac - Pointers will be updated in PHY_RxFrameCallback() on the reception of frame.
     * 3. Once callback is processed, clear the RadioFrame PSDU and phyBuffer pointers 
     * */
    for (uint32_t i = 0; i < OT_RADIO_RX_BUFFERS_NUM; i++)
    {
        if (sReceivedFrames[i].RadioFrame.mPsdu != NULL && sReceivedFrames[i].phyBuffer != NULL)
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
            if(sReceivedFrames[i].phyBuffer != NULL)
            {
                // free the buffer that was used for frame reception 
                bmm_buffer_free((buffer_t *)sReceivedFrames[i].phyBuffer);
            }
            
            sReceivedFrames[i].phyBuffer = NULL;   
        }
    }
    
}

static void handleTx(void)
{
    /* Signaling the recently transmitted packet status to higher layer
     * PHY layer doesn't provide ACK frame details for immediate ACK received for ACKRequest packet being sent.
     * So, Radio layer populates the ACK information on the successful reception of ACK Frame (PHY_SUCCESS & PHY_FRAME_PENDING).
     * rxAckFrame variable holds the ACK frame information.
     * 1. Check transmission is done (sTxDone). sTxDone will be updated in PHY_TxDoneCallback() and sRxAckBuffer[].
     * 2. Check sTxStatus transmission status. sTxStatus will be updated in PHY_TxDoneCallback() and PHY_Tx2015FrameDoneCallback().
     * 3. If the packet being sent out is not ACK Requested packet, ackFramePtr will be set as NULL.
     * 4. If the packet being sent out is ACK Requested packet, 
     *     - ackFramePtr = &rxAckFrame; When ImmACK received;
     *     - ackFramePtr is filled with sRxAckBuffer[] content ; When Enhanced ACK received; sRxAckBuffer[] will be updated in PHY_Tx2015FrameDoneCallback().
     * 5. Invoke otPlatRadioTxDone() with TxFrame, ACK Frame and status information 
     * */
    otError otStatus = OT_ERROR_NONE;
    
    
    otRadioFrame rxAckFrame;
    otRadioFrame *ackFramePtr = &rxAckFrame;
    uint8_t rxAckFramePsdu[ACK_PAYLOAD_LEN];
    rxAckFrame.mPsdu = &rxAckFramePsdu[0];

    if (sTxDone)
    {
        sTxDone = false;
        otLogInfoPlat("Radio transmit done, status: %d", otStatus);
        
        rxAckFrame.mLength = ACK_PAYLOAD_LEN;
        rxAckFrame.mPsdu[0] = FCF_FRAMETYPE_ACK;
        rxAckFrame.mPsdu[1] = 0;
        rxAckFrame.mPsdu[2] = sTransmitFrame.mPsdu[PL_POS_SEQ_NUM];

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
            rxAckFrame.mPsdu[0] |= FCF_FRAME_PENDING;
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
        
        if(ackFramePtr != NULL)
        {
            if(is2015FrameSent)
            {
                uint8_t frameLen = sRxAckBuffer[0];
                ackFramePtr->mPsdu               = &sRxAckBuffer[1];
                ackFramePtr->mLength             = frameLen;
                ackFramePtr->mInfo.mRxInfo.mRssi = (int8_t )(sRxAckBuffer[frameLen + LQI_LEN + ED_VAL_LEN] + PHY_GetRSSIBaseVal());
                ackFramePtr->mInfo.mRxInfo.mLqi  = sRxAckBuffer[frameLen + LQI_LEN];
                ackFramePtr->mChannel            = sChannel;
                
                ackFramePtr->mInfo.mRxInfo.mTimestamp = rxPktTimestamp;               
            }
        }
        
        sState = OT_RADIO_STATE_RECEIVE;
        otPlatRadioTxDone(sInstance, &sTransmitFrame, ackFramePtr, otStatus);
        
    }
}

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
/* Get current phase related to CSL period*/
static uint16_t getCslPhase(void)
{
    uint32_t curTime       = otPlatAlarmMicroGetNow();
    uint32_t cslPeriodInUs = sCslPeriod * OT_US_PER_TEN_SYMBOLS;
    uint32_t diff = ((sCslSampleTime % cslPeriodInUs) - (curTime % cslPeriodInUs) + cslPeriodInUs) % cslPeriodInUs;

    return (uint16_t)(diff / OT_US_PER_TEN_SYMBOLS);
}


/* Fill IE information in the frame to be transmitted */
static void updateIeInfoTxFrame(otRadioFrame *aFrame)
{
    assert(aFrame != NULL);

    // Update IE data in the 802.15.4 header with the newest CSL period / phase
    if (sCslPeriod > 0 && !aFrame->mInfo.mTxInfo.mIsHeaderUpdated)
    {
        otMacFrameSetCslIe(aFrame, (uint16_t)sCslPeriod, getCslPhase());
    }

}

#endif

/* Fill IE information in the ACK frame to be transmitted*/
static uint8_t updateIeInfoAckFrame(otRadioFrame *aFrame, uint8_t *ieData)
{
    uint8_t offset = 0;
    
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    otMacAddress aSrcAddress = {0};
    uint8_t      linkMetricsDataLen = 0;
    uint8_t linkMetricsData[OT_ENH_PROBING_IE_DATA_MAX_SIZE];
    
    otMacFrameGetSrcAddr(aFrame, &aSrcAddress);
    /* Check Link Metrics data is available for the particular device*/
    linkMetricsDataLen = otLinkMetricsEnhAckGenData(&aSrcAddress, aFrame->mInfo.mRxInfo.mLqi, aFrame->mInfo.mRxInfo.mRssi, linkMetricsData);
#endif
    

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (sCslPeriod > 0)
    {
        /* Fill ieData with CSL IE*/
        offset += otMacFrameGenerateCslIeTemplate(ieData);
    }
#endif
    
    
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    if (linkMetricsData != NULL && linkMetricsDataLen > 0)
    {
        /* Fill ieData with Link Metrics IE if Link metrics data is available*/
        offset += otMacFrameGenerateEnhAckProbingIe(ieData, linkMetricsData, linkMetricsDataLen);
    }
#endif
    
    return offset;
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

/*******************************************************************************
 * PHY
 ******************************************************************************/
/* PHY_EdEndCallback will be called for the PHY_EdStart request to PHY layer
 * It will be called after scanning the current 
 * channel for a time period of aBaseSuperframeDuration * (2^scanDuration + 1) symbols */
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

/* PHY_TxDoneCallback will be invoked by PHY layer to inform the status on the packet being transmitted
 * This will be called for the PHY_TxFrame() request */
void PHY_TxDoneCallback(PHY_Retval_t status, PHY_FrameInfo_t *frame)
{
    OT_Msg_T otRadioTxMsg;
    /* Keep compiler happy. */
    status = status;
    frame = frame;
    
    is2015FrameSent = false;

    sTxStatus = (uint8_t) status;
    sTxDone = true;
    otRadioTxMsg.OTMsgId = OT_MSG_RADIO_TX_DONE;
    OSAL_QUEUE_Send(&OTQueue, &otRadioTxMsg,0);
}

/* PHY_Tx2015FrameDoneCallback will be invoked by PHY layer to inform the status on the packet being transmitted
 * This will be called for the PHY_Tx2015Frame() request */
void PHY_Tx2015FrameDoneCallback(PHY_Retval_t status, PHY_FrameInfo_t *frame, PHY_FrameInfo_t *ackFramePtr)
{
    OT_Msg_T otRadioTxMsg;
    /* Keep compiler happy. */
    status = status;
    frame = frame;
    
    is2015FrameSent = true;
    
    if(ackFramePtr != NULL)
    {  
        //Fill the sRxAckBuffer[] with enhanced ACK information
        uint8_t frameLen = ackFramePtr->mpdu[0];
        memcpy(&sRxAckBuffer, &ackFramePtr->mpdu[0], frameLen + LQI_LEN + ED_VAL_LEN + 1);  
        
    }

    sTxStatus = (uint8_t) status;
    sTxDone = true;
    otRadioTxMsg.OTMsgId = OT_MSG_RADIO_TX_DONE;
    OSAL_QUEUE_Send(&OTQueue, &otRadioTxMsg,0);   
}

/* PHY_RxFrameCallback will be invoked by PHY layer on the reception of the frame*/
void PHY_RxFrameCallback(PHY_FrameInfo_t *rxFrame)
{
    OT_Msg_T otRadioRxMsg;
    otRadioFrame *receivedFrame = NULL;
    
    // Check the sReceivedFrames buffer 
    for (uint32_t i = 0; i < OT_RADIO_RX_BUFFERS_NUM; i++)
    {
        if (sReceivedFrames[i].RadioFrame.mPsdu == NULL)
        {
            //If Buffer is free, update the pointer with received frame details
            receivedFrame = &sReceivedFrames[i].RadioFrame;

            memset(receivedFrame, 0, sizeof(*receivedFrame));
            sReceivedFrames[i].phyBuffer = rxFrame->buffer_header;
            break;
        }
    }
    
    otEXPECT(receivedFrame != NULL);
    
    if(rxFrame != NULL)
    {
        // Fill otRadioFrame with PHY Rx Frame information
        uint8_t frameLen = rxFrame->mpdu[0];
        receivedFrame->mPsdu               = &rxFrame->mpdu[1];
        receivedFrame->mLength             = frameLen;
        receivedFrame->mInfo.mRxInfo.mRssi = (int8_t )(rxFrame->mpdu[frameLen + LQI_LEN + ED_VAL_LEN] + PHY_GetRSSIBaseVal());
        receivedFrame->mInfo.mRxInfo.mLqi  = rxFrame->mpdu[frameLen + LQI_LEN];
        receivedFrame->mChannel            = sChannel;        
        receivedFrame->mInfo.mRxInfo.mTimestamp = rxPktTimestamp;
        
        //Check whether packet is ACK requested one, If so fill in with sAckedWithFramePending
        if(receivedFrame->mPsdu[0] & FCF_ACK_REQUEST)
        {
            receivedFrame->mInfo.mRxInfo.mAckedWithFramePending = sAckedWithFramePending;
        }
        else
        {
            receivedFrame->mInfo.mRxInfo.mAckedWithFramePending = false;
        }
        
        // enhAckSent variable will be updated on PHY_GenerateEnhancedAck() callback 
        // when v2 ACK Requested Frame is received and PHY layer sent out the enhanced ACK.      
        if(enhAckSent)
        {
            // Update the ACK security material information is Enhanced ACK is sent
            receivedFrame->mInfo.mRxInfo.mAckFrameCounter = sAckFrameCounter;
            receivedFrame->mInfo.mRxInfo.mAckKeyId = sAckKeyId;
            receivedFrame->mInfo.mRxInfo.mAckedWithSecEnhAck = sAckedWithSecEnhAck;
            enhAckSent = false;
        }
        else
        {
            // Reset the ACK security material information is Imm ACK is sent
            receivedFrame->mInfo.mRxInfo.mAckFrameCounter = 0;
            receivedFrame->mInfo.mRxInfo.mAckKeyId = 0;
            receivedFrame->mInfo.mRxInfo.mAckedWithSecEnhAck = 0;
        }

        sAckedWithFramePending = false;

        otRadioRxMsg.OTMsgId = OT_MSG_RADIO_RX_DONE;
        OSAL_QUEUE_Send(&OTQueue, &otRadioRxMsg,0);
    }
    
    exit:
            return;
}

/* PHY_RxStartCallback will be invoked by PHY Layer on the start of SFD reception */
void PHY_RxStartCallback(void)
{    
    OSAL_CRITSECT_DATA_TYPE intStatus;
    intStatus = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);    
    rxPktTimestamp = (uint64_t)(otPlatRadioGetNow(sInstance));
    
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, intStatus);
        
}

/* Function to secure the Enhanced ACK frame to be transmitted */
static void processTxAckSecurity(otRadioFrame *radioRxFrame)
{
 
    otMacKeyMaterial *key = NULL;
    uint8_t           keyId;
    
    //Check the received frame is Security Enabled and Key ID Mode = 1
    //Note: After Joining procedure, all MAC frames should secured with Key ID Mode = 1
    if(otMacFrameIsSecurityEnabled(&sTxAckFrame) && otMacFrameIsKeyIdMode1(&sTxAckFrame))
    {   
        //Get the key Id from received frame. Key ID defines which key to use for encryption
        // sCurrKey or sPrevKey or sNextKey
        keyId = otMacFrameGetKeyId(&sTxAckFrame);

        otEXPECT((keyId != 0));

        if (keyId == sKeyId)
        {
            key = &sCurrKey;
        }
        else if (keyId == sKeyId - 1)
        {
            key = &sPrevKey;
        }
        else if (keyId == sKeyId + 1)
        {
            key = &sNextKey;
        }
        else
        {
            otEXPECT(false);
        }
        
        //Update the local variables
        sAckFrameCounter    = sMacFrameCounter;
        sAckKeyId           = keyId;
        sAckedWithSecEnhAck = true;
       
        //Set the Key Id in ACK Frame
        otMacFrameSetKeyId(&sTxAckFrame, sAckKeyId);
        //Set the Frame Counter in ACK Frame
        otMacFrameSetFrameCounter(&sTxAckFrame, sMacFrameCounter++);

        sTxAckFrame.mInfo.mTxInfo.mAesKey = key;  
        //Encrypt the ACK frame 
        otMacFrameProcessTransmitAesCcm(&sTxAckFrame, &sExtAddr);
               
    }
    else
    {
        // If received frame is not secured one clear the global variables related to ACK security material
        sAckFrameCounter    = 0;
        sAckKeyId           = 0;
        sAckedWithSecEnhAck = false;
    }

    exit:
        return;
}

/* PHY_GenerateEnhancedAck will be invoked on the reception of Ack Requested v2 (2015) frames
 * to generate the enhanced ack to be transmitted. PHY Layer will send the ack frame */
void PHY_GenerateEnhancedAck(PHY_FrameInfo_t *rxFrame, PHY_FrameInfo_t *ackFramePtr)
{
    otRadioFrame *receivedFrame = NULL;   
    otRadioFrame radioFrame;
    otMacAddress srcAddr = {0};
    
    (void)memset(&radioFrame,0,sizeof(otRadioFrame));
    receivedFrame = &radioFrame;
    bool framePendingSet = false;
    
    uint8_t sAckIeData[OT_ACK_IE_MAX_SIZE] ={0};
    uint8_t sAckIeDataLength = 0;  
  
    if(rxFrame != NULL)
    {
        // Fill otRadioFrame with PHY Rx Frame information, Its needed for generating the ACK
        uint8_t frameLen = rxFrame->mpdu[0];
        receivedFrame->mPsdu               = &rxFrame->mpdu[1];
        receivedFrame->mLength             = frameLen;
        receivedFrame->mInfo.mRxInfo.mRssi = (int8_t )(rxFrame->mpdu[frameLen + LQI_LEN + ED_VAL_LEN] + PHY_GetRSSIBaseVal());
        receivedFrame->mInfo.mRxInfo.mLqi  = rxFrame->mpdu[frameLen + LQI_LEN];
        receivedFrame->mChannel            = sChannel;  
        receivedFrame->mInfo.mRxInfo.mTimestamp = rxPktTimestamp;
    } 
    
    otMacFrameGetSrcAddr(receivedFrame, &srcAddr);
    
    // Check the Frame is pending for child to set in Enh ACK FP bit
    framePendingSet = ((srcAddr.mType == OT_MAC_ADDRESS_TYPE_EXTENDED) 
                    ?  (utilsSoftSrcMatchExtFindEntry( &srcAddr.mAddress.mExtAddress) >= 0)
                    : (utilsSoftSrcMatchShortFindEntry( srcAddr.mAddress.mShortAddress) >= 0));
    
    receivedFrame->mInfo.mRxInfo.mAckedWithFramePending = framePendingSet;
    
    // Update the global sAckedWithFramePending variable
    sAckedWithFramePending = framePendingSet;
    
    //Check IE data is available to send as part of Enh ACK
    sAckIeDataLength = updateIeInfoAckFrame(receivedFrame, sAckIeData);
    
    //Generate the Enh ACK 
    otMacFrameGenerateEnhAck(receivedFrame, receivedFrame->mInfo.mRxInfo.mAckedWithFramePending, sAckIeData, sAckIeDataLength, &sTxAckFrame);
    
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (sCslPeriod > 0)
    {
        // Update IE data in the 802.15.4 header with the newest CSL period / phase
        otMacFrameSetCslIe(&sTxAckFrame, (uint16_t)sCslPeriod, getCslPhase());
    }
#endif
    
    //Secure the Enhanced ACK if the packet being received is Security enabled at MAC level
    processTxAckSecurity(receivedFrame);
    
    sTxAckBuffer[0] = sTxAckFrame.mLength;
    
    ackFramePtr->mpdu = &sTxAckBuffer[0];
    
    enhAckSent = true;
}

/* PHY_GenerateImmAck will be invoked by the PHY layer on the reception of frame 
 * to generate the immediate acknowledgement - This will be called only when 
 * enhanced frame pending is required */
void PHY_GenerateImmAck(PHY_FrameInfo_t *rxFrame, PHY_FrameInfo_t *ackFramePtr )
{
    
    otRadioFrame *receivedFrame = NULL;    
    otRadioFrame RadioFrame;
    
    (void)memset(&RadioFrame,0,sizeof(otRadioFrame));    
    receivedFrame = &RadioFrame;
    
    if(rxFrame != NULL)
    {
        // Fill otRadioFrame with PHY Rx Frame information, Its needed for generating the ACK
        uint8_t frameLen = rxFrame->mpdu[0];
        receivedFrame->mPsdu               = &rxFrame->mpdu[1];
        receivedFrame->mLength             = frameLen;
        receivedFrame->mInfo.mRxInfo.mRssi = (int8_t )(rxFrame->mpdu[frameLen + LQI_LEN + ED_VAL_LEN] + PHY_GetRSSIBaseVal());
        receivedFrame->mInfo.mRxInfo.mLqi  = rxFrame->mpdu[frameLen + LQI_LEN];
        receivedFrame->mChannel            = sChannel;       
        receivedFrame->mInfo.mRxInfo.mTimestamp = rxPktTimestamp;
        receivedFrame->mInfo.mRxInfo.mAckedWithFramePending = sAckedWithFramePending;
 
    }  
    
    //Generate the Immediate ACK 
    otMacFrameGenerateImmAck(receivedFrame, sAckedWithFramePending, &sTxAckFrame);
    
    sTxAckBuffer[0] = sTxAckFrame.mLength;
    
    ackFramePtr->mpdu = &sTxAckBuffer[0];
    
}

/*******************************************************************************
 * Platform
 ******************************************************************************/
/* Initialization of radio module*/
void pic32cxRadioInit(void)
{
    sTransmitFrame.mLength = 0;
    sTransmitFrame.mPsdu = &sTxRxBuffer[1];

    for (uint32_t i = 0; i < OT_RADIO_RX_BUFFERS_NUM; i++)
    {
        sReceivedFrames[i].RadioFrame.mPsdu = NULL;
        sReceivedFrames[i].phyBuffer = NULL;
    }
    
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    otLinkMetricsInit(PHY_GetRSSIBaseVal());
#endif
    
    sTxAckFrame.mPsdu = &sTxAckBuffer[1];
	//Enable Enhanced Frame Pending
	PHY_EnableEnhancedFramepending(true);
}

/* Radio Process function - This will be called when callbacks are invoked from PHY layer. 
 * This is to inform the status of request to higher layer*/
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

/* Function to get the random number of length 4 bytes*/
uint32_t pic32cxRadioRandomGet(void)
{
    uint32_t result = 0;

    result = TRNG_ReadData();

    return result;
}

/* Function to get the random number of length n bytes*/
void pic32cxRadioRandomGetTrue(uint8_t *aOutput, uint16_t aOutputLength)
{
    otPlatEntropyGet(aOutput, aOutputLength);
}

/*******************************************************************************
 * Radio
 ******************************************************************************/
/* Get current state of the radio*/
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

/* Get the EUI64 value from the device*/
void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    bool status = false;
    
    /*Get MAC Address from Device, if there exists valid address, 
     *function will return status as true, otherwise false */
    status = IB_GetMACAddr(aIeeeEui64);
    
    if (!status)
    {
        for (uint8_t i = 0; i < OT_EXT_ADDRESS_SIZE; i++)
        {
            aIeeeEui64[i] = systemUniqueId[i];
        }
    }  
}

/* Set the PAN ID for address filtering */
void otPlatRadioSetPanId(otInstance *aInstance, uint16_t aPanId)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Set Pan ID: 0x%04X", aPanId);
    
    radioTrxOff();
    
    PHY_PibSet(macPANId, (PibValue_t *) & aPanId);
    
    radioRestore();
    
    utilsSoftSrcMatchSetPanId(aPanId);
}

/* Set the Extended Address for address filtering */
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

/* Set the Short Address for address filtering*/
void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    
    radioTrxOff();
    
    PHY_PibSet(macShortAddress, (PibValue_t *) & aAddress);
    
    radioRestore();
}

/* Check whether radio is enabled or not */
bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return (sState != OT_RADIO_STATE_DISABLED);
}

/* Enable the radio */
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

/* Disable the radio*/
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

/* Transition the radio from Receive to Sleep (turn off the radio) */
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

/* Transition the radio from Sleep to Receive (turn on the radio). */
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

/* Begin the transmit sequence on the radio */
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
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        updateIeInfoTxFrame(aFrame);
#endif
    
    if (otMacFrameIsSecurityEnabled(aFrame) && otMacFrameIsKeyIdMode1(aFrame) && 
        (!aFrame->mInfo.mTxInfo.mIsARetx) && (!aFrame->mInfo.mTxInfo.mIsSecurityProcessed))
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
   
    txFrame.mpdu = &sTxRxBuffer[0];
    
    if(otMacFrameIsVersion2015(aFrame))
    {
        if (PHY_SUCCESS == PHY_Tx2015Frame(&txFrame, csmaMode, performRetry))
        {
            //Frame transmitted successfully
            sState = OT_RADIO_STATE_TRANSMIT;
            otPlatRadioTxStarted(aInstance, aFrame);
        }
        else
        {
            error = OT_ERROR_FAILED;
        }
    }
    else
    {
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
    }

exit:

    return error;
}

/* Get the radio transmit frame buffer */
otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return &sTransmitFrame;
}

/* Get the most recent RSSI measurement */
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

/* Get the radio capabilities */
otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    return  OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_TRANSMIT_RETRIES |
            OT_RADIO_CAPS_ENERGY_SCAN | OT_RADIO_CAPS_TRANSMIT_SEC | OT_RADIO_CAPS_TRANSMIT_TIMING ;
}

/* Get the status of promiscuous mode */
bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return sPromiscuous;
}

/* Enable or disable promiscuous mode */
void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    PibValue_t pibValue;
    
    pibValue.pib_value_bool = aEnable;

    sPromiscuous = aEnable;
    
    PHY_PibSet(macPromiscuousMode, &pibValue);
}

/* Enable/Disable source address match feature 
 * The source address match feature controls how the radio layer decides the 
 * "frame pending" bit for acks sent in response to data request commands from children
 * */
void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

     sIsSrcMatchEnabled = aEnable;
}

/* Begin the energy scan sequence on the radio */
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

/* Gets the radio's transmit power in dBm.*/
otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    otError error = OT_ERROR_NONE;
  
    otEXPECT_ACTION(aPower != NULL, error = OT_ERROR_INVALID_ARGS);
    PHY_PibGet(phyTransmitPower, (uint8_t *)aPower);

exit:
    return error;
}

/* Set the radio's transmit power in dBm.*/
otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    OT_UNUSED_VARIABLE(aInstance);

    otLogDebgPlat("Radio set default TX power: %d", aPower);

    PHY_PibSet(phyTransmitPower, (PibValue_t *) &aPower);

    return OT_ERROR_NONE;
}

/* Get the radio's CCA ED threshold in dBm */
otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aThreshold);

    return OT_ERROR_NOT_IMPLEMENTED;
}

/* Set the radio's CCA ED threshold in dBm */
otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aThreshold);

    return OT_ERROR_NOT_IMPLEMENTED;
}

/* Get the radio receive sensitivity value in dBm */
int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    int8_t sensitivity = INT8_MIN;
    
    OT_UNUSED_VARIABLE(aInstance);
    
    //Get the radio base RSSI value
    sensitivity = PHY_GetRSSIBaseVal();

    return sensitivity;
}

#if OPENTHREAD_FTD || OPENTHREAD_RADIO
/* Callback function to get the information on Frame Pending for child on reception 
 * of Data Request or DataFrame (incase of Enhanced Frame Pending */
bool PHY_IsFramePendingFromNextLayer(PHY_Addr_t *addr, uint8_t *addrMode)
{
    //By default framePendingBit should be set as per Spec
    bool framePendingSet = false;
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
/* Update MAC keys and key index */
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

/* Sets the current MAC frame counter value */
void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    OT_UNUSED_VARIABLE(aInstance);

    sMacFrameCounter = aMacFrameCounter;
    
}

/* Sets the current MAC frame counter value only if the new given value is larger than the current value */
void otPlatRadioSetMacFrameCounterIfLarger(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    OT_UNUSED_VARIABLE(aInstance);
    
    OSAL_CRITSECT_DATA_TYPE intStatus;

   intStatus = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);

    if (aMacFrameCounter > sMacFrameCounter)
    {
        sMacFrameCounter = aMacFrameCounter;
    }

    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, intStatus);
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

/* Get the current estimated time (in microseconds) of the radio chip */
uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    uint64_t countNow = 0;
    
    countNow = otPlatTimeGet();

    return countNow;
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
/* Enable or disable CSL receiver */
otError otPlatRadioEnableCsl(otInstance         *aInstance,
                             uint32_t            aCslPeriod,
                             otShortAddress      aShortAddr,
                             const otExtAddress *aExtAddr)
{
    sCslPeriod = aCslPeriod;

    return OT_ERROR_NONE;
}

/* Update CSL sample time in radio driver */
void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    OT_UNUSED_VARIABLE(aInstance);

    sCslSampleTime = aCslSampleTime;
}

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

/* Get the current accuracy, in units of Â± ppm, of the clock used for scheduling CSL operations */
uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return (uint8_t)(otPlatTimeGetXtalAccuracy() / 2);
}

/* The fixed uncertainty of the Device for scheduling CSL Transmissions in units of 10 microseconds */
uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return (uint8_t)CSL_UNCERT;
}


#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
/* Enable/disable or update Enhanced-ACK Based Probing in radio for a specific Initiator */
otError otPlatRadioConfigureEnhAckProbing(otInstance          *aInstance,
                                          otLinkMetrics        aLinkMetrics,
                                          const otShortAddress aShortAddress,
                                          const otExtAddress  *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return otLinkMetricsConfigureEnhAckProbing(aShortAddress, aExtAddress, aLinkMetrics);
}
#endif
