/*******************************************************************************
  APP Config Header

  File Name:
    app_config.h

  Summary:
    This file contains the application specific definitions for 
    Resource management

  Description:
    These are application-specific resources which are used in the example 
    application of the coordinator in addition to the underlaying stack.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

/* Prevent double inclusion */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H



#include "stack_config.h"
// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

/** Defines the number of timers used by the application. */
#ifdef FFD
#define NUMBER_OF_APP_TIMERS        (4U)
#else
#define NUMBER_OF_APP_TIMERS        (2U)
#endif

#define NUMBER_OF_TOTAL_STACK_TIMERS (1U)

#if (!defined TOTAL_NUMBER_OF_TIMERS)

/** Defines the total number of timers used by the application and the layers
 * below. */
#define TOTAL_NUMBER_OF_TIMERS      (NUMBER_OF_APP_TIMERS + NUMBER_OF_TOTAL_PHY_TIMERS+ \
	NUMBER_OF_MAC_TIMERS)
#endif /* (!define TOTAL_NUMBER_OF_TIMERS) */

#define NUMBER_OF_LARGE_STACK_BUFS	(5U+NUMBER_OF_LARGE_MAC_BUFS)

#define NUMBER_OF_SMALL_STACK_BUFS	(3U+NUMBER_OF_SMALL_MAC_BUFS)

/** Defines the number of additional large buffers used by the application */
#define NUMBER_OF_LARGE_APP_BUFS    (4U)

/** Defines the number of additional small buffers used by the application */
#define NUMBER_OF_SMALL_APP_BUFS    (0U)

/**
 *  Defines the total number of large buffers used by the application and the
 *  layers below.
 */
#define TOTAL_NUMBER_OF_LARGE_BUFS  (NUMBER_OF_LARGE_APP_BUFS +	\
	NUMBER_OF_LARGE_STACK_BUFS)

/**
 *  Defines the total number of small buffers used by the application and the
 *  layers below.
 */
#define TOTAL_NUMBER_OF_SMALL_BUFS  (NUMBER_OF_SMALL_APP_BUFS +	\
	NUMBER_OF_SMALL_STACK_BUFS)

#define TOTAL_NUMBER_OF_BUFS        (TOTAL_NUMBER_OF_LARGE_BUFS + \
	TOTAL_NUMBER_OF_SMALL_BUFS)

#ifdef MAC_SECURITY_ZIP

/**
 * The maximum number of entries supported in the macKeyTable.
 * This value is implementation specific.
 */
#define MAC_ZIP_MAX_KEY_TABLE_ENTRIES           (4U)

/**
 * The maximum number of entries supported in the macDeviceTable.
 * This value is implementation specific.
 */
#if (MAC_START_REQUEST_CONFIRM == 1)    /* FFD like device */
#define MAC_ZIP_MAX_DEV_TABLE_ENTRIES           (10U)
#else
#define MAC_ZIP_MAX_DEV_TABLE_ENTRIES           (4U)
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

/**
 * The maximum number of entries supported in the macSecurityLevelTable.
 * This value is implementation specific.
 */
#define MAC_ZIP_MAX_SEC_LVL_TABLE_ENTRIES       (2U)

/**
 * The maximum number of entries supported in the KeyIdLookupList
 */
#define MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES  (1U)

/**
 * The maximum number of entries supported in the KeyDeviceList
 */
#define MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES        (4U)

/**
 * The maximum number of entries supported in the KeyUsageList
 */
#define MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES      (1U)
#endif /* MAC_SECURITY_ZIP */

#endif /* APP_CONFIG_H */
/* EOF */
