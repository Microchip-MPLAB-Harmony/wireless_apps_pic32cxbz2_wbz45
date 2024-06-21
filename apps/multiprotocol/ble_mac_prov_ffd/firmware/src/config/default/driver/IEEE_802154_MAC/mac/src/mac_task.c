/*******************************************************************************
  MAC Task

  File Name:
    mac_task.c

  Summary:
    This file contains MAC RTOS Task related fucntions

  Description:
    None
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
// DOM-IGNORE_END

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include "mac.h"
#include "mac_api.h"
#include "definitions.h"
#include "mac_tasks.h"
#include "phy.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Externals                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

static OSAL_SEM_HANDLE_TYPE semMacInternalHandler;
extern OSAL_SEM_HANDLE_TYPE semPhyInternalHandler;
static QueueSetHandle_t xQueueSet;
static QueueSetMemberHandle_t xActivatedMember;

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Macros                                                            */
/* ************************************************************************** */
/* ************************************************************************** */

/* Counting semaphore length to handle mac internal task processing */
#define MAC_INTERNAL_SEM_LENGTH	20
/* Counting semaphore length to handle phy internal task processing */
#define PHY_INTERNAL_SEM_LENGTH	20
/* No block wait */
#define NO_BLOCK_WAIT 0

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Prototypes                                                  */
/* ************************************************************************** */
/* ************************************************************************** */
    
void WPAN_Tasks(void)
{
    OSAL_RESULT result = OSAL_RESULT_TRUE;
    /* Create Semaphore for IEEE_802154_MAC */
    result = OSAL_SEM_Create(&semMacInternalHandler, OSAL_SEM_TYPE_COUNTING, MAC_INTERNAL_SEM_LENGTH, NO_BLOCK_WAIT);
    /* Create the queue set large enough to hold an event for every space in
    every queue and semaphore that is to be added to the set. */
    result = OSAL_QUEUE_CreateSet(&xQueueSet, PHY_INTERNAL_SEM_LENGTH + MAC_INTERNAL_SEM_LENGTH);
    /* Add the queues and semaphores to the set.  Reading from these queues and
  semaphore can only be performed after a call to xQueueSelectFromSet() has
  returned the queue or semaphore handle from this point on. */
    result = OSAL_QUEUE_AddToSet( &semMacInternalHandler, &xQueueSet );
    result = OSAL_QUEUE_AddToSet( &semPhyInternalHandler, &xQueueSet );
    while (true)
    {
    /* Block to wait for something to be available from the queues or
     semaphore that have been added to the set.*/
     result = OSAL_QUEUE_SelectFromSet(&xActivatedMember, &xQueueSet, OSAL_WAIT_FOREVER );

    /* Which set member was selected?  Receives/takes can use a block time
    of zero as they are guaranteed to pass because xQueueSelectFromSet()
    would not have returned the handle unless something was available. */
    
    if( xActivatedMember == semPhyInternalHandler )
    {
      /*Process Internal Stack Events*/
      result = OSAL_SEM_Pend(&semPhyInternalHandler, NO_BLOCK_WAIT);
      PHY_TaskHandler();
    }
    else if( xActivatedMember == semMacInternalHandler )
    {
      result = OSAL_SEM_Pend(&semMacInternalHandler, NO_BLOCK_WAIT);
      WPAN_Task();
    }
    else{
        // do nothing
    }
    (void)result;
    }
}

/* ************************************************************************** */
void WPAN_PostTask(void)
{
    OSAL_RESULT result = OSAL_RESULT_TRUE;
    
    if(semMacInternalHandler != NULL)
    {
        result = OSAL_SEM_Post(&semMacInternalHandler);
    }
    else
    {
        WPAN_Task();
    }
    
    (void)result;
}

/* *****************************************************************************
 End of File
 */
