/*********************************************************************
 *
 *                  CRC Calculation Library
 *
 *********************************************************************
 * FileName:        crc.c
 * Dependencies:    None
 * Processor:       PIC32MX
 *
 * Complier:        MPLAB C30 v1.31.00 or higher
 *                  MPLAB IDE v7.10.00 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?ompany?) for its Daytona/PIC24F Microcontroller is intended
 * and supplied to you, the Company? customer, for use solely and
 * exclusively on Microchip Daytona/PIC24F Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?S IS? CONDITION. NO WARRANTIES,
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

#define P_CCITT 0x1021

/********************************************************************
 Global Data
********************************************************************/
extern uint32_t row[];
static int32_t crc_tabccitt_init;
static uint16_t crc_tab_val;

/********************************************************************
 Global Functions
********************************************************************/
static void update_crc_ccitt(uint8_t c);
static void init_crcccitt_tab(void);

/********************************************************************
 Crc32Add
********************************************************************/
void Crc32Init(const uint16_t seed)
{
    crc_tabccitt_init = 0;

    crc_tab_val = seed;
    init_crcccitt_tab();
    crc_tabccitt_init=1;	
}

/********************************************************************
 Crc32Add
********************************************************************/
void Crc32Add(uint8_t* pBuff, uint32_t bSize)
{
    while(bSize--)
    {
        update_crc_ccitt(*pBuff++);
    }
}

/********************************************************************
 Crc32Result
********************************************************************/
uint16_t Crc32Result()
{
	return crc_tab_val;
}
    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_ccitt( char c );   *
    *                                                                   *
    *   The function update_crc_ccitt calculates  a  new  CRC-CCITT     *
    *   value  based  on the previous value of the CRC and the next     *
    *   byte of the data to be checked.                                 *
    *                                                                   *
    \*******************************************************************/

static void update_crc_ccitt(uint8_t c)
{
    uint16_t tmp, short_c;

    short_c  = 0x00ff & (uint16_t)c;
    tmp = (crc_tab_val >> 8) ^ short_c;
    crc_tab_val = (crc_tab_val << 8) ^ row[tmp];
}  /* update_crc_ccitt */


   /*******************************************************************\
    *                                                                   *
    *   static void init_crcccitt_tab( void );                          *
    *                                                                   *
    *   The function init_crcccitt_tab() is used to fill the  array     *
    *   for calculation of the CRC-CCITT with values.                   *
    *                                                                   *
    \*******************************************************************/

static void init_crcccitt_tab()
{
    int32_t i, j;
    uint16_t crc, c;

    for (i=0; i<256; i++) 
    {
        crc = 0;
        c   = ((uint16_t) i) << 8;

        for (j=0; j<8; j++) 
        {
            if( (crc ^ c) & 0x8000 ) 
               crc = ( crc << 1 ) ^ P_CCITT;
            else                      
               crc =   crc << 1;
            c = c << 1;
        }
        row[i] = crc;
    }
} 