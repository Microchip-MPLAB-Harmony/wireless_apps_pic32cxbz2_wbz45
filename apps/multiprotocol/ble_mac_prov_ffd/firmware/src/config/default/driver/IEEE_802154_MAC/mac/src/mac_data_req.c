/*******************************************************************************
  MAC Data req Source

  File Name:
    mac_data_req.c
  Summary:
    Data request related functions
  Description:
    This file implements data request related functions.

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

#ifdef MAC_SECURITY_ZIP
#include "mac_security.h"
#endif  /* MAC_SECURITY_ZIP */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/* === Macros =============================================================== */

/*
 * Data request payload length
 */
#define DATA_REQ_PAYLOAD_LEN            (1)

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

#if (MAC_INDIRECT_DATA_FFD == 1)
static buffer_t *buildNullDataFrame(void);
static uint8_t findLongBuffer(void *buf, void *long_addr);
static uint8_t findShortBuffer(void *buf, void *shortAddr);

#endif  /*  (MAC_INDIRECT_DATA_FFD == 1)*/

/* === Implementation ====================================================== */

/**
 * @brief Build and transmits data request command frame
 *
 * This function builds and transmits a data request command frame.
 *
 *
 * @param explPoll Data request due to explicit MLME poll request
 * @param forceOwnLongAddr Forces the usage of the Extended Address as
 *                            Source Address. This a allows for implicitly
 *                            poll for pending data at the coordinator if
 *                            the Extended Address was used in the Beacon frame.
 * @param explDestAddr_mode Mode of subsequent destination address to be used
 *                            explicitly (0/2/3).
 *                            0: No explicit destination address attached,
 *                               use either macCoordShortAddress or
 *                               macCoordExtendedAddress
 *                            2: Use explicitly attached address in parameter
 *                               explDestAddr as destination address as
 *                               short address
 *                            3: Use explicitly attached address in parameter
 *                               explDestAddr as destination address as
 *                               extended address
 * @param explDestAddr Explicitly attached destination address for data
 *                       request frame. This is to be treated as either not
 *                       present, short or extended address, depending on
 *                       parameter explDestAddr_mode.
 * @param explDestPanId Explicitly attached destination PAN-Id (Coordinator
 *                         PAN-Id) for data request frame.
 *                         This is to be treated only as present, depending on
 *                         parameter explDestAddr_mode.
 *
 * @return True if data request command frame was created and sent to
 *         the TAL successfully, false otherwise.
 */
bool MAC_BuildAndTxDataReq(bool explPoll,
		bool forceOwnLongAddr,
		uint8_t explDestAddrMode,
		AddressField_t *explDestAddr,
		uint16_t explDestPanId)
{
	PHY_Retval_t phyStatus = PHY_FAILURE;
	bool intrabit = false;
	uint8_t frameLen;
	uint8_t *framePtr;
	uint16_t fcf;
	buffer_t *bufPtr = bmm_buffer_alloc(LARGE_BUFFER_SIZE);
    PibValue_t pib_value;

	if (NULL == bufPtr) {
		return false;
	}

	MAC_FrameInfo_t *transmitFrame = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(
			bufPtr);

	/*
	 * If this data request cmd frame was initiated by a device due to
	 * implicit
	 * poll, set msgtype to DATAREQUEST_IMPL_POLL.
	 * If this data request cmd frame was initiated by a MLME poll request,
	 * set msgtype to DATAREQUEST.
	 */
	if (explPoll) {
		transmitFrame->msgType = DATAREQUEST;
	} else {
		transmitFrame->msgType = DATAREQUEST_IMPL_POLL;
	}

	/*
	 * The buffer header is stored as a part of MAC_FrameInfo_t structure
	 * before the
	 * frame is given to the TAL. After the transmission of the frame, reuse
	 * the buffer using this pointer.
	 */
	transmitFrame->buffer_header = bufPtr;

	/* Update the payload length. */
	frameLen = DATA_REQ_PAYLOAD_LEN +
			2 + /* Add 2 octets for FCS */
			2 + /* 2 octets for short Source Address */
			2 + /* 2 octets for short Destination Address */
			2 + /* 2 octets for Destination PAN-Id */
			3; /* 3 octets DSN and FCF */

	/* Get the payload pointer. */
	framePtr = (uint8_t *)transmitFrame +
			LARGE_BUFFER_SIZE -
			DATA_REQ_PAYLOAD_LEN - 2; /* Add 2 octets for FCS. */

	/*
	 * Build the command frame id.
	 * This is actually being written into "transmitFrame->payload[0]".
	 */
	*framePtr = (uint8_t)DATAREQUEST;

	/* Source Address */

	/*
	 * Long address needs to be used if a short address is not present
	 * or if we are forced to use the long address.
	 *
	 * This is used for example in cases where the coordinator indicates
	 * pending data for us using our extended address.
	 *
	 * This is also used for transmitting a data request frame
	 * during association, since here we always need to use our
	 * extended address.
	 */
    phyStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_value);
	if ((BROADCAST == pib_value.pib_value_16bit) ||
			(MAC_NO_SHORT_ADDR_VALUE ==
			pib_value.pib_value_16bit) ||
			forceOwnLongAddr) {
		framePtr -= 8;
		frameLen += 6U; /* Add further 6 octets for long Source Address
		                **/

		/* Build the Source address. */
        phyStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
        convert_64_bit_to_byte_array(pib_value.pib_value_64bit, framePtr);
		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_ACK_REQUEST;
	} else {
		framePtr -= 2U;

		/* Build the Source address. */
        convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);

		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_ACK_REQUEST;
	}

	/* Source PAN-Id */

	/*
	 * In IEEE 802.15.4 the PAN ID Compression bit may always be set.
	 * See page 154:
	 * If the data request command is being sent in response to the receipt
	 * of a beacon frame indicating that data are pending for that device,
	 * the Destination Addressing Mode subfield of the Frame Control field
	 * may be set to zero ..."
	 * In order to keep the implementation simple the address info is also
	 * in
	 * this case 2 or 3, i.e. the destination address info is present.
	 * This in return means that the PAN ID Compression bit is always set
	 * for
	 * data request frames, except the explDestPanId parameter is
	 * different from
	 * our own PAN-Id PIB attribute.
	 */
    phyStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
	if ((explDestAddrMode != FCF_NO_ADDR) &&
			(explDestPanId != pib_value.pib_value_16bit)
			) {
		framePtr -= 2;
		frameLen += 2U; /* Add further 6 octets for long Source Pan-Id
		                **/
        convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);

	} else {
		/*
		 * The source PAN Id is not present since the PAN ID
		 * Compression bit is set.
		 */
		/* Set intra-PAN bit. */
		intrabit = true;
		fcf |= FCF_PAN_ID_COMPRESSION;
	}

	/* Destination Address */
	if (FCF_SHORT_ADDR == explDestAddrMode) {
		/* An explicit short destination address is requested. */
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);

		framePtr -= 2;
		convert_16_bit_to_byte_array(explDestAddr->shortAddress,
				framePtr);
	} else if (FCF_LONG_ADDR == explDestAddrMode) {
		/* An explicit long destination address is requested. */
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);

		framePtr -= 8;
		frameLen += 6U; /* Add further 6 octets for long Destination
		                 * Address */
		convert_64_bit_to_byte_array(explDestAddr->longAddress,
				framePtr);
	} else {
		/* No explicit destination address is requested. */
		if (MAC_NO_SHORT_ADDR_VALUE !=
				macPib.mac_CoordShortAddress) {
			/*
			 * If current value of short address for coordinator PIB
			 * is
			 * NOT 0xFFFE, the current value of the short address
			 * for
			 * coordinator shall be used as desination address.
			 */
			fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR);

			framePtr -= 2;
			convert_16_bit_to_byte_array(
					macPib.mac_CoordShortAddress,
					framePtr);
		} else {
			/*
			 * If current value of short address for coordinator PIB
			 * is 0xFFFE,
			 * the current value of the extended address for
			 * coordinator
			 * shall be used as desination address.
			 */
			fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);

			framePtr -= 8;
			frameLen += 6U; /* Add further 6 octets for long
			                 * Destination Address */
			convert_64_bit_to_byte_array(
					macPib.mac_CoordExtendedAddress,
					framePtr);
		}
	}

	/* Destination PAN-Id */
	framePtr -= 2;

	if (intrabit) {
		/* Add our PAN-Id. */
        phyStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
        convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);
	} else {
		/*
		 * There is an expclicit destination address present AND
		 * the destination PAN-Id is different from our own PAN-ID,
		 * so include the source PAN-id into the frame.
		 */
		convert_16_bit_to_byte_array(explDestPanId, framePtr);
	}

	/* Set DSN. */
	framePtr--;
	*framePtr = macPib.mac_DSN++;

	/* Set the FCF. */
	framePtr -= 2;
	convert_spec_16_bit_to_byte_array(fcf, framePtr);

	/* First element shall be length of PHY frame. */
	framePtr--;
	*framePtr = frameLen;

	/* Finished building of frame. */
	transmitFrame->mpdu = framePtr;
    (void)phyStatus;

	/* In Nonbeacon build the frame is sent with unslotted CSMA-CA. */
	phyStatus = sendFrame(transmitFrame, CSMA_UNSLOTTED, true);

	if (PHY_SUCCESS == phyStatus) {
		MAKE_MAC_BUSY();
		return true;
	} else {
		/* TAL is busy, hence the data request could not be transmitted
		**/
		bmm_buffer_free(bufPtr);

		return false;
	}
    
} /* MAC_BuildAndTxDataReq() */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Constructs a null data frame
 *
 * @return Pointer to the created null data frame, NULL otherwise.
 */
static buffer_t *buildNullDataFrame(void)
{
	bool useLongAddrDest;
	MAC_FrameInfo_t *transmitFrame;
	uint8_t frameLen;
	uint8_t *framePtr;
	uint16_t fcf = 0;
	buffer_t *bufPtr = bmm_buffer_alloc(LARGE_BUFFER_SIZE);
    PHY_Retval_t pibStatus = PHY_FAILURE;
    PibValue_t pib_value;

	if (NULL == bufPtr) {
		return NULL;
	}

	transmitFrame = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(bufPtr);

	/* No data payload, this is a null packet.*/
	transmitFrame->msgType = NULL_FRAME;
	transmitFrame->buffer_header = bufPtr;

	/* No indirect transmission. */
	transmitFrame->indirectInTransit = false;

	/* Update the payload length. */
	frameLen = 2 + /* Add 2 octets for FCS */
			2 + /* 2 octets for short Destination Address */
			2 + /* 2 octets for Destination PAN-Id */
			2 + /* 2 octets for short Source Address */
			3; /* 3 octets DSN and FCF */

	/* Get the payload pointer. */
	framePtr = (uint8_t *)transmitFrame + LARGE_BUFFER_SIZE - 2; /* Add 2
	                                                                * octets
	                                                                * for
	                                                                * FCS.
	                                                                **/

	/*
	 * Set Source Address.
	 */
    pibStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_value);
	if ((BROADCAST == pib_value.pib_value_16bit) ||
			(MAC_NO_SHORT_ADDR_VALUE == pib_value.pib_value_16bit)
			) {
		/* Use long address as source address. */
		framePtr -= 8;
		frameLen += 6U;
        pibStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
        convert_64_bit_to_byte_array(pib_value.pib_value_64bit, framePtr);
		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR);
	} else {
		/* Use short address as source address. */
		framePtr -= 2;
        pibStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_value);
        convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);
		fcf |= FCF_SET_SOURCE_ADDR_MODE(FCF_SHORT_ADDR);
	}

	/* Shall the Intra-PAN bit set? */
    pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
	if (pib_value.pib_value_16bit == macParseData.srcPanid) {
		/*
		 * Both PAN-Ids are identical.
		 * Set intra-PAN bit.
		 */
		fcf |= FCF_PAN_ID_COMPRESSION;
	} else {
		/* Set Source PAN-Id. */
		framePtr -= 2;
		frameLen += 2U;
        convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);
	}

	/* Set Destination Address. */
	useLongAddrDest = (FCF_LONG_ADDR == macParseData.srcAddrMode);

	/* Destination address is set from source address of received frame. */
	if (useLongAddrDest) {
		framePtr -= 8;
		frameLen += 6U;
		convert_64_bit_to_byte_array(
				macParseData.srcAddr.longAddress,
				framePtr);

		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_DATA);
	} else {
		framePtr -= 2;
		convert_16_bit_to_byte_array(
				macParseData.srcAddr.shortAddress,
				framePtr);
		fcf |= FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_DATA);
	}

	/* Destination PANId is set from source PANId of received frame. */
	framePtr -= 2;
	convert_16_bit_to_byte_array(macParseData.srcPanid, framePtr);

	/* Set DSN. */
	framePtr--;
	*framePtr = macPib.mac_DSN++;

	/* Set the FCF. */
	framePtr -= 2;
	convert_spec_16_bit_to_byte_array(fcf, framePtr);

	/* First element shall be length of PHY frame. */
	framePtr--;
	*framePtr = frameLen;

	/* Finished building of frame. */
	transmitFrame->mpdu = framePtr;

    (void)pibStatus;
	return bufPtr;
} /* buildNullDataFrame() */

/**
 * @brief Processes a received data request command frame
 *
 * This function processes a received data request command frame
 * at the coordinator, searches for pending indirect data frames
 * for the originator and initiates the frame transmission of the
 * data frame with CSMA-CA.
 *
 * @param msg Frame reception buffer pointer
 */
void MAC_ProcessDataRequest(buffer_t *msg)
{
	/* Buffer pointer to next indirect data frame to be transmitted. */
	buffer_t *bufPtrNextData;
	search_t findBuf;
	MAC_FrameInfo_t *transmitFrame;
	PHY_Retval_t phyTxStatus;

	/* Free the buffer of the received frame. */
	bmm_buffer_free(msg);

	/* Ignore data request if we are not PAN coordinator or coordinator. */
	if ((MAC_IDLE == macState) ||
			(MAC_ASSOCIATED == macState)
			) {
		return;
	}

	/* Check the addressing mode */
	if (macParseData.srcAddrMode == FCF_SHORT_ADDR) {
		/*
		 * Look for pending data in the indirect queue for
		 * this short address.
		 */

		/*
		 * Assign the function pointer for searching the
		 * data having address of the requested device.
		 */
		findBuf.criteria_func = findShortBuffer;

		/* Update the short address to be searched. */
		findBuf.handle = &macParseData.srcAddr.shortAddress;
	} else if (macParseData.srcAddrMode == FCF_LONG_ADDR) {
		/*
		 * Look for pending data in the indirect queue for
		 * this long address.
		 */

		/*
		 * Assign the function pointer for searching the
		 * data having address of the requested device.
		 */
		findBuf.criteria_func = findLongBuffer;

		/* Update the long address to be searched. */
		findBuf.handle = &macParseData.srcAddr.longAddress;
	} else {
		return;
	}

	/*
	 * Read from the indirect queue. The removal of items from this queue
	 * will be done after successful transmission of the frame.
	 */
	bufPtrNextData = qmm_queue_read(&indirectDataQueue, &findBuf);

	/* Note: The find_buf structure is reused below, so do not change this.
	**/

	if (NULL == bufPtrNextData) {
		MAC_HandleTxNullDataFrame();
		return;
	} else {
		/* Indirect data found and to be sent. */
		transmitFrame = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(
				bufPtrNextData);

		/*
		 * We need to check whether the source PAN-Id of the previously
		 * received data request frame is identical to the destination
		 * PAN-Id
		 * of the pending frame. If not the frame shall not be
		 * transmitted,
		 * but a Null Data frame instead.
		 */
		if (macParseData.srcPanid !=
				convert_byte_array_to_16_bit(&transmitFrame->
				mpdu[PL_POS_DST_PAN_ID_START])
				) {
			MAC_HandleTxNullDataFrame();
			return;
		} else {
			/*
			 * The frame to be transmitted next is marked.
			 * This is necessary since the queue needs to be
			 * traversed again
			 * to find other pending indirect data frames for this
			 * particular
			 * recipient.
			 */
			transmitFrame->indirectInTransit = true;
			transmitFrame->buffer_header = bufPtrNextData;

			/*
			 * Go through the indirect data queue to find out the
			 * frame pending for
			 * the device which has requested for the data.
			 */

			/*
			 * Since the buffer header has already been stored in
			 * transmitFrame->buffer_header, it can be reused here
			 * for
			 * other purpose.
			 */

			/*
			 * It is assumed that the find_buf struct does still
			 * have
			 * the original values from above.
			 */
			bufPtrNextData = qmm_queue_read(&indirectDataQueue,
					&findBuf);

			/*
			 * Check whether there is another indirect data
			 * available
			 * for the same recipient.
			 */
			if (NULL != bufPtrNextData) {
				transmitFrame->mpdu[PL_POS_FCF_1]
					|= FCF_FRAME_PENDING;
			}
		}

#ifdef MAC_SECURITY_ZIP
		if (((transmitFrame->mpdu[1] & FCF_SECURITY_ENABLED)) == FCF_SECURITY_ENABLED) {
			MCPS_DataReq_t pmdr = {0};
            bool retVal;

			retVal = BuildSecMcpsDataFrame(&pmdr, transmitFrame);

			if (pmdr.SecurityLevel > 0U) {
				/* Secure the Frame */
				MAC_Retval_t buildSec = MAC_Secure(transmitFrame,	\
						transmitFrame->macPayload,
						&pmdr);

				if (MAC_SUCCESS != buildSec) {
					/* The MAC Data Payload is encrypted
					 * based on the security level. */
					transmitFrame->indirectInTransit
						= false;
					return;
				}
			}
            (void)retVal;
		}
#endif

		/*
		 * Transmission should be done with CSMA-CA or
		 * quickly after the ACK of the data request command.
		 * Here it's done quickly after the ACK w/o CSMA.
		 */
		phyTxStatus = sendFrame(transmitFrame, CSMA_UNSLOTTED,
				false);

		if (PHY_SUCCESS == phyTxStatus) {
			MAKE_MAC_BUSY();
		} else {
			/*
			 * TAL rejects frame transmission, since it's too close
			 * to the next
			 * beacon transmission. The frame is kept in the
			 * indirect queue.
			 */
			transmitFrame->indirectInTransit = false;
		}
	}
} /* MAC_ProcessDataRequest() */

/*
 * @brief Creates and transmits a Null Data frame
 *
 * This function creates and transmits a Null Data frame in case the
 * coordinator does not have pending data to be transmitted.
 */
void MAC_HandleTxNullDataFrame(void)
{
	MAC_FrameInfo_t *tx_frame;
	PHY_Retval_t phyTxStatus;
	buffer_t *bufPtr;

	/*
	 * No matching pending item in the queue,
	 * so a Null Data frame is created.
	 */
	bufPtr = buildNullDataFrame();

	if (NULL != bufPtr) {
		tx_frame = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(bufPtr);

		/*
		 * Transmission should be done with CSMA-CA or
		 * quickly after the ACK of the data request command.
		 * Here it's done quickly after the ACK w/o CSMA.
		 */
		phyTxStatus = sendFrame(tx_frame, CSMA_UNSLOTTED, false);

		if (PHY_SUCCESS == phyTxStatus) {
			MAKE_MAC_BUSY();
		} else {
			/*
			 * Transmission to TAL failed, free up the buffer used
			 * to create
			 * Null Data frame.
			 */
			bmm_buffer_free(bufPtr);
		}
	}
}

/*
 * @brief Checks for matching short address
 *
 * This callback function checks whether the passed short address
 * matches with the frame in the queue.
 *
 * @param buf Pointer to indirect data buffer
 * @param shortAddr Short address to be searched
 *
 * @return 1 if short address passed matches with the destination
 * address of the indirect frame , 0 otherwise
 */
static uint8_t findShortBuffer(void *buf, void *shortAddr)
{
	MAC_FrameInfo_t *data = (MAC_FrameInfo_t *)buf;
	uint16_t shortAddrTemp = 0;
	(void)memcpy((uint8_t *)&shortAddrTemp, (uint8_t *)shortAddr, \
			sizeof(shortAddrTemp));

	if (!data->indirectInTransit) {
		/*
		 * Read octet 2 of Frame Control Field containing the type
		 * of destination address.
		 */
		uint8_t dstAddrMode
			= (data->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK;

		/*
		 * Compare indirect data frame's dest_address(short)
		 * with short address passed.
		 */
		if ((dstAddrMode == FCF_SHORT_ADDR) &&
				(shortAddrTemp ==
				convert_byte_array_to_16_bit(&data->mpdu[
					PL_POS_DST_ADDR_START]))
				) {
			return 1;
		}
	}

	return 0;
}

/*
 * @brief Checks for matching extended address
 *
 * This callback function checks whether the passed short address
 * matches with the frame in the queue.
 *
 * @param buf Pointer to indirect data buffer
 * @param long_addr Extended address to be searched
 *
 * @return 1 if extended address passed matches with the destination
 * address of the indirect frame , 0 otherwise
 */
static uint8_t findLongBuffer(void *buf, void *long_addr)
{
	MAC_FrameInfo_t *data = (MAC_FrameInfo_t *)buf;
	uint64_t longAddrTemp = 0;
	(void)memcpy((uint8_t *)&longAddrTemp, (uint8_t *)long_addr, \
			sizeof(longAddrTemp));

	if (!data->indirectInTransit) {
		/*
		 * Read octet 2 of Frame Control Field containing the type
		 * of destination address.
		 */
		uint8_t dstAddrMode
			= (data->mpdu[PL_POS_FCF_2] >>
				FCF_2_DEST_ADDR_OFFSET) & FCF_ADDR_MASK;

		/*
		 * Compare indirect data frame's dest_address(extended)
		 * with the exended address passed.
		 */
		if ((dstAddrMode == FCF_LONG_ADDR) &&
				(longAddrTemp ==
				convert_byte_array_to_64_bit(&data->mpdu[
					PL_POS_DST_ADDR_START]))
				) {
			return 1;
		}
	}

	return 0;
}

#endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/* EOF */
