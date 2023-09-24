/*******************************************************************************
  SAL Source

  File Name:
    sal.c
  Summary:
    Low-level crypto API for an AES unit
  Description:
    This file contains Low-level crypto API for an AES unit.

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
#include "sal.h"
#include "definitions.h"
#include "wolfssl/wolfcrypt/port/pic32/crypt_wolfcryptcb.h"
#include "configuration.h"
#include "wolfcrypt/aes.h"

/* === Macros ============================================================== */

/* === Types =============================================================== */

/* === Globals ============================================================= */

/* aes structure in which to store the supplied key */
static struct Aes aes;

/* === Implementation ====================================================== */

/**
 * @brief Initialization of SAL.
 *
 * This functions initializes the SAL.
 */
void SAL_Init(void)
{
	int status = 0;
	(void)status;
}

/**
 * @brief Setup AES Key
 *
 * This function perform key initialization
 *
 * @param[in] key AES key or NULL (NULL: use last key)
 * @param[in] key_len  length of key
 *
 */
SAL_AesStatus_t SAL_AesSetKey(uint8_t *key, uint8_t key_len)
{
    int status = 0;
    SAL_AesStatus_t aesStatus = AES_FAILURE;
    
	if (key != NULL && key_len > 0U) {
		/* Setup key. */
		status = wc_AesCcmSetKey(&aes, key, key_len);
    }
    
    if(status == 0)
    {
        aesStatus = AES_SUCCESS;
    }

	return aesStatus;
}


/**
 * @brief aes ccm secure for SAL.
 *
 * This function for aes secure with CCM.
 *
 * @param[in]  buffer Input data
 * @param[in]  nonce   The nonce: Initialization Vector (IV) as used in
 *                     cryptography
 * @param[in] key The key to be used; if NULL, use the current key
 * @param[in] hdr_len Length of plaintext header (will not be encrypted)
 * @param[in] pld_len Length of payload to be encrypted; if 0, then only MIC
 *                    authentication implies
 * @param[in] aes_dir AES_DIR_ENCRYPT if secure, AES_DIR_DECRYPT if unsecure
 */

SAL_AesStatus_t SAL_AesCcmSecure(uint8_t *buffer,
		uint8_t nonce[AES_BLOCKSIZE],
		uint8_t hdr_len,
		uint8_t pld_len,
		uint8_t aes_dir, uint8_t mic_len, uint8_t enc_flag)
{ 
    uint8_t *plain_text = NULL;
    uint8_t plainTextLen = 0U;
    uint8_t *mic = buffer + hdr_len + pld_len;
    uint8_t *authData = buffer;
    int status = 0;
    SAL_AesStatus_t aesCcmStatus = AES_FAILURE;
    
    if(enc_flag > 0U)
    {
        plain_text = buffer + hdr_len;
        plainTextLen = pld_len;
    }

    if(aes_dir == AES_DIR_ENCRYPT)
    {
        status = wc_AesCcmEncrypt(&aes, plain_text, plain_text, (uint32_t)plainTextLen,
                       nonce, NONCE_SIZE,
                       mic,(uint32_t)mic_len,
                       authData, (uint32_t)hdr_len); 
    }else{
        
        status = wc_AesCcmDecrypt(&aes, plain_text, plain_text, (uint32_t)plainTextLen,
                   nonce, NONCE_SIZE,
                   mic,(uint32_t)mic_len,
                   authData, (uint32_t)hdr_len);
    }
    if(status == 0)
    {
       aesCcmStatus = AES_SUCCESS; 
    }
    
    return aesCcmStatus;    
}

/* EOF */
