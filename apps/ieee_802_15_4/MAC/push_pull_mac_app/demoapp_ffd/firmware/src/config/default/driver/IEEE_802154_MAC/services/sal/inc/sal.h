/*******************************************************************************
  SAL Header
  
  File Name:
    sal.h

  Summary:
    This file contains declarations for low-level security API

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

#ifndef SAL_H
#define SAL_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "sal_types.h"
#include "sal_generic.h"
#include "stdbool.h"
#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* 
 
   Summary:
    This macro defines AES direction as encryption.
 
   Description:
    None
   Remarks:
    None 
 */

#ifndef AES_DIR_ENCRYPT

#define AES_DIR_ENCRYPT              (0U)
#endif

// *****************************************************************************
/* 
 
   Summary:
    This macro defines AES direction as decryption.
 
   Description:
    None
   Remarks:
    None 
 */

#ifndef AES_DIR_DECRYPT

#define AES_DIR_DECRYPT              (1U)
#endif

// *****************************************************************************
/* 
 
   Summary:
    This macro defines AES mode as ECB.
 
   Description:
    None
   Remarks:
    None 
 */

#ifndef AES_MODE_ECB

#define AES_MODE_ECB                 (0)
#endif

// *****************************************************************************
/* 
 
   Summary:
    This macro defines AES mode as CBC.
 
   Description:
    None
   Remarks:
    None 
 */

#ifndef AES_MODE_CBC

#define AES_MODE_CBC                 (1)
#endif



// *****************************************************************************
/* 
 
   Summary:
    This macro This macro defines AES mode as CBC.
 
   Description:
    None
   Remarks:
    None 
 */

#define AES_MODE_CBC   (1)

// *****************************************************************************
/* 
 
   Summary:
    This macro This macro defines AES mode as ECB.
 
   Description:
    None
   Remarks:
    None 
 */

#define AES_MODE_ECB   (0)

// *****************************************************************************
/* 
 
   Summary:
    This macro This macro defines AES mode as CTR.
 
   Description:
    None
   Remarks:
    None 
 */

#define AES_MODE_CTR    (2)

// *****************************************************************************
/* 
 
   Summary:
    This macro This macro defines Nonce size.
 
   Description:
    None
   Remarks:
    None 
 */

#define NONCE_SIZE              (13)



// *****************************************************************************
// *****************************************************************************
// Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* AES status values
 
   Summary:
    Enumeration of AES status values
 
   Description:
    None
   Remarks:
    None 
 */

typedef enum {
	/** AES CCM en/decryption OK */
	AES_SUCCESS = 0,
	/** AES failure */
	AES_FAILURE,

} SAL_AesStatus_t;
/* === Externals ========================================================== */

// *****************************************************************************
// *****************************************************************************
// Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void SAL_Init(void)

  Summary:
    Initialization of SAL.

  Description:
    This functions initializes the SAL.
 
  Precondition:
    None

  Parameters:
    None
                      
  Returns:
    None

  Remarks:
    None 
*/

void SAL_Init(void);

// *****************************************************************************
/*
  Function:
    SAL_AesStatus_t SAL_AesSetKey(uint8_t *key, uint8_t key_len)

  Summary:
    aes set key for SAL.

  Description:
    This function set the aes set key and key length for SAL.
 
  Precondition:
    None

  Parameters:
    key     -  The key to be used; if NULL, use the current key
    key_len -  Length of key
                      
  Returns:
    returns AES_SUCCESS if key intialization successfully  otherwise AES_FAILURE

  Remarks:
    None 
*/

SAL_AesStatus_t SAL_AesSetKey(uint8_t *key, uint8_t key_len);

// *****************************************************************************
/*
  Function:
    SAL_AesStatus_t SAL_AesCcmSecure(uint8_t *buffer, uint8_t nonce[AES_BLOCKSIZE],
		uint8_t hdr_len, uint8_t pld_len, uint8_t aes_dir, uint8_t mic_len, uint8_t enc_flag)

  Summary:
    aes ccm secure for SAL.

  Description:
    This function for aes secure with CCM.
 
  Precondition:
    None

  Parameters:
    buffer     -  Input data
    nonce      -  The nonce: Initialization Vector (IV) as used in
                  cryptography
    key        -  The key to be used; if NULL, use the current key
    hdr_len    -  Length of plaintext header (will not be encrypted)
    pld_len    -  Length of payload to be encrypted; if 0, then only MIC
                 authentication implies
    aes_dir    -  AES_DIR_ENCRYPT if secure, AES_DIR_DECRYPT if unsecure
                      
  Returns:
    returns AES_SUCCESS if AES encryption and decryption successfully otherwise AES_FAILURE

  Remarks:
    None 
*/

SAL_AesStatus_t SAL_AesCcmSecure(uint8_t *buffer,
		uint8_t nonce[AES_BLOCKSIZE],
		uint8_t hdr_len,
		uint8_t pld_len,
		uint8_t aes_dir, uint8_t mic_len, uint8_t enc_flag);


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* SAL_H */
/* EOF */
