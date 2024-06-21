/*******************************************************************************
  SAL Header
  
  File Name:
    stb_generic.h

  Summary:
    This file contains declarations for security tool box

  Description:
   The IEEE standard 802.15.4 MAC Layer defines the protocol and compatible
   interconnection for data communication devices using low data rate, low 
   power and low complexity, short-range radio frequency (RF) transmissions in a
   wireless personal area network (WPAN).

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
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
#ifndef STB_GENERIC_H
#define STB_GENERIC_H

#if defined(STB_ON_SAL)
// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************
#include "sal_generic.h"

// *****************************************************************************
// *****************************************************************************
// Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* STB CCM status values
 
   Summary:
    list of CCM status values.
 
   Description:
    None
   Remarks:
    None 
 */

typedef enum {
	/** CCM en/decryption OK */
	STB_CCM_OK = 0,
	/** Illegal parameter value in STB_CcmSecure() */
	STB_CCM_ILLPARM,
	/** No previous key init in STB_CcmSecure() */
	STB_CCM_KEYMISS,
	/** MIC error detected in STB_CcmSecure() */
	STB_CCM_MICERR,
    /** Failure for STB */
    STB_CCM_FAILURE
} STB_Ccm_t;

// *****************************************************************************
/* security levels
 
   Summary:
    Enumeration for security levels.
 
   Description:
    None
   Remarks:
    None 
 */
typedef enum {
	/** Security Level 0 */
	SECURITY_00_LEVEL = 0x00,
	/** Security Level 1 */
	SECURITY_01_LEVEL,
	/** Security Level 2 */
	SECURITY_02_LEVEL,
	/** Security Level 3 */
	SECURITY_03_LEVEL,
	/** Security Level 4 */
	SECURITY_04_LEVEL,
	/** Security Level 5 */
	SECURITY_05_LEVEL,
	/** Security Level 6 */
	SECURITY_06_LEVEL,
	/** Security Level 7 */
	SECURITY_07_LEVEL
} Security_Level_t;

// *****************************************************************************
// *****************************************************************************
// Macros for MIC Calculation and Security
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  
 
   Summary:
    This macro holds ENCRYPTION_NOT_REQD
   Description:
    None
   Remarks:
    None 
 */

#define ENCRYPTION_NOT_REQD     (0x00)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds ENCRYPTION_REQD
   Description:
    None
   Remarks:
    None 
 */

#define ENCRYPTION_REQD         (0x01)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds PLAINTEXT_FLAG
   Description:
    None
   Remarks:
    None 
 */

#define PLAINTEXT_FLAG          (0x01)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds LEN_FIELD
   Description:
    None
   Remarks:
    None 
 */

#define LEN_FIELD                       (0x01)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds ADATA
   Description:
    None
   Remarks:
    None 
 */

#define ADATA                           (0x40)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds LEN_MIC_00
   Description:
    None
   Remarks:
    None 
 */

#define LEN_MIC_00                      (0x00)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds LEN_MIC_32
   Description:
    None
   Remarks:
    None 
 */

#define LEN_MIC_32                      (0x04)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds LEN_MIC_64
   Description:
    None
   Remarks:
    None 
 */

#define LEN_MIC_64                      (0x08)

// *****************************************************************************
/* 
 
   Summary:
    This macro holds LEN_MIC_128
   Description:
    None
   Remarks:
    None 
 */

#define LEN_MIC_128                     (0x10)

/* === Externals ========================================================== */

// *****************************************************************************
// *****************************************************************************
// Prototypes
// *****************************************************************************
// *****************************************************************************

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
/*
  Function:
    void STB_Init(void)

  Summary:
    STB Initialization

  Description:
    This function initializes the STB.
 
  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None 
*/

void STB_Init(void);

// *****************************************************************************
/*
  Function:
    STB_Ccm_t STB_CcmSecure(uint8_t * buffer, uint8_t nonce[AES_BLOCKSIZE], uint8_t * key,
     uint8_t hdr_len, uint8_t pld_len, uint8_t sec_level, uint8_t aes_dir);

  Summary:
    Secure one block with CCM*

  Description:
    This functions secures one block with CCM* according to 802.15.4.
 
  Precondition:
    None

  Parameters:
    buffer Input      -  plaintext header and payload concatenated;
                         for encryption: MUST HAVE 'AES_BLOCKSIZE'
                         BYTES SPACE AT THE END FOR THE MIC!
                         Output: frame secured (with MIC at end)/unsecured
    nonce             -  The nonce: Initialization Vector (IV) as used in
                         cryptography; the ZigBee nonce (13 bytes long)
                         are the bytes 2...14 of this nonce
    key               - The key to be used; if NULL, use the current key
    hdr_len           - Length of plaintext header (will not be encrypted)
    pld_len           - Length of payload to be encrypted; if 0, then only MIC
                        authentication implies
    sec_level         - Security level according to IEEE 802.15.4,
                        - the value may be 0 ... 7;
                        - the two LSBs contain the MIC length in bytes (0, 4, 8 or 16);
                        - bit 2 indicates whether encryption applies or not
    aes_dir           - AES_DIR_ENCRYPT if secure, AES_DIR_DECRYPT if unsecure

  Returns:
    STB CCM Status

  Remarks:
    None 
*/

STB_Ccm_t STB_CcmSecure(uint8_t * buffer,
uint8_t nonce[AES_BLOCKSIZE],
uint8_t * key,
uint8_t hdr_len,
uint8_t pld_len,
uint8_t sec_level,
uint8_t aes_dir);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* #if defined(STB_ON_SAL) */

#endif /* STB_GENERIC_H */
/* EOF */
