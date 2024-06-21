/*******************************************************************************
  MAC data extract mhr source

  File Name:
    mac_data_extract_mhr.c
  Summary:
    Common functionality for MAC
  Description:
    This file implements common functionality for MAC.

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
#include "mac_internal.h"

/* === Macros =============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

/*
 * @brief Helper function to extract the complete address information
 *        of the received frame
 *
 * @param frame_ptr Pointer to first octet of Addressing fields of received
 * frame
 *        (See IEEE 802.15.4-2006 Figure 41)
 *
 * @return bool Length of Addressing fields
 */
uint8_t MAC_ExtractMhrAddrInfo(uint8_t *framePtr)
{
	uint16_t fcf = macParseData.fcf;
	uint8_t srcAddrMode = (uint8_t)((fcf >> FCF_SOURCE_ADDR_OFFSET) & FCF_ADDR_MASK);
	uint8_t dstAddrMode = (uint8_t)((fcf >> FCF_DEST_ADDR_OFFSET) & FCF_ADDR_MASK);
	bool intraPan = (bool)(fcf & FCF_PAN_ID_COMPRESSION);
	uint8_t addrFieldLen = 0;

	if (dstAddrMode != 0U) {
		macParseData.destPanid = convert_byte_array_to_16_bit(
				framePtr);
		framePtr += PAN_ID_LEN;
		addrFieldLen += PAN_ID_LEN;

		if (FCF_SHORT_ADDR == dstAddrMode) {
			/*
			 * First initialize the complete long address with zero,
			 * since
			 * later only 16 bit are actually written.
			 */
			macParseData.destAddr.longAddress = 0;
			macParseData.destAddr.shortAddress
				= convert_byte_array_to_16_bit(framePtr);
			framePtr += SHORT_ADDR_LEN;
			addrFieldLen += SHORT_ADDR_LEN;
		} else if (FCF_LONG_ADDR == dstAddrMode) {
			macParseData.destAddr.longAddress
				= convert_byte_array_to_64_bit(framePtr);
			framePtr += EXT_ADDR_LEN;
			addrFieldLen += EXT_ADDR_LEN;
		}
        else{
            // do nothing
        }
        }

	if (srcAddrMode != 0U) {
		if (!intraPan) {
			/*
			 * Source PAN ID is present in the frame only if the
			 * intra-PAN bit
			 * is zero and srcAddrMode is non zero.
			 */
			macParseData.srcPanid = convert_byte_array_to_16_bit(
					framePtr);
			framePtr += PAN_ID_LEN;
			addrFieldLen += PAN_ID_LEN;
		} else {
			/*
			 * The received frame does not contain a source PAN ID,
			 * hence
			 * source PAN ID is updated with the destination PAN ID.
			 */
			macParseData.srcPanid = macParseData.destPanid;
		}

		/* The source address is updated. */
		if (FCF_SHORT_ADDR == srcAddrMode) {
			/*
			 * First initialize the complete long address with zero,
			 * since
			 * later only 16 bit are actually written.
			 */
			macParseData.srcAddr.longAddress = 0;
			macParseData.srcAddr.shortAddress
				= convert_byte_array_to_16_bit(framePtr);
			framePtr += SHORT_ADDR_LEN;

			addrFieldLen += SHORT_ADDR_LEN;
		} else if (FCF_LONG_ADDR == srcAddrMode) {
			macParseData.srcAddr.longAddress
				= convert_byte_array_to_64_bit(framePtr);
			framePtr += EXT_ADDR_LEN;
			addrFieldLen += EXT_ADDR_LEN;
		}
        else{
            // do nothing
        }
        }

	/*
	 * The length of the Addressing Field is known, so the length of the
	 * MAC payload can be calcluated.
	 * The actual MAC payload length is calculated from
	 * the length of the mpdu minus 2 octets FCS, minus 1 octet sequence
	 * number, minus the length of the addressing fields, minus 2 octet FCS.
	 */
	macParseData.macPayloadLength = macParseData.mpduLength -
			FCF_LEN -
			SEQ_NUM_LEN -
			addrFieldLen -
			FCS_LEN;

	macParseData.srcAddrMode = srcAddrMode;
	macParseData.destAddrMode = dstAddrMode;

	return (addrFieldLen);
}

/* EOF */
