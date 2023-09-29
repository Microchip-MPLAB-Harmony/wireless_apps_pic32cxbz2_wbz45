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
 *   This file implements the OpenThread platform abstraction for UART communication.
 *
 */


//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#include "definitions.h"
#include "platform-pic32cx.h"
#include "utils/uart.h"

extern OSAL_QUEUE_HANDLE_TYPE OTQueue;
OSAL_SEM_HANDLE_TYPE OtUartSemHandler;

//#define TEST_USART_RECEPTION

enum
{
    kBaudRate          = 115200,
    kReceiveBufferSize = 512,
};

typedef struct RecvBuffer
{
    // The data buffer
    uint8_t mBuffer[kReceiveBufferSize];
    // The offset of the first item written to the list.
    uint16_t mHead;
    // The offset of the next item to be written to the list.
    uint16_t mTail;
} RecvBuffer;

static RecvBuffer sReceive;

static const uint8_t *sTransmitBuffer = NULL;
static bool sTransmitDone = false;
static bool SemaphorePended = false;

#if OPEN_THREAD_UART_ENABLE
static DRV_HANDLE uartHandle;
static DRV_USART_BUFFER_HANDLE uartWriteBufferHandle;
static DRV_USART_BUFFER_HANDLE uartReadBufferHandle;

static void uartBufferEventHandler(DRV_USART_BUFFER_EVENT event, DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t context)
{
    OT_Msg_T otUARTMsg;
	
    if (event == DRV_USART_BUFFER_EVENT_COMPLETE)
    {
        if (bufferHandle == uartWriteBufferHandle)
        {
            sTransmitDone = true;
            if(SemaphorePended)
            {
                sTransmitBuffer = NULL;
                SemaphorePended = false;
                OSAL_SEM_PostISR(&OtUartSemHandler);
            }
            else
            {
                otUARTMsg.OTMsgId = OT_MSG_UART_TX_DONE;
                OSAL_QUEUE_Send(&OTQueue, &otUARTMsg,0);
            }
        }
        else if (bufferHandle == uartReadBufferHandle)
        {
#ifdef TEST_USART_RECEPTION
            RGB_LED_GREEN_On();
            RGB_LED_RED_Off();
            RGB_LED_BLUE_Off();
#endif
            if (sReceive.mHead != (sReceive.mTail + 1) % kReceiveBufferSize)
            {
                sReceive.mTail = (sReceive.mTail + 1) % kReceiveBufferSize;
            }

            otUARTMsg.OTMsgId = OT_MSG_UART_RX_DONE;
            OSAL_QUEUE_Send(&OTQueue, &otUARTMsg,0);

            DRV_USART_ReadBufferAdd(uartHandle, (uint8_t *)&sReceive.mBuffer[sReceive.mTail], sizeof(uint8_t), &uartReadBufferHandle);
        }
    }
}
#endif

static void processReceive(void)
{
    // Copy tail to prevent multiple reads
    uint16_t tail = sReceive.mTail;

    // If the data wraps around, process the first part
    if (sReceive.mHead > tail)
    {
       otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, kReceiveBufferSize - sReceive.mHead);

        // Reset the buffer mHead back to zero.
        sReceive.mHead = 0;
    }

    // For any data remaining, process it
    if (sReceive.mHead != tail)
    {
       otPlatUartReceived(sReceive.mBuffer + sReceive.mHead, tail - sReceive.mHead);
#ifdef TEST_USART_RECEPTION
        RGB_LED_GREEN_Off();
        RGB_LED_RED_Off();
        RGB_LED_BLUE_Off();
#endif
        // Set mHead to the local tail we have cached
        sReceive.mHead = tail;
    }
}

otError otPlatUartFlush(void)
{
    while (sTransmitBuffer && !sTransmitDone)
    {
        SemaphorePended = true;
        // Wait until the transmission is done
        OSAL_SEM_Pend(&OtUartSemHandler, OSAL_WAIT_FOREVER);
    }
    
    return OT_ERROR_NONE; 
}

static void processTransmit(void)
{
    if (sTransmitDone)
    {
        sTransmitBuffer = NULL;
        sTransmitDone = false;
       otPlatUartSendDone();
    }
}

void pic32cxUartProcess(OT_MsgId_T otUartMsgId)
{
    if(OT_MSG_UART_RX_DONE == otUartMsgId)
        processReceive();
    else if(OT_MSG_UART_TX_DONE == otUartMsgId)
        processTransmit();
}

otError otPlatUartEnable(void)
{
    sReceive.mHead = 0;
    sReceive.mTail = 0;

	#if OPEN_THREAD_UART_ENABLE
    uartHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_READWRITE);
    DRV_USART_BufferEventHandlerSet(uartHandle, uartBufferEventHandler, (uintptr_t)0);
    DRV_USART_ReadBufferAdd(uartHandle, (uint8_t *)&sReceive.mBuffer[sReceive.mTail], sizeof(uint8_t), &uartReadBufferHandle);
    #endif
	
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
	#if OPEN_THREAD_UART_ENABLE
    DRV_USART_Close(uartHandle);
    #endif
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error = OT_ERROR_NONE;

	sTransmitBuffer = aBuf;
	#if OPEN_THREAD_UART_ENABLE
    DRV_USART_WriteBufferAdd(uartHandle, (uint8_t *)sTransmitBuffer, (size_t)aBufLength, &uartWriteBufferHandle);
	#endif
    return error;
}

void pic32cxUartInit(void)
{
    OSAL_SEM_Create(&OtUartSemHandler, OSAL_SEM_TYPE_BINARY, 0, 0);
}

OT_TOOL_WEAK void otPlatUartSendDone(void)
{
    
}

OT_TOOL_WEAK void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{

}