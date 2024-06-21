/*******************************************************************************
  MAC Sync Source

  File Name:
    mac_sync.c
  Summary:
    Implements the MLME-SYNC.request
  Description:
    This file implements the MLME-SYNC.request.

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

/* === Macros =============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

#if (MAC_SYNC_LOSS_INDICATION == 1)

/**
 * @brief Function to initiate MLME-SYNC-LOSS.indication to NHLE.
 *
 * @param lossReason MAC_REALIGNMENT if sync loss is due to receiving
 * coordinator realignment command and MAC_BEACON_LOSS if beacon was
 * lost following a sync request.
 */
void MAC_SyncLoss(uint8_t lossReason)
{
	/*
	 * Static buffer used to give sync loss indication. This buffer is used
	 * in two
	 * instances
	 * 1) when the device looses sync with the parents beacons
	 * 2) when the device receives a coordinator realignment command from
	 * his
	 *    parent
	 * The buffer pointer is stored into the begin of the same static
	 * buffer.
	 */
	static uint8_t MAC_SyncLossBuffer[sizeof(buffer_t) +
	sizeof(MLME_SyncLossInd_t)];

	MLME_SyncLossInd_t *syncLossInd;
	buffer_t *msg_ptr;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
    PibValue_t pib_value;

	/* Update static buffer allocated for sync loss indication. */
	msg_ptr = (buffer_t *)((void *)MAC_SyncLossBuffer);
	msg_ptr->body = &MAC_SyncLossBuffer[sizeof(buffer_t)]; /* begin of
	                                                          * message */
	syncLossInd = (MLME_SyncLossInd_t *)((void *)((msg_ptr->body)));

	syncLossInd->cmdcode = MLME_SYNC_LOSS_INDICATION;
	syncLossInd->LossReason = lossReason;

#if (MAC_SCAN_SUPPORT == 1)
	if (MAC_SCAN_IDLE != macScanState) {
#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
		(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
		syncLossInd->PANId = macScanOrigPanid;
#else
        pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
		syncLossInd->PANId = pib_value.pib_value_16bit;
#endif  /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */
		syncLossInd->LogicalChannel = macScanOrigChannel;
		syncLossInd->ChannelPage = macScanOrigPage;
	} else
#endif  /* (MAC_SCAN_SUPPORT == 1) */
	{
        pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
		syncLossInd->PANId = pib_value.pib_value_16bit;
        pibStatus = PHY_PibGet(phyCurrentChannel, (uint8_t *)&pib_value);
		syncLossInd->LogicalChannel = pib_value.pib_value_8bit;
        pibStatus = PHY_PibGet(phyCurrentPage, (uint8_t *)&pib_value);
		syncLossInd->ChannelPage = pib_value.pib_value_8bit;
	}
    (void)pibStatus;

	/* Append the associate confirm message to MAC-NHLE queue. */
	qmmStatus = qmm_queue_append(&macNhleQueue, msg_ptr);

	/* A device that is neither scanning nor polling shall go to sleep now.
	**/
	if ((MAC_IDLE == macState) || (MAC_ASSOCIATED == macState)) {
		if ((MAC_SCAN_IDLE == macScanState) &&
				(MAC_POLL_IDLE == macPollState)) {
			/* Set radio to sleep if allowed */
			MAC_SleepTrans();
		}
	}

	macSyncState = MAC_SYNC_NEVER;
    WPAN_PostTask();
    (void)qmmStatus;
}

/**
 * @brief Processing a coordinator realignment command frame
 *
 * This function processes a coordinator realignment command frame received
 * from the coordinator (while NOT being in the middle of an Orphan scan, but
 * rather after initiation of a start request primitive from the coordinator
 * indicating realingment.
 * The PAN ID, coord. short address, logical channel, and the device's new
 * short address will be written to the PIB.
 *
 * @param ind Frame reception buffer
 */
void MAC_ProcessCoordRealign(buffer_t *ind)
{
    PHY_Retval_t pibSetStatus = PHY_FAILURE;
	/*
	 * The coordinator realignment command is received without the orphan
	 * notification. Hence a sync loss indication is given to NHLE.
	 */
	MAC_SyncLoss((uint8_t)MAC_REALIGNMENT);

	/*
	 * The buffer in which the coordinator realignment is received is
	 * freed up
	 */
	bmm_buffer_free((buffer_t *)ind);

	/* Set the appropriate PIB entries */

	pibSetStatus = SetPhyPibInternal(macPANId,
			(void *)&macParseData.macPayloadData.coordRealignData.panId);

	if (BROADCAST !=
			macParseData.macPayloadData.coordRealignData.
			shortAddr) {
		/* Short address only to be set if not broadcast address */
		pibSetStatus = SetPhyPibInternal(macShortAddress,
				(void *)&macParseData.macPayloadData.coordRealignData.shortAddr);
	}

	macPib.mac_CoordShortAddress
		= macParseData.macPayloadData.coordRealignData.
			coordShortAddr;
	/*
	 * If frame version subfield indicates a 802.15.4-2006 compatible frame,
	 * the channel page is appended as additional information element.
	 */
	if (((macParseData.fcf & FCF_FRAME_VERSION_2006)) == FCF_FRAME_VERSION_2006)  {

		pibSetStatus = SetPhyPibInternal(phyCurrentPage,
				(void *)&macParseData.macPayloadData.coordRealignData.channelPage);
	}

	pibSetStatus = SetPhyPibInternal(phyCurrentChannel,
			(void *)&macParseData.macPayloadData.coordRealignData.logicalChannel);
    (void)pibSetStatus;

} /* MAC_ProcessCoordRealign() */

#endif /* MAC_SYNC_LOSS_INDICATION */

/* EOF */
