/*******************************************************************************
  Application Utility Function Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_utility.c

  Summary:
    This file contains the Application utility functions for this project.

  Description:
    This file contains the Application utility functions for this project.
    Including the FIFO control (Linked list and circular queue).
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "app.h"
#include "stack_mgr.h"
#include "ble_util/byte_stream.h"
#include "app_utility.h"
#include "app_error_defs.h"
#include "app_trp_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
APP_UTILITY_QueueNode_T * APP_UTILITY_GetQueueNode(void)
{
    APP_UTILITY_QueueNode_T *p_tmpNode;

    p_tmpNode = (APP_UTILITY_QueueNode_T *) OSAL_Malloc(sizeof(APP_UTILITY_QueueNode_T));
    if (p_tmpNode != NULL)
    {
        memset((uint8_t *) p_tmpNode, 0, sizeof(APP_UTILITY_QueueNode_T));
        // p_tmpNode->prev = NULL;
        // p_tmpNode->next = NULL;
    }

    return p_tmpNode;
}

void APP_UTILITY_FreeQueueNode(APP_UTILITY_QueueNode_T *queueNode)
{
    if (queueNode == NULL)
        return;
    if (queueNode->p_data != NULL)
        OSAL_Free(queueNode->p_data);
    if (queueNode->p_selfData != NULL)
        OSAL_Free(queueNode->p_selfData);
    OSAL_Free(queueNode);
}

void APP_UTILITY_InsertDataToFifoQueue(APP_UTILITY_NodeData_T *p_nodeData, APP_UTILITY_QueueNode_T *p_queueNode, 
    APP_UTILITY_QueueNode_T **p_queueListHead_t, APP_UTILITY_QueueNode_T **p_queueListTail_t)
{    
    if (*p_queueListTail_t == NULL)
        *p_queueListHead_t = p_queueNode;
    p_queueNode->connHandle = p_nodeData->connHandle;
    p_queueNode->dataLeng = p_nodeData->dataLeng;
    p_queueNode->p_data = p_nodeData->p_data;
    p_queueNode->p_selfData = p_nodeData->p_selfData;
    if (*p_queueListTail_t != NULL)
        (*p_queueListTail_t)->next = p_queueNode;
    p_queueNode->prev= *p_queueListTail_t;
    *p_queueListTail_t = p_queueNode;
}
void APP_UTILITY_RestoreNodeToFifoQueue(APP_UTILITY_QueueNode_T *p_queueNode, 
    APP_UTILITY_QueueNode_T **p_queueListHead_t, APP_UTILITY_QueueNode_T **p_queueListTail_t)
{
    if (*p_queueListHead_t == NULL)
    {
        p_queueNode->prev = NULL;
        p_queueNode->next = NULL;
        *p_queueListTail_t = p_queueNode;
    }
    else
    {
        p_queueNode->prev = (*p_queueListHead_t)->prev;
        p_queueNode->next = *p_queueListHead_t;
        (*p_queueListHead_t)->prev = p_queueNode;
    }
    *p_queueListHead_t = p_queueNode;
}

APP_UTILITY_QueueNode_T * APP_UTILITY_GetFifoQueueNode(APP_UTILITY_QueueNode_T **p_queueListHead_t, 
    APP_UTILITY_QueueNode_T **p_queueListTail_t)
{
    APP_UTILITY_QueueNode_T *p_tmpNode, *p_tmpNextNode;

    if (*p_queueListHead_t == NULL)
        return *p_queueListHead_t;
    p_tmpNode = *p_queueListHead_t;
    p_tmpNextNode = (*p_queueListHead_t)->next;
    if (p_tmpNextNode != NULL)
        p_tmpNextNode->prev = (*p_queueListHead_t)->prev;
    *p_queueListHead_t = p_tmpNextNode;
    if (*p_queueListHead_t == NULL)
    {
        *p_queueListTail_t = *p_queueListHead_t;
    }

    return p_tmpNode;
}

uint8_t APP_UTILITY_GetValidCircQueueNum(APP_UTILITY_CircQueue_T *p_circQueue_t)
{
    uint8_t validNum = 0;
    
    if (p_circQueue_t != NULL)
    {
        if (p_circQueue_t->usedNum < APP_UTILITY_MAX_QUEUE_NUM)
            validNum = APP_UTILITY_MAX_QUEUE_NUM - p_circQueue_t->usedNum;
    }
    
    return validNum;
}

uint16_t APP_UTILITY_InsertDataToCircQueue(uint16_t dataLeng, uint8_t *p_data, 
    APP_UTILITY_CircQueue_T *p_circQueue_t)
{
    uint16_t status = APP_RES_SUCCESS;

    if ((dataLeng > 0) && (p_data != NULL) && (p_circQueue_t != NULL))
    {
        if (APP_UTILITY_GetValidCircQueueNum(p_circQueue_t) > 0)
        {
            p_circQueue_t->queueElem[p_circQueue_t->writeIdx].dataLeng = dataLeng;
            p_circQueue_t->queueElem[p_circQueue_t->writeIdx].p_data = p_data;
            p_circQueue_t->usedNum++;
            p_circQueue_t->writeIdx++;
            if (p_circQueue_t->writeIdx >= APP_UTILITY_MAX_QUEUE_NUM)
                p_circQueue_t->writeIdx = 0;
        }
        else
            return APP_RES_NO_RESOURCE;
    }
    else
        return APP_RES_INVALID_PARA;
    
    return status;
}

APP_UTILITY_QueueElem_T * APP_UTILITY_GetElemCircQueue(APP_UTILITY_CircQueue_T *p_circQueue_t)
{
    APP_UTILITY_QueueElem_T *p_queueElem_t;

    if (p_circQueue_t != NULL)
    {
        if (p_circQueue_t->usedNum > 0)
        {
            p_queueElem_t = (APP_UTILITY_QueueElem_T *) &(p_circQueue_t->queueElem[p_circQueue_t->readIdx].dataLeng);
        }
        else
            return NULL;
    }
    else
        return NULL;
    
    return p_queueElem_t;
}

void APP_UTILITY_FreeElemCircQueue(APP_UTILITY_CircQueue_T *p_circQueue_t)
{
    if (p_circQueue_t != NULL)
    {
        p_circQueue_t->queueElem[p_circQueue_t->readIdx].dataLeng = 0;
        if (p_circQueue_t->queueElem[p_circQueue_t->readIdx].p_data != NULL)
            OSAL_Free(p_circQueue_t->queueElem[p_circQueue_t->readIdx].p_data);
        if (p_circQueue_t->usedNum > 0)
            p_circQueue_t->usedNum--;
        p_circQueue_t->readIdx++;
        if (p_circQueue_t->readIdx >= APP_UTILITY_MAX_QUEUE_NUM)
            p_circQueue_t->readIdx = 0;
    }
}

