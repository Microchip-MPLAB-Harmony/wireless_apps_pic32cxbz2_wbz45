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
 *   This file implements the OpenThread platform abstraction for the alarm.
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
#include <openthread/platform/alarm-milli.h>

#include "configuration.h"
#include "timers.h"

#define TEST_SYS_TIME_RUNNING
#define SYS_TIMER_32BIT_MASK_MSB 0xFFFFFFFF00000000
#define SYS_TIMER_MSB_SHIFT_COUNT  32
#define NO_BLOCK_WAIT   0
#define INIT_TIMER_INTERVAL_MS   1000

typedef union {
    TickType_t sysTime32;
    uint64_t sysTime64;
}otSysTime_t;

static otSysTime_t otSysTime;

static uint32_t          sDeltaTime = 0;
static uint32_t          sStartTime = 0;
static bool sMilliTimerFired = true;

static TimerHandle_t sMilliTimerHandle = NULL;


extern OSAL_QUEUE_HANDLE_TYPE OTQueue;

void vApplicationDaemonTaskStartupHook( void )
{
    
}

static void milliTimeCallback(TimerHandle_t xTimer)
{
    OT_Msg_T otAlarmMsg;
    sMilliTimerFired = true;
    otAlarmMsg.OTMsgId = OT_MSG_TMR_MILLI_CB_DONE;
    xTimerStop(sMilliTimerHandle, NO_BLOCK_WAIT);
    OSAL_QUEUE_Send(&OTQueue, &otAlarmMsg,0);
}
void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    sDeltaTime = aDt;
    sStartTime = aT0;
    /* Timer is already running, stop the timer first and reload the timer with new interval*/
    if(!sMilliTimerFired)
    {
        xTimerStop(sMilliTimerHandle, NO_BLOCK_WAIT);
    }
    if(aDt)
    {
      if(NULL != sMilliTimerHandle)
      {
          xTimerChangePeriod(sMilliTimerHandle, (aDt / portTICK_PERIOD_MS), NO_BLOCK_WAIT);
          xTimerStart(sMilliTimerHandle, NO_BLOCK_WAIT);
      }
      else
      {
          sMilliTimerHandle = xTimerCreate("Milli_Timer", (aDt / portTICK_PERIOD_MS), false, ( void * ) 0, milliTimeCallback);
          xTimerStart(sMilliTimerHandle, NO_BLOCK_WAIT);
      }
      sMilliTimerFired = false;
    }
    else
    {
        milliTimeCallback(NULL);
    }
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    if(sMilliTimerHandle != NULL)
    {
        xTimerStop(sMilliTimerHandle, NO_BLOCK_WAIT);
    }
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    uint32_t currTickCount = 0;
    if(otSysTime.sysTime32 > xTaskGetTickCount())
    {
        otSysTime.sysTime64 = otSysTime.sysTime64 & SYS_TIMER_32BIT_MASK_MSB;
        otSysTime.sysTime64 = otSysTime.sysTime64 >> SYS_TIMER_MSB_SHIFT_COUNT;
        otSysTime.sysTime64++;
        otSysTime.sysTime64 = otSysTime.sysTime64 << SYS_TIMER_MSB_SHIFT_COUNT;
    }
    currTickCount = xTaskGetTickCount();
    otSysTime.sysTime64 &= SYS_TIMER_32BIT_MASK_MSB;
    otSysTime.sysTime64 |= (uint64_t)currTickCount;
    return (uint32_t)otSysTime.sysTime64;
}


void pic32cxAlarmInit(void)
{
  sMilliTimerHandle = xTimerCreate("Milli_Timer", (INIT_TIMER_INTERVAL_MS / portTICK_PERIOD_MS), false, ( void * ) 0, milliTimeCallback);
}

void pic32cxAlarmProcess(otInstance *aInstance, OT_MsgId_T otAlarmMsgId)
{

#if OPENTHREAD_CONFIG_DIAG_ENABLE

        if (otPlatDiagModeGet())
        {
            otPlatDiagAlarmFired(aInstance);
        }
        else
#endif
        {
            if(OT_MSG_TMR_MILLI_CB_DONE == otAlarmMsgId)
            {
                otPlatAlarmMilliFired(aInstance);
            }
        }
}


uint64_t otPlatTimeGet(void)
{   
    uint64_t countNow = 0;
    
    countNow = SYS_TIME_Counter64Get();

    return (((uint64_t)countNow * 1000000) / SYS_TIME_FrequencyGet());
}