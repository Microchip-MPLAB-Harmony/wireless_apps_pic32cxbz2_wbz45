/*******************************************************************************
  MAC API Header
  
  File Name:
    mac_api.h

  Summary:
    This file contains MAC Callback's,Confirmation Callback's,Indication Callback's
    declarations

  Description:
   The IEEE standard 802.15.4 MAC Layer defines the protocol and compatible
   interconnection for data communication devices using low data rate, low 
   power and low complexity, short-range radio frequency (RF) transmissions in a
   wireless personal area network (WPAN).

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

#ifndef MAC_API_H
#define MAC_API_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************


#include "mac.h"
#include "mac_config.h"

#ifdef MAC_SECURITY_ZIP
#include "app_config.h"
#endif

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Release Version Macros
// *****************************************************************************
// *****************************************************************************


/* Major Number
 
   Summary:
    This macro holds the stack Major number 
   Description:
    None
   Remarks:
    None 
 */
#define MAC_MAJOR_NUM                 "1"


/* Minor Number
 
   Summary:
    This macro holds the stack Minor number 
   Description:
    None
   Remarks:
    None 
 */
#define MAC_MINOR_NUM                 "0"

/* Patch Number
 
   Summary:
    This macro holds the stack patch number 
   Description:
    None
   Remarks:
    None 
 */
#define MAC_PATCH_NUM                 "0"


/* MAC Version
 
   Summary:
    This macro holds the MAC SW version as a String 
   Description:
    None
   Remarks:
    None 
 */
#if (defined RC_NUM)
#define MAC_VER   "802.15.4-MAC v" MAC_MAJOR_NUM"." MAC_MINOR_NUM"." MAC_PATCH_NUM"-rc." RC_NUM
#else
#define MAC_VER   "802.15.4-MAC v" MAC_MAJOR_NUM"." MAC_MINOR_NUM"." MAC_PATCH_NUM
#endif

/* Release version information in 32-bit bitfield 
 
| bit pos | field name      | meaning                        |
|---------|-----------------|------------------------------  |
| 0-13    | reserved        | NA                             |
| 14-17   | build itreation | running version of this release|
| 18-19   | qualifier       | 00 - reserved                  |
|         |                 | 01 - Production (P)            |
|         |                 | 10 - Engineering (E)           |
|         |                 | 11 - reserved                  |
| 20-23   | stack minor     | minor version                  |
| 24-27   | stack major     | major version                  |
| 28-31   | reserved        | NA                             |


Example:
  802.15.4-MAC v1.0.0 is represented as 0x01040000

|0000       |0001        | 0000        | 01        | 0000           | 00000000000000|
|-----------|------------|-------------|-----------|----------------|---------------|
|Reserved   | Stack Major| Stack Minor | Qualifier | Build Iteration| Reserved      |
*/

 
/* MAC Software Version Information in 32-bit bitfield
 
   Summary:
    This macro holds MAC Software Version Information in 32-bit bitfield
   Description:
    None
   Remarks:
    None 
*/
#define MAC_VERSION_VALUE      (0x01040000)


/* === Externals ============================================================ */

// *****************************************************************************
// *****************************************************************************
//Section: Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC Address type
 
   Summary:
    This union contain MAC address type information
 
   Description:
    None
   Remarks:
    None 
 */

typedef union {
	uint16_t shortAddress;
	uint64_t longAddress;
} AddressField_t;


// *****************************************************************************
/* device address specification
 
   Summary:
    This structure contain device address specification information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct wpan_addr_spec_tag {
	/** Address mode (@ref WPAN_ADDRMODE_NONE, @ref WPAN_ADDRMODE_SHORT, or
     * @ref WPAN_ADDRMODE_LONG) */
	uint8_t AddrMode;

	/** The 16 bit PAN identifier. */
	uint16_t PANId;

	/** Device address. If AddrMode is @ref WPAN_ADDRMODE_SHORT, it is
	 * interpreted as a 16 bit address. */
	AddressField_t Addr;
} WPAN_AddrSpec_t;

// *****************************************************************************
/* PAN descriptor
 
   Summary:
    This structure contain PAN descriptor information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct wpan_pandescriptor_tag {
	/** Coordinator address specification in received beacon frame */
	WPAN_AddrSpec_t CoordAddrSpec;
	/** The current logical channel used by the network */
	uint8_t LogicalChannel;
	/** The current channel page occupied by the network */
	uint8_t ChannelPage;
	/** Superframe specification in received beacon frame */
	uint16_t SuperframeSpec;
	/** Set to true if the beacon is from a PAN coordinator accepting GTS requests */
	bool GTSPermit;
	/** LQI at which the beacon was received.Lower values represent poorer link quality. */
	uint8_t LinkQuality;

#ifdef ENABLE_TSTAMP

	/** Time at which the beacon frame was received, in symbol counts.  This
	 * quantity shall be interpreted as only 24-bits, with the most significant
	 * 8-bits entirely ignored. */
	uint32_t TimeStamp;
#endif  /* ENABLE_TSTAMP */
} WPAN_Pandescriptor_t;

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

// *****************************************************************************
/* Device Descriptor
 
   Summary:
    This structure contain Device Descriptor information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct mac_device_desc_tag {
	/* The 16-bit PAN identifier of the device in this DeviceDescriptor. */
	uint16_t PANId;

	/* The 16-bit short address of the device in this DeviceDescriptor.
	   A value of 0xfffe indicates that this device is using only its
	   extended address. A value of 0xffff indicates that this value is
	   unknown. */
	uint16_t ShortAddress;

	/* The 64-bit IEEE extended address of the device in this DeviceDescriptor.
	   This element is also used in unsecuring operations on incoming frames. */
	uint64_t ExtAddress;

	/* The incoming frame counter of the device in this DeviceDescriptor.
       This value is used to ensure sequential freshness of frames. */
	uint32_t FrameCounter;

	/* Indication of whether the device may override the minimum security
       level settings. */
	bool Exempt;
} MAC_DeviceDesc_t;

// *****************************************************************************
/* KeyUsage Descriptor
 
   Summary:
    This structure contain KeyUsage Descriptor information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct mac_key_usage_tag {
	/* FrameType element of the KeyUsageDescriptor */
	uint8_t Frametype;

	/* CommandFrameIdentifier of the KeyUsageDescriptor */
	uint8_t CommandFrameIdentifier;
} MAC_KeyUsage_t;

// *****************************************************************************
/* KeyDevice Descriptor
 
   Summary:
    This structure contain KeyDevice Descriptor information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct mac_key_device_desc_tag {
	/* Handle to the DeviceDescriptor corresponding to the device. */
	uint8_t DeviceDescriptorHandle;

	/* Indication of whether the device indicated by DeviceDescriptorHandle
	   is uniquely associated with the KeyDescriptor, i.e., it is a link
	   key as opposed to a group key. */
	bool UniqueDevice;

	/* Indication of whether the device indicated by DeviceDescriptorHandle
       previously communicated with this key prior to the exhaustion of the
	   frame counter. If TRUE, this indicates that the device shall not use
	   this key further because it exhausted its use of the frame counter
	   used with this key. */
	bool BlackListed;
} MAC_KeyDeviceDesc_t;

// *****************************************************************************
/* KeyIdLookup Descriptor
 
   Summary:
    This structure contain KeyIdLookup Descriptor information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct mac_key_id_lookup_desc_tag {
	/* Data used to identify the key. */
	uint8_t LookupData[9];

	/* A value of 0x00 indicates a set of 5 octets, a value of 0x01 indicates
	   a set of 9 octets. */
	uint8_t LookupDataSize;
} MAC_KeyIdLookupDesc_t;

// *****************************************************************************
/* macKeyTable
 
   Summary:
    This structure contain KeyIdLookup Descriptor information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct mac_key_table_tag {
	/* List of KeyIdLookupDescriptor entries. */
	MAC_KeyIdLookupDesc_t KeyIdLookupList[
		MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES];
	/* The number of entries in KeyIdLookupList. */
	uint8_t KeyIdLookupListEntries;

	/* List of KeyDeviceDescriptor entries. */
	MAC_KeyDeviceDesc_t KeyDeviceList[MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES];
	/* The number of entries in KeyDeviceList. */
	uint8_t KeyDeviceListEntries;

	/* List of KeyUsageDescriptor entries. */
	MAC_KeyUsage_t KeyUsageList[MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES];
	/* The number of entries in KeyUsageList. */
	uint8_t KeyUsageListEntries;

	/* Set of 16 octets - the actual value of the key. */
	uint8_t Key[16];
} MAC_KeyTable_t;

// *****************************************************************************
/* macSecurityLevelTable
 
   Summary:
    This structure contain macSecurityLevelTable information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct mac_sec_lvl_table_tag {
	/* FrameType element of the KeyUsageDescriptor */
	uint8_t FrameType;

	/* CommandFrameIdentifier of the KeyUsageDescriptor */
	uint8_t CommandFrameIdentifier;

	/*  The minimal required/expected security level for incoming
	    MAC frames with the indicated frame type and, if present,
	    command frame type. */
	uint8_t SecurityMinimum;

	/* Indication of whether originating devices for which the Exempt flag
	   is set may override the minimum security level indicated by the
	   SecurityMinimum element.
	   If TRUE, this indicates that for originating devices with Exempt
	   status, the incoming security level zero is acceptable, in addition to 
       the incoming security levels meeting the minimum expected security level
	   indicated by the SecurityMinimum element. */
	bool DeviceOverrideSecurityMinimum;
} MAC_SecLvlTable_t;

// *****************************************************************************
/* macDeviceTable
 
   Summary:
    This structure contain macDeviceTable information
 
   Description:
    None
   Remarks:
    None 
 */

typedef struct mac_dev_table_tag {
	MAC_DeviceDesc_t DeviceDescriptor[1];
} MAC_DevTable_t;

// *****************************************************************************
/* MAC Security related PIB attributes
 
   Summary:
    This structure contain the MAC Security related PIB attributes information
 
   Description:
    None
   Remarks:
    None 
 */
typedef struct mac_sec_pib_tag {
	/* Holds a table of KeyDescriptor entries, each containing keys and
	   related information required for secured communications. */
	MAC_KeyTable_t KeyTable[MAC_ZIP_MAX_KEY_TABLE_ENTRIES];

	/* Holds the number of entries in macKeyTable. */
	uint8_t KeyTableEntries;

	/* Holds a table of DeviceDescriptor entries, each indicating a remote 
       device with which this device securely communicates. */
	MAC_DevTable_t DeviceTable[MAC_ZIP_MAX_DEV_TABLE_ENTRIES];

	/* Holds the number of entries in macDeviceTable. */
	uint16_t DeviceTableEntries;

	/* Holds a table of SecurityLevelDescriptor entries, each with
	   information about the minimum security level expected depending 
       on incoming frame type and subtype. */
	MAC_SecLvlTable_t SecurityLevelTable[MAC_ZIP_MAX_SEC_LVL_TABLE_ENTRIES
	];

	/* Holds the number of entries in macSecurityLevelTable. */
	uint8_t SecurityLevelTableEntries;

	/* Holds the outgoing frame counter for this device. */
	uint32_t FrameCounter;

	/* Holds the originator of the default key used for key identifier mode
       0x01. */
	uint8_t DefaultKeySource[8];

	/* Holds the 64-bit extended address of the PAN coordinator */
	uint8_t PANCoordExtendedAddress[8];

	/* Holds the 16-bit short address of the PAN coordinator */
	uint16_t PANCoordShortAddress;
} MAC_SecPib_t;
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC NHLE QUEUE CAPACITY
 
   Summary:
    This macro defines Capacity of queue between MAC and Next Higher Layer
   Description:
    None
   Remarks:
    None 
 */

#define MAC_NHLE_QUEUE_CAPACITY         255U

// *****************************************************************************
// *****************************************************************************
// Section: Symbolic constants for MAC API
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* WPAN_ADDRMODE_NONE
 
   Summary:
    Value for the address mode, where no address is given
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_ADDRMODE_NONE              (0x00)

// *****************************************************************************
/* WPAN_ADDRMODE_SHORT
 
   Summary:
    Value for the address mode, where a 16 bit short address is given
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_ADDRMODE_SHORT             (0x02U)

// *****************************************************************************
/* WPAN_ADDRMODE_LONG
 
   Summary:
    Value for the address mode, where a 64 bit long address is given
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_ADDRMODE_LONG              (0x03U)

// *****************************************************************************
/* WPAN_CAP_ALTPANCOORD
 
   Summary:
    Flag value for capability information field
    The alternate PAN coordinator subfield shall be set if the device is
    capable of becoming a PAN coordinator. Otherwise, the alternate 
    PAN coordinator subfield shall be set to 0.
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_CAP_ALTPANCOORD            (0x01)

// *****************************************************************************
/* WPAN_CAP_FFD
 
   Summary:
    Flag value for capability information field
    The device type subfield shall be set if the device is an FFD. Otherwise,
    the device type subfield shall be set to 0 to indicate an RFD.
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_CAP_FFD                    (0x02)

// *****************************************************************************
/* WPAN_CAP_PWRSOURCE
 
   Summary:
    Flag value for capability information field
    The power source subfield shall be set if the device is receiving power
    from the alternating current mains. Otherwise, the power source subfield
    shall be set to 0
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_CAP_PWRSOURCE              (0x04)

// *****************************************************************************
/* WPAN_CAP_RXONWHENIDLE
 
   Summary:
    Flag value for capability information field
    The receiver on when idle subfield shall be set if the device does not
    disable its receiver to conserve power during idle periods. Otherwise, the
    receiver on when idle subfield shall be set to 0.
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_CAP_RXONWHENIDLE           (0x08)

// *****************************************************************************
/* WPAN_CAP_ALLOCADDRESS
 
   Summary:
    Flag value for capability information field
    The allocate address subfield shall be set if the device wishes the
    coordinator to allocate a short address as a result of the association
    procedure. If this subfield is set to 0, the special short address of
    0xfffe shall be allocated to the device and returned through the
    association response command. In this case, the device shall communicate
    on the PAN using only its 64 bit extended address.
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_CAP_ALLOCADDRESS           (0x80)

// *****************************************************************************
/* WPAN_DISASSOC_BYPARENT
 
   Summary:
    Flag value for capability information field
    Symbolic constant for disassociate reason - initiated by parent(WPAN_MLME_DisassociateReq()).
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_DISASSOC_BYPARENT          (0x01)

// *****************************************************************************
/* WPAN_DISASSOC_BYCHILD
 
   Summary:
    Flag value for capability information field
    Symbolic constant for disassociate reason - initiated by child(WPAN_MLME_DisassociateReq()).
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_DISASSOC_BYCHILD           (0x02)

// *****************************************************************************
/* WPAN_NUM_SHORT_ADDR_PENDING
 
   Summary:
    This macro defines extract size of short address list in PAN descriptor
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_NUM_SHORT_ADDR_PENDING(x)      ((x) & 0x7)

// *****************************************************************************
/* WPAN_NUM_EXTENDED_ADDR_PENDING
 
   Summary:
    This macro defines extract size of extended address list in PAN descriptor
   Description:
    None
   Remarks:
    None 
 */

#define WPAN_NUM_EXTENDED_ADDR_PENDING(x)   (((x) >> 4) & 0x7)

// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    MAC_Retval_t WPAN_Init(void);

  Summary:
    Initialization of MAC Layer

  Description:
    This function initializes the MAC Layer.
    This is function is called to Initializes all stack resources including 
    the microcontroller and transceiver using functions provided by the PHY 
    and the PAL.

  Precondition:
    PHY_Init() should have been called before calling this function. 

  Parameters:
    None.

  Returns:
    MAC_SUCCESS - if MAC initialized successfully
    FAILURE     - otherwise

  Example:
    <code>
    MAC_Retval_t retVal = FAILURE;
 
    retVal = WPAN_Init();
    if (MAC_SUCCESS =! retVal)
    {
        while(1);
    }
    </code>

  Remarks:
    This routine must be called before any of the MAC-WPAN function is called
*/

MAC_Retval_t WPAN_Init(void);

// *****************************************************************************
/*
  Function:
    bool WPAN_Task(void);

  Summary:
    Runs the WPAN Task scheduler

  Description:
    This function invokes the corresponding task functions of the MCL, PHY, and PAL 
    Using the MAC software package it is required to call this function frequently 
    supporting a round robin approach ensures that the different layers state machines 
    are served, and their queues are processed.
    This routine must be called from the RTOS Task function in case of any RTOS is used.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    None.

  Returns:
    Boolean true if an event was processed otherwise false.

  Example:
    <code>
    WPAN_Task();
    </code>

  Remarks:
    This routine must be called before any of the MAC-WPAN function is called
*/

bool WPAN_Task(void);

// *****************************************************************************
/*
  Function:
    bool WPAN_MCPS_Datareq(uint8_t SrcAddrMode, WPAN_AddrSpec_t 
    *DstAddrSpec, uint8_t msduLength, uint8_t *msdu, uint8_t 
    msduHandle, uint8_t TxOptions)

  Summary:
    Initiate MCPS-DATA.request service and have it placed in the MCPS-SAP queue.

  Description:
    This function is used to send the data packets from one node to another.
    It Initiate MCPS-DATA.request service and have it placed in the MCPS-SAP queue.
    The MCPS-DATA.request primitive requests the transfer of a data SPDU (i.e., 
    MSDU) from a local entity to a single peer entity.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    SrcAddrMode     - Address Mode of the source address.
    DstAddrSpec     - Pointer to WPAN_AddrSpec_t structure for destination.
    msduHandle      - Handle (identification) of the MSDU.
    TxOptions     - Bitmap for transmission options. Valid values:
                      - @ref WPAN_TXOPT_OFF,
                      - @ref WPAN_TXOPT_ACK,
                      - @ref WPAN_TXOPT_INDIRECT,
                      - @ref WPAN_TXOPT_INDIRECT_ACK.
    msdu            - Pointer to the data to be transmitted.
    msduLength      - Length of the data to be transmitted.
    SecurityLevel   - Used security level; this parameter is only available
                      if MAC security is enabled via MAC_SECURITY_ZIP
    KeyIdMode       - Used mode to identify the key; this parameter is only 
                      available if MAC security is enabled via MAC_SECURITY_ZIP
    KeyIndex        - Used index of the key; this parameter is only available
                      if MAC security is enabled via MAC_SECURITY_ZIP

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    uint8_t src_addr_mode;
    WPAN_AddrSpec_t dst_addr;
    uint8_t msduHandle = 0;
    uint8_t data = (uint8_t)rand();
    src_addr_mode = WPAN_ADDRMODE_SHORT;
    dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
    dst_addr.PANId = DEFAULT_PAN_ID;
    bool retVal = false;
 
    retVal = WPAN_MCPS_Datareq(src_addr_mode,
            &dst_addr,
            1, 
            &data,
            msduHandle,
            WPAN_TXOPT_ACK);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MCPS_Datareq, MAC layer will perform respective operation 
    and asynchronously invokes USR_MCPS_DataConf with proper status
*/

#if (defined(MAC_SECURITY_ZIP) || defined(MAC_SECURITY_2006))
bool WPAN_MCPS_Datareq(uint8_t SrcAddrMode,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t msduHandle,
		uint8_t TxOptions,
		uint8_t SecurityLevel,
		uint8_t *KeySource,
		uint8_t KeyIdMode,
		uint8_t KeyIndex);

#else   /* No MAC_SECURITY */
bool WPAN_MCPS_Datareq(uint8_t SrcAddrMode,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t msduHandle,
		uint8_t TxOptions);

#endif

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

// *****************************************************************************
/*
  Function:
    bool WPAN_MCPS_PurgeReq(const uint8_t msduHandle)

  Summary:
     Initiates MCPS-PURGE.request service and have it placed in the MCPS-SAP 
     queue.

  Description:
    This function is used to purge(remove) the data packets from MAC?s internal 
    queue. It initiates MCPS-PURGE.request service and have it placed in the 
    MCPS-SAP queue.
    The MCPS-PURGE.request primitive allows the next higher layer to purge an 
    MSDU from the transaction queue

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    msduHandle     -  Handle of MSDU to be purged.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    uint8_t msduHandle = 0;
    bool retVal = false;
 
    retVal = WPAN_MCPS_PurgeReq(msduHandle);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MCPS_PurgeReq, MAC layer will perform respective 
    operation and asynchronously invokes USR_MCPS_PurgeConf with proper status.

 */

bool WPAN_MCPS_PurgeReq(uint8_t msduHandle);

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
        **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) 

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_AssociateReq(uint8_t LogicalChannel, uint8_t ChannelPage,
    WPAN_AddrSpec_t *CoordAddrSpec, uint8_t CapabilityInformation)

  Summary:
     Initiate MLME-ASSOCIATE.request service and have it placed in the MLME-SAP
     queue.

  Description:
    This function allows the device to request an association with coordinator.
    Initiate MLME-ASSOCIATE.request service and have it placed in the MLME-SAP 
    queue. The MLME-ASSOCIATE.request primitive allows a device to request an 
    association with a coordinator.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    LogicalChannel        -  The logical channel on which to attempt association.
    ChannelPage           -  The channel page on which to attempt association.
    CoordAddrSpec         -  Pointer to WPAN_AddrSpec_t structure for coordinator.
    CapabilityInformation -  Bitmap that describes the nodes capabilities.
                             (@ref WPAN_CAP_ALTPANCOORD |
                             @ref WPAN_CAP_FFD |
                             @ref WPAN_CAP_PWRSOURCE |
                             @ref WPAN_CAP_RXONWHENIDLE |
                             @ref WPAN_CAP_ALLOCADDRESS)

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    WPAN_AddrSpec_t coordAddrSpec;
    coordAddrSpec.AddrMode = WPAN_ADDRMODE_SHORT;
    coordAddrSpec.PANId = 0x1111;
    coordAddrSpec.Addr.shortAddress = 0x0000U;
    bool retVal = false;
 
    retVal = WPAN_MLME_AssociateReq(
                     13,
                     0,
                     &CoordAddrSpec,
                     WPAN_CAP_ALLOCADDRESS);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_AssociationReq, MAC layer will perform respective 
    operation and asynchronously invokes USR_MLME_AssociateConf with proper 
    status.
*/

bool WPAN_MLME_AssociateReq(uint8_t LogicalChannel,
		uint8_t ChannelPage,
		WPAN_AddrSpec_t *CoordAddrSpec,
		uint8_t CapabilityInformation);

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_AssociateResp(uint64_t DeviceAddress, uint16_t 
    AssocShortAddress,uint8_t status)

  Summary:
     Initiate MLME-ASSOCIATE.response service and place it in the MLME-SAP queue.

  Description:
    This function is used to send the associate response on the receipt of Associate 
    Indication from MAC layer.
    It initiates MLME-ASSOCIATE.response service and place it in the MLME-SAP
    queue. The MLME-ASSOCIATE.response primitive is used to initiate a response to 
    an MLMEASSOCIATE.indication primitive.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    DeviceAddress         -  Extended address for device requesting association.
    AssocShortAddress     -  Short address allocated on successful association.
    status                -  Status of the association attempt. Valid values:
                             - @ref ASSOCIATION_SUCCESSFUL,
                             - @ref PAN_AT_CAPACITY,
                             - @ref PAN_ACCESS_DENIED,
                             - @ref ASSOCIATION_RESERVED.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    usr_mlme_associate_ind_t *mlmeAssocInd;
    usr_mlme_associate_ind_t assocInd;
    assocInd.CapabilityInformation = mlmeAssocInd->CapabilityInformation;
    assocInd.DeviceAddress = mlmeAssocInd->DeviceAddress;
    uint16_t associate_short_addr = 0x0001U;
 
    bool retVal = false;
    retVal = WPAN_MLME_AssociateResp(assocInd.DeviceAddress,
                    associate_short_addr,
                    ASSOCIATION_SUCCESSFUL);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }

    </code>

  Remarks:
    On the receipt of WPAN_MLME_AssociationResp, MAC layer will perform respective 
    operation and asynchronously invokes USR_MLME_AssociateInd with proper 
    status.
*/

bool WPAN_MLME_AssociateResp(uint64_t DeviceAddress,
		uint16_t AssocShortAddress,
		uint8_t status);

#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) 

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_DisassociateReq(WPAN_AddrSpec_t *DeviceAddrSpec,
    uint8_t DisassociateReason, bool TxIndirect)


  Summary:
     Inititate MLME-DISASSOCIATE.request service and have it placed in the
     MLME-SAP queue

  Description:
    This function is used by the associated device to notify the coordinator of its 
    intent to leave the PAN. It is also used by the coordinator to instruct an associated 
    device to leave the PAN.
    The MLME-DISASSOCIATE.request primitive is used by an associated device to 
    notify the coordinator of its intent to leave the PAN. It is also used by the 
    coordinator to instruct an associated device to leave the PAN.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    DeviceAddrSpec        -  Pointer to WPAN_AddrSpec_t structure for device
                             to which to send the disassociation notification
                             command.
    DisassociateReason    -  Reason for disassociation. Valid values:
                             - @ref WPAN_DISASSOC_BYPARENT,
                             - @ref WPAN_DISASSOC_BYCHILD.
    TxIndirect            -  TRUE if the disassociation notification command
                             is to be sent indirectly

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    WPAN_AddrSpec_t DeviceAddrSpec;
    DeviceAddrSpec.AddrMode = WPAN_ADDRMODE_SHORT;
    DeviceAddrSpec.PANId = 0x1111;
    DeviceAddrSpec.Addr.shortAddress = 0x0001;
    uint16_t DisassociateReason = WPAN_DISASSOC_BYCHILD;
    bool TxIndirect = true;
 
    bool retVal = false;
    retVal = WPAN_MLME_DisassociateReq(&DeviceAddrSpec, WPAN_DISASSOC_BYCHILD, true); 
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_DisassociateReq, MAC layer will perform 
    respective operation and asynchronously invokes USR_MLME_DisassociateConf
    with proper status
*/

bool WPAN_MLME_DisassociateReq(WPAN_AddrSpec_t *DeviceAddrSpec,
		uint8_t DisassociateReason,
		bool TxIndirect);

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1) 

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_GetReq(uint8_t PIBAttribute)

  Summary:
    It Initiates MLME-GET.request service and have it placed in the MLME-SAP queue.

  Description:
    This function is used to request the information about the given PIB attribute.
    It Initiates MLME-GET.request service and have it placed in the MLME-SAP queue.
    The MLME-GET.request primitive requests information about a given PIB 
    attribute.


  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    PIBAttribute          -  PIB attribute to be retrieved.
    PIBAttributeIndex     -  Index of the PIB attribute to be read

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    uint8_t phyCurrentPage;
 
    bool retVal = false;
    retVal = WPAN_MLME_GetReq(phyCurrentPage)
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_GetReq, MAC layer will perform respective operation 
    and asynchronously invokes USR_MLME_GetConf with proper status.
*/

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
bool WPAN_MLME_GetReq(uint8_t PIBAttribute, uint8_t PIBAttributeIndex);

#else
bool WPAN_MLME_GetReq(uint8_t PIBAttribute);

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */
#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_OrphanResp(uint64_t OrphanAddress, uint16_t ShortAddress,
	bool AssociatedMember)

  Summary:
    Initiate MLME-ORPHAN.response service and have it placed in MLME_SAP queue.

  Description:
    This function is used to allow the next higher layer of a coordinator to respond to 
    the MLME-ORPHAN.indication primitive.
    It initiates MLME-ORPHAN.response service and have it placed in MLME_SAP 
    queue.
    The MLME-ORPHAN.indication primitive allows the MLME of a coordinator to 
    notify the next higher layer of the presence of an orphaned device.


  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    OrphanAddress         -  Address of orphaned device.
    ShortAddress          -  Short address allocated to orphaned device.
    AssociatedMember      -  Boolean true if the orphaned device is associated.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    uint64_t OrphanAddress = 0x0000111100001111;
    uint16_t shortaddr = 0x0001;
 
    bool retVal = false;
    retVal = WPAN_MLME_OrphanResp(OrphanAddress,shortaddr, true)
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_OrphanResp, MAC layer will perform respective 
    operation and asynchronously invokes USR_MLME_CommStatusInd()with proper 
    status.
*/

bool WPAN_MLME_OrphanResp(uint64_t OrphanAddress,
		uint16_t ShortAddress,
		bool AssociatedMember);

#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_PollReq(WPAN_AddrSpec_t *CoordAddrSpec)

  Summary:
    Initiate MLME-POLL.request service and have it placed in the MLME-SAP queue.

  Description:
    This function is used by device to request data from coordinator.
    It initiates MLME-POLL.request service and have it placed in the MLME-SAP 
    queue.
    The MLME-POLL.request primitive prompts the device to request data from the 
    coordinator.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    CoordAddrSpec         -  Pointer to WPAN_AddrSpec_t structure for the coordinator.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
    <code>
    WPAN_AddrSpec_t coordAddrSpec;
    coordAddrSpec.AddrMode = WPAN_ADDRMODE_SHORT;
    coordAddrSpec.PANId = DEFAULT_PAN_ID;
    coordAddrSpec.Addr.shortAddress = 0x0000U;
 
    bool retVal = false;
    retVal = WPAN_MLME_PollReq(&coordAddrSpec);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_PollReq, MAC layer will perform respective operation 
    and asynchronously invokes USR_MLME_PollConf()with proper status.
*/

bool WPAN_MLME_PollReq(WPAN_AddrSpec_t *CoordAddrSpec);

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_ResetReq(bool SetDefaultPib)

  Summary:
    Initiate MLME-RESET.request service and have it placed in the MLME-SAP queue.

  Description:
    This function is used by the next higher layer entity to request that the MLME 
    performs a reset operation.
    It initiates MLME-RESET.request service and have it placed in the MLME-SAP 
    queue, SetDefaultPib to set all PIB values to their respective defaults.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    SetDefaultPib         -  Defines whether PIB values need to be set to its default 
                             values.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
  <code>
    bool setDefault = true;
    bool retVal = false;
    retVal = WPAN_MLME_ResetReq(setDefault);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_ResetReq, MAC layer will perform respective 
    operation and asynchronously invokes USR_MLME_ResetConf()with proper status.

*/

bool WPAN_MLME_ResetReq(bool SetDefaultPib);

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_SetReq(uint8_t PIBAttribute, void *PIBAttributeValue)

  Summary:
    Initiate MLME-SET.request service and have it placed in MLME_SAP queue.

  Description:
    This function is used to write the given value to the indicated PIB attribute
    Initiate MLME-SET.request service and have it placed in MLME_SAP queue.
    MLME-SAP set primitives define how PIB attributes may be written.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    PIBAttribute         -  PIB attribute to be set.
    PIBAttributeValue    -  Pointer to new PIB attribute value.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
  <code>
    bool retVal = false;
    uint8_t beacon_payload_len = 20;
    
    retVal = WPAN_MLME_SetReq(macBeaconPayloadLength, &beacon_payload_len);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_SetReq, MAC layer will perform respective operation 
    and asynchronously invokes USR_MLME_SetConf()with proper status.

*/

#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
bool WPAN_MLME_SetReq(uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex,
        void *PIBAttributeValue);

#else
bool WPAN_MLME_SetReq(uint8_t PIBAttribute,
		void *PIBAttributeValue);

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006)  */

#if (MAC_RX_ENABLE_SUPPORT == 1) 

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_RxEnableReq(bool DeferPermit, uint32_t RxOnTime,
               uint32_t RxOnDuration)

  Summary:
    Initiate MLME-RX-ENABLE.request service and have it placed in the MLME-SAP 
    queue.

  Description:
    This function is used by the NHLE layer to request that the receiver is either 
    enabled for a finite period of time or disabled.
    It initiates MLME-RX-ENABLE.request service and have it placed in the MLME-SAP 
    queue.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    DeferPermit         - Set to true if receiver enable can be deferred until
                          next super frame if requested time has already passed.
    RxOnTime            - Number of symbols from start of superframe before
                          receiver
    RxOnDuration        - Number of symbols for which the receiver is enabled.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
  <code>
    bool retVal = false;
    bool DeferPermit = true;
    uint32_t RxOnTime = 0x000000;
    uint32_t RxOnDuration = 100;
    retVal = WPAN_MLME_RxEnableReq(DeferPermit, RxOnTime, RxOnDuration);
    if (true =! retVal)
    {
        printf("buffer not available or queue full"); 
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_RxEnableReq, MAC layer will perform respective 
    operation and asynchronously invokes USR_MLME_RxEnableConf()with proper 
    status.

*/

bool WPAN_MLME_RxEnableReq(bool DeferPermit,
		uint32_t RxOnTime,
		uint32_t RxOnDuration);

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1)) 

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_ScanReq(uint8_t ScanType, uint32_t ScanChannels,
        uint8_t ScanDuration, uint8_t ChannelPage)

  Summary:
    It initiates MLME-SCAN.request service and have it placed in the MLME-SAP 
    queue.

  Description:
    This function is used to initiate a channel scan over a given list of channels. A 
    device can use a channel scan to measure the energy on the channel, search for 
    the coordinator with which it associated, or search for all coordinators 
    transmitting beacon frames within the POS of the scanning device.
    It initiates MLME-SCAN.request service and have it placed in the MLME-SAP 
    queue.
    The MLME-SCAN.request primitive is used to initiate a channel scan over a given 
    list of channels. A device can use a channel scan to measure the energy on 
    the channel, search for the coordinator with which it associated, or search
    for all coordinators transmitting beacon frames within the POS of the
    scanning device.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    ScanType            - Type of scan to perform. Valid values:
                          - @ref MLME_SCAN_TYPE_ED,
                          - @ref MLME_SCAN_TYPE_ACTIVE,
                          - @ref MLME_SCAN_TYPE_PASSIVE,
                          - @ref MLME_SCAN_TYPE_ORPHAN.
    ScanChannels        - Channels to be scanned.
    ScanDuration        - Duration of each scan.
    ChannelPage         - The channel page on which to perform the scan.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
  <code>
    bool retVal = false;
    uint8_t current_channel = 13;
    uint8_t current_channel_page = 0;
    uint8_t scan_duration = 5;
    retVal = WPAN_MLME_ScanReq(MLME_SCAN_TYPE_ACTIVE,
                            SCAN_CHANNEL(current_channel),
                            scan_duration,
                            current_channel_page);
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_ScanReq, MAC layer will perform respective operation 
    and asynchronously invokes USR_MLME_ScanConf()with proper status

*/

bool WPAN_MLME_ScanReq(uint8_t ScanType,
		uint32_t ScanChannels,
		uint8_t ScanDuration,
		uint8_t ChannelPage);

#endif

#if (MAC_START_REQUEST_CONFIRM == 1)

// *****************************************************************************
/*
  Function:
    bool WPAN_MLME_StartReq(uint16_t PANId, uint8_t LogicalChannel,
    uint8_t ChannelPage, uint8_t BeaconOrder, uint8_t SuperframeOrder, 
    bool PANCoordinator, bool BatteryLifeExtension, bool CoordRealignment)


  Summary:
    Initiate MLME-START service and have it placed in the MLME-SAP queue.

  Description:
    This function is used by the PAN coordinator to initiate a new PAN.
    It initiates MLME-START service and have it placed in the MLME-SAP queue.
    The MLME-START.request primitive allows the PAN coordinator to initiate a new 
    PAN or to begin using a new super frame configuration. This primitive may also be 
    used by a device already associated with an existing PAN to begin using a new 
    super frame configuration.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    PANId               - PAN identifier to be used by device.
    LogicalChannel      - The logical channel on which to start using the 
                          new super frame configuration.
    ChannelPage         - The channel page on which to begin using 
                          the new super frame configuration.
    SuperframeOrder     - Duration of active portion of super frame.
    PANCoordinator      - Indicates whether node is PAN coordinator of PAN.
    BatteryLifeExtension- Boolean true disables receiver of beaconing device.
    CoordRealignment    - Boolean to transmit Coordinator Realignment command
                          prior to changing to new super frame configuration.

  Returns:
    true - success; false - buffer not available or queue full.

  Example:
  <code>
    bool retVal = false;
    uint8_t current_channel = 13;
    uint8_t current_channel_page = 0;
    uint16_t panid = 0x1111;
    uint8_t default_beacon_order = 15;
    uint8_t default_superframe_order = 15;
    retVal = WPAN_MLME_StartReq(panid,
            current_channel,
            current_channel_page,
            default_beacon_order,
            default_superframe_order,
            true, false,
            false
            );
    if (true =! retVal)
    {
        printf("buffer not available or queue full");
    }
    </code>

  Remarks:
    On the receipt of WPAN_MLME_StartReq, MAC layer will perform respective 
    operation and asynchronously invokes USR_MLME_StartConf()with proper status.

*/

bool WPAN_MLME_StartReq(uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage,
		uint8_t BeaconOrder,
		uint8_t SuperframeOrder,
		bool PANCoordinator,
		bool BatteryLifeExtension,
		bool CoordRealignment
		);

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */


// *****************************************************************************
// Section: callback Prototypes
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void USR_MCPS_DataConf(uint8_t msduHandle, uint8_t status, uint32_t Timestamp)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MCPS-DATA.confirm.

  Description:
    This function implemented by application (NHLE) for MAC
    service MCPS-DATA.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    msduHandle          - Handle of MSDU handed over to MAC earlier.
    status              - Result for requested data transmission request.
    Timestamp           - The time, in symbols, at which the data were transmitted;
                          this parameter is only available if timestamping is
                          enabled via compile switch ENABLE_TSTAMP.

  Returns:
    None.

  Example:
    <code>
    USR_MCPS_DataConf(uint8_t msduHandle, uint8_t status, uint32_t Timestamp)
    {
         msduHandle = msduHandle;
         status = status;
         Timestamp = Timestamp;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

#if defined(ENABLE_TSTAMP)
void USR_MCPS_DataConf(uint8_t msduHandle,
		uint8_t status,
		uint32_t Timestamp);

#else
void USR_MCPS_DataConf(uint8_t msduHandle,
		uint8_t status);

#endif  /* ENABLE_TSTAMP */

// *****************************************************************************
/*
  Function:
    USR_MCPS_DataInd(WPAN_AddrSpec_t * SrcAddrSpec, 
    WPAN_AddrSpec_t * DstAddrSpec,uint8_t msduLength,uint8_t * msdu, 
    uint8_t mpduLinkQuality,uint8_t DSN, uint32_t Timestamp)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MCPS-DATA.indication.

  Description:
    This function implemented by application (NHLE) for MAC
    service MCPS-DATA.indication.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    SrcAddrSpec          - Pointer to source address specification.
    DstAddrSpec          - Pointer to destination address specification.
    msduLength           - Number of octets contained in MSDU.
    DSN                 - Pointer to MSDU.
    mpduLinkQuality      - LQI measured during reception of the MPDU.
    mpduLinkQuality      - The DSN of the received data frame.
    Timestamp            - The time, in symbols, at which the data were received;
                           this parameter is only available if timestamping is 
                           enabled via compile switch ENABLE_TSTAMP.
    SecurityLevel        - Used security level; this parameter is only available
                           if MAC security is enabled via MAC_SECURITY_ZIP
    KeyIdMode            - Used mode to identify the key; this parameter is only
                           available if MAC security is enabled via MAC_SECURITY_ZIP
    KeyIndex             - Used index of the key; this parameter is only available
                           if MAC security is enabled via MAC_SECURITY_ZIP
  Returns:
    None.

  Example:
    <code>
    USR_MCPS_DataInd(WPAN_AddrSpec_t * SrcAddrSpec,
        WPAN_AddrSpec_t * DstAddrSpec,
        uint8_t msduLength,
        uint8_t * msdu,
        uint8_t mpduLinkQuality,
        uint8_t DSN,
        uint32_t Timestamp)

    {
        SrcAddrSpec = SrcAddrSpec;
        DstAddrSpec = DstAddrSpec;
        msduLength = msduLength;
        msdu = msdu;
        mpduLinkQuality = mpduLinkQuality;
        DSN = DSN;
        Timestamp = Timestamp;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

#if (defined(MAC_SECURITY_ZIP) || defined(MAC_SECURITY_2006))
void USR_MCPS_DataInd(WPAN_AddrSpec_t * SrcAddrSpec,
WPAN_AddrSpec_t * DstAddrSpec,
uint8_t msduLength,
uint8_t * msdu,
uint8_t mpduLinkQuality,
uint8_t DSN,
    #if defined(ENABLE_TSTAMP)
uint32_t Timestamp,
    #endif  /* ENABLE_TSTAMP */
uint8_t SecurityLevel,
uint8_t KeyIdMode,
uint8_t KeyIndex);
#else   /* No MAC_SECURITY */
void USR_MCPS_DataInd(WPAN_AddrSpec_t * SrcAddrSpec,
WPAN_AddrSpec_t * DstAddrSpec,
uint8_t msduLength,
uint8_t * msdu,
uint8_t mpduLinkQuality,
    #ifdef ENABLE_TSTAMP
uint8_t DSN,
uint32_t Timestamp);
    #else
uint8_t DSN);
    #endif  /* ENABLE_TSTAMP */
#endif  /* (MAC_SECURITY_ZIP) || (MAC_SECURITY_2006) */

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

// *****************************************************************************
/*
  Function:
    void USR_MCPS_PurgeConf(uint8_t msduHandle, uint8_t status)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MCPS-PURGE.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MCPS-PURGE.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    msduHandle          - Handle (id) of MSDU to be purged.
    status              - Result of requested purge operation.

  Returns:
    None.

  Example:
    <code>
    void USR_MCPS_PurgeConf(uint8_t msduHandle, uint8_t status)

    {
         msduHandle = msduHandle;
         status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MCPS_PurgeConf(uint8_t msduHandle,
		uint8_t status);

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
        **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_AssociateConf(uint16_t AssocShortAddress, uint8_t status)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-ASSOCIATE.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-ASSOCIATE.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    AssocShortAddress   - Short address allocated by the coordinator.
    status              - Result of requested association operation.

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_AssociateConf(uint16_t AssocShortAddress, uint8_t status)

    {
         AssocShortAddress = AssocShortAddress;
         status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_AssociateConf(uint16_t AssocShortAddress,
		uint8_t status);

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_AssociateInd(uint64_t DeviceAddress, uint8_t CapabilityInformation)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-ASSOCIATE.indication.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-ASSOCIATE.indication.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    DeviceAddress         - Extended address of device requesting association.
    CapabilityInformation - Capabilities of device requesting association.
                            (@ref WPAN_CAP_ALTPANCOORD |
                             @ref WPAN_CAP_FFD |
                             @ref WPAN_CAP_PWRSOURCE |
                             @ref WPAN_CAP_RXONWHENIDLE |
                             @ref WPAN_CAP_ALLOCADDRESS)

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_AssociateInd(uint64_t DeviceAddress, uint8_t CapabilityInformation)

    {
         DeviceAddress = DeviceAddress;
         CapabilityInformation = CapabilityInformation;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_AssociateInd(uint64_t DeviceAddress,
		uint8_t CapabilityInformation);

#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_BeaconNotifyInd(uint8_t BSN, WPAN_Pandescriptor_t *PANDescriptor,
       uint8_t PendAddrSpec, uint8_t *AddrList, uint8_t sduLength, uint8_t *sdu)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-BEACON-NOTIFY.indication.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-BEACON-NOTIFY.indication.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    BSN            - Beacon sequence number.
    PANDescriptor  - Pointer to PAN descriptor for received beacon.
    PendAddrSpec   - Pending address specification in received beacon.
    AddrList       - List of addresses of devices the coordinator has pending data.
    sduLength      - Length of beacon payload.
    sdu            - Pointer to beacon payload.

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_BeaconNotifyInd(uint8_t BSN, WPAN_Pandescriptor_t *PANDescriptor,
         uint8_t PendAddrSpec, uint8_t *AddrList, uint8_t sduLength, uint8_t *sdu)

    {
         DeviceAddress = DeviceAddress;
         CapabilityInformation = CapabilityInformation;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_BeaconNotifyInd(uint8_t BSN,
		WPAN_Pandescriptor_t *PANDescriptor,
		uint8_t PendAddrSpec,
		uint8_t *AddrList,
		uint8_t sduLength,
		uint8_t *sdu);

#endif  /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || \
	(MAC_ASSOCIATION_INDICATION_RESPONSE == 1))

// *****************************************************************************
/*
  Function:
    void USR_MLME_CommStatusInd(WPAN_AddrSpec_t *SrcAddrSpec,
       WPAN_AddrSpec_t *DstAddrSpec, uint8_t status)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-COMM-STATUS.indication.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-COMM-STATUS.indication.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    SrcAddrSpec  - Pointer to source address specification.
    DstAddrSpec  - Pointer to destination address specification.
    status       - Result for related response operation.

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_CommStatusInd(WPAN_AddrSpec_t *SrcAddrSpec,
          WPAN_AddrSpec_t *DstAddrSpec, uint8_t status)

    {
         SrcAddrSpec = SrcAddrSpec;
         DstAddrSpec = DstAddrSpec;
         status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_CommStatusInd(WPAN_AddrSpec_t *SrcAddrSpec,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t status);

#endif  /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) ||
         *(MAC_ASSOCIATION_INDICATION_RESPONSE == 1)) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_DisassociateConf(uint8_t status, WPAN_AddrSpec_t *DeviceAddrSpec)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-DISASSOCIATE.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-DISASSOCIATE.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status          - Result of requested disassociate operation.
    DeviceAddrSpec  - Pointer to WPAN_AddrSpec_t structure for device
                      that has either requested disassociation or been
                      instructed to disassociate by its coordinator.

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_DisassociateConf(uint8_t status, WPAN_AddrSpec_t *DeviceAddrSpec)
    {
         SrcAddrSpec = SrcAddrSpec;
         DstAddrSpec = DstAddrSpec;
         status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_DisassociateConf(uint8_t status,
		WPAN_AddrSpec_t *DeviceAddrSpec);

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) || defined(__DOXYGEN__) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_DisassociateInd(uint64_t DeviceAddress,
		uint8_t DisassociateReason)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-DISASSOCIATE.indication.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-DISASSOCIATE.indication.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    DeviceAddress       - Extended address of device which initiated the
                          disassociation request.
    DisassociateReason  - Reason for the disassociation. Valid values:
                        - @ref WPAN_DISASSOC_BYPARENT,
                        - @ref WPAN_DISASSOC_BYCHILD.

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_DisassociateInd(uint64_t DeviceAddress, uint8_t DisassociateReason)
    {
         DeviceAddress = DeviceAddress;
         DisassociateReason = DisassociateReason;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_DisassociateInd(uint64_t DeviceAddress,
		uint8_t DisassociateReason);

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) || defined(__DOXYGEN__) */

#if (MAC_GET_SUPPORT == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_GetConf(uint8_t status,uint8_t PIBAttribute, void *PIBAttributeValue)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-GET.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-GET.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status            - Result of requested PIB attribute get operation.
    PIBAttribute      - Retrieved PIB attribute.
    PIBAttributeValue - Pointer to data containing retrieved PIB attribute

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_GetConf(uint8_t status,uint8_t PIBAttribute, void *PIBAttributeValue)
    {
       status = status;
       PIBAttribute = PIBAttribute;
       PIBAttributeValue = PIBAttributeValue;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_GetConf(uint8_t status,
uint8_t PIBAttribute,
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
uint8_t PIBAttributeIndex,
#endif  /* ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006)) */
void *PIBAttributeValue);
#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_OrphanInd(uint64_t OrphanAddress)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-ORPHAN.indication.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-ORPHAN.indication.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    OrphanAddress            - Address of orphaned device.                 

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_OrphanInd(uint64_t OrphanAddress)

    {
        OrphanAddress = OrphanAddress;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_OrphanInd(uint64_t OrphanAddress);

#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_PollConf(uint8_t status)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-POLL.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-POLL.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status            - Result of requested poll operation.                

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_PollConf(uint8_t status)
    {
        status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_PollConf(uint8_t status);

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

// *****************************************************************************
/*
  Function:
    void USR_MLME_ResetConf(uint8_t status)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-RESET.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-RESET.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status            - Result of requested reset operation.               

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_ResetConf(uint8_t status)
    {
        status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_ResetConf(uint8_t status);

#if (MAC_RX_ENABLE_SUPPORT == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_RxEnableConf(uint8_t status)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-RX-ENABLE.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-RX-ENABLE.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status            - Result of requested receiver enable operation.               

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_RxEnableConf(uint8_t status)
    {
       status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_RxEnableConf(uint8_t status);

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

// *****************************************************************************
/*
  Function:
    void USR_MLME_ScanConf(uint8_t status, uint8_t ScanType,uint8_t ChannelPage,
       uint32_t UnscannedChannels, uint8_t ResultListSize, void *ResultList)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-SCAN.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-SCAN.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status            - Result of requested scan operation.  
    ScanType          - Type of scan performed.   
    ChannelPage       - The channel page on which the scan was performed.
    UnscannedChannels - Bitmap of unscanned channels
    ResultListSize    - Number of elements in ResultList.
    ResultList        - Pointer to array of scan results .          

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_ScanConf(uint8_t status, uint8_t ScanType,uint8_t ChannelPage,
      uint32_t UnscannedChannels, uint8_t ResultListSize, void *ResultList)
    {
       status = status;
       ScanType = ScanType;
       ChannelPage = ChannelPage;
       UnscannedChannels = UnscannedChannels;
       ResultListSize = ResultListSize;
       ResultList = ResultList;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_ScanConf(uint8_t status,
		uint8_t ScanType,
		uint8_t ChannelPage,
		uint32_t UnscannedChannels,
		uint8_t ResultListSize,
		void *ResultList);

#endif

// *****************************************************************************
/*
  Function:
    void USR_MLME_SetConf(uint8_t status, uint8_t PIBAttribute)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-SET.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-SET.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status            - Result of requested PIB attribute set operation.  
    PIBAttribute      - Updated PIB attribute.              

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_SetConf(uint8_t status, uint8_t PIBAttribute);
    {
       status = status;
       PIBAttribute = PIBAttribute;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
void USR_MLME_SetConf(uint8_t status,
		uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex);

#else
void USR_MLME_SetConf(uint8_t status,
		uint8_t PIBAttribute);

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#if (MAC_START_REQUEST_CONFIRM == 1)

// *****************************************************************************
/*
  Function:
    void USR_MLME_StartConf(uint8_t status)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-START.confirm.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-START.confirm.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    status            - Result of requested start operation.       

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_StartConf(uint8_t status)
    {
        status = status;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_StartConf(uint8_t status);

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

// *****************************************************************************
/*
  Function:
    void USR_MLME_SyncLossInd(uint8_t LossReason, uint16_t PANId,
		uint8_t LogicalChannel, uint8_t ChannelPage)

  Summary:
    Callback function that must be implemented by application (NHLE) for MAC
    service MLME-SYNC-LOSS.indication.
  Description:
    This function implemented by application (NHLE) for MAC
    service MLME-SYNC-LOSS.indication.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    LossReason           - Result of requested start operation.     
    PANId                - The PAN identifier with which the device lost
                           synchronization or to which it was realigned.
    LogicalChannel       - The logical channel on which the device lost
                           synchronization or to which it was realigned.
    ChannelPage          - The channel page on which the device lost
                           synchronization or to which it was realigned.  

  Returns:
    None.

  Example:
    <code>
    void USR_MLME_SyncLossInd(uint8_t LossReason, uint16_t PANId,
         uint8_t LogicalChannel, uint8_t ChannelPage)
    {
       LossReason = LossReason;
       PANId = PANId;
       LogicalChannel = LogicalChannel;
       ChannelPage = ChannelPage;
    }
    </code>

  Remarks:
    There is weak function for this callback. User has to define own implementation 
    for required operation on the reception of particular callback.

*/

void USR_MLME_SyncLossInd(uint8_t LossReason,
		uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage);

// *****************************************************************************
/*
  Function:
    uint8_t MAC_GetPibAttributeSize(uint8_t pibAttributeId)

  Summary:
    Gets the size of a PIB attribute.
  Description:
    This function gets the size of a PIB attribute.

  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    pibAttributeId   - PIB attribute.     

  Returns:
    Size (number of bytes) of the PIB attribute.

  Remarks:
    None.

*/

uint8_t MAC_GetPibAttributeSize(uint8_t pibAttributeId);

// *****************************************************************************
/*
  Function:
    uint32_t MAC_ReadyToSleep(void)

  Summary:
    Checks if the mac stack is ready to sleep.
  Description:
    Checks if the mac stack is in inactive state for beacon support
    or idle in case of no beacon support.

  Precondition:
    WPAN_Init() should have been called before calling this function.. 

  Parameters:
    None.     

  Returns:
    32bit time duration in microseconds for which the mac is ready to sleep.
    Uint32_t ? 0- If MAC is busy, 1 ? If MAC is not busy, ready to sleep.

  Example:
    <code>

    if(MAC_ReadyToSleep())
    {           
        prinf("MAC is idle, device can sleep now");                    
    }
    else
    {
        prinf("MAC is busy now");
    }
    </code>

  Remarks:
    Application or the next higher layer can use this API to find the MAC state.

*/

uint32_t MAC_ReadyToSleep(void);

#if  (defined ENABLE_DEVICE_DEEP_SLEEP)
// *****************************************************************************
/*
  Function:
    void MAC_Wakeup(void)

  Summary:
    MAC Wakeup Callback Function from application
  Description:
    This function implements the post sleep functionalities. If device is going to 
    Deep sleep, then this function must be called after device wakeup routine to 
    retrieve the Retention RAM parameters to system RAM.


  Precondition:
    WPAN_Init() should have been called before calling this function. 

  Parameters:
    None.     

  Returns:
    None.

  Example:
    <code>
    MAC_Wakeup();
    </code>

  Remarks:
    None.

*/
void MAC_Wakeup(void);

#endif

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* MAC_API_H */
/* EOF */
