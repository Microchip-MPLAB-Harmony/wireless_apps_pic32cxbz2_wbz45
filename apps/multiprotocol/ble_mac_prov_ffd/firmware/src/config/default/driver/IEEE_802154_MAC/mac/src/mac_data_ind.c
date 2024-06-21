/*******************************************************************************
  MAC data Ind Source

  File Name:
    mac_data_ind.c
  Summary:
    Incoming frame handling
  Description:
    This file implements incoming frame handling in the MAC.

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

/* === Macros =============================================================== */

/*
 * Mask of the GTS descriptor counter
 */
#define GTS_DESCRIPTOR_COUNTER_MASK     (7U)

/*
 * Extract the PAN Coordinator bit from the Superframe Spec.
 */
#define GET_PAN_COORDINATOR(spec)      (((spec) & 0x4000U) >> 14U)

#if (MAC_PAN_ID_CONFLICT_NON_PC == 1)

/*
 * PAN-Id conflict notification payload length
 */
#define PAN_ID_CONFLICT_PAYLOAD_LEN     (1)
#endif  /* (MAC_PAN_ID_CONFLICT_NON_PC == 1) */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

static bool ParseMpdu(MAC_FrameInfo_t *rxFramePtr);
static bool ProcessDataIndNotTransient(buffer_t *b_ptr,
		MAC_FrameInfo_t *f_ptr);

#if (MAC_SCAN_SUPPORT == 1)
static bool ProcessDataIndScanning(buffer_t *b_ptr);

#endif /* (MAC_SCAN_SUPPORT == 1) */

#ifdef PROMISCUOUS_MODE
static void PromModeRxFrame(buffer_t *b_ptr, MAC_FrameInfo_t *f_ptr);

#endif  /* PROMISCUOUS_MODE */

#if (MAC_PAN_ID_CONFLICT_AS_PC == 1)
static void CheckForPanIdConflictAsPc(bool inScan);

#endif  /* (MAC_PAN_ID_CONFLICT_AS_PC == 1) */

#if (MAC_PAN_ID_CONFLICT_NON_PC == 1)
static void CheckForPanIdConflictNonPc(bool inScan);
static bool TxPanIdConfNotif(void);

#endif  /* (MAC_PAN_ID_CONFLICT_NON_PC == 1) */

/* === Implementation ====================================================== */

/**
 * @brief Depending on received frame the appropriate function is called
 *
 * @param msg Pointer to the buffer header.
 */
void MAC_ProcessPhyDataInd(void *msg)
{
	buffer_t *bufPtr = (buffer_t *)msg;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
    PAL_Status_t palStatus;
	MAC_FrameInfo_t *frameptr = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(bufPtr);
	bool processedPhyDataIndication = false;

	macParseData.mpduLength = frameptr->mpdu[0];

	/* First extract LQI since this is already needed in Promiscuous Mode.
	**/
	macParseData.ppduLinkQuality
		= frameptr->mpdu[macParseData.mpduLength + LQI_LEN];

#ifdef PROMISCUOUS_MODE
    PibValue_t pib_value;
    PHY_Retval_t pibStatus;
    pibStatus = PHY_PibGet(macPromiscuousMode, (uint8_t *)&pib_value);
	if (pib_value.pib_value_8bit) {
		/*
		 * In promiscuous mode all received frames are forwarded to the
		 * higher layer or application my means of MCPS_DATA.indication
		 * primitives.
		 */
		PromModeRxFrame(bufPtr, frameptr);
        (void)pibStatus;
		return;
	}

#endif  /* PROMISCUOUS_MODE */

	if (!ParseMpdu(frameptr)) {
		/* Frame parsing failed */
		bmm_buffer_free(bufPtr);
		return;
	}

	/* Check if the MAC is busy processing the previous requests */
	if (macBusy) {
		/*
		 * If MAC has to process an incoming frame that requires a
		 * response
		 * (i.e. beacon request and data request) then process this
		 * operation
		 * once the MAC has become free. Put the request received back
		 * into the
		 * MAC internal event queue.
		 */
		if (FCF_FRAMETYPE_MAC_CMD == macParseData.frameType) {
			if (DATAREQUEST == ((FrameMsgtype_t)macParseData.macCommand) ||
					BEACONREQUEST == ((FrameMsgtype_t)macParseData.macCommand)) {
				qmmStatus = qmm_queue_append(&phyMacQueue, bufPtr);
                WPAN_PostTask();
                (void)qmmStatus;
				return;
			}
		}
	}

	switch (macPollState) {
	case MAC_POLL_IDLE:

		/*
		 * We are in no transient state.
		 * Now are either in a non-transient MAC state or scanning.
		 */
		if (MAC_SCAN_IDLE == macScanState) {
			/*
			 * Continue with handling the "real" non-transient MAC
			 * states now.
			 */
			processedPhyDataIndication
				= ProcessDataIndNotTransient(
					bufPtr, frameptr);
		}

#if (MAC_SCAN_SUPPORT == 1)
		else {
			/* Scanning is ongoing. */
			processedPhyDataIndication
				= ProcessDataIndScanning(bufPtr);
		}
#endif /* (MAC_SCAN_SUPPORT == 1) */
		break;

#if (MAC_INDIRECT_DATA_BASIC == 1)

	/*
	 * This is the 'wait for data' state after either
	 * explicit poll or implicit poll.
	 */
	case MAC_POLL_EXPLICIT:
	case MAC_POLL_IMPLICIT:

		/*
		 * Function MAC_ProcessDataResponse() resets the
		 * MAC poll state.
		 */
		MAC_ProcessDataResponse();

		switch (macParseData.frameType) {
		case FCF_FRAMETYPE_MAC_CMD:
		{
			switch ((FrameMsgtype_t)macParseData.macCommand) {
#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
			case ASSOCIATIONREQUEST:
				MAC_ProcessAssociateRequest(bufPtr);
				processedPhyDataIndication = true;
				break;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
			case DISASSOCIATIONNOTIFICATION:
			{
				MAC_ProcessDisassociateNotification(bufPtr);

				processedPhyDataIndication = true;

				/*
				 * Device needs to scan for networks again,
				 * go into idle mode and reset variables
				 */
				MAC_IdleTrans();
			}
			break;
#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

			case DATAREQUEST:
#if (MAC_INDIRECT_DATA_FFD == 1)
				if (indirectDataQueue.size > 0U) {
					MAC_ProcessDataRequest(bufPtr);
					processedPhyDataIndication = true;
				} else {
					MAC_HandleTxNullDataFrame();
				}

#endif  /*  (MAC_INDIRECT_DATA_FFD == 1) */
				break;

			case PANIDCONFLICTNOTIFICAION:
				break;

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
			case ORPHANNOTIFICATION:
				MAC_ProcessOrphanNotification(bufPtr);
				processedPhyDataIndication = true;
				break;
#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
			case BEACONREQUEST:
				if (MAC_COORDINATOR == macState) {
					/*
					 * Only a Coordinator can both poll and
					 * AND answer beacon request frames.
					 * PAN Coordinators do not poll;
					 * End devices do not answer beacon
					 * requests.
					 */
					MAC_ProcessBeaconRequest(bufPtr);
					processedPhyDataIndication = true;
				}
				break;
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_SYNC_LOSS_INDICATION == 1)
			case COORDINATORREALIGNMENT:

				/*
				 * Received coordinator realignment frame for
				 * entire PAN.
				 */
				MAC_ProcessCoordRealign(bufPtr);
				processedPhyDataIndication = true;
				break;
#endif  /* (MAC_SYNC_LOSS_INDICATION == 1) */

			default:
                processedPhyDataIndication = false;
                break;
			}
		}
		break; /* case FCF_FRAMETYPE_MAC_CMD: */

		case FCF_FRAMETYPE_DATA:
			MAC_ProcessDataFrame(bufPtr);
			processedPhyDataIndication = true;
			break;

		default:
            processedPhyDataIndication = false;
            break;
		}
		break;
		/* MAC_POLL_EXPLICIT, MAC_POLL_IMPLICIT */
#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	case MAC_AWAIT_ASSOC_RESPONSE:

		/*
		 * We are either expecting an association reponse frame
		 * or a null data frame.
		 */
		if ((FCF_FRAMETYPE_MAC_CMD == macParseData.frameType) &&
				(ASSOCIATIONRESPONSE == (FrameMsgtype_t)macParseData.macCommand)
				) {
			/* This is the expected association response frame. */
			palStatus = PAL_TimerStop(Timer_PollWaitTime);
			MAC_ProcessAssociateResponse(bufPtr);
			processedPhyDataIndication = true;
		} else if (FCF_FRAMETYPE_DATA == macParseData.frameType) {
			MAC_ProcessDataFrame(bufPtr);
			processedPhyDataIndication = true;
		}        
        else{
            // do nothing
        }
        (void)palStatus;
		break;
		/*  MAC_AWAIT_ASSOC_RESPONSE */
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

	default:
        processedPhyDataIndication = false;
        break;
	}

	/* If message is not processed */
	if (!processedPhyDataIndication) {
		bmm_buffer_free(bufPtr);
	}
} /* MAC_ProcessPhyDataInd() */

#if (MAC_SCAN_SUPPORT == 1)

/**
 * @brief Continues processing a data indication from the TAL for
 *        scanning states of the MAC (macScanState == MAC_SCAN_IDLE).
 *
 * @param b_ptr Pointer to the buffer header.
 *
 * @return bool True if frame has been processed, or false otherwise.
 */
static bool ProcessDataIndScanning(buffer_t *b_ptr)
{
	bool processedInScanning = false;
    PAL_Status_t palStatus;

	/*
	 * We are in a scanning process now (mac_scan_state is not
	 * MAC_SCAN_IDLE),
	 * so continue with the specific scanning states.
	 */
	switch (macScanState) {
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	/* Energy Detect scan */
	case MAC_SCAN_ED:

		/*
		 * Ignore all frames received while performing ED measurement,
		 * or while performing CCA.
		 */
		b_ptr = b_ptr; /* Keep compiler happy. */
		break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

#if ((MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) ||	\
		(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1))
	/* Active scan or passive scan */
	case MAC_SCAN_ACTIVE:
	case MAC_SCAN_PASSIVE:
		if (FCF_FRAMETYPE_BEACON == macParseData.frameType) {
#if (MAC_PAN_ID_CONFLICT_AS_PC == 1)
			/* PAN-Id conflict detection as PAN-Coordinator. */
			if (MAC_PAN_COORD_STARTED == macState) {
				/* Node is currently scanning. */
				CheckForPanIdConflictAsPc(true);
			}

#endif  /* (MAC_PAN_ID_CONFLICT_AS_PC == 1) */
#if (MAC_PAN_ID_CONFLICT_NON_PC == 1)
			if ((macPib.mac_AssociatedPANCoord == 1U) &&
					((MAC_ASSOCIATED == macState) ||
					(MAC_COORDINATOR == macState))
					) {
				CheckForPanIdConflictNonPc(true);
			}

#endif  /* (MAC_PAN_ID_CONFLICT_NON_PC == 1) */
			MAC_ProcessBeaconFrame(b_ptr);
			processedInScanning = true;
		}
		break;
#endif /* ((MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
	/* Orphan scan */
	case MAC_SCAN_ORPHAN:
		if (FCF_FRAMETYPE_MAC_CMD == macParseData.frameType &&
				COORDINATORREALIGNMENT == (FrameMsgtype_t)macParseData.macCommand) {
			/*
			 * Received coordinator realignment frame in the middle
			 * of
			 * an orphan scan.
			 */
			palStatus = PAL_TimerStop(Timer_ScanDuration);

			MAC_ProcessOrphanRealign(b_ptr);
			processedInScanning = true;
		}
		break;
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

	default:
        processedInScanning = false;
        break;
	}

    (void)palStatus;
	return (processedInScanning);
}

#endif /* (MAC_SCAN_SUPPORT == 1) */

/**
 * @brief Continues processing a data indication from the TAL for
 *        non-polling and non-scanning states of the MAC
 *        (macPollState == MAC_POLL_IDLE, macScanState == MAC_SCAN_IDLE).
 *
 * @param b_ptr Pointer to the buffer header.
 * @param f_ptr Pointer to the MAC_FrameInfo_t structure.
 *
 * @return bool True if frame has been processed, or false otherwise.
 */
static bool ProcessDataIndNotTransient(buffer_t *b_ptr, MAC_FrameInfo_t *f_ptr)
{
	bool processedInNotTransient = false;

	/*
	 * We are in MAC_POLL_IDLE and MAC_SCAN_IDLE now,
	 * so continue with the real MAC states.
	 */
	switch (macState) {
#if (MAC_START_REQUEST_CONFIRM == 1)
	case MAC_PAN_COORD_STARTED:
	{
		switch (macParseData.frameType) {
		case FCF_FRAMETYPE_MAC_CMD:
		{
			switch ((FrameMsgtype_t)macParseData.macCommand) {
#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
			case ASSOCIATIONREQUEST:
				MAC_ProcessAssociateRequest(b_ptr);
				processedInNotTransient = true;
				break;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
			case DISASSOCIATIONNOTIFICATION:
				MAC_ProcessDisassociateNotification(b_ptr);
				processedInNotTransient = true;
				break;
#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)
			case DATAREQUEST:
				if (indirectDataQueue.size > 0U) {
					MAC_ProcessDataRequest(b_ptr);
					processedInNotTransient = true;
				} else {
					MAC_HandleTxNullDataFrame();
				}
				break;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
			case ORPHANNOTIFICATION:
				MAC_ProcessOrphanNotification(b_ptr);
				processedInNotTransient = true;
				break;
#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

			case BEACONREQUEST:
				MAC_ProcessBeaconRequest(b_ptr);
				processedInNotTransient = true;
				break;

#if (MAC_PAN_ID_CONFLICT_AS_PC == 1)
			case PANIDCONFLICTNOTIFICAION:
				MAC_SyncLoss((uint8_t)MAC_PAN_ID_CONFLICT);
				break;

#endif  /* (MAC_PAN_ID_CONFLICT_AS_PC == 1) */

			default:
                processedInNotTransient = false;
                break;
			}
		}
		break;

		case FCF_FRAMETYPE_DATA:
			MAC_ProcessDataFrame(b_ptr);
			processedInNotTransient = true;
			break;

#if (MAC_PAN_ID_CONFLICT_AS_PC == 1)
		case FCF_FRAMETYPE_BEACON:
			/* PAN-Id conflict detection as PAN-Coordinator. */
			/* Node is not scanning. */
			CheckForPanIdConflictAsPc(false);
			break;
#endif  /* (MAC_PAN_ID_CONFLICT_AS_PC == 1) */

		default:
            processedInNotTransient = false;
            break;
		}
	}
	break;
		/* MAC_PAN_COORD_STARTED */
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

	case MAC_IDLE:
#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	case MAC_ASSOCIATED:
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */
#if (MAC_START_REQUEST_CONFIRM == 1)
	case MAC_COORDINATOR:
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */
		{
			/* Is it a Beacon from our parent? */
			switch (macParseData.frameType) {
			case FCF_FRAMETYPE_BEACON:
			{
				/* Check for PAN-Id conflict being NOT a PAN
				 * Corodinator. */
#if ((MAC_PAN_ID_CONFLICT_NON_PC == 1) && (MAC_ASSOCIATION_REQUEST_CONFIRM == \
				1))
				if ((macPib.mac_AssociatedPANCoord == 1U)&&
						(MAC_IDLE !=
						macState)) {
					CheckForPanIdConflictNonPc(false);
				}

#endif  /* ((MAC_PAN_ID_CONFLICT_NON_PC == 1) &&
					 *(MAC_ASSOCIATION_REQUEST_CONFIRM ==
					 * 1)) */
			}
			break;

			case FCF_FRAMETYPE_MAC_CMD:
				switch ((FrameMsgtype_t)macParseData.macCommand) {
#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
				case DISASSOCIATIONNOTIFICATION:
					MAC_ProcessDisassociateNotification(
							b_ptr);
					processedInNotTransient = true;

					if (MAC_ASSOCIATED == macState) {
						/*
						 * Device needs to scan for
						 * networks again,
						 * go into idle mode and reset
						 * variables
						 */
						MAC_IdleTrans();
					}
					break;
#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_SYNC_LOSS_INDICATION == 1)
				case COORDINATORREALIGNMENT:

					/*
					 * Received coordinator realignment
					 * frame from
					 * coordinator while NOT performing
					 * orphan scan.
					 */
					MAC_ProcessCoordRealign(b_ptr);
					processedInNotTransient = true;
					break;
#endif /* (MAC_SYNC_LOSS_INDICATION == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
				case BEACONREQUEST:
					if (MAC_COORDINATOR == macState) {
						/*
						 * Only Coordinators (no End
						 * devices) answer
						 * Beacon requests.
						 */
						MAC_ProcessBeaconRequest(b_ptr);
						processedInNotTransient
							= true;
					}
					break;
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
				case ASSOCIATIONREQUEST:
					MAC_ProcessAssociateRequest(b_ptr);
					processedInNotTransient = true;
					break;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)
				case DATAREQUEST:
					if (indirectDataQueue.size > 0U) {
						MAC_ProcessDataRequest(b_ptr);
						processedInNotTransient
							= true;
					} else {
						MAC_HandleTxNullDataFrame();
					}
					break;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
				case ORPHANNOTIFICATION:
					MAC_ProcessOrphanNotification(b_ptr);
					processedInNotTransient = true;
					break;

#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */
				default:
                    processedInNotTransient = false;
                    break;
				}
				break;

			case FCF_FRAMETYPE_DATA:
				MAC_ProcessDataFrame(b_ptr);
				processedInNotTransient = true;
				break;

			default:
                processedInNotTransient = false;
                break;
			}
		}
		break;
	/* MAC_IDLE, MAC_ASSOCIATED, MAC_COORDINATOR */

	default:
        processedInNotTransient = false;
        break;
	}

	f_ptr = f_ptr; /* Keep compiler happy. */

	return (processedInNotTransient);
}

/*
 * @brief Parse an MPDU
 *
 * This function parses an MPDU which got from data_indication,
 * and leaves the parse result in mac_parse_data.
 *
 * @param frameptr Pointer to frame received from TAL
 *
 * @return bool True if frame OK, or false if frame is invalid.
 */
static bool ParseMpdu(MAC_FrameInfo_t *rxFramePtr)
{
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	uint8_t payloadIndex[4] = {0};
#endif
	uint8_t payloadLoc = 0U;
	uint8_t tempByte;
	uint16_t fcf;
	uint8_t *tempFramePtr = &(rxFramePtr->mpdu[1]);
    bool retVal = true;
	/* temp_frame_ptr points now to first octet of FCF. */

	/* Extract the FCF. */
	fcf = convert_byte_array_to_16_bit(tempFramePtr);
	macParseData.fcf = fcf;
	tempFramePtr += 2;

	/* Extract the Sequence Number. */
	macParseData.sequenceNumber = *tempFramePtr++;

	/* Extract the complete address information from the MHR. */
	tempFramePtr += MAC_ExtractMhrAddrInfo(tempFramePtr);

	/*
	 * Note: temp_frame_ptr points now to the first octet of the MAC payload
	 * if available.
	 */

#if (!defined MAC_SECURITY_ZIP && !defined MAC_SECURITY_2006)
	if (fcf & FCF_SECURITY_ENABLED) {
		return false;
	}

#endif

	macParseData.frameType = (uint8_t)FCF_GET_FRAMETYPE(fcf);

	if (FCF_FRAMETYPE_MAC_CMD == macParseData.frameType) {
		macParseData.macCommand = *tempFramePtr;
	}

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	if ((fcf & FCF_SECURITY_ENABLED) == FCF_SECURITY_ENABLED) {
		MAC_Retval_t status;
        PHY_Retval_t pibStatus;
        qmm_status_t  qmmStatus;
        PibValue_t pib_value;
		status = MAC_Unsecure(&macParseData, &rxFramePtr->mpdu[1],
				tempFramePtr, payloadIndex);

		payloadLoc = payloadIndex[0];

		if (status != MAC_SUCCESS) {
			/* Generate MLME-COMM-STATUS.indication. */

			/*
			 * In order to not interfere with the regular flow of
			 * parsing the
			 * frame and buffer handling, a fresh buffer is seized
			 * for the
			 * MLME-COMM-STATUS.indication, which will be release at
			 * the API
			 * level.
			 */
			buffer_t *buffer_header = bmm_buffer_alloc(
					LARGE_BUFFER_SIZE);

			if (NULL == buffer_header) {
				/* Buffer is not available */
				return false;
			}

			/* Get the buffer body from buffer header */
			MLME_CommStatusInd_t *csi
				= (MLME_CommStatusInd_t *)MAC_BUFFER_POINTER(
					buffer_header);

			csi->cmdcode = MLME_COMM_STATUS_INDICATION;
            
            pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);

			csi->PANId = pib_value.pib_value_16bit;

			csi->SrcAddrMode = macParseData.srcAddrMode;
			csi->SrcAddr = macParseData.srcAddr.longAddress;

			csi->DstAddrMode = macParseData.destAddrMode;
			csi->DstAddr = macParseData.destAddr.longAddress;

			csi->status = (uint8_t)status;

			qmmStatus = qmm_queue_append(&macNhleQueue, buffer_header);
            WPAN_PostTask();

			/*
			 * Return false - this will lead to the release of the
			 * original buffer.
			 */
            (void)pibStatus;
            (void)qmmStatus;
			return false;
		}
	} else {
		macParseData.secCtrl.secLevel = 0;
	}

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/* temp_frame_ptr still points to the first octet of the MAC payload. */
	switch (macParseData.frameType) {
	case FCF_FRAMETYPE_BEACON:
		/* Get the Super frame specification */
		(void)memcpy(
				((uint8_t *)(&macParseData.macPayloadData.beaconData.superframeSpec)),
				&tempFramePtr[payloadLoc],
				sizeof(uint16_t));
		payloadLoc += (uint8_t)sizeof(uint16_t);

		/* Get the GTS specification */
		macParseData.macPayloadData.beaconData.gtsSpec
			= tempFramePtr[payloadLoc++];

		/*
		 * If the GTS specification descriptor count is > 0, then
		 * increase the index by the correct GTS field octet number
		 * GTS directions and GTS address list will not be parsed
		 */
		tempByte
			= (macParseData.macPayloadData.beaconData.gtsSpec
				&
				GTS_DESCRIPTOR_COUNTER_MASK);
		if (tempByte > 0U) {
			/* 1 octet GTS direction */
			payloadLoc += 1U + tempByte;
		}

		/* Get the Pending address specification */

		macParseData.macPayloadData.beaconData.pendingAddrSpec
			= tempFramePtr[payloadLoc++];
		{
			/*
			 * If the Pending address specification indicates that
			 * the number of
			 * short or long addresses is > 0, then get the short
			 * and/or
			 * long addresses
			 */
			uint8_t number_bytes_short_addr = NUM_SHORT_PEND_ADDR(
					macParseData.macPayloadData.beaconData.pendingAddrSpec);
			uint8_t number_bytes_long_addr = NUM_LONG_PEND_ADDR(
					macParseData.macPayloadData.beaconData.pendingAddrSpec);

			if ((number_bytes_short_addr > 0U) ||
					(number_bytes_long_addr > 0U)) {
				macParseData.macPayloadData.beaconData.
				pendingAddrList
					= &tempFramePtr[payloadLoc];
			}

			if (number_bytes_short_addr > 0U) {
				payloadLoc += (number_bytes_short_addr * (uint8_t)sizeof(uint16_t));
			}

			if (number_bytes_long_addr > 0U) {
				payloadLoc += (number_bytes_long_addr * (uint8_t)sizeof(uint64_t));
			}
		}

		/* Is there a beacon payload ? */
		if (macParseData.macPayloadLength > payloadLoc) {
			macParseData.macPayloadData.beaconData.
			beaconPayloadLen
				= macParseData.macPayloadLength -
					payloadLoc;
			/* Store pointer to received beacon payload. */
			macParseData.macPayloadData.beaconData.
			beaconPayload
				= &tempFramePtr[payloadLoc];
		} else {
			macParseData.macPayloadData.beaconData.
			beaconPayloadLen = 0;
		}

		break;

	case FCF_FRAMETYPE_DATA:
		if ((macParseData.macPayloadLength) > 0U) {
			/*
			 * In case the device got a frame with a corrupted
			 * payload
			 * length
			 */
			if (macParseData.macPayloadLength >=
					aMaxMACPayloadSize) {
				macParseData.macPayloadLength
					= aMaxMACPayloadSize;
			}

			/*
			 * Copy the pointer to the data frame payload for
			 * further processing later.
			 */
			macParseData.macPayloadData.data.payload
				= &tempFramePtr[payloadLoc];
		} else {
			macParseData.macPayloadLength = 0;
		}

		break;

	case FCF_FRAMETYPE_MAC_CMD:

		/*
		 * Leave the SDL path here.
		 *
		 * SDL would normally complete parsing the command, then
		 * fall back to the caller, which would eventually notice
		 * that an ACK needs to be sent (76(154), p. 328). Upon
		 * that, it would walk a lot of state transitions until it
		 * could eventually transmit the ACK frame. This is very
		 * likely to happen way too late to be in time for an ACK
		 * frame, and it's somewhat funny why that part of SDL
		 * would actually go all that way down as all other ACK
		 * frames are pushed out onto the queue really quickly,
		 * without taking care to await all the response message
		 * ping-pong first.
		 */
		payloadLoc = 1;

		switch ((FrameMsgtype_t)macParseData.macCommand) {
#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
		case ASSOCIATIONREQUEST:
			macParseData.macPayloadData.assocReqData.
			capabilityInfo
				= tempFramePtr[payloadLoc++];
			break;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
		case ASSOCIATIONRESPONSE:
			(void)memcpy(
					((uint8_t *)(&macParseData.macPayloadData.assocResponseData.shortAddr)),
					&tempFramePtr[payloadLoc],
					(uint8_t)sizeof(uint16_t));
			payloadLoc += (uint8_t)sizeof(uint16_t);
			macParseData.macPayloadData.assocResponseData.
			assocStatus = tempFramePtr[payloadLoc];
			break;
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
		case DISASSOCIATIONNOTIFICATION:
			macParseData.macPayloadData.disassocReqData.
			disassocReason
				= tempFramePtr[payloadLoc++];
			break;
#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

		case COORDINATORREALIGNMENT:
			(void)memcpy(((uint8_t *)(&macParseData.macPayloadData.coordRealignData.panId)), &tempFramePtr[payloadLoc], sizeof(uint16_t));
			payloadLoc += (uint8_t)sizeof(uint16_t);
			(void)memcpy(((uint8_t *)(&macParseData.macPayloadData.coordRealignData.coordShortAddr)), &tempFramePtr[payloadLoc], sizeof(uint16_t));
			payloadLoc += (uint8_t)sizeof(uint16_t);

			macParseData.macPayloadData.coordRealignData.
			logicalChannel = tempFramePtr[payloadLoc++];

			(void)memcpy(((uint8_t *)(&macParseData.macPayloadData.coordRealignData.shortAddr)), &tempFramePtr[payloadLoc], sizeof(uint16_t));
			payloadLoc += (uint8_t)sizeof(uint16_t);

			/*
			 * If frame version subfield indicates a 802.15.4-2006
			 * compatible frame,
			 * the channel page is appended as additional
			 * information element.
			 */
			if ((fcf & FCF_FRAME_VERSION_2006) == FCF_FRAME_VERSION_2006) {
				macParseData.macPayloadData.
				coordRealignData.channelPage
					= tempFramePtr[payloadLoc++];
			}

			break;

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
		case ORPHANNOTIFICATION:
#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */
		case DATAREQUEST:
		case BEACONREQUEST:
#if (MAC_PAN_ID_CONFLICT_AS_PC == 1)
		case PANIDCONFLICTNOTIFICAION:
#endif  /* (MAC_PAN_ID_CONFLICT_AS_PC == 1) */
			break;

		default:
            retVal = false;
            break;
		}
		break;

	default:
		retVal = false;
        break;
    }
    return retVal;
} /* ParseMpdu() */

/**
 * @brief Callback function called by TAL on reception of any frame.
 *
 * This function pushes an event into the TAL-MAC queue, indicating a
 * frame reception.
 *
 * @param frame Pointer to recived frame
 */

void PHY_RxFrameCallback(PHY_FrameInfo_t *rxFrame)
{
    buffer_t *buffer_header;
	MAC_FrameInfo_t *macRxFrame;
    uint8_t frameLen = 0;
    uint8_t *framePtr = NULL;
    qmm_status_t  qmmStatus;

    frameLen = rxFrame->mpdu[0];
    
	/* Allocate a buffer */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Check for buffer availability */
	if (NULL == buffer_header) {
		return;
	}

	/* Get the buffer body from buffer header */
	macRxFrame = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(
			buffer_header);

	framePtr = (uint8_t *)macRxFrame + LARGE_BUFFER_SIZE - frameLen;
    
    macRxFrame->mpdu = framePtr;

	if((frameLen > 0U) && (NULL != rxFrame->mpdu))
    {
        (void)memcpy(macRxFrame->mpdu, rxFrame->mpdu, frameLen);
    }
    
    macRxFrame->msgType = (FrameMsgtype_t)PHY_DATA_INDICATION;
      
	if (NULL == rxFrame->buffer_header) {
		return;
	}

	qmmStatus = qmm_queue_append(&phyMacQueue, buffer_header);
    
    bmm_buffer_free(rxFrame->buffer_header);
    
    WPAN_PostTask();
    (void)qmmStatus;
}

#ifdef PROMISCUOUS_MODE
static void PromModeRxFrame(buffer_t *b_ptr, MAC_FrameInfo_t *f_ptr)
{
    qmm_status_t qmmStatus;
	MCPS_DataInd_t *mdi
		= (MCPS_DataInd_t *)MAC_BUFFER_POINTER(b_ptr);

	/*
	 * In promiscous mode the MCPS_DATA.indication is used as container
	 * for the received frame.
	 */

	/*
	 * Since both f_ptr and mdi point to the same data storage place,
	 * we need to save all required data first.
	 * The time stamp has already been saved into .
	 * So lets save the payload now.
	 */

	/* Set payload pointer to MPDU of received frame. */
	mdi->msdu = &f_ptr->mpdu[1];
	mdi->msduLength = f_ptr->mpdu[0];

	/* Build the MLME_Data_indication parameters */
	mdi->DSN = 0;
#ifdef ENABLE_TSTAMP
	mdi->Timestamp = f_ptr->timeStamp;
#endif  /* ENABLE_TSTAMP */

	/* Source address mode is 0. */
	mdi->SrcAddrMode = FCF_NO_ADDR;
	mdi->SrcPANId = 0;
	mdi->SrcAddr = 0;

	/* Destination address mode is 0.*/
	mdi->DstAddrMode = FCF_NO_ADDR;
	mdi->DstPANId = 0;
	mdi->DstAddr = 0;

	mdi->mpduLinkQuality = macParseData.ppduLinkQuality;
	mdi->cmdcode = MCPS_DATA_INDICATION;

	/* Append MCPS data indication to MAC-NHLE queue */
	qmmStatus = qmm_queue_append(&macNhleQueue, b_ptr);
    WPAN_PostTask();
    (void)qmmStatus;
}

#endif  /* PROMISCUOUS_MODE */

#if (MAC_PAN_ID_CONFLICT_AS_PC == 1)

/**
 * @brief PAN-Id conflict detection as PAN-Coordinator.
 *
 * This function handles the detection of PAN-Id Conflict detection
 * in case the node is a PAN Coordinator.
 *
 * @param inScan Indicates whether node is currently scanning
 */
static void CheckForPanIdConflictAsPc(bool inScan)
{
	/*
	 * Check whether the received frame has the PAN Coordinator bit set
	 * in the Superframe Specification field of the beacon frame, and
	 * whether the received beacon frame has the same PAN-Id as the current
	 * network.
	 */
    PibValue_t pib_value;
    PHY_Retval_t pibStatus;
    pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
	if ((GET_PAN_COORDINATOR(macParseData.macPayloadData.beaconData.
			superframeSpec)) == 1U) {
		if (
#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
			(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
			((!inScan) &&
			(macParseData.srcPanid == pib_value.pib_value_16bit)) ||
			(macParseData.srcPanid == macScanOrigPanid)
#else
			(!inScan) &&
			(macParseData.srcPanid == pib_value.pib_value_16bit)
#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */
			) {
			MAC_SyncLoss((uint8_t)MAC_PAN_ID_CONFLICT);
		}
	}
    (void)pibStatus;
}

#endif  /* (MAC_PAN_ID_CONFLICT_AS_PC == 1) */

#if (MAC_PAN_ID_CONFLICT_NON_PC == 1)

/**
 * @brief PAN-Id conflict detection NOT as PAN-Coordinator.
 *
 * This function handles the detection of PAN-Id Conflict detection
 * in case the node is NOT a PAN Coordinator.
 *
 * @param inScan Indicates whether node is currently scanning
 */
static void CheckForPanIdConflictNonPc(bool inScan)
{
	/*
	 * Check whether the received frame has the PAN Coordinator bit set
	 * in the Superframe Specification field of the beacon frame.
	 */
    PibValue_t pib_value;
    bool status = false;
    PHY_Retval_t pibStatus = PHY_FAILURE;
	if ((GET_PAN_COORDINATOR(macParseData.macPayloadData.beaconData.superframeSpec)) == 1U) {
		/*
		 * The received beacon frame is from a PAN Coordinator
		 * (not necessarily ours).
		 * Now check if the PAN-Id is ours.
		 */ 
        pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
		if (
#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
			(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
			((!inScan) &&
			(macParseData.srcPanid == pib_value.pib_value_16bit)) ||
			(macParseData.srcPanid == macScanOrigPanid)
#else
			(!inScan) &&
			(macParseData.srcPanid == pib_value.pib_value_16bit)
#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */
			) {
			/* This beacon frame has our own PAN-Id.
			 * If the address of the source is different from our
			 * own
			 * parent, a PAN-Id conflict has been detected.
			 */
			if (
				(macParseData.srcAddr.shortAddress !=
				macPib.mac_CoordShortAddress) &&
				(macParseData.srcAddr.longAddress !=
				macPib.mac_CoordExtendedAddress)
				) {
				status = TxPanIdConfNotif();
                (void)status;
			}
		}
	}
    (void)pibStatus;
}

#endif /* (MAC_PAN_ID_CONFLICT_NON_PC == 1) */

#if (MAC_PAN_ID_CONFLICT_NON_PC == 1)

/**
 * @brief Sends a PAN-Id conflict detection command frame
 *
 * This function is called in case a device (that is associated to a
 * PAN Coordinator) detects a PAN-Id conflict situation.
 *
 * @return True if the PAN-Id conflict notification is sent successfully,
 *         false otherwise
 */
static bool TxPanIdConfNotif(void)
{
	PHY_Retval_t phyStatus = PHY_FAILURE;
	uint8_t frameLen;
	uint8_t *framePtr;
	uint16_t fcf;
    PibValue_t pib_value;

	buffer_t *panIdConfBuffer = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == panIdConfBuffer) {
		return false;
	}

	MAC_FrameInfo_t *panIdConfFrame
		= (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(panIdConfBuffer);

	panIdConfFrame->msgType = PANIDCONFLICTNOTIFICAION;

	/*
	 * The buffer header is stored as a part of MAC_FrameInfo_t structure
	 * before the
	 * frame is given to the TAL. After the transmission of the frame, reuse
	 * the buffer using this pointer.
	 */
	panIdConfFrame->buffer_header = panIdConfBuffer;

	/* Update the payload length. */
	frameLen = PAN_ID_CONFLICT_PAYLOAD_LEN +
			2 + /* Add 2 octets for FCS */
			8 + /* 8 octets for long Source Address */
			8 + /* 8 octets for long Destination Address */
			2 + /* 2 octets for Destination PAN-Id */
			3; /* 3 octets DSN and FCF */

	/* Get the payload pointer. */
	framePtr = (uint8_t *)panIdConfFrame +
			LARGE_BUFFER_SIZE -
			PAN_ID_CONFLICT_PAYLOAD_LEN - 2; /* Add 2 octets for
	                                                  * FCS. */

	/*
	 * Build the command frame id.
	 * This is actually being written into "transmitFrame->payload[0]".
	 */
	*framePtr = (uint8_t)PANIDCONFLICTNOTIFICAION;

	/* Source Address */
	framePtr -= 8;
    
    phyStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
    convert_64_bit_to_byte_array(pib_value.pib_value_64bit, framePtr);
  

	/* Destination Address */
	framePtr -= 8;
	convert_64_bit_to_byte_array(macPib.mac_CoordExtendedAddress,
			framePtr);

	/* Destination PAN-Id */
	framePtr -= 2;
    phyStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);

    convert_16_bit_to_byte_array(pib_value.pib_value_16bit, framePtr);
    

	/* Set DSN. */
	framePtr--;
	*framePtr = macPib.mac_DSN++;

	/* Build the FCF. */
	fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
			FCF_ACK_REQUEST |
			FCF_PAN_ID_COMPRESSION |
			FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
			FCF_SET_DEST_ADDR_MODE(FCF_LONG_ADDR);

	/* Set the FCF. */
	framePtr -= 2;
	convert_spec_16_bit_to_byte_array(fcf, framePtr);

	/* First element shall be length of PHY frame. */
	framePtr--;
	*framePtr = frameLen;

	/* Finished building of frame. */
	panIdConfFrame->mpdu = framePtr;
    (void)phyStatus;
	/* In Nonbeacon build the frame is sent with unslotted CSMA-CA. */
	phyStatus = sendFrame(panIdConfFrame, CSMA_UNSLOTTED, true);

	if (PHY_SUCCESS == phyStatus) {
		MAKE_MAC_BUSY();
		return true;
	} else {
		/* TAL is busy, hence the data request could not be transmitted
		**/
		bmm_buffer_free(panIdConfBuffer);

		return false;
	}
} /* TxPanIdConfNotif() */

#endif  /* (MAC_PAN_ID_CONFLICT_NON_PC == 1) */

/* EOF */
