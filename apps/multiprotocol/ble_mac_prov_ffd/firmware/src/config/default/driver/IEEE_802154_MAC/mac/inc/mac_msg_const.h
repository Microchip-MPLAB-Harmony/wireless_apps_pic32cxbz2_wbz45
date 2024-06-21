/*******************************************************************************
  MAC Msg Constants
  
  File Name:
    mac_msg_const.h

  Summary:
    all MAC message constants

  Description:
   This file defines all message constants.

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

#ifndef MAC_MSG_CONST_H
#define MAC_MSG_CONST_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "mac_config.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Data types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* MAC
 
   Summary:
    This type contains the service primitives of the PHY-, MAC- and Network-layer
    as well the command-interface primitives
   Description:
    None
   Remarks:
    None 
 */
        
enum msg_code {
	PHY_DATA_INDICATION                 = (0x00),

	MLME_ASSOCIATE_REQUEST              = (0x01),
	MLME_ASSOCIATE_RESPONSE             = (0x02),

	MCPS_DATA_REQUEST                   = (0x03),
	MCPS_PURGE_REQUEST                  = (0x04),

	MLME_DISASSOCIATE_REQUEST           = (0x05),
	MLME_SET_REQUEST                    = (0x06),
	MLME_ORPHAN_RESPONSE                = (0x07),
	MLME_GET_REQUEST                    = (0x08),
	MLME_RESET_REQUEST                  = (0x09),
	MLME_RX_ENABLE_REQUEST              = (0x0A),
	MLME_SCAN_REQUEST                   = (0x0B),
	MLME_GTS_REQUEST                    = (0x0C),
	MLME_START_REQUEST                  = (0x0D),
	MLME_POLL_REQUEST                   = (0x0E),
	MLME_SYNC_REQUEST                   = (0x0F),

	MCPS_DATA_CONFIRM                   = (0x10),
	MCPS_DATA_INDICATION                = (0x11),
	MCPS_PURGE_CONFIRM                  = (0x12),

	MLME_ASSOCIATE_INDICATION           = (0x13),
	MLME_ASSOCIATE_CONFIRM              = (0x14),
	MLME_DISASSOCIATE_INDICATION        = (0x15),
	MLME_DISASSOCIATE_CONFIRM           = (0x16),
	MLME_BEACON_NOTIFY_INDICATION       = (0x17),
	MLME_GTS_CONFIRM                    = (0x18),
	MLME_GTS_INDICATION                 = (0x19),
	MLME_ORPHAN_INDICATION              = (0x1A),
	MLME_SCAN_CONFIRM                   = (0x1B),
	MLME_COMM_STATUS_INDICATION         = (0x1C),
	MLME_SYNC_LOSS_INDICATION           = (0x1D),
	MLME_GET_CONFIRM                    = (0x1E),
	MLME_SET_CONFIRM                    = (0x1F),
	MLME_RESET_CONFIRM                  = (0x20),
	MLME_RX_ENABLE_CONFIRM              = (0x21),
	MLME_START_CONFIRM                  = (0x22),
	MLME_POLL_CONFIRM                   = (0x23),
	MAC_INC_FRAME                       = (0x24)
};

// *****************************************************************************
// *****************************************************************************
// Macros
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* LAST_MESSAGE
 
   Summary:
    Bump this when extending the list
   Description:
    None
   Remarks:
    None 
 */
#define LAST_MESSAGE                    MLME_POLL_CONFIRM

/** The following defines describe the minimum length of a primitive message. */
#ifdef ENABLE_TSTAMP
// *****************************************************************************
/* MCPS_DATA_CON_LEN & MCPS_DATA_CON_LEN
 
   Summary:
    the minimum length of a MCPS data confirm & indication primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MCPS_DATA_CON_LEN               (7)
#define MCPS_DATA_IND_LEN               (31)
#else   /* ENABLE_TSTAMP not enabled */
#define MCPS_DATA_CON_LEN               (7 - 4)
#define MCPS_DATA_IND_LEN               (31 - 4)
#endif  /* ENABLE_TSTAMP */

// *****************************************************************************
/* MCPS_PURGE_CONF_LEN
 
   Summary:
    the minimum length of a MCPS purge confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MCPS_PURGE_CONF_LEN             (3)

// *****************************************************************************
/* MLME_ASSOCIATE_CONF_LEN
 
   Summary:
    the minimum length of a MCPS Associate confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_ASSOCIATE_CONF_LEN         (4)

// *****************************************************************************
/* MLME_ASSOCIATE_IND_LEN
 
   Summary:
    the minimum length of a MCPS Associate indication primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_ASSOCIATE_IND_LEN          (10)

// *****************************************************************************
/* MLME_BEACON_NOTIFY_IND_LEN
 
   Summary:
    the minimum length of a MCPS beacon notify indication primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_BEACON_NOTIFY_IND_LEN      (2)

// *****************************************************************************
/* MLME_COMM_STATUS_IND_LEN
 
   Summary:
    the minimum length of a MLME comm status indication primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_COMM_STATUS_IND_LEN        (22)

// *****************************************************************************
/* MLME_DISASSOCIATE_CONF_LEN
 
   Summary:
    the minimum length of a MLME disassociate confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_DISASSOCIATE_CONF_LEN      (13)

// *****************************************************************************
/* MLME_DISASSOCIATE_IND_LEN
 
   Summary:
    the minimum length of a MLME disassociate indication primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_DISASSOCIATE_IND_LEN       (10)

// *****************************************************************************
/* MLME_GET_CONF_LEN
 
   Summary:
    the minimum length of a MLME get confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
#define MLME_GET_CONF_LEN               (5)
#else
#define MLME_GET_CONF_LEN               (4)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

// *****************************************************************************
/* MLME_ORPHAN_IND_LEN
 
   Summary:
    the minimum length of a MLME orphan indication primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_ORPHAN_IND_LEN             (9)

// *****************************************************************************
/* MLME_POLL_CONF_LEN
 
   Summary:
    the minimum length of a MLME poll confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_POLL_CONF_LEN              (2)

// *****************************************************************************
/* MLME_RESET_CONF_LEN
 
   Summary:
    the minimum length of a MLME reset confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_RESET_CONF_LEN             (2)

// *****************************************************************************
/* MLME_RX_ENABLE_CONF_LEN
 
   Summary:
    the minimum length of a MLME RX enable confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_RX_ENABLE_CONF_LEN         (2)

// *****************************************************************************
/* MLME_SCAN_CONF_LEN
 
   Summary:
    the minimum length of a MLME scan confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_SCAN_CONF_LEN              (10)

// *****************************************************************************
/* MLME_SET_CONF_LEN
 
   Summary:
    the minimum length of a MLME set confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#if ((defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006))
#define MLME_SET_CONF_LEN               (4)
#else
#define MLME_SET_CONF_LEN               (3)
#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

// *****************************************************************************
/* MLME_START_CONF_LEN
 
   Summary:
    the minimum length of a MLME start confirm primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_START_CONF_LEN             (2)

// *****************************************************************************
/* MLME_SYNC_LOSS_IND_LEN
 
   Summary:
    the minimum length of a MLME sync loss indication primitive message
   Description:
    None
   Remarks:
    None 
 */
#define MLME_SYNC_LOSS_IND_LEN          (6)

// *****************************************************************************
/* WPAN_DESCRIPTOR_LEN
 
   Summary:
    the minimum length of a WPAN Descriptor primitive message
   Description:
    None
   Remarks:
    None 
 */
#ifdef ENABLE_TSTAMP
#define WPAN_DESCRIPTOR_LEN             (21)
#else   /* ENABLE_TSTAMP not enabled */
#define WPAN_DESCRIPTOR_LEN             (21 - 4)
#endif  /* ENABLE_TSTAMP */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* MAC_MSG_CONST_H */
/* EOF */
