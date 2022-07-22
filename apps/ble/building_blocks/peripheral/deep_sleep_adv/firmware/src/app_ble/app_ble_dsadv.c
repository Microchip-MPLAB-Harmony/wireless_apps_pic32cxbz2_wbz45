/*******************************************************************************
  Application BLE Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_dsadv.c

  Summary:
    This file contains the Application BLE Deep Sleep Advertising (DSADV) function for this project.

  Description:
    This file contains the Application BLE Deep Sleep Advertising (DSADV) function for this project.
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

#include "app_ble.h"
#include "device_deep_sleep.h"



// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define DSADV_INTERVAL    1536

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

bool APP_BleDsadvIsEnable(void)
{
    DEVICE_DeepSleepWakeSrc_T wakeSrc;

    DEVICE_GetDeepSleepWakeUpSrc(&wakeSrc);
    if (wakeSrc == DEVICE_DEEP_SLEEP_WAKE_NONE)
        return false;
    else
        return true;
}

void APP_BleDsadvStart(bool flag)
{
    if (flag == false)
    {
        //Enable DSADV and request controller to store adv. parameters into backup ram
        BLE_GAP_EnableOneTimeAdv(BLE_GAP_ADV_OPTION_STORE_PARAMS);

        //Request controller to unlock traffic
        BLE_GAP_TrafficUnlock();
    }
    else
    {
        //Step 1. Only initialize required function of DSADV
        APP_BleStackInitBasic();

        //Step 2. Enable DSADV by loading adv parameters from backup ram
        BLE_GAP_EnableOneTimeAdv(BLE_GAP_ADV_OPTION_LOAD_PARAMS);

        //Step 3. Continue to initialize the advanced fucntion
        APP_BleStackInitAdvance();

        //Step 4. Request controller to unlock traffic
        BLE_GAP_TrafficUnlock();
    }
}

void APP_BleDsadvRestart(void)
{
    DEVICE_SetDeepSleepWakeUpSrc(DEVICE_DEEP_SLEEP_WAKE_NONE);   //Clear deep sleep wake up source

    BLE_GAP_EnableOneTimeAdv(BLE_GAP_ADV_OPTION_LOAD_PARAMS);

    //Request controller to unlock traffic
    BLE_GAP_TrafficUnlock();
}

void APP_BleDsadvCompl(void)
{
    DEVICE_EnterDeepSleep(true, DEVICE_DeepSleepIntervalCal((uint32_t)(DSADV_INTERVAL * 625) / 1000));
}