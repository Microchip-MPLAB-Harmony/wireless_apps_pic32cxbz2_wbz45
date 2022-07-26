/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_dsadv.h

  Summary:
    This header file provides prototypes and definitions for the application BLE Deep Sleep Advertising (DSADV).

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_BLE_DSADV_H
#define _APP_BLE_DSADV_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************




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

/*******************************************************************************
  Function:
    bool APP_BleDsadvIsEnable(void)

  Summary:
     Check if DSADV has been performed or not.

  Description:
     Return true means DSADV has been performed and the device wakes up from deep sleep.
     Return false means DSADV has not been performed yet and the device boots up from power on reset.

  Precondition:

  Parameters:
    None.

  Returns:
    True means DSADV has been performed; false means not.

*/
bool APP_BleDsadvIsEnable(void);

/*******************************************************************************
  Function:
    void APP_BleDsadvStart (bool flag)

  Summary:
     Start DSADV.

  Description:
     This function is used to start DSADV in the initialization.

  Precondition:

  Parameters:
    flag            True means the device wakes up from deep sleep; false means the device boots from power on reset.

  Returns:
    None.

*/
void APP_BleDsadvStart(bool flag);

/*******************************************************************************
  Function:
    void APP_BleDsadvRestart ( void )

  Summary:
     Restart DSADV when the BLE link is terminated.

  Description:
     This function is used to restart DSADV when the BLE link is terminated.

  Precondition:
     Only can be called once applocation receivces the callback event BLE_GAP_EVT_DISCONNECTED.
  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleDsadvRestart(void);

/*******************************************************************************
  Function:
    uint16_t APP_BleDsadvCompl(void);

  Summary:
     Handle DSADV complete event.

  Description:

  Precondition:

  Parameters:
    None.

  Returns:
    None.

*/
void APP_BleDsadvCompl(void);


#endif /* _APP_BLE_DSADV_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

