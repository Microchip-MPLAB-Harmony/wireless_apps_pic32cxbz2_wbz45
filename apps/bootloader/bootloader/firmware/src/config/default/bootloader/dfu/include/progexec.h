//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2021 Microchip Technology Inc. and its subsidiaries.
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
#include <stdint.h> 
#include <stdbool.h>
#include "crc.h"
#include "mem_interface.h"

#ifndef __progexec_h__
#define __progexec_h__
//DOM-IGNORE-END

/* Program Executive Version Number */
#define PROG_EXEC_VERSION_NUMBER    0x1
#define PROG_EXEC_VNUM_IDENTIFIER   0xCDAB

/* SRAM data area */
#define PE_CMD_SRAM_ADDR            0xA00AFFF0
#define PE_RESP_SRAM_ADDR           0xA00AFFFC

#define PE_CMD_PAYLOAD_SIZE         4096
#define PE_CMD_MAX_HEADER           16     
#define PE_CMD_SIZE                 (PE_CMD_PAYLOAD_SIZE + \
                                     PE_CMD_MAX_HEADER)

#define PE_CMD_TIMEOUT              10000
#define PE_RESP_SIZE                0x4

#define PE_DEVICEID(x)              x //(( (x>>12) & 0xFF ))

#define CMD(x)          ((x>>16))      /* extract command */
#define LEN(x)          ((x&0xFFFF))   /* extract length */

#define ROW_SIZE        128

/* This enumeration identifies the command packet values and structure used 
   in the DFU process */
typedef struct {
   
    // command values
    #define PROG_ROW_CMD            0x0
    #define READ_CMD                0x1
    #define PROGRAM_CMD             0x2
    #define PROG_WORD_CMD           0x3
    #define CHIP_ERASE_CMD          0x4
    #define PAGE_ERASE_CMD          0x5
    #define BLANK_CHECK_CMD         0x6
    #define EXEC_VERSION_CMD        0x7
    #define GET_CRC_CMD             0x8
    #define PROGRAM_CLUSTER_CMD     0x9
    #define GET_DEVICEID            0xA
    #define CHANGE_CFG_CMD          0xB
    #define GET_CHECKSUM_CMD        0xC
    #define PROG_QUAD_WORD_CMD      0xD
    #define PROG_DBL_WORD_CMD       0xE
    #define PROG_QUAD_DWORD_CMD     0x10
    #define PROG_CLUSTER_VERIFY_CMD 0x11
    #define RESET_CMD               0x12
    #define PE_TEST                 0xF // general purpose test command

    // Image cluster verification methods
    #define PE_CFG_CHECKSUM         0x1
    #define PE_CFG_CRC              0x2           
    
    uint16_t cmd;
    uint16_t operand;
   
   // command specific data
    union 
    {      
        // general purpose generic
        struct 
        {
            uint32_t data1;
            uint32_t data2;
        } generic;

       struct
       {
            void *addr;
            uint32_t len;
            uint32_t checksum;  
            void *payload;
          // data of variable length to follow
       } progClusterVerify;      

       // erase page
       struct
       {
            void *addr;
            uint32_t reserved;
       } erasePage;
       
       // erase Chip
       struct
       {
            void *addr;
            uint32_t reserved;
       } eraseChip;       

       // verify
       struct
       {
            void *addr;
            uint32_t len;         
            uint32_t reserved;
       } blanckCheck;
    };  
} FPP_CMD_PACKET, *PFPP_CMD_PACKET;

/* Identifies the command packet response structure used for DFU process */
typedef struct {
   
   uint16_t lastCmd;
   
   // error code values
   #define PASS_RESP    0x00
   #define FAIL_RESP    0x02
   #define NACK_RESP    0x03
   uint16_t errCode;
} FPP_RESP_PACKET, *PFPP_RESP_PACKET;

// *****************************************************************************
/**
*@brief This routine will initiate the device firmware upgrade where it waits for commands
*       and response for commands received from serial.
*
*@param  IMG_MEM_TOPOLOGY ** tops - valid toplogies list
*         uint8_t count            - number of valid topologies
*
*@retval None
*/
void dfu(const IMG_MEM_TOPOLOGY ** tops, uint8_t count);

// *****************************************************************************
/**
*@brief This routine is the entry point for the programming executive. It receives commands
*    from the host, dispatches the command, and then sends response back to 
*    the host.
*
*@param  IMG_MEM_TOPOLOGY ** tops - valid toplogies list
*        uint8_t count            - number of valid topologies
*
*@retval None
*/
int32_t program_exec_main(const IMG_MEM_TOPOLOGY ** tops, uint8_t count);

#endif /* __progexec_h__ */
