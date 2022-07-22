/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_BLE_H
#define _APP_BLE_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <string.h>
#include "bt_sys_log.h"
#include "stack_mgr.h"
#include "mba_error_defs.h"
#include "gap_defs.h"
#include "ble_gap.h"
#include "ble_l2cap.h"
#include "ble_smp.h"
#include "gatt.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_BleStackInit ( void )

  Summary:
     BLE stack complete initialization routine.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleStackInit(void);

/*******************************************************************************
  Function:
    void APP_BleStackInitBasic ( void )

  Summary:
     BLE stack basic initialization routine.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleStackInitBasic(void);

/*******************************************************************************
  Function:
    void APP_BleStackInitAdvance ( void )

  Summary:
     BLE stack advanced initialization routine.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleStackInitAdvance(void);


/*******************************************************************************
  Function:
    void APP_BleStackEvtHandler( STACK_Event_T *p_stackEvt )

  Summary:
     Function for handling APP_MSG_BLE_STACK_EVT message.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleStackEvtHandler(STACK_Event_T *p_stackEvt);


/*******************************************************************************
  Function:
    void APP_BleStackLogHandler(BT_SYS_LogEvent_T *p_logEvt)

  Summary:
     Function for handling APP_MSG_BLE_STACK_LOG message.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleStackLogHandler(BT_SYS_LogEvent_T *p_logEvt);

#endif /* _APP_BLE_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

