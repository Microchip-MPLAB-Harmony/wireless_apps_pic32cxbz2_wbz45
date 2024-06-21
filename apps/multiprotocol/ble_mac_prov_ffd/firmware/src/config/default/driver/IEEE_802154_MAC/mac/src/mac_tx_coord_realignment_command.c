/*******************************************************************************
  MAC Tx Coord Realignment Command Source

  File Name:
    mac_tx_coord_realignment_command.c
  Summary:
    Coordinator realignment command
  Description:
    This file implements the coordinator realignment command.

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
#include "mac_tasks.h"

/* === Macros =============================================================== */

/*
 * Coordinator realignment payload length
 */

/*
 * In 802.15.4-2006 the channel page may be added, if the new channel page is
 * different than the original value. In order to simplify the code, it
 * is always added.
 */
#define COORD_REALIGN_PAYLOAD_LEN       (9)

/* === Globals ============================================================== */

/* === Prototypes =========================================================== */

/* === Implementation ======================================================= */

#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_START_REQUEST_CONFIRM == 1))

/**
 * @brief Sends a coordinator realignment command frame
 *
 * This function is called either in response to the reception of an orphan
 * notification command from a device (cmdType = ORPHANREALIGNMENT),
 * or gratuitously whenever the PAN parameters are about to be changed
 * (cmd_type = COORDINATORREALIGNMENT). In the first case, the
 * paramater mor contains a pointer to the respective
 * MLME_ORPHAN.response message, while in the latter case this
 * parameter is unused, and can be passed as NULL.
 *
 * @param cmd_type Determines directed or broadcast mode
 * @param buf_ptr Pointer to the buffer, using which coord_realignment_command
 *                to be sent
 * @param newPanid Contains the new PAN-ID in case there is a network
 *                  realignment
 * @param new_channel Contains the new channel in case there is a network
 *                    realignment
 * @param new_page Contains the new channel page in case there is a network
 *                 realignment
 *
 * @return True if coord_realignment_command is sent successfully,
 *         false otherwise
 */
bool MAC_TxCoordRealignmentCommand(FrameMsgtype_t cmdType,
		buffer_t *bufPtr,
		uint16_t newPanid,
		uint8_t newChannel,
		uint8_t newPage)
{
	PHY_Retval_t phyStatus = PHY_FAILURE;
	uint8_t frameLen;
	uint8_t *framePtr;
	uint8_t *tempFramePtr;
	uint16_t fcf;
	uint16_t bcAddr = BROADCAST;
    PibValue_t pib_value;

	/*
	 * Orphan request is reused to send coordinator realignment
	 * command frame and finally to send comm-status-indication
	 */
	MLME_OrphanResp_t orphan_resp;
	(void)memcpy(&orphan_resp,
			(MLME_OrphanResp_t *)MAC_BUFFER_POINTER((buffer_t *)
			bufPtr),
			sizeof(MLME_OrphanResp_t));

	MAC_FrameInfo_t *coord_realignment_frame
		= (MAC_FrameInfo_t *)MAC_BUFFER_POINTER((buffer_t *)bufPtr);

	coord_realignment_frame->msgType = cmdType;
	coord_realignment_frame->buffer_header = bufPtr;

	/* Get the payload pointer. */
	framePtr = tempFramePtr
				= (uint8_t *)coord_realignment_frame +
					LARGE_BUFFER_SIZE -
					COORD_REALIGN_PAYLOAD_LEN - 2; /* Add 2
	                                                                * octets
	                                                                * for
	                                                                * FCS.
	                                                                **/

	/* Update the payload field. */
	*framePtr++ = (uint8_t)((FrameMsgtype_t)COORDINATORREALIGNMENT);

	/*
	 * The payload of the frame has the parameters of the new PAN
	 * configuration
	 */
	*framePtr++ = (uint8_t)newPanid;
	*framePtr++ = (uint8_t)(newPanid >> 8U);

    phyStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_value);

    
	*framePtr++ = (uint8_t)(pib_value.pib_value_16bit);
	*framePtr++ = (uint8_t)(pib_value.pib_value_16bit >> 8);
	*framePtr++ = newChannel;

	/*
	 * Insert the device's short address, or 0xFFFF if this is a
	 * gratuitous realigment.
	 */
	if (ORPHANREALIGNMENT == cmdType) {
		*framePtr++ = (uint8_t)(orphan_resp.ShortAddress);
		*framePtr++ = (uint8_t)(orphan_resp.ShortAddress >> 8U);
	} else {
		*framePtr++ = (uint8_t)BROADCAST;
		*framePtr++ = (BROADCAST >> 8U);
	}

	/* Add channel page no matter if it changes or not. */
	*framePtr++ = newPage;

	/* Get the payload pointer again to add the MHR. */
	framePtr = tempFramePtr;

	/* Update the length. */
	frameLen = COORD_REALIGN_PAYLOAD_LEN +
			2 + /* Add 2 octets for FCS */
			2 + /* 2 octets for Destination PAN-Id */
			2 + /* 2 octets for short Destination Address */
			2 + /* 2 octets for Source PAN-Id */
			8 + /* 8 octets for long Source Address */
			3; /* 3 octets DSN and FCF */

	/* Source address */
	framePtr -= 8;
    
    phyStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
    convert_64_bit_to_byte_array(pib_value.pib_value_64bit, framePtr);

	/* Source PAN-Id */
	framePtr -= 2;
    phyStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
    convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);

	/* Destination Address and FCF */
	if (ORPHANREALIGNMENT == cmdType) {
		/*
		 * Coordinator realignment in response to an orphan
		 * notification command received from a device. This is always
		 * sent to a 64-bit device address, and the device is
		 * requested to acknowledge the reception.
		 */
		fcf
			= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_ACK_REQUEST | FCF_FRAME_VERSION_2006;

		framePtr -= 8;
		frameLen += 6U; /* Add further 6 octets for long Destination
		                 * Address */
		convert_64_bit_to_byte_array(orphan_resp.OrphanAddress,
				framePtr);
	} else {
		/*
		 * Coordinator realignment gratuitously sent when the PAN
		 * configuration changes. This is sent to the (16-bit)
		 * broadcast address.
		 */
		fcf
			= FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |

		                /*
		                 * Since the channel page is always added at the
		                 * end of the
		                 * coordinator realignment command frame, the
		                 * frame version subfield
		                 * needs to indicate a 802.15.4-2006 compatible
		                 * frame.
		                 */
				FCF_FRAME_VERSION_2006;

		framePtr -= 2;
		convert_16_bit_to_byte_array(bcAddr, framePtr);
	}

	/* Destination PAN-Id */
	framePtr -= 2;
	convert_16_bit_to_byte_array(bcAddr, framePtr);

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
	coord_realignment_frame->mpdu = framePtr;
    (void)phyStatus;

	/* The frame is given to the TAL for transmission */
	/* In build without beacon support the frame is sent with unslotted
	 * CSMA-CA. */
	phyStatus = sendFrame(coord_realignment_frame, CSMA_UNSLOTTED,
			true);

	if (PHY_SUCCESS == phyStatus) {
		/*
		 * A positive confirmation is given since the TAL has
		 * successfuly accepted
		 * the frame for transmission.
		 */
		MAKE_MAC_BUSY();
		return true;
	} else {
		/*
		 * TAL was unable to transmit the frame, hence a negative
		 * confirmation
		 * is returned.
		 */
		return false;
	}
} /* MAC_TxCoordRealignmentCommand() */

#endif /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || (MAC_START_REQUEST_CONFIRM
        *== 1)) */

/* EOF */
