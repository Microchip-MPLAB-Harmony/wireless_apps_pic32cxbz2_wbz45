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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "config.h"
#include "application.h"
#include "mem_interface.h"
#include "config/default/peripheral/nvm/plib_nvm.h"
#include "component/nvm.h"

// *****************************************************************************
// *****************************************************************************
// Section: Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/**
*@brief This routine is a wrapper that initializes the flash/NVM.
*
*@param None
*
*@retval None
*/
void Flash_Initialize(void)
{
  NVM_Initialize();
}

/**
*@brief This routine writes the data into flash address.
*
*@param data  - pointer which points to the data to write
*       addr  - start flash address
*       rcount - number of bytes to write.
*        
*@retval status of write operation 0- success, 1- failure
*/
uint8_t Flash_WriteData(uint8_t *data, uint32_t addr, uint32_t rcount)
{
    bool written = false;
    int32_t count = (int32_t)rcount;
    
    // address must be 16 assigned
    addr &= 0xFFFFFFF0;

    count = (count%16 == 0) ? count:(count-count%16+16);
    
    while(count > 0)
    {
        /*If NVM is busy, then wait for NVM to be idle before writting the data */
        if (false == NVM_IsBusy())
        {
            written = NVM_QuadWordWrite((uint32_t *)data, addr);
            if (written != true)
            {
                return 1;
            }
            else
            {
                data += 16;
                addr += 16;
                count -= 16;
            }
        }
    }
    
    return 0;
}

/**
*@brief This routine reads the data from flash address .
*
*@param fastRead - option to read the data faster
*       buffer  - pointer where read data should be written
*       addr  - start flash address
*       count - number of bytes to read.
*        
*@retval status of read operation 0- success, otherwise - failure
*/
uint8_t Flash_ReadData(bool fastRead, uint8_t *buffer, uint32_t addr, uint32_t count)
{
  NVM_Read( (uint32_t *)buffer, count, addr );
  return 0;
}

/**
*@brief This routine erases the sector.
*
*@param
*       addr  - start flash address
*       pageCount - number of pages to erase.
*        
*@retval status of read operation 0- success, otherwise - failure
*/
uint8_t Flash_EraseSector(uint32_t addr, uint16_t pageCount)
{
    uint8_t erased = 0;
    
    if ((addr % NVM_PAGE_SIZE) != 0)
    {
        return 2;
    }
    
    while (pageCount != 0)
    {
      if (false == NVM_IsBusy())
      {
        erased = NVM_PageErase(addr);
        if (erased == false)
        {
            return 1;
        }
        pageCount--;
        addr += NVM_PAGE_SIZE;
      }
    }
    
    return 0;  
}

uint32_t Flash_ReadJedecId(void)
{
  return 0;
}

// *****************************************************************************
// *****************************************************************************
// Section: Global Defines and TypeDefs
// *****************************************************************************
// ************************************************************************

const IMG_MEM_INTERFACE flashInterface =
{
    Flash_Initialize,
    Flash_WriteData,
    Flash_ReadData,
    Flash_EraseSector,
    Flash_ReadJedecId
};

const SLOT_PARAMS slotsSram =
{
    0x00000000,
    {0xA0000, 0},
    true
};

const SLOT_PARAMS slots512kFlash[2] =
{
    {
        0x60000000,
        {0x38000, 0x70000},
        false
    },
    {
        0x60038000,
        {0x38000,0},
        false        
    }
};

const SLOT_PARAMS slots1MFlash[2] =
{
    {
        0x1000000,
        {0x7ffff, 0},
        true
    },
    {
        0x1800000,
        {0x7ffff,0},
        true        
    }
};

const SLOT_PARAMS slots2MFlash[2] =
{
    {
        0x60000000,
        {0xF0000,0},
        false
    },
    {
        0x600F0000,
        {0xF0000,0},
        false        
    }
};

const uint32_t devId2MFlash = 0x09C60053;
const uint32_t devId1MFlash = 0x09C40053;
const uint32_t devIdEon512kFlash = 0x09C30053;
const uint32_t devIdSST512kFlash = 0x09C20053;

const IMG_MEM_TOPOLOGY imgMems[IMG_MEM_TOPOLOGY_COUNT] =
{
    // 1 MB Flashes
    {
        .u16ErasePageSz = NVM_PAGE_SIZE,
        .u16ProgRowSz = NVM_ROW_SIZE,
        .u32UmmAddrStart = 0x1000000,
        .u32TotSize = 0x100000,
        .u8SlotCount = 2,
        .pSlots = &slots1MFlash[0],
        .u32CalIdx = 0xF00000,
        .ifFlash = &flashInterface,
        .u8DevIdCount = 1,
        .pDevIds = &devId1MFlash,
        .u32DevIdMask = 0xFFFF0FFE,
        .u32AddrPosMask = 0x00000000,
        .u32AddrNegMask = 0x0FFFFFFF,
        .u32BlankCheck = 0xFFFFFFFF                
    },
};

typedef enum
{
    AUTH_METHOD_NONE = 0,
    AUTH_METHOD_CRC16 = 1,
    AUTH_METHOD_ECDSA_P256 = 2,
    AUTH_METHOD_ECDSA_P384 = 3,
    AUTH_METHOD_MAX = 4
} AUTH_METHODS;

typedef enum
{
    DEC_METHOD_NONE = 0
} DEC_METHODS;

typedef struct
{
    uint8_t             mdAuthKeyMethodCount;
    const uint8_t *     mdAuthKeyMethods;
    uint8_t             mdAuthKeyIndexCount;    
    const uint8_t *     mdAuthKeyIndicies;

    uint8_t             mdDecKeyMethodCount;
    const uint8_t *     mdDecKeyMethods;
    uint8_t             mdDecKeyIndexCount;    
    const uint8_t *     mdDecKeyIndicies;

    uint8_t             fwAuthKeyMethodCount;
    const uint8_t *     fwAuthKeyMethods;
    uint8_t             fwAuthKeyIndexCount;    
    const uint8_t *     fwAuthKeyIndicies;

    uint8_t             fwDecKeyMethodCount;
    const uint8_t *     fwDecKeyMethods;
    uint8_t             fwDecKeyIndexCount;    
    const uint8_t *     fwDecKeyIndicies;
} KEYS_SUPPORTED;

const uint8_t validAuthMethods = (uint8_t)AUTH_METHOD_ECDSA_P384;
const uint8_t validDecMethods = (uint8_t)DEC_METHOD_NONE;
const uint8_t validKeyIndex = 0;

const KEYS_SUPPORTED validKeyTypes =
{
    .mdAuthKeyMethodCount  = 1,
    .mdAuthKeyMethods = &validAuthMethods,
    .mdAuthKeyIndexCount = 1,
    .mdAuthKeyIndicies = &validKeyIndex,

    .mdDecKeyMethodCount = 1,
    .mdDecKeyMethods = &validDecMethods,
    .mdDecKeyIndexCount = 1,
    .mdDecKeyIndicies = &validKeyIndex,

    .fwAuthKeyMethodCount  = 1,
    .fwAuthKeyMethods = &validAuthMethods,
    .fwAuthKeyIndexCount = 1,
    .fwAuthKeyIndicies = &validKeyIndex,

    .fwDecKeyMethodCount = 1,
    .fwDecKeyMethods = &validDecMethods,
    .fwDecKeyIndexCount = 1,
    .fwDecKeyIndicies = &validKeyIndex,
};


const void * GetTopologies(void)
{
    return &imgMems[0];
}

const void * GetKeysSupported(void)
{
    return &validKeyTypes;
}

