/*******************************************************************************
  MAC Start Source

  File Name:
    mac_start.c
  Summary:
    MLME-START.request
  Description:
    This file implements the MLME-START.request.

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

/* === Globals ============================================================== */

#if (MAC_START_REQUEST_CONFIRM == 1) 

static MLME_StartReq_t msrParams;    /* Intermediate start parameters */

/* === Prototypes =========================================================== */

/* === Implementation ======================================================= */

/*
 * @brief Internal function to generate confirmation
 * for MLME_START.request
 *
 * @param start_buf_ptr Pointer to MLME_START.request
 * @param start_req_status Status of the MLME_START.request
 */
static void Gen_MLME_StartConf(buffer_t *startBufPtr,
		uint8_t startReqStatus)
{
    qmm_status_t  status;
	/* Use the same start request buffer for start confirm */
	MLME_StartConf_t *msc = (MLME_StartConf_t*)MAC_BUFFER_POINTER(
			startBufPtr);

	msc->cmdcode = MLME_START_CONFIRM;
	msc->status = startReqStatus;

	/* The confirmation message is appended to the MAC-NHLE queue */
	status = qmm_queue_append(&macNhleQueue, startBufPtr);
    WPAN_PostTask();
    (void)status;
}

/*
 * @brief Checks the parameters of a MLME_START.request
 *
 * This function validates the parameters of the MLME_START.request.
 *
 * @param msg Pointer to the MLME_START.request message which holds the
 * start parameters
 *
 * @return true if parameters are valid, false otherwise
 */
static bool checkStartParameter(MLME_StartReq_t *msg)
{
	bool paramValidationStatus = false;

	/*
	 * In a build without beacon support the beacon order and/or
	 * superframe order needs to be 15, since only a nonbeacon-enabled
	 * network
	 * can be created.
	 * The following code is valid for starting a new network as well as for
	 * for a coordinator realignment attempt.
	 */
	if ((msg->BeaconOrder != NON_BEACON_NWK) ||
			(msg->SuperframeOrder != NON_BEACON_NWK)) {
		/* This is not allowed in a build without beacon support. */
		return paramValidationStatus;
	}

	/*
	 * Value of beacon order has to be less than or equal to 15 (Non beacon
	 * Network). The value of superframe has to be either less than equal to
	 * beacon order.
	 */
	if ((msg->BeaconOrder <= NON_BEACON_NWK) &&
			((msg->SuperframeOrder <= msg->BeaconOrder) ||
			(NON_BEACON_NWK == msg->SuperframeOrder))) {
		/*
		 * Coordinator realignment command can only be given by a
		 * PAN coordinator or cordinator (which is associated to a
		 * PAN coordinator).
		 */
		if (((msg->CoordRealignment) == 1U) &&
				((MAC_ASSOCIATED == macState) ||
				(MAC_IDLE == macState))
				) {
			/*
			 * We are neigher the requested to be the PAN
			 * Coordinator,
			 * nor are we associated, so Realignment is not allowed
			 * at
			 * this stage.
			 */
			paramValidationStatus = false;
		} else {
			paramValidationStatus = true;
		}
	}

	return paramValidationStatus;
}

/**
 * @brief The MLME-START.request primitive makes a request for the device to
 * start using a new superframe configuration
 *
 * @param m Pointer to MLME_START.request message issued by the NHLE
 */
void MAC_MLME_StartRequest(void *m)
{
    PHY_Retval_t channelSetStatus, channelPageSetStatus;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    PibValue_t pib_value;
	MLME_StartReq_t *msg
		= (MLME_StartReq_t *)MAC_BUFFER_POINTER((buffer_t *)m);

	/*
	 * The MLME_START.request parameters are copied into a global variable
	 * structure, which is used by checkStartParameter() function.
	 */
	(void)memcpy(&msrParams, msg, sizeof(msrParams));

	pibStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_value);
    if (BROADCAST == pib_value.pib_value_16bit) {
		/*
		 * The device is void of short address. This device cannot start
		 * a
		 * network, hence a confirmation is given back.
		 */
		Gen_MLME_StartConf((buffer_t *)m, (uint8_t)MAC_NO_SHORT_ADDRESS);
		return;
	}
	if (!checkStartParameter(msg)) {
		/*
		 * The MLME_START.request parameters are invalid, hence
		 * confirmation
		 * is given to NHLE.
		 */
		Gen_MLME_StartConf((buffer_t *)m, (uint8_t)MAC_INVALID_PARAMETER);
	} else
	{
		/*
		 * All the start parameters are valid, hence MLME_START.request
		 * can
		 * proceed.
		 */
		pibStatus = SetPhyPibInternal(mac_i_pan_coordinator,
				(void *)&(msg->PANCoordinator));

		if ((msrParams.CoordRealignment) == 1U) {
			/* First inform our devices of the configuration change
			**/
			if (!MAC_TxCoordRealignmentCommand(
					COORDINATORREALIGNMENT,
					(buffer_t *)m,
					msrParams.PANId,
					msrParams.LogicalChannel,
					msrParams.ChannelPage)) {
				/*
				 * The coordinator realignment command was
				 * unsuccessful,
				 * hence the confiramtion is given to NHLE.
				 */
				Gen_MLME_StartConf((buffer_t *)m,
						(uint8_t)MAC_INVALID_PARAMETER);
			}
		} else {
			/* This is a normal MLME_START.request. */

			/* The new PIBs are set at the TAL. */
			pibStatus = SetPhyPibInternal(macBeaconOrder,
					(void *)&(msg->BeaconOrder));

			/* If macBeaconOrder is equal to 15, set also
			 * macSuperframeOrder to 15. */
			if (msg->BeaconOrder == NON_BEACON_NWK) {
				msg->SuperframeOrder = NON_BEACON_NWK;
			}

			pibStatus = SetPhyPibInternal(macSuperframeOrder,
					(void *)&(msg->SuperframeOrder));

			/* Wake up radio first */
			MAC_TrxWakeup();

			/* MLME_START.request parameters other than BO and SO
			 * are set at TAL */
			pibStatus = SetPhyPibInternal(macPANId,
					(void *)&(msrParams.PANId));
			channelPageSetStatus
				= SetPhyPibInternal(phyCurrentPage,
					(void *)&(msrParams.ChannelPage));

			channelSetStatus
				= SetPhyPibInternal(phyCurrentChannel,
					(void *)&(msrParams.LogicalChannel));

			pibStatus = SetPhyPibInternal(mac_i_pan_coordinator,
					(void *)&(msrParams.PANCoordinator));

			if ((PHY_SUCCESS == channelPageSetStatus) &&
					(PHY_SUCCESS == channelSetStatus) &&
					(PHY_RX_ON == PHY_RxEnable(PHY_STATE_RX_ON))
					) {
				if ((msrParams.PANCoordinator) == 1U) {
					macState = MAC_PAN_COORD_STARTED;
				} else {
					macState = MAC_COORDINATOR;
				}

				Gen_MLME_StartConf((buffer_t *)m, (uint8_t)MAC_SUCCESS);

			} else {
				/* Start of network failed. */
				Gen_MLME_StartConf((buffer_t *)m,
						(uint8_t)MAC_INVALID_PARAMETER);
			}

			/* Set radio to sleep if allowed */
			MAC_SleepTrans();
		}
	}
    (void)pibStatus;
}

/**
 * @brief Continues handling of MLME_START.request (Coordinator realignment)
 * command
 *
 * This function is called once the coordinator realignment command is
 * sent out to continue the handling of the MLME_START.request command.
 *
 * @param txStatus Status of the coordinator realignment command
 *                  transmission
 * @param buf_ptr Buffer for start confirmation
 */
void MAC_CoordRealignmentCommandTxSuccess(uint8_t txStatus,
		buffer_t *bufPtr)
{
	uint8_t conf_status = (uint8_t)MAC_INVALID_PARAMETER;
	PHY_Retval_t channelSetStatus, channelPageSetStatus;
    PHY_Retval_t pibSetStatus = PHY_FAILURE;

	if (txStatus == ((uint8_t)MAC_SUCCESS)) {
		/* The parameters of the existing PAN are updated. */
		channelPageSetStatus
			= SetPhyPibInternal(phyCurrentPage,
				(void *)&(msrParams.ChannelPage));
		channelSetStatus
			= SetPhyPibInternal(phyCurrentChannel,
				(void *)&msrParams.LogicalChannel);
		if ((PHY_SUCCESS == channelSetStatus) &&
				(PHY_SUCCESS == channelPageSetStatus)) {
			conf_status = (uint8_t)MAC_SUCCESS;

			pibSetStatus = SetPhyPibInternal(macPANId,
					(void *)&(msrParams.PANId));
		}
	}

	Gen_MLME_StartConf(bufPtr, conf_status);

	/* Set radio to sleep if allowed */
	MAC_SleepTrans();
    (void)pibSetStatus;
} /* MAC_CoordRealignmentCommandTxSuccess() */

#endif /* (MAC_START_REQUEST_CONFIRM == 1) || defined(__DOXYGEN__) */

/* EOF */
