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
  BLE Alert Notification Source File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_ans.c

  Summary:
    This file contains the BLE Alert Notification Service functions for application user.

  Description:
    This file contains the BLE Alert Notification Service functions for application user.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include <stdint.h>
#include "mba_error_defs.h"
#include "gatt.h"
#include "ble_util/byte_stream.h"
#include "ble_ans.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

#define BLE_ANS_CCC_NUM                     0x02
#define BLE_ANS_UUID_ANS_SVC                0x1811
#define BLE_ANS_UUID_ANCP                   0x2A44 /**< Alert Notification Control Point.*/
#define BLE_ANS_UUID_UNREAD_ALERT_STAT      0x2A45 /**< Unread Alert Status Declaration.*/
#define BLE_ANS_UUID_NEW_ALERT              0x2A46 /**< New Alert Declaration.*/
#define BLE_ANS_UUID_SUPP_NEW_ALERT_CAT     0x2A47 /**< Supported New Alert Category Declaration.*/
#define BLE_ANS_UUID_SUPP_UNREAD_ALERT_CAT  0x2A48 /**< Support Unread Alert Category Declaration.*/

// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************
/* Service Declaration */
static const uint8_t s_svcUuidAns[] = {UINT16_TO_BYTES(BLE_ANS_UUID_ANS_SVC)};
static const uint16_t s_svcUuidAnsLen = (uint16_t)sizeof(s_svcUuidAns);

/* Supported New Alert Category Characteristic Declaration */
static const uint8_t s_charAnsSuppNewAlertCat[] = {ATT_PROP_READ, UINT16_TO_BYTES(ANS_HDL_CHARVAL_SUPP_NEW_ALERT_CAT), UINT16_TO_BYTES(BLE_ANS_UUID_SUPP_NEW_ALERT_CAT)};
static const uint16_t s_charAnsSuppNewAlertCatLen = (uint16_t)sizeof(s_charAnsSuppNewAlertCat);

/* Supported New Alert Category Characteristic Value */
static const uint8_t s_chUuidAnsSuppNewAlertCat[] = {UINT16_TO_BYTES(BLE_ANS_UUID_SUPP_NEW_ALERT_CAT)};
static uint8_t s_ansSuppNewAlertCatVal[] = {0x00,0x00};
static uint16_t s_ansSuppNewAlertCatValLen =(uint16_t) sizeof(s_ansSuppNewAlertCatVal);

/* New Alert  Characteristic Declaration */
static const uint8_t s_charAnsNewAlert[] = {ATT_PROP_NOTIFY, UINT16_TO_BYTES(ANS_HDL_CHARVAL_NEW_ALERT), UINT16_TO_BYTES(BLE_ANS_UUID_NEW_ALERT)};
static const uint16_t s_charAnsNewAlertLen = (uint16_t)sizeof(s_charAnsNewAlert);

/* New Alert Characteristic Value */
static const uint8_t s_chUuidAnsNewAlert[] = {UINT16_TO_BYTES(BLE_ANS_UUID_NEW_ALERT)};
static uint8_t s_ansNewAlertVal[] = {0,0,0,0,0,0,0,0,0,0};
static uint16_t s_ansNewAlertValLen = (uint16_t)sizeof(s_ansNewAlertVal);

/* New Alert Client Characteristic Configuration Descriptor */
static uint8_t s_descCccAnsNewAlertCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccAnsNewAlertCccLen =(uint16_t)sizeof(s_descCccAnsNewAlertCcc);


/* Supported Unread Alert Category Characteristic Declaration */
static const uint8_t s_charAnsSuppUnreadAlertCat[] = {ATT_PROP_READ, UINT16_TO_BYTES(ANS_HDL_CHARVAL_SUPP_UNREAD_ALERT_CAT), UINT16_TO_BYTES(BLE_ANS_UUID_SUPP_UNREAD_ALERT_CAT)};
static const uint16_t s_charAnsSuppUnreadAlertCatLen = (uint16_t)sizeof(s_charAnsSuppUnreadAlertCat);

/* Supported Unread Alert Category Characteristic Value */
static const uint8_t s_chUuidAnsSuppUnreadAlertCat[] = {UINT16_TO_BYTES(BLE_ANS_UUID_SUPP_UNREAD_ALERT_CAT)};
static uint8_t s_ansSuppUnreadAlertCatVal[] = {0x00,0x00};
static uint16_t s_ansSuppUnreadAlertCatValLen = (uint16_t)sizeof(s_ansSuppUnreadAlertCatVal);

/* Unread Alert Status Characteristic Declaration */
static const uint8_t s_charAnsUnreadAlertStat[] = {ATT_PROP_NOTIFY, UINT16_TO_BYTES(ANS_HDL_CHARVAL_UNREAD_ALERT_STAT), UINT16_TO_BYTES(BLE_ANS_UUID_UNREAD_ALERT_STAT)};
static const uint16_t s_charAnsUnreadAlertStatLen =(uint16_t)sizeof(s_charAnsUnreadAlertStat);

/* Unread Alert Status Characteristic Value */
static const uint8_t s_chUuidAnsUnreadAlertStat[] = {UINT16_TO_BYTES(BLE_ANS_UUID_UNREAD_ALERT_STAT)};
static uint8_t s_ansUnreadAlertStatVal[] = {0,0,0,0,0,0,0,0,0,0};
static uint16_t s_ansUnreadAlertStatValLen = (uint16_t)sizeof(s_ansUnreadAlertStatVal);

/* Unread Alert Status Client Characteristic Configuration Descriptor */
static uint8_t s_descCccAnsUnreadAlertStatCcc[] = {UINT16_TO_BYTES(0x0000)};
static const uint16_t s_descCccAnsUnreadAlertStatCccLen = (uint16_t)sizeof(s_descCccAnsUnreadAlertStatCcc);


/* Alert Notification Control Point Characteristic Declaration */
static const uint8_t s_charAnsAncp[] = {ATT_PROP_WRITE_REQ, UINT16_TO_BYTES(ANS_HDL_CHARVAL_ANCP), UINT16_TO_BYTES(BLE_ANS_UUID_ANCP)};
static const uint16_t s_charAnsAncpLen = (uint16_t)sizeof(s_charAnsAncp);

/* Alert Notification Control Point Characteristic Value */
static const uint8_t s_descUuidAnsAncp[] = {UINT16_TO_BYTES(BLE_ANS_UUID_ANCP)};
static uint8_t s_ansAncpVal[] = {0,0}; /** <Command ID; Alert Category ID. */
static uint16_t s_ansAncpValLen = (uint16_t)sizeof(s_ansAncpVal);

/* Attributes list for Alert Notification service */
static GATTS_Attribute_T s_ansList[] =
{
    /* Service Delcaration */
    {
        (uint8_t *) g_gattUuidPrimSvc,
        (uint8_t *) s_svcUuidAns,
        (uint16_t *) &s_svcUuidAnsLen,
        (uint16_t)sizeof(s_svcUuidAns),
        0,
        (PERMISSION_READ)
    },
    /* Characteristic Delcaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_charAnsSuppNewAlertCat,
        (uint16_t *) &s_charAnsSuppNewAlertCatLen,
        (uint16_t)sizeof(s_charAnsSuppNewAlertCat),
        0,
        (PERMISSION_READ)
    },
    /* Characteristic Value */
    {
        (uint8_t *) s_chUuidAnsSuppNewAlertCat,
        (uint8_t *) s_ansSuppNewAlertCatVal,
        (uint16_t *) &s_ansSuppNewAlertCatValLen,
        (uint16_t)sizeof(s_ansSuppNewAlertCatVal),
        SETTING_MANUAL_READ_RSP,
        (PERMISSION_READ)
    },
    /* Characteristic Delcaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_charAnsNewAlert,
        (uint16_t *) &s_charAnsNewAlertLen,
        (uint16_t)sizeof(s_charAnsNewAlert),
        0,
        (PERMISSION_READ)
    },
    /* Characteristic Value */
    {
        (uint8_t *) s_chUuidAnsNewAlert,
        (uint8_t *) s_ansNewAlertVal,
        (uint16_t *) &s_ansNewAlertValLen,
        BLE_ATT_MAX_MTU_LEN,
        0,
        0,
    },
    /* Client Characteristic Configuration Descriptor */
    {
        (uint8_t *) g_descUuidCcc,
        (uint8_t *) s_descCccAnsNewAlertCcc,
        (uint16_t *) &s_descCccAnsNewAlertCccLen,
        (uint16_t)sizeof(s_descCccAnsNewAlertCcc),
        (SETTING_CCCD),
        (PERMISSION_READ|PERMISSION_WRITE|PERMISSION_WRITE_ENC)
    },
    /* Characteristic Delcaration */
    {   (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_charAnsSuppUnreadAlertCat,
        (uint16_t *) &s_charAnsSuppUnreadAlertCatLen,
        (uint16_t)sizeof(s_charAnsSuppUnreadAlertCat),
        0,
        (PERMISSION_READ)
    },
    /* Characteristic Value */
    {
        (uint8_t *) s_chUuidAnsSuppUnreadAlertCat,
        (uint8_t *) s_ansSuppUnreadAlertCatVal,
        (uint16_t *) &s_ansSuppUnreadAlertCatValLen,
        BLE_ATT_MAX_MTU_LEN,
        SETTING_MANUAL_READ_RSP,
        (PERMISSION_READ)
    },
    /* Characteristic Delcaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_charAnsUnreadAlertStat,
        (uint16_t *) &s_charAnsUnreadAlertStatLen,
        (uint16_t)sizeof(s_charAnsUnreadAlertStat),
        0,
        (PERMISSION_READ)
    },
    /* Characteristic Value */
    {
        (uint8_t *) s_chUuidAnsUnreadAlertStat,
        (uint8_t *) s_ansUnreadAlertStatVal,
        (uint16_t *) &s_ansUnreadAlertStatValLen,
        BLE_ATT_MAX_MTU_LEN,
        0,
        0
    },
    /* Client Characteristic Configuration Descriptor */
    {
        (uint8_t *) g_descUuidCcc,
        (uint8_t *) s_descCccAnsUnreadAlertStatCcc,
        (uint16_t *) &s_descCccAnsUnreadAlertStatCccLen,
        (uint16_t)sizeof(s_descCccAnsUnreadAlertStatCcc),
        (SETTING_CCCD),
        (PERMISSION_READ|PERMISSION_WRITE|PERMISSION_WRITE_ENC)
    },
    /* Characteristic Delcaration */
    {
        (uint8_t *) g_gattUuidChar,
        (uint8_t *) s_charAnsAncp,
        (uint16_t *) &s_charAnsAncpLen,
        (uint16_t)sizeof(s_charAnsAncp),
        0,
        (PERMISSION_READ)
    },
    /* Characteristic Value */
    {
        (uint8_t *) s_descUuidAnsAncp,
        (uint8_t *) s_ansAncpVal,
        (uint16_t *) &s_ansAncpValLen,
        (uint16_t)sizeof(s_ansAncpVal),
        SETTING_MANUAL_WRITE_RSP,
        (PERMISSION_WRITE)
    },
};

static const GATTS_CccdSetting_T s_ansCccdSetting[] = 
{
    {(uint16_t)ANS_HDL_CHARVAL_NEW_ALERT_CCC, (NOTIFICATION)},
    {(uint16_t)ANS_HDL_CHARVAL_UNREAD_ALERT_STAT_CCC, (NOTIFICATION)}
};

/* Alert Notification Service structure */
static GATTS_Service_T s_svcAns =
{
    NULL,
    (GATTS_Attribute_T *) s_ansList,
    (GATTS_CccdSetting_T const *)s_ansCccdSetting,
    (uint16_t)BLE_ANS_START_HDL,
    (uint16_t)BLE_ANS_END_HDL,
    BLE_ANS_CCC_NUM
};

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

uint16_t BLE_ANS_Add(void)
{
    return GATTS_AddService(&s_svcAns, (uint8_t)((uint16_t)BLE_ANS_END_HDL - (uint16_t)BLE_ANS_START_HDL + 1U));
}
