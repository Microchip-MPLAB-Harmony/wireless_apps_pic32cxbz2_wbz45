/*******************************************************************************
  MAC Process Beacon Frame Source

  File Name:
    mac_process_beacon_frame.c
  Summary:
    Processing of a received beacon frame
  Description:
    This file implements the processing of a received beacon frame.
    This happens either after scan, where the PAN Descriptor list is filled, or 
    while the device is tracking beacon frames. In the latter case the device
    checks whether the coordinator does have pending data and initiates the
    transmission of a data request frame if required.

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
#include <string.h>
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

#if (MAC_SCAN_SUPPORT == 1)

/* === Macros =============================================================== */

/*
 * Extract the beacon order from the Superframe Spec.
 */
#define GET_BEACON_ORDER(spec)          ((spec) & 0x000F)

/*
 * Extract the superframe order from the Superframe Spec.
 */
#define GET_SUPERFRAME_ORDER(spec)      (((spec) & 0x00F0) >> 4)

/*
 * Extract the final CAP slot from Superframe Spec.
 */
#define GET_FINAL_CAP(spec)             (((spec) & 0x0F00) >> 8)

/*
 * Extract the GTS permit from GTS Spec.
 */
#define GET_GTS_PERMIT(spec)            (((spec) & 0x80) >> 7)
/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

/**
 * @brief Processes received beacon frame
 *
 * This function processes a received beacon frame.
 * When the system is scanning it records PAN descriptor information
 * contained in the beacon. These PAN descriptors will be reported to the
 * next higher layer via MLME_SCAN.confirm.
 * Also this routine constructs the MLME_BEACON_NOTIFY.indication.
 * Additionally when a device is synced with the coordinator, it tracks beacon
 * frames, checks whether the coordinator does have pending data and will
 * initiate the transmission of a data request frame.
 * The routine uses global "parse_data" structure.
 * The PAN descriptors are stored in the MLME_ScanConf_t structure.
 *
 * @param beacon Pointer to the buffer in which the beacon was received
 *
 */
void MAC_ProcessBeaconFrame(buffer_t *beacon)
{
	bool matchflag;
	WPAN_Pandescriptor_t *pand_long_start_p = NULL;
	WPAN_Pandescriptor_t pandLong;
	MLME_ScanConf_t *msc = NULL;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    PibValue_t pib_channel;
    PibValue_t pib_channelpage;
    
#if (MAC_BEACON_NOTIFY_INDICATION == 1)
	uint8_t numaddrshort;
	uint8_t numaddrlong;
#endif

	/*
	 * The following section needs to be done when we are
	 * either scanning (and look for a new PAN descriptor to be returned
	 * as part of the scan confirm message),
	 * or we need to create a beacon notification (in which case we are
	 * interested in any beacon, but omit the generation of scan confirm).
	 */
	{
		uint8_t index;

		/*
		 * If we are scanning a scan confirm needs to be created.
		 *
		 * According to 802.15.4-2006 this is only done in case the PIB
		 * attribute macAutoRequest is true. Otherwise the PAN
		 * descriptor will
		 * NOT be put into the PAN descriptor list of the Scan confirm
		 * message.
		 */
		if (
			((MAC_SCAN_ACTIVE == macScanState) ||
			(MAC_SCAN_PASSIVE == macScanState)) &&
			(macPib.mac_AutoRequest == 1U)
			) {
			/*
			 * mac_conf_buf_ptr points to the buffer allocated for
			 * scan
			 * confirmation.
			 */
			msc =  (MLME_ScanConf_t *)MAC_BUFFER_POINTER(
					((buffer_t *)((void *)macConfBufPtr)));

			/*
			 * The PAN descriptor list is updated with the
			 * PANDescriptor of the
			 * received beacon
			 */
			pand_long_start_p
				= (WPAN_Pandescriptor_t *)((void *)(&msc->scanResultList));
		}

		/*
		 * The beacon data received from the parse variable is arranged
		 * into a PAN descriptor structure style
		 */
		pandLong.CoordAddrSpec.AddrMode = macParseData.srcAddrMode;
		pandLong.CoordAddrSpec.PANId = macParseData.srcPanid;

		if (FCF_SHORT_ADDR == pandLong.CoordAddrSpec.AddrMode) {
			/* Initially clear the complete address. */
			pandLong.CoordAddrSpec.Addr.longAddress = 0;

			ADDR_COPY_DST_SRC_16(
					pandLong.CoordAddrSpec.Addr.shortAddress,
					macParseData.srcAddr.shortAddress);
		} else {
			ADDR_COPY_DST_SRC_64(
					pandLong.CoordAddrSpec.Addr.longAddress,
					macParseData.srcAddr.longAddress);
		}

		pibStatus = PHY_PibGet(phyCurrentChannel, (uint8_t *)&pib_channel);
        pandLong.LogicalChannel = pib_channel.pib_value_8bit;
        pibStatus = PHY_PibGet(phyCurrentPage, (uint8_t *)&pib_channelpage);
		pandLong.ChannelPage    = pib_channelpage.pib_value_8bit;
		pandLong.SuperframeSpec
			= macParseData.macPayloadData.beaconData.
				superframeSpec;
		pandLong.GTSPermit
			= (bool)(macParseData.macPayloadData.beaconData.gtsSpec >> 7U);
		pandLong.LinkQuality    = macParseData.ppduLinkQuality;
#ifdef ENABLE_TSTAMP
		pandLong.TimeStamp      = macParseData.timeStamp;
#endif  /* ENABLE_TSTAMP */

		/*
		 * If we are scanning we need to check whether this is a new
		 * PAN descriptor.
		 *
		 * According to 802.15.4-2006 this is only done in case the PIB
		 * attribute macAutoRequest is true. Otherwise the PAN
		 * descriptor will
		 * NOT be put into the PAN descriptor list of the Scan confirm
		 * message.
		 */
		if (
			((MAC_SCAN_ACTIVE == macScanState) ||
			(MAC_SCAN_PASSIVE == macScanState)) &&
			(macPib.mac_AutoRequest == 1U)
			) {
			/*
			 * This flag is used to indicate a match of the current
			 *(received) PAN
			 * descriptor with one of those present already in the
			 * list.
			 */
			matchflag = false;

			/*
			 * The beacon frame PAN descriptor is compared with the
			 * PAN descriptors
			 * present in the list and determine if the current PAN
			 * descriptor is to be taken as a valid one. A PAN is
			 * considered to be
			 * the same as an existing one, if all, the PAN Id, the
			 * coordinator address
			 * mode, the coordinator address, and the Logical
			 * Channel are same.
			 */
			for (index = 0; index < msc->ResultListSize; index++) {
				if ((pandLong.CoordAddrSpec.PANId ==
						pand_long_start_p->CoordAddrSpec
						.PANId) &&
						(pandLong.CoordAddrSpec.
						AddrMode ==
						pand_long_start_p->CoordAddrSpec
						.AddrMode) &&
						(pandLong.LogicalChannel ==
						pand_long_start_p->
						LogicalChannel) &&
						(pandLong.ChannelPage ==
						pand_long_start_p->ChannelPage)
						) {
					if (pandLong.CoordAddrSpec.AddrMode ==
							WPAN_ADDRMODE_SHORT) {
						if (pandLong.CoordAddrSpec.Addr
								.shortAddress
								==
								pand_long_start_p
								->CoordAddrSpec.
								Addr.
								shortAddress) {
							/* Beacon with same
							 * parameters already
							 * received */
							matchflag = true;
							break;
						}
					} else {
						if (pandLong.CoordAddrSpec.Addr
								.longAddress ==
								pand_long_start_p
								->CoordAddrSpec.
								Addr.
								longAddress) {
							/* Beacon with same
							 * parameters already
							 * received */
							matchflag = true;
							break;
						}
					}
				}
                pand_long_start_p++;
			}

			/*
			 * If the PAN descriptor is not in the current list, and
			 * there is space
			 * left, it is put into the list
			 */
			if ((!matchflag) &&
					(msc->ResultListSize <
					MAX_PANDESCRIPTORS)) {
				(void)memcpy(pand_long_start_p, &pandLong, sizeof(pandLong));
				msc->ResultListSize++;
			}
		}
	}
    (void)pibStatus;

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

	/* The short and extended pending addresses are extracted from the
	 * beacon */
	numaddrshort
        = NUM_SHORT_PEND_ADDR(
            macParseData.macPayloadData.beaconData.pendingAddrSpec);

	numaddrlong
		= NUM_LONG_PEND_ADDR(
			macParseData.macPayloadData.beaconData.pendingAddrSpec);
#endif

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

	/*
	 * In all cases (PAN or device) if the payload is not equal to zero
	 * or macAutoRequest is false, MLME_BEACON_NOTIFY.indication is
	 * generated
	 */
	if ((macParseData.macPayloadData.beaconData.beaconPayloadLen >
			0U) ||
			((macPib.mac_AutoRequest) == 0U)
			) {
		MLME_BeaconNotifyInd_t *mbni
			= (MLME_BeaconNotifyInd_t *)MAC_BUFFER_POINTER(((
					buffer_t *)beacon));
        qmm_status_t  qmmStatus = QMM_QUEUE_FULL;

		/* The beacon notify indication structure is built */
		mbni->cmdcode       = MLME_BEACON_NOTIFY_INDICATION;
		mbni->BSN           = macParseData.sequenceNumber;
		mbni->PANDescriptor = pandLong;
		mbni->PendAddrSpec
			= macParseData.macPayloadData.beaconData.
				pendingAddrSpec;

		if ((numaddrshort > 0U) || (numaddrlong > 0U)) {
			mbni->AddrList
				= macParseData.macPayloadData.beaconData
					.pendingAddrList;
		}

		mbni->sduLength
			= macParseData.macPayloadData.beaconData.
				beaconPayloadLen;
		mbni->sdu
			= macParseData.macPayloadData.beaconData.
				beaconPayload;

		/*
		 * The beacon notify indication is given to the NHLE and then
		 * the buffer
		 * is freed up.
		 */
		qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)beacon);
        WPAN_PostTask();
        (void)qmmStatus;
	} else
#endif /* (MAC_BEACON_NOTIFY_INDICATION == 1) */
	{
		/* Payload is not present, hence the buffer is freed here */
		bmm_buffer_free(beacon);
	}
} /* MAC_ProcessBeaconFrame() */

#endif /* (MAC_SCAN_SUPPORT == 1) */

/* EOF */