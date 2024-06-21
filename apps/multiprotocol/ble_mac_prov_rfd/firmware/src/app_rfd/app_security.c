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
#include "app_rfd.h"
#include "stb_generic.h"
#include "mac_security.h"
#include "ieee_mac_const.h"
#include "mac_api.h"
#include "app.h"

/* === MACROS ============================================================== */
static uint8_t COORD_IEEE_ADDRESS[] = {0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x00};
/* === GLOBALS ============================================================= */


/*
 * This is implemented as an array of bytes, but actually this is a
 * 128-bit variable. This is the reason why the array needs to be filled
 * in in reverse order than expected.
 */
static uint8_t default_key[4][16] = {
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

uint8_t default_key_source[8]
	= {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* === PROTOTYPES ========================================================== */

/** Initialize the security PIB and setting the Security PIB for use by mac
 * security */
static void init_secuity_pib(uint8_t PIBAttribute, uint8_t PIBAttributeIndex);
static void usr_mlme_set_conf_run_time(uint8_t status, uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex);

/* === IMPLEMENTATION ====================================================== */

/*
 * @brief Callback function USR_MLME_SetConf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
static uint8_t mlme_set_conf_run_time;
void USR_MLME_SetConf(uint8_t status, uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex)
{
    
    usr_mlme_set_conf_t setConf;
    APP_Msg_T appMsg;
    
    setConf.status = status;
    setConf.PIBAttribute = PIBAttribute;
    setConf.PIBAttributeIndex =PIBAttributeIndex;
            
    appMsg.msgId = APP_MSG_MAC_SET_CONF;
    
    (void)memcpy((void *)appMsg.msgData,(const void *)&setConf,sizeof(setConf));
    
    (void)OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);
}

/** Initialize the security PIB and set the security parameters */
static void init_secuity_pib(uint8_t PIBAttribute, uint8_t PIBAttributeIndex)
{
	switch (PIBAttribute) {
	case macDefaultKeySource:
	{
		uint8_t mac_sec_level_table_entries
			= DEFAULT_MAC_SEC_LVL_TABLE_ENTRIES;

		(void)WPAN_MLME_SetReq(macSecurityLevelTableEntries,
				NO_PIB_INDEX,
				&mac_sec_level_table_entries);
	}
	break;

	case macSecurityLevelTableEntries:
	{
		uint8_t mac_sec_level_table[4] = {  FRAME_TYPE_BEACON, 
						    CMD_FRAME_ID_NA,
						    (uint8_t)SECURITY_05_LEVEL,
						    DEV_OVERRIDE_SEC_MIN };

		(void)WPAN_MLME_SetReq(macSecurityLevelTable,
				INDEX_1,    /* Index: 1 */
				&mac_sec_level_table);
	}
	break;

	case macSecurityLevelTable:
	{
		if (INDEX_1 == PIBAttributeIndex) {
			uint8_t mac_sec_level_table[4] =
			{       FRAME_TYPE_DATA,
				CMD_FRAME_ID_NA,  
				(uint8_t)SECURITY_05_LEVEL,
				DEV_OVERRIDE_SEC_MIN 
			};
			(void)WPAN_MLME_SetReq(macSecurityLevelTable,
					INDEX_0, /* Index: 0 */
					&mac_sec_level_table);
		} else if (INDEX_0 == PIBAttributeIndex) {
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
		uint16_t coord_key_index = DEFAULT_MAC_DEV_TABLE_ENTRIES;
		(void)WPAN_MLME_SetReq(macDeviceTableEntries,
				NO_PIB_INDEX,
				&coord_key_index);
	}
	break;

	case macDeviceTableEntries:
	{
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
			/* KeyDeviceList[1] */
			DEV_DESC_HANDLE_IDX_0, 
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			DEV_DESC_HANDLE_IDX_0,
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			DEV_DESC_HANDLE_IDX_0,       /* DeviceDescriptorHandle**/
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			DEV_DESC_HANDLE_IDX_0, /**DeviceDescriptorHandle**/                                
			(uint8_t)true,          /* UniqueDevice */
			(uint8_t)false,          /* Blacklisted */
			MAC_ZIP_MAX_KEY_DEV_LIST_ENTRIES, /**KeyDeviceListEntries**/
			                         
			/*  KeyUsageList */
			FRAME_TYPE_DATA, /* FrameType - Beacon frames */
			CMD_FRAME_ID_NA, /* CommandFrameIdentifier not used in
				          * ZIP */
			MAC_ZIP_MAX_KEY_USAGE_LIST_ENTRIES, /**KeyUsageListEntries**/
				                       
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
		switch (PIBAttributeIndex) {
		case INDEX_3:
		{
			uint8_t mac_key_table1[43] = {
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
				KEY_INDEX_1,            
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
			uint8_t coord_dev_index = 3;
			uint8_t mac_dev_table[17];
            Addr16_t coord_shrt_addr;
            coord_shrt_addr.m16 = COORD_SHORT_ADDR;


			mac_dev_table[0] = (uint8_t)DEFAULT_PAN_ID;
			mac_dev_table[1] = (uint8_t)(DEFAULT_PAN_ID >> 8U);
			
            mac_dev_table[2] = coord_shrt_addr.m8[0];
            mac_dev_table[3] = coord_shrt_addr.m8[1];
			mac_dev_table[4] =  COORD_IEEE_ADDRESS[7];
			mac_dev_table[5] =  COORD_IEEE_ADDRESS[6];
			mac_dev_table[6] =  COORD_IEEE_ADDRESS[5];
			mac_dev_table[7] =  COORD_IEEE_ADDRESS[4];
			mac_dev_table[8] =  COORD_IEEE_ADDRESS[3];
			mac_dev_table[9] =  COORD_IEEE_ADDRESS[2];
			mac_dev_table[10] = COORD_IEEE_ADDRESS[1];
			mac_dev_table[11] = COORD_IEEE_ADDRESS[0];
			mac_dev_table[12] = 0;                  /* Frame counter     **/
			mac_dev_table[13] = 0;
			mac_dev_table[14] = 0;
			mac_dev_table[15] = 0;
			mac_dev_table[16] = 0;                  /* Exempt */

			(void)WPAN_MLME_SetReq(macDeviceTable,
					coord_dev_index,       
					&mac_dev_table);
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
				NO_PIB_INDEX,       
				pan_coord_add);
	}
	break;

	case macPANCoordExtendedAddress:
	{
		uint16_t short_add = COORD_SHORT_ADDR;
		(void)WPAN_MLME_SetReq(macPANCoordShortAddress,
				NO_PIB_INDEX,         
				&short_add);
	}
	break;

	case macPANCoordShortAddress:
	{
		uint8_t mac_sec_level_table_entries
			= DEFAULT_MAC_SEC_LVL_TABLE_ENTRIES;

		(void)WPAN_MLME_SetReq(macSecurityLevelTableEntries,
				NO_PIB_INDEX,
				&mac_sec_level_table_entries);
		mlme_set_conf_run_time = (uint8_t)true;
	}
	break;

	default:
         /*do nothing*/
		break;
	}
}

/*
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
static void usr_mlme_set_conf_run_time(uint8_t status, uint8_t PIBAttribute,
		uint8_t PIBAttributeIndex)
{
	if (status != (uint8_t)MAC_SUCCESS) {
		/* something went wrong; restart */
		(void)WPAN_MLME_ResetReq(true);
	} else {
		switch (PIBAttribute) {
		case macPANId:
		{
			/*
			 * Set the Coordinator Short Address of the scanned
			 * network.
			 * This is required in order to perform a proper sync
			 * before association.
			 */
			(void)WPAN_MLME_SetReq(macCoordShortAddress,
					NO_PIB_INDEX,
					&coordAddrSpec.Addr);
		}
		break;

		case macCoordShortAddress:
		{
            
		}
		break;

		case macDefaultKeySource:
		{
			uint8_t mac_sec_level_table_entries
				= DEFAULT_MAC_SEC_LVL_TABLE_ENTRIES;

			(void)WPAN_MLME_SetReq(macSecurityLevelTableEntries,
					NO_PIB_INDEX,
					&mac_sec_level_table_entries);
		}
		break;

		case macSecurityLevelTableEntries:
		{
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
			uint8_t mac_key_table_entries
				= DEFAULT_MAC_KEY_TABLE_ENTRIES;

			(void)WPAN_MLME_SetReq(macKeyTableEntries,
					NO_PIB_INDEX,
					&mac_key_table_entries);
		}
		break;

		case macKeyTableEntries:
		{
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
				DEV_DESC_HANDLE_IDX_0,     
				(uint8_t)true,    
				(uint8_t)false,   
				DEV_DESC_HANDLE_IDX_0,  
				(uint8_t)true,    
				(uint8_t)false, 
				DEV_DESC_HANDLE_IDX_0,   
				(uint8_t)true,  
				(uint8_t)false,  
				DEV_DESC_HANDLE_IDX_0,  
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
		{
			/*
			 * Sync with beacon frames from our coordinator.
			 * Use: bool WPAN_MLME_SyncReq(uint8_t LogicalChannel,
			 *                              uint8_t ChannelPage,
			 *                              bool TrackBeacon);
			 *
			 * This does not lead to an immediate reaction.
			 *
			 * In case we receive beacon frames from our coordinator
			 * including
			 * a beacon payload, this is indicated in the callback
			 * function
			 * usr_mlme_beacon_notify_ind().
			 *
			 * In case the device cannot find its coordinator or
			 * later
			 * looses
			 * synchronization with its parent, this is indicated in
			 * the
			 * callback function usr_mlme_sync_loss_ind().
			 */
			if (PIBAttributeIndex == INDEX_3 && device_state ==
					DEVICE_IDLE) {
				(void)WPAN_MLME_GetReq(phyCurrentPage, NO_PIB_INDEX);
			}
		}
		break;

		case macDeviceTableEntries:
		{
            Addr16_t coord_shrt_addr;
            coord_shrt_addr.m16 = COORD_SHORT_ADDR;

			uint8_t mac_dev_table[17] = {
				/* DeviceDescriptor */
				/* PANId */
				(uint8_t)DEFAULT_PAN_ID,
				(uint8_t)(DEFAULT_PAN_ID >> 8U),
				/* Device ShortAddress */
				coord_shrt_addr.m8[0],
				coord_shrt_addr.m8[1],
				/* Device ExtAddress */
				COORD_IEEE_ADDRESS[7],
				COORD_IEEE_ADDRESS[6],
				COORD_IEEE_ADDRESS[5],
				COORD_IEEE_ADDRESS[4],
				COORD_IEEE_ADDRESS[3],
				COORD_IEEE_ADDRESS[2],
				COORD_IEEE_ADDRESS[1],
				COORD_IEEE_ADDRESS[0],
				/* FrameCounter */
				0, 0, 0, 0,
				/* Exempt */
				0
			};

			(void)WPAN_MLME_SetReq(macDeviceTable,
					INDEX_0, /* Index: 0 */
					&mac_dev_table);
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
			(void)WPAN_MLME_GetReq(macKeyTable,(uint8_t)(deviceShortAddress - 1U));
		}
		break;

		default:
			/* undesired PIB attribute; restart */
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
		/* something went wrong; restart */
		(void)WPAN_MLME_ResetReq(true);
	} else {
		if ((bool)mlme_set_conf_run_time) {
			usr_mlme_set_conf_run_time(setConf.status, setConf.PIBAttribute,
					setConf.PIBAttributeIndex);
			return;
		} else {
			init_secuity_pib(setConf.PIBAttribute, setConf.PIBAttributeIndex);
		}
	}

	/* Keep compiler happy. */
	setConf.PIBAttributeIndex = setConf.PIBAttributeIndex;
}

#endif
