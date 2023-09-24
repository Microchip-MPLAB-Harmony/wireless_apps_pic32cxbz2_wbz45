/*******************************************************************************
   User MAC Callback file - Coordinator

  Company:
    Microchip Technology Inc.

  File Name:
    usr_mac_callbacks.c

  Summary:
    This file contains the definitions for user callback function for 
    each request to MAC layer

  Description:
    For each MLME or MCPS request to MAC layer, ther will be a confirmation/indication messages from MAC layer. 
    Those callbacks are defined here and APP Queue is posted with respective messages in order to 
    initiate the application specific handling
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

/* === INCLUDES ============================================================ */
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "mac_api.h"
#include "ieee_mac_const.h"
#include "mac_msg_const.h"
#include "app.h"
#include "demo_app_rfd.h"
/* === IMPLEMENTATION ====================================================== */

#if defined(ENABLE_TSTAMP)
/*
 * Callback function usr_mcps_data_conf
 *
 * @param msduHandle  Handle of MSDU handed over to MAC earlier
 * @param status      Result for requested data transmission request
 * @param Timestamp   The time, in symbols, at which the data were transmitted
 *                    (only if time stamping is enabled).
 *
 */
void USR_MCPS_DataConf(uint8_t msduHandle,uint8_t status,uint32_t Timestamp)
#else
void USR_MCPS_DataConf(uint8_t msduHandle,
		uint8_t status)
#endif  /* ENABLE_TSTAMP */
{
    usr_mcps_data_cnf_t dataconf;
    APP_Msg_T appMsg;
    
    dataconf.msduHandle = msduHandle;
    dataconf.status     = status;
    #ifdef ENABLE_TSTAMP
        dataconf.Timestamp = Timestamp;
    #endif

    appMsg.msgId = APP_MSG_MAC_DATA_CONF;    
    (void)memcpy((void *)appMsg.msgData,(const void *)&dataconf,sizeof(dataconf));    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}


/*
 * @brief Callback function usr_mcps_data_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param msduLength       Number of octets contained in MSDU
 * @param msdu             Pointer to MSDU
 * @param mpduLinkQuality  LQI measured during reception of the MPDU
 * @param DSN              DSN of the received data frame.
 * @param Timestamp        The time, in symbols, at which the data were
 * received.
 *                         (only if timestamping is enabled).
 */
#ifdef MAC_SECURITY_ZIP
void USR_MCPS_DataInd(WPAN_AddrSpec_t *SrcAddrSpec,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
		uint8_t DSN,
#ifdef ENABLE_TSTAMP
		uint32_t Timestamp,
				#endif  /* ENABLE_TSTAMP */
		uint8_t SecurityLevel,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else /* No MAC_SECURITY */

void USR_MCPS_DataInd(WPAN_AddrSpec_t *SrcAddrSpec,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
#ifdef ENABLE_TSTAMP
		uint8_t DSN,
		uint32_t Timestamp)
#else
		uint8_t DSN)
#endif  /* ENABLE_TSTAMP */
#endif
{
    usr_mcps_data_ind_t dataInd;
    APP_Msg_T appMsg;
    
    dataInd.DSN = DSN;
    dataInd.DstAddrSpec = DstAddrSpec;
    dataInd.SrcAddrSpec = SrcAddrSpec;
    dataInd.msduLength = msduLength;
    dataInd.msdu = msdu;
    dataInd.mpduLinkQuality = mpduLinkQuality;
    
 #ifdef ENABLE_TSTAMP
    dataInd.Timestamp = Timestamp;
#endif  /* ENABLE_TSTAMP */
    
#ifdef MAC_SECURITY_ZIP
    dataInd.SecurityLevel = SecurityLevel;
    dataInd.KeyIdMode = KeyIdMode;
    dataInd.KeyIndex = KeyIndex;
   
#endif
         
    appMsg.msgId = APP_MSG_MAC_DATA_IND;
    
    (void)memcpy((void *)appMsg.msgData,(const void *)&dataInd,sizeof(dataInd));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

/*
 * Callback function usr_mcps_purge_conf
 *
 * @param msduHandle           Handle (id) of MSDU to be purged.
 * @param status               Result of requested purge operation.
 *
 * @return void
 *
 */
void USR_MCPS_PurgeConf(uint8_t msduHandle,
		uint8_t status)
{
}

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
        **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

/*
 * @brief Callback function usr_mlme_associate_conf
 *
 * @param AssocShortAddress    Short address allocated by the coordinator
 * @param status               Result of requested association operation
 */
void USR_MLME_AssociateConf(uint16_t AssocShortAddress,
		uint8_t status)
{
	usr_mlme_associate_conf_t associateConf;
    APP_Msg_T appMsg;
    
    associateConf.status = status;
    associateConf.AssocShortAddress = AssocShortAddress;
            
    appMsg.msgId = APP_MSG_MAC_ASSOCIATE_CONF;
    
    (void)memcpy((void *)appMsg.msgData,(const void *)&associateConf, sizeof(associateConf));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);

}

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

/*
 * @brief Callback function usr_mlme_associate_ind
 *
 * @param DeviceAddress         Extended address of device requesting
 * association
 * @param CapabilityInformation Capabilities of device requesting association
 */
void USR_MLME_AssociateInd(uint64_t DeviceAddress,
		uint8_t CapabilityInformation)
{
	/* Keep compiler happy. */
	DeviceAddress = DeviceAddress;
	CapabilityInformation = CapabilityInformation;
}

#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

/*
 * Callback function usr_mlme_beacon_notify_ind
 *
 * @param BSN            Beacon sequence number.
 * @param PANDescriptor  Pointer to PAN descriptor for received beacon.
 * @param PendAddrSpec   Pending address specification in received beacon.
 * @param AddrList       List of addresses of devices the coordinator has
 * pending data.
 * @param sduLength      Length of beacon payload.
 * @param sdu            Pointer to beacon payload.
 */
void USR_MLME_BeaconNotifyInd(uint8_t BSN,
		WPAN_Pandescriptor_t *PANDescriptor,
		uint8_t PendAddrSpec,
		uint8_t *AddrList,
		uint8_t sduLength,
		uint8_t *sdu)
{
    usr_mlme_beacon_notify_ind_t beacon;
    APP_Msg_T appMsg;
    
    beacon.AddrList = AddrList;
    beacon.BSN = BSN;
    beacon.PANDescriptor = PANDescriptor;
    beacon.PendAddrSpec = PendAddrSpec;
    beacon.sdu = sdu;
    beacon.sduLength = sduLength;
    
    appMsg.msgId = APP_MSG_MAC_BEACON_NOTIFY_IND;
    
    (void)memcpy((void *)appMsg.msgData,(const void *)&beacon, sizeof(beacon));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
	
}

#endif  /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || \
	(MAC_ASSOCIATION_INDICATION_RESPONSE == 1))

/*
 * @brief Callback function usr_mlme_comm_status_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param status           Result for related response operation
 */
void USR_MLME_CommStatusInd(WPAN_AddrSpec_t *SrcAddrSpec,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t status)
{
	if (status == MAC_SUCCESS) {
		/*
		 * Now the association of the device has been successful and its
		 * information, like address, could  be stored.
		 * But for the sake of simple handling it has been done
		 * during assignment of the short address within the function
		 * assign_new_short_addr()
		 */
	}

	/* Keep compiler happy. */
	SrcAddrSpec = SrcAddrSpec;
	DstAddrSpec = DstAddrSpec;
}

#endif  /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) ||
         *(MAC_ASSOCIATION_INDICATION_RESPONSE == 1)) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function USR_MLME_DisassociateConf
 *
 * @param status             Result of requested disassociate operation.
 * @param DeviceAddrSpec     Pointer to WPAN_AddrSpec_t structure for device
 *                           that has either requested disassociation or been
 *                           instructed to disassociate by its coordinator.
 *
 * @return void
 */
void USR_MLME_DisassociateConf(uint8_t status,
		WPAN_AddrSpec_t *DeviceAddrSpec)
{
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)*/

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function usr_mlme_disassociate_ind
 *
 * @param DeviceAddress        Extended address of device which initiated the
 *                             disassociation request.
 * @param DisassociateReason   Reason for the disassociation. Valid values:
 *                           - @ref WPAN_DISASSOC_BYPARENT,
 *                           - @ref WPAN_DISASSOC_BYCHILD.
 *
 * @return void
 */
void USR_MLME_DisassociateInd(uint64_t DeviceAddress,
		uint8_t DisassociateReason)
{
}

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1)

/*
 * Callback function USR_MLME_GetConf
 *
 * @param status            Result of requested PIB attribute get operation.
 * @param PIBAttribute      Retrieved PIB attribute.
 * @param PIBAttributeIndex Index of the PIB attribute to be read.
 * @param PIBAttributeValue Pointer to data containing retrieved PIB attribute,
 *
 * @return void
 */
void USR_MLME_GetConf(uint8_t status,
		uint8_t PIBAttribute,
#ifdef MAC_SECURITY_ZIP
		uint8_t PIBAttributeIndex,
#endif  /* MAC_SECURITY_ZIP */
		void *PIBAttributeValue)
{
    usr_mlme_get_conf_t getConf;
    APP_Msg_T appMsg;
    
    getConf.PIBAttribute = PIBAttribute;
#ifdef MAC_SECURITY_ZIP
    getConf.PIBAttributeIndex = PIBAttributeIndex;
#endif
    getConf.PIBAttributeValue = PIBAttributeValue;
    getConf.status = status;
    
    appMsg.msgId = APP_MSG_MAC_GET_CONF;
    
    (void)memcpy((void *)appMsg.msgData,(const void *)&getConf, sizeof(getConf));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);

}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

/*
 * Callback function USR_MLME_OrphanInd
 *
 * @param OrphanAddress     Address of orphaned device.
 *
 * @return void
 *
 */
void USR_MLME_OrphanInd(uint64_t OrphanAddress)
{
}

#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * Callback function that must be implemented by application (NHLE) for MAC
 * service
 * MLME-POLL.confirm.
 *
 * @param status           Result of requested poll operation.
 *
 * @return void
 *
 */
void USR_MLME_PollConf(uint8_t status)
{

    usr_mlme_poll_conf_t pollConf;
    APP_Msg_T appMsg;
    pollConf.status = status;
            
    appMsg.msgId = APP_MSG_MAC_POLL_CONF;
    
    (void)memcpy((void *)appMsg.msgData,(const void *)&pollConf, sizeof(pollConf));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/*
 * @brief Callback function usr_mlme_reset_conf
 *
 * @param status Result of the reset procedure
 */
void USR_MLME_ResetConf(uint8_t status)
{
    usr_mlme_reset_conf_t resetConf;
    APP_Msg_T appMsg;
    resetConf.status = status;
            
    appMsg.msgId = APP_MSG_MAC_RESET_CONF;
    
    (void)memcpy((void *)appMsg.msgData,(const void *)&resetConf, sizeof(resetConf));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);

}

#if (MAC_RX_ENABLE_SUPPORT == 1)

/*
 * Callback function USR_MLME_RxEnableConf
 *
 * @param status           Result of requested receiver enable operation.
 *
 * @return void
 */
void USR_MLME_RxEnableConf(uint8_t status)
{
}

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/*
 * @brief Callback function USR_MLME_ScanConf
 *
 * @param status            Result of requested scan operation
 * @param ScanType          Type of scan performed
 * @param ChannelPage       Channel page on which the scan was performed
 * @param UnscannedChannels Bitmap of unscanned channels
 * @param ResultListSize    Number of elements in ResultList
 * @param ResultList        Pointer to array of scan results
 */
void USR_MLME_ScanConf(uint8_t status,
		uint8_t ScanType,
		uint8_t ChannelPage,
		uint32_t UnscannedChannels,
		uint8_t ResultListSize,
		void *ResultList)
{
    usr_mlme_scan_conf_t scanConf;
    APP_Msg_T appMsg;
    
    scanConf.status = status;
    scanConf.ChannelPage = ChannelPage;
    scanConf.ResultList = ResultList;
    scanConf.ResultListSize = ResultListSize;
    scanConf.ScanType = ScanType;
    scanConf.UnscannedChannels = UnscannedChannels;
         
    appMsg.msgId = APP_MSG_MAC_SCAN_CONF;
    
    (void)memcpy((void *)appMsg.msgData, (const void *)&scanConf, sizeof(scanConf));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}

#endif

/*
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
#ifndef MAC_SECURITY_ZIP

/*
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
void USR_MLME_SetConf(uint8_t status,
		uint8_t PIBAttribute)
{
	usr_mlme_set_conf_t setConf;
    APP_Msg_T appMsg;
    
    setConf.status = status;
    setConf.PIBAttribute = PIBAttribute;
            
    appMsg.msgId = APP_MSG_MAC_SET_CONF;
    
    memcpy(appMsg.msgData, &setConf, sizeof(setConf));
    
    OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}

#endif

#if (MAC_START_REQUEST_CONFIRM == 1)
void USR_MLME_StartConf(uint8_t status)
{
}

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */