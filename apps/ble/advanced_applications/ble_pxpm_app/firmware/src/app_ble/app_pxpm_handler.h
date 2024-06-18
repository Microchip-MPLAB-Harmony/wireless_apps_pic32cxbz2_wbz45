/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_pxpm_handler.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef APP_PXPM_HANDLER_H
#define APP_PXPM_HANDLER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>
#include "ble_pxpm/ble_pxpm.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END
    
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
 
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

#define TH_MID_ALERT                    ((uint8_t)0x48) //  75db
#define TH_HIGH_ALERT                   ((uint8_t)0x5F) //  95db
#define PWR_CTRL_TH_HIGH                ((uint8_t)0x50) //  80db
#define PWR_CTRL_TH_MILD                ((uint8_t)0x3C) //  60db

#ifdef __HPA__
#define HIGH_HYSTERESIS                 ((uint8_t)0x3)
#define LOW_HYSTERESIS                  ((uint8_t)0x3)
#else
#define HIGH_HYSTERESIS                 ((uint8_t)0x4)
#define LOW_HYSTERESIS                  ((uint8_t)0x4)
#endif

typedef uint16_t (*reissue)(uint16_t , BLE_PXPM_AlertLevel_T);

/*******************************************************************************
  Function:
    void APP_PxpmEvtHandler(BLE_PXPM_Event_T *p_event)

  Summary:
     BLE application Promixity profile monitor event handler.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_PxpmEvtHandler(BLE_PXPM_Event_T *p_event);


void APP_PXPM_Recovery(void);
void APP_PXPM_RssiCheck(uint16_t connHandle,uint8_t link_budget);
void APP_PXPM_BackupService(reissue func,uint16_t connHandle,BLE_PXPM_AlertLevel_T level);
void APP_PXPM_ReissueService(void);
void APP_SetResolvingList(bool isSet);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* APP_PXPM_HANDLER_H */


/*******************************************************************************
 End of File
 */

