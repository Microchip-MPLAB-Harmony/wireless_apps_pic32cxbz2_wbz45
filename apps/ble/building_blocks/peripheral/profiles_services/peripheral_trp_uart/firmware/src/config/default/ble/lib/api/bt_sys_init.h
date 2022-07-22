/*******************************************************************************
  Bluetooth System Header File

  Company:
    Microchip Technology Inc.

  File Name:
    bt_sys_init.h

  Summary:
    This file contains the BLE System functions for the project.

  Description:
    This file contains the BLE System functions for the project. The "BT_SYS_Init" 
    function shall be called in the "SYS_Initialize" function to initialize the 
    this modules in the system. And the "BM_Task"  shall be called in the "SYS_Tasks".
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

#ifndef BT_SYS_INIT_H
#define BT_SYS_INIT_H

#include <stdbool.h>

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define BT_SYS_DEV_ADDR_LEN     		7

// *****************************************************************************
// *****************************************************************************
// SSection: Data Types
// *****************************************************************************

/**@brief Bluetooth system configuration. */
typedef struct BT_SYS_Cfg_T
{
    uint8_t     addrValid:1;                        /**< Set TRUE if devAddr field is valid. */
    uint8_t     rssiOffsetValid:1;                  /**< Set TRUE if rssiOffset field is valid. */
    uint8_t     txCompValid:1;                      /**< Set TRUE if rfTxPathComp field is valid. */
    uint8_t     rxCompValid:1;                      /**< Set TRUE if rfRxPathComp field is valid. */

    uint8_t     devAddr[BT_SYS_DEV_ADDR_LEN];       /**< Device address. */
    int8_t      rssiOffset;                         /**< RSSI offset. */
    int16_t     rfTxPathComp;                       /**< RF TX path compensation. */
    int16_t     rfRxPathComp;                       /**< RF RX path compensation. */
} BT_SYS_Cfg_T;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**@brief BT Init Entry
 *
 * @param[in] bleRequestQueueHandle             os handle for bm task
 * @param[in] osal_api_list                     osal list array
 * @param[in] sram_vector_table                 vector table
 * @param[in] p_cfg                             Bluetooth system configuration
 *
 * @retval None
 */
/*************************************************************************************************/
void BT_SYS_Init(void *bleRequestQueueHandle, void *osal_api_list, uint32_t *sram_vector_table, BT_SYS_Cfg_T *p_cfg);


/**@brief The API is used to retrieve BT version.
 *
 * @param[out] p_version                        Point to version buffer.
 *
 * @retval None
 */
void BT_SYS_ReadVersion(uint32_t *p_version);

/**@brief The API is used to query if BT is ready for idle.
 *
 * @param[out] None
 *
 * @retval A boolean value, True means ready for idle
 */
bool BT_SYS_Ready4Idle();


/**@brief The API is used to request if BT can suspend RF
 *
 * @param[out] None
 *
 * @retval A boolean value, True means BT can suspend RF
 */
bool BT_SYS_RfSuspendReq(uint8_t enable);



/**@brief BM_Task
 *
 * @retval None
 */
/*************************************************************************************************/
void BM_Task(void *pdata);

#endif//BT_SYS_INIT_H
