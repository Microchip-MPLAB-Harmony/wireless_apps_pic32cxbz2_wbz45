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

// DOM-IGNORE-BEGIN
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEM_INTERFACE_H
#define _MEM_INTERFACE_H
//DOM-IGNORE-END

/** Maximum memory topologies on the device (one SRAM and one Flash)) */
#define MAX_MEM_TOPOLOGIES                  2
/** The maximum possible number of slots (one SRAM slot and 2 Flash slots)) */
#define MAX_SLOTS                           3
/** The size of the reserved header block */
#define HEADER_SIZE                         512


/** Identifies the possible authentication statuses */
typedef enum 
{
    AUTH_STATUS_BUSY = 0x530839fa,
    AUTH_STATUS_FAILED = 0xe97d40ce,
    AUTH_STATUS_SUCCESS = 0x0ac60ce4
} AUTH_STATUS;

/** Defines the firmware image header information used in bootloader */
typedef struct
{
	// Meta Data Sequence Number.
    uint32_t        MD_SEQ_NUM;
	// Meta Data Revision.
    uint8_t         MD_REV;
	// Meta Data Container Index.
    uint8_t         MD_CONT_IDX;
	// Meta Data Payload Length.
    uint16_t        MD_PL_LEN;
	// Meta Data Coherence.
    uint32_t        MD_COHERENCE;
	// Meta Data Authentication Method.
    uint8_t         MD_AUTH_METHOD;
	// Meta Data Authentication Key.
    uint8_t         MD_AUTH_KEY;
	// Meta Data Decryption Method.
    uint8_t         MD_DEC_METHOD;
	// Meta Data Decryption Key.
    uint8_t         MD_DEC_KEY;

	// Firmware Image Revision.
    uint32_t        FW_IMG_REV;
	// Firmware Image Source Address.
    uint32_t        FW_IMG_SRC_ADDR;
	// Firmware Image Destionation Address.
    uint32_t        FW_IMG_DST_ADDR;
	// Firmware Image Image Length.
    uint32_t        FW_IMG_LEN;
	// Firmware Image Authentication Method.
    uint8_t         FW_IMG_AUTH_METHOD;
	// Firmware Image Authentication Key.
    uint8_t         FW_IMG_AUTH_KEY;
	// Firmware Image Decryption Method.
    uint8_t         FW_IMG_DEC_METHOD;
	// Firmware Image Decryption Key.
    uint8_t         FW_IMG_DEC_KEY;
	// Firmware Image Signature.
    uint8_t         FW_IMG_SIG[96];

	// PAT Image Source Address.
    uint32_t        PAT_IMG_SRC_ADDR;
	// PAT Image Destination Address.
    uint32_t        PAT_IMG_DST_ADDR;
	// PAT Image Length.
    uint32_t        PAT_IMG_LEN;
	// PAT Image Signature.
    uint8_t         PAT_IMG_SIG[96];

	// MetaData Signature.
    uint8_t         MD_SIG[96];
} FW_IMG_HDR;


/** Defines the slot information */
typedef struct
{
    // Header Offset
    uint32_t        hdrOffset;
    // Slot Size
    uint32_t        slotSize[2];
    // Executable flag
    bool            executable;
}SLOT_PARAMS;

/** function pointer inits the image memory space */
typedef void (*IMG_MEM_INITIALIZE)(void);

/** function pointer gets called for writing into the image memory space with the given data */
typedef uint8_t (*IMG_MEM_WRITE)(uint8_t * data, uint32_t addr, uint32_t bytes);

/** function pointer gets called for reading from the image memory space from the given address */
typedef uint8_t (*IMG_MEM_READ)(bool fastRead, uint8_t * buffer, uint32_t addr, uint32_t bytes);

/** function pointer gets called for erasing image memory space with the given number of pages */
typedef uint8_t (*IMG_MEM_ERASE)(uint32_t addr, uint16_t pageCount);

/** function pointer gets called for reading the JEDEC ID from Image memory */
typedef uint32_t (*IMG_MEM_READ_JEDEC_ID)(void);

/** Defines the image memory interface functions for initialization, read, write, erase and read id */
typedef struct
{
    //Memory Initialize
    IMG_MEM_INITIALIZE          fInit;
    // Memory Write
    IMG_MEM_WRITE               fWrite;
    // Memory Read
    IMG_MEM_READ                fRead;
    // Memory Erase
    IMG_MEM_ERASE               fErase;
    // Memory Read JEDEC Identifier
    IMG_MEM_READ_JEDEC_ID       fReadId;
} IMG_MEM_INTERFACE;

/** Defines the image memory interface topology defintions */
typedef struct
{
    uint16_t                    u16ErasePageSz;     // Erase page size
    uint16_t                    u16ProgRowSz;       // Programming row size
    uint32_t                    u32UmmAddrStart;    // Unified memory model address start
    uint32_t                    u32TotSize;         // Total flash size
    uint8_t                     u8SlotCount;        // Count of slots in memory
    const SLOT_PARAMS *         pSlots;             // Pointer to array of slot structs
    uint32_t                    u32CalIdx;          // Index of calibration data
    const IMG_MEM_INTERFACE *   ifFlash;            // Flash interface
    uint8_t                     u8DevIdCount;       // Count of valid device IDs
    const uint32_t *            pDevIds;            // Pointer to array of device IDs
    uint32_t                    u32DevIdMask;       // Device ID negative mask
    uint32_t                    u32AddrPosMask;     // UMM address positive mask
    uint32_t                    u32AddrNegMask;     // UMM addr negative mask
    uint32_t                    u32BlankCheck;     // UMM addr negative mask    
}IMG_MEM_TOPOLOGY;


/** Defines the valid slot information like header,slot number,etc */
typedef struct
{
    // Firmware Image Header
    FW_IMG_HDR *                pHdr;
    // Slot Information
    const SLOT_PARAMS *         pSlot;
    // Topology Information
    const IMG_MEM_TOPOLOGY *    pTop;
} VALID_SLOT;


/** Defines the device context information ie., valid tops and valid slots */
typedef struct
{
    // Valid Toplogies
    const IMG_MEM_TOPOLOGY *     validTops[MAX_MEM_TOPOLOGIES];
    // Valid slots information
    VALID_SLOT                   validSlots[MAX_SLOTS];
    // Number of Topologies
    uint8_t topologyCount;
    // Number of slots
    uint8_t slotCount;
} DEVICE_CONTEXT;

// *****************************************************************************
/**
*@brief This function populate the DEVICE_CONTEXT structure with the valid topologies for the 
* device and the count of valid topologies
*
*@param ctx - Device context
*
*@retval None
*/
void IMG_MEM_FindValidTopologies(DEVICE_CONTEXT * ctx);

// *****************************************************************************
/**
*@brief This function cache, validate, and sort headers from all firmware image slots for valid topologies
*       Prerequisite: ctx initialized with FindValidTopologies
*
*@param ctx - A device context structure
*       buffer - A buffer for cached headers (HEADER_SIZE * MAX_SLOTS bytes)
*
*@retval None
*/
void IMG_MEM_CacheAndValidateHeaders(DEVICE_CONTEXT * ctx, uint8_t * buffer);

// *****************************************************************************
/**
*@brief This function cache a header from a firmware image slot
*
*@param ctx - A context structure
*       top - Pointer to an image memory topology
*       pSlot - The image header slot to cache the header from
*       buffer - Pointer to a buffer in which to cache the header
*       bufSlot - Offset in the buffer in HEADER_SIZE units. If a header is cached
*                  the contents of this pointer will be incremented by 1.
*
*@retval FW_IMG_HDR*  Firmware image header pointer
*/
FW_IMG_HDR* IMG_MEM_CacheHeader(DEVICE_CONTEXT * ctx, 
    const IMG_MEM_TOPOLOGY * top, const SLOT_PARAMS * slot, 
    uint8_t * buffer, uint32_t * bufSlot);

// *****************************************************************************
/**
*@brief This function validate a header
*       Criteria - Metadata revision, Sequence number (not 0 or 0xFFFFFFFF), rollback 
*              counter, image len % 4096 != 0, firmware and header auth method 
*              is ECDSA on P-384, encryption methods are None, Firmware image 
*              header is 0xE0 bytes, metadata container index is 1
*
*@param ctx - A device context structure
*       fwHdr - Firmware image header pointer
*       top - Pointer to an image memory topology
*
*@retval FW_IMG_HDR* Firmware image header pointer
*/
bool IMG_MEM_ValidateHeader(DEVICE_CONTEXT * ctx, FW_IMG_HDR * fwHdr);

// *****************************************************************************
/**
*@brief This function returns a topology based on a specific address
*       Prerequisite: ctx initialized with FindValidTopologies
*
*@param ctx - A device context structure
*       address - address to get topology
*       top - Pointer to an image memory topology
*
*@retval IMG_MEM_TOPOLOGY* topology pointer or NULL
*/
IMG_MEM_TOPOLOGY* IMG_MEM_GetTopologyByAddress (DEVICE_CONTEXT * ctx, 
        uint32_t address);

// *****************************************************************************
/**
*@brief This function sorts valid slots by priority
*
*@param ctx - A device context structure with cached and validated header pointers
*          in the header slots
*
*@retval None
*/
void IMG_MEM_SlotSort(DEVICE_CONTEXT * ctx);

// *****************************************************************************
/**
*@brief This function begin a header authentication operation. Note: This function must
*       not be called while the Public Key engine is in use
*
*@param digest - 48-byte buffer for the hash digest (maintain until authentication done)
*      ctx - A hash context (this must persist )
*      hdr - Pointer to a firmware image header
*      x - The x term of an ECDSA P-384 public key
*      y - The y term of an ECDSA P-384 public key
*
*@retval status of the authentication start
*/
bool IMG_MEM_AuthenticateHeaderStart(uint8_t * digest, FW_IMG_HDR * hdr, uint8_t * x, uint8_t * y);

// *****************************************************************************
/**
*@brief This function check the status of a header authentication operation
*       Prerequisite: Header authentication started with IMG_MEM_AuthenticateHeaderStart
*
*@param None
*
*@retval AUTH_STATUS Success, failure, or busy (no result available)
*/
AUTH_STATUS IMG_MEM_AuthenticateHeaderStatusGet(void);

#endif
