/*******************************************************************************
  MAC API Source

  File Name:
    mac_api.c
  Summary:
    MAC API functions
  Description:
    This file contains MAC API functions.

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
#include "phy.h"
#include "bmm.h"
#include "qmm.h"
#include "mac_api.h"
#include "stack_config.h"
#include "mac_config.h"
#include "mac.h"
#include "pal.h"
#include "mac_internal.h"
#include "mac_msg_types.h"
#include "mac_msg_const.h"
#include "mac_tasks.h"
/* === Types =============================================================== */

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/**
 * Queue used by MAC for communication to next higher layer.
 */
queue_t macNhleQueue;

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

MAC_Retval_t WPAN_Init(void)
{
	/* Init queue used for MAC to next higher layer communication */
#ifdef ENABLE_QUEUE_CAPACITY
	qmm_queue_init(&macNhleQueue, MAC_NHLE_QUEUE_CAPACITY);
#else
	qmm_queue_init(&macNhleQueue);
#endif  /* ENABLE_QUEUE_CAPACITY */

	/*
	 * Initialize MAC.
	 */
	return MAC_Init();
}

bool WPAN_Task(void)
{
	bool eventProcessed;
	uint8_t *event = NULL;

	/* mac_task returns true if a request was processed completely */
	eventProcessed = MAC_TaskHandler();

	/*
	 * MAC to NHLE event queue should be dispatched
	 * irrespective of the dispatcher state.
	 */
	event = (uint8_t *)qmm_queue_remove(&macNhleQueue, NULL);

	/* If an event has been detected, handle it. */
	if (NULL != event) {
		DispatchEvent(event);
		eventProcessed = true;
	}

	return (eventProcessed);
}

/* MAC level API */

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
bool WPAN_MCPS_Datareq(uint8_t SrcAddrMode,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t msduHandle,
		uint8_t TxOptions,
		uint8_t SecurityLevel,
		uint8_t *KeySource,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else   /* No MAC_SECURITY */
bool WPAN_MCPS_Datareq(uint8_t SrcAddrMode,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t msduHandle,
		uint8_t TxOptions)
#endif  /* MAC_SECURITY */
{
	buffer_t *buffer_header;
	MCPS_DataReq_t *mcpsDataReq;
	uint8_t *payloadPos;

	if (msduLength > aMaxMACPayloadSize) {
		/* Frame is too long and thus rejected immediately */
		return false;
	}

	/* Allocate a large buffer for mcps data request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */

	mcpsDataReq =  (MCPS_DataReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* Construct MCPS_DataReq_t message */
	mcpsDataReq->cmdcode = MCPS_DATA_REQUEST;

	/* Source addr mode */
	mcpsDataReq->SrcAddrMode = SrcAddrMode;

	/* Destination addr spec */
	mcpsDataReq->DstAddrMode = DstAddrSpec->AddrMode;

	mcpsDataReq->DstPANId = DstAddrSpec->PANId;

	if ((WPAN_ADDRMODE_SHORT == mcpsDataReq->DstAddrMode) ||
			(WPAN_ADDRMODE_LONG  == mcpsDataReq->DstAddrMode)) {
		/*
		 * In case a short address is indicated, but the address is not
		 * properly set, the entire address is first cleared.
		 */
		ADDR_COPY_DST_SRC_64(mcpsDataReq->DstAddr,
				DstAddrSpec->Addr.longAddress);
	}

	/* Other fields */
	mcpsDataReq->msduHandle = msduHandle;
	mcpsDataReq->TxOptions = TxOptions;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	mcpsDataReq->SecurityLevel = SecurityLevel;
	mcpsDataReq->KeySource = KeySource;
	mcpsDataReq->KeyIdMode = KeyIdMode;
	mcpsDataReq->KeyIndex = KeyIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	mcpsDataReq->msduLength = msduLength;

	/* Find the position where the data payload is to be updated */
	payloadPos = ((uint8_t *)mcpsDataReq) +
			(LARGE_BUFFER_SIZE - FCS_LEN - msduLength);

	/* Copy the payload to the end of buffer */
	(void)memcpy(payloadPos, msdu, msduLength);

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MCPS-DATA.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
bool WPAN_MCPS_PurgeReq(uint8_t msduHandle)
{
	buffer_t *buffer_header;
	MCPS_PurgeReq_t *mcpsPurgeReq;

	/* Allocate small buffer for mcps purge request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mcpsPurgeReq = (MCPS_PurgeReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* Update the purge request structure */
	mcpsPurgeReq->cmdcode = MCPS_PURGE_REQUEST;
	mcpsPurgeReq->msduHandle = msduHandle;

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MCPS-PURGE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
       **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
bool WPAN_MLME_AssociateReq(uint8_t LogicalChannel,
		uint8_t ChannelPage,
		WPAN_AddrSpec_t *CoordAddrSpec,
		uint8_t CapabilityInformation)
{
	buffer_t *buffer_header;
	MLME_AssociateReq_t *mlmeAssociateReq;

	/* Allocate a buffer for mlme associate request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Check for buffer availability */
	if (NULL == buffer_header) {
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeAssociateReq = (MLME_AssociateReq_t *)MAC_BUFFER_POINTER(
			buffer_header);

	/* Construct MLME_AssociateReq_t message */
	mlmeAssociateReq->cmdcode = MLME_ASSOCIATE_REQUEST;

	/* Operating channel */
	mlmeAssociateReq->LogicalChannel = LogicalChannel;

	/* Coordinator address spec */
	mlmeAssociateReq->CoordAddrMode = CoordAddrSpec->AddrMode;

	mlmeAssociateReq->CoordPANId = CoordAddrSpec->PANId;


	ADDR_COPY_DST_SRC_64(mlmeAssociateReq->CoordAddress.longAddress,
			CoordAddrSpec->Addr.longAddress);

	/* Other fields */
	mlmeAssociateReq->CapabilityInformation = CapabilityInformation;
	mlmeAssociateReq->ChannelPage = ChannelPage;

	/* Insert service message into NHLE MLME queue */
#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-ASSOCIATE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
bool WPAN_MLME_AssociateResp(uint64_t DeviceAddress,
		uint16_t AssocShortAddress,
		uint8_t status)
{
	buffer_t *buffer_header;
	MLME_AssociateResp_t *mlmeAssociateResp;

	/* Allocate a small buffer for association response */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeAssociateResp = (MLME_AssociateResp_t *)MAC_BUFFER_POINTER(
			buffer_header);

	/* Construct MLME_AssociateResp_t message */
	mlmeAssociateResp->cmdcode = MLME_ASSOCIATE_RESPONSE;

	/* Other fields */
	mlmeAssociateResp->DeviceAddress  = DeviceAddress;
	mlmeAssociateResp->AssocShortAddress = AssocShortAddress;
	mlmeAssociateResp->status = status;

	/* Insert MLME_AssociateResp_t into NHLE MAC queue */
#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-ASSOCIATE.response is not appended into NHLE MAC
		 * queue, hence free the buffer allocated
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
bool WPAN_MLME_DisassociateReq(WPAN_AddrSpec_t *DeviceAddrSpec,
		uint8_t DisassociateReason,
		bool TxIndirect)
{
	buffer_t *buffer_header;
	MLME_DisassociateReq_t *mlmeDisassociateReq;

	/* Allocate a small buffer for disassociation request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeDisassociateReq = (MLME_DisassociateReq_t *)MAC_BUFFER_POINTER(
			buffer_header);

	/* Update the disassociate request structure */
	mlmeDisassociateReq->cmdcode = MLME_DISASSOCIATE_REQUEST;
	mlmeDisassociateReq->DisassociateReason = DisassociateReason;
	mlmeDisassociateReq->DeviceAddrMode = DeviceAddrSpec->AddrMode;
	mlmeDisassociateReq->DevicePANId = DeviceAddrSpec->PANId;
	ADDR_COPY_DST_SRC_64(mlmeDisassociateReq->DeviceAddress,
			DeviceAddrSpec->Addr.longAddress);
	mlmeDisassociateReq->TxIndirect = TxIndirect;

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-DISASSOCIATE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
bool WPAN_MLME_OrphanResp(uint64_t OrphanAddress,
		uint16_t ShortAddress,
		bool AssociatedMember)
{
	buffer_t *buffer_header;
	MLME_OrphanResp_t *mlmeOrphanResp;

	/* Allocate a small buffer for orphan response */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeOrphanResp = (MLME_OrphanResp_t *)MAC_BUFFER_POINTER(
			buffer_header);

	/* Update the orphan response structure */
	mlmeOrphanResp->cmdcode = MLME_ORPHAN_RESPONSE;
	mlmeOrphanResp->OrphanAddress = OrphanAddress;
	mlmeOrphanResp->ShortAddress  = ShortAddress;
	mlmeOrphanResp->AssociatedMember = (uint8_t)AssociatedMember;

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-ORPHAN.response is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

bool WPAN_MLME_ResetReq(bool SetDefaultPib)
{
	buffer_t *buffer_header;
	MLME_ResetReq_t *mlmeResetReq;

	/* Allocate a small buffer for reset request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeResetReq = (MLME_ResetReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* Update the reset request structure */
	mlmeResetReq->cmdcode = MLME_RESET_REQUEST;
	mlmeResetReq->SetDefaultPIB = (uint8_t)SetDefaultPib;

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-RESET.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#if (MAC_GET_SUPPORT == 1)
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
bool WPAN_MLME_GetReq(uint8_t PIBAttribute, uint8_t PIBAttributeIndex)
#else
bool WPAN_MLME_GetReq(uint8_t PIBAttribute)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
{
	buffer_t *buffer_header;
	MLME_GetReq_t *mlmeGetReq;

	/* Allocate a large buffer for get request as maximum beacon payload
	 * should be accommodated */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Check for buffer availability */
	if (NULL == buffer_header) {
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeGetReq = (MLME_GetReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* Update the get request structure */
	mlmeGetReq->cmdcode = MLME_GET_REQUEST;
	mlmeGetReq->PIBAttribute = PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	mlmeGetReq->PIBAttributeIndex = PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-GET.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
bool WPAN_MLME_SetReq(uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex,
		void *PIBAttributeValue)
#else
bool WPAN_MLME_SetReq(uint8_t PIBAttribute,
		void *PIBAttributeValue)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
{
	buffer_t *buffer_header;
	MLME_SetReq_t *mlmeSetReq;
	uint8_t pibAttributeOctetNo;

	/*
	 * Allocate a large buffer for set request as maximum beacon payload
	 * should be accommodated
	 */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	/* Check for buffer availability */
	if (NULL == buffer_header) {
		return false;
	}

	/* Get size of PIB attribute to be set */
	pibAttributeOctetNo = MAC_GetPibAttributeSize(PIBAttribute);

	/* Get the buffer body from buffer header */
	mlmeSetReq = (MLME_SetReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* Construct MLME_SetReq_t message */
	mlmeSetReq->cmdcode = MLME_SET_REQUEST;

	/* Attribute and attribute value length */
	mlmeSetReq->PIBAttribute = PIBAttribute;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	mlmeSetReq->PIBAttributeIndex = PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/* Attribute value */

    if (pibAttributeOctetNo > 0U) {
        (void)memcpy(((void *)&(mlmeSetReq->PIBAttributeValue)),
			(void *)PIBAttributeValue,
			(size_t)pibAttributeOctetNo);
    }

	/* Insert message into NHLE MAC queue */
#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-SET.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#if (MAC_RX_ENABLE_SUPPORT == 1)
bool WPAN_MLME_RxEnableReq(bool DeferPermit,
		uint32_t RxOnTime,
		uint32_t RxOnDuration)
{
	buffer_t *buffer_header;
	MLME_RxEnableReq_t *mlmeRxEnableReq;

	/* Allocate a small buffer for rx enable request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeRxEnableReq = (MLME_RxEnableReq_t *)MAC_BUFFER_POINTER(
			buffer_header);

	/* Update the rx enable request structure */
	mlmeRxEnableReq->cmdcode = MLME_RX_ENABLE_REQUEST;
	mlmeRxEnableReq->DeferPermit = (uint8_t)DeferPermit;
	mlmeRxEnableReq->RxOnTime = RxOnTime;
	mlmeRxEnableReq->RxOnDuration = RxOnDuration;

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-RX-ENABLE.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if (MAC_SCAN_SUPPORT == 1)
bool WPAN_MLME_ScanReq(uint8_t ScanType,
		uint32_t ScanChannels,
		uint8_t ScanDuration,
		uint8_t ChannelPage)
{
	buffer_t *buffer_header;
	MLME_ScanReq_t *mlmeScanReq;

	/* Allocate a small buffer for scan request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeScanReq = (MLME_ScanReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* Update the scan request structure */
	mlmeScanReq->cmdcode = MLME_SCAN_REQUEST;
	mlmeScanReq->ScanType = ScanType;
	mlmeScanReq->ScanChannels = ScanChannels;
	mlmeScanReq->ScanDuration = ScanDuration;
	mlmeScanReq->ChannelPage = ChannelPage;

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-SCAN.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_SCAN_SUPPORT == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
bool WPAN_MLME_StartReq(uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage,
		uint8_t BeaconOrder,
		uint8_t SuperframeOrder,
		bool PANCoordinator,
		bool BatteryLifeExtension,
		bool CoordRealignment
		)
{
	buffer_t *buffer_header;
	MLME_StartReq_t *mlmeStartReq;

	/* Allocate a small buffer for start request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmeStartReq = (MLME_StartReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* Update the start request structure */
	mlmeStartReq->cmdcode = MLME_START_REQUEST;

	mlmeStartReq->PANId = PANId;
	mlmeStartReq->LogicalChannel = LogicalChannel;
	mlmeStartReq->BeaconOrder = BeaconOrder;
	mlmeStartReq->SuperframeOrder = SuperframeOrder;
	mlmeStartReq->PANCoordinator = (uint8_t)PANCoordinator;
	mlmeStartReq->BatteryLifeExtension = (uint8_t)BatteryLifeExtension;
	mlmeStartReq->CoordRealignment = (uint8_t)CoordRealignment;
	mlmeStartReq->ChannelPage = ChannelPage;

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-START.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated ad return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
bool WPAN_MLME_PollReq(WPAN_AddrSpec_t *CoordAddrSpec)
{
	buffer_t *buffer_header;
	MLME_PollReq_t *mlmePollReq;

	/* Allocate a small buffer for poll request */
	buffer_header = bmm_buffer_alloc(LARGE_BUFFER_SIZE);

	if (NULL == buffer_header) {
		/* Buffer is not available */
		return false;
	}

	/* Get the buffer body from buffer header */
	mlmePollReq = (MLME_PollReq_t *)MAC_BUFFER_POINTER(buffer_header);

	/* construct mlme_poll_req_t message */
	mlmePollReq->cmdcode = MLME_POLL_REQUEST;

	/* Other fileds. */
	mlmePollReq->CoordAddrMode = CoordAddrSpec->AddrMode;
	mlmePollReq->CoordPANId = CoordAddrSpec->PANId;

	if (WPAN_ADDRMODE_SHORT == CoordAddrSpec->AddrMode) {
		ADDR_COPY_DST_SRC_16(mlmePollReq->CoordAddress,
				CoordAddrSpec->Addr.shortAddress);
	} else {
		ADDR_COPY_DST_SRC_64(mlmePollReq->CoordAddress,
				CoordAddrSpec->Addr.longAddress);
	}

#ifdef ENABLE_QUEUE_CAPACITY
	if (QMM_SUCCESS != qmm_queue_append(&nhleMacQueue, buffer_header)) {
		/*
		 * MLME-POLL.request is not appended into NHLE MAC
		 * queue, hence free the buffer allocated and return false
		 */
		bmm_buffer_free(buffer_header);
		return false;
	}

#else
	qmm_queue_append(&nhleMacQueue, buffer_header);
#endif  /* ENABLE_QUEUE_CAPACITY */

	WPAN_PostTask();
    return true;
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */
/* EOF */
