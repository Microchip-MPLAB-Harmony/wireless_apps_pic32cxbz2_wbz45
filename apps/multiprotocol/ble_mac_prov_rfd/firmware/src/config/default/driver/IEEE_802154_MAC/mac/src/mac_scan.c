/*******************************************************************************
  MAC Scan Source

  File Name:
    mac_scan.c
  Summary:
    Implements the MLME_SCAN primitives
  Description:
    This file implements functions to handle all MLME-SCAN primitives and the
    corresponding scan algorithm. This includes ED, Active, Passive and Orphan
    scan. All required timers and frames (beacon request and orphan
    notification frames) are assembled and their transmission is initiated.

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
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "pal.h"
#include "bmm.h"
#include "qmm.h"
#include "phy.h"
#include "phy_constants.h"
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

#if (MAC_SCAN_SUPPORT == 1)

/* === Macros =============================================================== */

/**
 * Scan duration formula: \f$aBaseSuperframeDuration (2^SD + 1)\f$
 * where \f$0 <= SD <= 14\f$
 */
#define MAC_CALCULATE_SYMBOL_TIME_SCANDURATION(SD) \
	(aBaseSuperframeDuration * ((1UL << (SD)) + 1U))

/*
 * Max beacon Order in beacon-enabled network
 */
#define BEACON_NETWORK_MAX_BO           (14U)

/*
 * Beacon Request and Orphan Notification command frame payload length
 */
#define BEAC_REQ_ORPH_NOT_PAYLOAD_LEN       (1U)

/* === Globals ============================================================= */

static uint8_t scanType;
static uint32_t scanChannels;
static uint8_t scanCurrChannel;
static uint8_t scanCurrPage;
static uint8_t scanDuration;

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
/* Original PAN-ID before starting of active or passive scan. */
#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
        *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */

/* === Prototypes ========================================================== */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
static void MAC_Timer_ScanDurationCb(void *callbackParameter);

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
        */

static void MAC_AwakeScan(buffer_t *scanBuf);
static void ScanSetComplete(MAC_Retval_t setStatus);
static void ScanCleanUp(buffer_t *buffer);
static void ScanProceed(uint8_t scanningType, buffer_t *buf);
static bool SendScanCmd(bool beaconReq);

/* === Implementation ====================================================== */

/*
 * @brief Proceed with a scan request
 *
 * This function proceeds with the scanning.
 * The current channel is incremented. It is checked if it belongs to the
 * list of channels to scan. If so, start scanning. If all channels done,
 * send out the MLME_SCAN.confirm message.
 *
 * @param scanning_type The type of the scan operation to proceed with.
 * @param buf Buffer to send mlme scan confirm to NHLE.
 */
static void ScanProceed(uint8_t scanningType, buffer_t *buf)
{
	MAC_Retval_t setStatus = FAILURE;
    PHY_Retval_t pibSetStatus = PHY_FAILURE;
    qmm_status_t qmmStatus = QMM_QUEUE_FULL;
	MLME_ScanConf_t *msc = (MLME_ScanConf_t *)MAC_BUFFER_POINTER(buf);

	/* Set the channel page to perform scan */
	pibSetStatus = SetPhyPibInternal(phyCurrentPage,
			(void *)&scanCurrPage);

	/* Loop over all channels the MAC has been requested to scan */
	for (; scanCurrChannel <= MAX_CHANNEL; scanCurrChannel++) {
#if ((MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) ||	\
		(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1))
		if (
			((MAC_SCAN_ACTIVE == macScanState) ||
			(MAC_SCAN_PASSIVE == macScanState)) &&
			((macPib.mac_AutoRequest) == 1U)
			) {
			/*
			 * For active or passive scans, bail out if we
			 * reached our maximum number of PANDescriptors that
			 * could
			 * be stored. That way, the upper layer will get the
			 * correct set of unscanned channels returned, so it can
			 * continue scanning if desired.
			 *
			 * According to 802.15.4-2006 PAN descriptor are only
			 * present
			 * in the scan confirm message in case the PIB attribute
			 * macAutoRequest is true.
			 */
			if (msc->ResultListSize >= MAX_PANDESCRIPTORS) {
				break;
			}
		}
#endif /* ((MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)) */
#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
		if (MLME_SCAN_TYPE_ORPHAN == scanningType) {
			/*
			 * In an orphan scan, terminate if any coordinator
			 * realignment packet has been received.
			 */
			if ((msc->ResultListSize) > 0U) {
				break;
			}
		}
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

		if ((msc->UnscannedChannels & (1UL << scanCurrChannel)) != 0U) {
#if (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)
			if (MLME_SCAN_TYPE_ACTIVE == scanningType) {
				macScanState = MAC_SCAN_ACTIVE;
			}
#endif /* (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) */
#if (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)
			if (MLME_SCAN_TYPE_PASSIVE == scanningType) {
				macScanState = MAC_SCAN_PASSIVE;
			}
#endif /* (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) */
			if (MLME_SCAN_TYPE_ORPHAN == scanningType) {
				macScanState = MAC_SCAN_ORPHAN;
			}

			/* Set the channel to perform scan */
			pibSetStatus = SetPhyPibInternal(phyCurrentChannel,
					(void *)&scanCurrChannel);

			if (PHY_SUCCESS != pibSetStatus) {
				/*
				 * Free the buffer used for sending orphan
				 * notification command
				 */
				bmm_buffer_free((buffer_t *)((void *)macScanCmdBufPtr));

				macScanCmdBufPtr = NULL;

				/* Set radio to sleep if allowed */
				MAC_SleepTrans();

				msc->status = (uint8_t)MAC_NO_BEACON;

				/* Orphan scan does not return any list. */
				msc->ResultListSize = 0;

				/* Append the scan confirm message to the
				 * MAC-NHLE queue */
				qmmStatus = qmm_queue_append(&macNhleQueue, buf);

				macScanState = MAC_SCAN_IDLE;
			}
			/* Continue scanning, after setting channel */
			if(pibSetStatus == PHY_SUCCESS){
                setStatus = MAC_SUCCESS;
            }
            ScanSetComplete(setStatus);
            WPAN_PostTask();
            (void)qmmStatus;
			return;
		}
	}

	/* All channels were scanned. The confirm needs to be prepared */
	switch (scanningType) {
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ED:
		msc->status = (uint8_t)MAC_SUCCESS;
		ScanCleanUp(buf);
		break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ACTIVE:
	{
		/*
		 * Free the buffer which was received from scan request and
		 * reused
		 * for beacon request frame transmission.
		 */
		bmm_buffer_free((buffer_t *)((void *)macScanCmdBufPtr));

		macScanCmdBufPtr = NULL;

		if (((macPib.mac_AutoRequest) == 0U)) {
			msc->status = (uint8_t)MAC_SUCCESS;
		} else if (msc->ResultListSize >= MAX_PANDESCRIPTORS) {
			msc->status = (uint8_t)MAC_LIMIT_REACHED;
		} else if ((msc->ResultListSize) > 0U) {
			msc->status = (uint8_t)MAC_SUCCESS;
		} else {
			msc->status = (uint8_t)MAC_NO_BEACON;
		}

		/* Restore macPANId after active scan completed. */
		pibSetStatus = SetPhyPibInternal(macPANId, (void *)&macScanOrigPanid);
		/* Done with scanning */
		ScanCleanUp((buffer_t *)((void *)macConfBufPtr));
        (void)pibSetStatus;
	}
	break;
#endif /* (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_PASSIVE:
		if (((macPib.mac_AutoRequest) == 0U)) {
			msc->status = (uint8_t)MAC_SUCCESS;
		} else if (msc->ResultListSize >= MAX_PANDESCRIPTORS) {
			msc->status = (uint8_t)MAC_LIMIT_REACHED;
		} else if (msc->ResultListSize > 0U) {
			msc->status = (uint8_t)MAC_SUCCESS;
		} else {
			msc->status = (uint8_t)MAC_NO_BEACON;
		}

		/* Restore macPANId after passive scan completed. */
		pibSetStatus = SetPhyPibInternal(macPANId, (void *)&macScanOrigPanid);

		ScanCleanUp(buf);
        (void)pibSetStatus;
		break;
#endif /* (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ORPHAN:

		/* Free the buffer used for sending orphan notification command
		**/
		bmm_buffer_free((buffer_t *)((void *)macScanCmdBufPtr));

		macScanCmdBufPtr = NULL;

		if (msc->ResultListSize > 0U) {
			msc->status = (uint8_t)MAC_SUCCESS;
		} else {
			msc->status = (uint8_t)MAC_NO_BEACON;
		}

		/* Orphan scan does not return any list. */
		msc->ResultListSize = 0;

		ScanCleanUp(buf);
		break;
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

	default:
        __NOP();
		break;
	}
} /* ScanProceed() */

/*
 * @brief Continues handling of MLME_SCAN.request once the radio is awake
 *
 * @param scan_buf Pointer to Scan request buffer.
 */
static void MAC_AwakeScan(buffer_t *scanBuf)
{
	MLME_ScanConf_t *msc;
    PibValue_t pib_value;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
	msc = (MLME_ScanConf_t *)MAC_BUFFER_POINTER(scanBuf);

	/* Set the first channel at which the scan is started */
	scanCurrChannel = MIN_CHANNEL;

	switch (scanType) {
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ED:
		msc->scanResultList[0].edValue[1] = 0; /* First channel's
		                                           * accumulated energy
		                                           * level */
		macScanState = MAC_SCAN_ED;
		ScanProceed(MLME_SCAN_TYPE_ED, (buffer_t *)scanBuf);
		break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
		(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1))
	case MLME_SCAN_TYPE_ACTIVE:
	case MLME_SCAN_TYPE_PASSIVE:
	{
		/*
		 * Before commencing an active or passive scan, the MAC sublayer
		 * shall store the value of macPANId and then set it to 0cFFFF
		 * for
		 * the duration of the scan. This enables the receive filter to
		 * accept all beacons rather than just the beacons from its
		 * current PAN (see 7.5.6.2). On completion of the scan, the
		 * MAC sublayer shall restore the value of macPANId to the
		 * value stored before the scan began.
		 */
		uint16_t broadcast_panid = BROADCAST;

		pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
        macScanOrigPanid = pib_value.pib_value_16bit;

		pibStatus = SetPhyPibInternal(macPANId, (void *)&broadcast_panid);
        (void)pibStatus;

		if (MLME_SCAN_TYPE_ACTIVE == scanType) {
			/*
			 * In active scan reuse the scan request buffer for
			 * sending beacon request.
			 */
			macScanCmdBufPtr = (uint8_t *)scanBuf;
		}

		/* Allocate a large size buffer for scan confirm. */
		macConfBufPtr
			= (uint8_t *)bmm_buffer_alloc(LARGE_BUFFER_SIZE);

		if (NULL == macConfBufPtr) {
			/*
			 * Large buffer is not available for sending scan
			 * confirmation,
			 * hence the scan request buffer (small buffer) is used
			 * to send
			 * the scan confirmation.
			 */
			msc->status = (uint8_t)MAC_INVALID_PARAMETER;

			/* Append scan confirm message to the MAC-NHLE queue */
			qmmStatus = qmm_queue_append(&macNhleQueue, scanBuf);

			/* Set radio to sleep if allowed */
			MAC_SleepTrans();
            WPAN_PostTask();
            (void)qmmStatus;
			return;
		}

		if (MLME_SCAN_TYPE_PASSIVE == scanType) {
			/* Free the scan request buffer when in passive scan. */
			bmm_buffer_free(scanBuf);
		}

		msc = (MLME_ScanConf_t *)MAC_BUFFER_POINTER(
				(buffer_t *)((void *)macConfBufPtr));

		msc->cmdcode = MLME_SCAN_CONFIRM;
		msc->ScanType = scanType;
		msc->ChannelPage = scanCurrPage;
		msc->UnscannedChannels = scanChannels;
		msc->ResultListSize = 0;
		msc->scanResultList[0].edValue[0] = 0;

		ScanProceed(scanType, (buffer_t *)((void *)macConfBufPtr));
		break;
	}
#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
		 *(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
	case MLME_SCAN_TYPE_ORPHAN:
		/* Buffer allocated for orphan notification command */
		macScanCmdBufPtr
			= (uint8_t *)bmm_buffer_alloc(LARGE_BUFFER_SIZE);

		if (NULL == macScanCmdBufPtr) {
			msc->status = (uint8_t)MAC_INVALID_PARAMETER;

			/* Append scan confirm message to the MAC-NHLE queue */
			qmmStatus = qmm_queue_append(&macNhleQueue, scanBuf);

			/* Set radio to sleep if allowed */
			MAC_SleepTrans();
            WPAN_PostTask();
            (void)qmmStatus;
			return;
		}

		ScanProceed(MLME_SCAN_TYPE_ORPHAN,
				(buffer_t *)((void *)macConfBufPtr));
		break;
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

	default:
		msc->status = (uint8_t)MAC_INVALID_PARAMETER;
		/* Append scan confirm message to the MAC-NHLE queue */
		qmmStatus = qmm_queue_append(&macNhleQueue, scanBuf);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
        WPAN_PostTask();
        (void)qmmStatus;
		break;
	}
} /* MAC_AwakeScan() */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/**
 * @brief Send a beacon request or orphan notification command frame
 *
 * This function sends a beacon request or orphan notification command frame.
 * An MPDU containing either a beacon request or an orphan notification command
 * frame is constructed and sent.
 *
 * @param beacon_req True if a beacon request command frame shall be sent,
 *                   otherwise (false) an orphan notification command frame
 *                   will be sent.
 *
 * @return True if the frame transmission has been initiated, false otherwise.
 */
static bool SendScanCmd(bool beaconReq)
{
	PHY_Retval_t phyStatus = PHY_FAILURE;
	uint8_t frameLen;
	uint8_t *framePtr;
	uint16_t fcf;
	uint16_t bcAddr = BROADCAST;
    PibValue_t pib_value;

	/*
	 * mac_scan_cmd_buf_ptr holds the buffer allocated for sending beacon
	 * request or orphan notification command. In active scan the scan
	 * request
	 * buffer is used to send a beacon request and in orphan scan new buffer
	 * is
	 * allocated to send an orphan notification.
	 */
	MAC_FrameInfo_t *transmitFrame
		= (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(
			(buffer_t *)((void *)macScanCmdBufPtr));

	/* Get the payload pointer. */
	framePtr = (uint8_t *)transmitFrame +
			LARGE_BUFFER_SIZE -
			BEAC_REQ_ORPH_NOT_PAYLOAD_LEN - 2; /* Add 2 octets for
	                                                    * FCS. */

	transmitFrame->buffer_header = (buffer_t *)((void *)macScanCmdBufPtr);

	if (beaconReq) {
		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_NO_ADDR);

		/* Update the length. */
		frameLen = BEAC_REQ_ORPH_NOT_PAYLOAD_LEN +
				2U + /* Add 2 octets for FCS */
				2U + /* 2 octets for short Destination Address */
				2U + /* 2 octets for Destination PAN-Id */
				3U; /* 3 octets DSN and FCF */

		/* Build the command frame id. */
        transmitFrame->msgType = BEACONREQUEST;
		*framePtr = (uint8_t)(transmitFrame->msgType);
	} else { /* Orphan Notification command */
		fcf = FCF_SET_FRAMETYPE(FCF_FRAMETYPE_MAC_CMD) |
				FCF_SET_DEST_ADDR_MODE(FCF_SHORT_ADDR) |
				FCF_SET_SOURCE_ADDR_MODE(FCF_LONG_ADDR) |
				FCF_PAN_ID_COMPRESSION;

		/* Update the length. */
		frameLen = BEAC_REQ_ORPH_NOT_PAYLOAD_LEN +
				2U + /* Add 2 octets for FCS */
				2U + /* 2 octets for short Destination Address */
				2U + /* 2 octets for Destination PAN-Id */
				8U + /* 8 octets for long Source Address */
				3U; /* 3 octets DSN and FCF */

		/* Build the command frame id. */
        transmitFrame->msgType = ORPHANNOTIFICATION;
		*framePtr = (uint8_t)(transmitFrame->msgType);

		/* Orphan notification contains long source address. */
		framePtr -= 8;
        phyStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
        convert_64_bit_to_byte_array(pib_value.pib_value_64bit, framePtr);

	}

	/* Destination address */
	framePtr -= 2;
	convert_16_bit_to_byte_array(bcAddr, framePtr);

	/* Destination PANid */
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
	transmitFrame->mpdu = framePtr;
    (void)phyStatus;

	/* Transmit data with unslotted CSMA-CA and no frame retry. */
	phyStatus = sendFrame(transmitFrame, CSMA_UNSLOTTED, false);

	if (PHY_SUCCESS == phyStatus) {
		MAKE_MAC_BUSY();
		return true;
	} else {
		return false;
	}
} /* SendScanCmd() */

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
        *(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) */

/**
 * @brief Continue scanning after setting of PIB attributes
 *
 * This functions continues scanning once the corresponding PIB
 * attribute change has been completed depending on the status.
 *
 * @param set_status Status of the Request to change the PIB attribute
 */
static void ScanSetComplete(MAC_Retval_t setStatus)
{
	switch (macScanState) {
#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)
	case MAC_SCAN_ED:
	{
		if (MAC_SUCCESS == setStatus) {
			MAKE_MAC_BUSY();
			PHY_Retval_t status = PHY_FAILURE;
			status = PHY_EdStart(scanDuration);
            (void)status;
		} else {
			/* Channel not supported, continue. */
			scanCurrChannel++;
		}
	}
	break;
#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)
	case MAC_SCAN_ACTIVE:
		if (MAC_SUCCESS == setStatus) {
			/*
			 * The TAL switches ON the transmitter while sending a
			 * beacon request, hence the MAC can call
			 * SendScanCmd() to send
			 * the beacon directly
			 */

			/* Send an beacon request command */
			if (!SendScanCmd(true)) {
				uint8_t timer_id = Timer_ScanDuration;

				/*
				 * Beacon request could not be transmitted
				 * since there is no buffer available stop
				 * scanning
				 * we call the function usually called by the
				 * scan
				 * duration timer to pretend scanning has
				 * finished
				 */
				MAC_Timer_ScanDurationCb((uint8_t *)&timer_id);
			}
		} else {
			/* Channel not supported, continue. */
			scanCurrChannel++;
		}
		break;
#endif /* (MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1)
	case MAC_SCAN_PASSIVE:
	{
		if (MAC_SUCCESS == setStatus) {
            PHY_TrxStatus_t status;
			status = PHY_RxEnable(PHY_STATE_RX_ON);

			if (PHY_RX_ON == status) {
				PAL_Status_t timerStatus;
				uint32_t tmr
					= MAC_CALCULATE_SYMBOL_TIME_SCANDURATION(
						scanDuration);

				timerStatus = PAL_TimerStart(Timer_ScanDuration,
						PHY_CONVERT_SYMBOLS_TO_US(
						tmr),
						TIMEOUT_RELATIVE,
						&MAC_Timer_ScanDurationCb,
						NULL,CALLBACK_SINGLE);

				if (PAL_SUCCESS != timerStatus) {
					uint8_t timer_id = Timer_ScanDuration;

					/*
					 * Scan duration timer could not be
					 * started so we
					 * call the timer callback function
					 * directly this
					 * will basically shorten scanning
					 * without having
					 * really scanned.
					 */
					MAC_Timer_ScanDurationCb((void *)&timer_id);
				}
			} else {
				/* Did not work, continue. */
				scanCurrChannel++;
				ScanProceed(MLME_SCAN_TYPE_PASSIVE,
						(buffer_t *)((void *)macConfBufPtr));
			}
		} else {
			/* Channel not supported, continue. */
			scanCurrChannel++;
		}
	}
	break;
#endif /* (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) */

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)
	case MAC_SCAN_ORPHAN:
		if (MAC_SUCCESS == setStatus) {
			/* Send an orphan notification command */
			if (!SendScanCmd(false)) {
				uint8_t timer_id = Timer_ScanDuration;

				/*
				 * Orphan notification could not be transmitted
				 * since there
				 * is no buffer available stop scanning
				 * we call the function usually called by the
				 * scan duration
				 * timer to pretend scanning has finished
				 */
				MAC_Timer_ScanDurationCb((uint8_t *)&timer_id);
			}
		} else {
			/* Channel not supported, continue. */
			scanCurrChannel++;
		}
		break;
#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

	default:
        __NOP();
		break;
	}
} /* ScanSetComplete() */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/**
 * @brief Continue scanning after the completion of frame transmission.
 *
 * This functions continues the corresponding scaning depending on status
 * from the transmission of a beacon request or orphan notification frame.
 *
 * @param status Status of transmission
 */
void MAC_ScanSendComplete(MAC_Retval_t status)
{
	PAL_Status_t timerStatus;

	macPib.mac_DSN++;

	if (MAC_SUCCESS == status) {
		uint32_t tmr = 0;

		if (MAC_SCAN_ACTIVE == macScanState) {
			tmr = MAC_CALCULATE_SYMBOL_TIME_SCANDURATION(
					scanDuration);
		} else {
			/*
			 * Since this function is only called in active or
			 * orphan scan state,
			 * this case is handling the orphan scan state.
			 */
			tmr = macPib.mac_ResponseWaitTime;
		}

		timerStatus = PAL_TimerStart(Timer_ScanDuration,
				PHY_CONVERT_SYMBOLS_TO_US(tmr),
				TIMEOUT_RELATIVE,
				&MAC_Timer_ScanDurationCb,
				NULL,CALLBACK_SINGLE);

		if (PAL_SUCCESS != timerStatus) {
			uint8_t timer_id = Timer_ScanDuration;

			/*
			 * Scan duration timer could not be started, so we call
			 * the timer callback function directly. This will
			 * basically
			 * shorten scanning without having really scanned.
			 */
			MAC_Timer_ScanDurationCb((void *)&timer_id);
		}
	} else {
		/* Did not work, continue. */
		scanCurrChannel++;
		ScanProceed(scanType, (buffer_t *)((void *)macConfBufPtr));
	}
}

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) ||
        *(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) */

#if ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/*
 * @brief MAC scan timer callback
 *
 * @param callbackParameter Callback parameter.
 */
static void MAC_Timer_ScanDurationCb(void *callbackParameter)
{
	/* mac_conf_buf_ptr holds the buffer for scan confirm */
	MLME_ScanConf_t *msc
		= (MLME_ScanConf_t *)MAC_BUFFER_POINTER(
			(buffer_t *)((void *)macConfBufPtr));

	switch (macScanState) {
	case MAC_SCAN_ACTIVE:
	case MAC_SCAN_PASSIVE:
	case MAC_SCAN_ORPHAN:
		msc->UnscannedChannels &= ~(1UL << scanCurrChannel);
		ScanProceed(scanType, (buffer_t *)((void *)macConfBufPtr));
		break;

	default:
        __NOP();
		break;
	}

	callbackParameter = callbackParameter; /* Keep compiler happy. */
}

#endif /* ((MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
        *  (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
        */

/**
 * @brief The MLME-SCAN.request primitive makes a request for a node to
 * start a scan procedure.
 *
 * 802.15.4. Section 7.1.11.1.
 *
 * The MLME-SCAN.request primitive is used to initiate a channel scan over a
 * given list of channels. A device can use a channel scan to measure the
 * energy on the channel, search for the coordinator with which it associated,
 * or search for all coordinators transmitting beacon frames within the
 * POS of the scanning device.
 *
 * The MLME-SCAN.request primitive is generated by the next higher layer and
 * issued to its MLME to initiate a channel scan to search for activity within
 * the POS of the device. This primitive can be used to perform an ED scan to
 * determine channel usage, an active or passive scan to locate beacon frames
 * containing any PAN identifier, or an orphan scan to locate a PAN to which
 * the device is currently associated.
 *
 * ED or active scans can be performed before an FFD starts operation as a
 * PAN coordinator. Active or passive scans can be performed prior to selecting
 * a PAN for association. Orphan scans can be performed to attempt to locate a
 * specific coordinator with which communication has been lost.
 *
 * All devices shall be capable of performing passive scans and orphan scans;
 * ED scans and active scans are optional for an RFD.
 *
 * When the MLME receives the MLME-SCAN.request primitive, it initiates a scan
 * in all channels specified in the ScanChannels parameter. The MLME suspends
 * all beacon transmissions for the duration of the scan. During a scan, the
 * MAC sublayer only accepts frames received over the PHY data service that are
 * relevant to the scan being performed (see 7.5.2.1).
 *
 * An ED scan allows a device to obtain a measure of the peak energy in each
 * requested channel. The ED scan is performed on each channel by the MLMEs
 * repeatedly issuing the PLME-ED.request primitive to the PHY until
 * [aBaseSuperframeDuration * (2n + 1)] symbols, where n is the value of the
 * ScanDuration parameter, have elapsed. The MLME notes the maximum energy
 * measurement and moves on to the next channel in the channel list. A device
 * will be able to store between one and an implementation-specified maximum
 * number of channel ED measurements. The ED scan terminates when the number
 * of channel ED measurements stored equals this implementation-specified
 * maximum or when every channel specified in the channel list has been scanned.
 *
 * An active scan is used by an FFD to locate all coordinators transmitting
 * beacon frames within its POS. The active scan is performed on each channel
 * by the MLMEs first sending a beacon request command (see 7.3.2.4). The MLME
 * then enables the receiver and records the information contained in each
 * received beacon in a PAN descriptor structure (see Table 41 in 7.1.5.1.1).
 * A device will be able to store between one and an implementation-specified
 * maximum number of PAN descriptors. The active scan on a particular channel
 * terminates when the number of PAN descriptors stored equals this
 * implementation-specified maximum or when [aBaseSuperframeDuration*(2n + 1)]
 * symbols, where n is the value of the ScanDuration parameter, have elapsed.
 * If the latter condition has been satisfied, the channel is considered to
 * have been scanned. Where possible, the scan is repeated on each channel and
 * terminates when the number of PAN descriptors stored equals the
 * implementation-specified maximum or when every channel in the set of
 *  available channels has been scanned.
 *
 * A passive scan, like an active scan, is used to locate all coordinators
 * transmitting beacon frames within the POS of the scanning device; the
 * difference is that the passive scan is a receive-only operation and does not
 * transmit the beacon request command. The passive scan is performed on each
 * channel by the MLMEs enabling its receiver and recording the information
 * contained in each received beacon in a PAN descriptor structure
 * (see Table 41 in 7.1.5.1.1). A device will be able to store between one and
 * an implementation-specified maximum number of PAN descriptors. The passive
 * scan on a particular channel terminates when the number of PAN descriptors
 * stored equals this implementation-specified maximum or when
 * [aBaseSuperframeDuration * (2n + 1)] symbols, where n is the value of the
 * ScanDuration parameter, have elapsed. If the latter condition has been
 * satisfied, the channel is considered to have been scanned. Where possible,
 * the scan is repeated on each channel and terminates when the number of PAN
 * descriptors stored equals the implementation-specified maximum or when
 * every channel in the set of available channels has been scanned.
 *
 * An orphan scan is used to locate the coordinator with which the scanning
 * device had previously associated. The orphan scan is performed on each
 * channel by the MLME first sending an orphan notification command
 * (see 7.3.2.3). The MLME then enables its receiver for at most
 * aResponseWaitTime symbols. If the device receives a coordinator realignment
 * command within this time, the MLME will disable its receiver. Otherwise, the
 * scan is repeated on the next channel in the channel list. The scan
 * terminates when the device receives a coordinator realignment command
 * (see 7.3.2.5) or when every channel in the set of available channels has
 * been scanned.
 *
 * The results of an ED scan are recorded in a list of ED values and reported
 * to the next higher layer through the MLME-SCAN.confirm primitive with a
 * status of MAC_SUCCESS. The results of an active or passive scan are recorded
 * in a set of PAN descriptor values and reported to the next higher layer
 * through the MLME-SCAN.confirm primitive. If no beacons were found, the
 * MLME-SCAN.confirm primitive will contain a null set of PAN descriptor
 * values and a status of NO_BEACON. Otherwise, the MLME-SCAN.confirm primitive
 * will contain the set of PANDescriptor values found, a list of unscanned
 * channels, and a status of MAC_SUCCESS.
 *
 * The results of an orphan scan are reported to the next higher layer through
 * the MLME-SCAN.confirm primitive. If successful, the MLME-SCAN.confirm
 * primitive will contain a status of MAC_SUCCESS. If the device did not receive
 * a
 * coordinator realignment command, MLME-SCAN.confirm primitive will contain
 * a status of NO_BEACON. In either case, the PANDescriptorList and
 * EnergyDetectList parameters of the MLMESCAN.confirm primitive are null.
 *
 * If any parameter in the MLME-SCAN.request primitive is not supported or is
 * out of range, the MAC sublayer will issue the MLME-SCAN.confirm primitive
 * with a status of MAC_INVALID_PARAMETER.
 *
 * @param m The MLME_SCAN.request message
 */
void MAC_MLME_ScanRequest(void *m)
{
	MLME_ScanReq_t *msr = (MLME_ScanReq_t *)MAC_BUFFER_POINTER(
			(buffer_t *)m);
	MLME_ScanConf_t *msc;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
    PibValue_t pib_value;
	/* Save the original channel. */
    pibStatus = PHY_PibGet(phyCurrentChannel, (uint8_t *)&pib_value);
	macScanOrigChannel = pib_value.pib_value_8bit;

	/* Save the original channel page. */
    pibStatus = PHY_PibGet(phyCurrentPage, (uint8_t *)&pib_value);
	macScanOrigPage = pib_value.pib_value_8bit;
    (void)pibStatus;

	/* Save the scan request parameters */
	scanDuration = msr->ScanDuration;
	scanType = msr->ScanType;
	scanChannels = msr->ScanChannels;
	scanCurrPage = msr->ChannelPage;

	msc = (MLME_ScanConf_t *)((void *)msr);

	/*
	 * Store the scan request buffer reused to create the corresponding
	 * scan confirmation
	 */
	macConfBufPtr = m;

	msc->cmdcode = MLME_SCAN_CONFIRM;
	msc->ScanType = scanType;
	msc->UnscannedChannels = scanChannels;
	msc->ChannelPage = scanCurrPage;
	msc->ResultListSize = 0;
	msc->scanResultList[0].edValue[0] = 0;

	if ((MAC_POLL_IDLE != macPollState) ||
			(MAC_SCAN_IDLE != macScanState)
			) { 
		/* Ignore scan request while being in a polling state or
		 * scanning. */
		msc->status = (uint8_t)MAC_INVALID_PARAMETER;

		/* Append the scan confirmation message to the MAC-NHLE queue */
		qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)m);
        WPAN_PostTask();
        (void)qmmStatus;

		return;
		/* no break here due to return */
	}

	/*
	 * Check for invalid channels to scan.
	 * This can be either an emtpy scan mask, or a scan mask that contains
	 * invalid channels for this band.
	 */

	/*
	 * Checck also for a scan duration that is lower than
	 * the max. beacon order.
	 */
	if ((0U == scanChannels) ||
			((scanChannels & INVERSE_CHANNEL_MASK) != 0U) ||
			(scanDuration > BEACON_NETWORK_MAX_BO)
			) {
		msc->status = (uint8_t)MAC_INVALID_PARAMETER;

		/* Append the scan confirm message to the MAC-NHLE queue */
		qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)m);
        WPAN_PostTask();
        (void)qmmStatus;

		return;
	}

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 0)

	/*
	 * If ED scan is not supported, the ED scan request
	 * will be rejected before node will be woken up.
	 */
	if (MLME_SCAN_TYPE_ED == scanType) {
		msc->status = (uint8_t)MAC_INVALID_PARAMETER;

		/* Append the scan confirm message to the MAC-NHLE queue */
		qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)m);
        WPAN_PostTask();
        (void)qmmStatus;

		return;
	}
#endif /*  (MAC_SCAN_ED_REQUEST_CONFIRM == 0) */

	/* wake up radio first */
	MAC_TrxWakeup();

	MAC_AwakeScan((buffer_t *)m);
} /* MAC_MLME_ScanRequest() */

#if (MAC_SCAN_ED_REQUEST_CONFIRM == 1)

/**
 * @brief ED scan callback function.
 *
 * This function is a callback function from the TAL after ED scan
 * is performed on a specified channel.
 *
 * @param energy_level Maximum energy on the channel
 */
void PHY_EdEndCallback(uint8_t energyLevel)
{
	MAKE_MAC_NOT_BUSY();

	MLME_ScanConf_t *msc;

	/*
	 * Scan request buffer is used to generate a scan confirm for the ED
	 * scan
	 * which is stored in macConfBufPtr.
	 */
	msc = (MLME_ScanConf_t *)MAC_BUFFER_POINTER(
			(buffer_t *)((void *)macConfBufPtr));

	uint8_t n_eds;

	n_eds = msc->ResultListSize;
	msc->scanResultList[0].edValue[n_eds] = energyLevel;
	msc->ResultListSize++;
	msc->scanResultList[0].edValue[n_eds + 1U] = 0U;

	msc->UnscannedChannels &= ~(1UL << scanCurrChannel);

	/* Continue with next channel */
	ScanProceed(MLME_SCAN_TYPE_ED, (buffer_t *)((void *)macConfBufPtr));
}

#endif /* (MAC_SCAN_ED_REQUEST_CONFIRM == 1) */

/*
 * @brief Clean-up for scanning
 *
 * This is a helper function for clean-up functionality during the end of
 * scanning.
 *
 * @param buffer Pointer to MLME_ScanConf_t structure
 */
static void ScanCleanUp(buffer_t *buffer)
{
    PHY_Retval_t pibSetStatus = PHY_FAILURE;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
	/* Append the scan confirm message into the internal event queue */
	qmmStatus = qmm_queue_append(&macNhleQueue, buffer);
    (void)qmmStatus;

	/* Set original channel page and channel. */
	scanCurrPage = macScanOrigPage;

	pibSetStatus = SetPhyPibInternal(phyCurrentPage, (void *)&scanCurrPage);

	scanCurrChannel = macScanOrigChannel;

	pibSetStatus = SetPhyPibInternal(phyCurrentChannel, (void *)&scanCurrChannel);

	macScanState = MAC_SCAN_IDLE;

	/* Set radio to sleep if allowed */
	MAC_SleepTrans();
    (void)pibSetStatus;
    
    WPAN_PostTask();
}

#if (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)

/**
 * @brief Processing a coordinator realignment command frame during Orphan scan
 *
 * This function processes a coordinator realignment command frame received
 * as a response to the reception of an orphan notification
 * command frame (i.e. while being in the middle of an orphan scan procedure).
 * The PAN ID, coord. short address, logical channel, and the device's new
 * short address will be written to the PIB.
 *
 * @param ind Frame reception buffer
 */
void MAC_ProcessOrphanRealign(buffer_t *bufPtr)
{
	MAC_Retval_t setStatus = FAILURE;
    PHY_Retval_t pibSetStatus = PHY_FAILURE;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;

	/* Device received a coordinator realignment during an orphan scan */

	/* Free the buffer used for sending orphan notification command */
	bmm_buffer_free((buffer_t *)((void *)macScanCmdBufPtr));
	macScanCmdBufPtr = NULL;

	/*
	 * Scan confirm with scan type orphan is given to the NHLE using the
	 * scan request buffer, which was stored in macConfBufPtr.
	 */
	MLME_ScanConf_t *msc
		= (MLME_ScanConf_t *)MAC_BUFFER_POINTER(
			(buffer_t *)((void *)macConfBufPtr));

	msc->cmdcode = MLME_SCAN_CONFIRM;
	msc->status = (uint8_t)MAC_SUCCESS;
	msc->ScanType = MLME_SCAN_TYPE_ORPHAN;
	msc->UnscannedChannels = 0;
	msc->ResultListSize = 0;

	/* Append the scan confirmation message to the MAC-NHLE queue */
	qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)((void *)macConfBufPtr));

	macScanState = MAC_SCAN_IDLE;
    (void)qmmStatus;

	/* Set radio to sleep if allowed */
	MAC_SleepTrans();

	/*
	 * The buffer in which the coordinator realignment is received is
	 * freed up
	 */
	bmm_buffer_free(bufPtr);

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
	if ((macParseData.fcf & FCF_FRAME_VERSION_2006) == FCF_FRAME_VERSION_2006) {
		pibSetStatus
			= SetPhyPibInternal(phyCurrentPage,
				(void *)&macParseData.macPayloadData.coordRealignData.channelPage);
	}

	pibSetStatus = SetPhyPibInternal(phyCurrentChannel,
			(void *)&macParseData.macPayloadData.coordRealignData.logicalChannel);

	if(pibSetStatus == PHY_SUCCESS){
        setStatus = MAC_SUCCESS;
    }
    ScanSetComplete(setStatus);
    WPAN_PostTask();
    
} /* MAC_ProcessOrphanRealign() */

#endif /* (MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1) */

#endif /* (MAC_SCAN_SUPPORT == 1) */

/* EOF */
