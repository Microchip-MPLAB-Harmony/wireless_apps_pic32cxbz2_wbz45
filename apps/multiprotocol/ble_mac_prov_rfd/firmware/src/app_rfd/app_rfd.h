/*******************************************************************************
  MAC No Beacon RFD Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    nobeacon_rfd.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    These are application-specific resources which are used
    in the example application of the coordinator in addition to the
    underlaying stack.
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

/* Prevent double inclusion */
#ifndef NO_BEACON_APP_H
#define NO_BEACON_APP_H

/* === Includes ============================================================= */
#include "app.h"
#include "mac_api.h"
#include "app_prov/app_prov.h"
/* === TYPES =============================================================== */

/** This type definition of a structure can store the short address and the
 *  extended address of a device.
 */
typedef struct associated_device_tag {
	uint16_t shortAddr;
	uint64_t ieeeAddr;
} AssociatedDevice_t;

/**
 * This enum stores the current state of the device.
 */
typedef enum device_state_tag {
	DEVICE_IDLE = 0,
	DEVICE_SCAN_DONE,
	DEVICE_ASSOC_IN_PROGRESS,
	DEVICE_RUNNING
}
DeviceState_t;

typedef struct usr_mcps_data_cnf
{
    uint8_t msduHandle;
	uint8_t status;
	uint32_t Timestamp;
}usr_mcps_data_cnf_t;

typedef struct usr_mcps_data_ind
{
    WPAN_AddrSpec_t *SrcAddrSpec;
		WPAN_AddrSpec_t *DstAddrSpec;
		uint8_t msduLength;
		uint8_t *msdu;
		uint8_t mpduLinkQuality;
		uint8_t DSN;
#ifdef ENABLE_TSTAMP
		uint32_t Timestamp;
#endif  /* ENABLE_TSTAMP */
#ifdef MAC_SECURITY_ZIP
		uint8_t SecurityLevel;
		uint8_t KeyIdMode;
		uint8_t KeyIndex;
#endif
}usr_mcps_data_ind_t;

typedef struct usr_mlme_associate_conf
{
    uint16_t AssocShortAddress;
	uint8_t status;
}usr_mlme_associate_conf_t;

typedef struct usr_mlme_beacon_notify_ind
{
    uint8_t BSN;
	WPAN_Pandescriptor_t *PANDescriptor;
	uint8_t PendAddrSpec;
	uint8_t *AddrList;
	uint8_t sduLength;
	uint8_t *sdu;
}usr_mlme_beacon_notify_ind_t;

typedef struct usr_mlme_dissociate_conf
{
    uint8_t status;
	WPAN_AddrSpec_t *DeviceAddrSpec;
}usr_mlme_dissociate_conf_t;

typedef struct usr_mlme_dissociate_ind
{
    uint64_t DeviceAddress;
	uint8_t DisassociateReason;
}usr_mlme_dissociate_ind_t;

typedef struct usr_mlme_get_conf
{
    uint8_t status;
	uint8_t PIBAttribute;
#ifdef MAC_SECURITY_ZIP
    uint8_t PIBAttributeIndex;
#endif  /* MAC_SECURITY_ZIP */
	void *PIBAttributeValue;
}usr_mlme_get_conf_t;

typedef struct usr_mlme_poll_conf
{
    uint8_t status;
}usr_mlme_poll_conf_t;

typedef struct usr_mlme_reset_conf
{
    uint8_t status;
}usr_mlme_reset_conf_t;

typedef struct usr_mlme_rxenable_conf
{
    uint8_t status;
}usr_mlme_rxenable_conf_t;

typedef struct usr_mlme_scan_conf
{
    uint8_t status;
	uint8_t ScanType;
	uint8_t ChannelPage;
    uint32_t UnscannedChannels;
	uint8_t ResultListSize;
    void *ResultList;
}usr_mlme_scan_conf_t;

typedef struct usr_mlme_set_conf
{
    uint8_t status;
    uint8_t PIBAttribute;
#ifdef MAC_SECURITY_ZIP
    uint8_t PIBAttributeIndex;
#endif
    
}usr_mlme_set_conf_t;

typedef struct usr_mlme_start_conf
{
    uint8_t status;
}usr_mlme_start_conf_t;

typedef union Addr16
{
  uint8_t m8[2];   // 8-bit fields
  uint16_t m16;
} Addr16_t;

typedef __PACKED_STRUCT macCommissioningParam
{
    uint8_t channel;
    uint16_t pan_id;
}macCommissioningParam_t;

/* === Macros =============================================================== */
#define DEFAULT_PAN_ID                  (0x1111U)

/** Defines the short address of the coordinator. */
#define COORD_SHORT_ADDR                (0x0000U)

#define SCAN_CHANNEL(x)                    (1UL << (x))

#ifdef MAC_SECURITY_ZIP
/* MAC security macros */
#define KEY_INDEX_0                     (0)
#define KEY_INDEX_1                     (1)
#define KEY_INDEX_2                     (2)
#define KEY_INDEX_3                     (3)
#define KEY_INDEX_4                     (4)
#define LOOKUP_DATA_SIZE_1              (1) /* Size is 9 octets */
#define FRAME_TYPE_BEACON               (0)
#define FRAME_TYPE_DATA                 (1)
#define CMD_FRAME_ID_NA                 (0) /* CommandFrameIdentifier is n/a */
#define ZIP_SEC_MIN                     (0x05) /* SecurityMinimum for ZIP is 5
	                                       **/
#define DEV_OVERRIDE_SEC_MIN            (1) /* DeviceOverrideSecurityMinimum:
	                                     * True */
#define ZIP_KEY_ID_MODE                 (1) /* ZIP uses KeyIdMode 1 */

#define INDEX_0                         (0U)
#define INDEX_1                         (1U)
#define INDEX_2                         (2U)
#define INDEX_3                         (3U)
#define DEV_DESC_HANDLE_IDX_0                   (0x00)
#define EMPTY_DEV_HANDLE                (0xFFU) /* key device desc is invalid */
#define KEY_INFO_FRAME                  (0xDE)
#define NO_SECURITY                     (0)
#define SEC_PIB_INIT                                    (1)

/* Security Default Device Table Entries */
#define DEFAULT_MAC_DEV_TABLE_ENTRIES (0x04)

/* Security Default Key Table Entries */
#define DEFAULT_MAC_KEY_TABLE_ENTRIES (0x04)

/* Security Default Security Level Table Entries */
#define DEFAULT_MAC_SEC_LVL_TABLE_ENTRIES (0x02)
#endif

/* === Externals ============================================================ */
extern uint8_t default_key_source[8];

extern WPAN_AddrSpec_t coordAddrSpec;

extern DeviceState_t device_state;

extern uint32_t deviceShortAddress;

extern uint8_t current_channel;

extern uint8_t current_channel_page;
/* === Prototypes =========================================================== */
/** This function shows the stack and application
 *  capabilities on terminal.
 */
void print_stack_app_build_features(void);

/* Converts a 4 Byte array into a 32-Bit value */
static inline uint32_t convert_byte_array_to_32_bit(uint8_t *data)
{
	union
	{
		uint32_t u32;
		uint8_t u8[4];
	}long_addr;
	uint8_t index;
	for (index = 0; index < 4U; index++)
	{
		long_addr.u8[index] = *data++;
	}
	return long_addr.u32;
}

void MAC_RFDDemoInit(void);

void Handle_Associate_Req(APP_ProvNwData_T *macparam);

void Handle_MACMessages(APP_Msg_T *appMsg);

void Handle_ResetConf(APP_Msg_T *appMsg);

void Handle_GetConf(APP_Msg_T *appMsg);

void Handle_SetConf(APP_Msg_T *appMsg);

void Handle_ScanConf(APP_Msg_T *appMsg);

void Handle_AssociateConf(APP_Msg_T *appMsg);

void Handle_PollConf(APP_Msg_T *appMsg);

void Handle_BeaconNotifyInd(APP_Msg_T *appMsg);

void Handle_DataInd(APP_Msg_T *appMsg);

void Handle_DeviceSleep(APP_Msg_T *appMsg);

void Handle_DataConf(APP_Msg_T *appMsg);

void ble_device_init(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NO_BEACON_APP_H */
/* EOF */
