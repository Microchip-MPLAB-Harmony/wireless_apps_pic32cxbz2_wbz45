/*******************************************************************************
  MAC mcps data Source

  File Name:
    mac_mcps_data.c
  Summary:
    Handles MCPS related primitives and frames
  Description:
    This file handles MCPS related primitives and frames.

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
#include "mac_tasks.h"
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
#include "mac_security.h"
#endif

/* === Macros =============================================================== */
/* Length of the MAC Aux Header Frame Counter */
#define FRAME_COUNTER_LEN               (0x04)

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
/* Security Control Field: Security Level mask */
#define SEC_CTRL_SEC_LVL_MASK           (0x07)

/* Security Control Field: Key Identifier mask */
#define SEC_CTRL_KEY_ID_MASK            (0x03)

/* Security Control Field: Key Identifier Field position */
#define SEC_CTRL_KEY_ID_FIELD_POS       (3)

#define KEY_ID_MODE_0                   (0x00)
#define KEY_ID_MODE_1                   (0x01)
#define KEY_ID_MODE_2                   (0x02)
#define KEY_ID_MODE_3                   (0x03)

#endif
/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

static MAC_Retval_t BuildDataFrame(MCPS_DataReq_t *pmdr,
		MAC_FrameInfo_t *frame);

#if (MAC_INDIRECT_DATA_FFD == 1)
static void HandlePersistenceTimeDecrement(void);
static void HandleExpPersistenceTimer(buffer_t *bufPtr);
static void MAC_Timer_PersistenceCb(void *callbackParameter);
static uint8_t DecrementPersistenceTime(void *bufPtr, void *handle);
static uint8_t CheckPersistenceTimeZero(void *bufPtr, void *handle);

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

/* MAC-internal Buffer functions */
#if (MAC_PURGE_REQUEST_CONFIRM == 1)
static uint8_t CheckMsduHandleCb(void *buf, void *handle);
static bool MAC_BufferPurge(uint8_t msdu_handle);

#endif /* (MAC_PURGE_REQUEST_CONFIRM == 1) */

/* === Implementation ====================================================== */

/*
 * @brief Initiates mcps data confirm message
 *
 * This function creates the mcps data confirm structure,
 * and appends it into internal event queue.
 *
 * @param buf Buffer for mcps data confirmation.
 * @param status Data transmission status.
 * @param handle MSDU handle.
 * @param timestamp Time in symbols at which the data were transmitted
 *        (only if timestampting is enabled).
 */
#ifdef ENABLE_TSTAMP
void MAC_GenMCPSDataConf(buffer_t *buf, uint8_t status, uint8_t handle,
		uint32_t timestamp)
#else
void MAC_GenMCPSDataConf(buffer_t *buf, uint8_t status, uint8_t handle)
#endif  /* ENABLE_TSTAMP */
{
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
	MCPS_DataConf_t *mdc = (MCPS_DataConf_t *)MAC_BUFFER_POINTER(buf);

	mdc->cmdcode = MCPS_DATA_CONFIRM;
	mdc->msduHandle = handle;
	mdc->status = status;
#ifdef ENABLE_TSTAMP
	mdc->Timestamp = timestamp;
#endif  /* ENABLE_TSTAMP */

	qmmStatus = qmm_queue_append(&macNhleQueue, buf);
    WPAN_PostTask();
    (void)qmmStatus;
}

/**
 * @brief Builds the data frame for transmission
 *
 * This function builds the data frame for transmission.
 * The NWK layer has supplied the parameters.
 * The MAC_FrameInfo_t data type is constructed and filled in.
 * Also the FCF is constructed based on the parameters passed.
 *
 * @param msg Pointer to the MCPS-DATA.request parameter
 */
void MAC_MCPS_DataRequest(void *msg)
{
	MAC_Retval_t status = FAILURE;
    PHY_Retval_t phyTxStatus;
	MCPS_DataReq_t mdr;

	(void)memcpy(((void *)&mdr), MAC_BUFFER_POINTER(
			(buffer_t *)msg), sizeof(MCPS_DataReq_t));

	if ((mdr.TxOptions & WPAN_TXOPT_INDIRECT) == 0U) {
		/*
		 * Data Requests for a coordinator using direct transmission are
		 * accepted in all non-transient states (no polling and no
		 * scanning
		 * is ongoing).
		 */
		if ((MAC_POLL_IDLE != macPollState) ||
				(MAC_SCAN_IDLE != macScanState)
				) {
			MAC_GenMCPSDataConf((buffer_t *)msg,
					(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
#ifdef ENABLE_TSTAMP
					mdr.msduHandle,
					0);
#else
					mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
			return;
		}
	}
	/*To check the broadcst address*/
	uint16_t broadcastCheck;
	ADDR_COPY_DST_SRC_16(broadcastCheck, ((uint16_t)(mdr.DstAddr)));
	/* Check whether somebody requests an ACK of broadcast frames */
	if (((mdr.TxOptions & WPAN_TXOPT_ACK) == WPAN_TXOPT_ACK) &&
			(FCF_SHORT_ADDR == mdr.DstAddrMode) &&
			(BROADCAST == broadcastCheck)) {
		MAC_GenMCPSDataConf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_PARAMETER,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}

	/* Check whether both Src and Dst Address are not present */
	if ((FCF_NO_ADDR == mdr.SrcAddrMode) &&
			(FCF_NO_ADDR == mdr.DstAddrMode)) {
		MAC_GenMCPSDataConf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_ADDRESS,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}

	/* Check whether Src or Dst Address indicate reserved values */
	if ((FCF_RESERVED_ADDR == mdr.SrcAddrMode) ||
			(FCF_RESERVED_ADDR == mdr.DstAddrMode)) {
		MAC_GenMCPSDataConf((buffer_t *)msg,
				(uint8_t)MAC_INVALID_PARAMETER,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}

	/* Now all is fine, continue... */
	MAC_FrameInfo_t *transmitFrame
		= (MAC_FrameInfo_t *)MAC_BUFFER_POINTER((buffer_t *)msg);

	/* Store the message type */
	transmitFrame->msgType = MCPS_MESSAGE;
	transmitFrame->msduHandle = mdr.msduHandle;

#if (MAC_INDIRECT_DATA_FFD == 1)
	/* Indirect transmission not ongoing yet. */
	transmitFrame->indirectInTransit = false;
#endif  /* (MAC_INDIRECT_DATA_FFD == 1) */

	status = BuildDataFrame(&mdr, transmitFrame);

	if (MAC_SUCCESS != status) {
		/* The frame is too long. */
		MAC_GenMCPSDataConf((buffer_t *)msg,
				(uint8_t)status,
#ifdef ENABLE_TSTAMP
				mdr.msduHandle,
				0);
#else
				mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
		return;
	}

	/*
	 * Indirect transmission is only allowed if we are
	 * a PAN coordinator or coordinator.
	 */
#if (MAC_INDIRECT_DATA_FFD == 1)
	if (
		((mdr.TxOptions & WPAN_TXOPT_INDIRECT) == WPAN_TXOPT_INDIRECT) &&
		((MAC_PAN_COORD_STARTED == macState) ||
		(MAC_COORDINATOR == macState))
		) {
		/* Append the MCPS data request into the indirect data queue */
#ifdef ENABLE_QUEUE_CAPACITY
		if (QMM_QUEUE_FULL ==
				qmm_queue_append(&indirectDataQueue,
				(buffer_t *)msg)) {
			MAC_GenMCPSDataConf((buffer_t *)msg,
					(uint8_t)MAC_TRANSACTION_OVERFLOW,
#ifdef ENABLE_TSTAMP
					mdr.msduHandle,
					0);
#else
					mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */
			return;
		}

#else
		qmm_queue_append(&indirectDataQueue, (buffer_t *)msg);
#endif  /* ENABLE_QUEUE_CAPACITY */

		/*
		 * If an FFD does have pending data,
		 * the MAC persistence timer needs to be started.
		 */
		transmitFrame->persistenceTime
			= macPib.mac_TransactionPersistenceTime;
		MAC_CheckPersistenceTimer();
        WPAN_PostTask();
	} else
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

	/*
	 * We are NOT indirect, so we need to transmit using
	 * CSMA_CA in the CAP (for beacon enabled) or immediately (for
	 * a non-beacon enabled).
	 */
	{
		MAC_TrxWakeup();

		transmitFrame->buffer_header = (buffer_t *)msg;

		/* Transmission should be done with CSMA-CA and with frame
		 * retries. */

#ifdef MAC_SECURITY_ZIP
		if ((transmitFrame->mpdu[1] & FCF_SECURITY_ENABLED) == FCF_SECURITY_ENABLED) {
			MCPS_DataReq_t pmdr = {0};
            bool retVal = false;

			retVal = BuildSecMcpsDataFrame(&pmdr, transmitFrame);
            (void)retVal;

			if (pmdr.SecurityLevel > 0U) {
				/* Secure the Frame */
				MAC_Retval_t buildSec = MAC_Secure(transmitFrame,	\
						transmitFrame->macPayload,
						&pmdr);

				if (MAC_SUCCESS != buildSec) {
					/* The MAC Data Payload is encrypted
					 * based on the security level. */
					MAC_GenMCPSDataConf((buffer_t *)msg,
							(uint8_t)buildSec,
			#ifdef ENABLE_TSTAMP
							mdr.msduHandle,
							0);
			#else
							mdr.msduHandle);
			#endif  /* ENABLE_TSTAMP */

					return;
				}
			}
		}
#endif

		/* In Non beacon build the frame is sent with unslotted CSMA-CA.
		**/
		phyTxStatus = sendFrame(transmitFrame, CSMA_UNSLOTTED, true);

		if (PHY_SUCCESS == phyTxStatus) {
			MAKE_MAC_BUSY();
		} else {
			/* Transmission to TAL failed, generate confirmation
			 * message. */
			MAC_GenMCPSDataConf((buffer_t *)msg,
					(uint8_t)MAC_CHANNEL_ACCESS_FAILURE,
#ifdef ENABLE_TSTAMP
					mdr.msduHandle,
					0);
#else
					mdr.msduHandle);
#endif  /* ENABLE_TSTAMP */

			/* Set radio to sleep if allowed */
			MAC_SleepTrans();
		}
	}
} /* MAC_MCPS_DataRequest() */

/**
 * @brief Processes data frames
 *
 * This function processes the data frames received and sends
 * mcps_data_indication to the NHLE.
 *
 * @param buf_ptr Pointer to receive buffer of the data frame
 */
void MAC_ProcessDataFrame(buffer_t *bufPtr)
{
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
	MCPS_DataInd_t *mdi
		= (MCPS_DataInd_t *)MAC_BUFFER_POINTER(bufPtr);

	if (macParseData.macPayloadLength == 0U) {
		/*
		 * A null frame is neither indicated to the higher layer
		 * nor checked for for frame pending bit set, since
		 * null data frames with frame pending bit set are nonsense.
		 */

		/* Since no indication is generated, the frame buffer is
		 * released. */
		bmm_buffer_free(bufPtr);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
	} else {
		/* Build the MLME_Data_indication parameters. */
		mdi->DSN = macParseData.sequenceNumber;
#ifdef ENABLE_TSTAMP
		mdi->Timestamp = macParseData.timeStamp;
#endif /* ENABLE_TSTAMP */

		/* Source address info */
		mdi->SrcAddrMode = macParseData.srcAddrMode;
		mdi->SrcPANId = macParseData.srcPanid;

		if (FCF_LONG_ADDR == mdi->SrcAddrMode ||
				FCF_SHORT_ADDR == mdi->SrcAddrMode) {
			mdi->SrcAddr = 0;
			if (FCF_SHORT_ADDR == mdi->SrcAddrMode) {
				ADDR_COPY_DST_SRC_16(mdi->SrcAddr,
						macParseData.srcAddr.shortAddress);
			} else if (FCF_LONG_ADDR == mdi->SrcAddrMode) {
				ADDR_COPY_DST_SRC_64(mdi->SrcAddr,
						macParseData.srcAddr.longAddress);
			}
            else
            {
                // do nothing
            }
		} else {
			/*
			 * Even if the Source address mode is zero, and the
			 * source address
			 * informationis is not present, the values are cleared
			 * to prevent
			 * the providing of trash information.
			 */
			mdi->SrcPANId = 0;
			mdi->SrcAddr = 0;
		}

		/* Start of duplicate detection. */
		if ((mdi->DSN == macLastDsn) &&
				(mdi->SrcAddr == macLastSrcAddr)
				) {
			/*
			 * This is a duplicated frame.
			 * It will not be indicated to the next higher layer,
			 * but nevetheless the frame pending bit needs to be
			 * checked and acted upon.
			 */

			/* Since no indication is generated, the frame buffer is
			 * released. */
			bmm_buffer_free(bufPtr);
		} else {
			/* Generate data indication to next higher layer. */

			/* Store required information for perform subsequent
			 * duplicate detections.
			 */
			macLastDsn = mdi->DSN;
			macLastSrcAddr = mdi->SrcAddr;

			/* Destination address info */
			mdi->DstAddrMode = macParseData.destAddrMode;

			/*
			 * Setting the address to zero is required for a short
			 * address
			 * and in case no address is included. Therefore the
			 * address
			 * is first always set to zero to reduce code size.
			 */
			mdi->DstAddr = 0;

			/*
			 * Setting the PAN-Id to the Destiantion PAN-Id is
			 * required
			 * for a both short and long address, but not in case no
			 * address
			 * is included. Therefore the PAN-ID is first always set
			 * to
			 * the Destination PAN-IDto reduce code size.
			 */
			mdi->DstPANId = macParseData.destPanid;
			if (FCF_LONG_ADDR == mdi->DstAddrMode) {
				ADDR_COPY_DST_SRC_64(mdi->DstAddr,
						macParseData.destAddr.longAddress);
			} else if (FCF_SHORT_ADDR == mdi->DstAddrMode) {
				ADDR_COPY_DST_SRC_16(mdi->DstAddr,
						macParseData.destAddr.shortAddress);
			} else {
				/*
				 * Even if the Destination address mode is zero,
				 * and the destination
				 * address information is not present, the
				 * values are cleared to
				 * prevent the providing of trash information.
				 * The Desintation address was already cleared
				 * above.
				 */
				mdi->DstPANId = 0;
			}

			mdi->mpduLinkQuality = macParseData.ppduLinkQuality;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
			mdi->SecurityLevel = macParseData.secCtrl.secLevel;
			mdi->KeyIdMode = macParseData.secCtrl.keyIdMode;
			mdi->KeyIndex = macParseData.keyId[0];
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006)  */

			mdi->msduLength = macParseData.macPayloadLength;

			/* Set pointer to data frame payload. */
			mdi->msdu
				= macParseData.macPayloadData.data.payload;

			mdi->cmdcode = MCPS_DATA_INDICATION;

			/* Append MCPS data indication to MAC-NHLE queue */
			qmmStatus = qmm_queue_append(&macNhleQueue, bufPtr);
            (void)qmmStatus;

		} /* End of duplicate detection. */

		/* Continue with checking the frame pending bit in the received
		 * data frame.
		 */
#if (MAC_INDIRECT_DATA_BASIC == 1)
		if ((macParseData.fcf & FCF_FRAME_PENDING) == FCF_FRAME_PENDING) {
#if (MAC_START_REQUEST_CONFIRM == 1)

			/* An node that is not PAN coordinator may poll for
			 * pending data. */
			if (MAC_PAN_COORD_STARTED != macState)
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */
			{
				AddressField_t srcAddr;
                bool retVal = false;

				/* Build command frame due to implicit poll
				 * request */

				/*
				 * No explicit destination address attached, so
				 * use current
				 * values of PIB attributes macCoordShortAddress
				 * or
				 * macCoordExtendedAddress.
				 */

				/*
				 * This implicit poll (i.e. corresponding data
				 * request
				 * frame) is to be sent to the same node that we
				 * have received
				 * this data frame. Therefore the source address
				 * information
				 * from this data frame needs to be extracted,
				 * and used for the
				 * data request frame appropriately.
				 * Use this as destination address explicitly
				 * and
				 * feed this to the function
				 * MAC_BuildAndTxDataReq
				 */
				if (FCF_SHORT_ADDR ==
						macParseData.srcAddrMode) {
					ADDR_COPY_DST_SRC_16(
							srcAddr.shortAddress,
							macParseData.srcAddr.shortAddress);

					retVal = MAC_BuildAndTxDataReq(false,
							false,
							FCF_SHORT_ADDR,
							(AddressField_t *)&(
								srcAddr),
							macParseData.srcPanid);
				} else if (FCF_LONG_ADDR ==
						macParseData.srcAddrMode) {
					ADDR_COPY_DST_SRC_64(
							srcAddr.longAddress,
							macParseData.srcAddr.longAddress);

					retVal = MAC_BuildAndTxDataReq(false,
							false,
							FCF_LONG_ADDR,
							(AddressField_t *)&(
								srcAddr),
							macParseData.srcPanid);
				} else {
					retVal = MAC_BuildAndTxDataReq(false, false,
							0, NULL, 0);
				}
                (void)retVal;
			}
		} else
#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */
		{
			/* Frame pending but was not set, so no further action
			 * required. */
			/* Set radio to sleep if allowed */
			MAC_SleepTrans();
		} /* if (macParseData.fcf & FCF_FRAME_PENDING) */
	} /* (macParseData.payload_length == 0) */
    WPAN_PostTask();
} /* MAC_ProcessDataFrame() */

/*
 * @brief Builds MCPS data frame
 *
 * This function builds the data frame.
 *
 * @param pmdr Request parameters
 * @param buffer Pointer to transmission frame
 * @param indirect Transmission is direct or indirect
 *
 * @return Status of the attempt to build the data frame
 */
static MAC_Retval_t BuildDataFrame(MCPS_DataReq_t *pmdr,
		MAC_FrameInfo_t *frame)
{
	uint8_t frameLen;
	uint8_t *framePtr;
	uint16_t fcf = 0;
    PibValue_t pib_value;
    PHY_Retval_t pibStatus = PHY_FAILURE;
	frameLen = pmdr->msduLength +
			2U + /* Add 2 octets for FCS */
			3U; /* 3 octets DSN and FCF */

	/*
	 * Payload pointer points to data, which was already been copied
	 * into buffer
	 */
	framePtr = (uint8_t *)frame +
			LARGE_BUFFER_SIZE -
			pmdr->msduLength - 2; /* Add 2 octets for FCS. */

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	uint8_t *macPayloadPtr = framePtr;

	/*
	 * Note: The value of the payload_length parameter will be updated
	 *       if security needs to be applied.
	 */
	if (pmdr->SecurityLevel > 0U) {
		MAC_Retval_t buildSec = MAC_BuildAuxSecHeader(&framePtr, pmdr,
				&frameLen);

		if (MAC_SUCCESS != buildSec) {
			return (buildSec);
		}
	}
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/*
	 * Set Source Address.
	 */
	if (FCF_SHORT_ADDR == pmdr->SrcAddrMode) {
		framePtr -= 2;
		frameLen += 2U;
        pibStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_value);

        convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);
        
	} else if (FCF_LONG_ADDR == pmdr->SrcAddrMode) {
		framePtr -= 8;
		frameLen += 8U;
        pibStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);

        convert_64_bit_to_byte_array(pib_value.pib_value_64bit, framePtr);
        
	}
    else
    {
        // do nothing
    }

	/* Shall the Intra-PAN bit set? */
    pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
	if ((pib_value.pib_value_16bit == pmdr->DstPANId) &&
			(FCF_NO_ADDR != pmdr->SrcAddrMode) &&
			(FCF_NO_ADDR != pmdr->DstAddrMode)) {
		/*
		 * Both address are present and both PAN-Ids are identical.
		 * Set intra-PAN bit.
		 */
		fcf |= FCF_PAN_ID_COMPRESSION;
	} else if (FCF_NO_ADDR != pmdr->SrcAddrMode) {
		/* Set Source PAN-Id. */
		framePtr -= 2;
		frameLen += 2U;
        convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);
        
	}
    else{
        //do nothing
    }

	/* Set the Destination Addressing fields. */
	if (FCF_NO_ADDR != pmdr->DstAddrMode) {
		if (FCF_SHORT_ADDR == pmdr->DstAddrMode) {
			framePtr -= 2;
			frameLen += 2U;
			convert_16_bit_to_byte_address(((uint16_t)(pmdr->DstAddr)), framePtr);
		} else {
			framePtr -= 8;
			frameLen += 8U;
			convert_64_bit_to_byte_array(pmdr->DstAddr, framePtr);
		}

		framePtr -= 2;
		frameLen += 2U;
		convert_16_bit_to_byte_array(pmdr->DstPANId, framePtr);
	}

	/* Set DSN. */
	framePtr--;
	*framePtr = macPib.mac_DSN++;

	/* Set the FCF. */
#ifdef TEST_HARNESS

	/*
	 * When performing tests this PIB attribute defaults to 1
	 * (i.e. a standard data frame). If not set to 1, it is used as a
	 * (supposedly illegal) frame type to fill into the frame type
	 * field of the data frame's FCF. In effect, valid (illegal)
	 * values range from 4 through 7.
	 */
	if (macPib.privateIllegalFrameType != 1) {
		fcf |= FCF_SET_FRAMETYPE(macPib.privateIllegalFrameType);
	} else
#endif /* TEST_HARNESS */
	{
		fcf |= FCF_SET_FRAMETYPE(FCF_FRAMETYPE_DATA);
	}

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	if (pmdr->SecurityLevel > 0U) {
		fcf |= FCF_SECURITY_ENABLED | FCF_FRAME_VERSION_2006;
	}
#endif

	if ((pmdr->TxOptions & WPAN_TXOPT_ACK) == WPAN_TXOPT_ACK) {
		fcf |= FCF_ACK_REQUEST;
	}

	/*
	 * 802.15.4-2006 section 7.1.1.1.3:
	 *
	 * If the msduLength parameter is greater than aMaxMACSafePayloadSize,
	 * the MAC sublayer will set the Frame Version subfield of the
	 * Frame Control field to one.
	 */
	if (pmdr->msduLength > aMaxMACSafePayloadSize) {
		fcf |= FCF_FRAME_VERSION_2006;
	}

	/* Set FCFs address mode */
	fcf |= FCF_SET_SOURCE_ADDR_MODE(pmdr->SrcAddrMode);
	fcf |= FCF_SET_DEST_ADDR_MODE(pmdr->DstAddrMode);

	framePtr -= 2;
	convert_spec_16_bit_to_byte_array(fcf, framePtr);

	/*
	 * In case the frame gets too large, return error.
	 */
	if (frameLen > aMaxPHYPacketSize) {
		return MAC_FRAME_TOO_LONG;
	}

	/* First element shall be length of PHY frame. */
	framePtr--;
	*framePtr = frameLen;

	/* Finished building of frame. */
	frame->mpdu = framePtr;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	frame->macPayload = macPayloadPtr;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
    (void)pibStatus;

	return MAC_SUCCESS;
} /* BuildDataFrame() */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Start the Persistence timer for indirect data
 *
 * This function starts the persistence timer for handling of indirect
 * data.
 */
void MAC_StartPersistenceTimer(void)
{
	PAL_Status_t status = PAL_FAILURE;
	/* Interval of indirect data persistence timer */
	uint32_t persistence_int_us;

	/*
	 * This is a nonbeacon build. The timeout interval for the indirect data
	 * persistence timer is based on the define below.
	 */

	persistence_int_us
		= PHY_CONVERT_SYMBOLS_TO_US(GET_BEACON_INTERVAL_TIME(
			BO_USED_FOR_MAC_PERS_TIME));

	/* Start the indirect data persistence timer now. */
	status = PAL_TimerStart(Timer_DataPersistence,
			persistence_int_us,
			TIMEOUT_RELATIVE,
			&MAC_Timer_PersistenceCb,
			NULL,CALLBACK_SINGLE);

	if (PAL_SUCCESS != status) {
		/* Got to the persistence timer callback function immediately.
		**/
		MAC_Timer_PersistenceCb(NULL);
	}
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Handles timeout of indirect data persistence timer
 *
 * This function is a callback function of the timer started for checking
 * the mac persistence time of indirect data in the queue.
 *
 * @param callbackParameter Callback parameter
 */
static void MAC_Timer_PersistenceCb(void *callbackParameter)
{
	/* Decrement the persistence time for indirect data. */
	HandlePersistenceTimeDecrement();

	if (indirectDataQueue.size > 0U) {
		/* Restart persistence timer. */
		MAC_StartPersistenceTimer();
	}
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Handles the decrement of the persistence time
 *
 * Handles the decrement of the persistance time of each indirect data frame
 * in the indirect queue.
 * If the persistance time of any indirect data reduces to zero, the
 * frame is removed from the indirect queue and
 * a confirmation for that indirect data is sent with the status
 * transaction expired.
 */
static void HandlePersistenceTimeDecrement(void)
{
	search_t findBuf;
    buffer_t *buf;

	/*
	 * This callback function traverses through the indirect queue and
	 * decrements the persistence time for each data frame.
	 */
	findBuf.criteria_func = DecrementPersistenceTime;
    findBuf.handle = NULL;

	/*
	 * At the end of this function call (qmm_queue_read), the indirect data
	 * will be updated with the decremented persistence time.
	 */
	buf = qmm_queue_read(&indirectDataQueue, &findBuf);

	/*
	 * Once we have updated the persistence timer, any frame with a
	 * persistence
	 * time of zero needs to be removed from the indirect queue.
	 */
	buffer_t *buffer_persistent_zero = NULL;

	/*
	 * This callback function traverses through the indirect queue and
	 * searches for a data frame with persistence time equal to zero.
	 */
	findBuf.criteria_func = CheckPersistenceTimeZero;

	do {
		buffer_persistent_zero = qmm_queue_remove(&indirectDataQueue,
				&findBuf);

		if (NULL != buffer_persistent_zero) {
			HandleExpPersistenceTimer(buffer_persistent_zero);
		}
	} while (NULL != buffer_persistent_zero);
    (void)buf;
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Decrements the persistence time of an indirect data frame
 *
 * @param buf_ptr Pointer to the indirect data in the indirect queue
 * @param handle Callback parameter
 *
 * @return 0 to traverse through the full indirect queue
 *
 */
static uint8_t DecrementPersistenceTime(void *bufPtr, void *handle)
{
	MAC_FrameInfo_t *frame = (MAC_FrameInfo_t *)bufPtr;

	/*
	 * In case the frame is currently in the process of being transmitted,
	 * the persistence time is not decremented, to avoid the expiration of
	 * the persistence timer during transmission.
	 * Once the transmission is done (and was not successful),
	 * the frame will still be in the indirect queue and the persistence
	 * timer will be decremented again.
	 */
	if (!frame->indirectInTransit) {
		/* Decrement the persistence time for this indirect data frame.
		**/
		frame->persistenceTime--;
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Checks for indirect data with persistence time zero
 *
 * This callback function checks whether the persistence time
 * of the indirect data is set to zero.
 *
 * @param buf Pointer to indirect data buffer
 *
 * @return 1 if extended address passed matches with the destination
 * address of the indirect frame , 0 otherwise
 */
static uint8_t CheckPersistenceTimeZero(void *bufPtr, void *handle)
{
	MAC_FrameInfo_t *frame = (MAC_FrameInfo_t *)bufPtr;

	/* Frame shall not be in transmission. */
	if (!frame->indirectInTransit) {
		if (frame->persistenceTime== 0U) {
			return 1;
		}
	}

	handle = handle; /* Keep compiler happy. */

	return 0;
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/*
 * @brief Generates notification for expired transaction
 *
 * This function generates the confirmation for those indirect data buffers
 * whose persistence time has reduced to zero.
 *
 * @param buf_ptr Pointer to buffer of indirect data whose persistance time
 * has reduced to zero
 */
static void HandleExpPersistenceTimer(buffer_t *bufPtr)
{
	MAC_FrameInfo_t *trans_frame = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(bufPtr);

	/*
	 * The frame should never be in transmission while this function
	 * is called.
	 */

	switch (trans_frame->msgType) {
#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	case ASSOCIATIONRESPONSE:
	{
		MAC_MLME_CommStatus((uint8_t)MAC_TRANSACTION_EXPIRED,
				bufPtr);
	}
	break;
#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
	case DISASSOCIATIONNOTIFICATION:

		/*
		 * Prepare disassociation confirm message after transmission of
		 * the disassociation notification frame.
		 */
		MAC_PrepDisassocConf((buffer_t *)bufPtr,
				(uint8_t)MAC_TRANSACTION_EXPIRED);
		break;
#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

	case MCPS_MESSAGE:
	{
		MAC_GenMCPSDataConf((buffer_t *)bufPtr,
				(uint8_t)MAC_TRANSACTION_EXPIRED,
#ifdef ENABLE_TSTAMP
				trans_frame->msduHandle,
				0);
#else
				trans_frame->msduHandle);
#endif  /* ENABLE_TSTAMP */
	}
	break;

	default:
		/* Nothing to be done here. */
		break;
	}
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_FFD == 1))

/*
 * @brief Purges a buffer corresponding to a MSDU handle
 *
 * This function tries to purge a given msdu by finding its msdu handle.
 * If the handle is found, that buffer is freed up for further use.
 * This routine will typically be called from the mlme_purge_request routine.
 *
 * @param msdu_handle MSDU handle
 *
 * @return True if the MSDU handle is found in the indirect queue
 *         and removed successfully, false otherwise.
 */
static bool MAC_BufferPurge(uint8_t msdu_handle)
{
	buffer_t *bufPtr;
	search_t findBuf;
	uint8_t handle = msdu_handle;

	/*
	 * Callback function  for searching the data having MSDU handle
	 * given by purge request
	 */
	findBuf.criteria_func = CheckMsduHandleCb;

	/* Update the MSDU handle to be searched */
	findBuf.handle = &handle;

	/* Remove from indirect queue if the short address matches */
	bufPtr = qmm_queue_remove(&indirectDataQueue, &findBuf);

	if (NULL != bufPtr) {
		/* Free the buffer allocated, after purging */
		bmm_buffer_free((buffer_t *)bufPtr);

		return true;
	}

	/* No data available in the indirect queue with the MSDU handle. */
	return false;
}

/**
 * @brief Processes a MCPS-PURGE.request primitive
 *
 * This functions processes a MCPS-PURGE.request from the NHLE.
 * The MCPS-PURGE.request primitive allows the next higher layer
 * to purge an MSDU from the transaction queue.
 * On receipt of the MCPS-PURGE.request primitive, the MAC sublayer
 * attempts to find in its transaction queue the MSDU indicated by the
 * msduHandle parameter. If an MSDU matching the given handle is found,
 * the MSDU is discarded from the transaction queue, and the MAC
 * sublayer issues the MCPSPURGE. confirm primitive with a status of
 * MAC_SUCCESS. If an MSDU matching the given handle is not found, the MAC
 * sublayer issues the MCPS-PURGE.confirm primitive with a status of
 * INVALID_HANDLE.
 *
 * @param msg Pointer to the MCPS-PURGE.request parameter
 */
void MAC_MCPS_PurgeRequest(void *msg)
{
    qmm_status_t qmmStatus = QMM_QUEUE_FULL;
	MCPS_PurgeReq_t *mpr
		= (MCPS_PurgeReq_t *)MAC_BUFFER_POINTER(((buffer_t *)msg));

	MCPS_PurgeConf_t *mpc = (MCPS_PurgeConf_t *)((void *)mpr);

	uint8_t purge_handle = mpr->msduHandle;

	/* Update the purge confirm structure */
	mpc->cmdcode = MCPS_PURGE_CONFIRM;
	mpc->msduHandle = purge_handle;

	if (MAC_BufferPurge(purge_handle)) {
		mpc->status = (uint8_t)MAC_SUCCESS;
	} else {
		mpc->status = (uint8_t)MAC_INVALID_HANDLE;
	}

	qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)msg);
    WPAN_PostTask();
    (void)qmmStatus;
}

/*
 * @brief Checks whether MSDU handle matches
 *
 * @param buf Pointer to indirect data buffer
 * @param handle MSDU handle to be searched
 *
 * @return 1 if MSDU handle matches with the indirect data, 0 otherwise
 */
static uint8_t CheckMsduHandleCb(void *buf, void *handle)
{
	MAC_FrameInfo_t *frame = (MAC_FrameInfo_t *)buf;
	uint8_t msdu;

	msdu = *((uint8_t *)handle);

	/* Compare the MSDU handle */
	if (frame->msduHandle == msdu) {
		return 1;
	}

	return 0;
}

#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_FFD == 1)) */
/* EOF */
