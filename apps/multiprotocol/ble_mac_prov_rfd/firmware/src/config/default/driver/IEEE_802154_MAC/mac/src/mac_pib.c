/*******************************************************************************
  MAC PIB Source

  File Name:
    mac_pib.c
  Summary:
    MAC PIB attribute handling
  Description:
    This file implements the MAC PIB attribute handling.

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
#include "phy_constants.h"
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
#include "mac_security.h"
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

/* === Macros ============================================================== */

#define MIN(a, b)                       (((a) < (b)) ? (a) : (b))

#define DEF_UINT8_T_SIZE       (1U)
#define DEF_UINT16_T_SIZE      (2U)
#define DEF_UINT32_T_SIZE      (4U)
#define DEF_UINT64_T_SIZE      (8U)
/* === Globals ============================================================= */

/* Size constants for PHY PIB attributes */
static const uint8_t phy_PibSize[] = {
	DEF_UINT8_T_SIZE,            /* 0x00: phyCurrentChannel */
	DEF_UINT32_T_SIZE,           /* 0x01: phyChannelsSupported */
	DEF_UINT8_T_SIZE,            /* 0x02: phyTransmitPower */
	DEF_UINT8_T_SIZE,            /* 0x03: phyCCAMode */
	DEF_UINT8_T_SIZE,            /* 0x04: phyCurrentPage */
	DEF_UINT16_T_SIZE,           /* 0x05: phyMaxFrameDuration */
	DEF_UINT8_T_SIZE,            /* 0x06: phySHRDuration */
	DEF_UINT8_T_SIZE             /* 0x07: phySymbolsPerOctet */
};

/* Update this one the arry phy_pib_size is updated. */
#define MAX_PHY_PIB_ATTRIBUTE_ID            (phySymbolsPerOctet)

/* Size constants for MAC PIB attributes */
static const uint8_t mac_PibSize[] = {
	DEF_UINT8_T_SIZE,            /* 0x40: macAckWaitDuration */
	DEF_UINT8_T_SIZE,            /* 0x41: macAssociationPermit */
	DEF_UINT8_T_SIZE,            /* 0x42: macAutoRequest */
	DEF_UINT8_T_SIZE,            /* 0x43: macBattLifeExt */
	DEF_UINT8_T_SIZE,            /* 0x44: macBattLifeExtPeriods */
	DEF_UINT8_T_SIZE,            /* 0x45: macBeaconPayload */
	DEF_UINT8_T_SIZE,            /* 0x46: macBeaconPayloadLength */
	DEF_UINT8_T_SIZE,            /* 0x47: macBeaconOrder */
	DEF_UINT16_T_SIZE,           /* 0x48: macBeaconTxTime */
	DEF_UINT8_T_SIZE,            /* 0x49: macBSN */
	DEF_UINT64_T_SIZE,           /* 0x4A: macCoordExtendedAddress */
	DEF_UINT16_T_SIZE,           /* 0x4B: macCoordShortAddress */
	DEF_UINT8_T_SIZE,            /* 0x4C: macDSN */
	DEF_UINT8_T_SIZE,            /* 0x4D: macGTSPermit */
	DEF_UINT8_T_SIZE,            /* 0x4E: macMaxCSMAbackoffs */
	DEF_UINT8_T_SIZE,            /* 0x4F: macMinBE */
	DEF_UINT16_T_SIZE,           /* 0x50: macPANId */
	DEF_UINT8_T_SIZE,            /* 0x51: macPromiscuousMode */
	DEF_UINT8_T_SIZE,            /* 0x52: macRxOnWhenIdle */
	DEF_UINT16_T_SIZE,           /* 0x53: macShortAddress */
	DEF_UINT8_T_SIZE,            /* 0x54: macSuperframeOrder */
	DEF_UINT16_T_SIZE,           /* 0x55: macTransactionPersistenceTime */
	DEF_UINT8_T_SIZE,            /* 0x56: macAssociatedPANCoord */
	DEF_UINT8_T_SIZE,            /* 0x57: macMaxBE */
	DEF_UINT16_T_SIZE,           /* 0x58: macMaxFrameTotalWaitTime */
	DEF_UINT8_T_SIZE,            /* 0x59: macMaxFrameRetries */
	DEF_UINT16_T_SIZE,           /* 0x5A: macResponseWaitTime */
	DEF_UINT16_T_SIZE,           /* 0x5B: macSyncSymbolOffset */
	DEF_UINT8_T_SIZE,            /* 0x5C: macTimestampSupported */
	DEF_UINT8_T_SIZE,            /* 0x5D: macSecurityEnabled */
	DEF_UINT8_T_SIZE,            /* 0x5E: macMinLIFSPeriod */
	DEF_UINT8_T_SIZE             /* 0x5F: macMinSIFSPeriod */
};

/* Update this one the arry mac_pib_size is updated. */
#define MIN_MAC_PIB_ATTRIBUTE_ID            (macAckWaitDuration)
#define MAX_MAC_PIB_ATTRIBUTE_ID            (macMinSIFSPeriod)

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
/* Size constants for MAC Security PIB attributes */
static const uint8_t mac_SecPibSize[] = {
	(uint8_t)sizeof(MAC_KeyTable_t),    /* 0x71: macKeyTable */
	DEF_UINT8_T_SIZE,            /* 0x72: macKeyTableEntries */

	/* Since the structure is not packed, we need to use the hardcode value
	**/
	17U,                         /* 0x73: macDeviceTable */
	DEF_UINT16_T_SIZE,            /* 0x74: macDeviceTableEntries */
	(uint8_t)sizeof(MAC_SecLvlTable_t), /* 0x75: macSecurityLevelTable */
	DEF_UINT8_T_SIZE,            /* 0x76: macSecurityLevelTableEntries */
	DEF_UINT32_T_SIZE,           /* 0x77: macFrameCounter */
	DEF_UINT8_T_SIZE,            /* 0x78: macAutoRequestSecurityLevel Not used in ZIP */
	DEF_UINT8_T_SIZE,            /* 0x79: macAutoRequestKeyIdMode Not used in ZIP */
	DEF_UINT8_T_SIZE,            /* 0x7A: macAutoRequestKeySource Not used in ZIP */
	DEF_UINT8_T_SIZE,            /* 0x7B: macAutoRequestKeyIndex Not used in ZIP */
	(8 * sizeof(uint8_t)),      /* 0x7C: macDefaultKeySource - 8 octets */
	(8 * sizeof(uint8_t)),      /* 0x7D: macPANCoordExtendedAddress Not used in ZIP */
	DEF_UINT16_T_SIZE            /* 0x7E: macPANCoordShortAddress Not used in ZIP */
};

/* Update this one the arry mac_pib_size is updated. */
#define MIN_MAC_SEC_PIB_ATTRIBUTE_ID        (macKeyTable)
#define MAX_MAC_SEC_PIB_ATTRIBUTE_ID        (macPANCoordShortAddress)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

/* Size constants for Private PIB attributes */
static const uint8_t private_PibSize[] = {
	DEF_UINT64_T_SIZE            /* 0xF0: macIeeeAddress */
#ifdef TEST_HARNESS
	,
	0,                          /* 0xF1: Unused */
	0,                          /* 0xF2: Unused */
	DEF_UINT8_T_SIZE,            /* 0xF3: macPrivateNoDataAfterAssocReq */
	DEF_UINT8_T_SIZE,            /* 0xF4: macPrivateIllegalFrameType */
	0,                          /* 0xF5: Unused */
	DEF_UINT8_T_SIZE,            /* 0xF6: macPrivateMACState */
	DEF_UINT8_T_SIZE,            /* 0xF7: macPrivateVirtualPANs */
	DEF_UINT8_T_SIZE             /* 0xF8: macPrivateMACSyncState */
#endif /* TEST_HARNESS */
};

/* Update this one the arry private_pib_size is updated. */
#define MIN_PRIVATE_PIB_ATTRIBUTE_ID            (macIeeeAddress)

/* === Prototypes ========================================================== */

#if (MAC_INDIRECT_DATA_BASIC == 1)
static void Recalc_MacMaxFrameTotalWaitTime(void);

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/* === Implementation ====================================================== */

#if (MAC_GET_SUPPORT == 1)

/**
 * @brief Handles an MLME-GET.request
 *
 * This function handles an MLME-GET.request.
 * The MLME-GET.request primitive requests information about a
 * given PIB attribute.
 *
 * @param m Pointer to the request structure
 */
void MAC_MLME_GetRequest(void *m)
{
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
	/* Use the mlme get request buffer for mlme get confirmation */
	MLME_GetConf_t *mgc = (MLME_GetConf_t *)MAC_BUFFER_POINTER(
			(buffer_t *)m);
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	uint8_t attributeIndex = ((MLME_GetReq_t *)((void *)mgc))->PIBAttributeIndex;
#endif /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	/* Do actual PIB attribute reading */
	{
		PibValue_t *attributeValue = &mgc->PIBAttributeValue;
		MAC_Retval_t status = MAC_SUCCESS;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
		status = MAC_MLME_Get(((MLME_GetReq_t *)((void *)mgc))->PIBAttribute,
				attributeValue, attributeIndex);
		mgc->PIBAttributeIndex = attributeIndex;
#else
		status = MAC_MLME_Get(((MLME_GetReq_t *)((void *)mgc))->PIBAttribute,
				attributeValue);
#endif /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
		mgc->PIBAttribute = ((MLME_GetReq_t *)((void *)mgc))->PIBAttribute;
		mgc->cmdcode      = MLME_GET_CONFIRM;
		mgc->status       = (uint8_t)status;
	}

	/* Append the mlme get confirmation to MAC-NHLE queue */
	qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)m);
    WPAN_PostTask();
    (void)qmmStatus;
}

#endif  /* (MAC_GET_SUPPORT == 1) */

/**
 * @brief Setting of MAC PIB attributes via functional access
 *
 * In case the highest stack layer is above MAC (e.g. NWK or even
 * higher), it is not efficient to change PIB attributes using
 * the standard request / confirm primitive concept via the NHLE_MAC
 * queue. In order to allow a more efficient way to change PIB attributes
 * residing in MAC or TAL, this function replaces the standard primitive
 * access via a functional interface.
 *
 * An additional parameter allows for forcing the transceiver back to sleep
 * after PIB setting. Otherwise the transceiver will stay awake (if it has been
 * woken up before).
 * This enables the higher layer to change several PIB attributes without
 * waking up the transceiver and putting it back to sleep several times.
 *
 * @param attribute PIB attribute to be set
 * @param attribute_index Index of the PIB attribute to be set
 * @param attributeValue Attribute value to be set
 * @param set_trx_to_sleep Set TRX back to sleep after this PIB access if it was
 *        before starting this TRX access. Otherwise the transceiver state will
 *        remain as it is, i.e. in case the transceiver was woken up, it will
 * stay
 *        awake.
 *        The default value for just changing one variable is true, i.e. the
 *        transceiver will be put back to sleep if it has been woken up.
 *
 * @return Status of the attempt to set the TAL PIB attribute:
 *         MAC_UNSUPPORTED_ATTRIBUTE if the PIB attribute was not found
 *         MAC_SUCCESS if the attempt to set the PIB attribute was successful
 *         TAL_BUSY if the TAL is not in an idle state to change PIB attributes
 */
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
MAC_Retval_t MAC_MLME_Set(uint8_t attribute, uint8_t attributeIndex,
		PibValue_t *attributeValue, bool setTrxToSleep)
#else
MAC_Retval_t MAC_MLME_Set(uint8_t attribute, PibValue_t *attributeValue,
		bool setTrxToSleep)
#endif
{
	/*
	 * Variables indicates whether the transceiver has been woken up for
	 * setting a TAL PIB attribute.
	 */
	static bool trx_pib_wakeup;

	MAC_Retval_t status = MAC_SUCCESS;
    PHY_Retval_t phy_status = PHY_SUCCESS;

	switch (attribute) {
#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	case macAssociatedPANCoord:
		macPib.mac_AssociatedPANCoord
			= attributeValue->pib_value_8bit;
		break;
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
	case macMaxFrameTotalWaitTime:
		macPib.mac_MaxFrameTotalWaitTime
			= attributeValue->pib_value_16bit;
		break;
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

	case macResponseWaitTime:
		macPib.mac_ResponseWaitTime = attributeValue->pib_value_16bit;
		break;

	case macAutoRequest:
#if (MAC_BEACON_NOTIFY_INDICATION == 1)

		/*
		 * If the beacon notification indications are not included
		 * in the build, macAutoRequest can be changed as desired, since
		 * beacon frames will be indicated to the higher
		 * layer if required as defined by IEEE 802.15.4.
		 */
		macPib.mac_AutoRequest = attributeValue->pib_value_8bit;
		break;
#else

		/*
		 * If the beacon notification indications are not included
		 * in the build, macAutoRequest must not be changed, since
		 * beacon frames will never be indicated to the higher
		 * layer, i.e. the higher would not be able to act on
		 * received beacon frame information itself.
		 */
		status = MAC_INVALID_PARAMETER;
		break;
#endif  /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

	case macBattLifeExtPeriods:
		macPib.mac_BattLifeExtPeriods
			= attributeValue->pib_value_8bit;
		break;

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	case macAssociationPermit:
		macPib.mac_AssociationPermit = attributeValue->pib_value_8bit;
		break;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	case macBeaconPayload:
		(void)memcpy(macBeacon_Payload, (uint8_t *)attributeValue,
				macPib.mac_BeaconPayloadLength);
		break;

	case macBeaconPayloadLength:

		/*
		 * If the application sits directly  on top of the MAC,
		 * this is also checked in mac_api.c.
		 */
		if (attributeValue->pib_value_8bit > aMaxBeaconPayloadLength) {
			status = MAC_INVALID_PARAMETER;
			break;
		}
		macPib.mac_BeaconPayloadLength
			= attributeValue->pib_value_8bit;
		break;

	case macBSN:
		macPib.mac_BSN = attributeValue->pib_value_8bit;
		break;
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)
	case macTransactionPersistenceTime:
		macPib.mac_TransactionPersistenceTime
			= attributeValue->pib_value_16bit;
		break;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */
	case macCoordExtendedAddress:
		macPib.mac_CoordExtendedAddress
			= attributeValue->pib_value_64bit;
		break;

	case macCoordShortAddress:
		macPib.mac_CoordShortAddress
			= attributeValue->pib_value_16bit;
		break;

	case macDSN:
		macPib.mac_DSN = attributeValue->pib_value_8bit;
		break;

	case macRxOnWhenIdle:
		macPib.mac_RxOnWhenIdle = attributeValue->pib_value_bool;
        
		/* Check whether radio state needs to change now, */
		if (macPib.mac_RxOnWhenIdle) 
        {
            PHY_TrxStatus_t trxStatus;
			/* Check whether the radio needs to be woken up. */
			MAC_TrxWakeup();

			/* Set transceiver in rx mode, otherwise it may stay in
			 * TRX_OFF). */
			trxStatus = PHY_RxEnable(PHY_STATE_RX_ON);
            (void)trxStatus;
		} else {
			/* Check whether the radio needs to be put to sleep. */
			MAC_SleepTrans();
		}

		break;
        
	case macBattLifeExt:
	case macBeaconOrder:
	case macMaxCSMABackoffs:
	case macMaxBE:
	case macMaxFrameRetries:
	case macMinBE:
	case macPANId:
#ifdef PROMISCUOUS_MODE
	case macPromiscuousMode:
#endif /* PROMISCUOUS_MODE */
	case macShortAddress:
	case macSuperframeOrder:
	case macIeeeAddress:
	case phyCurrentChannel:
	case phyCurrentPage:
	case phyTransmitPower:
	case phyCCAMode:
#ifdef TEST_HARNESS
	case macPrivateCCAFailure:
	case macPrivateDisableACK:
#endif /* TEST_HARNESS */
		{
			/* Now only TAL PIB attributes are handled anymore. */
			phy_status = PHY_PibSet(attribute, attributeValue);

			if (phy_status == PHY_TRX_ASLEEP) {
				/*
				 * Wake up the transceiver and repeat the
				 * attempt
				 * to set the TAL PIB attribute.
				 */
				phy_status = PHY_TrxWakeup();
				phy_status
					= PHY_PibSet(attribute,
						attributeValue);
				if (phy_status == PHY_SUCCESS) {
					/*
					 * Set flag indicating that the trx has
					 * been woken up
					 * during PIB setting.
					 */
					trx_pib_wakeup = true;
				}
			}

#if (MAC_INDIRECT_DATA_BASIC == 1)

			/*
			 * In any case that the PIB setting was successful (no
			 * matter
			 * whether the trx had to be woken up or not), the PIB
			 * attribute
			 * recalculation needs to be done.
			 */
			if (phy_status == PHY_SUCCESS) {
				/*
				 * The value of the PIB attribute
				 * macMaxFrameTotalWaitTime depends on the
				 * values of the
				 * following PIB attributes:
				 * macMinBE
				 * macMaxBE
				 * macMaxCSMABackoffs
				 * phyMaxFrameDuration
				 * In order to save code space and since
				 * changing of PIB
				 * attributes is going to happen not too often,
				 * this is done
				 * always whenever a PIB attribute residing in
				 * TAL is changed
				 * (since all above mentioned PIB attributes are
				 * in TAL).
				 */
				Recalc_MacMaxFrameTotalWaitTime();
			}
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */
		}
		break;

	case macAckWaitDuration:

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	case macSecurityEnabled:
		macPib.mac_SecurityEnabled = attributeValue->pib_value_bool;
		break;

	case macKeyTable:
		if (attributeIndex >= macSecPib.KeyTableEntries) {
			status = MAC_INVALID_INDEX;
		} else {
			(void)memcpy(&macSecPib.KeyTable[attributeIndex],
					&attributeValue->pib_value_8bit,
					sizeof(MAC_KeyTable_t));
		}

		break;

	case macKeyTableEntries:
		if (attributeValue->pib_value_8bit >
				MAC_ZIP_MAX_KEY_TABLE_ENTRIES) {
			status = MAC_INVALID_PARAMETER;
		} else {
			macSecPib.KeyTableEntries
				= attributeValue->pib_value_8bit;
		}

		break;

	case macDeviceTable:
		if (attributeIndex >= macSecPib.DeviceTableEntries) {
			status = MAC_INVALID_INDEX;
		} else {
			uint8_t *attributeTempPtr
				= (uint8_t *)attributeValue;

			/*
			 * Since the members of the mac_dev_table_t structure do
			 * contain padding bytes,
			 * each member needs to be filled in separately.
			 */
			/* PAN-Id */
			(void)memcpy((uint8_t *)&macSecPib.DeviceTable[
						attributeIndex].DeviceDescriptor[
						0].PANId,
					attributeTempPtr,
					sizeof(uint16_t));

			/*
			 *
			 *
			 *ADDR_COPY_DST_SRC_16(macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[0].PANId,
			 *(uint16_t *)attributeTempPtr); */
			attributeTempPtr += sizeof(uint16_t);

			/* Short Address */
			(void)memcpy((uint8_t *)&macSecPib.DeviceTable[
						attributeIndex].DeviceDescriptor[
						0].ShortAddress,
					attributeTempPtr,
					sizeof(uint16_t));

			/*ADDR_COPY_DST_SRC_16(macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[0].ShortAddress,
			 *(uint16_t *)attributeTempPtr);*/
			attributeTempPtr += sizeof(uint16_t);

			/* Extended Address */
			(void)memcpy((uint8_t *)&macSecPib.DeviceTable[
						attributeIndex].DeviceDescriptor[
						0].ExtAddress,
					attributeTempPtr,
					sizeof(uint64_t));

			/*ADDR_COPY_DST_SRC_64(macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[0].ExtAddress,
			 *(uint64_t *)attributeTempPtr);*/
			attributeTempPtr += sizeof(uint64_t);

			/* Extended Address */
			(void)memcpy(
					((uint8_t *)&macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[
						0].FrameCounter),
					attributeTempPtr,
					sizeof(uint32_t));
			attributeTempPtr += sizeof(uint32_t);

			/* Exempt */
			macSecPib.DeviceTable[attributeIndex].
			DeviceDescriptor[0].Exempt
				= (bool)*attributeTempPtr;
		}

		break;

	case macDeviceTableEntries:
		if (attributeValue->pib_value_16bit >
				MAC_ZIP_MAX_DEV_TABLE_ENTRIES) {
			status = MAC_INVALID_PARAMETER;
		} else {
			macSecPib.DeviceTableEntries
				= attributeValue->pib_value_16bit;
		}

		break;

	case macSecurityLevelTable:
		if (attributeIndex >= macSecPib.SecurityLevelTableEntries) {
			status = MAC_INVALID_INDEX;
		} else {
			(void)memcpy((&macSecPib.SecurityLevelTable[attributeIndex]),
					&attributeValue->pib_value_16bit,
					sizeof(MAC_SecLvlTable_t));
		}

		break;

	case macSecurityLevelTableEntries:
		if (attributeValue->pib_value_8bit >
				MAC_ZIP_MAX_SEC_LVL_TABLE_ENTRIES) {
			status = MAC_INVALID_PARAMETER;
		} else {
			macSecPib.SecurityLevelTableEntries
				= attributeValue->pib_value_8bit;
		}

		break;

	case macFrameCounter:
		macSecPib.FrameCounter = attributeValue->pib_value_32bit;
		break;

	case macDefaultKeySource:
		/* Key Source length is 8 octets. */
		(void)memcpy(macSecPib.DefaultKeySource, (uint8_t *)attributeValue, 8);
		break;

	case macPANCoordExtendedAddress:
		(void)memcpy(macSecPib.PANCoordExtendedAddress, (uint8_t *)attributeValue, 8);
		break;

	case macPANCoordShortAddress:
		macSecPib.PANCoordShortAddress
			= attributeValue->pib_value_16bit;
		break;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#ifdef TEST_HARNESS
	case macPrivateIllegalFrameType:
		macPib.privateIllegalFrameType
			= attributeValue->pib_value_8bit;
		break;

	case macPrivateNoDataAfterAssocReq:
		macPib.privateNoDataAfterAssocReq
			= attributeValue->pib_value_8bit;
		break;

	case macPrivateVirtualPANs:
		macPib.privateVirtualPANs = attributeValue->pib_value_8bit;
		break;
#endif /* TEST_HARNESS */
    default:
		status = MAC_UNSUPPORTED_ATTRIBUTE;
		break;
	}

	/*
	 * In case the transceiver shall be forced back to sleep and
	 * has been woken up, it is put back to sleep again.
	 */
	if (setTrxToSleep && trx_pib_wakeup && !macPib.mac_RxOnWhenIdle) {
#ifdef ENABLE_DEEP_SLEEP
		phy_status = PHY_TrxSleep(DEEP_SLEEP_MODE);
#else
		phy_status = PHY_TrxSleep(SLEEP_MODE_1);
#endif
		trx_pib_wakeup = false;
	}
    (void)phy_status;

	return status;
}

#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
MAC_Retval_t MAC_MLME_Get(uint8_t attribute, PibValue_t *attributeValue,
		uint8_t attributeIndex)
#else
MAC_Retval_t MAC_MLME_Get(uint8_t attribute, PibValue_t *attributeValue)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006)  */
{
	/*
	 * Variables indicates whether the transceiver has been woken up for
	 * setting a TAL PIB attribute.
	 */

	MAC_Retval_t status = MAC_SUCCESS;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    PibValue_t pib_value;

	switch (attribute)
    {
#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
	case macAssociatedPANCoord:
		attributeValue->pib_value_8bit
			= macPib.mac_AssociatedPANCoord;
		break;
#endif /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

	case macMaxBE:
        pibStatus = PHY_PibGet(macMaxBE, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

#if (MAC_INDIRECT_DATA_BASIC == 1)
	case macMaxFrameTotalWaitTime:
		(void)memcpy(&attributeValue->pib_value_16bit,
				&macPib.mac_MaxFrameTotalWaitTime,
				sizeof(uint16_t));

		break;
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

	case macMaxFrameRetries:
        pibStatus = PHY_PibGet(macMaxFrameRetries, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case macResponseWaitTime:
		(void)memcpy(&attributeValue->pib_value_16bit,
				&macPib.mac_ResponseWaitTime,
				sizeof(uint16_t));

		break;

	case macSecurityEnabled:
		attributeValue->pib_value_8bit = (uint8_t)(macPib.mac_SecurityEnabled);
		break;

	case phyCurrentPage:
        pibStatus = PHY_PibGet(phyCurrentPage, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case phyMaxFrameDuration:
        pibStatus = PHY_PibGet(phyMaxFrameDuration, (uint8_t *)&pib_value);
		(void)memcpy(&attributeValue->pib_value_16bit, &pib_value.pib_value_16bit,
				sizeof(uint16_t));
        (void)pibStatus;
		break;

	case phySHRDuration:
        pibStatus = PHY_PibGet(phySHRDuration, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case phySymbolsPerOctet:
        pibStatus = PHY_PibGet(phySymbolsPerOctet, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case macAutoRequest:
		attributeValue->pib_value_8bit = macPib.mac_AutoRequest;
		break;

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
	case macAssociationPermit:
		attributeValue->pib_value_8bit = macPib.mac_AssociationPermit;
		break;
#endif /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_START_REQUEST_CONFIRM == 1)
	case macBeaconPayload:
		(void)memcpy((uint8_t *)attributeValue,
				macBeacon_Payload,
				macPib.mac_BeaconPayloadLength);
		break;

	case macBeaconPayloadLength:
		attributeValue->pib_value_8bit
			= macPib.mac_BeaconPayloadLength;
		break;

	case macBSN:
		attributeValue->pib_value_8bit = macPib.mac_BSN;
		break;
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

#if (MAC_INDIRECT_DATA_FFD == 1)
	case macTransactionPersistenceTime:
		(void)memcpy(&attributeValue->pib_value_16bit,
				&macPib.mac_TransactionPersistenceTime,
				sizeof(uint16_t));
		break;
#endif /* (MAC_INDIRECT_DATA_FFD == 1) */

#ifdef PROMISCUOUS_MODE
	case macPromiscuousMode:
        pibStatus = PHY_PibGet(macPromiscuousMode, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;
#endif  /* PROMISCUOUS_MODE */

	case macCoordExtendedAddress:
		(void)memcpy(&attributeValue->pib_value_64bit,
				&macPib.mac_CoordExtendedAddress,
				sizeof(uint64_t));
		break;

	case macCoordShortAddress:
		(void)memcpy(&attributeValue->pib_value_16bit,
				&macPib.mac_CoordShortAddress,
				sizeof(uint16_t));
		break;

	case macDSN:
		attributeValue->pib_value_8bit = macPib.mac_DSN;
		break;

	case macMaxCSMABackoffs:
        pibStatus = PHY_PibGet(macMaxCSMABackoffs, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case macMinBE:
        pibStatus = PHY_PibGet(macMinBE, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case macPANId:
        pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
		(void)memcpy(&attributeValue->pib_value_16bit,
				&pib_value.pib_value_16bit,
				sizeof(uint16_t));
        (void)pibStatus;
		break;

	case macRxOnWhenIdle:
		attributeValue->pib_value_8bit = (uint8_t)(macPib.mac_RxOnWhenIdle);
		break;

	case macShortAddress:
        pibStatus = PHY_PibGet(macShortAddress, (uint8_t *)&pib_value);
		(void)memcpy(&attributeValue->pib_value_16bit,
				&pib_value.pib_value_16bit,
				sizeof(uint16_t));
        (void)pibStatus;
		break;

	case macIeeeAddress:
        pibStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
		(void)memcpy(&attributeValue->pib_value_64bit,
				&pib_value.pib_value_64bit,
				sizeof(uint64_t));
        (void)pibStatus;
		break;

	case phyCurrentChannel:
        pibStatus = PHY_PibGet(phyCurrentChannel, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case phyChannelsSupported:
        pibStatus = PHY_PibGet(phyChannelsSupported, (uint8_t *)&pib_value);
		(void)memcpy(&attributeValue->pib_value_32bit,
				&pib_value.pib_value_32bit,
				sizeof(uint32_t));
        (void)pibStatus;
		break;

	case phyTransmitPower:
        pibStatus = PHY_PibGet(phyTransmitPower, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;

	case phyCCAMode:
        pibStatus = PHY_PibGet(phyCCAMode, (uint8_t *)&pib_value);
		attributeValue->pib_value_8bit = pib_value.pib_value_8bit;
        (void)pibStatus;
		break;    

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	case macKeyTable:
		if (attributeIndex >= macSecPib.KeyTableEntries) {
			status = MAC_INVALID_INDEX;
		} else {
			(void)memcpy(&attributeValue->pib_value_8bit,
					&macSecPib.KeyTable[attributeIndex],
					sizeof(MAC_KeyTable_t));
		}

		break;

	case macKeyTableEntries:
		attributeValue->pib_value_8bit = macSecPib.KeyTableEntries;
		break;

	case macDeviceTable:
		if (attributeIndex >= macSecPib.DeviceTableEntries) {
			status = MAC_INVALID_INDEX;
		} else {
			/*
			 * Since the members of the MAC_DevTable_t structure do
			 * contain padding bytes,
			 * each member needs to be filled in separately.
			 */
			uint8_t *attributeTempPtr
				= (uint8_t *)attributeValue;

			/*
			 * Since the members of the MAC_DevTable_t structure do
			 * contain padding bytes,
			 * each member needs to be filled in separately.
			 */
			/* PAN-Id */
			ADDR_COPY_DST_SRC_16(*(uint16_t *)attributeTempPtr,
					macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[
						0].PANId);
			attributeTempPtr += sizeof(uint16_t);

			/* Short Address */
			ADDR_COPY_DST_SRC_16(*(uint16_t *)attributeTempPtr,
					macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[
						0].ShortAddress);
			attributeTempPtr += sizeof(uint16_t);

			/* Extended Address */
			ADDR_COPY_DST_SRC_64(*(uint64_t *)attributeTempPtr,
					macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[
						0].ExtAddress);
			attributeTempPtr += sizeof(uint64_t);

			/* Extended Address */
			(void)memcpy(attributeTempPtr,
					((uint8_t *)&macSecPib.DeviceTable[attributeIndex].DeviceDescriptor[
						0].FrameCounter),
					sizeof(uint32_t));
			attributeTempPtr += sizeof(uint32_t);

			/* Exempt */
			*attributeTempPtr
				= (uint8_t)(macSecPib.DeviceTable[attributeIndex
					].DeviceDescriptor[0].Exempt);
		}

		break;

	case macDeviceTableEntries:
		(void)memcpy(&attributeValue->pib_value_16bit,
				&macSecPib.DeviceTableEntries, 2);
		break;

	case macSecurityLevelTable:
		if (attributeIndex >= macSecPib.SecurityLevelTableEntries) {
			status = MAC_INVALID_INDEX;
		} else {
			(void)memcpy(&attributeValue->pib_value_8bit,
					&macSecPib.SecurityLevelTable[
						attributeIndex],
					sizeof(MAC_SecLvlTable_t));
		}

		break;

	case macSecurityLevelTableEntries:
		attributeValue->pib_value_8bit
			= macSecPib.SecurityLevelTableEntries;
		break;

	case macFrameCounter:
		(void)memcpy(&attributeValue->pib_value_32bit,
				&macSecPib.FrameCounter,
				sizeof(uint32_t));
		break;

	case macDefaultKeySource:
		/* Key Source length is 8 octets. */
		(void)memcpy((uint8_t *)attributeValue, macSecPib.DefaultKeySource, 8);
		break;
#endif /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#ifdef TEST_HARNESS

	/*
	 * The following PIB attributes are privately used for special
	 * test scenarios and are not part of the product code path
	 */
	case macPrivateIllegalFrameType:
		attributeValue->pib_value_8bit
			= macPib.privateIllegalFrameType;
		break;

	case macPrivateMACState:
		attributeValue->pib_value_8bit = macState;
		break;

	case macPrivateNoDataAfterAssocReq:
		attributeValue->pib_value_8bit
			= macPib.privateNoDataAfterAssocReq;
		break;

	case macPrivateVirtualPANs:
		attributeValue->pib_value_8bit = macPib.privateVirtualPANs;
		break;

	case macPrivateMACSyncState:
		attributeValue->pib_value_8bit = macSyncState;
		break;
#endif /* TEST_HARNESS */
    default:
        status = MAC_UNSUPPORTED_ATTRIBUTE;
		break;
	}
	return status;
}

/**
 * @brief Handles an MLME-SET.request primitive
 *
 * This function handles the MLME-SET.request. The MLME-SET.request primitive
 * attempts to write the given value to the indicated PIB attribute.
 *
 * @param m Pointer to the request structure
 */
void MAC_MLME_SetRequest(void *m)
{
    qmm_status_t  qmmStatus = QMM_QUEUE_FULL;
	MLME_SetReq_t *msr
		= (MLME_SetReq_t *)MAC_BUFFER_POINTER((buffer_t *)m);

	/* Do the actual PIB attribute set operation */
	{
		PibValue_t *attributeValue = &msr->PIBAttributeValue;
		MAC_Retval_t status = MAC_SUCCESS;
		MLME_SetConf_t *msc;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

		/*
		 * Store attribute index in local var, because
		 * it will be overwritten later.
		 */
		uint8_t attributeIndex = msr->PIBAttributeIndex;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

		/*
		 * Call internal PIB attribute handling function. Always force
		 * the trx back to sleep when using request primitives via the
		 * MLME queue.
		 */
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
		status = MAC_MLME_Set(msr->PIBAttribute, msr->PIBAttributeIndex,
				attributeValue, true);
#else
		status = MAC_MLME_Set(msr->PIBAttribute, attributeValue, true);
#endif
		msc = (MLME_SetConf_t *)((void *)msr);
		msc->PIBAttribute = msr->PIBAttribute;
#ifdef MAC_SECURITY_ZIP
		msc->PIBAttributeIndex = attributeIndex;
#endif  /* MAC_SECURITY_ZIP */
		msc->cmdcode      = MLME_SET_CONFIRM;
		msc->status       = (uint8_t)status;
	}

	/* Append the mlme set confirmation message to the MAC-NHLE queue */
	qmmStatus = qmm_queue_append(&macNhleQueue, (buffer_t *)m);
    WPAN_PostTask();
    (void)qmmStatus;
}

/**
 * @brief Wakes-up the radio and sets the corresponding TAL PIB attribute
 *
 * @param attribute PIB attribute to be set
 * @param attribute_value Attribute value to be set
 *
 * @return Status of the attempt to set the TAL PIB attribute
 */
PHY_Retval_t SetPhyPibInternal(uint8_t attribute, PibValue_t *attributeValue)
{
	PHY_Retval_t status;

	if (RADIO_SLEEPING == macRadioSleepState) {
		/* Wake up the radio */
		MAC_TrxWakeup();

		status = PHY_PibSet(attribute, attributeValue);

		/* Set radio to sleep if allowed */
		MAC_SleepTrans();
	} else {
		status = PHY_PibSet(attribute, attributeValue);
	}

	return status;
}

#if (MAC_INDIRECT_DATA_BASIC == 1)

/**
 * @brief Re-caclulates PIB attribute macMaxFrameTotalWaitTime
 *
 * This function re-calculates the MAC PIB attribute macMaxFrameTotalWaitTime
 * whenever one of the following PIB attributes change:
 * macMinBE
 * macMaxBE
 * macMaxCSMABackoffs
 * phyMaxFrameDuration
 *
 * See IEEE 802.15.4-2006 equation (14) in section 7.4.2.
 */
static void Recalc_MacMaxFrameTotalWaitTime(void)
{
	uint8_t m;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    PibValue_t pib_maxBE;
    PibValue_t pib_minBE;
    PibValue_t pib_maxCSMAbackoff;

	pibStatus = PHY_PibGet(macMaxBE, (uint8_t *)&pib_maxBE);
    pibStatus = PHY_PibGet(macMinBE, (uint8_t *)&pib_minBE);
    pibStatus = PHY_PibGet(macMaxCSMABackoffs, (uint8_t *)&pib_maxCSMAbackoff);
    m = (uint8_t)MIN((pib_maxBE.pib_value_8bit - pib_minBE.pib_value_8bit),
			pib_maxCSMAbackoff.pib_value_8bit);

	macPib.mac_MaxFrameTotalWaitTime
		= (((uint16_t)pib_maxCSMAbackoff.pib_value_8bit) -
			m) * ((1U << pib_maxBE.pib_value_8bit) - 1U);

	/* Calculate sum of equation (14). */
	for (uint8_t k = 0; k < m; k++) {
		macPib.mac_MaxFrameTotalWaitTime += (uint16_t)(1U << (pib_minBE.pib_value_8bit + k));
	}

	/* Calculate the rest. */
	macPib.mac_MaxFrameTotalWaitTime *= aUnitBackoffPeriod;
	macPib.mac_MaxFrameTotalWaitTime += MAX_FRAME_DURATION;
    (void)pibStatus;
}

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/**
 * @brief Gets the size of a PIB attribute
 *
 * @param attribute PIB attribute
 *
 * @return Size (number of bytes) of the PIB attribute
 */
uint8_t MAC_GetPibAttributeSize(uint8_t pibAttributeId)
{
#if (MAC_START_REQUEST_CONFIRM == 1)

	/*
	 * Since the current length of the beacon payload is not a contant, but
	 * a variable, it cannot be stored in a Flash table. Therefore we need
	 * to handle this PIB attribute special.
	 */
	if (macBeaconPayload == pibAttributeId) {
		return (macPib.mac_BeaconPayloadLength);
	}
#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

	if (MAX_PHY_PIB_ATTRIBUTE_ID >= pibAttributeId) {
		return (PGM_READ_BYTE(&phy_PibSize[pibAttributeId]));
	}

	if (MIN_MAC_PIB_ATTRIBUTE_ID <= pibAttributeId &&
			MAX_MAC_PIB_ATTRIBUTE_ID >= pibAttributeId) {
		return(PGM_READ_BYTE(&mac_PibSize[pibAttributeId -
		       MIN_MAC_PIB_ATTRIBUTE_ID]));
	}

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	if (MIN_MAC_SEC_PIB_ATTRIBUTE_ID <= pibAttributeId &&
			MAX_MAC_SEC_PIB_ATTRIBUTE_ID >= pibAttributeId) {
		return(PGM_READ_BYTE(&mac_SecPibSize[pibAttributeId -
		       MIN_MAC_SEC_PIB_ATTRIBUTE_ID]));
	}
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

	if (MIN_PRIVATE_PIB_ATTRIBUTE_ID <= pibAttributeId) {
		return(PGM_READ_BYTE(&private_PibSize[pibAttributeId -
		       MIN_PRIVATE_PIB_ATTRIBUTE_ID]));
	}

	return(0);
}

/* EOF */
