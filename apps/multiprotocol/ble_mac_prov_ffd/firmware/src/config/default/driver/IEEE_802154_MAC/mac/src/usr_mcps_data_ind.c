/*******************************************************************************
  MAC User Call back Source

  File Name:
    usr_mcps_data_ind.c
  Summary:
    MCPS-DATA.indication Call back
  Description:
    This file contains user call back function for MCPS-DATA.indication.

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

/* === Includes ============================================================= */

#include <stdint.h>
#include <stdbool.h>
#include "mac_api.h"

/* === Macros ============================================================== */

/* === Globals ============================================================= */

/* === Prototypes ========================================================== */

/* === Implementation ====================================================== */
void __attribute__((weak))
#ifdef MAC_SECURITY_ZIP
USR_MCPS_DataInd(WPAN_AddrSpec_t *SrcAddrSpec,
		WPAN_AddrSpec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
		uint8_t DSN,
    #ifdef ENABLE_TSTAMP
		uint32_t Timestamp,
    #endif  /* ENABLE_TSTAMP */
		uint8_t SecurityLevel,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else   /* No MAC_SECURITY */
USR_MCPS_DataInd(WPAN_AddrSpec_t * SrcAddrSpec,
		WPAN_AddrSpec_t * DstAddrSpec,
		uint8_t msduLength,
		uint8_t * msdu,
		uint8_t mpduLinkQuality,
    #ifdef ENABLE_TSTAMP
		uint8_t DSN,
		uint32_t Timestamp)
    #else
		uint8_t DSN)
    #endif  /* ENABLE_TSTAMP */
#endif  /* MAC_SECURITY */
{
	/* Keep compiler happy. */
	SrcAddrSpec = SrcAddrSpec;
	DstAddrSpec = DstAddrSpec;
	msduLength = msduLength;
	msdu = msdu;
	mpduLinkQuality = mpduLinkQuality;
	DSN = DSN;
#ifdef ENABLE_TSTAMP
	Timestamp = Timestamp;
#endif  /* ENABLE_TSTAMP */
#ifdef MAC_SECURITY_ZIP
	SecurityLevel = SecurityLevel;
	KeyIdMode = KeyIdMode;
	KeyIndex = KeyIndex;
#endif  /* MAC_SECURITY */
}

/* EOF */
