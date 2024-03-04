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
 *   This file implements the OpenThread platform abstraction for the alarm.
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


#include "definitions.h"
#include "platform-pic32cx.h"
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/alarm-micro.h>
#include "configuration.h"

void vApplicationDaemonTaskStartupHook( void )
{
    
}

#define INIT_TIMER_INTERVAL   1000

static SYS_TIME_HANDLE sMilliTimerHandle = SYS_TIME_HANDLE_INVALID;
static bool sMilliTimerFired = true;
static uint32_t milliPeriod = 0;

#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
static SYS_TIME_HANDLE sMicroTimerHandle = SYS_TIME_HANDLE_INVALID;
static bool sMicroTimerFired = true;
static uint32_t microPeriod = 0;
#endif

extern OSAL_QUEUE_HANDLE_TYPE OTQueue;

#define XTAL_ACCURACY       40 // The crystal used on PIC32CX-BZ2 devices has ±20ppm accuracy.

typedef enum timer_type_tag
{
    ALARM_MILLI,
    ALARM_MICRO
}timerType_t;

static void sStartTimerAt (timerType_t timerType, SYS_TIME_HANDLE timerHandle, uint32_t period, SYS_TIME_CALLBACK callBack,
    uintptr_t context)
{
    if(SYS_TIME_HANDLE_INVALID != timerHandle)
    {
          SYS_TIME_TimerReload(timerHandle, 0, period , callBack, context, SYS_TIME_PERIODIC);
          SYS_TIME_TimerStart(timerHandle);
    }
    else
    {
        if(timerType == ALARM_MILLI)
        {
            sMilliTimerHandle = SYS_TIME_CallbackRegisterMS( callBack, context, SYS_TIME_CountToMS(period), SYS_TIME_PERIODIC);   
        }
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
        else
        {
            sMicroTimerHandle = SYS_TIME_CallbackRegisterUS( callBack, context, SYS_TIME_CountToUS(period), SYS_TIME_PERIODIC);   
        }
#endif
    }
}
        
        

/*********************  Milli Timer Implementation ****************************/
static void milliTimerCallback(uintptr_t context)
{
    OT_Msg_T otAlarmMsg;
    
    SYS_TIME_HANDLE *timerHandle = (SYS_TIME_HANDLE *)context ;
    
    if((NULL != timerHandle) && (*timerHandle == sMilliTimerHandle) && (milliPeriod != 0))
    {
		/* Stop the Periodic Milli Timer once it expires. 
		 * Higher layer will start the timer again*/
        SYS_TIME_TimerStop(sMilliTimerHandle);
		milliPeriod = 0;
    }  
    sMilliTimerFired = true;
    otAlarmMsg.OTMsgId = OT_MSG_TMR_MILLI_CB_DONE;
    
    OSAL_QUEUE_Send(&OTQueue, &otAlarmMsg, 0);
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    /* Timer is already running, stop the timer first and reload the timer with new interval*/
    if((!sMilliTimerFired) && (sMilliTimerHandle != SYS_TIME_HANDLE_INVALID))
    {
        SYS_TIME_TimerStop(sMilliTimerHandle);
    }
    
    if(aDt)
    {   
		/* Start the Periodic Micro Timer with aDt */
        sStartTimerAt(ALARM_MILLI, sMilliTimerHandle, (uint32_t)SYS_TIME_MSToCount(aDt), milliTimerCallback, (uintptr_t)&sMilliTimerHandle);
        sMilliTimerFired = false;
        
    }
    else
    {	
		/* Directly invoke callback if aDt = 0 */
        milliTimerCallback((uintptr_t)&sMilliTimerHandle);
    }
    
    milliPeriod = aDt;
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    
    if(sMilliTimerHandle != SYS_TIME_HANDLE_INVALID)
    {
        SYS_TIME_TimerStop(sMilliTimerHandle);
    }
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    uint32_t currTimeMs = 0;

    currTimeMs = SYS_TIME_CountToMS(SYS_TIME_CounterGet());
    
    return currTimeMs ;
}
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
/***********************MicroSec Timer Implementation**************************/
static void microTimerCallback(uintptr_t context)
{
    OT_Msg_T otAlarmMsg;
    SYS_TIME_HANDLE *timerHandle = (SYS_TIME_HANDLE *)context ;
    
    if((NULL != timerHandle) && (*timerHandle == sMicroTimerHandle) && (microPeriod != 0))
    {
		/* Stop the Periodic Micro Timer once it expires. 
		 * Higher layer will start the timer again*/
        SYS_TIME_TimerStop(sMicroTimerHandle);
		microPeriod = 0;
    } 
    
    sMicroTimerFired = true;
    otAlarmMsg.OTMsgId = OT_MSG_TMR_MICRO_CB_DONE;
    OSAL_QUEUE_Send(&OTQueue, &otAlarmMsg,0);
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    /* Timer is already running, stop the timer first and reload the timer with new interval*/
    if((!sMicroTimerFired) && (sMicroTimerHandle != SYS_TIME_HANDLE_INVALID) && (aDt != 0))
    {
        SYS_TIME_TimerStop(sMicroTimerHandle);
    }
    
    if(aDt)
    {
		/* Start the Periodic Micro Timer with aDt */
        sStartTimerAt(ALARM_MICRO, sMicroTimerHandle, SYS_TIME_USToCount(aDt), microTimerCallback, (uintptr_t)&sMicroTimerHandle);       
        sMicroTimerFired = false;
    }
    else
    {
		/* Directly invoke callback if aDt = 0 */
        microTimerCallback((uintptr_t)&sMicroTimerHandle);
    }
    
    microPeriod = aDt;
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    if(sMicroTimerHandle != SYS_TIME_HANDLE_INVALID)
    {
        SYS_TIME_TimerStop(sMicroTimerHandle);
    }
}


#endif

uint32_t otPlatAlarmMicroGetNow(void)
{
    uint32_t currTimeUs = 0;  
    
    currTimeUs = SYS_TIME_CountToUS(SYS_TIME_CounterGet());
    
    return currTimeUs ;
}

/******************************************************************************/
void pic32cxAlarmInit(void)
{
	/* Create SYS Time Periodic timer for alarmMilliTimer*/
    sMilliTimerHandle = SYS_TIME_TimerCreate(0, SYS_TIME_MSToCount(INIT_TIMER_INTERVAL), 
			&milliTimerCallback, (uintptr_t)&sMilliTimerHandle, SYS_TIME_PERIODIC);
			
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
	/* Create SYS Time Periodic timer for alarmMicroTimer */
    sMicroTimerHandle = SYS_TIME_TimerCreate(0, SYS_TIME_USToCount(INIT_TIMER_INTERVAL), 
			&microTimerCallback, (uintptr_t)&sMicroTimerHandle, SYS_TIME_PERIODIC);
#endif
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
#if OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
            if(OT_MSG_TMR_MICRO_CB_DONE == otAlarmMsgId)
            {
                otPlatAlarmMicroFired(aInstance);
            }
#endif
            
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

uint16_t otPlatTimeGetXtalAccuracy(void)
{
    uint16_t accuracy = 0;
    accuracy = XTAL_ACCURACY;
    return accuracy;
}