/*
 * Copyright (c) 2023, The OpenThread Authors.
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */


/*******************************************************************************
* Copyright (C) [2023], Microchip Technology Inc., and its subsidiaries. All rights reserved.
  
* The software and documentation is provided by Microchip and its contributors 
* "as is" and any express, implied or statutory warranties, including, but not 
* limited to, the implied warranties of merchantability, fitness for a particular 
* purpose and non-infringement of third party intellectual property rights are 
* disclaimed to the fullest extent permitted by law. In no event shall Microchip 
* or its contributors be liable for any direct, indirect, incidental, special,
* exemplary, or consequential damages (including, but not limited to, procurement 
* of substitute goods or services; loss of use, data, or profits; or business 
* interruption) however caused and on any theory of liability, whether in contract, 
* strict liability, or tort (including negligence or otherwise) arising in any way 
* out of the use of the software and documentation, even if advised of the 
* possibility of such damage.
* 
* Except as expressly permitted hereunder and subject to the applicable license terms 
* for any third-party software incorporated in the software and any applicable open 
* source software license terms, no license or other rights, whether express or 
* implied, are granted under any patent or other intellectual property rights of 
* Microchip or any third party.
 *******************************************************************************/


#include "aes_alt.h"
#include <mbedtls/aes.h>
#include "pic32cx-mbedtls-config.h"
#ifdef MBEDTLS_AES_ALT

#include <common/code_utils.hpp>
#include <string.h>

#include <wolfssl/wolfcrypt/port/pic32/crypt_wolfcryptcb.h>

#include "configuration.h"
#include <wolfcrypt/aes.h>

static struct Aes wcAes;

void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
    (void)ctx;
    
//    CRYPT_WCCB_Initialize();
}

void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
    (void)ctx;
}

int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    int retval = 0;
    uint8_t dummyIV[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    (void)dummyIV;
    (void) wcAes;

    switch (keybits)
    {
    case 128:
        wc_AesSetKey(&wcAes, key, AES_BLOCK_SIZE, dummyIV, AES_ENCRYPTION);
        break;

    default:
        retval = MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
        break;
    }

    return retval;
}

int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx, const unsigned char *key, unsigned int keybits)
{
    int retval = 0;
    uint8_t dummyIV[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    (void)dummyIV;

    switch (keybits)
    {
    case 128:
        wc_AesSetKey(&wcAes, key, AES_BLOCK_SIZE, dummyIV, AES_DECRYPTION);
        break;

    default:
        retval = MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
        break;
    }

    return retval;
}

/**
 * \brief          AES-ECB block encryption/decryption
 *
 * \param ctx      AES context
 * \param mode     MBEDTLS_AES_ENCRYPT or MBEDTLS_AES_DECRYPT
 * \param input    16-byte input block
 * \param output   16-byte output block
 *
 * \return         0 if successful
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx, int mode, const unsigned char input[16], unsigned char output[16])
{
    int retval = 0;
    
    if (mode == MBEDTLS_AES_DECRYPT)
    {
        wc_AesEcbDecrypt(&wcAes, output, input, AES_BLOCK_SIZE);
    }
    else if (mode == MBEDTLS_AES_ENCRYPT)
    {
        wc_AesEcbEncrypt(&wcAes, output, input, AES_BLOCK_SIZE);
    }
    
    (void)ctx;

    return retval;
}

int mbedtls_aes_self_test(int verbose)
{
    (void)verbose;

    /* 128-bit Key 2b7e151628aed2a6abf7158809cf4f3c */
    const uint8_t       key_128b[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                                  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    mbedtls_aes_context aes;
    int                 retval = 0;

    uint8_t input[16]   = {0};
    uint8_t output[16]  = {0};
    uint8_t decrypt[16] = {0};

    strcpy((char *)input, (const char *)"hw_aes_test");

    mbedtls_aes_init(&aes);

    retval = mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key_128b, 128);
    VerifyOrExit(retval != 0, retval = -1);

    retval = mbedtls_aes_setkey_dec(&aes, (const unsigned char *)key_128b, 128);
    VerifyOrExit(retval != 0, retval = -1);

    retval = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, input, output);
    VerifyOrExit(retval != 0, retval = -1);

    retval = mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, output, decrypt);
    VerifyOrExit(retval != 0, retval = -1);
    
    if (0 != memcmp(input, decrypt, sizeof(input)))
        while(1);

    mbedtls_aes_free(&aes);

exit:

    return retval;
}

#endif /* MBEDTLS_AES_ALT */
