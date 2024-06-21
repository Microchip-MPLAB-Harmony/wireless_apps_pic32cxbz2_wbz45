/*******************************************************************************
  MAC Misc Source

  File Name:
    mac_misc.c
  Summary:
    Miscellaneous MAC sublayer components
  Description:
    This file implements miscellaneous MAC sublayer components.

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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "pal.h"
#include "bmm.h"
#include "qmm.h"
#include "phy.h"
#include "mac_msg_const.h"
#include "mac_api.h"
#include "mac_msg_types.h"
#include "mac_data_structures.h"
#include "stack_config.h"
#include "mac_internal.h"
#include "mac.h"
#include "mac_config.h"
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
#include "mac_security.h"
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
#ifdef STB_ON_SAL
#include "stb.h"
#endif

/* === Macros =============================================================== */

/* === Globals ============================================================= */

#if (NUMBER_OF_MAC_TIMERS > 0)
#ifdef BEACON_SUPPORT
uint8_t Timer_BeaconTrackingPeriod;
uint8_t Timer_Superframe;
uint8_t Timer_MissedBeacon;
    #if (MAC_START_REQUEST_CONFIRM == 1)
uint8_t Timer_Beacon;
uint8_t Timer_BeaconPreparation;
    #endif /* (MAC_START_REQUEST_CONFIRM == 1) */
#ifdef GTS_SUPPORT
uint8_t Timer_CAP;
#endif /* GTS_SUPPORT */
#endif  /* BEACON_SUPPORT / No BEACON_SUPPORT */

#if (MAC_INDIRECT_DATA_BASIC == 1)
uint8_t Timer_PollWaitTime;
    #if (MAC_INDIRECT_DATA_FFD == 1)
uint8_t Timer_DataPersistence;
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)
uint8_t Timer_ScanDuration;
#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
uint8_t Timer_RxEnable;
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */

/* === Prototypes ========================================================== */

static void DoInitPib(void);
static void FlushQueues(void);
static uint8_t MAC_Reset(uint8_t initPib);
static void MAC_SoftReset(bool initPib);
static void ResetGlobals(void);
static void SendResetConf(buffer_t *bufPtr, uint8_t status);

/* === Implementation ====================================================== */

/*
 * @brief Initializes the MAC global variables
 */
static void ResetGlobals(void)
{
	macBusy = false;
	macState = MAC_IDLE;
	macRadioSleepState = RADIO_AWAKE;
	macScanState = MAC_SCAN_IDLE;
	macSyncState = MAC_SYNC_NEVER;
	macPollState = MAC_POLL_IDLE;
	macLastDsn = 0;
	(void)memset((uint8_t *)&macLastSrcAddr, 0xFF, sizeof(macLastSrcAddr));
	/* macLastSrcAddr = 0xFFFFFFFFFFFFFFFFULL; */
	macRxEnabled = false;
}

/**
 * @brief Initializes the MAC sublayer
 *
 * @return MAC_SUCCESS  if TAL is intialized successfully else FAILURE
 */
MAC_Retval_t MAC_Init(void)
{
    uint64_t randomNumber; 
    PHY_Retval_t pibSetStatus = PHY_FAILURE;

#ifdef STB_ON_SAL
	STB_Init();
#endif
    
	if (MAC_SUCCESS != MAC_TimersInit()) {
		return FAILURE;
	}

	MAC_SoftReset(true);
       
    if (PAL_SUCCESS != PAL_GetRandomNumber((uint8_t*)&randomNumber, (uint16_t)sizeof(randomNumber))) {
		return FAILURE;
	}    
    pibSetStatus = PHY_PibSet(macIeeeAddress,(PibValue_t *) ((void *)&randomNumber));

	/* Set radio to sleep if allowed */
	MAC_SleepTrans();
    (void)pibSetStatus;

	/* Initialize the queues */
#ifdef ENABLE_QUEUE_CAPACITY
	qmm_queue_init(&nhleMacQueue, NHLE_MAC_QUEUE_CAPACITY);
	qmm_queue_init(&phyMacQueue, PHY_MAC_QUEUE_CAPACITY);
    #if (MAC_INDIRECT_DATA_FFD == 1)
	qmm_queue_init(&indirectDataQueue, INDIRECT_DATA_QUEUE_CAPACITY);
    #endif /* (MAC_INDIRECT_DATA_FFD == 1) */
#else
	qmm_queue_init(&nhleMacQueue);
	qmm_queue_init(&phyMacQueue);
    #if (MAC_INDIRECT_DATA_FFD == 1)
	qmm_queue_init(&indirectDataQueue);
    #endif /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* ENABLE_QUEUE_CAPACITY */

	return MAC_SUCCESS;
}

/**
 * @brief Resets the MAC helper variables and transition to idle state
 *
 * This function sets the MAC to idle state and resets
 * MAC helper variables
 */
void MAC_IdleTrans(void)
{
	/* Wake up radio first */
	MAC_TrxWakeup();

	{
		uint16_t defaultShortaddress = macShortAddress_def;
		uint16_t defaultPanid = macPANId_def;
        PHY_Retval_t pibSetStatus = PHY_FAILURE;
 
		pibSetStatus = SetPhyPibInternal(macShortAddress,
				(void *)&defaultShortaddress);
		pibSetStatus = SetPhyPibInternal(macPANId, (void *)&defaultPanid); 
        (void)pibSetStatus;
	}

	MAC_SoftReset(true);

	/* Set radio to sleep if allowed */
	MAC_SleepTrans();
}

/*
 * @brief Initializes MAC PIBs
 *
 * This function initializes all MAC PIBs to their defaults as stated by
 * 802.15.4.
 */
static void DoInitPib(void)
{
#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	macPib.mac_AssociatedPANCoord = macAssociatedPANCoord_def;
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */
#if (MAC_INDIRECT_DATA_BASIC == 1)
	macPib.mac_MaxFrameTotalWaitTime = macMaxFrameTotalWaitTime_def;
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */
	macPib.mac_ResponseWaitTime = macResponseWaitTime_def;
	macPib.mac_SecurityEnabled = macSecurityEnabled_def;

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	macPib.mac_AssociationPermit = macAssociationPermit_def;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	macPib.mac_BeaconPayloadLength = macBeaconPayloadLength_def;
	macPib.mac_BSN = (uint8_t)rand();
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)
	macPib.mac_TransactionPersistenceTime
		= macTransactionPersistenceTime_def;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

	macPib.mac_AutoRequest = macAutoRequest_def;
	macPib.mac_BattLifeExtPeriods = macBattLifeExtPeriods_def;
	(void)memset((uint8_t *)&macPib.mac_CoordExtendedAddress, 0,
			sizeof(macPib.mac_CoordExtendedAddress));
	/* mac_pib.mac_CoordExtendedAddress = (uint64_t)CLEAR_ADDR_64; */
	macPib.mac_CoordShortAddress = macCoordShortAddress_def;
	macPib.mac_DSN = (uint8_t)rand();
	macPib.mac_RxOnWhenIdle = macRxOnWhenIdle_def;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	/* TODO: Create a specific function for security PIB initialization? */
	macSecPib.KeyTableEntries = macKeyTableEntriesDef;
	macSecPib.DeviceTableEntries = macDeviceTableDef;
	macSecPib.SecurityLevelTableEntries = macSecurityLevelTableDef;
	macSecPib.FrameCounter = macFrameCounterDef;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#ifdef TEST_HARNESS
	macPib.privateIllegalFrameType = 1;
	macPib.privateNoDataAfterAssocReq = 0;
	macPib.privateVirtualPANs = 0;
#endif /* TEST_HARNESS */
}

/**
 * @brief Resets the MAC layer
 *
 * The MLME-RESET.request primitive allows the next higher layer to request
 * that the MLME performs a reset operation.
 *
 * @param m Pointer to the MLME_RESET.request given by the NHLE
 */
void MAC_MLME_ResetRequest(void *m)
{
	MLME_ResetReq_t *mrr
		= (MLME_ResetReq_t *)MAC_BUFFER_POINTER((buffer_t *)m);

	/* Wakeup the radio */
	MAC_TrxWakeup();

	/* Start MAC reset functionality */
	uint8_t status = MAC_Reset(mrr->SetDefaultPIB);

	/* Set radio to sleep if allowed */
	MAC_SleepTrans();

	/*
	 * As this is a mlme_reset request, all the requests, data (whether
	 * direct
	 * or indirect), incoming frames are removed from the queues
	 */
	FlushQueues();

	SendResetConf((buffer_t *)m, status);
} /* MAC_MLME_ResetRequest() */

/*
 * @brief Internal MAC reset function
 *
 * This function resets the MAC variables, stops all running timers and
 * initializes the PIBs.
 *
 * @param init_pib Boolean indicates whether PIB attributes shall be
 * initialized or not.
 *
 * @return Success or failure status
 */
static uint8_t MAC_Reset(uint8_t initPib)
{
	uint8_t status = (uint8_t)MAC_DISABLE_TRX_FAILURE;
    bool pib = (bool)(initPib);

	/* Reset TAL */
	status = (uint8_t)(PHY_Reset(pib));

	MAC_SoftReset(pib);

	return status;
}

/*
 * @brief Internal MAC soft reset function
 *
 * This function resets the MAC variables, stops all running timers and
 * initializes the PIBs.
 *
 * @param init_pib Boolean indicates whether PIB attributes shall be
 * initialized or not.
 */
static void MAC_SoftReset(bool initPib)
{
    PHY_TrxStatus_t trxStatus;
    MAC_Retval_t timerstatus = FAILURE;
	ResetGlobals();

	/* Set trx to PHY_TRX_OFF */
	trxStatus = PHY_RxEnable(PHY_STATE_TRX_OFF);
    (void)trxStatus;

	ENTER_CRITICAL_REGION();
	timerstatus = MAC_TimersStop();
    (void)timerstatus;
	LEAVE_CRITICAL_REGION();

	if (initPib == true) {
		DoInitPib();
	}
}

/**
 * @brief Puts the radio to sleep if this is allowed
 */
void MAC_SleepTrans(void)
{
	/* Go to sleep? */
	if ((!macPib.mac_RxOnWhenIdle) && (!macRxEnabled)) {
		MAC_TrxInitSleep();

	}
}

/*
 * @brief Flushes all queues
 */
static void FlushQueues(void)
{
	/* Flush NHLE MAC queue */
	qmm_queue_flush(&nhleMacQueue);

	/* Flush TAL_MAC queue */
	qmm_queue_flush(&phyMacQueue);

	/* Flush MAC-NHLE queue */
	qmm_queue_flush(&macNhleQueue);

#if (MAC_INDIRECT_DATA_FFD == 1)
	/* Flush MAC indirect queue */
	qmm_queue_flush(&indirectDataQueue);
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */
}

/*
 * @brief Sends mlme reset confirm
 *
 * @param m Buffer for reset confirm
 * @param status Status of MAC reset operation
 */
static void SendResetConf(buffer_t *bufPtr, uint8_t status)
{
	MLME_ResetConf_t *mrc;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;

	mrc = (MLME_ResetConf_t *)MAC_BUFFER_POINTER(bufPtr);

	mrc->status = status;
	mrc->cmdcode = MLME_RESET_CONFIRM;

	/* Append the mlme reset confirm to the MAC-NHLE queue */
	qmmStatus = qmm_queue_append(&macNhleQueue, bufPtr);
    WPAN_PostTask();
    (void)qmmStatus;
}

#if (MAC_COMM_STATUS_INDICATION == 1)

/**
 * @brief Creates a Communication Status Indication message to the upper layer
 *
 * @param status Status of the last operation
 * @param buf_ptr Buffer for Communication Status Indication to the NHLE
 */
void MAC_MLME_CommStatus(uint8_t status,
		buffer_t *bufPtr)
{
	/*
	 * The pointer to the destination address (received as one of the
	 * function
	 * paramters) points to a location in buf_ptr.
	 * As the same buffer is used to generate the comm status
	 * indication, it is typecasted to the 'MLME_CommStatusInd_t'. This
	 * may
	 * result in loosing destination address (which is still a part of this
	 * buffer), hence the destination address is backed up in a stack
	 * variable.
	 */
	MAC_FrameInfo_t *framePtr = (MAC_FrameInfo_t *)MAC_BUFFER_POINTER(bufPtr);
    PibValue_t pib_value;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
	uint64_t destinationAddress;
	(void)memcpy(((uint8_t *)&destinationAddress),
			&framePtr->mpdu[PL_POS_DST_ADDR_START],
			(uint8_t)sizeof(uint64_t));
	MLME_CommStatusInd_t *csi
		= (MLME_CommStatusInd_t *)MAC_BUFFER_POINTER(bufPtr);

	csi->cmdcode = MLME_COMM_STATUS_INDICATION;

    pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
	csi->PANId = pib_value.pib_value_16bit;

	csi->SrcAddrMode = FCF_LONG_ADDR;

	/* Initialize the source address */
    pibStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
	csi->SrcAddr = pib_value.pib_value_64bit;

	csi->DstAddrMode = FCF_LONG_ADDR;

	/* Initialize the destination address */
	csi->DstAddr = destinationAddress;

	csi->status = status;
    (void)pibStatus;

	qmmStatus = qmm_queue_append(&macNhleQueue, bufPtr);
    WPAN_PostTask();
    (void)qmmStatus;
    
}

#endif /* MAC_COMM_STATUS_INDICATION */

/**
 * @brief MAC function to put the radio to sleep mode
 */
void MAC_TrxInitSleep(void)
{
	/* If the radio is not sleeping, it is put to sleep */
	if (RADIO_AWAKE == macRadioSleepState) {

#ifdef ENABLE_DEEP_SLEEP
		if (PHY_SUCCESS == PHY_TrxSleep(DEEP_SLEEP_MODE))
#else
		if (PHY_SUCCESS == PHY_TrxSleep(SLEEP_MODE_1))
#endif
		{
			macRadioSleepState = RADIO_SLEEPING;
		} 
	}
}

/**
 * @brief MAC function to wake-up the radio from sleep state
 */
void MAC_TrxWakeup(void)
{
	/* If the radio is sleeping, it is woken-up */
	if (RADIO_SLEEPING == macRadioSleepState) {
		if (PHY_FAILURE != PHY_TrxWakeup()) {
			macRadioSleepState = RADIO_AWAKE;
		}
	}
}

MAC_Retval_t MAC_TimersInit(void)
{
#if (NUMBER_OF_MAC_TIMERS > 0)

#if (MAC_INDIRECT_DATA_BASIC == 1)
	if (PAL_SUCCESS != PAL_TimerGetId(&Timer_PollWaitTime)) {
		return FAILURE;
	}

    #if (MAC_INDIRECT_DATA_FFD == 1)
	if (PAL_SUCCESS != PAL_TimerGetId(&Timer_DataPersistence)) {
		return FAILURE;
	}
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)
	if (PAL_SUCCESS != PAL_TimerGetId(&Timer_ScanDuration)) {
		return FAILURE;
	}

#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
	if (PAL_SUCCESS != PAL_TimerGetId(&Timer_RxEnable)) {
		return FAILURE;
	}
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */
	return MAC_SUCCESS;
}

MAC_Retval_t MAC_TimersStop(void)
{
#if (NUMBER_OF_MAC_TIMERS > 0)

#if (MAC_INDIRECT_DATA_BASIC == 1)
    if (PAL_SUCCESS != PAL_TimerStop(Timer_PollWaitTime)) {
		return FAILURE;
	}
    #if (MAC_INDIRECT_DATA_FFD == 1)
	if (PAL_SUCCESS != PAL_TimerStop(Timer_DataPersistence)) {
		return FAILURE;
	}
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)
	if (PAL_SUCCESS != PAL_TimerStop(Timer_ScanDuration)) {
		return FAILURE;
	}
#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
	if (PAL_SUCCESS != PAL_TimerStop(Timer_RxEnable)) {
		return FAILURE;
	}
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */
	return MAC_SUCCESS;
}

/* EOF */
