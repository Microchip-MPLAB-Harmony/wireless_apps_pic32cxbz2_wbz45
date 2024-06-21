/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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

/*******************************************************************************
  Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_prov.c

  Summary:
    Todo.

  Description:
    Todo.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_prov.h"

#include "app_trsps_handler.h"

#include <string.h>
#include "stdint.h"
#include "ble_trsps/ble_trsps.h"
#include "osal/osal_freertos_extend.h"
#include "definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
void APP_Prov_StartCommissioning(APP_ProvNwData_T *provNwData)
{
    
    /*Todo*/
    APP_Msg_T   appMsg;
    (void)memcpy((void*)appMsg.msgData,(const void*)provNwData,sizeof(APP_ProvNwData_T)); 
    appMsg.msgId = (uint8_t)APP_MSG_MAC_PROV;
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);       
}

void APP_Prov_ResetToFactoryNew(void)
{
     APP_Msg_T   appMsg;
    appMsg.msgId = (uint8_t)APP_MSG_MAC_FACTORY_RESET;
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);      
    
}


/* *****************************************************************************
 End of File
 */
