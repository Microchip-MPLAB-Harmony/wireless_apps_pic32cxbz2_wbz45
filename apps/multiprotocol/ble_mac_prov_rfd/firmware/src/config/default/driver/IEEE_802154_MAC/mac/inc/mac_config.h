/*******************************************************************************
  MAC Header
  
  File Name:
    mac_config.h

  Summary:
    This file contains all timer IDs and Standard MAC configurations which are used in MAC

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

#ifndef MAC_CONFIG_H
#define MAC_CONFIG_H

/* === Includes ============================================================= */

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
/* Standard MAC configurations
 
   Summary:
    This macro defines Standard MAC configurations according to IEEE 802.15.4: FFD vs. RFD build
   Description:
    None
   Remarks:
    None 
 */

#ifdef FFD

/* FEATURE SET FOR FFD BUILD */
#define MAC_ASSOCIATION_INDICATION_RESPONSE     (1)
#define MAC_ASSOCIATION_REQUEST_CONFIRM         (1)
#define MAC_BEACON_NOTIFY_INDICATION            (1)
#define MAC_DISASSOCIATION_BASIC_SUPPORT        (1)
#define MAC_DISASSOCIATION_FFD_SUPPORT          (1)
#define MAC_GET_SUPPORT                         (1)
#define MAC_INDIRECT_DATA_BASIC                 (1)
#define MAC_INDIRECT_DATA_FFD                   (1)
#define MAC_ORPHAN_INDICATION_RESPONSE          (1)
#define MAC_PAN_ID_CONFLICT_AS_PC               (1)
#define MAC_PAN_ID_CONFLICT_NON_PC              (1)
#define MAC_PURGE_REQUEST_CONFIRM               (1)
#define MAC_RX_ENABLE_SUPPORT                   (1)
#define MAC_SCAN_ACTIVE_REQUEST_CONFIRM         (1)
#define MAC_SCAN_ED_REQUEST_CONFIRM             (1)
#define MAC_SCAN_ORPHAN_REQUEST_CONFIRM         (1)
#define MAC_SCAN_PASSIVE_REQUEST_CONFIRM        (0) /* No Passive Scan in
	                                             * nonbeacon networks */
#define MAC_START_REQUEST_CONFIRM               (1)
#else

/**
 * FEATURE SET FOR RFD BUILD
 */
#define MAC_ASSOCIATION_INDICATION_RESPONSE     (0) /* No acceptance of
	                                             * association FROM other
	                                             * nodes */
#define MAC_ASSOCIATION_REQUEST_CONFIRM         (1)
#define MAC_BEACON_NOTIFY_INDICATION            (1)
#define MAC_DISASSOCIATION_BASIC_SUPPORT        (1)
#define MAC_DISASSOCIATION_FFD_SUPPORT          (0) /* No indirect tx of
	                                             * disassocation
	                                             * notification frames */
#define MAC_GET_SUPPORT                         (1)
#define MAC_INDIRECT_DATA_BASIC                 (1)
#define MAC_INDIRECT_DATA_FFD                   (0) /* No Transmission of
	                                             * indirect data */
#define MAC_ORPHAN_INDICATION_RESPONSE          (0) /* No Handling of Orphan
	                                             * Notification */
#define MAC_PAN_ID_CONFLICT_AS_PC               (0) /* No PAN-Id conflict
	                                             * detection as PAN
	                                             * Coordinator */
#define MAC_PAN_ID_CONFLICT_NON_PC              (1)
#define MAC_PURGE_REQUEST_CONFIRM               (0) /* No Purging of indirect
	                                             * data */
#define MAC_RX_ENABLE_SUPPORT                   (1)
#define MAC_SCAN_ACTIVE_REQUEST_CONFIRM         (1)
#define MAC_SCAN_ED_REQUEST_CONFIRM             (0) /* No Energy Detect Scan */
#define MAC_SCAN_ORPHAN_REQUEST_CONFIRM         (1)
#define MAC_SCAN_PASSIVE_REQUEST_CONFIRM        (1)
#define MAC_START_REQUEST_CONFIRM               (0) /* No Start or realignment
	                                             * of networks */
#endif  /* FFD / RFD */


/** FEATURE SET FOR BUILD WITH PURE NONBEACON ONLY NETWORK SUPPORT */
#define MAC_SYNC_REQUEST                        (0) /* No Tracking of beacon
	                                             * frames for nonbeacon
	                                             * networks */
/**
 * FEATURE SET FOR BUILD WITH PURE NONBEACON ONLY NETWORK SUPPORT
 */
#define MAC_GTS_REQUEST                        (0)


/* Sync Loss Indication primitive is always required:
   In Beacon- and Nonbeacon-enabled networks this is required in
   case the peer node, acting as coordinator has MAC_START_REQUEST_CONFIRM
   set to 1. */
#define MAC_SYNC_LOSS_INDICATION                    (1)

/* The following defines are general and thus are also valid if a
   user_build_config.h file was available. */

/* Communication Status Indication primitive is only required for an FDD
   handling also Association Response or Orphan Response Primitive. */
#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || \
	(MAC_ASSOCIATION_INDICATION_RESPONSE == 1))
#define MAC_COMM_STATUS_INDICATION              (1)
#else
#define MAC_COMM_STATUS_INDICATION              (0)
#endif

/* In case any of the scan features is required, the general portions
   supporting scanning have to be enabled. */
#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
#define MAC_SCAN_SUPPORT                        (1)
#else
#define MAC_SCAN_SUPPORT                        (0)
#endif

/* Some sanity checks are done here to prevent having configurations
   that might violate the standard or lead to undefined behaviour
   (e.g. by having unreasonable combinations in file user_build_config.h). */

/* 1) If Association Response is used, also Indirect Data Tx Support is
 * required. */
#if ((MAC_ASSOCIATION_INDICATION_RESPONSE == 1) && (MAC_INDIRECT_DATA_FFD == 0))
#error  ("MAC_INDIRECT_DATA_FFD needs to be set to 1 in	\
	user_build_config.h since MAC_ASSOCIATION_INDICATION_RESPONSE is 1")
#endif

/* 2) If Association Request is used, also Indirect Data Rx Support is required. */
#if ((MAC_ASSOCIATION_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_ASSOCIATION_REQUEST_CONFIRM is 1")
#endif

/* 3) If Disassociation is used as basic feature, also Indirect Data Rx Support
 *    is required. */
#if ((MAC_DISASSOCIATION_BASIC_SUPPORT == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_DISASSOCIATION_BASIC_SUPPORT is 1")
#endif

/* 4) If Disassociation is used as FFD (allow tx of indirect disassociation
 * frames),also Indirect Data Rx Support is required. */
#if ((MAC_DISASSOCIATION_FFD_SUPPORT == 1) && (MAC_INDIRECT_DATA_FFD == 0))
#error  ("MAC_INDIRECT_DATA_FFD needs to be set to 1 in	\
	user_build_config.h since MAC_DISASSOCIATION_FFD_SUPPORT is 1")
#endif

/* 5) If Purging is used, also Indirect Data Tx Support is required. */
#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_FFD == 0))
#error  ("MAC_INDIRECT_DATA_FFD needs to be set to 1 in	\
	user_build_config.h since MAC_PURGE_REQUEST_CONFIRM is 1")
#endif

/*6) If Indirect Data Tx Support is used, also Indirect Data Rx Support is
 * required. */
#if ((MAC_INDIRECT_DATA_FFD == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_INDIRECT_DATA_FFD is 1")
#endif

/* 7) If Disassociation is used as FFD (allow tx of indirect disassociation
 * frames), also Disassociation as basic feature is required. */
#if ((MAC_DISASSOCIATION_FFD_SUPPORT == 1) && (MAC_INDIRECT_DATA_BASIC == 0))
#error  ("MAC_INDIRECT_DATA_BASIC needs to be set to 1 in \
	user_build_config.h since MAC_DISASSOCIATION_FFD_SUPPORT is 1")
#endif

/* 8) If PAN-Id conflict detection on the PAN Coordinator side is used,
     also Sync Loss Indication is required. */
#if ((MAC_PAN_ID_CONFLICT_AS_PC == 1) && (MAC_SYNC_LOSS_INDICATION == 0))
#error  ("MAC_SYNC_LOSS_INDICATION needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_AS_PC is 1")
#endif

/* 9) If PAN-Id conflict detection on the PAN Coordinator side is used,
    also Start Request is required. */
#if ((MAC_PAN_ID_CONFLICT_AS_PC == 1) && (MAC_START_REQUEST_CONFIRM == 0))
#error  ("MAC_START_REQUEST_CONFIRM needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_AS_PC is 1")
#endif

/* 10) If PAN-Id conflict detection not as PAN Coordinator is used,
      also Sync Loss Indication is required. */
#if ((MAC_PAN_ID_CONFLICT_NON_PC == 1) && (MAC_SYNC_LOSS_INDICATION == 0))
#error  ("MAC_SYNC_LOSS_INDICATION needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_NON_PC is 1")
#endif

/* 11) If PAN-Id conflict detection not as PAN Coordinator is used,
    either Association Request or Sync Request is required. */
#if ((MAC_PAN_ID_CONFLICT_NON_PC == 1) && \
	((MAC_ASSOCIATION_REQUEST_CONFIRM == 0)) \
)
#error \
	("MAC_ASSOCIATION_REQUEST_CONFIRM needs to be set to 1 in \
	user_build_config.h since MAC_PAN_ID_CONFLICT_NON_PC is 1")
#endif

// *****************************************************************************
/*  NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS
 
   Summary:
    Number of timers required to support indirect data transmission
   Description:
    None
   Remarks:
    None 
 */

#if (MAC_INDIRECT_DATA_BASIC == 1)
    #if (MAC_INDIRECT_DATA_FFD == 1)
	#define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (2)
    #else
	#define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (1)
    #endif /* MAC_INDIRECT_DATA_FFD */
#else   /* No indirect data support */
	#define NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS  (0)
#endif  /* MAC_INDIRECT_DATA_BASIC */

// *****************************************************************************
/*  NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS
 
   Summary:
    Number of timers required to support scanning
   Description:
    None
   Remarks:
    None 
 */
        
#if (MAC_SCAN_SUPPORT == 1)
	#define NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS           (1)
#else
	#define NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS           (0)
#endif  /* MAC_SCAN_SUPPORT */

// *****************************************************************************
/*  NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS
 
   Summary:
    Number of timers required to support Rx-Enable
   Description:
    None
   Remarks:
    None 
 */
        
#if (MAC_RX_ENABLE_SUPPORT == 1)
	#define NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS      (1)
#else
	#define NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS      (0)
#endif  /* MAC_RX_ENABLE_SUPPORT */

// *****************************************************************************
/*  NUMBER_OF_MAC_TIMERS
 
   Summary:
    Total numbers of timers used in MAC layer
   Description:
    None
   Remarks:
    None 
 */

#define NUMBER_OF_MAC_TIMERS        (NUMBER_OF_MAC_INDIRECT_DATA_SUPPORT_TIMERS + \
	NUMBER_OF_MAC_SCAN_SUPPORT_TIMERS + \
	NUMBER_OF_MAC_RX_ENABLE_SUPPORT_TIMERS)
        
#ifdef ENABLE_QUEUE_CAPACITY

// *****************************************************************************
/* PHY_MAC_QUEUE_CAPACITY
   Summary:
    This macro holds configuring the phy mac queue capacity. 
   Description:
    None
   Remarks:
    None 
 */

#define PHY_MAC_QUEUE_CAPACITY              (255)

// *****************************************************************************
/* NHLE_MAC_QUEUE_CAPACITY
   Summary:
    This macro holds configuring the nhle mac queue capacity. 
   Description:
    None
   Remarks:
    None 
 */

#define NHLE_MAC_QUEUE_CAPACITY             (255)

// *****************************************************************************
/* INDIRECT_DATA_QUEUE_CAPACITY
   Summary:
    This macro holds configuring the indirect queue capacity. 
   Description:
    None
   Remarks:
    None 
 */

#define INDIRECT_DATA_QUEUE_CAPACITY        (255)

#endif /* ENABLE_QUEUE_CAPACITY */
        
// *****************************************************************************
// *****************************************************************************
// Externals
// *****************************************************************************
// *****************************************************************************

#if (NUMBER_OF_MAC_TIMERS > 0)
/* Timer type definition only in case there is any MAC timer */

#if (MAC_INDIRECT_DATA_BASIC == 1)
// *****************************************************************************
/* Timer_PollWaitTime
   Summary:
    Timer used by MAC for Maximum frame response time, also used for association process.
   Description:
    None
   Remarks:
    None 
 */

extern uint8_t Timer_PollWaitTime;
    #if (MAC_INDIRECT_DATA_FFD == 1)

// *****************************************************************************
/* Timer_DataPersistence
   Summary:
    Timer used by MAC for Indirect Data persistence timer.
   Description:
    None
   Remarks:
    None 
 */

extern uint8_t Timer_DataPersistence;
    #endif  /* (MAC_INDIRECT_DATA_FFD == 1) */
#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

#if (MAC_SCAN_SUPPORT == 1)

// *****************************************************************************
/* Timer_ScanDuration
   Summary:
    Timer used by MAC for Scan duration timer for SCAN.
   Description:
    None
   Remarks:
    None 
 */

extern uint8_t Timer_ScanDuration;
#endif  /* MAC_SCAN_SUPPORT */

#if (MAC_RX_ENABLE_SUPPORT == 1)
// *****************************************************************************
/* Timer_RxEnable
   Summary:
    Timer used by MAC for Receiver Enable timer. 
   Description:
    None
   Remarks:
    None 
 */

extern uint8_t Timer_RxEnable;
#endif  /* MAC_RX_ENABLE_SUPPORT */
#endif /* (NUMBER_OF_MAC_TIMERS != 0) */

/* === Prototypes =========================================================== */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* MAC_CONFIG_H */
/* EOF */
