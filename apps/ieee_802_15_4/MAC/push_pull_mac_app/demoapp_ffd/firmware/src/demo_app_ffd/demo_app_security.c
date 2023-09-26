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

/*******************************************************************************
  APP Security file

  Company:
    Microchip Technology Inc.

  File Name:
    app_security.c

  Summary:
    This module configures the MAC Security PIBs as configured by application layer

 *******************************************************************************/


/* === INCLUDES ============================================================ */
#ifdef MAC_SECURITY_ZIP
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "phy.h"
#include "mac_msg_types.h"
#include "demo_app_ffd.h"
#include "stb_generic.h"
#include "mac_security.h"
#include "ieee_mac_const.h"
#include "app.h"

/* === MACROS ============================================================== */

/* === GLOBALS ============================================================= */
/** Coordinator IEEE Address */
static uint8_t COORD_IEEE_ADDRESS[]={0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x00};

/*
 * This is implemented as an array of bytes, but actually this is a
 * 128-bit variable. This is the reason why the array needs to be filled
 * in in reverse order than expected.
 */
static const uint8_t default_key[4][16] = {
	{
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
	},
	{
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xD0
	},
	{
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xD1
	},
	{
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF
	}
};

/* Default key source for MAC Security */
uint8_t default_key_source[8]
	= {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Parameter to check the which mlme set confirm callback will currently using
**/
static uint8_t mlme_set_conf_run_time;
/* === PROTOTYPES ========================================================== */

/* Initialize the security PIB and setting the Security PIB for use by mac
 * security */
static void init_secuity_pib(uint8_t PIBAttribute, uint8_t PIBAttributeIndex);

/* mlme set confirm callback will be used after starting the network */
static void usr_mlme_set_conf_run_time(uint8_t status, uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex);

/* === IMPLEMENTATION ====================================================== */

/** @brief Initialize the security PIB and set the security parameters
 *
 *  @param PIBAttribute MAC PIB Attribute type
 *  @param PIBAttributeIndex MAC PIB Attribute Index
 *
 */
static void init_secuity_pib(uint8_t PIBAttribute, uint8_t PIBAttributeIndex)
{
	switch (PIBAttribute) {
	case macDefaultKeySource:
	{
		/* Set the no.of security level table  entries */
		uint8_t mac_sec_level_table_entries
			= DEFAULT_MAC_SEC_LVL_TABLE_ENTRIES;

		(void)WPAN_MLME_SetReq(macSecurityLevelTableEntries,
				NO_PIB_INDEX,
				&mac_sec_level_table_entries);
	}
	break;

	case macSecurityLevelTableEntries:
	{
		/* set type of frames will be encrypted and decrypted */
		uint8_t mac_sec_level_table[4] = {  FRAME_TYPE_BEACON, 
                                            CMD_FRAME_ID_NA,  
                                            (uint8_t)SECURITY_05_LEVEL,
                                            DEV_OVERRIDE_SEC_MIN 		};

		(void)WPAN_MLME_SetReq(macSecurityLevelTable,
				INDEX_1,    /* Index: 1 */
				&mac_sec_level_table);
	}
	break;

	case macSecurityLevelTable:
	{
		if (INDEX_1 == PIBAttributeIndex) 
        {
			/* set type of frames will be encrypted and decrypted */
			uint8_t mac_sec_level_table[4] =
			{       FRAME_TYPE_DATA,             
                    CMD_FRAME_ID_NA, 
                    (uint8_t)SECURITY_05_LEVEL,
                    DEV_OVERRIDE_SEC_MIN 
			};
			(void)WPAN_MLME_SetReq(macSecurityLevelTable,
					INDEX_0, /* Index: 0 */
					&mac_sec_level_table);
		} 
        else if (INDEX_0 == PIBAttributeIndex) 
        {
			/* set the maximum no.of mac key table entries */
			uint8_t mac_key_table_entries
				= DEFAULT_MAC_KEY_TABLE_ENTRIES;

			(void)WPAN_MLME_SetReq(macKeyTableEntries,
					NO_PIB_INDEX,
					&mac_key_table_entries);
		}
        else
        {      
            /*do nothing*/              
        }
	}
	break;

	case macKeyTableEntries:
	{
		/* set the maximum no.of device table entries */
		uint16_t coord_key_index = DEFAULT_MAC_DEV_TABLE_ENTRIES;
		(void)WPAN_MLME_SetReq(macDeviceTableEntries,
				NO_PIB_INDEX,
				&coord_key_index);
	}
	break;

	case macDeviceTableEntries:
	{
		/* set the default mac key table values */
		uint8_t mac_key_table[43] = {
			default_key_source[0], /* LookupData[0] */
			default_key_source[1], /* LookupData[1] */
			default_key_source[2], /* LookupData[2] */
			default_key_source[3], /* LookupData[3] */
			default_key_source[4], /* LookupData[4] */
			default_key_source[5], /* LookupData[5] */
			default_key_source[6], /* LookupData[6] */
			default_key_source[7], /* LookupData[7] */
			KEY_INDEX_4,       /* LookupData[8] */
			LOOKUP_DATA_SIZE_1, /* LookupDataSize: 0x01 : Size 9
				             * octets */
			MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES,
			EMPTY_DEV_HANDLE,       /* DeviceDescriptorHandle **/
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			EMPTY_DEV_HANDLE,       /* DeviceDescriptorHandle **/
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			EMPTY_DEV_HANDLE,       /* DeviceDescriptorHandle **/
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			EMPTY_DEV_HANDLE,                   /* DeviceDescriptorHandle*/
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES, /**KeyDeviceListEntries**/                         
			/*  KeyUsageList */
			FRAME_TYPE_DATA, /* FrameType - Beacon frames */
			CMD_FRAME_ID_NA, /* CommandFrameIdentifier not used in ZIP */				          
			MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES, /*KeyUsageListEntries**/
				                             				                             
			/* Key */
			default_key[3][0],
			default_key[3][1],
			default_key[3][2],
			default_key[3][3],
			default_key[3][4],
			default_key[3][5],
			default_key[3][6],
			default_key[3][7],
			default_key[3][8],
			default_key[3][9],
			default_key[3][10],
			default_key[3][11],
			default_key[3][12],
			default_key[3][13],
			default_key[3][14],
			default_key[3][15],
		};
		(void)WPAN_MLME_SetReq(macKeyTable,
				INDEX_3, /* Index: 3 */
				&mac_key_table);
	}
	break;

	case macKeyTable:
	{
		/* set the default mac key table values */
		switch (PIBAttributeIndex) {
		case INDEX_3:
		{
			uint8_t mac_key_table1[43] = {
				/* KeyIdLookupList[1].LookupData :
				 * macDefaultKeySource || g_Sec_KeyIndex_1 */
				default_key_source[0],/*LookupData[0]**/
						                               					                                
				default_key_source[1],/*LookupData[1]**/
						                               						                                
				default_key_source[2],/*LookupData[2]**/
						                               						                                
				default_key_source[3],/**LookupData[3]**/
						                              
				default_key_source[4],/**LookupData[4]**/
						                                
				default_key_source[5],/**LookupData[5]**/
					                                
				default_key_source[6],/**LookupData[6]**/
						                              
				default_key_source[7],/**LookupData[7] **/
						                            					                               
				KEY_INDEX_1,/**LookupData[8] **/
				                        
				LOOKUP_DATA_SIZE_1,/**LookupDataSize:
						              *0x01:
						               *Size
						               * 9              
						               octets **/  
				MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES,/*KeyIdLookupListEntries=1 **/
						                                              
				/* KeyDeviceList[1] */
				EMPTY_DEV_HANDLE, /*DeviceDescriptorHandle**/
						                                     
				(uint8_t)false,/* UniqueDevice **/
				(uint8_t)false,/* Blacklisted **/
						                             
				EMPTY_DEV_HANDLE, /*DeviceDescriptorHandle**/
				(uint8_t)false,   /*UniqueDevice  **/
				(uint8_t)false, /** Blacklisted**/
				EMPTY_DEV_HANDLE,
				(uint8_t)false,
				(uint8_t)false,
				EMPTY_DEV_HANDLE,
				(uint8_t)false,
				(uint8_t)false,
				MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES,
				/*  KeyUsageList */
				FRAME_TYPE_DATA, 
				CMD_FRAME_ID_NA,
				MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES, 
				/* Key */
				default_key[0][0],
				default_key[0][1],
				default_key[0][2],
				default_key[0][3],
				default_key[0][4],
				default_key[0][5],
				default_key[0][6],
				default_key[0][7],
				default_key[0][8],
				default_key[0][9],
				default_key[0][10],
				default_key[0][11],
				default_key[0][12],
				default_key[0][13],
				default_key[0][14],
				default_key[0][15],
			};
			(void)WPAN_MLME_SetReq(macKeyTable,
					INDEX_0,            /* Index: 0 */
					&mac_key_table1);
		}
		break;

		case INDEX_0:
		{
			uint8_t mac_key_table2[43] = {
				/* KeyIdLookupList[1].LookupData :
				 * macDefaultKeySource || g_Sec_KeyIndex_1 */
				default_key_source[0],
				default_key_source[1],
				default_key_source[2], 
				default_key_source[3],
				default_key_source[4],
				default_key_source[5],
				default_key_source[6], 
				default_key_source[7],
				KEY_INDEX_2,
				LOOKUP_DATA_SIZE_1, 
				MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES, 
				/* KeyDeviceList[1] */
				EMPTY_DEV_HANDLE,
				(uint8_t)false,
				(uint8_t)false,
				EMPTY_DEV_HANDLE, 
				(uint8_t)false,
				(uint8_t)false,
				EMPTY_DEV_HANDLE, 
				(uint8_t)false,
				(uint8_t)false,
				EMPTY_DEV_HANDLE,
				(uint8_t)false, 
				(uint8_t)false, 
				MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES,
				/*  KeyUsageList */
				FRAME_TYPE_DATA,
				CMD_FRAME_ID_NA,
				MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES, 
				/* Key */
				default_key[1][0],
				default_key[1][1],
				default_key[1][2],
				default_key[1][3],
				default_key[1][4],
				default_key[1][5],
				default_key[1][6],
				default_key[1][7],
				default_key[1][8],
				default_key[1][9],
				default_key[1][10],
				default_key[1][11],
				default_key[1][12],
				default_key[1][13],
				default_key[1][14],
				default_key[1][15],
			};

			(void)WPAN_MLME_SetReq(macKeyTable,
					INDEX_1,            /* Index: 1 */
					&mac_key_table2);
		}
		break;

		case INDEX_1:
		{
			uint8_t mac_key_table3[43] = {
				/* KeyIdLookupList[1].LookupData :
				 * macDefaultKeySource || g_Sec_KeyIndex_1 */
				default_key_source[0],
				default_key_source[1],
				default_key_source[2],  
				default_key_source[3],  
				default_key_source[4],
				default_key_source[5],  
				default_key_source[6], 
				default_key_source[7],
				KEY_INDEX_3,            
				LOOKUP_DATA_SIZE_1, 
				MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES, 
				/* KeyDeviceList[1] */
				EMPTY_DEV_HANDLE, 
				(uint8_t)false,
				(uint8_t)false, 
				EMPTY_DEV_HANDLE, 
				(uint8_t)false,  
				(uint8_t)false,
				EMPTY_DEV_HANDLE, 
				(uint8_t)false,  
				(uint8_t)false, 
				EMPTY_DEV_HANDLE,   
				(uint8_t)false,     
				(uint8_t)false,  
				MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES, 
				/*  KeyUsageList */
				FRAME_TYPE_DATA, 
				CMD_FRAME_ID_NA, 
				MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES,
				/* Key */
				default_key[2][0],
				default_key[2][1],
				default_key[2][2],
				default_key[2][3],
				default_key[2][4],
				default_key[2][5],
				default_key[2][6],
				default_key[2][7],
				default_key[2][8],
				default_key[2][9],
				default_key[2][10],
				default_key[2][11],
				default_key[2][12],
				default_key[2][13],
				default_key[2][14],
				default_key[2][15],
			};
			(void)WPAN_MLME_SetReq(macKeyTable,
					INDEX_2,                        
					&mac_key_table3);
		}
		break;

		case INDEX_2:
		{
			/** set the coordinator IEEE Address and default PAN ID
			 *  into the MAC Device table
			 */
			uint8_t coord_dev_index = 3;
			uint8_t mac_dev_table[17];
            Addr16_t coord_shrt_addr;
            coord_shrt_addr.m16 = COORD_SHORT_ADDR;

			mac_dev_table[0] = (uint8_t)DEFAULT_PAN_ID;
			mac_dev_table[1] = (uint8_t)(DEFAULT_PAN_ID >> 8);
			mac_dev_table[2] = coord_shrt_addr.m8[0];
            mac_dev_table[3] = coord_shrt_addr.m8[1];            
			mac_dev_table[4] = COORD_IEEE_ADDRESS[7];
			mac_dev_table[5] = COORD_IEEE_ADDRESS[6];
			mac_dev_table[6] = COORD_IEEE_ADDRESS[5];
			mac_dev_table[7] = COORD_IEEE_ADDRESS[4];
			mac_dev_table[8] = COORD_IEEE_ADDRESS[3];
			mac_dev_table[9] = COORD_IEEE_ADDRESS[2];
			mac_dev_table[10] = COORD_IEEE_ADDRESS[1];
			mac_dev_table[11] = COORD_IEEE_ADDRESS[0];
			mac_dev_table[12] = 0;                  /* Frame counter    **/
			mac_dev_table[13] = 0;
			mac_dev_table[14] = 0;
			mac_dev_table[15] = 0;
			mac_dev_table[16] = 0;                  /* Exempt */

			(void)WPAN_MLME_SetReq(macDeviceTable,
					coord_dev_index,&mac_dev_table);
		}
		break;

		default:
            /*do nothing*/
			break;
		}
	}
	break;

	case macDeviceTable:
	{
        	/* Use DSN start value as in ZIP test spec. */
			uint32_t frame_counter = 1;

			(void)WPAN_MLME_SetReq(macFrameCounter,
					NO_PIB_INDEX,
					&frame_counter);
    }
    break;
    
    case macFrameCounter:
    {
		/* Set the PAN Coordinator Extended Address */
		uint8_t pan_coord_add[8];
		pan_coord_add[0] = COORD_IEEE_ADDRESS[7];
		pan_coord_add[1] = COORD_IEEE_ADDRESS[6];
		pan_coord_add[2] = COORD_IEEE_ADDRESS[5];
		pan_coord_add[3] = COORD_IEEE_ADDRESS[4];
		pan_coord_add[4] = COORD_IEEE_ADDRESS[3];
		pan_coord_add[5] = COORD_IEEE_ADDRESS[2];
		pan_coord_add[6] = COORD_IEEE_ADDRESS[1];
		pan_coord_add[7] = COORD_IEEE_ADDRESS[0];

		(void)WPAN_MLME_SetReq(macPANCoordExtendedAddress,
				NO_PIB_INDEX,pan_coord_add);
	}
	break;

	case macPANCoordExtendedAddress:
	{
		/* Set the PAN Coordinator IEEE Address */
		uint8_t pan_coord_add[8];
		pan_coord_add[0] = COORD_IEEE_ADDRESS[7];
		pan_coord_add[1] = COORD_IEEE_ADDRESS[6];
		pan_coord_add[2] = COORD_IEEE_ADDRESS[5];
		pan_coord_add[3] = COORD_IEEE_ADDRESS[4];
		pan_coord_add[4] = COORD_IEEE_ADDRESS[3];
		pan_coord_add[5] = COORD_IEEE_ADDRESS[2];
		pan_coord_add[6] = COORD_IEEE_ADDRESS[1];
		pan_coord_add[7] = COORD_IEEE_ADDRESS[0];

		(void)WPAN_MLME_SetReq(macIeeeAddress,
				NO_PIB_INDEX, /* Index */
				pan_coord_add);
	}
	break;

	case macIeeeAddress:
	{
		/* Set the PAN Coordinator Short Address */
		uint16_t short_add = COORD_SHORT_ADDR;
		(void)WPAN_MLME_SetReq(macPANCoordShortAddress,
				NO_PIB_INDEX,&short_add);
	}
	break;

	case macPANCoordShortAddress:
	{
		/* Set the PAN Coordinator Short Address */
		uint16_t short_add = COORD_SHORT_ADDR;        

		(void)WPAN_MLME_SetReq(macShortAddress,
				NO_PIB_INDEX,
				(void *)&short_add);
		mlme_set_conf_run_time = (uint8_t)true;
	}
	break;

	default:
        /*do nothing*/
		break;
	}
}
/*
 * @brief Callback function usr_mlme_set_conf will be used after starting the
 * network
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 *
 */
static void usr_mlme_set_conf_run_time(uint8_t status, uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex)
{
	if (status != (uint8_t)MAC_SUCCESS) {
		/* something went wrong at mlme set request; restart */
		(void)WPAN_MLME_ResetReq(true);
	} else {
		switch (PIBAttribute) {
		case macShortAddress:
		{
			/* set the mac association permission */
			uint8_t association_permit = (uint8_t)true;

			(void)WPAN_MLME_SetReq(macAssociationPermit, NO_PIB_INDEX,
					&association_permit);
		}
		break;

		case macAssociationPermit:
		{
			/* Set the MAC Rx on when Idle */
			bool rx_on_when_idle = true;

			(void)WPAN_MLME_SetReq(macRxOnWhenIdle, NO_PIB_INDEX,
					&rx_on_when_idle);
		}
		break;

		case macRxOnWhenIdle:
		{
			/* Set the beacon payload length. */
			uint8_t beacon_payload_len = BEACON_PAYLOAD_LEN;
			(void)WPAN_MLME_SetReq(macBeaconPayloadLength, NO_PIB_INDEX,
					&beacon_payload_len);
		}
		break;

		case macBeaconPayloadLength:
		{
			/*
			 * Once the length of the beacon payload has been
			 * defined,
			 * set the actual beacon payload.
			 */
			(void)WPAN_MLME_SetReq(macBeaconPayload, NO_PIB_INDEX,
					&beacon_payload);
		}
		break;

		case macBeaconPayload:
		{
			if (COORD_STARTING == coord_state) {
				/*
				 * Initiate an active scan over all channels to
				 * determine
				 * which channel to use.
				 * Use: bool wpan_mlme_scan_req(uint8_t
				 * ScanType,
				 *                              uint32_t
				 * ScanChannels,
				 *                              uint8_t
				 * ScanDuration,
				 *                              uint8_t
				 * ChannelPage);
				 *
				 * This request leads to a scan confirm message
				 *->
				 * usr_mlme_scan_conf
				 * Scan for about 50 ms on each channel ->
				 * ScanDuration
				 *= 1
				 * Scan for about 1/2 second on each channel ->
				 * ScanDuration = 5
				 * Scan for about 1 second on each channel ->
				 * ScanDuration = 6
				 */
				(void)WPAN_MLME_ScanReq(MLME_SCAN_TYPE_ACTIVE,SCAN_CHANNEL(current_channel),SCAN_DURATION_COORDINATOR,current_channel_page);
			}
		}
		break;

		case macDefaultKeySource:
		{
			/* set the maximum no.of mac security level table
			 * entries */
			uint8_t mac_sec_level_table_entries
				= DEFAULT_MAC_SEC_LVL_TABLE_ENTRIES;

			(void)WPAN_MLME_SetReq(macSecurityLevelTableEntries,
					NO_PIB_INDEX,
					&mac_sec_level_table_entries);
		}
		break;

		case macSecurityLevelTableEntries:
		{
			/* set the mac security level table for the MAC Data
			 * Security */
			uint8_t mac_sec_level_table[4] = {FRAME_TYPE_DATA,
							  CMD_FRAME_ID_NA,
							  0X05,
							  DEV_OVERRIDE_SEC_MIN };

			(void)WPAN_MLME_SetReq(macSecurityLevelTable,
					INDEX_0,  /* Index: 0 */
					&mac_sec_level_table);
		}
		break;

		case macSecurityLevelTable:
		{
			/* set the maximum no.of key table entries */
			uint8_t mac_key_table_entries
				= DEFAULT_MAC_KEY_TABLE_ENTRIES;

			(void)WPAN_MLME_SetReq(macKeyTableEntries,
					NO_PIB_INDEX,
					&mac_key_table_entries);
		}
		break;

		case macKeyTableEntries:
		{
			/* MAC Beacon Frame type default mac key table values */
			uint8_t mac_key_table[43] = {
				default_key_source[0], 
				default_key_source[1], 
				default_key_source[2],  
				default_key_source[3],  
				default_key_source[4],  
				default_key_source[5],  
				default_key_source[6],
				default_key_source[7], 
				KEY_INDEX_4,       
				LOOKUP_DATA_SIZE_1, 
				MAC_ZIP_MAX_KEY_ID_LOOKUP_LIST_ENTRIES, 
				/* KeyDeviceList[1] */
				EMPTY_DEV_HANDLE, 
				(uint8_t)true,  
				(uint8_t)false, 
				EMPTY_DEV_HANDLE, 
				(uint8_t)true, 
				(uint8_t)false, 
				EMPTY_DEV_HANDLE,  
				(uint8_t)true,
				(uint8_t)false, 
				EMPTY_DEV_HANDLE,
				(uint8_t)true, 
				(uint8_t)false, 
				MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES, 
				/*  KeyUsageList */
				FRAME_TYPE_DATA, 
				CMD_FRAME_ID_NA,  
				MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES,
				/* Key */
				default_key[3][0],
				default_key[3][1],
				default_key[3][2],
				default_key[3][3],
				default_key[3][4],
				default_key[3][5],
				default_key[3][6],
				default_key[3][7],
				default_key[3][8],
				default_key[3][9],
				default_key[3][10],
				default_key[3][11],
				default_key[3][12],
				default_key[3][13],
				default_key[3][14],
				default_key[3][15],
			};
			(void)WPAN_MLME_SetReq(macKeyTable,
					INDEX_3,    /* Index: 3 */
					&mac_key_table);
		}
		break;

		case macKeyTable:
			/*
			 * Setting of PIB attributes will continue once a device
			 * has associated successful.
			 */
			break;

		case macDeviceTableEntries:
		{
			/* MAC Device Table entries for the recently associated
			 * device */
			static uint8_t Temp = 0;
			uint8_t mac_dev_table[17];
           
			/* Temp is used to not update the already device table
			 * again */
			for (uint16_t i = Temp; i < no_of_assoc_devices; i++) {
				mac_dev_table[0]
					= (uint8_t)(DEFAULT_PAN_ID &
						0x00FFU);
				mac_dev_table[1]
					= (uint8_t)(DEFAULT_PAN_ID >>
						8U);
				mac_dev_table[2]
					= (uint8_t)device_list[i].shortAddr;
				mac_dev_table[3]
					= (uint8_t)(device_list[i].shortAddr
						>> 8);
				mac_dev_table[4]
					= (uint8_t)device_list[i].ieeeAddr;
				mac_dev_table[5]
					= (uint8_t)(device_list[i].ieeeAddr
						>> 8);
				mac_dev_table[6]
					= (uint8_t)(device_list[i].ieeeAddr
						>> 16);
				mac_dev_table[7]
					= (uint8_t)(device_list[i].ieeeAddr
						>> 24);
				mac_dev_table[8]
					= (uint8_t)(device_list[i].ieeeAddr
						>> 32);
				mac_dev_table[9]
					= (uint8_t)(device_list[i].ieeeAddr
						>> 40);
				mac_dev_table[10]
					= (uint8_t)(device_list[i].ieeeAddr
						>> 48);
				mac_dev_table[11]
					= (uint8_t)(device_list[i].ieeeAddr
						>> 56);
				mac_dev_table[12] = 0;      /* Initial Frame counter value */
				mac_dev_table[13] = 0;
				mac_dev_table[14] = 0;
				mac_dev_table[15] = 0;
				mac_dev_table[16] = 0;      /* Exempt */

				(void)WPAN_MLME_SetReq(macDeviceTable,
						(uint8_t)i, /* Index */
						&mac_dev_table);
			}
			Temp++;
		}
		break;

		case macDeviceTable:
		{
			/* Use DSN start value as in ZIP test spec. */
			uint8_t new_dsn = 0x0;

			(void)WPAN_MLME_SetReq(macDSN,
					NO_PIB_INDEX,
					&new_dsn);
		}
		break;

		case macDSN:
            break;

		case macFrameCounter:
			break;
			
		case macTransactionPersistenceTime:
			break;

		default:
			/* undesired PIB attribute set request; restart */
			(void)WPAN_MLME_ResetReq(true);
			break;
		}
	}
	/* Keep compiler happy. */
	PIBAttributeIndex = PIBAttributeIndex;
}

void Handle_SetConf(APP_Msg_T *appMsg)
{
    usr_mlme_set_conf_t setConf;
    (void)memcpy((void *)&setConf,(const void *)appMsg->msgData,sizeof(usr_mlme_set_conf_t));
     
    if (setConf.status != (uint8_t)MAC_SUCCESS) {
		/* something went wrong with mlme set request; restart */
		(void)WPAN_MLME_ResetReq(true);
	} else {
		if ((bool)mlme_set_conf_run_time) {
			/* post initialization security pib callback */
			usr_mlme_set_conf_run_time(setConf.status, setConf.PIBAttribute,
					setConf.PIBAttributeIndex);
		} else {
			/* Initialize the mac security PIB before starting the
			 * network */
			init_secuity_pib(setConf.PIBAttribute, setConf.PIBAttributeIndex);
		}
	}

	/* Keep compiler happy. */
	(void)setConf.PIBAttributeIndex; 
}

#endif

  

