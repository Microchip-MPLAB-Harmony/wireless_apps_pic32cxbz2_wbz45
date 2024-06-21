/*******************************************************************************
  STB Source

  File Name:
    stb.c
  Summary:
    High-level security tool box
  Description:
    This file contains High-level security tool box.

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
* INCIDENTAL OR CONSEQUENTIALHigh-level security tool box LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
//DOM-IGNORE-END

#ifdef STB_ON_SAL

/* === Includes ============================================================ */

#include <string.h>
#include "pal.h"
#include "ieee_mac_const.h"
#include "stb.h"
#include "sal.h"

/* === Macros ============================================================== */
#define NO_HDR_MIC             (0x00)
#define NONCE_INDEX1             (1U)
/* === Types =============================================================== */

/* === Globals ============================================================= */

/* True indicates if key has to be setup. */
static bool key_change = true;
static uint8_t last_key[AES_BLOCKSIZE];
static bool stb_restart_required = false;
/* === Implementation ====================================================== */

/**
 * @brief STB Initialization
 *
 * This function initializes the STB.
 */
void STB_Init(void)
{
	SAL_Init();
}

/**
 * @brief STB Restart
 *
 * This function re-starts the STB after power down.
 *
 * @ingroup apiStbApi
 */
void STB_Restart(void)
{
	/*
	 * Re-use key_change flag indicating that the AES engine has been power-
	 * cycled and needs to be restarted.
	 */
	stb_restart_required = true;
}

/**
 * @brief Secure one block with CCM*
 *
 * This functions secures one block with CCM* according to 802.15.4.
 *
 * @param[in,out] buffer Input: plaintext header and payload concatenated;
 *                       for encryption: MUST HAVE 'AES_BLOCKSIZE'
 *                       BYTES SPACE AT THE END FOR THE MIC!
 *                       Output: frame secured (with MIC at end)/unsecured
 * @param[in]  nonce   The nonce: Initialization Vector (IV) as used in
 *                     cryptography; the ZigBee nonce (13 bytes long)
 *                     are the bytes 2...14 of this nonce
 * @param[in] key The key to be used; if NULL, use the current key
 * @param[in] hdr_len Length of plain text header (will not be encrypted)
 * @param[in] pld_len Length of payload to be encrypted; if 0, then only MIC
 *                    authentication implies
 * @param[in] sec_level Security level according to IEEE 802.15.4,
 *                    7.6.2.2.1, Table 95:
 *                    - the value may be 0 ... 7;
 *                    - the two LSBs contain the MIC length in bytes
 *                      (0, 4, 8 or 16);
 *                    - bit 2 indicates whether encryption applies or not
 * @param[in] aes_dir AES_DIR_ENCRYPT if secure, AES_DIR_DECRYPT if unsecured
 *
 * @return STB CCM Status
 */
STB_Ccm_t STB_CcmSecure(uint8_t *buffer,
		uint8_t nonce[AES_BLOCKSIZE],
		uint8_t *key,
		uint8_t hdr_len,
		uint8_t pld_len,
		uint8_t sec_level,
		uint8_t aes_dir)
{
	uint8_t mic_len = 0;
	uint8_t enc_flag = ENCRYPTION_NOT_REQD;
    SAL_AesStatus_t aesStatus = AES_FAILURE;
    STB_Ccm_t stbCcmStatus = STB_CCM_FAILURE;

	switch ((Security_Level_t)sec_level) {
	case SECURITY_00_LEVEL:
		/* No MIC & No Encryption at Security Level -0 */
		mic_len = LEN_MIC_00;
		break;

	case SECURITY_01_LEVEL:
		/* MIC-32 & No Encryption at Security Level -1 */
		mic_len = LEN_MIC_32;
		break;

	case SECURITY_02_LEVEL:
		/* MIC-64 & No Encryption at Security Level -2 */
		mic_len = LEN_MIC_64;
		break;

	case SECURITY_03_LEVEL:
		/* MIC-128 & No Encryption at Security Level -3 */
		mic_len = LEN_MIC_128;
		break;

	case SECURITY_04_LEVEL:
		/* No MIC & Encryption at Security Level -4 */
		mic_len = LEN_MIC_00;
		enc_flag = ENCRYPTION_REQD;
		break;

	case SECURITY_05_LEVEL:
		/* MIC-32 & Encryption at Security Level -5 */
		mic_len = LEN_MIC_32;
		enc_flag = ENCRYPTION_REQD;
		break;

	case SECURITY_06_LEVEL:
		/* MIC-64 & Encryption at Security Level -6 */
		mic_len = LEN_MIC_64;
		enc_flag = ENCRYPTION_REQD;
		break;

	case SECURITY_07_LEVEL:
		/* MIC-128 & Encryption at Security Level -7 */
		mic_len = LEN_MIC_128;
		enc_flag = ENCRYPTION_REQD;
		break;

	default:
        mic_len = (uint8_t)STB_CCM_MICERR;
        enc_flag = ENCRYPTION_NOT_REQD;
		break;
	}

	/* Test on correct parameters. */

	if (((sec_level & ~0x7U) == 0xf8U) ||
			(buffer == NULL) ||
			(nonce == NULL) ||
			((uint16_t)pld_len + (uint16_t)hdr_len +
			(uint16_t)mic_len > aMaxPHYPacketSize)
			) {
		return (STB_CCM_ILLPARM);
	}

	if (key_change && (key == NULL)) {

		/* Initial call, but no key given. */
		return (STB_CCM_KEYMISS);
	}

	/* Setup key if necessary. */

	if (!key_change && key != NULL) { /* There was some previous key. */
		uint8_t i;

		/* Test on changed key. */
		for (i = AES_BLOCKSIZE; i--; /* */) {
			key_change |= (last_key[i] ^ key[i]);
		}
	}

	if (key_change) {
		/*
		 * Key must be non-NULL because of test above, and
		 * ECB encryption is always the initial encryption mode.
		 */
		aesStatus = SAL_AesSetKey(key, AES_KEYSIZE);
        if(aesStatus != AES_SUCCESS)
        {
            return(STB_CCM_FAILURE);
        }
		(void)memcpy(last_key, key, AES_KEYSIZE);
		key_change = false;
	}
   
    aesStatus = SAL_AesCcmSecure(buffer, &nonce[NONCE_INDEX1], hdr_len, pld_len, aes_dir, mic_len, enc_flag);
    
    if(aesStatus == AES_SUCCESS)
    {
        stbCcmStatus = STB_CCM_OK;
    }

	return stbCcmStatus;
}

#endif /* #ifdef STB_ON_SAL */

/* EOF */
