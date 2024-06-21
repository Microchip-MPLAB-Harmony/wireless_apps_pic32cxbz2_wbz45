/*******************************************************************************
  MAC Process Tal Tx Frame Status Source

  File Name:
    mac_process_tal_tx_frame_status.c
  Summary:
    Processes the TAL tx frame status
  Description:
    This file Processes the TAL tx frame status received on the frame transmission.

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

/* === Macros =============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

static void MAC_ProcessPhyTxStatus(MAC_Retval_t txStatus, MAC_FrameInfo_t *frame);

#if (MAC_INDIRECT_DATA_FFD == 1)
static uint8_t FindBufferCb(void *buf, void *buffer);

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */
#if (MAC_INDIRECT_DATA_FFD == 1)
static void RemoveFrameFromIndirectQueue(MAC_FrameInfo_t *f_ptr);

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

/* === Implementation ====================================================== */

/*
 * @brief Process PHY_TxDoneCallback status
 *
 * This function is called, if an ACK is requested in the last transmitted
 * frame.
 * According to the frame type that has previously been sent, the
 * corresponding actions are taken and the MAC returns to its standard state.
 *
 * @param txStatus Status of transmission
 * @param frame Pointer to the transmitted frame
 */
static void MAC_ProcessPhyTxStatus(MAC_Retval_t txStatus, MAC_FrameInfo_t *frame)
{
	switch (frame->msgType) {
	case MCPS_MESSAGE:
#if (MAC_INDIRECT_DATA_FFD == 1)
		if (frame->indirectInTransit) {
			/* Clear indirect in transmit flag */
			frame->indirectInTransit = false;
			if ((txStatus == MAC_SUCCESS) ||
					(txStatus == MAC_FRAME_PENDING)) {
				RemoveFrameFromIndirectQueue(frame);

				buffer_t *mcpsBuf = frame->buffer_header;

				/* Create the MCPS DATA confirmation message */
				MAC_GenMCPSDataConf((buffer_t *)mcpsBuf,
						(uint8_t)txStatus,
#ifdef ENABLE_TSTAMP
						frame->msduHandle,
						frame->timeStamp);
#else
						frame->msduHandle);
#endif  /* ENABLE_TSTAMP */
			}
		} else /* Not indirect */
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */
		{
			buffer_t *mcpsBuf = frame->buffer_header;

			/* Create the MCPS DATA confirmation message */
			MAC_GenMCPSDataConf((buffer_t *)mcpsBuf,
					(uint8_t)txStatus,
#ifdef ENABLE_TSTAMP
					frame->msduHandle,
					frame->timeStamp);
#else
					frame->msduHandle);
#endif  /* ENABLE_TSTAMP */
		}

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;

#if (MAC_INDIRECT_DATA_FFD == 1)
	case NULL_FRAME:
		/* Free the buffer allocated for the Null data frame */
		bmm_buffer_free(frame->buffer_header);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
	case DISASSOCIATIONNOTIFICATION:
#if (MAC_DISASSOCIATION_FFD_SUPPORT == 1)
		if (frame->indirectInTransit) {
			/* Clear indirect in transmit flag */
			frame->indirectInTransit = false;
			if ((txStatus == MAC_SUCCESS) ||
					(txStatus == MAC_FRAME_PENDING)) {
				RemoveFrameFromIndirectQueue(frame);

				/* Create the MLME DISASSOCIATION confirmation
				 * message */

				/*
				 * Prepare disassociation confirm message after
				 * transmission of
				 * the disassociation notification frame.
				 */
				MAC_PrepDisassocConf(frame->buffer_header,
						(uint8_t)txStatus);
			}
		} else /* Not indirect */
#endif  /* (MAC_DISASSOCIATION_FFD_SUPPORT == 1) */
		{
			/* Create the MLME DISASSOCIATION confirmation message
			**/

			/*
			 * Prepare disassociation confirm message after
			 * transmission of
			 * the disassociation notification frame.
			 */
			MAC_PrepDisassocConf(frame->buffer_header,
					(uint8_t)txStatus);
		}

		/*
		 * Only an associated device should go to idle on transmission
		 * of a
		 * disassociation frame.
		 */
		if (MAC_ASSOCIATED == macState) {
			/*
			 * Entering sleep mode is already done implicitly in
			 * MAC_IdleTrans().
			 */
			MAC_IdleTrans();
		}

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;
#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	case ASSOCIATIONREQUEST:
		if ((MAC_NO_ACK == txStatus) ||
				(MAC_CHANNEL_ACCESS_FAILURE == txStatus)) {
			/*
			 * Association Request frame could not be sent.
			 *
			 * The broadcast short address will only be used if the
			 * association attempt was unsuccessful.
			 * On a successful association, the
			 * actual value will be filled in based on the response
			 * to the
			 * data request obtained from the coordinator.
			 */
			MAC_GenMLMEAssociateConf(frame->buffer_header,
					(uint8_t)txStatus, BROADCAST);

			/* Set radio to sleep if allowed */
			MAC_SleepTrans();
		} else {
			/* Successful transmission of Association Request. */
#ifdef TEST_HARNESS

			/*
			 * For purely testing purposes the coordinator may be
			 * set
			 * into a state where it will never respond with an
			 * Assoc Response frame
			 */
			if (macPib.privateNoDataAfterAssocReq >= 1) {
				bmm_buffer_free(frame->buffer_header);

				break;
			}
#endif /* TEST_HARNESS */

			macPollState = MAC_AWAIT_ASSOC_RESPONSE;

			{
				PAL_Status_t status;
				uint32_t responseTimer;

				/* Start the response wait timer */
				responseTimer = macPib.mac_ResponseWaitTime;
				responseTimer = PHY_CONVERT_SYMBOLS_TO_US(
						responseTimer);

				status = PAL_TimerStart(Timer_PollWaitTime,
						responseTimer,
						TIMEOUT_RELATIVE,
						&MAC_Timer_ResponseWaitCb,
						NULL,CALLBACK_SINGLE);
				if (PAL_SUCCESS != status) {
					MAC_Timer_ResponseWaitCb(NULL);
				}
			}
		}
		break;
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
	case DATAREQUEST:           /* Explicit poll caused by MLME-POLL.request
		                    **/
	case DATAREQUEST_IMPL_POLL: /* Implicit poll without MLME-POLL.request
		                    **/
	{
		/* Free the data_request buffer */
		bmm_buffer_free(frame->buffer_header);

		/*
		 * In case we are in the middle of an association procedure,
		 * we nothing here, but keep in the same state.
		 * Also the poll timer is NOT canceled here, because it acts
		 * as a timer for the entire association procedure.
		 */
		if (MAC_AWAIT_ASSOC_RESPONSE != macPollState) {
			if (DATAREQUEST == frame->msgType) {
				/* Explicit poll caused by MLME-POLL.request */
				if (MAC_FRAME_PENDING != txStatus) {
					/* Reuse the poll request buffer for
					 * poll confirmation */
					MLME_PollConf_t *mpc
						= (MLME_PollConf_t *) MAC_BUFFER_POINTER ((buffer_t *)((void*)macConfBufPtr));
                    qmm_status_t  qmmStatus;

					mpc->cmdcode = MLME_POLL_CONFIRM;
					mpc->status = (uint8_t)txStatus;
					qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)((void*)macConfBufPtr));

					/* Set radio to sleep if allowed */
					MAC_SleepTrans();
                    WPAN_PostTask();
                    (void)qmmStatus;
					return;
				}

				/* Wait for data reception */
				macPollState = MAC_POLL_EXPLICIT;
			} else {
				if ((MAC_NO_ACK != txStatus) &&
						(MAC_CHANNEL_ACCESS_FAILURE
						!= txStatus)) {
					/*
					 * Sucessful transmission of Data
					 * Request frame due to
					 * implicit poll without explicit poll
					 * request.
					 */
					macPollState = MAC_POLL_IMPLICIT;
				} else {
					/* Data request for implicit poll could
					 * not be sent. */
					/* Set radio to sleep if allowed */
					MAC_SleepTrans();
					return;
				}
			}

			{
				PAL_Status_t status;
				uint32_t responseTimer;

				/*
				 * Start Timer_PollWaitTime only if we are not in
				 * the
				 * middle of an association.
				 */
				responseTimer
					= macPib.mac_MaxFrameTotalWaitTime;
				responseTimer = PHY_CONVERT_SYMBOLS_TO_US(
						responseTimer);

				status = PAL_TimerStart(Timer_PollWaitTime,
						responseTimer,
						TIMEOUT_RELATIVE,
						&MAC_Timer_PollWaitTimeCb,
						NULL,CALLBACK_SINGLE);

				/*
				 * If we are waiting for a pending frame,
				 * the MAC needs to remain busy.
				 */
				MAKE_MAC_BUSY();

				if (PAL_SUCCESS != status) {
					MAC_Timer_PollWaitTimeCb(NULL);
				}
			}
		}   /* (MAC_AWAIT_ASSOC_RESPONSE != macPollState) */
	}
	break;
#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	case ASSOCIATIONRESPONSE:

		/*
		 * Association Response is ALWAYS indirect, so not further check
		 * for
		 * indirect_in_transit required, but clear the flag.
		 */
		frame->indirectInTransit = false;
		if ((txStatus == MAC_SUCCESS) ||
				(txStatus == MAC_FRAME_PENDING)) {
			/* Association resonse frames are always sent
			 * indirectly. */

			/*
			 * The removal of this frame from the indirect queue
			 * needs
			 * to be done BEFORE the subsequent call of function
			 * MAC_MLME_CommStatus(), since the variable frame is
			 * reused
			 * for the comm status indication buffer and would be
			 * invalid
			 * afterwards.
			 */
			RemoveFrameFromIndirectQueue(frame);

			MAC_MLME_CommStatus((uint8_t)txStatus, frame->buffer_header);
		}

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;
#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
	case ORPHANREALIGNMENT:
		MAC_MLME_CommStatus((uint8_t)txStatus,
				frame->buffer_header);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;
#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_PAN_ID_CONFLICT_NON_PC == 1)
	case PANIDCONFLICTNOTIFICAION:

		/* Free the buffer allocated for the Pan-Id conflict
		 * notification frame */
		bmm_buffer_free(frame->buffer_header);

		/* Generate a sync loss to the higher layer. */
		MAC_SyncLoss((uint8_t)MAC_PAN_ID_CONFLICT);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;
#endif  /* (MAC_PAN_ID_CONFLICT_NON_PC == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	case COORDINATORREALIGNMENT:

		/*
		 * The coordinator realignment command has been sent out
		 * successfully. Hence the MAC should be updated with the new
		 * parameters given in the MLME.START_request with
		 * coordinator realignment command
		 */
		MAC_CoordRealignmentCommandTxSuccess((uint8_t)txStatus,
				frame->buffer_header);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	case BEACON_MESSAGE:
		/*
		 * The code for freeing up the buffer is only included for a
		 * build without beacon support, since in this case the static
		 * beacon buffer is NOT used, but a standard buffer is used
		 * instead, which needs to be freed up.
		 */
		bmm_buffer_free(frame->buffer_header);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();

		break;
#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

	default:
		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
		break;
	}
} /* MAC_ProcessPhyTxStatus() */

/**
 * @brief Callback function from TAL after the frame is transmitted
 *
 * This is a callback function from the TAL. It is used when an attempt
 * to transmit a frame is finished.
 *
 * @param status Status of transmission
 * @param frame Specifies pointer to the transmitted frame
 */
void PHY_TxDoneCallback(PHY_Retval_t status, PHY_FrameInfo_t *frame)
{
	/* Frame transmission completed, set dispatcher to not busy */
	MAKE_MAC_NOT_BUSY();
    
    MAC_Retval_t macStatus = FAILURE;
    
    switch (status)
    {
        case PHY_FAILURE:
            macStatus = FAILURE;
            break;
            
        case PHY_SUCCESS:
            macStatus = MAC_SUCCESS;
            break;
            
        case PHY_FRAME_PENDING:
            macStatus = MAC_FRAME_PENDING;
            break;
            
        case PHY_CHANNEL_ACCESS_FAILURE:
            macStatus = MAC_CHANNEL_ACCESS_FAILURE;
            break;
            
        case PHY_NO_ACK:
            macStatus = MAC_NO_ACK;
            break;
            
        case PHY_RF_REQ_ABORTED:
            macStatus = FAILURE;
            break;
            
        case PHY_RF_UNAVAILABLE:
            macStatus = FAILURE;
            break;  
            
        default:
            macStatus = FAILURE;
            break;
    }

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
	/* If ack requested and ack not received, or ack not requested */ 
	if (
		((MAC_SCAN_ACTIVE == macScanState) &&
		(macFrameInfo->msgType == BEACONREQUEST)) ||
		((MAC_SCAN_ORPHAN == macScanState) &&
		(macFrameInfo->msgType == ORPHANNOTIFICATION))
		) {
		MAC_ScanSendComplete(macStatus);
	} else
#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
	 *(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) */
	{
		MAC_ProcessPhyTxStatus(macStatus, macFrameInfo);
	}
}

#if (MAC_INDIRECT_DATA_FFD == 1)

/**
 * @brief Helper function to remove transmitted indirect data from the queue
 *
 * @param f_ptr Pointer to MAC_FrameInfo_t structure of previously transmitted
 * frame
 */
static void RemoveFrameFromIndirectQueue(MAC_FrameInfo_t *f_ptr)
{
	search_t findBuf;
    buffer_t *buf;

	findBuf.criteria_func = FindBufferCb;

	/* Update the address to be searched */
	findBuf.handle = (void *)f_ptr->buffer_header;

	buf = qmm_queue_remove(&indirectDataQueue, &findBuf);
    (void)buf;
}

#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)

/**
 * @brief Checks whether the indirect data frame address matches
 * with the address passed.
 *
 * @param buf Pointer to indirect data buffer
 * @param buffer Pointer to the buffer to be searched
 *
 * @return 1 if address matches, 0 otherwise
 */

static uint8_t FindBufferCb(void *buf, void *buffer)
{
	uint8_t *bufBody = (uint8_t *)MAC_BUFFER_POINTER((buffer_t *)buffer);
	if (buf == bufBody) {
		return 1;
	}

	return 0;
}

#endif /*(MAC_INDIRECT_DATA_FFD == 1)*/
/* EOF */
