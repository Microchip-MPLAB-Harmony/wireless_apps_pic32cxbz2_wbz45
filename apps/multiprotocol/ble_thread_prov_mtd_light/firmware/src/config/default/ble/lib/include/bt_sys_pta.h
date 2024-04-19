/*******************************************************************************
  Bluetooth Sys Pta Header File

  Company:
    Microchip Technology Inc.

  File Name:
    bt_sys_pta.h

  Summary:
    This file contains the BT system pta definitions for the project.

  Description:
    This file contains the BT system pta definitions for the project.
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

/**
 * @addtogroup BT_SYS
 * @{
 */

#ifndef BT_SYS_PTA_H
#define BT_SYS_PTA_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdbool.h>
#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
/**@addtogroup BT_SYS_PTA_STRUCTS Structures
 * @{ */

/**@brief PTA callback type. This callback function sends PTA bt_active event to the application.
 *
 * @param[in] btActive         Indicate BT requests RF or not. True means BT requests RF and false means BT doesn't request RF.
 */
typedef void (*BT_SYS_PtaEventBtActiveCb_T)(bool btActive);

/**@brief PTA callback type. This callback function sends PTA bt_priority event to the application.
 *
 * @param[in] btPriority       Indicate BT priority is high or low. True is high priority and false is low priority.
 */
typedef void (*BT_SYS_PtaEventBtPriorityCb_T)(bool btPriority);


/**@brief PTA callback type. This callback function get PTA wlan_active status from the application.
 *
 * @retval true                                 WLAN is active.
 * @retval false                                WLAN is not active.
 */
typedef bool (*BT_SYS_PtaGetWlanStatusCb_T)(void);

/**@} */ //BT_SYS_PTA_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

/**@addtogroup BT_SYS_PTA_FUNS Functions
 * @{ */

/**@brief The API is used to Initial BT PTA.
 *
 * @param[in] notifyBtActiveCb                            notifyBtActive callback.
 * @param[in] notifyBtPriorityCb                          notifyBtPriority callback.
 * @param[in] getWlanActiveStatusCb                       getWlanActiveStatus callback.
 *
 */
void BT_SYS_PtaInit(BT_SYS_PtaEventBtActiveCb_T notifyBtActiveCb, BT_SYS_PtaEventBtPriorityCb_T notifyBtPriorityCb, BT_SYS_PtaGetWlanStatusCb_T getWlanActiveStatusCb);

/**@brief The API is used to handle wlanActiveIrq.
 * @note This API should be called in the application while wlanActiveIrq occurs
 */
void BT_SYS_PtaWlanActiveIrqHandler();

/**@} */ //BT_SYS_PTA_FUNS

/**
  @}
*/

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif//BT_SYS_PTA_H

/**
  @}
*/
