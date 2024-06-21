/*******************************************************************************
  MAC callback wrapper Source

  File Name:
    mac_callback_wrapper.c
  Summary:
    Wrapper code for MAC callback functions
  Description:
    This file Wrapper code for MAC callback functions.

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

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */

/**
 * @brief Wrapper function for messages of type mcps_data_ind_t
 *
 * This function is a callback for mcps data indication
 *
 * @param m Pointer to message structure
 */
void MAC_MCPS_DataInd(void *m)
{
	MCPS_DataInd_t *pmsg;
	WPAN_AddrSpec_t srcAddr;
	WPAN_AddrSpec_t dstAddr;

	/* Get the buffer body from buffer header */
	pmsg = (MCPS_DataInd_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	/* Source address spec */
	srcAddr.AddrMode = pmsg->SrcAddrMode;
	srcAddr.PANId = pmsg->SrcPANId;
	srcAddr.Addr.longAddress = pmsg->SrcAddr;
	ADDR_COPY_DST_SRC_64(srcAddr.Addr.longAddress, pmsg->SrcAddr);

	/* Destination address spec */
	dstAddr.AddrMode = pmsg->DstAddrMode;
	dstAddr.PANId = pmsg->DstPANId;
	ADDR_COPY_DST_SRC_64(dstAddr.Addr.longAddress, pmsg->DstAddr);

	/* Callback function */
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	USR_MCPS_DataInd(&srcAddr,
			&dstAddr,
			pmsg->msduLength,
			pmsg->msdu,
			pmsg->mpduLinkQuality,
			pmsg->DSN,
    #ifdef ENABLE_TSTAMP
			pmsg->Timestamp,
    #endif  /* ENABLE_TSTAMP */
			pmsg->SecurityLevel,
			pmsg->KeyIdMode,
			pmsg->KeyIndex);
#else   /* No MAC_SECURITY */
	USR_MCPS_DataInd(&srcAddr,
			&dstAddr,
			pmsg->msduLength,
			pmsg->msdu,
			pmsg->mpduLinkQuality,
    #ifdef ENABLE_TSTAMP
			pmsg->DSN,
			pmsg->Timestamp);
    #else
			pmsg->DSN);
    #endif  /* ENABLE_TSTAMP */
#endif  /* MAC_SECURITY */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

/**
 * @brief Wrapper function for messages of type MCPS_DataConf_t
 *
 * This function is a callback for mcps data confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MCPS_DataConf(void *m)
{
	MCPS_DataConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MCPS_DataConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

#ifdef ENABLE_TSTAMP
	USR_MCPS_DataConf(pmsg->msduHandle, pmsg->status, pmsg->Timestamp);
#else
	USR_MCPS_DataConf(pmsg->msduHandle, pmsg->status);
#endif  /* ENABLE_TSTAMP */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

/**
 * @brief Wrapper function for messages of type MCPS_PurgeConf_t
 *
 * This function is a callback for mcps purge confirm.
 *
 * @param m Pointer to message structure
 */

void MAC_MCPS_PurgeConf(void *m)
{
	MCPS_PurgeConf_t *pmsg;

	/* Get the buffer body from buffer header */
    
	pmsg = (MCPS_PurgeConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));
    
	USR_MCPS_PurgeConf(pmsg->msduHandle, pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
       **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

/**
 * @brief Wrapper function for messages of type MLME_AssociateConf_t
 *
 * This function is a callback for mlme associate confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_AssociateConf(void *m)
{
	MLME_AssociateConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_AssociateConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	USR_MLME_AssociateConf(pmsg->AssocShortAddress, pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

/**
 * @brief Wrapper function for messages of type MLME_AssociateInd_t
 *
 * This function is a callback for mlme associate indication.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_AssociateInd(void *m)
{
	MLME_AssociateInd_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_AssociateInd_t *)MAC_BUFFER_POINTER((buffer_t *)m);
	USR_MLME_AssociateInd(pmsg->DeviceAddress,
			pmsg->CapabilityInformation);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

/**
 * @brief Wrapper function for messages of type MLME_BeaconNotifyInd_t
 *
 * This function is a callback for mlme beacon notify indication.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_BeaconNotifyInd(void *m)
{
	MLME_BeaconNotifyInd_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_BeaconNotifyInd_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	/* Callback function */
	USR_MLME_BeaconNotifyInd(pmsg->BSN,           /* BSN */
			&(pmsg->PANDescriptor),         /* PANDescriptor */
			pmsg->PendAddrSpec,             /* PendAddrSpec */
			pmsg->AddrList,                 /* AddrList */
			pmsg->sduLength,                /* sduLength */
			pmsg->sdu);                     /* sdu */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

#if (MAC_COMM_STATUS_INDICATION == 1)

/**
 * @brief Wrapper function for messages of type MLME_CommStatusInd_t
 *
 * This function is a callback for mlme comm status indication.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_CommStatusInd(void *m)
{
	MLME_CommStatusInd_t *pmsg;
	WPAN_AddrSpec_t srcAddr;
	WPAN_AddrSpec_t dstAddr;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_CommStatusInd_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	/* Source address spec */
	srcAddr.PANId = pmsg->PANId;
	srcAddr.AddrMode = pmsg->SrcAddrMode;
	ADDR_COPY_DST_SRC_64(srcAddr.Addr.longAddress, pmsg->SrcAddr);

	/* Destintion address spec */
	dstAddr.PANId = pmsg->PANId;
	dstAddr.AddrMode = pmsg->DstAddrMode;
	ADDR_COPY_DST_SRC_64(dstAddr.Addr.longAddress, pmsg->DstAddr);

	/* Callback function */
	USR_MLME_CommStatusInd(&srcAddr,
			&dstAddr,
			pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_COMM_STATUS_INDICATION == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type MLME_DisassociateConf_t
 *
 * This function is a callback for mlme disassociate confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_DisassociateConf(void *m)
{
	MLME_DisassociateConf_t *pmsg;
	WPAN_AddrSpec_t device_addr;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_DisassociateConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	/* Device address spec */
	device_addr.AddrMode = pmsg->DeviceAddrMode;
	device_addr.PANId = pmsg->DevicePANId;
	ADDR_COPY_DST_SRC_64(device_addr.Addr.longAddress,
			pmsg->DeviceAddress);

	USR_MLME_DisassociateConf(pmsg->status,
			&device_addr);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type MLME_DisassociateInd_t
 *
 * This function is a callback for mlme disassociate indication.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_DisassociateInd(void *m)
{
	MLME_DisassociateInd_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_DisassociateInd_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	USR_MLME_DisassociateInd(pmsg->DeviceAddress,
			pmsg->DisassociateReason);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type MLME_GetConf_t
 *
 * This function is a callback for mlme get confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_GetConf(void *m)
{
	MLME_GetConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_GetConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	/* Callback function */
	USR_MLME_GetConf(pmsg->status,
			pmsg->PIBAttribute,
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
			pmsg->PIBAttributeIndex,
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
			&pmsg->PIBAttributeValue);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

/**
 * @brief Wrapper function for messages of type MLME_OrphanInd_t
 *
 * This function is a callback for MLME orphan indication.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_OrphanInd(void *m)
{
	MLME_OrphanInd_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_OrphanInd_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	/* Callback function */
	USR_MLME_OrphanInd(pmsg->OrphanAddress);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/**
 * @brief Wrapper function for messages of type MLME_PollConf_t
 *
 * This function is a callback for MLME poll confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_PollConf(void *m)
{
	MLME_PollConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_PollConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	USR_MLME_PollConf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_INDIRECT_DATA_BASIC == 1) */

/**
 * @brief Wrapper function for messages of type MLME_ResetConf_t
 *
 * This function is a callback for MLME reset confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_ResetConf(void *m)
{
	MLME_ResetConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_ResetConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	USR_MLME_ResetConf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#if (MAC_RX_ENABLE_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type MLME_RxEnableConf_t
 *
 * This function is a callback for MLME RX enable confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_RxEnableConf(void *m)
{
	MLME_RxEnableConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_RxEnableConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	USR_MLME_RxEnableConf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if (MAC_SCAN_SUPPORT == 1)

/**
 * @brief Wrapper function for messages of type MLME_ScanConf_t
 *
 * This function is a callback for MLME scan confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_ScanConf(void *m)
{
	MLME_ScanConf_t*pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_ScanConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	/* Callback */
	USR_MLME_ScanConf(pmsg->status,
			pmsg->ScanType,
			pmsg->ChannelPage,
			pmsg->UnscannedChannels,
			pmsg->ResultListSize,
			&pmsg->scanResultList);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_SCAN_SUPPORT == 1) */

/**
 * @brief Wrapper function for messages of type MLME_SetConf_t
 *
 * This function is a callback for MLME set confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_SetConf(void *m)
{
	MLME_SetConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_SetConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	USR_MLME_SetConf(pmsg->status, pmsg->PIBAttribute,
			pmsg->PIBAttributeIndex);
#else
	USR_MLME_SetConf(pmsg->status, pmsg->PIBAttribute);
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#if (MAC_START_REQUEST_CONFIRM == 1)

/**
 * @brief Wrapper function for messages of type MLME_StartConf_t
 *
 * This function is a callback for MLME start confirm.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_StartConf(void *m)
{
	MLME_StartConf_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_StartConf_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	USR_MLME_StartConf(pmsg->status);

	/* Free the buffer */
	bmm_buffer_free((buffer_t *)m);
}

#endif /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_SYNC_LOSS_INDICATION == 1)

/**
 * @brief Wrapper function for messages of type MLME_SyncLossInd_t
 *
 * This function is a callback for MLME sync loss indication.
 *
 * @param m Pointer to message structure
 */
void MAC_MLME_SyncLossInd(void *m)
{
	MLME_SyncLossInd_t *pmsg;

	/* Get the buffer body from buffer header */
	pmsg = (MLME_SyncLossInd_t *)MAC_BUFFER_POINTER(((buffer_t *)m));

	USR_MLME_SyncLossInd(pmsg->LossReason,
			pmsg->PANId,
			pmsg->LogicalChannel,
			pmsg->ChannelPage);

	/* Uses static buffer for sync loss indication and it is not freed */
}

#endif /* (MAC_SYNC_LOSS_INDICATION == 1) */

/* EOF */
