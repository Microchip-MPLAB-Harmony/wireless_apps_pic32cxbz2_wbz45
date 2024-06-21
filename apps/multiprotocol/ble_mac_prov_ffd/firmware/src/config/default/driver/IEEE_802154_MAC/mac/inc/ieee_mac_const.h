/*******************************************************************************
    IEEE MAC Constants

  File Name:
    ieee_mac_const.h
 Summary:
    IEEE PHY & MAC constants and Attributes definitions
  Description:
    This header holds all IEEE 802.15.4-2015 constants and attribute
    identifiers.

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

#ifndef IEEE_MAC_CONST_H
#define IEEE_MAC_CONST_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

/* === Includes ============================================================= */

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: MAC Constants
// *****************************************************************************
// *****************************************************************************
   
// *****************************************************************************
/* 

   Summary:
    This macro holds the number of symbols forming a superframe when the superframe 
    order is equal to 0
   Description:
	None
   Remarks:
    None 
 */
 
#define aBaseSuperframeDuration         (aBaseSlotDuration * \
	aNumSuperframeSlots)
        
// *****************************************************************************
/* 

   Summary:
    This macro holds Beacon Interval time in symbols
   Description:
	None
   Remarks:
    None 
 */

#define GET_BEACON_INTERVAL_TIME(BO) \
	    ((1UL * aBaseSuperframeDuration) << (BO))

// *****************************************************************************
/* 

   Summary:
    This macro holds the number of superframes in which a GTS descriptor exists 
	in the beacon frame of a PAN coordinator
   Description:
	None
   Remarks:
    None 
 */

#define aGTSDescPersistenceTime         (4U)

// *****************************************************************************
/* 

   Summary:
    This macro holds the maximum number of octets added by the MAC sublayer to the 
	payload of its beacon frame
   Description:
	None
   Remarks:
    None 
 */

#define aMaxBeaconOverhead              (75U)

// *****************************************************************************
/* 

   Summary:
    This macro holds the maximum size, in octets, of a beacon payload
   Description:
	None
   Remarks:
    None 
 */

#define aMaxBeaconPayloadLength         (aMaxPHYPacketSize - aMaxBeaconOverhead)

// *****************************************************************************
/* 

   Summary:
    This macro holds the number of consecutive lost beacons that will cause the MAC 
	sublayer of a receiving device to declare a loss of synchronization
   Description:
	None
   Remarks:
    None 
 */

#define aMaxLostBeacons                 (4U)

// *****************************************************************************
/* 

   Summary:
    This macro holds the maximum number of octets that can be transmitted in the MAC 
	Payload field
   Description:
	None
   Remarks:
    None 
 */

#define aMaxMACPayloadSize              (aMaxPHYPacketSize - aMinMPDUOverhead)

// *****************************************************************************
/* 

   Summary:
    This macro holds the maximum number of octets that can be transmitted in the MAC Payload
    field of an unsecured MAC frame that will be guaranteed not to exceed aMaxPHYPacketSize
   Description:
	None
   Remarks:
    None 
 */

#define aMaxMACSafePayloadSize          (aMaxPHYPacketSize - \
	aMaxMPDUUnsecuredOverhead)

// *****************************************************************************
/* 

   Summary:
    This macro holds the maximum number of octets added by the MAC sublayer to the 
	PSDU without security
   Description:
	None
   Remarks:
    None 
 */

#define aMaxMPDUUnsecuredOverhead       (25U)


// *****************************************************************************
// *****************************************************************************
// Section: MAC PIB Attributes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* macAssociationPermit

   Summary:
    This macro holds Indication of whether a coordinator is currently allowing association.
    A value of true indicates that association is permitted
   Description:
	None
   Remarks:
    None 
 */

#define macAssociationPermit            (0x41U)

// *****************************************************************************
/* macAssociationPermit_def

   Summary:
    This macro holds Default value for PIB macAssociationPermit
   Description:
	None
   Remarks:
    None 
 */

#define macAssociationPermit_def        (false)

// *****************************************************************************
/* macAutoRequest

   Summary:
    This macro holds Indication of whether a device automatically sends a data request 
	command if its address is listed in the beacon frame. A value of true indicates 
	that the data request command is automatically sent
   Description:
	None
   Remarks:
    None 
 */

#define macAutoRequest                  (0x42U)

// *****************************************************************************
/* macAutoRequest_def

   Summary:
    This macro holds Default value for PIB macAutoRequest
   Description:
	None
   Remarks:
    None 
 */

#define macAutoRequest_def              (true)

// *****************************************************************************
/* macBattLifeExt

   Summary:
    This macro holds Indication of whether battery life extension, by reduction of
	coordinator receiver operation time during the CAP, is enabled. A value of 
	true indicates that it is enabled
   Description:
	None
   Remarks:
    None 
 */

#define macBattLifeExt                  (0x43U)

// *****************************************************************************
/* macBattLifeExt_def

   Summary:
    This macro holds Default value for PIB macBattLifeExt
   Description:
	None
   Remarks:
    None 
 */

#define macBattLifeExt_def              (false)

// *****************************************************************************
/* macBattLifeExtPeriods

   Summary:
    This macro holds The number of backoff periods during which the receiver is enabled 
	following a beacon in battery life extension mode. This value is dependent on the 
	currently selected logical channel. For 0 <= * phyCurrentChannel <= 10, this value 
	is equal to 8. For 11 <= * phyCurrentChannel <= 26, this value is equal to 6
   Description:
	None
   Remarks:
    None 
 */

#define macBattLifeExtPeriods           (0x44U)

// *****************************************************************************
/* macBattLifeExtPeriods_def

   Summary:
    This macro holds Default value for PIB macBattLifeExtPeriods
   Description:
	None
   Remarks:
    None 
 */

#define macBattLifeExtPeriods_def       (6U)

// *****************************************************************************
/* macBeaconPayload

   Summary:
    This macro holds the contents of the beacon payload
   Description:
	None
   Remarks:
    None 
 */

#define macBeaconPayload                (0x45U)

// *****************************************************************************
/* macBeaconPayloadLength

   Summary:
    This macro holds the length, in octets, of the beacon payload
   Description:
	None
   Remarks:
    None 
 */

#define macBeaconPayloadLength          (0x46U)

// *****************************************************************************
/* macBeaconPayloadLength_def

   Summary:
    This macro holds default value for PIB macBeaconPayloadLength
   Description:
	None
   Remarks:
    None 
 */

#define macBeaconPayloadLength_def      (0U)

// *****************************************************************************
/* macBeaconOrder

   Summary:
    This macro holds Specification of how often the coordinator transmits a beacon.
    The macBeaconOrder, BO, and the beacon interval, BI, are related as follows: 
	for 0 <= BO <= 14, BI = aBaseSuperframeDuration * 2^BO symbols.
    If BO = 15, the coordinator will not transmit a beacon
   Description:
	None
   Remarks:
    None 
 */

#define macBeaconOrder                  (0x47U)

// *****************************************************************************
/* macBeaconOrder_def

   Summary:
    This macro holds default value for PIB macBeaconOrder
   Description:
	None
   Remarks:
    None 
 */

#define macBeaconOrder_def              (15U)

// *****************************************************************************
/* NON_BEACON_NWK

   Summary:
    This macro holds BO value for nonbeacon-enabled network
   Description:
	None
   Remarks:
    None 
 */

#define NON_BEACON_NWK                  (0x0FU)

// *****************************************************************************
/* macBeaconTxTime

   Summary:
    This macro holds the time that the device transmitted its last beacon frame,
	in symbol periods. The measurement shall be taken at the same symbol boundary
	withinevery transmitted beacon frame, the location of which is implementation 
	specific. The precision of this value shall be a minimum of 20 bits, with the
	lowest four bits being the least significant
   Description:
	None
   Remarks:
    None 
 */

#define macBeaconTxTime                 (0x48U)

// *****************************************************************************
/* macBeaconTxTime_def

   Summary:
    This macro holds default value for PIB macBeaconTxTime
   Description:
	None
   Remarks:
    None 
 */

#define macBeaconTxTime_def             (0x000000)

// *****************************************************************************
/* macBSN

   Summary:
    This macro holds the sequence number added to the transmitted beacon frame
   Description:
	None
   Remarks:
    None 
 */

#define macBSN                          (0x49U)

// *****************************************************************************
/* macCoordExtendedAddress

   Summary:
    This macro holds the 64 bit address of the coordinator with which the device 
	is associated
   Description:
	None
   Remarks:
    None 
 */

#define macCoordExtendedAddress         (0x4AU)

// *****************************************************************************
/* macCoordShortAddress

   Summary:
    This macro holds the 16 bit short address assigned to the coordinator 
	with which the device is associated. A value of 0xfffe indicates that
	the coordinator is only using its 64 bit extended address. A value of
	0xffff indicates that this value is unknown is associated
   Description:
	None
   Remarks:
    None 
 */

#define macCoordShortAddress            (0x4BU)

// *****************************************************************************
/* macCoordShortAddress_def

   Summary:
    This macro holds default value for PIB macCoordShortAddress
   Description:
	None
   Remarks:
    None 
 */

#define macCoordShortAddress_def        (0xFFFF)

// *****************************************************************************
/* macDSN

   Summary:
    This macro holds the sequence number added to the transmitted data 
	or MAC command frame
   Description:
	None
   Remarks:
    None 
 */

#define macDSN                          (0x4CU)

// *****************************************************************************
/* macGTSPermit

   Summary:
    This macro holds macGTSPermit is true if the PAN coordinator is to accept 
	GTS requests,false otherwise
   Description:
	None
   Remarks:
    None 
 */

#define macGTSPermit                    (0x4DU)

// *****************************************************************************
/* macGTSPermit_def

   Summary:
    This macro holds default value for PIB macGTSPermit
   Description:
	None
   Remarks:
    None 
 */

#define macGTSPermit_def                (true)

// *****************************************************************************
/* macRxOnWhenIdle

   Summary:
    This indicates whether the MAC sublayer is to enable its receiver
    during idle periods.
   Description:
	None
   Remarks:
    None 
 */

#define macRxOnWhenIdle                 (0x52U)

// *****************************************************************************
/* macRxOnWhenIdle_def

   Summary:
    This macro holds the default value for PIB macRxOnWhenIdle
   Description:
	None
   Remarks:
    None 
 */

#define macRxOnWhenIdle_def             (false)

// *****************************************************************************
/* macSuperframeOrder

   Summary:
    This specifies the length of the active portion of the superframe, including
    the beacon frame. The macSuperframeOrder, SO, and the superframe duration,
    SD, are related as follows: for 0 <= SO <= BO <= 14, SD = 
	aBaseSuperframeDuration * 2SO symbols. If SO = 15, the superframe will
    not be active following the beacon
   Description:
	None
   Remarks:
    None 
 */

#define macSuperframeOrder              (0x54U)

// *****************************************************************************
/* macSuperframeOrder_def

   Summary:
    This macro holds the default value for PIB macSuperframeOrder
   Description:
	None
   Remarks:
    None 
 */

#define macSuperframeOrder_def          (15U)

// *****************************************************************************
/* macTransactionPersistenceTime

   Summary:
    This macro holds the maximum time (in superframe periods) that a transaction
	is stored by a coordinator and indicated in its beacon
   Description:
	None
   Remarks:
    None 
 */

#define macTransactionPersistenceTime   (0x55U)

// *****************************************************************************
/* macTransactionPersistenceTime_def

   Summary:
    This macro holds default value for PIB macTransactionPersistenceTime
   Description:
	None
   Remarks:
    None 
 */

#define macTransactionPersistenceTime_def (0x01F4)

// *****************************************************************************
/* macAssociatedPANCoord

   Summary:
    Indication of whether the device is associated to the PAN through the PAN
    coordinator. A value of TRUE indicates the device has associated through the
    PAN coordinator. Otherwise, the value is set to FALSE
   Description:
	None
   Remarks:
    None 
 */
 
#define macAssociatedPANCoord           (0x56U)

// *****************************************************************************
/* macAssociatedPANCoord_def

   Summary:
    This macro holds the default value for PIB macAssociatedPANCoord
   Description:
	None
   Remarks:
    None 
 */

#define macAssociatedPANCoord_def       (false)

// *****************************************************************************
/* macMaxFrameTotalWaitTime

   Summary:
    The maximum number of CAP symbols in a beaconenabled PAN, or symbols in a
    nonbeacon-enabled PAN, to wait either for a frame intended as a response to
    a data request frame or for a broadcast frame following a beacon with the
    Frame Pending subfield set to one.
    This attribute, which shall only be set by the next higher layer, is
    dependent upon macMinBE, macMaxBE, macMaxCSMABackoffs and the number of
    symbols per octet
   Description:
	Dependent on currently selected PHY, indicated by phyCurrentPage
   Remarks:
    None 
 */

#define macMaxFrameTotalWaitTime        (0x58U)

// *****************************************************************************
/* macResponseWaitTime

   Summary:
    The maximum time, in multiples of aBaseSuperframeDuration, a device shall
    wait for a response command frame to be available following a request
    command frame
   Description:
	None
   Remarks:
    None 
 */

#define macResponseWaitTime             (0x5AU)

// *****************************************************************************
/* macResponseWaitTime_def

   Summary:
    This macro holds the default value for PIB macResponseWaitTime
   Description:
	None
   Remarks:
    None 
 */

#define macResponseWaitTime_def         (32U * aBaseSuperframeDuration)

// *****************************************************************************
/* macSyncSymbolOffset

   Summary:
    The offset, measured in symbols, between the symbol boundary at which the
    MLME captures the timestamp of each transmitted or received frame, and the
    onset of the first symbol past the SFD, namely, the first symbol of the
    Length field
   Description:
	Range: 0x000-0x100 for the 2.4 GHz PHY
           0x000-0x400 for the 868/915 MHz PHY
   Remarks:
    None 
 */

#define macSyncSymbolOffset             (0x5BU)

// *****************************************************************************
/* macTimestampSupported

   Summary:
    Indication of whether the MAC sublayer supports the optional timestamping
    feature for incoming and outgoing data frames
   Description:
	None
   Remarks:
    None 
 */

#define macTimestampSupported           (0x5CU)

// *****************************************************************************
/* macSecurityEnabled

   Summary:
    Indication of whether the MAC sublayer has security enabled. A value of
    TRUE indicates that security is enabled, while a value of FALSE indicates
    that security is disabled
   Description:
	None
   Remarks:
    None 
 */

#define macSecurityEnabled              (0x5DU)

// *****************************************************************************
/* macSecurityEnabled

   Summary:
    default value for PIB macSecurityEnabled
   Description:
	None
   Remarks:
    None 
 */

#ifdef MAC_SECURITY_ZIP
#define macSecurityEnabled_def          (true)
#else
#define macSecurityEnabled_def          (false)
#endif  /* No MAC_SECURITY */

#ifdef MAC_SECURITY_ZIP

// *****************************************************************************
/* macKeyTable

   Summary:
    This macro holds a table of KeyDescriptor entries, each containing keys 
	and related information required for secured communications
   Description:
	None
   Remarks:
    None 
 */

#define macKeyTable                     (0x71U)

// *****************************************************************************
/* macKeyTableEntries

   Summary:
    This macro holds the number of entries in macKeyTable
   Description:
	None
   Remarks:
    None 
 */


#define macKeyTableEntries              (0x72U)

// *****************************************************************************
/* macDeviceTable

   Summary:
    This macro holds A table of DeviceDescriptor entries, each indicating a 
	remote device with which this device securely communicates
   Description:
	None
   Remarks:
    None 
 */

#define macDeviceTable                  (0x73U)

// *****************************************************************************
/* macDeviceTableEntries

   Summary:
    This macro holds the number of entries in macDeviceTable
   Description:
	None
   Remarks:
    None 
 */

#define macDeviceTableEntries           (0x74U)

// *****************************************************************************
/* macSecurityLevelTable

   Summary:
    This macro holds a table of SecurityLevelDescriptor entries, each with 
	information about the minimum security level expected depending on 
	incoming frame type and subtype
   Description:
	None
   Remarks:
    None 
 */

#define macSecurityLevelTable           (0x75U)

// *****************************************************************************
/* macSecurityLevelTableEntries

   Summary:
    This macro holds the number of entries in macSecurityLevelTable
   Description:
	None
   Remarks:
    None 
 */

#define macSecurityLevelTableEntries    (0x76U)

// *****************************************************************************
/* macFrameCounter

   Summary:
    This macro holds the outgoing frame counter for this device
   Description:
	None
   Remarks:
    None 
 */

#define macFrameCounter                 (0x77U)

// *****************************************************************************
/* macDefaultKeySource

   Summary:
    This macro holds the originator of the default key used for 
	key identifier mode 0x01
   Description:
	None
   Remarks:
    None 
 */

#define macDefaultKeySource             (0x7CU)
#endif  /* MAC_SECURITY_ZIP */

// *****************************************************************************
/* macPANCoordExtendedAddress

   Summary:
    This macro holds the 64-bit address of the PAN coordinator
   Description:
	None
   Remarks:
    None 
 */

#define macPANCoordExtendedAddress      (0x7DU)

// *****************************************************************************
/* macPANCoordShortAddress

   Summary:
    This macro holds the 16-bit short address assinged to the PAN coordinator
   Description:
	None
   Remarks:
    None 
 */

#define macPANCoordShortAddress         (0x7EU)

// *****************************************************************************
/* macMaxNumRxFramesDuringBackoff

   Summary:
    This macro holds Maximum number of frames that are allowed to be 
    received during CSMA backoff periods for a tx transaction
	This is Private MAC PIB attribute to control the CSMA algorithm

   Description:
	None
   Remarks:
    None 
 */

#define macMaxNumRxFramesDuringBackoff  (0x7FU)

// *****************************************************************************
// Section:MAC commands attributes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MLME_SCAN_TYPE_ED

   Summary:
    This macro holds Energy scan(WPAN_MLME_ScanReq())
   Description:
	None
   Remarks:
    None 
 */

#define MLME_SCAN_TYPE_ED               (0x00U)

// *****************************************************************************
/* MLME_SCAN_TYPE_ACTIVE

   Summary:
    This macro holds Active scan(WPAN_MLME_ScanReq())
   Description:
	None
   Remarks:
    None 
 */

#define MLME_SCAN_TYPE_ACTIVE           (0x01U)

// *****************************************************************************
/* MLME_SCAN_TYPE_PASSIVE

   Summary:
    This macro holds Passive scan(WPAN_MLME_ScanReq())
   Description:
	None
   Remarks:
    None 
 */

#define MLME_SCAN_TYPE_PASSIVE          (0x02U)

// *****************************************************************************
/* MLME_SCAN_TYPE_ORPHAN

   Summary:
    This macro holds Orphan scan(WPAN_MLME_ScanReq())
   Description:
	None
   Remarks:
    None 
 */

#define MLME_SCAN_TYPE_ORPHAN           (0x03U)

// *****************************************************************************
/* WPAN_TXOPT_OFF

   Summary:
    This macro holds Value for TxOptions parameter(WPAN_MCPS_DataReq())
   Description:
	None
   Remarks:
    None 
 */

#define WPAN_TXOPT_OFF                  (0x00U)

// *****************************************************************************
/* WPAN_TXOPT_ACK

   Summary:
    This macro holds Value for TxOptions parameter(WPAN_MCPS_DataReq())
   Description:
	None
   Remarks:
    None 
 */

#define WPAN_TXOPT_ACK                  (0x01U)

// *****************************************************************************
/* WPAN_TXOPT_INDIRECT

   Summary:
    This macro holds Value for TxOptions parameter(WPAN_MCPS_DataReq())
   Description:
	None
   Remarks:
    None 
 */

#define WPAN_TXOPT_INDIRECT             (0x04U)

// *****************************************************************************
/* WPAN_TXOPT_INDIRECT_ACK

   Summary:
    This macro holds Value for TxOptions parameter(WPAN_MCPS_DataReq())
   Description:
	None
   Remarks:
    None 
 */

#define WPAN_TXOPT_INDIRECT_ACK         (0x05U)

// *****************************************************************************
/* WPAN_TXOPT_GTS

   Summary:
    This macro holds Value for TxOptions parameter(WPAN_MCPS_DataReq())
   Description:
	None
   Remarks:
    None 
 */

#define WPAN_TXOPT_GTS                  (0x02U)

// *****************************************************************************
/* WPAN_TXOPT_GTS_ACK

   Summary:
    This macro holds Value for TxOptions parameter(WPAN_MCPS_DataReq())
   Description:
	None
   Remarks:
    None 
 */

#define WPAN_TXOPT_GTS_ACK              (0x03U)

// *****************************************************************************
// *****************************************************************************
// Section:Various constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* INVERSE_CHANNEL_MASK

   Summary:
    This macro holds Inverse channel masks for scanning
   Description:
	None
   Remarks:
    None 
 */

#define INVERSE_CHANNEL_MASK            (~VALID_CHANNEL_MASK)

// *****************************************************************************
/* ASSOCIATION_SUCCESSFUL

   Summary:
    This macro holds Association status code value(WPAN_MLME_AssociateResp())
   Description:
	None
   Remarks:
    None 
 */

#define ASSOCIATION_SUCCESSFUL          (0U)

// *****************************************************************************
/* PAN_AT_CAPACITY

   Summary:
    This macro holds Association status code value(WPAN_MLME_AssociateResp())
   Description:
	None
   Remarks:
    None 
 */

#define PAN_AT_CAPACITY                 (1U)

// *****************************************************************************
/* PAN_ACCESS_DENIED

   Summary:
    This macro holds Association status code value(WPAN_MLME_AssociateResp())
   Description:
	None
   Remarks:
    None 
 */

#define PAN_ACCESS_DENIED               (2U)

// *****************************************************************************
/* ASSOCIATION_RESERVED

   Summary:
    This macro holds Association status code value(WPAN_MLME_AssociateResp())
   Description:
	None
   Remarks:
    None 
 */

#define ASSOCIATION_RESERVED            (3U)

// *****************************************************************************
/* NUM_SHORT_PEND_ADDR

   Summary:
    This macro holds Mask for the number of short addresses pending
   Description:
	None
   Remarks:
    None 
 */

#define NUM_SHORT_PEND_ADDR(x)          ((x) & 0x07U)

// *****************************************************************************
/* NUM_LONG_PEND_ADDR

   Summary:
    This macro holds Mask for the number of long addresses pending
   Description:
	None
   Remarks:
    None 
 */

#define NUM_LONG_PEND_ADDR(x)           (((x) & 0x70U) >> 4U)

// *****************************************************************************
/* CLEAR_ADDR_64

   Summary:
    This macro holds Unused EUI-64 address
   Description:
	None
   Remarks:
    None 
 */

#define CLEAR_ADDR_64                   (0ULL)

// *****************************************************************************
/* MAC_NO_SHORT_ADDR_VALUE

   Summary:
    This macro holds MAC is using long address (by now)
   Description:
	None
   Remarks:
    None 
 */

#define MAC_NO_SHORT_ADDR_VALUE         (0xFFFEU)

// *****************************************************************************
/* INVALID_SHORT_ADDRESS

   Summary:
    This macro holds Invalid short address
   Description:
	None
   Remarks:
    None 
 */

#define INVALID_SHORT_ADDRESS           (0xFFFF)

// *****************************************************************************
/* BATT_LIFE_EXT_BIT_POS

   Summary:
    This macro holds Battery life extention bit position and 
    Bit position within beacon Superframe Specification field
   Description:
	None
   Remarks:
    None 
 */

#define BATT_LIFE_EXT_BIT_POS               (12U)

// *****************************************************************************
/* PAN_COORD_BIT_POS

   Summary:
    This macro holds PAN coordinator bit position
   Description:
	None
   Remarks:
    None 
 */

#define PAN_COORD_BIT_POS                   (14U)

// *****************************************************************************
/* ASSOC_PERMIT_BIT_POS

   Summary:
    This macro holds Association permit bit position
   Description:
	None
   Remarks:
    None 
 */

#define ASSOC_PERMIT_BIT_POS                (15U)

// *****************************************************************************
/* ASSOC_PERMIT_BIT_POS

   Summary:
    This macro holds Maximum length of the key id field
   Description:
	None
   Remarks:
    None 
 */

#ifdef MAC_SECURITY_ZIP
#define MAX_KEY_ID_FIELD_LEN                (9U)
#endif

/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* IEEE_MAC_CONST_H */
/* EOF */
