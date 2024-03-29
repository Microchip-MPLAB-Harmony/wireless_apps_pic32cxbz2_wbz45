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


/*******************************************************************************
* Copyright (C) [2024], Microchip Technology Inc., and its subsidiaries. All rights reserved.
  
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


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "configuration.h"
#include "definitions.h"
#include "platform-pic32cx.h"

#include <assert.h>
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <openthread/logging.h>
#include <openthread-system.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>
#include <openthread/platform/logging.h>

// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

void taskOpenThread(void *pvParam);
bool otIsIdle(void);

TaskHandle_t taskHandleOpenThread;

extern OSAL_QUEUE_HANDLE_TYPE OTQueue;

otInstance *instance;




void otTaskletsSignalPending(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    OT_Msg_T otTaskletMsg;
    otTaskletMsg.OTMsgId = OT_MSG_TASKLET_PROCESS_PENDING;
    OSAL_QUEUE_Send(&OTQueue, &otTaskletMsg,0);
}

bool otIsIdle(void)
{
    if(otTaskletsArePending(instance))
    {
        return false;
    }

    return true;
}


void taskOpenThread(void *pvParam)
{
    OT_Msg_T   otMessage = {0};
    instance = (otInstance *) pvParam;
    
    
pseudo_reset:   

    instance = otInstanceInitSingle();
    assert(instance);
    otSysProcessDrivers(instance);
    otLoggingSetLevel(OT_LOG_LEVEL_NONE);
    
    while (true)
    {
        while (!otSysPseudoResetWasRequested())
        {
             /* Block to wait for something to be available from the queues or
              semaphore that have been added to the set.*/
                OSAL_QUEUE_Receive(&OTQueue, &otMessage, OSAL_WAIT_FOREVER);
                switch (otMessage.OTMsgId & PLAT_MODULE_ID_MASK)
                {
                    case PLAT_RADIO_MODULE_ID:
                    {
                        pic32cxRadioProcess(instance, otMessage.OTMsgId);
                        break;
                    }
                    case PLAT_ALARM_MODULE_ID:
                    {
                        pic32cxAlarmProcess(instance, otMessage.OTMsgId);
                        break;
                    }
                    case OT_TASKLET_PROCESS_ID:
                    {
                        otTaskletsProcess(instance);
                        break;
                    }
                    default:
                        break;
                }

        }
        
        otInstanceFinalize(instance);
    }
    goto pseudo_reset;
}


/*******************************************************************************
 End of File
 */