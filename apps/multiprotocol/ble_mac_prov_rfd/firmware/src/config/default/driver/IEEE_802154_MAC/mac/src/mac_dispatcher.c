/*******************************************************************************
  MAC Dispatcher Source

  File Name:
    mac_dispatcher.c
  Summary:
    dispatches the events by decoding the message type
  Description:
    This file dispatches the events by decoding the message type.

 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

/* === Includes ============================================================ */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "pal.h"
#include "bmm.h"
#include "qmm.h"
#include "phy.h"
#include "ieee_mac_const.h"
#include "mac_msg_const.h"
#include "mac_api.h"
#include "mac_msg_types.h"
#include "mac_data_structures.h"
#include "stack_config.h"
#include "mac_internal.h"
#include "mac.h"
#include "mac_config.h"
/* === Macros ============================================================== */
#define CMD_ID_OCTET     (0U)
/* === Globals ============================================================= */
/* Regular MAC Dispatcher table */
static const Handler_t dispatch_Table[LAST_MESSAGE + 1] = {
	/* Internal message */
	[MLME_RESET_REQUEST]                  = MAC_MLME_ResetRequest,

#if (MAC_GET_SUPPORT == 1)
	[MLME_GET_REQUEST]                    = MAC_MLME_GetRequest,
#endif  /* (MAC_GET_SUPPORT == 1) */

	[MLME_SET_REQUEST]                    = MAC_MLME_SetRequest,

#if (MAC_SCAN_SUPPORT == 1)
	[MLME_SCAN_REQUEST]                   = MAC_MLME_ScanRequest,
#endif /* (MAC_SCAN_SUPPORT == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	[MLME_START_REQUEST]                  = MAC_MLME_StartRequest,
#endif /* (MAC_START_REQUEST_CONFIRM== 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	[MLME_ASSOCIATE_REQUEST]              = MAC_MLME_AssociateRequest,
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	[MLME_ASSOCIATE_RESPONSE]             = MAC_MLME_AssociateResponse,
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE== 1) */

	[MCPS_DATA_REQUEST]                   = MAC_MCPS_DataRequest,

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
	[MLME_DISASSOCIATE_REQUEST]           = MAC_MLME_DisassociateRequest,
#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
	[MLME_ORPHAN_RESPONSE]                = MAC_MLME_OrphanResponse,
#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
	[MLME_POLL_REQUEST]                   = MAC_MLME_PollRequest,
#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_RX_ENABLE_SUPPORT == 1)
	[MLME_RX_ENABLE_REQUEST]              = MAC_MLME_RxEnableRequest,
#endif /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
	[MCPS_PURGE_REQUEST]                  = MAC_MCPS_PurgeRequest,
#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
	**/

	[PHY_DATA_INDICATION]                 = MAC_ProcessPhyDataInd,
	[MCPS_DATA_CONFIRM]                   = MAC_MCPS_DataConf,
	[MCPS_DATA_INDICATION]                = MAC_MCPS_DataInd,

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
	[MCPS_PURGE_CONFIRM]                  = MAC_MCPS_PurgeConf,
#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
	**/

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	[MLME_ASSOCIATE_INDICATION]           = MAC_MLME_AssociateInd,
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	[MLME_ASSOCIATE_CONFIRM]              = MAC_MLME_AssociateConf,
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
	[MLME_DISASSOCIATE_INDICATION]        = MAC_MLME_DisassociateInd,
	[MLME_DISASSOCIATE_CONFIRM]           = MAC_MLME_DisassociateConf,
#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)
	[MLME_BEACON_NOTIFY_INDICATION]       = MAC_MLME_BeaconNotifyInd,
#endif /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
	[MLME_ORPHAN_INDICATION]              = MAC_MLME_OrphanInd,
#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_SCAN_SUPPORT == 1)
	[MLME_SCAN_CONFIRM]                   = MAC_MLME_ScanConf,
#endif /* (MAC_SCAN_SUPPORT == 1) */

#if (MAC_COMM_STATUS_INDICATION == 1)
	[MLME_COMM_STATUS_INDICATION]         = MAC_MLME_CommStatusInd,
#endif /* (MAC_COMM_STATUS_INDICATION == 1) */

#if MAC_SYNC_LOSS_INDICATION == 1
	[MLME_SYNC_LOSS_INDICATION]           = MAC_MLME_SyncLossInd,
#endif /* (MAC_SYNC_LOSS_INDICATION == 1) */

#if (MAC_GET_SUPPORT == 1)
	[MLME_GET_CONFIRM]                    = MAC_MLME_GetConf,
#endif  /* (MAC_GET_SUPPORT == 1) */

	[MLME_SET_CONFIRM]                    = MAC_MLME_SetConf,
	[MLME_RESET_CONFIRM]                  = MAC_MLME_ResetConf,

#if (MAC_RX_ENABLE_SUPPORT == 1)
	[MLME_RX_ENABLE_CONFIRM]              = MAC_MLME_RxEnableConf,
#endif /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	[MLME_START_CONFIRM]                  = MAC_MLME_StartConf,
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
	[MLME_POLL_CONFIRM]                   = MAC_MLME_PollConf,
#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */
};

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

/**
 * @brief Obtains the message type from the buffer and calls the respective
 * handler
 *
 * This function decodes all events/messages and calls the appropriate handler.
 *
 * @param event Pointer to the buffer header whose body part holds the message
 * type and message elemnets
 */
void DispatchEvent(void *event)
{
	/*
	 * A pointer to the body of the buffer is obtained from the pointer to
	 * the
	 * received header.
	 */
	uint8_t *bufferBody = (uint8_t *)MAC_BUFFER_POINTER((buffer_t *)event);

	/* Check is done to see if the message type is valid */

	/* Please note:
	 * The macro PGM_READ_WORD is only relevant for AVR-GCC builds and
	 * reads a DWord from flash, which is the start address of the function.
	 *
	 * How does this work for builds that are larger than 128K?
	 *
	 * For IAR builds this statement is fine, since PGM_READ_WORD just
	 * turns to "*". The size of the function pointer is automatically
	 * 3 byte for MCUs which have more than 128K flash. The the address
	 * of the callback is properly derived from this location.
	 *
	 * AVR-GCC currently does not support function pointers larger than
	 * 16 bit. This implies that functions residing in the upper 128K
	 * cannot be addressed properly. Therefore this code does not work
	 * in these cases.
	 * In regular cases, where code is not larger than 128K, the size
	 * of a function pointer is 16 bit and properly read via PGM_READ_WORD.
	 */
    
	/* Check for regular MAC requests. */
	if (bufferBody[CMD_ID_OCTET] <= (uint8_t)(LAST_MESSAGE)) {
		/*
		 * The following statement reads the address from the dispatch
		 * table
		 * of the function to be called by utilizing function pointers.
		 */
		Handler_t handler
			= (Handler_t)PGM_READ_WORD(&dispatch_Table[bufferBody
				[CMD_ID_OCTET]]);

		if (handler != NULL) {
			handler(event);
		} else {
			bmm_buffer_free((buffer_t *)event);
		}
	}
}

/* EOF */
