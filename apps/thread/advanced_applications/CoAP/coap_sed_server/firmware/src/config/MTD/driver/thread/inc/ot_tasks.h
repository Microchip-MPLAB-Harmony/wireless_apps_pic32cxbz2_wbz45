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

// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/*
Function:
    void taskOpenThread(void *pvParam)

  Summary:
    RTOS task for the Openthread

  Description:
    This function inturn calls the Openthread task handlers upon reception of 
    Semaphore signals.
 
  Precondition:
    otSysInit(0U,0U),OSAL_QUEUE_Create(&OTQueue, OT_TASK_QUEUE_SIZE, sizeof(OT_Msg_T))
	should have been called before calling this function

  Parameters:
    pvParam : Pointer to open thread Instance 

  Returns:
    None

  Example:
    <code>
    (void) xTaskCreate(taskOpenThread,
                       "ot-task",
                       4096,
                       NULL,
                       3,
                       &taskHandleOpenThread);
 
    void taskOpenThread(void *pvParam);

    TaskHandle_t taskHandleOpenThread;
    
    </code>

  Remarks:
    None 
*/


void taskOpenThread(void *pvParam);

// *****************************************************************************
/*
Function:
    bool otIsIdle(void);

  Summary:
    Thread Stack is Idle

  Description:
    This function will provide whether thread stack is ongoing any operation or Idle. 
 
  Precondition:
    None

  Parameters:
    None

  Returns:
    true: if thread stack is Idle
	false: if thread stack is not Idle.

  Example:
    <code>
	if(otIsIdle())
    {
       SERCOM0_USART_Disable();
       DEVICE_EnterDeepSleep(false, DEVICE_SLEEP_TIME);
    }
    </code>

  Remarks:
    None 
*/

bool otIsIdle(void);



/*******************************************************************************
 End of File
 */