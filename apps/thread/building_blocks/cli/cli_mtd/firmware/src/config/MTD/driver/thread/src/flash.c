/*
 *  Copyright (c) 2023, The OpenThread Authors.
 *  All rights reserved.
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

/**
 * @file
 *   This file implements the OpenThread platform abstraction for the non-volatile storage.
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


#include <stdio.h>
#include <string.h>

#include <openthread/instance.h>

#include "definitions.h"


#if OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE  // Note: Not Supported by MCHP Thread SDK

#define OT_FLASH_BASE_ADDRESS ((uint32_t)&__d_nv_mem_start)
#define OT_FLASH_PAGE_SIZE 0x1000

/*
 * This value should not exceed:
 *     (((uint32_t)&__d_nv_mem_end - (uint32_t)&__d_nv_mem_start) / OT_FLASH_PAGE_SIZE)
 *
 * __d_nv_mem_start and __d_nv_mem_end is defined in linker script.
 * The size of NVRAM region is 4k. Page size is 256 bytes. Maximum OT_FLASH_PAGE_NUM
 * should be equal or less than 16.
 *
 */
#define OT_FLASH_PAGE_NUM 2

#define OT_FLASH_SWAP_SIZE (OT_FLASH_PAGE_SIZE * (OT_FLASH_PAGE_NUM / 2))

extern uint32_t __d_nv_mem_start;
extern uint32_t __d_nv_mem_end;

static uint32_t mapAddress(uint8_t aSwapIndex, uint32_t aOffset)
{
    uint32_t address = OT_FLASH_BASE_ADDRESS + aOffset;

    if (aSwapIndex)
    {
        address += OT_FLASH_SWAP_SIZE;
    }

    return address;
}

void otPlatFlashInit(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

}

uint32_t otPlatFlashGetSwapSize(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

}

void otPlatFlashErase(otInstance *aInstance, uint8_t aSwapIndex)
{
    OT_UNUSED_VARIABLE(aInstance);

}

void otPlatFlashWrite(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, const void *aData, uint32_t aSize)
{

    OT_UNUSED_VARIABLE(aInstance);
}

void otPlatFlashRead(otInstance *aInstance, uint8_t aSwapIndex, uint32_t aOffset, void *aData, uint32_t aSize)
{
    OT_UNUSED_VARIABLE(aInstance);

}

#else
#include "platform-pic32cx.h"
#define SETTINGS_BUFFER_SIZE 1024
#include <settings.h>
#include <openthread/dataset.h>
#include "flash_config.h"




/* Buffer for storing items before writing to NVM */
uint8_t sSettingsBuf[SETTINGS_BUFFER_SIZE];

/* Starting buffer index. Used for calculating offset of Items */
#define SETTINGSBUF_STARTINDX                  0


/******************************************************************************************************************
 *                  Definitions of Item Offset in relation to sSettingsBuf                                         *
 ******************************************************************************************************************/
#define PDS_ITEM_ACTIVE_DATASET_OFFSET         SETTINGSBUF_STARTINDX
#define PDS_ITEM_PENDING_DATASET_OFFSET        PDS_ITEM_ACTIVE_DATASET_OFFSET + PDS_ITEM_ACTIVE_DATASET_SZ
#define PDS_ITEM_NETWORK_INFO_OFFSET           PDS_ITEM_PENDING_DATASET_OFFSET + PDS_ITEM_PENDING_DATASET_SZ
#define PDS_ITEM_PARENT_INFO_OFFSET            PDS_ITEM_NETWORK_INFO_OFFSET + PDS_ITEM_NETWORK_INFO_SZ
#define PDS_ITEM_CHILD_INFO_OFFSET             PDS_ITEM_PARENT_INFO_OFFSET + PDS_ITEM_PARENT_INFO_SZ
#define PDS_ITEM_SLAAC_IID_SECRET_KEY_OFFSET   PDS_ITEM_CHILD_INFO_OFFSET + (PDS_ITEM_CHILD_INFO_SZ * OPENTHREAD_CONFIG_MLE_MAX_CHILDREN)
#define PDS_ITEM_DAD_INFO_OFFSET               PDS_ITEM_SLAAC_IID_SECRET_KEY_OFFSET + PDS_ITEM_SLAAC_IID_SECRET_KEY_SZ 
#define PDS_ITEM_SRP_ECDSA_KEY_OFFSET          PDS_ITEM_DAD_INFO_OFFSET + PDS_ITEM_DAD_INFO_SZ
#define PDS_ITEM_SRP_CLIENT_INFO_OFFSET        PDS_ITEM_SRP_ECDSA_KEY_OFFSET + PDS_ITEM_SRP_ECDSA_KEY_SZ
#define PDS_ITEM_SRP_SERVER_INFO_OFFSET        PDS_ITEM_SRP_CLIENT_INFO_OFFSET + PDS_ITEM_SRP_CLIENT_INFO_SZ
#define PDS_ITEM_BR_ULA_PREFIX_OFFSET          PDS_ITEM_SRP_SERVER_INFO_OFFSET + PDS_ITEM_SRP_SERVER_INFO_SZ
#define PDS_ITEM_BR_ON_LINK_PREFIXES_OFFSET    PDS_ITEM_BR_ULA_PREFIX_OFFSET + PDS_ITEM_BR_ULA_PREFIX_SZ

/******************************************************************************************************************
 *                Definitions of Item Address in relation to sSettingsBuf                                          *
 ******************************************************************************************************************/
#define PDS_ITEM_ACTIVE_DATASET_ADDR              &sSettingsBuf[PDS_ITEM_ACTIVE_DATASET_OFFSET]
#define PDS_ITEM_PENDING_DATASET_ADDR             &sSettingsBuf[PDS_ITEM_PENDING_DATASET_OFFSET]
#define PDS_ITEM_NETWORK_INFO_ADDR                &sSettingsBuf[PDS_ITEM_NETWORK_INFO_OFFSET]
#define PDS_ITEM_PARENT_INFO_ADDR                 &sSettingsBuf[PDS_ITEM_PARENT_INFO_OFFSET]
#define PDS_ITEM_CHILD_INFO_ADDR                  &sSettingsBuf[PDS_ITEM_CHILD_INFO_OFFSET]
#define PDS_ITEM_SLAAC_IID_SECRET_KEY_ADDR        &sSettingsBuf[PDS_ITEM_SLAAC_IID_SECRET_KEY_OFFSET]
#define PDS_ITEM_DAD_INFO_ADDR                    &sSettingsBuf[PDS_ITEM_DAD_INFO_OFFSET]
#define PDS_ITEM_SRP_ECDSA_KEY_ADDR               &sSettingsBuf[PDS_ITEM_SRP_ECDSA_KEY_OFFSET]
#define PDS_ITEM_SRP_CLIENT_INFO_ADDR             &sSettingsBuf[PDS_ITEM_SRP_CLIENT_INFO_OFFSET]
#define PDS_ITEM_SRP_SERVER_INFO_ADDR             &sSettingsBuf[PDS_ITEM_SRP_SERVER_INFO_OFFSET]
#define PDS_ITEM_BR_ULA_PREFIX_ADDR               &sSettingsBuf[PDS_ITEM_BR_ULA_PREFIX_OFFSET]
#define PDS_ITEM_BR_ON_LINK_PREFIXES_ADDR         &sSettingsBuf[PDS_ITEM_BR_ON_LINK_PREFIXES_OFFSET]

/*************************************************************************************************************
  *                  Files definitions																		*
 *************************************************************************************************************/
PDS_DECLARE_ITEM(PDS_ITEM_ACTIVE_DATASET, PDS_ITEM_ACTIVE_DATASET_SZ, PDS_ITEM_ACTIVE_DATASET_ADDR, (void *) NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_PENDING_DATASET, PDS_ITEM_PENDING_DATASET_SZ, PDS_ITEM_PENDING_DATASET_ADDR, (void *) NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_NETWORK_INFO, PDS_ITEM_NETWORK_INFO_SZ, PDS_ITEM_NETWORK_INFO_ADDR, (void *) NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_PARENT_INFO, PDS_ITEM_PARENT_INFO_SZ, PDS_ITEM_PARENT_INFO_ADDR, (void *) NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_CHILD_INFO, (PDS_ITEM_CHILD_INFO_SZ * OPENTHREAD_CONFIG_MLE_MAX_CHILDREN), PDS_ITEM_CHILD_INFO_ADDR, (void *)NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_SLAAC_IID_SECRET_KEY, PDS_ITEM_SLAAC_IID_SECRET_KEY_SZ, PDS_ITEM_SLAAC_IID_SECRET_KEY_ADDR, (void *)NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_DAD_INFO, PDS_ITEM_DAD_INFO_SZ, PDS_ITEM_DAD_INFO_ADDR, (void *)NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_SRP_ECDSA_KEY, PDS_ITEM_SRP_ECDSA_KEY_SZ, PDS_ITEM_SRP_ECDSA_KEY_ADDR, (void *)NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_SRP_CLIENT_INFO, PDS_ITEM_SRP_CLIENT_INFO_SZ, PDS_ITEM_SRP_CLIENT_INFO_ADDR, (void *)NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_SRP_SERVER_INFO, PDS_ITEM_SRP_SERVER_INFO_SZ, PDS_ITEM_SRP_SERVER_INFO_ADDR, (void *)NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_BR_ULA_PREFIX, PDS_ITEM_BR_ULA_PREFIX_SZ, PDS_ITEM_BR_ULA_PREFIX_ADDR, (void *)NULL, 0x00);
PDS_DECLARE_ITEM(PDS_ITEM_BR_ON_LINK_PREFIXES, PDS_ITEM_BR_ON_LINK_PREFIXES_SZ, PDS_ITEM_BR_ON_LINK_PREFIXES_ADDR, (void *)NULL, 0x00);

/******************************************************************************************************************/
/* Maximum number of Buffer Index*/
#define NO_OF_BUFFER_ITEM_INDEX         13

/* Buffer Item Index stores the address of each item in relation to sSettingsBuf */
static uint16_t sSettingsBufItemIndex [NO_OF_BUFFER_ITEM_INDEX];

/* Number of Child Table entries registered */
static uint8_t sChildInfoCount;

// *****************************************************************************
/**
*@brief This routine Restores all Items from Persistent Data Server
*
*@param None
*
*@retval None
*/

void flashRestoreItemsAll(void) {
    if (PDS_IsAbleToRestore(PDS_ITEM_ACTIVE_DATASET)) {
        PDS_Restore(PDS_ITEM_ACTIVE_DATASET);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_PENDING_DATASET)) {
        PDS_Restore(PDS_ITEM_PENDING_DATASET);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_NETWORK_INFO)) {
        PDS_Restore(PDS_ITEM_NETWORK_INFO);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_PARENT_INFO)) {
        PDS_Restore(PDS_ITEM_PARENT_INFO);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_CHILD_INFO)) {
        PDS_Restore(PDS_ITEM_CHILD_INFO);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_SLAAC_IID_SECRET_KEY)) {
        PDS_Restore(PDS_ITEM_SLAAC_IID_SECRET_KEY);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_DAD_INFO)) {
        PDS_Restore(PDS_ITEM_DAD_INFO);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_SRP_ECDSA_KEY)) {
        PDS_Restore(PDS_ITEM_SRP_ECDSA_KEY);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_SRP_CLIENT_INFO)) {
        PDS_Restore(PDS_ITEM_SRP_CLIENT_INFO);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_SRP_SERVER_INFO)) {
        PDS_Restore(PDS_ITEM_SRP_SERVER_INFO);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_BR_ULA_PREFIX)) {
        PDS_Restore(PDS_ITEM_BR_ULA_PREFIX);
    }
    if (PDS_IsAbleToRestore(PDS_ITEM_BR_ON_LINK_PREFIXES)) {
        PDS_Restore(PDS_ITEM_BR_ON_LINK_PREFIXES);
    }
}

/* settings APIs */

void otPlatSettingsInit(otInstance *aInstance) {
    OT_UNUSED_VARIABLE(aInstance);

    /* Initialize sSettingsBuf */
    memset(sSettingsBuf, 0xFF, sizeof (sSettingsBuf));

    sChildInfoCount = 0;

    /* Initialize address index value for each item */

    sSettingsBufItemIndex[PDS_ITEM_ACTIVE_DATASET - PDS_MODULE_OT_OFFSET] = PDS_ITEM_ACTIVE_DATASET_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_PENDING_DATASET - PDS_MODULE_OT_OFFSET] = PDS_ITEM_PENDING_DATASET_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_NETWORK_INFO - PDS_MODULE_OT_OFFSET] = PDS_ITEM_NETWORK_INFO_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_PARENT_INFO - PDS_MODULE_OT_OFFSET] = PDS_ITEM_PARENT_INFO_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_CHILD_INFO - PDS_MODULE_OT_OFFSET] = PDS_ITEM_CHILD_INFO_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_SLAAC_IID_SECRET_KEY - PDS_MODULE_OT_OFFSET] = PDS_ITEM_SLAAC_IID_SECRET_KEY_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_DAD_INFO - PDS_MODULE_OT_OFFSET] = PDS_ITEM_DAD_INFO_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_SRP_ECDSA_KEY - PDS_MODULE_OT_OFFSET] = PDS_ITEM_SRP_ECDSA_KEY_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_SRP_CLIENT_INFO - PDS_MODULE_OT_OFFSET] = PDS_ITEM_SRP_CLIENT_INFO_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_SRP_SERVER_INFO - PDS_MODULE_OT_OFFSET] = PDS_ITEM_SRP_SERVER_INFO_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_BR_ULA_PREFIX - PDS_MODULE_OT_OFFSET] = PDS_ITEM_BR_ULA_PREFIX_OFFSET;
    sSettingsBufItemIndex[PDS_ITEM_BR_ON_LINK_PREFIXES - PDS_MODULE_OT_OFFSET] = PDS_ITEM_BR_ON_LINK_PREFIXES_OFFSET;

    /* Restore all Items and store in sSettingsBuf */
    flashRestoreItemsAll();
    
    /* After restoring from PDS, check the valid child info entries in PDS and update the ChildInfoCount*/
    for (uint8_t i = 0; i < OPENTHREAD_CONFIG_MLE_MAX_CHILDREN; i++) {
        if (0xFF != (sSettingsBuf[ PDS_ITEM_CHILD_INFO_OFFSET + (i * PDS_ITEM_CHILD_INFO_SZ)])) {
            sChildInfoCount++;
        }
    }

}

void otPlatSettingsDeinit(otInstance *aInstance) {
    OT_UNUSED_VARIABLE(aInstance);

    memset(sSettingsBuf, 0xFF, sizeof (sSettingsBuf));

    sChildInfoCount = 0;
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength) {
    OT_UNUSED_VARIABLE(aInstance);
    otError error = OT_ERROR_NOT_FOUND;
    //flashItemId_t itemId = (flashItemId_t)(aKey - 1);
    pdsItemID_t itemId = getPdsItemidfromKeyid(aKey);
    uint16_t index = getOffsetfromItemid(itemId);
    
    if (!((itemId >= PDS_ITEM_MAX) || (aValue == NULL) || (aValueLength == 0))) 
    {
        if (itemId == PDS_ITEM_CHILD_INFO) {
            //Read from sSettingsBuf with Key and Index and Return.
            if (0xFF != (sSettingsBuf[index + (aIndex * PDS_ITEM_CHILD_INFO_SZ)])) {
                memcpy(aValueLength, &sSettingsBuf[index + (aIndex * PDS_ITEM_CHILD_INFO_SZ)], FILE_ITEM_LEN_SZ);
                memcpy(aValue, &(sSettingsBuf[index + (aIndex * PDS_ITEM_CHILD_INFO_SZ) + FILE_ITEM_LEN_SZ]), *aValueLength);
                error = OT_ERROR_NONE;
            }
        } 
        else 
        {
            //Read from sSettingsBuf with Key and Return.
            if (0xFF != sSettingsBuf[index]) {
                memcpy(aValueLength, &sSettingsBuf[index], FILE_ITEM_LEN_SZ);
                memcpy(aValue,  &sSettingsBuf[index + FILE_ITEM_LEN_SZ], *aValueLength);
                error = OT_ERROR_NONE;
            }
        }
    }
    return error;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength) {
    OT_UNUSED_VARIABLE(aInstance);
    //flashItemId_t itemId = (flashItemId_t)(aKey - 1);
    pdsItemID_t itemId = getPdsItemidfromKeyid(aKey);
    otError error = OT_ERROR_NOT_FOUND;
    uint16_t index = getOffsetfromItemid(itemId);
    
    if (!((itemId >= PDS_ITEM_MAX) || (aValue == NULL) || (aValueLength == 0)))
    {
        if (itemId == PDS_ITEM_CHILD_INFO) {
            memcpy( &sSettingsBuf[index + (sChildInfoCount * PDS_ITEM_CHILD_INFO_SZ)], &aValueLength, FILE_ITEM_LEN_SZ);
            memcpy( &(sSettingsBuf[index + (sChildInfoCount * PDS_ITEM_CHILD_INFO_SZ) + FILE_ITEM_LEN_SZ] ), aValue, aValueLength);
            PDS_Operation_Offset_t offset;
            offset.id = (PDS_ITEM_CHILD_INFO);
            offset.size = PDS_ITEM_CHILD_INFO_SZ;
            offset.offset = (sChildInfoCount * PDS_ITEM_CHILD_INFO_SZ);
            offset.ramAddr = &sSettingsBuf[index + (sChildInfoCount * PDS_ITEM_CHILD_INFO_SZ)];

            PDS_storeOffset(&offset);

            sChildInfoCount++;
        } else
        {
            memcpy(&sSettingsBuf[index], &aValueLength, FILE_ITEM_LEN_SZ);
            memcpy(&(sSettingsBuf[index + FILE_ITEM_LEN_SZ]), aValue, aValueLength);
            PDS_Store((itemId));
        }
        error = OT_ERROR_NONE;
    }

    return error;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength) {
    OT_UNUSED_VARIABLE(aInstance);

    return otPlatSettingsAdd(aInstance, aKey, aValue, aValueLength);
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex) {
    OT_UNUSED_VARIABLE(aInstance);
    //flashItemId_t itemId = (flashItemId_t)(aKey - 1);
    pdsItemID_t itemId = getPdsItemidfromKeyid(aKey);
    otError error = OT_ERROR_NOT_FOUND;
    uint16_t index = getOffsetfromItemid(itemId);
    uint8_t item_index = (uint8_t)(itemId - PDS_MODULE_OT_OFFSET);
    
    if (aIndex == -1) {
        // Call PDS Delete with aKey
        PDS_Delete((itemId));
        memset(&sSettingsBuf[index], 0xFF, item_size[item_index]);
        error = OT_ERROR_NONE;
    }
    else if (itemId == PDS_ITEM_CHILD_INFO) {
        /* Store 0xFF for the specific Index. PDS component doesn't support delete with offset.
         * So to mitigate this, using store offset with value of 0xFF.
         * During restore, for this specific Index, 0xFF will be copied to sSettingsBuf.
         * Then in reading this index, we'll return Record not found to Stack.
         */
        memset(&sSettingsBuf[index + (aIndex * PDS_ITEM_CHILD_INFO_SZ)], 0xFF, PDS_ITEM_CHILD_INFO_SZ);
        PDS_Operation_Offset_t offset;
        offset.id = (PDS_ITEM_CHILD_INFO);
        offset.size = PDS_ITEM_CHILD_INFO_SZ;
        offset.offset = (aIndex * PDS_ITEM_CHILD_INFO_SZ);
        offset.ramAddr = &sSettingsBuf[(index + (aIndex * PDS_ITEM_CHILD_INFO_SZ))];

        PDS_storeOffset(&offset);
        sChildInfoCount--;
        error = OT_ERROR_NONE;
    }
    return error;
}

void otPlatSettingsWipe(otInstance *aInstance) {
    otPlatSettingsInit(aInstance);
    // Call PDS Delete All
    PDS_DeleteAll(false);
}

pdsItemID_t getPdsItemidfromKeyid (uint16_t key)
{
    pdsItemID_t itemId = PDS_ITEM_MAX;

// Based on the below settings value defined in OT stack, corresponding PDS ITEM id is assigned.
//    OT_SETTINGS_KEY_ACTIVE_DATASET       = 0x0001, ///< Active Operational Dataset.
//    OT_SETTINGS_KEY_PENDING_DATASET      = 0x0002, ///< Pending Operational Dataset.
//    OT_SETTINGS_KEY_NETWORK_INFO         = 0x0003, ///< Thread network information.
//    OT_SETTINGS_KEY_PARENT_INFO          = 0x0004, ///< Parent information.
//    OT_SETTINGS_KEY_CHILD_INFO           = 0x0005, ///< Child information.
//    OT_SETTINGS_KEY_SLAAC_IID_SECRET_KEY = 0x0007, ///< SLAAC key to generate semantically opaque IID.
//    OT_SETTINGS_KEY_DAD_INFO             = 0x0008, ///< Duplicate Address Detection (DAD) information.
//    OT_SETTINGS_KEY_SRP_ECDSA_KEY        = 0x000b, ///< SRP client ECDSA public/private key pair.
//    OT_SETTINGS_KEY_SRP_CLIENT_INFO      = 0x000c, ///< The SRP client info (selected SRP server address).
//    OT_SETTINGS_KEY_SRP_SERVER_INFO      = 0x000d, ///< The SRP server info (UDP port).
//    OT_SETTINGS_KEY_BR_ULA_PREFIX        = 0x000f, ///< BR ULA prefix.
//    OT_SETTINGS_KEY_BR_ON_LINK_PREFIXES  = 0x0010, ///< BR local on-link prefixes.
//
//    // Deprecated and reserved key values:
//    //
//    //   0x0006  previously auto-start.
//    //   0x0009  previously OMR prefix.
//    //   0x000a  previously on-link prefix.
//    //   0x000e  previously NAT64 prefix.
//
//    // Keys in range 0x8000-0xffff are reserved for vendor-specific use.
//    OT_SETTINGS_KEY_VENDOR_RESERVED_MIN = 0x8000,
//    OT_SETTINGS_KEY_VENDOR_RESERVED_MAX = 0xffff,
   

    switch (key)
    {
        case 0x0001:
            itemId = PDS_ITEM_ACTIVE_DATASET;
            break;
        case 0x0002:
            itemId = PDS_ITEM_PENDING_DATASET;
            break;
        case 0x0003:
            itemId = PDS_ITEM_NETWORK_INFO;
            break;            
        case 0x0004:
            itemId = PDS_ITEM_PARENT_INFO;
            break;        
        case 0x0005:
            itemId = PDS_ITEM_CHILD_INFO;
            break;
        case 0x0007:
            itemId = PDS_ITEM_SLAAC_IID_SECRET_KEY;
            break;
        case 0x0008:
            itemId = PDS_ITEM_DAD_INFO;
            break;
        case 0x000b:
            itemId = PDS_ITEM_SRP_ECDSA_KEY;
            break;
        case 0x000c:
            itemId = PDS_ITEM_SRP_CLIENT_INFO;
            break;
        case 0x000d:
            itemId = PDS_ITEM_SRP_SERVER_INFO;
            break;
        case 0x000f:
            itemId = PDS_ITEM_BR_ULA_PREFIX;
            break;
        case 0x0010:
            itemId = PDS_ITEM_BR_ON_LINK_PREFIXES;
            break;          
        default:
            itemId = PDS_ITEM_MAX;
    }
    return itemId;
}

uint16_t getOffsetfromItemid (pdsItemID_t itemId)
{
    uint16_t offset = sSettingsBufItemIndex[itemId - PDS_MODULE_OT_OFFSET];
    return offset;
}
#endif


