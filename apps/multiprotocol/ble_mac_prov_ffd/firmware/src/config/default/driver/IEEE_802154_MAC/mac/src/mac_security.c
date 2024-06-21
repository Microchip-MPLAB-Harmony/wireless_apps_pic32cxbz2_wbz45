/*******************************************************************************
  MAC Security Source

  File Name:
    mac_security.c
  Summary:
    Handles MAC security
  Description:
    This file handles MAC security.

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

#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
#include <string.h>
#include "phy.h"
#include "ieee_mac_const.h"
#include "stack_config.h"
#include "mac_internal.h"
#include "stb.h"
#include "mac_security.h"

/* === Macros =============================================================== */

/* Security Control Field: Security Level mask */
#define SEC_CTRL_SEC_LVL_MASK           (0x07U)

/* Security Control Field: Key Identifier mask */
#define SEC_CTRL_KEY_ID_MASK            (0x03U)

/* Security Control Field: Key Identifier Field position */
#define SEC_CTRL_KEY_ID_FIELD_POS       (3U)

/* Security Control : Security Enabled Mask */
#define SEC_CTRL_SEC_EN_MASK            (0x08U)

/* Frame Control Field: Frame Type Identifier mask */
#define FCF_FRAME_TYPE_MASK             (0x03U)

/* Frame Counter MAX Value used to identify the
 * frame counter overflow
 */
#define FRAME_COUNTER_MAX_VAL           (0xFFFFFFFFU)

/* Security Control Filed : Length */
#define SEC_CTRL_FLD_LEN                (0x01U)

/* Frame Counter Size or length */
#define FRAME_COUNTER_LEN               (0x04U)

/* IEEE Address Len */
#define IEEE_ADDR_LEN                   (0x08U)

/* CRC Calculation Output Length */
#define CRC_LEN                         (0x02U)

/* GTS Super Frame specification Length */
#define SUPER_FRAME_SPEC_LEN                    (0x02U)

/* GTS Frame Address Specification Length */
#define GTS_ADDR_SPEC_LEN                               (0x01U)

/* Indirect Data Pending Address Specification Length */
#define PENDING_ADDR_SPEC_LEN                   (0x01U)

/* key id mode and key identifier macros */
#define KEY_ID_MODE_0                   (0x00U)
#define KEY_ID_MODE_1                   (0x01U)
#define KEY_ID_MODE_2                   (0x02U)
#define KEY_ID_MODE_3                   (0x03U)
#define KEY_SRC_LEN_4                   (0x04U)
#define KEY_SRC_LEN_8                   (0x08U)
#define KEY_ID_LEN_MODE_1               (0x01U)
#define KEY_ID_LEN_MODE_2               (0x05U)
#define KEY_ID_LEN_MODE_3               (0x09U)

/* MAC Security : Look Up Data Size and macros */
#define FIVE_OCTET_LOOK_UP              (0x00U)
#define NINE_OCTET_LOOK_UP              (0x01U)

/* 16-bit Short Address Maximum usage Value */
#define SHORT_ADDR_MAX                  (0xFFFDU)

/* 16-bit Short Address minimum Value */
#define SHORT_ADDR_MIN                  (0x0000U)
#define NO_SHORT_ADDR                   (0xFFFEU)

#define LOOK_UP_SIZE_4                    (0x04U)
#define LOOK_UP_SIZE_8                    (0x08U)

#define LOOK_UP_SIZE_5                    (0x05U)
#define LOOK_UP_SIZE_9                    (0x09U)

/* FCF Position in the MAC Header */
#define FCF_POS                          (0x01U)

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

static uint8_t GetMicLength(uint8_t securityLevel);

static inline uint8_t SecAdditionalLen(MCPS_DataReq_t *pmdr);

static inline MAC_Retval_t ParseAuxSecHeader(Parse_t *macParseDataInstance,
		uint8_t *macPayload);

static inline MAC_Retval_t UnsecureFrame(Parse_t *macParseDataBuf,
		uint8_t *mpdu,
		uint8_t *macPayload,
		uint8_t *payloadIndex);

static inline MAC_Retval_t OutgoingKeyRetrieval(MCPS_DataReq_t *pmdr,
		MAC_KeyTable_t **keyDesc);

static inline MAC_Retval_t IncomingSecMaterialRetrieval(Parse_t *macParseDataBuf,
		MAC_KeyTable_t **keyDesc,
		MAC_DeviceDesc_t **deviceDesc,
		MAC_KeyDeviceDesc_t **keyDeviceDesc);

static void CreateNonce(uint8_t *ieeeAddr, uint8_t *frameCnt,
		uint8_t securityLevel, uint8_t *nonce);

static MAC_Retval_t KeyDescriptorLookup(uint8_t *lookupData,
		uint8_t lookupDataSize,
		MAC_KeyTable_t **keyDesc);

static MAC_Retval_t DeviceDescriptorLookup(MAC_DeviceDesc_t deviceDesc,
		uint8_t *deviceLookupData,
		uint8_t deviceLookupDataSize);

static MAC_Retval_t BlacklistCheckingProcedure(uint8_t *deviceLookupData,
		uint8_t deviceLookupDataSize,
		MAC_KeyTable_t *keyDesc,
		MAC_DeviceDesc_t **deviceDesc,
		MAC_KeyDeviceDesc_t **keyDeviceDesc);

static MAC_Retval_t IncomingKeyUsagePolicy(MAC_KeyTable_t *keyDesc,
		uint8_t frameType,
		uint8_t macCmd);

/* === Implementation ====================================================== */

/* --- Helper Functions ---------------------------------------------------- */

/** @brief Gets the length of the Key Identifier field
 *
 *  This function returns the length of the Auxiliary Security Header
 *
 *  @param pmdr Frame information structure of current frame
 *
 *  @return Length of Auxiliary Security Header
 *
 */

static inline uint8_t SecAdditionalLen(MCPS_DataReq_t *pmdr)
{
	uint8_t len;

	/* Auxiliary security header length */
	len  = SEC_CTRL_FLD_LEN /* security ctrl field */
			+ FRAME_COUNTER_LEN /* frame counter length */
			+ GetKeyIdFieldLen(pmdr->KeyIdMode);

	/* MIC length */
	len += GetMicLength(pmdr->SecurityLevel);

	return len;
}

/** @brief Gets the length of the Key Identifier field
 *
 *  This function returns the length of the Key Identifier field
 *  within the Auxiliary Security Header of a secured frame based
 *  on the Key Identifier Mode.
 *
 *  @param keyIdMode Key Identifier Mode
 *
 *  @return Length of Key Identifier field in octets.
 *
 */
uint8_t GetKeyIdFieldLen(uint8_t keyIdMode)
{
	uint8_t lenField = 0;

	switch (keyIdMode) {
	case KEY_ID_MODE_1:
		lenField = KEY_ID_LEN_MODE_1;
		break;

	case KEY_ID_MODE_2:
		lenField = KEY_ID_LEN_MODE_2;
		break;

	case KEY_ID_MODE_3:
		lenField = KEY_ID_LEN_MODE_3;
		break;

	default:
        lenField = 0;
		break;
	}

	return (lenField);
}

/** @brief Calculates the length of the MIC
 *
 *  This function returns the length of the MIC depending on the given security
 *  level
 *
 *  @param security_level Security Level of current frame
 *
 *  @return Length of MIC in octets.
 *
 */
static uint8_t GetMicLength(uint8_t securityLevel)
{
	uint8_t micLen = 0;

	switch (securityLevel) {
	case 1:
	case 5:
		micLen = LEN_MIC_32;
		break;

	case 2:
	case 6:
		micLen = LEN_MIC_64;
		break;

	case 3:
	case 7:
		micLen = LEN_MIC_128;
		break;

	default:
        micLen = 0;
		break;
	}

	return micLen;
}

/** @brief Create the nonce
 *
 *  This function is used to create the nonce using
 *  frame counter for replay protection
 *  with unique IEEE address and security
 *  levels
 *
 *  @param ieee_addr IEEE Address of the destination
 *  @param frameCnt Frame Counter
 *  @param security_level Security Level
 *  @param nonce pointer to store the nonce
 *
 */
static void CreateNonce(uint8_t *ieeeAddr, uint8_t *frameCnt,
		uint8_t securityLevel, uint8_t *nonce)
{
	/*
	 * Create Nonce - Attentation: byte order is inverse in comparison to
	 * RF4CE
	 * RF4CE: Little endian
	 */
	uint8_t *ptr;
	uint8_t *noncePtr = &nonce[1];

	ptr = ieeeAddr;
	ptr += IEEE_ADDR_LEN - 1U;
	for (uint8_t i = 0; i < IEEE_ADDR_LEN; i++) {
		*noncePtr++ = *ptr--;
	}
	ptr = frameCnt;
	ptr += FRAME_COUNTER_LEN - 1U;
	for (uint8_t i = 0; i < FRAME_COUNTER_LEN; i++) {
		*noncePtr++ = *ptr--;
	}
	*noncePtr = securityLevel; /* Only security level NOT security control
	                              * field */
}

/* --- Outgoing Security --------------------------------------------------- */

/** @brief Generates Auxiliary Security Header fields
 *
 *  This function generates MAC Frame
 *  Auxiliary Security Header of a secured frame based
 *  on the given security parameter.
 *
 * @param frame_ptr Frame information structure of outgoing mac frame
 * @param pmdr MCPS_DataReq_t struct pointer of mcps data frame
 * @param frameLen frame length of current frame after appending the aux header
 *
 * @return MAC_COUNTER_ERROR, MAC_SUCCESS
 *
 */
MAC_Retval_t MAC_BuildAuxSecHeader(uint8_t **framePtr, MCPS_DataReq_t *pmdr,
		uint8_t *frameLen)
{
	uint8_t secInfoLen = SecAdditionalLen(pmdr);
	*framePtr = *framePtr - secInfoLen;
	/* Add security field length to original frame length */
	*frameLen += secInfoLen;

	/* Create Auxiliary security header. */
	uint8_t *secMsduPtr = *framePtr;
	/* Fill in Security Control Field. */
	*secMsduPtr = (pmdr->SecurityLevel & SEC_CTRL_SEC_LVL_MASK) |
			((pmdr->KeyIdMode &
			SEC_CTRL_KEY_ID_MASK) << SEC_CTRL_KEY_ID_FIELD_POS);
	secMsduPtr++;

	/* Fill in Frame Counter. */
	if (macSecPib.FrameCounter == FRAME_COUNTER_MAX_VAL) {
		return MAC_COUNTER_ERROR;
	} else {
		(void)memcpy(secMsduPtr, ((uint8_t *)(&macSecPib.FrameCounter)),
				FRAME_COUNTER_LEN);
		secMsduPtr += FRAME_COUNTER_LEN;
	}

	/*
	 * Key Source and Key Index subfields are only present
	 * if KeyIdMode is not zero.
	 */
	if (pmdr->KeyIdMode == KEY_ID_MODE_1) {
		*secMsduPtr = pmdr->KeyIndex;
	} else if (pmdr->KeyIdMode == KEY_ID_MODE_2) {
		(void)memcpy(secMsduPtr, pmdr->KeySource, KEY_SRC_LEN_4);
		secMsduPtr += KEY_SRC_LEN_4;
		*secMsduPtr = pmdr->KeyIndex;
	} else if (pmdr->KeyIdMode == KEY_ID_MODE_3) {
		(void)memcpy(secMsduPtr, pmdr->KeySource, KEY_SRC_LEN_8);
		secMsduPtr += KEY_SRC_LEN_8;
		*secMsduPtr = pmdr->KeyIndex;
	}
    else
    {
        // do nothing
    }

	return MAC_SUCCESS;
}

/** Test vectors for Testing the MAC Security as per IEEE 802.15.4 2011 Spec */
#ifdef MAC_SECURITY_TEST_VECTOR
/** Test Vector  for MAC Cmd Testing */
#if 0
uint8_t keyLocal[16]
	= { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
	    0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};
uint8_t testPayload[] = {0x01, 0xCE};
uint8_t textHeader[]
	= {0x2B, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00, 0x00, 0x00,
	   0x48, \
	   0xDE, 0xAC, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00,
	   0x48, 0xDE, \
	   0xAC, 0x06, 0x05, 0x00, 0x00, 0x00, 0x01, 0xCE};
uint8_t nonceTest[AES_BLOCKSIZE] = {0};

uint8_t hdrLenTest = 29;
uint64_t IeeeAddress = 0xacde480000000001;
uint64_t IeeeAddress_dst  = 0xacde480000000002;
uint32_t FrameCounter = 0x5;
uint8_t SecurityLevel = 0x06;
uint8_t encDataTest[aMaxPHYPacketSize - FCS_LEN + AES_BLOCKSIZE];
uint8_t msduLength = 1;
#endif

/** Test Vector  for MAC Beacon Testing */
#if 0
uint8_t keyLocal[16]
	= { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
	    0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};
uint8_t testPayload[] = {0x51, 0x52, 0x53, 0x54};
uint8_t textHeader[]
	= {0x08, 0xD0, 0x84, 0x21, 0x43, 0x01, 0x00, 0x00, 0x00, 0x00,
	   0x48, \
	   0xDE, 0xAC, 0x02, 0x05, 0x00, 0x00, 0x00, 0x55, 0xCF,
	   0x00, 0x00, \
	   0x51, 0x52, 0x53, 0x54};
uint8_t nonceTest[AES_BLOCKSIZE] = {0};

uint8_t hdrLenTest = 26;
uint64_t IeeeAddress = 0xacde480000000001;
uint32_t FrameCounter = 0x5;
uint8_t SecurityLevel = 0x02;
uint8_t encDataTest[aMaxPHYPacketSize - FCS_LEN + AES_BLOCKSIZE];
uint8_t msduLength = 0;
#endif

/* Test Vector  for Data Encryption Testing */
#if 0
uint8_t keyLocal[16]
	= { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
	    0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF};
uint8_t testPayload[] = {0x61, 0x62, 0x63, 0x64};
uint8_t textHeader[]
	= {0x69, 0xDC, 0x84, 0x21, 0x43, 0x02, 0x00, 0x00, 0x00, 0x00,
	   0x48, 0xDE, 0xAC, 0x01, 0x00, 0x00, 0x00, 0x00, 0x48,
	   0xDE, 0xAC, \
	   0x04, 0x05, 0x00, 0x00, 0x00, 0x61, 0x62, 0x63, 0x64};
uint8_t nonceTest[AES_BLOCKSIZE] = {0};

uint8_t hdrLenTest = 26;
uint64_t IeeeAddress = 0xacde480000000001;
uint64_t IeeeAddress_dst  = 0xacde480000000002;
uint32_t FrameCounter = 0x5;
uint8_t SecurityLevel = 0x04;
uint8_t encDataTest[aMaxPHYPacketSize - FCS_LEN + AES_BLOCKSIZE] = {0};
uint8_t msduLength = 4;
#endif
#endif

/** @brief Secures Outgoing MAC frames
 *
 *  This function secures the given MAC frame.
 *
 *  @param frame Outgoing MAC Frame information
 *  @param mac_payload_ptr mac payload pointer
 *  @param pmdr mcps data request structure
 *
 *  @return MAC_Retval_t MAC_SUCCESS or MAC_UNSUPPORTED_SECURITY
 *
 */
MAC_Retval_t MAC_Secure(MAC_FrameInfo_t *frame, uint8_t *macPayloadPtr,
		MCPS_DataReq_t *pmdr)
{
    MAC_Retval_t retStatus = MAC_SUCCESS;
	/* Test Vector for testing the security module */
#ifdef MAC_SECURITY_TEST_VECTOR
	{
		CreateNonce((uint8_t *)&IeeeAddress, (uint8_t *)&FrameCounter,
				SecurityLevel, nonceTest);
		memcpy(encDataTest, textHeader,
				(hdrLenTest + msduLength));
		stbCcmSecure( encDataTest, /* plain text header (string a)
		                                * and payload concatenated */
				nonceTest,
				keyLocal,         /* security_key */
				hdrLenTest,         /* plain text header
		                                       * length */
				msduLength,         /* Length of payload to be
		                                      * encrypted */
				SecurityLevel,         /* security level **/
				AES_DIR_ENCRYPT);
	}
#endif

	uint8_t *key;
	MAC_KeyTable_t *keyDesc;
    PHY_Retval_t pibStatus = PHY_FAILURE;
    PibValue_t pib_value;
    STB_Ccm_t stbCcmStatus = STB_CCM_FAILURE;
	bool securityEnabled = (bool)(frame->mpdu[FCF_POS] & SEC_CTRL_SEC_EN_MASK);
	/* 7.5.8.2.1 (a) */
	if (securityEnabled == false) {
		pmdr->SecurityLevel = 0;
	}

	/* 7.5.8.2.1 (b) */
	if (securityEnabled == true && pmdr->SecurityLevel == 0U) {
		return MAC_UNSUPPORTED_SECURITY;
	}

	/* 7.5.8.2.1 (c) */
	if ((macPib.mac_SecurityEnabled == false) &&
			(pmdr->SecurityLevel != 0U)) {
		return MAC_UNSUPPORTED_SECURITY;
	}

	/* 7.5.8.2.1 (g) */
	/* Retrieval of the key */
	MAC_Retval_t keyStatus = OutgoingKeyRetrieval(pmdr, &keyDesc);
	if (keyStatus != MAC_SUCCESS) {
		return keyStatus;
	}

	key = keyDesc->Key;

	uint8_t m = GetMicLength(pmdr->SecurityLevel);

	/* Encrypt payload data */
	uint8_t nonce[AES_BLOCKSIZE] = {0};
	uint8_t encData[aMaxPHYPacketSize - FCS_LEN + AES_BLOCKSIZE];
    pibStatus = PHY_PibGet(macIeeeAddress, (uint8_t *)&pib_value);
	/* Extra AES Bloc size at end required for MIC generation. */
	CreateNonce((uint8_t *)&pib_value.pib_value_64bit,
			(uint8_t *)&macSecPib.FrameCounter,
			pmdr->SecurityLevel,
			nonce);
    (void)pibStatus;

	/*
	 * Create string a
	 * Here: MHR | Aux sec header = MHR | SecLvl | FrameCounter
	 * = FCF (2) | SeqNo (1) | AddrFields
	 */
	uint8_t hdrLen = macPayloadPtr - &frame->mpdu[1] - m;
	(void)memcpy(encData, &frame->mpdu[FCF_POS], hdrLen);

	switch (frame->mpdu[FCF_POS] & FCF_FRAME_TYPE_MASK) { /* FCF */
	case FCF_FRAMETYPE_BEACON:                  /* (0x00) */
	{
		/* Append payload (string m) */
		(void)memcpy(&encData[hdrLen], macPayloadPtr, pmdr->msduLength);
		{
			uint8_t *currentKey;

			/* Shall the real key be used or rather a test key? */
			currentKey = key;

			stbCcmStatus = STB_CcmSecure(encData,         /* plain text header
				                          * (string a)
				                          * and payload
				                          * concatenated */
					nonce,
					currentKey,         /*security_key */
					hdrLen,         /* plain text header
				                          * length */
					pmdr->msduLength,         /* Length of
				                                   * payload
				                                   * to be
				                                   * encrypted
				                                   **/
					pmdr->SecurityLevel,         /* security
				                                      * level
				                                      **/
					AES_DIR_ENCRYPT);    
		}
		macSecPib.FrameCounter++;

		/* Replace original payload by secured payload */
		(void)memcpy(macPayloadPtr - m, &encData[hdrLen],
				(uint8_t)(pmdr->msduLength + m));
        (void)stbCcmStatus;
	}
	break;

	case FCF_FRAMETYPE_DATA:                  /* (0x01) */
	{
		/* Append payload (string m) */
		(void)memcpy(&encData[hdrLen], macPayloadPtr,(uint8_t)(pmdr->msduLength));
		{
			uint8_t *currentKey;

			/* Shall the real key be used or rather a test key? */
			currentKey = key;

			stbCcmStatus = STB_CcmSecure(encData,         /* plaintext header
				                          * (string a)
				                          * and payload
				                          * concatenated */
					nonce,
					currentKey,         /*security_key */
					hdrLen,         /* plaintext header
				                          * length */
					pmdr->msduLength,         /* Length of
				                                   * payload
				                                   * to be
				                                   * encrypted
				                                   **/
					pmdr->SecurityLevel,         /* security
				                                      * level
				                                      **/
					AES_DIR_ENCRYPT);
		}

		macSecPib.FrameCounter++;

		/* Replace original payload by secured payload */
		(void)memcpy(macPayloadPtr - m, &encData[hdrLen],
				(uint8_t)(pmdr->msduLength + m));
        (void)stbCcmStatus;
	}
	break;

	default:
    {
		retStatus = MAC_UNSUPPORTED_SECURITY;
        break;
    }
	}
	return retStatus;
}

/** @brief This function retrieves the  key descriptor for the outgoing
 * procedure
 *  IEEE Spec Section 7.5.8.2.2
 *
 *  @param tx_frame Frame information structure of received frame
 *  @param Pointer to the key descriptor
 *
 *  @return MAC_Retval_t MAC_SUCCESS or MAC_UNSUPPORTED_SECURITY or
 * MAC_UNAVAILABLE_KEY
 *
 */
static inline MAC_Retval_t OutgoingKeyRetrieval(MCPS_DataReq_t *pmdr,
		MAC_KeyTable_t **keyDesc)
{
	uint8_t keyLookupDataSize = 0;
	uint8_t lookupData[9] = {0};
	MAC_KeyTable_t *currKeyDesc = {NULL};
    MAC_Retval_t retStatus = MAC_SUCCESS;
	if (pmdr->KeyIdMode == KEY_ID_MODE_0) { /* implicit key identification
		                                **/
		switch (pmdr->DstAddrMode) {
			/* 1) is not applicable for ZIP */
			/* 2) is not applicable for ZIP */
#ifndef MAC_SECURITY_ZIP
        PibValue_t pib_value;
        PHY_Retval_t pibStatus = PHY_FAILURE;
		/* 7.5.8.2.2 - a.1 & a.2 */
		case FCF_NO_ADDR:
			if (macSecPib.PANCoordShortAddress <=
					SHORT_ADDR_MAX &&
					macSecPib.PANCoordShortAddress >=
					SHORT_ADDR_MIN) {
                pibStatus = PHY_PibGet(macPANId, (uint8_t *)&pib_value);
				lookupData[1] = (uint8_t)pib_value.pib_value_16bit;
				lookupData[0] = (uint8_t)(pib_value.pib_value_16bit >> 8);
				lookupData[3]
					= (uint8_t)macSecPib.
						PANCoordShortAddress;
				lookupData[2]
					= (uint8_t)(macSecPib.
						PANCoordShortAddress
						>> 8);
				lookupData[4] = 0x00;
				keyLookupDataSize = FIVE_OCTET_LOOK_UP; /*
				                                            *
				                                            *
				                                            *key
				                                            *
				                                            *
				                                            *lookup
				                                            *
				                                            *
				                                            *size
				                                            * =
				                                            * 5
				                                            * ->
				                                            *
				                                            *
				                                            *key
				                                            *
				                                            *
				                                            *lookup
				                                            *
				                                            *
				                                            *data
				                                            *
				                                            *
				                                            *size
				                                            * =
				                                            * 0
				                                            **/
			} else if (macSecPib.PANCoordShortAddress ==
					NO_SHORT_ADDR) {
				(void)memcpy(&lookupData[0],
						&(macSecPib.
						PANCoordExtendedAddress),
						8);
				lookupData[8] = 0x00;
				keyLookupDataSize = NINE_OCTET_LOOK_UP; /*
				                                            *
				                                            *
				                                            *key
				                                            *
				                                            *
				                                            *lookup
				                                            *
				                                            *
				                                            *size
				                                            * =
				                                            * 9
				                                            * ->
				                                            *
				                                            *
				                                            *key
				                                            *
				                                            *
				                                            *lookup
				                                            *
				                                            *
				                                            *data
				                                            *
				                                            *
				                                            *size
				                                            * =
				                                            * 1
				                                            **/
			}
            (void)pibStatus;
			break;
#endif
		/* 7.5.8.2.2 -  a.3 */
		case FCF_SHORT_ADDR:
			/* lookup_data: Dest PANId right-concatenated */
			lookupData[1] = (uint8_t)pmdr->DstPANId; /* LSB */
			lookupData[0] = (uint8_t)(pmdr->DstPANId >> 8); /* MSB
			                                                 **/
			/* 2-octet dest addr right-concatenated */
			lookupData[3] = (uint8_t)pmdr->DstAddr; /* LSB */
			lookupData[2] = (uint8_t)(pmdr->DstAddr >> 8); /* MSB
			                                                **/
			lookupData[4] = 0x00; /* single octet 0x00 */
			keyLookupDataSize = FIVE_OCTET_LOOK_UP; /* key lookup
			                                            * size = 5
			                                            * -> key
			                                            * lookup
			                                            * data size
			                                            * = 0 */
			break;

		case FCF_LONG_ADDR:
			/* lookup_data: 8-octet dest addr right-concatenated */
			(void)memcpy(&lookupData[0], ((uint8_t *)(&pmdr->DstAddr)), 8);
			lookupData[8] = 0x00; /* single octet 0x00 */
			keyLookupDataSize = NINE_OCTET_LOOK_UP; /* key lookup
			                                            * size = 9
			                                            * -> key
			                                            * lookup
			                                            * data size
			                                            *=
			                                            * 1 */
			break;

		default:
			retStatus = MAC_UNSUPPORTED_SECURITY; /* applies for ZIP only**/
			break;                                 
		}
	} else if (pmdr->KeyIdMode == KEY_ID_MODE_1) { /* explicit key
		                                        * identification */
		/*
		 * lookup_data:
		 * 8-octet of macDefaultKeySource right-concatenated with
		 * single octet: Key Index parameter = tx_frame->keyId[0]
		 */
		for (uint8_t i = 0; i < KEY_SRC_LEN_8; i++) {
			lookupData[i] = macSecPib.DefaultKeySource[i];
		}
		lookupData[KEY_SRC_LEN_8] = pmdr->KeyIndex; /*
		                                              *
		                                              *
		                                              *tx_frame->keyId[0];
		                                              **/
		keyLookupDataSize = NINE_OCTET_LOOK_UP; /* key lookup size =
		                                            * 9 -> key lookup
		                                            * data size = 1 */
	}

#ifdef MAC_SECURITY_2006
	else if (pmdr->KeyIdMode == KEY_ID_MODE_2) {
		/*
		 * lookup_data: 4-octet of KeySource
		 * right-concatenated with
		 * single octet: Key Index parameter
		 */
		uint8_t *FrameKeySource = pmdr->KeySource;
		for (uint8_t i = 0; i < KEY_SRC_LEN_4; i++) {
			lookupData[i] = *FrameKeySource++;
		}
		lookupData[KEY_SRC_LEN_4] =  pmdr->KeyIndex;
		keyLookupDataSize = FIVE_OCTET_LOOK_UP; /* key lookup size =
		                                            * 5 -> key lookup
		                                            * data size = 0 */
	} else if (pmdr->KeyIdMode == KEY_ID_MODE_3) {
		/*
		 * lookup_data: 8-octet of KeySource
		 * right-concatenated with
		 * single octet: Key Index parameter
		 */
		uint8_t *FrameKeySource = pmdr->KeySource;
		for (uint8_t i = 0; i < KEY_SRC_LEN_8; i++) {
			lookupData[i] = *FrameKeySource++;
		}
		lookupData[KEY_SRC_LEN_8] =  pmdr->KeyIndex;
		keyLookupDataSize = NINE_OCTET_LOOK_UP; /* key lookup size =
		                                            * 9 -> key lookup
		                                            * data size = 1 */
	}
#endif  /* MAC_SECURITY_2006 */
	else {
		/* other than 0x00 and 0x01 are NOT used for ZIP */
		return MAC_UNSUPPORTED_SECURITY; /* applies for ZIP only */
	}

	/* Get key from KeyDescriptor as 7.5.8.2.5 */
	MAC_Retval_t status = KeyDescriptorLookup(lookupData,
			keyLookupDataSize,
			&currKeyDesc);
	*keyDesc = currKeyDesc;
	if (status != MAC_SUCCESS) {
		return status;
	}

	return retStatus;
}

/* --- Incoming Security --------------------------------------------------- */

/**
 * @brief Handles Decrypt the received secure MAC frame
 *
 * This function handles the complete Decryption procedure of a received frame.
 * This includes the extraction of the Auxiliary Security Header and
 * the actual frame decryption.
 *
 * @param mac_parse_data_buf Frame information structure of current frame
 * @param mpdu Pointer to the MAC Header
 * @param mac_payload Pointer to the MAC payload
 * @param payload_index Key payload index pointer
 *
 * @return Status of extraction of MAC Secure frame to Unsecured frame
 *
 */
MAC_Retval_t MAC_Unsecure(Parse_t *macParseDataBuf, uint8_t *mpdu,
		uint8_t *macPayload, uint8_t *payloadIndex)
{
	MAC_Retval_t status;
	status = ParseAuxSecHeader(macParseDataBuf, macPayload);
	if (status != MAC_SUCCESS) {
		return status;
	}

	/* 7.5.8.2.3 (d) */
	if (macPib.mac_SecurityEnabled == false) {
		if (macParseDataBuf->secCtrl.secLevel == 0U) {
			return MAC_SUCCESS;
		} else {
			return MAC_UNSUPPORTED_SECURITY;
		}
	}

	/* 7.5.8.2.3 (e) todo implementation of 7.5.8.2.8*/
	/* 7.5.8.2.3 (f) */
	if (macParseDataBuf->secCtrl.secLevel == 0U) {
		return MAC_SUCCESS;
	}

	status = UnsecureFrame(macParseDataBuf, mpdu, macPayload,
			payloadIndex);
	return status;
}

/** @brief Parse the Auxiliary Security Header info fields
 *
 *  This function extracts the actual security parameters
 *  from the received MAC secured frame.
 *
 *  @param mac_parse_data Parsed information of the received frame
 *  @param mac_payload incoming frame mac payload pointer
 *
 *  @return MAC_UNSUPPORTED_LEGACY, MAC_UNSUPPORTED_SECURITY, MAC_SUCCESS
 *
 */
static inline MAC_Retval_t ParseAuxSecHeader(Parse_t *macParseDataInstance,
		uint8_t *macPayload)
{
	(void)memcpy(((uint8_t *)(&macParseDataInstance->secCtrl)), &macPayload[0],
			SEC_CTRL_FLD_LEN);
	(void)memcpy(((uint8_t *)(&macParseDataInstance->frameCnt)), &macPayload[1],
			FRAME_COUNTER_LEN);
	macParseDataInstance->keyIdLen = GetKeyIdFieldLen(
			macParseDataInstance->secCtrl.keyIdMode);
	/* 7.5.8.2.3 (c)*/
	if (macParseDataInstance->secCtrl.secLevel == 0U) {
		return MAC_UNSUPPORTED_SECURITY;
	}

	if (macParseDataInstance->secCtrl.keyIdMode != 0U) {
		macPayload += 5;
		if (macParseDataInstance->keyIdLen > 1U) {
			/* Key Id Len must not be larger than 1 for ZIP. */
#ifndef MAC_SECURITY_2006
			return MAC_UNSUPPORTED_SECURITY;

#else
			(void)memcpy(macParseDataInstance->keyId, macPayload,(uint8_t)(macParseDataInstance->keyIdLen - 1U));
#endif
		}

		macPayload += macParseDataInstance->keyIdLen - 1U;
		macParseDataInstance->keyId[macParseDataInstance->
		keyIdLen - 1U] = *(macPayload);
	}

	/* See 802.15.4-2006 section 7.5.8.2.3 b) */
	if (((bool)(macParseDataInstance->fcf & FCF_SECURITY_ENABLED)) &&
			(!(bool)(macParseDataInstance->fcf &
			FCF_FRAME_VERSION_2006))
			) {
		return MAC_UNSUPPORTED_LEGACY;
	}

	return MAC_SUCCESS;
}

/**
 * @brief Unsecures MAC frame
 *
 * This function unsecures the given MAC frame.
 *
 * @param mac_parse_data Frame information structure of current secured frame
 * @param security_level Security Level of current frame
 *
 * @return MAC_Retval_t MAC_SUCCESS, MAC_UNSUPPORTED_SECURITY or MAC_SECURITY_ERROR
 */
static inline MAC_Retval_t UnsecureFrame(Parse_t *macParseDataBuf,
		uint8_t *mpdu, uint8_t *macPayload,
		uint8_t *payloadIndex)
{
	/* Encrypt payload data */
	uint8_t nonce[AES_BLOCKSIZE] = {0}; /* AES_BLOCKSIZE 16 */

	/* Incoming key retrieval */
	uint8_t *key;
	MAC_KeyTable_t *keyDesc;
	MAC_DeviceDesc_t *deviceDesc;
	MAC_KeyDeviceDesc_t *keyDeviceDesc;
    MAC_Retval_t retStatus = MAC_SUCCESS;

	/* Todo 7.5.8.2.3 (e) 7.5.8.2.8 security level checking procedure*/
	MAC_Retval_t status = IncomingSecMaterialRetrieval(macParseDataBuf,
			&keyDesc,
			&deviceDesc,
			&keyDeviceDesc);
	if (status != MAC_SUCCESS) {
		return MAC_UNAVAILABLE_KEY;
	}

	key = keyDesc->Key;

	/* 7.5.8.2.3 (j) & (k)*/
	if (((FCF_FRAMETYPE_DATA == macParseDataBuf->frameType) || \
			(FCF_FRAMETYPE_BEACON ==
			macParseDataBuf->frameType)) && \
			((macParseDataBuf->frameCnt ==
			FRAME_COUNTER_MAX_VAL) || \
			(macParseDataBuf->frameCnt <
			deviceDesc->FrameCounter))) {
		return MAC_COUNTER_ERROR;
	}

	/*
	 * Create Nonce - Attentation: byte order is inverse in comparison to
	 * RF4CE
	 * RF4CE: Little endian
	 */
	uint8_t *addr_ptr;
	uint8_t *currentKey; /* Pointer to actually used key */

	/* Standard way of extracting Src IEEE address. */
	addr_ptr = (uint8_t *)&deviceDesc->ExtAddress;

	uint8_t m = GetMicLength(macParseDataBuf->secCtrl.secLevel);

	CreateNonce(addr_ptr, (uint8_t *)&macParseDataBuf->frameCnt,
			macParseDataBuf->secCtrl.secLevel, nonce);

	switch (macParseDataBuf->frameType) {
	case FCF_FRAMETYPE_DATA:                  /* (0x01) */
	{
		/* Shall the real key be used or rather a test key? */
		currentKey = key;

		uint8_t secHdrLen = SEC_CTRL_FLD_LEN + FRAME_COUNTER_LEN +
				macParseDataBuf->keyIdLen;                                               /*
			                                                                                       *
			                                                                                       *
			                                                                                       *5
			                                                                                       *
			                                                                                       *=
			                                                                                       *
			                                                                                       *
			                                                                                       *sec
			                                                                                       *
			                                                                                       *
			                                                                                       *ctrl
			                                                                                       *
			                                                                                       *+
			                                                                                       *
			                                                                                       *
			                                                                                       *frame
			                                                                                       *
			                                                                                       *
			                                                                                       *counter
			                                                                                       **/
		uint8_t mhrLen = macPayload - mpdu + secHdrLen;
		uint8_t encryPayloadLen = macParseDataBuf->mpduLength -
				mhrLen - m - CRC_LEN;                                                 /*
			                                                                                *
			                                                                                *
			                                                                                *2
			                                                                                *
			                                                                                *=
			                                                                                *
			                                                                                *
			                                                                                *CRC
			                                                                                **/

		if (STB_CcmSecure(mpdu, /* plaintext header (string a) and
			                  * payload concatenated */
				nonce,
				currentKey,    /* security_key */
				mhrLen,    /* plaintext header length */
				encryPayloadLen,    /* Length of payload to
			                                * be encrypted */
				macParseDataBuf->secCtrl.secLevel,    /*
			                                                    *
			                                                    *
			                                                    *security
			                                                    *
			                                                    *
			                                                    *level
			                                                    **/
				AES_DIR_DECRYPT)
				== STB_CCM_OK) {
			/* Adjust payload by secured payload */
			*payloadIndex = secHdrLen;
			/* Subtract the MIC length from the payload length. */
			macParseDataBuf->macPayloadLength
				= encryPayloadLen;
		} else {
			return MAC_SECURITY_ERROR;
            break;
		}
	}
	break;

	case FCF_FRAMETYPE_BEACON:                          /* (0x00) */
	{
		/* Shall the real key be used or rather a test key? */
		currentKey = key;
		uint8_t beaconAddLen = 0;
		uint8_t gtsFieldLen = 0;
		uint8_t pendingAddrLen = 0;

		uint8_t secHdrLen = SEC_CTRL_FLD_LEN + FRAME_COUNTER_LEN + \
				macParseDataBuf->keyIdLen;                                             /*
			                                                                                     *
			                                                                                     *
			                                                                                     *5
			                                                                                     *
			                                                                                     *=
			                                                                                     *
			                                                                                     *
			                                                                                     *sec
			                                                                                     *
			                                                                                     *
			                                                                                     *ctrl
			                                                                                     *
			                                                                                     *+
			                                                                                     *
			                                                                                     *
			                                                                                     *frame
			                                                                                     *
			                                                                                     *
			                                                                                     *counter
			                                                                                     **/

		uint8_t mhrLen = macPayload - mpdu + secHdrLen;
		uint8_t encryPayloadLen = macParseDataBuf->mpduLength -
				mhrLen - m - CRC_LEN;                                                                 /*
			                                                                                                *
			                                                                                                *
			                                                                                                *2
			                                                                                                *
			                                                                                                *=
			                                                                                                *
			                                                                                                *
			                                                                                                *CRC
			                                                                                                **/

		/* calculate the Fixed Field of the GTS Length and Pending
		 * Address Field Length and
		 * Super Frame Specification
		 */
		beaconAddLen = SUPER_FRAME_SPEC_LEN + GTS_ADDR_SPEC_LEN +
				PENDING_ADDR_SPEC_LEN;

		/* Calculate the variable field GTS Field Length */
		gtsFieldLen
			= (uint8_t)((mpdu[(mhrLen +
				SUPER_FRAME_SPEC_LEN)] & 0x07U) * 3U);
		if ((bool)gtsFieldLen) {
			/* GTS direction fields are available */
			gtsFieldLen++;
		}

		/* Calculate the variable field Pending Address Field Length */
		pendingAddrLen
			= (uint8_t)(((mpdu[(mhrLen + SUPER_FRAME_SPEC_LEN +
				GTS_ADDR_SPEC_LEN + \
				gtsFieldLen)] & 0x07U) * 2U) + \
				(((mpdu[(mhrLen + SUPER_FRAME_SPEC_LEN +
				GTS_ADDR_SPEC_LEN + \
				gtsFieldLen)] & 0x70U) >> 4U) * 8U));

		/* Total Additional Address Length Field */
		beaconAddLen += pendingAddrLen + gtsFieldLen;

		if (STB_CcmSecure(mpdu, /* plaintext header (string a) and
			                  * payload concatenated */
				nonce,
				currentKey,    /* security key */
				(mhrLen + beaconAddLen),    /* plaintext
			                                        * header length
			                                        **/
				(encryPayloadLen - beaconAddLen),    /*
			                                                   *
			                                                   *
			                                                   *Length
			                                                   * of
			                                                   *
			                                                   *
			                                                   *payload
			                                                   * to
			                                                   * be
			                                                   *
			                                                   *
			                                                   *encrypted
			                                                   **/
				macParseDataBuf->secCtrl.secLevel,    /*
			                                                    *
			                                                    *
			                                                    *security
			                                                    *
			                                                    *
			                                                    *level
			                                                    **/
				AES_DIR_DECRYPT)
				== STB_CCM_OK) {
			/* Adjust payload by secured payload */
			*payloadIndex = secHdrLen;
			/* Subtract the MIC length from the payload length. */
			macParseDataBuf->macPayloadLength
				= encryPayloadLen +
					secHdrLen;
		} else {
			return MAC_SECURITY_ERROR;
		}
	}
	break;

	default:
		retStatus = MAC_UNSUPPORTED_SECURITY;
        break;
	}
	/* 7.5.8.2.3 (n) */
	if ((FCF_FRAMETYPE_DATA == macParseDataBuf->frameType) ||
			(FCF_FRAMETYPE_BEACON ==
			macParseDataBuf->frameType)) {
		deviceDesc->FrameCounter = (macParseDataBuf->frameCnt) + 1U;
		/* 7.5.8.2.3 (o) */
		if (deviceDesc->FrameCounter == FRAME_COUNTER_MAX_VAL) {
			keyDeviceDesc->BlackListed = true;
		}
	}

	return retStatus;
}

/** @brief Incoming Security material retrieval
 *
 *  This function frames the lookup data, lookup data size in order
 *  to get the key. Framed lookup data will be used to check the
 *  incoming key usage policy and blacklisted device.
 *
 *  @param mac_parse_data_buf Structure to hold the mac Auxiliary header & data
 *  @param device_desc Structure to hold the device key, device usage list, key
 * usage list
 *  @param key_device_desc structure to hold the KeyDeviceDescriptor
 *
 * @return MAC_UNSUPPORTED_SECURITY or FAILURE, MAC_SUCCESS, MAC_UNAVAILABLE_KEY
 *
 */

static inline MAC_Retval_t IncomingSecMaterialRetrieval(
		Parse_t *macParseDataBuf, MAC_KeyTable_t **keyDesc,
		MAC_DeviceDesc_t **deviceDesc,
		MAC_KeyDeviceDesc_t **keyDeviceDesc)
{
	/* @ToDo: Check a holy bunch of other stuff, see 7.5.8.2.3*/

	uint8_t lookupDataSize = 0;
	uint8_t lookupData[9] = {0};
	uint8_t deviceLookupDataSize = 0;
	uint8_t deviceLookupData[8];
	MAC_KeyTable_t *currKeyDesc;
	MAC_DeviceDesc_t *currentDeviceDesc;
	MAC_KeyDeviceDesc_t *currKeyDeviceDesc;
    MAC_Retval_t retStatus = MAC_SUCCESS;
	/* 7.5.8.2.3 (a) & (b) */
	if (macParseDataBuf->secCtrl.keyIdMode == KEY_ID_MODE_0) { /*
		                                                          *
		                                                          *
		                                                          *implicit
		                                                          **/
		switch (macParseDataBuf->srcAddrMode) {
		/* 1) is not applicable for ZIP */
		/* 2) is not applicable for ZIP */
		case FCF_NO_ADDR:
		{
			if (macSecPib.PANCoordShortAddress <=
					SHORT_ADDR_MAX &&
					macSecPib.PANCoordShortAddress
					>= SHORT_ADDR_MIN) {
				lookupData[1]
					= (uint8_t)(macParseDataBuf->
						destPanid);
				lookupData[0]
					= (uint8_t)(macParseDataBuf->
						destPanid
						>> 8);
				lookupData[3]
					= (uint8_t)macSecPib.
						PANCoordShortAddress;
				lookupData[2]
					= (uint8_t)(macSecPib.
						PANCoordShortAddress
						>> 8);
				lookupData[4] = 0x00;
				lookupDataSize = FIVE_OCTET_LOOK_UP; /* key
					                               * lookup
					                               * size =
					                               * 5 ->
					                               * key
					                               * lookup
					                               * data
					                               * size =
					                               * 0 */
			} else if (macSecPib.PANCoordShortAddress ==
					NO_SHORT_ADDR) {
				(void)memcpy(&lookupData[0],
						&(macSecPib.
						PANCoordExtendedAddress),
						8);
				lookupData[8] = 0x00;
				lookupDataSize = NINE_OCTET_LOOK_UP; /* key
					                               * lookup
					                               * size =
					                               * 9 ->
					                               * key
					                               * lookup
					                               * data
					                               * size =
					                               * 1 */
			}
            else
           {
              // do nothing  
           }
		}
		break;

		case FCF_SHORT_ADDR:
		{
			/* PAN Id: TAL handles PAN ID compression issue */
			lookupData[1] = (uint8_t)macParseDataBuf->srcPanid; /*
				                                                  *
				                                                  *
				                                                  *LSB
				                                                  **/
			lookupData[0]
				= (uint8_t)(macParseDataBuf->srcPanid >>
					8);                                         /*
				                                                     *
				                                                     *
				                                                     *MSB
				                                                     **/
			/* 2-octet dest addr right-concatenated */
			lookupData[3]
				= (uint8_t)macParseDataBuf->srcAddr.
					shortAddress;                                    /*
				                                                           *
				                                                           *
				                                                           *LSB
				                                                           **/
			lookupData[2]
				= (uint8_t)(macParseDataBuf->srcAddr.
					shortAddress
					>> 8);                                                   /*
				                                                                  *
				                                                                  *
				                                                                  *MSB
				                                                                  **/
			lookupData[4] = 0x00; /* single octet 0x00 */
			lookupDataSize = FIVE_OCTET_LOOK_UP; /* key lookup
				                                * size = 5 ->
				                                * key lookup
				                                * data size = 0
				                                **/
		}
		break;

		case FCF_LONG_ADDR:
		{
			/* lookup_data: 8-octet dest addr right-concatenated */
			(void)memcpy(&lookupData[0],
					((uint8_t *)(&macParseDataBuf->srcAddr.longAddress)),
					8);
			lookupData[8] = 0x00; /* single octet 0x00 */
			lookupDataSize = NINE_OCTET_LOOK_UP; /* key lookup
				                                * size = 9 ->
				                                * key lookup
				                                * data size = 1
				                                **/
		}
		break;

		default:
			retStatus = MAC_UNSUPPORTED_SECURITY;
            break;
		}
	} else if (macParseDataBuf->secCtrl.keyIdMode == KEY_ID_MODE_1) {
		/* lookup_data: 8-octet of macDefaultKeySource
		 * right-concatenated */
		/* single octet: Key Index parameter */
		for (uint8_t i = 0; i < 8U; i++) {
			lookupData[i] = macSecPib.DefaultKeySource[i];
		}
		lookupData[8] = macParseDataBuf->keyId[0];
		lookupDataSize = NINE_OCTET_LOOK_UP; /* key lookup size = 9 ->
		                                        * key lookup data size =
		                                        * 1 */
	} else if (macParseDataBuf->secCtrl.keyIdMode == KEY_ID_MODE_2) 
    {
		for (uint8_t i = 0; i < KEY_ID_LEN_MODE_2; i++) 
        {
			lookupData[i] = macParseDataBuf->keyId[i];
		}
		lookupDataSize = FIVE_OCTET_LOOK_UP;
	} 
    else if (macParseDataBuf->secCtrl.keyIdMode == KEY_ID_MODE_3)
    {
		for (uint8_t i = 0; i < KEY_ID_LEN_MODE_3; i++) 
        { 
			lookupData[i] = macParseDataBuf->keyId[i];
		}
        
		lookupDataSize = NINE_OCTET_LOOK_UP;
	} else {
		return MAC_UNSUPPORTED_SECURITY; /* not used by ZIP 0r 2006
		                                  * security */
	}

	/* Get key from KeyDescriptor as 7.5.8.2.5 */
	MAC_Retval_t status = KeyDescriptorLookup(lookupData, lookupDataSize,
			&currKeyDesc);
	if (status != MAC_SUCCESS) {
		return status;
	}

	/** Check the Key Usage Policy from the KeyDescriptor for the
	 * Incoming Frame Type. The policy checking procedure  as per
	 * 7.5.8.2.9
	 */
	status = IncomingKeyUsagePolicy(currKeyDesc,
			macParseDataBuf->frameType,
			macParseDataBuf->macCommand);
	if (status != MAC_SUCCESS) {
		return status;
	}

	*keyDesc = currKeyDesc;
	/* 7.5.8.2.3 (d) */
	switch (macParseDataBuf->srcAddrMode) {
	case FCF_NO_ADDR:
	{
		if (macSecPib.PANCoordShortAddress <= SHORT_ADDR_MAX &&
				macSecPib.PANCoordShortAddress >=
				SHORT_ADDR_MIN) {
			deviceLookupData[1]
				= (uint8_t)macParseDataBuf->destPanid;
			deviceLookupData[0]
				= (uint8_t)(macParseDataBuf->destPanid
					>> 8);
			deviceLookupData[3]
				= (uint8_t)macSecPib.PANCoordShortAddress;
			deviceLookupData[2]
				= (uint8_t)(macSecPib.PANCoordShortAddress
					>> 8);
			deviceLookupDataSize = LOOK_UP_SIZE_4;
		} else if (macSecPib.PANCoordShortAddress == NO_SHORT_ADDR) {
			(void)memcpy(&deviceLookupData[0],
					&(macSecPib.PANCoordExtendedAddress),
					8);
			deviceLookupDataSize = LOOK_UP_SIZE_8;
		}
        else
        {
            // do nothing
        }
	}
	break;

	case FCF_SHORT_ADDR:
	{
		deviceLookupData[1] = (uint8_t)macParseDataBuf->srcPanid; /*
			                                                         *
			                                                         *
			                                                         *LSB
			                                                         **/
		deviceLookupData[0]
			= (uint8_t)(macParseDataBuf->srcPanid >>
				8);                                                    /*
			                                                                *
			                                                                *
			                                                                *MSB
			                                                                **/
		/* 2-octet dest addr right-concatenated */
		deviceLookupData[3]
			= (uint8_t)macParseDataBuf->srcAddr.shortAddress;               /*
			                                                                      *
			                                                                      *
			                                                                      *LSB
			                                                                      **/
		deviceLookupData[2]
			= (uint8_t)(macParseDataBuf->srcAddr.shortAddress
				>> 8);                                                              /*
			                                                                             *
			                                                                             *
			                                                                             *MSB
			                                                                             **/
		deviceLookupDataSize = LOOK_UP_SIZE_4;
	}
	break;

	case FCF_LONG_ADDR:
	{
		(void)memcpy(&deviceLookupData[0],
				((uint8_t *)(&macParseDataBuf->srcAddr.longAddress)),
				LOOK_UP_SIZE_8);
		deviceLookupDataSize = LOOK_UP_SIZE_8; /* key lookup size = 9
			                                   * -> key lookup data
			                                   * size = 1 */
	}
	break;

	default:
		retStatus = MAC_UNSUPPORTED_SECURITY;
        break;
	}
	/* 7.5.8.2.3 (e) */
	status = BlacklistCheckingProcedure(deviceLookupData,
			deviceLookupDataSize,
			currKeyDesc,
			&currentDeviceDesc,
			&currKeyDeviceDesc);
	if (status != MAC_SUCCESS) {
		return FAILURE;
	}

	*deviceDesc = currentDeviceDesc;
	*keyDeviceDesc = currKeyDeviceDesc;
	return retStatus;
}

/** @brief Compare the received key descriptor and available key descriptor from
 * keytable
 *
 *  This function will checks the received key descriptor against with
 *  with the available key descriptor based  on the lookup data size.
 *  If device lookup data match found then based on the match corresponding key
 * will used.
 *
 *  @param lookup_data device lookup data
 *  @param lookup_data_size Incoming frame lookup data size
 *         maximum lookup data size is 9
 *  @param key_desc as per IEEE specification mac key table definitions.
 *
 *  @return MAC_UNSUPPORTED_SECURITY, MAC_SUCCESS, MAC_UNAVAILABLE_KEY
 *
 */
static MAC_Retval_t KeyDescriptorLookup(uint8_t *lookupData,
		uint8_t lookupDataSize,
		MAC_KeyTable_t **keyDesc)
{
    MAC_Retval_t retStatus = MAC_SUCCESS;
	/* Get key from KeyDescriptor as 7.5.8.2.5 */
	for (uint8_t i = 0; i < macSecPib.KeyTableEntries; i++) {
		for (uint8_t k
					= 0;
				k <
				macSecPib.KeyTable[i].KeyIdLookupListEntries;
				k++) {
			if (macSecPib.KeyTable[i].KeyIdLookupList[k].
					LookupDataSize == lookupDataSize) {
				int16_t compare;
				uint8_t len = 5U;

				switch (lookupDataSize) {
				case FIVE_OCTET_LOOK_UP:
					len = 5;
					break;

				case NINE_OCTET_LOOK_UP:
					len = 9;
					break;

				default:
					retStatus = MAC_UNSUPPORTED_SECURITY;
                    break;
				}
				compare = (int16_t)memcmp(
						macSecPib.KeyTable[i].KeyIdLookupList[
							k].LookupData,
						lookupData,
						len);
				if (compare == 0x00) {
					*keyDesc = &(macSecPib.KeyTable[i]);
					return retStatus;
				}
			}
		}
	}
	/* No key has been found. * / */
	return MAC_UNAVAILABLE_KEY;
}

/** @brief  This function checks the particular device is blacklisted or not
 *
 *  If the FrameCounter element is equal to 0xffffffff,
 *  the procedure shall set the Blacklisted element of the
 *  KeyDeviceDescriptor. If the Blacklisted element of the KeyDeviceDescriptor
 *  is set to FALSE, or the procedure shall return with a failed status
 *
 *  @param device_lookup_data Incoming frame device lookup data
 *  @param device_lookup_data_size Incoming frame lookup data size
 *  @param MAC_DeviceDesc_t Incoming mac frame device descriptor
 *  @param key_device_desc key device descriptor
 *
 *  @return MAC_SUCCESS, FAILURE
 *
 */

static MAC_Retval_t BlacklistCheckingProcedure(uint8_t *deviceLookupData,
		uint8_t deviceLookupDataSize,
		MAC_KeyTable_t *keyDesc,
		MAC_DeviceDesc_t **deviceDesc,
		MAC_KeyDeviceDesc_t **keyDeviceDesc)
{
	for (uint8_t i = 0; i < keyDesc->KeyDeviceListEntries; i++) {
		/* 7.5.8.2.6 (a.1) */
		uint8_t handle
			= keyDesc->KeyDeviceList[i].DeviceDescriptorHandle;
		*keyDeviceDesc = &(keyDesc->KeyDeviceList[i]);
		*deviceDesc
			= &(macSecPib.DeviceTable[handle].DeviceDescriptor
				[0]);
		/* 7.5.8.2.6 (a.2) */
		if (keyDesc->KeyDeviceList[i].UniqueDevice == true) {
			if (keyDesc->KeyDeviceList[i].BlackListed == false) {
				return MAC_SUCCESS;
			} else {
				return FAILURE;
			}
		}
		/* 7.5.8.2.6 (a.3) */
		else {
			MAC_Retval_t status = DeviceDescriptorLookup(
					macSecPib.DeviceTable[handle].DeviceDescriptor[
						0],
					deviceLookupData,
					deviceLookupDataSize);
			if (status == MAC_SUCCESS &&
					keyDesc->KeyDeviceList[i].BlackListed
					== false) {
				return MAC_SUCCESS;
			}
		}
	}
	return FAILURE;
}

/** @brief device descriptor look up procedure for checking the incoming frame
 * info
 *
 *  This function validates the incoming frame info of PANID and Short Address
 * or Extended
 *  IEEE Address against with associated device info.
 *
 *  @param device_desc device descriptor of associated device
 *  @param device_lookup_data incoming frame device lookup data
 *  @param device_lookup_data_size device lookup data size
 *
 *  @return MAC_SUCCESS, FAILURE
 *
 */
static MAC_Retval_t DeviceDescriptorLookup(MAC_DeviceDesc_t deviceDesc,
		uint8_t *deviceLookupData,
		uint8_t deviceLookupDataSize)
{
	uint8_t lookupData[8];
	int16_t compare;

	if (deviceLookupDataSize == LOOK_UP_SIZE_4) {
		lookupData[1] = (uint8_t)deviceDesc.PANId; /* LSB */
		lookupData[0] = (uint8_t)(deviceDesc.PANId >> 8); /* MSB */
		/* 2-octet dest addr right-concatenated */
		lookupData[3] = (uint8_t)deviceDesc.ShortAddress; /* LSB */
		lookupData[2] = (uint8_t)(deviceDesc.ShortAddress >> 8); /*
		                                                            *
		                                                            *
		                                                            *MSB
		                                                            **/
		compare
			= (int16_t)memcmp(deviceLookupData, lookupData,
				LOOK_UP_SIZE_4);
		if (compare == 0x00) {
			return MAC_SUCCESS;
		}
	} else if (deviceLookupDataSize == LOOK_UP_SIZE_8) {
		(void)memcpy(&lookupData[0], ((uint8_t *)(&deviceDesc.ExtAddress)),
				LOOK_UP_SIZE_8);
		compare
			= (int16_t)memcmp(deviceLookupData, lookupData,
				LOOK_UP_SIZE_8);
		if (compare == 0x00) {
			return MAC_SUCCESS;
		}
	}
    else
    {
        // do nothing
    }

	return FAILURE;
}

/** @brief Build the Security MCPS Data Request frame from the mpdu data
 *
 *  This function will extract the MAC Frame into individual mac parameters
 *
 *  @param mpdr mac payload pointer used to identify the payload location
 *  @param mframe tal frame info used to get the mac frame info
 *
 *  @return MAC_SUCCESS
 *
 */
bool BuildSecMcpsDataFrame(MCPS_DataReq_t *mpdr, MAC_FrameInfo_t *mframe)
{
	uint16_t fcf;
	bool intraPan;
	uint8_t addrFieldLen = 0;

	uint8_t *mpduPtr = &(mframe->mpdu[1]);

	/* Extract the FCF */
	fcf = convert_byte_array_to_16_bit(mpduPtr);

	mpduPtr += (2 /*FCF*/ + 1 /*Seq.No*/);

	mpdr->SrcAddrMode =(uint8_t)((fcf >> FCF_SOURCE_ADDR_OFFSET) & FCF_ADDR_MASK);
	mpdr->DstAddrMode = (uint8_t)((fcf >> FCF_DEST_ADDR_OFFSET) & FCF_ADDR_MASK);
	intraPan = (bool)(fcf & FCF_PAN_ID_COMPRESSION);

	if (mpdr->DstAddrMode != 0U) {
		mpdr->DstPANId = convert_byte_array_to_16_bit(
				mpduPtr);
		mpduPtr += PAN_ID_LEN;
		addrFieldLen += PAN_ID_LEN;

		mpdr->DstAddr = 0;
		if (FCF_SHORT_ADDR == mpdr->DstAddrMode) {
			/*
			 * First initialize the complete long address with zero,
			 * since
			 * later only 16 bit are actually written.
			 */
			mpdr->DstAddr
				= convert_byte_array_to_16_bit(mpduPtr);
			mpduPtr += SHORT_ADDR_LEN;
			addrFieldLen += SHORT_ADDR_LEN;
		} else if (FCF_LONG_ADDR == mpdr->DstAddrMode) {
			mpdr->DstAddr
				= convert_byte_array_to_64_bit(mpduPtr);
			mpduPtr += EXT_ADDR_LEN;
			addrFieldLen += EXT_ADDR_LEN;
		}
        else
        {
            // do nothing
        }
	}

	if (mpdr->SrcAddrMode != 0U) {
		if (!intraPan) {
			/*
			 * Source PAN ID is present in the frame only if the
			 * intra-PAN bit
			 * is zero and srcAddrMode is non zero.
			 */
			/* = convert_byte_array_to_16_bit( */
			/* framePtr); */
			mpduPtr += PAN_ID_LEN;
			addrFieldLen += PAN_ID_LEN;
		} else {
			/*
			 * The received frame does not contain a source PAN ID,
			 * hence
			 * source PAN ID is updated with the destination PAN ID.
			 */
			/* srcPanid = mpdr->DstPANId; */
		}

		/* The source address is updated. */
		if (FCF_SHORT_ADDR == mpdr->SrcAddrMode) {
			/*
			 * First initialize the complete long address with zero,
			 * since
			 * later only 16 bit are actually written.
			 */

			/* = convert_byte_array_to_16_bit(mpduPtr); */
			mpduPtr += SHORT_ADDR_LEN;

			addrFieldLen += SHORT_ADDR_LEN;
		} else if (FCF_LONG_ADDR == mpdr->SrcAddrMode) {
			/* = convert_byte_array_to_64_bit(mpduPtr); */
			mpduPtr += EXT_ADDR_LEN;
			addrFieldLen += EXT_ADDR_LEN;
		}
        else
        {
            // do nothing
        }
	}

	if ((bool)(fcf & FCF_SECURITY_ENABLED)) {
		uint8_t keyidLen;

		/* Security Fields are available so extract the aux security
		 * header */
		mpdr->SecurityLevel = (*mpduPtr & SEC_CTRL_SEC_LVL_MASK);
		mpdr->KeyIdMode
			= ((*mpduPtr++ >>
				SEC_CTRL_KEY_ID_FIELD_POS) &
				SEC_CTRL_KEY_ID_MASK);

		(void)memcpy(mpduPtr, (uint8_t *)&macSecPib.FrameCounter, \
				FRAME_COUNTER_LEN);
		mpduPtr += FRAME_COUNTER_LEN;

		keyidLen = GetKeyIdFieldLen(mpdr->KeyIdMode);

		switch (keyidLen) {
		case KEY_ID_MODE_0:

			/** No Key source & Key Index will be available
			 * Key is determined implicitly from the originator and
			 * recipient(s) of the frame
			 */
			mpdr->KeyIndex = 0;
			mpdr->KeySource = NULL;
			break;

		case KEY_ID_MODE_1:

			/** Key is determined from the Key Index field in
			 * conjunction with macDefault-KeySource
			 */
			mpdr->KeySource = NULL;
			mpdr->KeyIndex = *mpduPtr;
			mpduPtr += keyidLen;
			break;

		case KEY_ID_MODE_2:

			/** Key is determined explicitly from the 4-octet Key
			 * Source field and the Key Index field.
			 */
			mpdr->KeySource = mpduPtr;
			mpduPtr += (keyidLen - 1U);
			mpdr->KeyIndex = *mpduPtr++;
			break;

		case KEY_ID_MODE_3:

			/** Key is determined explicitly from the 8-octet Key
			 * Source field and the Key Index field.
			 */
			mpdr->KeySource = mpduPtr;
			mpduPtr += (keyidLen - 1U);
			mpdr->KeyIndex = *mpduPtr++;
			break;

		default:
			mpdr->KeyIndex = 0;
			mpdr->KeySource = NULL;
			break;
		}
	}

	mpdr->msduHandle = mframe->msduHandle;

	mpdr->msduLength = mframe->mpdu[0] - (mframe->macPayload - \
			mframe->mpdu + 1);

	mpdr->msdu = mframe->macPayload;
	return (bool)MAC_SUCCESS;
}

/** @brief Incoming key usage policy for the frame particular frame type
 *
 *  Inputs to this procedure are KeyDescriptor,
 *  frame type, and command frame identifier. output from
 *  this procedure is a passed or failed status.
 *
 *  @param key_desc KeyDescriptor of mac key table
 *  @param frameType incoming frame type
 *  @param mac_cmd Incoming mac_cmd field identifier
 *
 *  @return MAC_SUCCESS, MAC_IMPROPER_KEY_TYPE
 *
 */
static MAC_Retval_t IncomingKeyUsagePolicy(MAC_KeyTable_t *keyDesc,
		uint8_t frameType,
		uint8_t macCmd)
{
	/* as per spec 7.5.8.2.9 (a.1) */
	if (FCF_FRAMETYPE_MAC_CMD != frameType) {
		for (uint8_t index = 0; index < keyDesc->KeyUsageListEntries;
				index++) {
			if (keyDesc->KeyUsageList[index].Frametype ==
					frameType) {
				return MAC_SUCCESS;
			}
		}
	} else if (FCF_FRAMETYPE_MAC_CMD == frameType) {
		for (uint8_t index = 0; index < keyDesc->KeyUsageListEntries;
				index++) {
			if ((keyDesc->KeyUsageList[index].Frametype ==
					frameType) &&
					(keyDesc->KeyUsageList[index].
					CommandFrameIdentifier == macCmd)) {
				return MAC_SUCCESS;
			}
		}
	}
    else
    {
        // do nothing
    }

	return MAC_IMPROPER_KEY_TYPE;
}

#endif  /* (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006) */

/* EOF */
