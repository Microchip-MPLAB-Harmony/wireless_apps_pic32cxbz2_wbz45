/*********************************************************************
 *
 *                  DFU - Device Firmware Upgrade
 *
 *********************************************************************
 * FileName:        dfu.c
 * Dependencies:    None
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its Daytona/PIC24F Microcontroller is intended
 * and supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip Daytona/PIC24F Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * $ID$
 *
 ********************************************************************/
#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "dfu/include/progexec.h"
#include "dfu/include/uart.h"
#include "mem_interface.h"
#include "definitions.h"
#include "application.h"
/******************************************************************************
 Global Data
******************************************************************************/

#define GET_PE_COMMAND(buffer,len) UART_Read(buffer, len, PE_CMD_TIMEOUT)
#define SET_PE_RESPONSE(buffer,size) UART_Write(buffer, size)

static uint8_t vBuf[8096];

/* PE version number embedded in the binary code */
/* Not used in PE code, but keep it in hex file to be able to tell the version
 from the hex file */
const uint16_t PEVersion[] = {PROG_EXEC_VNUM_IDENTIFIER, 
        PROG_EXEC_VERSION_NUMBER};

/* other globals */
uint32_t row[ROW_SIZE] __attribute__ ((aligned (4)));

/* scratch buffer used for flash verification commands */
uint32_t *scratch_buffer = (uint32_t *)(PE_CMD_SRAM_ADDR + PE_CMD_MAX_HEADER);
uint32_t scratch_size = PE_CMD_PAYLOAD_SIZE;
uint32_t incmd;
FPP_CMD_PACKET Cmd;
/******************************************************************************
 Function Prototypes
******************************************************************************/
static uint16_t prog (const uint8_t *dataAddr, const uint32_t address, 
        const uint32_t len, IMG_MEM_WRITE pfWrite );     
static uint16_t getCRC(const uint8_t * addr, uint32_t len, 
        uint32_t * const hwCrc, IMG_MEM_READ pfRead );
static uint16_t getCsum(const uint8_t *addr, uint32_t len, 
        uint32_t * const csum, IMG_MEM_READ pfRead);
static const IMG_MEM_TOPOLOGY * getDeviceTop(const uint32_t startAddr, 
        const uint32_t endAddr, const IMG_MEM_TOPOLOGY ** tops, 
        const uint8_t count);
static uint32_t *read_from_scratch(const uint32_t addr, uint32_t * buf, const uint32_t size, 
        IMG_MEM_READ pfRead);
static uint32_t virt_to_phy(const uint32_t addr, const IMG_MEM_TOPOLOGY *top);

/* erase chip */
static uint16_t eraseChip(const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop); 
static uint16_t pageErase(const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop); 

/* program flash */
static uint16_t programClusterVerify (const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop); 

/* Verify blank check*/
static uint16_t blankCheck(const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop);

volatile uint8_t pe_command[PE_CMD_SIZE];
uint8_t pe_resp[PE_RESP_SIZE+20];

/******************************************************************************
 Run the Device Firmware Upgrade if valid topoligies found.
******************************************************************************/
void dfu(const IMG_MEM_TOPOLOGY ** tops, uint8_t count)
{
    if (count && tops)   
        program_exec_main(tops, count);
}

/******************************************************************************
 main
 This is the entry point for the programming executive. It receives commands
 from the host, dispatches the command, and then sends response back to 
 the host.
******************************************************************************/
int32_t program_exec_main(const IMG_MEM_TOPOLOGY ** tops, uint8_t count)
{
    uint16_t retval;
    volatile uint32_t *sram_cmd_addr = 0;
    const IMG_MEM_TOPOLOGY *pDeviceTop = 0;
    uint32_t respSize = 0;
    
	/* read command */        
	if (GET_PE_COMMAND((uint8_t *)pe_command, PE_CMD_SIZE) <= 0)
		return -1;

	sram_cmd_addr =  (uint32_t *)pe_command;
	incmd = *sram_cmd_addr++;

	/* read command and operand from command address */       
	Cmd.cmd = CMD( incmd );
	Cmd.operand = LEN( incmd );
	
	retval = FAIL_RESP;
	/* dispatch commands */   
	switch( Cmd.cmd )
	{ 
    case RESET_CMD:
        RCON_SoftwareReset();
        retval = PASS_RESP;
        break;
	case EXEC_VERSION_CMD:
		retval = PROG_EXEC_VERSION_NUMBER;   
		break;  
	case GET_DEVICEID:
		retval = PASS_RESP;
		break;
	case PAGE_ERASE_CMD:
		Cmd.erasePage.addr = (void *)*sram_cmd_addr; 
		pDeviceTop = getDeviceTop((uint32_t)Cmd.erasePage.addr, 
				(uint32_t)Cmd.erasePage.addr, tops, count);
		if (pDeviceTop)
		{
			retval = pageErase(&Cmd, pDeviceTop);   
		}
		break;
	case CHIP_ERASE_CMD:
		Cmd.eraseChip.addr = (void *)*sram_cmd_addr; 
		pDeviceTop = getDeviceTop((uint32_t)Cmd.eraseChip.addr, 
				(uint32_t)Cmd.eraseChip.addr, tops, count);
		if (pDeviceTop)
		{
			retval = eraseChip(&Cmd, pDeviceTop);   
		}
		break;
	case PROG_CLUSTER_VERIFY_CMD:
		Cmd.progClusterVerify.addr = (void *) *sram_cmd_addr++;
		Cmd.progClusterVerify.len = (uint32_t)*sram_cmd_addr++;
		Cmd.progClusterVerify.checksum = (uint32_t)*sram_cmd_addr++;   
		Cmd.progClusterVerify.payload = (void *)sram_cmd_addr;
		pDeviceTop = getDeviceTop((uint32_t)Cmd.progClusterVerify.addr,
				(uint32_t)(Cmd.progClusterVerify.addr+
				Cmd.progClusterVerify.len), tops, count);
		
		if (pDeviceTop)
		{
			retval = programClusterVerify(&Cmd, pDeviceTop);
		}
		break;            
	case BLANK_CHECK_CMD:
		Cmd.blanckCheck.addr = (void *) *sram_cmd_addr++;;
		Cmd.blanckCheck.len = (uint32_t) *sram_cmd_addr;
		pDeviceTop = getDeviceTop((uint32_t)Cmd.blanckCheck.addr, 
				(uint32_t)(Cmd.blanckCheck.addr+Cmd.blanckCheck.len), 
				tops, count);  
		if (pDeviceTop)
		{
			retval = blankCheck(&Cmd, pDeviceTop);
		}
		break;
	default:  
		retval = NACK_RESP;
		break;  

	} /* end of switch */
	respSize = PE_RESP_SIZE;      
	if( Cmd.cmd == EXEC_VERSION_CMD || Cmd.cmd == GET_DEVICEID )
	{
		*((volatile uint32_t *) pe_resp ) = 
				( ( (Cmd.cmd) << 16 ) | (uint16_t) retval );
		
		if ( Cmd.cmd == GET_DEVICEID )
		{
			uint32_t devID = 0;
#ifdef DEVID
			devID = DEVID; //PE_DEVICEID(DEVID); 
#endif 
			*( (volatile uint32_t *) pe_resp + 1 ) = devID; 
			respSize += 4;
		}            
	}
	else {
		/* send response for most commands */
		*( (volatile uint32_t *) pe_resp ) = ( (Cmd.cmd) << 16 ) | 
				( ( retval <= NACK_RESP ) ? retval : FAIL_RESP );
	}

	SET_PE_RESPONSE((int8_t *)pe_resp, respSize); 
   
   return 0;
   
} /* end of main */

/******************************************************************************
* Function: programClusterVerify
* This routine programs the specified number of bytes to the specified address. 
* The address must be 32-bit aligned, and the number of bytes must be a
* multiple of a 32-bit word.
*
* PreCondition:    none.
*
* Input:    pCmd  - Command data to program/verify in the device memory 
*           | CMD:Operand | Address | Length | Checksum | Payload[] | 
* 
*           pDeviceTop - Valid device topology based on command address.          
*
* Output:   PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   None
*
* Overview: None
* Note:     None
******************************************************************************/
static uint16_t programClusterVerify (const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop)
{
    uint8_t *dataAddr;      /* command data */
    uint32_t destAddr;         /* flash/sram address */
    uint32_t len;           /* data length to program */
    uint16_t checksumMethod;/* checksum method to use */
    uint32_t cmdChecksum;   /* checksum value from PC */ 
    uint32_t checksum = 0;  /* checksum value computed by PE */
    
    if (!pCmd)
        return FAIL_RESP;
    
    dataAddr = (uint8_t *)pCmd->progClusterVerify.payload;
    destAddr =  virt_to_phy((uint32_t)pCmd->progClusterVerify.addr, pDeviceTop);
    len = pCmd->progClusterVerify.len;
    checksumMethod = pCmd->operand;
    cmdChecksum = pCmd->progClusterVerify.checksum;    

    if (pDeviceTop->ifFlash && pDeviceTop->ifFlash->fInit)
        pDeviceTop->ifFlash->fInit();    
    
   if (PASS_RESP != prog(dataAddr, destAddr, len, 
           (pDeviceTop->ifFlash?pDeviceTop->ifFlash->fWrite:0)))
       return FAIL_RESP;
   
    if (checksumMethod == PE_CFG_CHECKSUM)
    {
        getCsum((uint8_t *)destAddr, len, &checksum, 
                (pDeviceTop->ifFlash?pDeviceTop->ifFlash->fRead:0));
    }
    else if (checksumMethod == PE_CFG_CRC)
    {
        getCRC((uint8_t *)destAddr, len, &checksum, 
                (pDeviceTop->ifFlash?pDeviceTop->ifFlash->fRead:0));
    }
    
    if (checksum != cmdChecksum)
       return FAIL_RESP;
       
   return PASS_RESP;
}

/******************************************************************************
* Function: eraseChip
* This function erases the entire flash memory.
*
* PreCondition:    none.
*
* Input:    pCmd  - Command data to program/verify in the device memory 
*           | CMD:Operand | Address |
* 
*           pDeviceTop - Valid device topology based on command address.          
*
* Output:   PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   None
*
* Overview: None
* Note:     None
******************************************************************************/
static uint16_t eraseChip(const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop)
{
    uint32_t addr;
    uint16_t pageCount;
    uint32_t len;
    uint16_t retval = PASS_RESP;
    
    if (!pCmd)
        return FAIL_RESP;
    
    addr = (uint32_t)pCmd->erasePage.addr;
    pageCount = (pDeviceTop->u32CalIdx / pDeviceTop->u16ErasePageSz);
    len = (pageCount * pDeviceTop->u16ErasePageSz);
    if ((pDeviceTop->u32UmmAddrStart + len) > (pDeviceTop->u32UmmAddrStart + pDeviceTop->u32CalIdx))
        return FAIL_RESP;

    if (pDeviceTop->ifFlash && pDeviceTop->ifFlash->fInit)
        pDeviceTop->ifFlash->fInit();
    
    addr =  virt_to_phy(pDeviceTop->u32UmmAddrStart, pDeviceTop);
    if (pDeviceTop->ifFlash && pDeviceTop->ifFlash->fErase)
    {
        retval = pDeviceTop->ifFlash->fErase(addr, pageCount);
    }
    
    return retval;
}

/******************************************************************************
* Function: blankCheck
* This function checks if specified length of flash memory is blank or not.
*
* PreCondition:    none.
*
* Input:    pCmd  - Command data to program/verify in the device memory 
*           | CMD:Operand | Address | Length | 
* 
*           pDeviceTop - Valid device topology based on command address.          
*
* Output:   PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   None
*
* Overview: None
* Note:     None
******************************************************************************/
static uint16_t blankCheck(const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop)
{
    uint32_t addr;
    uint32_t len;
    uint32_t *pBuf;
	uint32_t size;
    uint32_t u32BlankCheck;
    
    if (!pCmd && !pDeviceTop)
        return FAIL_RESP;
    
    if (pDeviceTop->ifFlash && pDeviceTop->ifFlash->fInit)
        pDeviceTop->ifFlash->fInit();
    
    addr = (uint32_t)pCmd->blanckCheck.addr;
    addr =  virt_to_phy(addr, pDeviceTop);
    len = pCmd->blanckCheck.len;
    u32BlankCheck = pDeviceTop->u32BlankCheck;    
    
    while (len) {
        
        if (len > scratch_size)
            size = scratch_size;
        else
            size = len;
        
        read_from_scratch((uint32_t)addr, (uint32_t*)vBuf, size, 
                (IMG_MEM_READ)(pDeviceTop->ifFlash?pDeviceTop->ifFlash->fRead:0));                
        
        pBuf = (uint32_t *)vBuf;
        
        addr += size;
        len -= size;
        size /= 4;

        while(size--)
        {
            if (*pBuf++ != u32BlankCheck)
            {
                return FAIL_RESP;
            }
        }
    }
    
   return PASS_RESP; 
}
/******************************************************************************
* Function: getCRC
* This function calculates the CRC of the locations starting with the
* specified address upto the specified length.
*
* PreCondition: none.
*
* Input:    addr - is the address where to begin CRC calculations.
*           len -  is the length in bytes of the buffer whose CRC
*                  needs to be calculated
*           pfRead - API to read from device interface
*
* Output:   crc - computed CRC value
*           PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   None
*
* Overview: None
* Note:     None
******************************************************************************/
static uint16_t getCRC(const uint8_t *addr, uint32_t len, 
        uint32_t * const crc, IMG_MEM_READ pfRead )
{
    uint32_t seed;         // initial seed
    uint32_t size = 0;
        
    seed=0xffff;
    Crc32Init( seed );
      
    while (len) {
        
        if (len > scratch_size)
            size = scratch_size;
        else
            size = len;

        read_from_scratch((uint32_t)addr, (uint32_t *)vBuf, size, pfRead);                
        
        addr += size;
        len -= size;

        Crc32Add(vBuf, size );
    }      

    *crc = (uint32_t) Crc32Result();

    return PASS_RESP;
}

/******************************************************************************
* Function: getCsum
* This function calculates the checksum of the locations starting with the
* specified address upto the specified length. 
*
* PreCondition: none.
*
* Input:    addr - is the address where to begin Checksum calculations.
*           len -  is the length in bytes of the buffer whose CRC
*                  needs to be calculated
*           pfRead - API to read from device interface
*
* Output:   csum - computed CRC value
*           PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   None
*
* Overview: None
* Note:     None
******************************************************************************/
static uint16_t getCsum(const uint8_t *addr, uint32_t len, 
        uint32_t * const csum, IMG_MEM_READ pfRead)
{
    uint8_t *pBuf;
    uint32_t size;
    uint32_t result = 0; 
            
    while (len) {
        
        if (len > scratch_size)
            size = scratch_size;
        else
            size = len;
        read_from_scratch((uint32_t)addr, (uint32_t *)vBuf, size, pfRead);                
        pBuf = vBuf;
        
        addr += size;
        len -= size;

        while(size--)
        {
            result += (*pBuf++);
        }
    }
    
    *csum = result;
    
    return 0;
}
/*****************************************************************************
* Function: prog
* Program specified number of rows in parallel for optimized programming.
* The function receives all the bytes for the specified length, and then
* sends a status back to the host indicating if the entire operation
* succeeded or something failed. 
*
* PreCondition:    none.
*
* Input:    address - is the address where to begin CRC calculations.
*           len -  is the length in bytes of the buffer whose CRC
*                  needs to be calculated
*           dataAddr - command payload address
*           pfWrite - API to write from device interface
*
* Output:   PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   None
*
* Overview: None
* Note:     None
******************************************************************************/
static uint16_t prog (const uint8_t *dataAddr, const uint32_t address, 
        const uint32_t len, IMG_MEM_WRITE pfWrite )
{
    uint16_t retval = PASS_RESP;
    if (pfWrite)
    {
        retval = pfWrite((uint8_t *)dataAddr, (uint32_t)address, (uint32_t)len); 	
    }    
    return retval;
}

static const IMG_MEM_TOPOLOGY * getDeviceTop(const uint32_t startAddr, 
        const uint32_t endAddr, const IMG_MEM_TOPOLOGY ** tops, 
        const uint8_t count)
{
    uint32_t i;
    
    for (i = 0; i < count; i++)
    {
        if ((startAddr >= tops[i]->u32UmmAddrStart) && (endAddr <= 
                (tops[i]->u32UmmAddrStart + tops[i]->u32CalIdx)))
            return (const IMG_MEM_TOPOLOGY *)tops[i];
    }
    return (const IMG_MEM_TOPOLOGY * )0;
}
/******************************************************************************
* Function: blankCheck
* This function erase the specified number of pages from flash.
*
* PreCondition:    none.
*
* Input:    pCmd  - Command data to program/verify in the device memory 
*           | CMD:Operand | Address |
* 
*           pDeviceTop - Valid device topology based on command address.          
*
* Output:   PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   None
*
* Overview: None
* Note:     None
******************************************************************************/
static uint16_t pageErase(const FPP_CMD_PACKET *pCmd, 
        const IMG_MEM_TOPOLOGY *pDeviceTop)
{
    uint32_t addr;
    uint16_t pageCount;
    uint32_t len;
    uint16_t retval = PASS_RESP;
    
    if (!pCmd)
        return FAIL_RESP;
    
    addr = (uint32_t)pCmd->erasePage.addr;
    pageCount = (uint16_t)pCmd->operand;
    len = (pageCount * pDeviceTop->u16ErasePageSz);
    if ((addr + len) >= (pDeviceTop->u32UmmAddrStart + pDeviceTop->u32CalIdx))
        return FAIL_RESP;

    if (pDeviceTop->ifFlash && pDeviceTop->ifFlash->fInit)
        pDeviceTop->ifFlash->fInit();
    
    addr =  virt_to_phy(addr, pDeviceTop);
    if (pDeviceTop->ifFlash && pDeviceTop->ifFlash->fErase)
    {
        retval = pDeviceTop->ifFlash->fErase(addr, pageCount);
    }    
    return retval;
}
    
static uint32_t virt_to_phy(const uint32_t addr, const IMG_MEM_TOPOLOGY *top) 
{
    return  ((addr | top->u32AddrPosMask) & top->u32AddrNegMask);
}

/******************************************************************************
* Function: read_from_scratch
* This function aid blankcheck/crc/cheksum methods to perform temporary read 
* flash content to scratch buffer
*
* PreCondition:    none.
*
* Input:    addr -  address from where contents are read
*           size -  size of buffer to read
*           pfRead - device interface read function.          
*
* Output:   PASS_RESP - if everything is OK.
*           FAIL_RESP - otherwise.
*
* Side Effects:   This fun re-use the command buffer, calling prior to handling 
*                 command may invalidate command buffer.
* Overview: None
* Note:     None
******************************************************************************/
static uint32_t *read_from_scratch(const uint32_t addr, uint32_t *pBuf, const uint32_t size, 
        IMG_MEM_READ pfRead)
{
    if (pfRead)
    {
        memset(pBuf, 0, size);
        if (pfRead(0, (uint8_t *)pBuf, (uint32_t)addr, size))
            return (uint32_t *)0;    
    }
    else
    {
        pBuf = (uint32_t *)addr; /* allowing in-place read */
    }

    return pBuf;
}