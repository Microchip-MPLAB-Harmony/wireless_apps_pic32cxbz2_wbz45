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
 *   This file includes the platform-specific initializers.
 *
 */


// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

#ifndef PLATFORM_PIC32CX_H_
#define PLATFORM_PIC32CX_H_

#include <stdint.h>

#include <openthread/instance.h>

#define PLAT_MODULE_ID_MASK 0xFF00
#define PLAT_UART_MODULE_ID 0x0100
#define PLAT_RADIO_MODULE_ID 0x0200
#define PLAT_ALARM_MODULE_ID 0x0300
#define OT_TASKLET_PROCESS_ID 0x0400

typedef enum ot_MsgId_t
{
    OT_MSG_UART_RX_DONE = 0x0101,
    OT_MSG_UART_TX_DONE = 0x0102,
    OT_MSG_RADIO_RX_DONE= 0x0201,
    OT_MSG_RADIO_TX_DONE = 0x0202,
    OT_MSG_RADIO_SCAN_DONE = 0x0203,
    OT_MSG_TMR_MILLI_CB_DONE = 0x0301,
    OT_MSG_TASKLET_PROCESS_PENDING = 0x0401
} OT_MsgId_T;

typedef struct OT_Msg_T
{
    uint16_t OTMsgId;
    //uint8_t OTMsgData[256];
} OT_Msg_T;

// Global OpenThread instance structure
extern otInstance *sInstance;

/**
 * This function initializes the alarm service used by OpenThread.
 *
 */
void pic32cxAlarmInit(void);

/**
 * This function performs alarm driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void pic32cxAlarmProcess(otInstance *aInstance,OT_MsgId_T otAlarmMsgId);

/**
 * This function initializes the radio service used by OpenThread.
 *
 */
void pic32cxRadioInit(void);

/**
 * This function performs radio driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void pic32cxRadioProcess(otInstance *aInstance,OT_MsgId_T otRadioMsg);

/**
 * This function returns 32-bits random value.
 *
 */
uint32_t pic32cxRadioRandomGet(void);

/**
 * This function returns random value sequence.
 *
 */
void pic32cxRadioRandomGetTrue(uint8_t *aOutput, uint16_t aOutputLength);

/**
 * This function initializes the random number service used by OpenThread.
 *
 */
void pic32cxRandomInit(void);

/**
 * This function initializes the Uart service used by OpenThread.
 *
 */
void pic32cxUartInit(void);

/**
 * This function performs UART driver processing.
 *
 */
void pic32cxUartProcess(OT_MsgId_T otUartMsgId);

/**
 * This function returns platform IEEE EUI-64.
 *
 */
void pic32cxGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64);

/**
 * Initialization of Logger driver.
 *
 */
void pic32cxLogInit(void);

#endif // PLATFORM_PIC32CX_H_
