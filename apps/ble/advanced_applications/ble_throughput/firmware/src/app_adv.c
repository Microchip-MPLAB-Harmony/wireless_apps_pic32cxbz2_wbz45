/*******************************************************************************
  Application Advertising Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_adv.c

  Summary:
    This file contains the Application advertising functions for this project.

  Description:
    This file contains the Application advertising functions for this project.
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
#include "app_ble/app_ble_handler.h"
#include "app_adv.h"
#include "app_error_defs.h"
#include "app_led.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************




// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
static APP_BLE_AdvParams_T s_bleAdvParam;
static BLE_GAP_AdvDataParams_T s_bleAdvData;
static BLE_GAP_AdvDataParams_T s_bleScanRspData;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

static void APP_ADV_InitAdvParams(void) {
    s_bleAdvParam.intervalMin = APP_ADV_DEFAULT_INTERVAL;
    s_bleAdvParam.intervalMax = APP_ADV_DEFAULT_INTERVAL;
    s_bleAdvParam.advType = BLE_GAP_ADV_TYPE_ADV_IND;
    s_bleAdvParam.filterPolicy = BLE_GAP_ADV_FILTER_DEFAULT;
}

static uint8_t APP_ADV_CalculateDataLength(uint8_t *advData) {
    /* Caculate Total Length of Adv Data /Scan Response Data Elements  */
    uint8_t len = 0, i = 0;

    while (1) {
        if (advData[i] != 0x00) // Check the length is Zero or not
        {
            len++; // Add Length field size
            len += advData[i]; // Add this Element Data Size

            if (len >= BLE_GAP_ADV_MAX_LENGTH) {
                len = BLE_GAP_ADV_MAX_LENGTH;
                break;
            } else {
                i = len;
            }
        } else {
            break;
        }
    }
    return len;
}

uint8_t APP_ADV_GetAdvertisingType(void) {
    return s_bleAdvParam.advType;
}

uint16_t APP_ADV_SetAdvertisingType(uint8_t advType) {
    if (advType > BLE_GAP_ADV_TYPE_ADV_DIRECT_LOW_IND) {
        return APP_RES_INVALID_PARA;
    }

    s_bleAdvParam.advType = advType;

    return APP_RES_SUCCESS;
}

uint16_t APP_ADV_SetAdvertisingParams(void) {
    BLE_GAP_AdvParams_T advParams;

    memset((uint8_t *) & advParams, 0, sizeof (BLE_GAP_AdvParams_T));

    advParams.intervalMin = s_bleAdvParam.intervalMin;
    advParams.intervalMax = s_bleAdvParam.intervalMax;
    advParams.type = s_bleAdvParam.advType;
    advParams.peerAddr.addrType = BLE_GAP_ADDR_TYPE_PUBLIC;

    //advParams.peerAddr.addr = ;   //Peer address (For directed adverising used)

    advParams.advChannelMap = BLE_GAP_ADV_CHANNEL_ALL;
    advParams.filterPolicy = s_bleAdvParam.filterPolicy;

    return BLE_GAP_SetAdvParams(&advParams);
}

void APP_ADV_UpdateAdvData(void) {
    uint8_t idx = 0;

    memset(&s_bleAdvData.advData[0], 0x00, BLE_GAP_ADV_MAX_LENGTH);

    //Flags
    s_bleAdvData.advData[idx++] = (APP_ADV_TYPE_LEN + 1); //length
    s_bleAdvData.advData[idx++] = APP_ADV_TYPE_FLAGS; //AD Type: flags
    s_bleAdvData.advData[idx++] = APP_ADV_FLAG_LE_GEN_DISCOV | APP_ADV_FLAG_BREDR_NOT_SUPPORTED;

    //Service Data
    s_bleAdvData.advData[idx++] = (APP_ADV_TYPE_LEN + APP_ADV_SRV_DATA_LEN); //length
    s_bleAdvData.advData[idx++] = APP_ADV_TYPE_SRV_DATA_16BIT_UUID; //AD Type: Service Data
    s_bleAdvData.advData[idx++] = (uint8_t) APP_ADV_SERVICE_UUID_MCHP;
    s_bleAdvData.advData[idx++] = (uint8_t) (APP_ADV_SERVICE_UUID_MCHP >> 8);
    s_bleAdvData.advData[idx++] = APP_ADV_ADD_DATA_CLASS_BYTE;
    s_bleAdvData.advData[idx++] = APP_ADV_PROD_TYPE_BLE_UART;

    s_bleAdvData.advLen = APP_ADV_CalculateDataLength(&s_bleAdvData.advData[0]);
}

void APP_ADV_UpdateScanRspData(void) {
    uint8_t devNameLen;

    memset(&s_bleScanRspData.advData[0], 0x00, BLE_GAP_ADV_MAX_LENGTH);

    BLE_GAP_GetDeviceName(&devNameLen, &s_bleScanRspData.advData[2]);

    //Local Name
    s_bleScanRspData.advData[0] = (APP_ADV_TYPE_LEN + devNameLen); //length
    s_bleScanRspData.advData[1] = APP_ADV_TYPE_COMPLETE_LOCAL_NAME;

    s_bleScanRspData.advLen = APP_ADV_CalculateDataLength(&s_bleScanRspData.advData[0]);
}

void APP_ADV_SetFilterPolicy(uint8_t filterPolicy) {
    s_bleAdvParam.filterPolicy = filterPolicy;
}

uint8_t APP_ADV_GetFilterPolicy(void) {
    return s_bleAdvParam.filterPolicy;
}

uint16_t APP_ADV_Ctrl(uint8_t enable) {
    uint16_t result = APP_RES_BAD_STATE;

    if (enable) {
        if (APP_GetBleState() == APP_BLE_STATE_STANDBY) {
            result = BLE_GAP_SetAdvEnable(true, 0);

            if (result == APP_RES_SUCCESS) {
                APP_SetBleState(APP_BLE_STATE_ADVERTISING);
                g_appLedGreenHandler = APP_LED_StartByMode(APP_LED_MODE_ADV);
            }
        }
    }
    else {
        if (APP_GetBleState() == APP_BLE_STATE_ADVERTISING) {
            result = BLE_GAP_SetAdvEnable(false, 0);

            if (result == APP_RES_SUCCESS) {
                APP_SetBleState(APP_BLE_STATE_STANDBY);
            }
        }
    }

    return result;
}

void APP_ADV_Start(void) {
    APP_ADV_SetAdvertisingParams();
    APP_ADV_UpdateAdvData();
    APP_ADV_UpdateScanRspData();

    BLE_GAP_SetAdvData(&s_bleAdvData);
    BLE_GAP_SetScanRspData(&s_bleScanRspData);

    APP_ADV_Ctrl(true);
}

void APP_ADV_Stop(void) {
    APP_ADV_Ctrl(false);
}

void APP_ADV_Init(void) {
    APP_ADV_InitAdvParams();
    APP_ADV_Start();
}
