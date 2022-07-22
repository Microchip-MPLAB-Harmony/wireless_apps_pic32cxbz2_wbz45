/*******************************************************************************
  Application Transparent Common Function Header File

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


#ifndef APP_UTILITY_H
#define APP_UTILITY_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_UTILITY_MAX_QUEUE_NUM      16

/** @brief Retrieve an uint32_t data in little endian format from an address of byte array.
 * @{ */
#define APP_UTILITY_BUF_LE_TO_U32(p_value, p_src)\
    *p_value = ((uint32_t)*(p_src)) + (((uint32_t)*(p_src + 1)) << 8) + (((uint32_t)*(p_src + 2)) << 16)\
        + (((uint32_t)*(p_src + 3)) << 24);
/** @} */

/** @brief Retrieve an uint32_t data in big endian format from an address of byte array.
 * @{ */
#define APP_UTILITY_BUF_BE_TO_U32(p_value, p_src)\
    *p_value = (((uint32_t)*(p_src)) << 24) + (((uint32_t)*(p_src + 1)) << 16) + (((uint32_t)*(p_src+ 2)) << 8)\
        + (uint32_t)*(p_src + 3);
/** @} */


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

/**@brief The structure contains information about node data stored in a FIFO queue. */
typedef struct APP_UTILITY_NodeData_T {
    uint16_t connHandle;                /**< Connection handle associated with this connection. */
    uint16_t dataLeng;                  /**< Data length for p_data data pointer. */
    uint8_t *p_data;                    /**< Point to stored data. */
    void *p_selfData;                   /**< Point to a private data structure. */
} APP_UTILITY_NodeData_T;

typedef struct APP_UTILITY_Node_T APP_UTILITY_QueueNode_T;

/**@brief The structure contains information about queue node to store data in a FIFO queue. */
typedef struct APP_UTILITY_Node_T
{
    uint16_t connHandle;                /**< Connection handle associated with this connection. */
    uint16_t dataLeng;                  /**< Data length for p_data data pointer. */
    uint8_t *p_data;                    /**< Point to stored data. */
    void *p_selfData;                   /**< Point to a private data structure. */
    APP_UTILITY_QueueNode_T *prev;      /**< Point to a previous node. */
    APP_UTILITY_QueueNode_T *next;      /**< Point to the next node. */
}APP_UTILITY_Node_T;

/**@brief The structure contains information about circular queue element. */
typedef struct APP_UTILITY_QueueElem_T
{
    uint16_t                   dataLeng;    /**< Data length. */
    uint8_t                    *p_data;     /**< Pointer to the data buffer */
} APP_UTILITY_QueueElem_T;

/**@brief The structure contains information about circular queue format. */
typedef struct APP_UTILITY_CircQueue_T
{
    uint8_t                     usedNum;                                /**< The number of data list in circular queue. */
    uint8_t                     writeIdx;                               /**< The Index of data, written in circular queue. */
    uint8_t                     readIdx;                                /**< The Index of data, read in circular queue. */
    APP_UTILITY_QueueElem_T     queueElem[APP_UTILITY_MAX_QUEUE_NUM];   /**< The circular data queue. @ref APP_UTILITY_QueueElem_T.*/
} APP_UTILITY_CircQueue_T;


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

/**@brief The function is to get a queue node.
 *
 * @return A queue node pointer. See @ref APP_UTILITY_Node_T.
 *
 */
APP_UTILITY_QueueNode_T * APP_UTILITY_GetQueueNode(void);

/**@brief The function is to free a queue node.
 *
 * *@param[in] queueNode    Point to a queue node to be freed. See @ref APP_UTILITY_Node_T.
 *
 */
void APP_UTILITY_FreeQueueNode(APP_UTILITY_QueueNode_T *queueNode);

/**@brief The function is to insert data to a FIFO queue.
 *
 * *@param[in] p_nodeData           Point to the insert data. See @ref APP_UTILITY_NodeData_T.
 * *@param[in] p_queueNode          Point to a queue node. See @ref APP_UTILITY_Node_T.
 * *@param[out] p_queueListHead_t   An address to point to a head of a FIFO queue. See @ref APP_UTILITY_Node_T.
 * *@param[out] p_queueListTail_t   An address to point to a tail of a FIFO queue. See @ref APP_UTILITY_Node_T.
 *
 */
void APP_UTILITY_InsertDataToFifoQueue(APP_UTILITY_NodeData_T *p_nodeData, 
    APP_UTILITY_QueueNode_T *p_queueNode, APP_UTILITY_QueueNode_T **p_queueListHead_t, 
    APP_UTILITY_QueueNode_T **p_queueListTail_t);

/**@brief The function is to restore a node to a FIFO queue.
 *
 * *@param[in] p_queueNode          Point to a queue node. See @ref APP_UTILITY_Node_T.
 * *@param[out] p_queueListHead_t   An address to point to a head of a FIFO queue. See @ref APP_UTILITY_Node_T.
 * *@param[out] p_queueListTail_t   An address to point to a tail of a FIFO queue. See @ref APP_UTILITY_Node_T.
 *
 */
void APP_UTILITY_RestoreNodeToFifoQueue(APP_UTILITY_QueueNode_T *p_queueNode, 
    APP_UTILITY_QueueNode_T **p_queueListHead_t, APP_UTILITY_QueueNode_T **p_queueListTail_t);
    
/**@brief The function is to get a node from a FIFO queue.
 *
 * *@param[out] p_queueListHead_t   An address to point to a head of a FIFO queue. See @ref APP_UTILITY_Node_T.
 * *@param[out] p_queueListTail_t   An address to point to a tail of a FIFO queue. See @ref APP_UTILITY_Node_T.
 *
 * @return A queue node pointer. See @ref APP_UTILITY_Node_T.
 */
APP_UTILITY_QueueNode_T * APP_UTILITY_GetFifoQueueNode(APP_UTILITY_QueueNode_T **p_queueListHead_t, 
    APP_UTILITY_QueueNode_T **p_queueListTail_t);

/**@brief The function is to get the valid number stored in a circular queue.
 *
 * *@param[in] p_circQueue_t     Point to the circular queue of insert data. See @ref APP_UTILITY_CircQueue_T.
 *
 * @return A valid number stored in a circular queue.
 */
uint8_t APP_UTILITY_GetValidCircQueueNum(APP_UTILITY_CircQueue_T *p_circQueue_t);

/**@brief The function is to insert data to a circular queue.
 *
 * *@param[in] dataLeng         Data length
 * *@param[in] p_data           Pointer to the data buffer
 * *@param[out] p_circQueue_t   Point to the circular queue of insert data. See @ref APP_UTILITY_CircQueue_T.
 *
 * @return A status.
 */
uint16_t APP_UTILITY_InsertDataToCircQueue(uint16_t dataLeng, uint8_t *p_data, 
    APP_UTILITY_CircQueue_T *p_circQueue_t);

/**@brief The function is to get an element from a circular queue.
 *
 * *@param[in] p_circQueue_t     Point to the circular queue of insert data. See @ref APP_UTILITY_CircQueue_T.
 *
 * @return A data queue pointer. See @ref APP_UTILITY_QueueElem_T.
 */
APP_UTILITY_QueueElem_T * APP_UTILITY_GetElemCircQueue(APP_UTILITY_CircQueue_T *p_circQueue_t);

/**@brief The function is to free an element from a circular queue.
 *
 * *@param[in] p_circQueue_t     Point to the circular queue of insert data. See @ref APP_UTILITY_CircQueue_T.
 *
 */
// void APP_UTILITY_FreeElemCircQueue(APP_UTILITY_QueueElem_T *p_queueElem_t);
void APP_UTILITY_FreeElemCircQueue(APP_UTILITY_CircQueue_T *p_circQueue_t);

#endif
