/*******************************************************************************
  SAL Header
  
  File Name:
    sal_generic.h

  Summary:
    This file contains macros for low-level security API

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

#ifndef SAL_GENERIC_H
#define SAL_GENERIC_H

/* === Includes =========================================================== */

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* 

   Summary:
    This macro holds Size of AES blocks
   Description:
	None
   Remarks:
    None 
 */

#define AES_BLOCKSIZE               (16U)

// *****************************************************************************
/* 

   Summary:
    This macro holds Size of AES key
   Description:
	None
   Remarks:
    None 
 */

#define AES_KEYSIZE                 (16U)

/* === Types ============================================================== */

/* === Externals ========================================================== */

/* === Prototypes ========================================================= */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* SAL_GENERIC_H */
/* EOF */
