/*******************************************************************************
  MAC Header
  
  File Name:
    mac_data_structures.h

  Summary:
    MAC related data structures, types and enums.

  Description:
    This file contains  MAC related data structures, types and enums.

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

#ifndef MAC_DATA_STRUCTURES_H
#define MAC_DATA_STRUCTURES_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "mac.h"
#include "mac_api.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

/* === Macros =============================================================== */

// *****************************************************************************
// *****************************************************************************
// Section: Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Beacon Payload type structure
 
   Summary:
    MAC_BeaconPayload_t holds Beacon Payload frame structure
 
   Description:
    None
 
   Remarks:
    None 
 */
        
typedef struct mac_beacon_payload_tag {
	uint16_t superframeSpec;
	uint8_t gtsSpec;
	uint8_t pendingAddrSpec;
	uint8_t *pendingAddrList;
	uint8_t beaconPayloadLen;
	uint8_t *beaconPayload;
} MAC_BeaconPayload_t;

// *****************************************************************************
/* Data Payload type structure
 
   Summary:
    MAC_DataPayload_t holds Data Payload structure
 
   Description:
    None
 
   Remarks:
    None 
 */

typedef struct mac_data_payload_tag {
	uint8_t *payload;
} MAC_DataPayload_t;

// *****************************************************************************
/* Association Request type structure
 
   Summary:
    MAC_AssocReq_t holds Association Request type structure
 
   Description:
    None
 
   Remarks:
    None 
 */

typedef struct mac_assoc_req_tag {
	uint8_t capabilityInfo;
} MAC_AssocReq_t;

// *****************************************************************************
/* Association Response type structure
 
   Summary:
    MAC_AssocResponse_t holds Association Response type structure
 
   Description:
    None
 
   Remarks:
    None 
 */

typedef struct mac_assoc_response_tag {
	uint16_t shortAddr;
	uint8_t assocStatus;
} MAC_AssocResponse_t;

// *****************************************************************************
/* Disassociation Request type structure
 
   Summary:
    MAC_DisassocReq_t holds Disassociation Request type structure
 
   Description:
    None
 
   Remarks:
    None 
 */

typedef struct mac_disassoc_req_tag {
	uint8_t disassocReason;
} MAC_DisassocReq_t;

// *****************************************************************************
/* Coordinator Realignment type structure
 
   Summary:
    MAC_CoordRealign_t holds Coordinator Realignment type structure
 
   Description:
    None
 
   Remarks:
    None 
 */

typedef struct mac_coord_realign_tag {
	uint16_t panId;
	uint16_t coordShortAddr;
	uint8_t logicalChannel;
	uint16_t shortAddr;
	uint8_t channelPage;
} MAC_CoordRealign_t;

// *****************************************************************************
/* General Command frame payload type structure
 
   Summary:
    FramePayload_t holds General Command frame payload type structure
 
   Description:
    None
 
   Remarks:
    None 
 */

typedef union {
	MAC_BeaconPayload_t beaconData;
	MAC_DataPayload_t data;
	MAC_AssocReq_t assocReqData;
	MAC_AssocResponse_t assocResponseData;
	MAC_DisassocReq_t disassocReqData;
	MAC_CoordRealign_t coordRealignData;
} FramePayload_t;

// *****************************************************************************
/* auxiliary security header information
 
   Summary:
    This structure containing auxiliary security header information
 
   Description:
    None
 
   Remarks:
    None 
 */

typedef struct sec_ctrl_tag {
	unsigned int secLevel : 3;
	unsigned int keyIdMode : 2;
	unsigned int /* reserved */ : 3;
} SecCtrl_t;

typedef struct parse_tag {
	uint16_t fcf;
	uint8_t frameType;
	uint8_t mpduLength;
	uint8_t sequenceNumber;
	uint8_t destAddrMode;
	uint16_t destPanid;
	AddressField_t destAddr;
	uint8_t srcAddrMode;
	uint16_t srcPanid;
	AddressField_t srcAddr;
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
	SecCtrl_t secCtrl;
	uint8_t keyIdLen;
	uint32_t frameCnt;
	uint8_t keyId[MAX_KEY_ID_FIELD_LEN];
#endif  /* #if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006)) */
	uint8_t macCommand;
	uint8_t ppduLinkQuality;
#if (defined ENABLE_TSTAMP)

	/* The timestamping is only required for beaconing networks
	 * or if timestamping is explicitly enabled. */
	uint32_t timeStamp;
#endif  /*  (defined ENABLE_TSTAMP) */
	uint8_t macPayloadLength; /* Length of the MAC payload without FCS */
	FramePayload_t macPayloadData;
} Parse_t;


/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END


#endif /* MAC_DATA_STRUCTURES_H */
/* EOF */
