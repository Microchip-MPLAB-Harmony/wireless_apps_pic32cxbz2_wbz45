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