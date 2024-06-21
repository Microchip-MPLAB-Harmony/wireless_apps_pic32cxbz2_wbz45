/*******************************************************************************
  MAC beacon Source

  File Name:
    mac_beacon.c
  Summary:
    Building of beacon frames and initiates transmission
  Description:
    This file implements the building of beacon frames and initiates transmission 
    via CSMA-CA after reception of a beacon request frame in a nonbeacon-enabled PAN.

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

/* === Includes ============================================================= */
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

/* === Macros =============================================================== */

/*
 * (Minimal) Beacon payload length
 * 2 octets Superframe Spec
 * At least 1 octet GTS fields
 * At least 1 octet Pending Address fields
 */
#define BEACON_PAYLOAD_LEN                  (4U)

/*
 * Maximum number of pending extended and/short addresses to be added to
 * Beacon frame indicating pending data.
 */
#define BEACON_MAX_PEND_ADDR_CNT            (7U)

/* === Globals ============================================================== */

#if (MAC_START_REQUEST_CONFIRM == 1)

#if (MAC_INDIRECT_DATA_FFD == 1)
/* Pointer used for adding pending addresses to the beacon frame. */
static uint8_t *beaconPtr;

/* Variable to hold number the pending addresses. */
static uint8_t pendingAddressCount;

#endif  /* (MAC_INDIRECT_DATA_FFD == 1) */

#ifdef TEST_HARNESS
static uint8_t vpan_no;
#endif  /* TEST_HARNESS */

/* === Prototypes =========================================================== */

#if (MAC_INDIRECT_DATA_BASIC == 1)
static uint8_t AddPendingExtendedAddressCb(void *bufPtr, void *handle);
static uint8_t AddPendingShortAddressCb(void *bufPtr, void *handle);
static uint8_t MAC_BufferAddPending(uint8_t *bufPtr);

#endif

/* === Implementation ======================================================= */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Populates the beacon frame with pending addresses
 *
 * This function populates the beacon frame with pending addresses by
 * traversing through the indirect queue.
 *
 * @param buf_ptr Pointer to the location in the beacon frame buffer where the
 * pending addresses are to be updated
 *
 * @return Number of bytes added in the beacon frame as a result of pending
 * address
 */
static uint8_t MAC_BufferAddPending(uint8_t *bufPtr)
{
	search_t findBuf;
    buffer_t *buf;
	uint8_t numberOfExtAddress = 0;

	/*
	 * Set pointer to beacon to proper place. Currently it is still
	 * pointing at the first octet of the beacon payload (if there
	 * is any included.
	 * The beacon_ptr pointer will be updated (i.e. decreased) in
	 * the functions add_pending_short_address_cb() &
	 * add_pending_extended_address_cb() according to the included
	 * octets containing the pending addresses.
	 *
	 * Note: Since the pending addresses is filled from the back,
	 * the extended are filled in first.
	 */
	beaconPtr = bufPtr;

	/* Initialize extended address count. */
	pendingAddressCount = 0;

	/*
	 * This callback function traverses through the indirect queue and
	 * updates the beacon buffer with the pending extended addresses.
	 */
	findBuf.criteria_func = AddPendingExtendedAddressCb;
    findBuf.handle = NULL;

	/*
	 * At the end of this function call (qmm_queue_read), the beacon buffer
	 * will be updated with the short address (if any) of the indirect
	 * data (if any) present in the indirect queue.
	 */
    buf = qmm_queue_read(&indirectDataQueue, &findBuf);

	/*
	 * The count of extended addresses added in the beacon frame is backed
	 * up
	 * (as the same variable will be used to count the number of added
	 * short addresses).
	 */
	numberOfExtAddress = pendingAddressCount;

	/* Initialize extended address count. */
	pendingAddressCount = 0;

	/*
	 * This callback function traverses through the indirect queue and
	 * updates the beacon buffer with the pending short addresses.
	 */
	findBuf.criteria_func = AddPendingShortAddressCb;

	/*
	 * At the end of this function call (qmm_queue_read), the beacon buffer
	 * will be updated with the extended address (if any) of the indirect
	 * data (if any) present in the indirect queue.
	 */
	buf = qmm_queue_read(&indirectDataQueue, &findBuf);

	/*
	 * Update buf_ptr to current position of beginning of
	 * pending address specifications filled above.
	 * Fill in Pending Address Specification (see IEEE 802.15.4-2006 Table
	 * 46).
	 * In order to this buf_ptr needs to be decremented.
	 */
	bufPtr = beaconPtr - 1;
	*bufPtr = (pendingAddressCount) | (numberOfExtAddress  << 4);

	/*
	 * Total number of bytes used for pending address in beacon frame.
	 * Note: The length of the one octet for the Pending Address
	 * Specification
	 * is already included in the default beacon frame length
	 * (see BEACON_PAYLOAD_LEN).
	 */
	pendingAddressCount = (uint8_t)((pendingAddressCount * sizeof(uint16_t)) +
			(numberOfExtAddress * sizeof(uint64_t)));
    (void)buf;

	return pendingAddressCount;
} /* mac_buffer_add_pending() */

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

/**
 * @brief Builds and transmits the beacon frame
 *
 * This function is called to build a beacon frame. For beaconless network
 * this function also transmits the generated beacon frame.
 *
 * @param beaconEnabled Flag indicating the mode of beacon transmission
 * @param beaconBufferHeader For build without beacon support only:
 *                             Pointer to buffer of beacon frame to be
 *                             transmitted in nonbeacon network.
 **/
void MAC_BuildAndTxBeacon(bool beaconEnabled,
		buffer_t *beaconBufferHeader)
{
	MAC_FrameInfo_t *transmitFrame;
	uint16_t superframeSpec= 0;
	uint16_t fcf;
	uint8_t frameLen;
	uint8_t *framePtr;
    PHY_Retval_t phyStatus = PHY_FAILURE;
    PibValue_t pib_shortaddr;
    PibValue_t pib_ieeeaddr;
    PibValue_t pib_panid;
    
	uint8_t *beaconBuffer = (uint8_t *)MAC_BUFFER_POINTER(
			beaconBufferHeader);

	/*
	 * The frame is given to the TAL in the 'MAC_FrameInfo_t' format,
	 * hence an instance of the MAC_FrameInfo_t is created.
	 */
	transmitFrame = (MAC_FrameInfo_t *)(void *)beaconBuffer;

	/* Store buffer header to be able to release it later properly. */
	transmitFrame->buffer_header = beaconBufferHeader;

	/* MAC transmit frame type */
	transmitFrame->msgType = BEACON_MESSAGE;

	/* Update the payload length. */
	frameLen = BEACON_PAYLOAD_LEN +
			2U + /* Add 2 octets for FCS */
			2U + /* 2 octets for short Source Address */
			2U + /* 2 octets for short Source PAN-Id */
			3U; /* 3 octets DSN and FCF */

	/* Get the payload pointer. */
	framePtr = (uint8_t *)transmitFrame +
			LARGE_BUFFER_SIZE - 2; /* Add 2 octets for FCS. */

	/* Build the beacon payload if it exists. */
	if (macPib.mac_BeaconPayloadLength > 0U) {
		framePtr -= macPib.mac_BeaconPayloadLength;
		frameLen += macPib.mac_BeaconPayloadLength;

		(void *)memcpy(framePtr, macBeacon_Payload,
				macPib.mac_BeaconPayloadLength);
	}

	/* Build the Pending address field. */
#if (MAC_INDIRECT_DATA_FFD == 1)
	{
		/*
		 * Check if the indirect queue has entries, otherwise there is
		 * nothing
		 * to add as far as pending addresses is concerned.
		 */
		if (indirectDataQueue.size > 0U) {
			uint8_t pending_addr_octets = MAC_BufferAddPending(
					framePtr);
			frameLen += pending_addr_octets;
			framePtr -= pending_addr_octets + 1U;
		} else {
			/* No pending data available. */
			framePtr--;
			*framePtr = 0;
		}
	}
#else

	/*
	 * If indirect transmission is not enabled, the Pending Address Spec
	 * field is always 0.
	 */
	framePtr--;
	*framePtr = 0;
#endif
	framePtr--;
	*framePtr = 0;

	/* The superframe specification field is updated. */

	superframeSpec = NON_BEACON_NWK;
	superframeSpec |= (NON_BEACON_NWK << 4U);
	superframeSpec |= (uint16_t)FINAL_CAP_SLOT_DEFAULT << 8U;

	if (MAC_PAN_COORD_STARTED == macState) {
		superframeSpec |= ((uint16_t)1 << PAN_COORD_BIT_POS);
	}

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	if ((macPib.mac_AssociationPermit) == 1U) {
		superframeSpec |= ((uint16_t)1 << ASSOC_PERMIT_BIT_POS);
	}
#endif

	/* Set the Superframe Specification Field. */
	framePtr -= 2;
	convert_spec_16_bit_to_byte_array(superframeSpec, framePtr);
	/*
	 * Source address.
	 */
    
    phyStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_shortaddr);
	if (MAC_NO_SHORT_ADDR_VALUE ==
			pib_shortaddr.pib_value_16bit) {
		framePtr -= 8;
		frameLen += 6U; /* Add further 6 octets for long Source Address
		                **/
        phyStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_ieeeaddr);
        convert_64_bit_to_byte_array(pib_ieeeaddr.pib_value_64bit, framePtr);

		fcf = FCF_SET_SOURCE_ADDR_MODE((uint16_t)FCF_LONG_ADDR);
	} else {
		framePtr -= 2;
        convert_16_bit_to_byte_array(pib_shortaddr.pib_value_16bit, framePtr);
		fcf = FCF_SET_SOURCE_ADDR_MODE((uint16_t)FCF_SHORT_ADDR);
	}

	/* Source PAN-Id */
	framePtr -= 2;
    phyStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_panid);
    convert_16_bit_to_byte_array(pib_panid.pib_value_16bit, framePtr);

#ifdef TEST_HARNESS
	if (macPib.privateVirtualPANs > 0) {
		uint16_t temp = (pib_panid.pib_value_16bit);

		/*
		 * This changes the PAN-ID of subsequent beacon frames to
		 * simulate
		 * virtual PANs for testing purposes.
		 */
		temp += vpan_no;
		convert_16_bit_to_byte_array((
				temp), framePtr);
		vpan_no++;
		vpan_no = vpan_no % macPib.privateVirtualPANs;
	}
#endif /* TEST_HARNESS */

	/* Set BSN. */
	framePtr--;
	*framePtr = macPib.mac_BSN++;

	fcf = fcf | FCF_SET_FRAMETYPE(FCF_FRAMETYPE_BEACON);
	/* Set the FCF. */
	framePtr -= 2;
	convert_spec_16_bit_to_byte_array(fcf, framePtr);

	/* First element shall be length of PHY frame. */
	framePtr--;
	*framePtr = frameLen;

	/* Finished building of frame. */
	transmitFrame->mpdu = framePtr;
	/*
	 * In a beaconless network the beacon is transmitted with
	 * unslotted CSMA-CA.
	 */
	phyStatus = sendFrame(transmitFrame, CSMA_UNSLOTTED, false);

	MAKE_MAC_BUSY();

	beaconEnabled = beaconEnabled; /* Keep compiler happy. */
    (void)phyStatus;
} /* MAC_BuildAndTxBeacon() */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * @brief Appends the pending short addresses into the beacon frame
 *
 * This function appends the pending short addresses to the beacon based
 * on frames currently in the indirect queue.
 *
 * @param buf_ptr Pointer to the indirect data in the indirect queue
 * @param handle Callback parameter
 *
 * @return 0 to traverse through the full indirect queue
 *
 */
static uint8_t AddPendingShortAddressCb(void *bufPtr, void *handle)
{
	MAC_FrameInfo_t *frame = (MAC_FrameInfo_t *)bufPtr;

	/* Only 7 short addresses are allowed in one Beacon frame. */
	if (pendingAddressCount < BEACON_MAX_PEND_ADDR_CNT) {
		/*
		 * Only if the destination addressing mode is short address mode
		 * then the
		 * indirect data is used to populate the beacon buffer with
		 * short
		 * destination address.
		 */
		if (FCF_SHORT_ADDR ==
				((frame->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK)) {
			beaconPtr -= sizeof(uint16_t);
			(void *)memcpy(beaconPtr, &frame->mpdu[PL_POS_DST_ADDR_START],
					sizeof(uint16_t));
			pendingAddressCount++;
		}
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * @brief Appends the pending extended address into beacon
 *
 * This function appends pending extended addresses in the indirect queue
 * to the beacon.
 *
 * @param buf_ptr Pointer to the indirect data in the indirect queue
 * @param handle Callback parameter
 *
 * @return 0 to traverse through the full indirect queue
 *
 */
static uint8_t AddPendingExtendedAddressCb(void *bufPtr, void *handle)
{
	MAC_FrameInfo_t *frame = (MAC_FrameInfo_t *)bufPtr;

	/* Only 7 extended addresses are allowed in one Beacon frame. */
	if (pendingAddressCount < BEACON_MAX_PEND_ADDR_CNT) {
		/*
		 * Only if the destination addressing mode is extended address
		 * mode then the
		 * indirect data is used to populate the beacon buffer with
		 * extended
		 * destination address.
		 */
		if (FCF_LONG_ADDR ==
				((frame->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK)) {
			beaconPtr -= sizeof(uint64_t);
			(void *)memcpy(beaconPtr, &frame->mpdu[PL_POS_DST_ADDR_START],
					sizeof(uint64_t));
			pendingAddressCount++;
		}
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

/**
 * @brief Processes a beacon request
 *
 *  This function is called in case a beacon request frame has been received by
 *  a coordinator. In a nonbeacon-enabled PAN the generation of a beacon frame
 *  using CSMA-CA is initiated. In a beacon-enabled PAN no extra beacon frame
 *  will be transmitted apart from the standard beacon frames.
 *
 * @param msg Pointer to the buffer in which the beaocn request was received
 */
void MAC_ProcessBeaconRequest(buffer_t *bufPtr)
{
	MAC_BuildAndTxBeacon(false, (buffer_t *)bufPtr);
}

#endif /* MAC_START_REQUEST_CONFIRM */

/* EOF */
