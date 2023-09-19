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

#ifndef UART_H
#define	UART_H

// DOM-IGNORE-BEGIN
#ifdef	__cplusplus
extern "C" {
#endif
#include <xc.h>
//DOM-IGNORE-END

/** Length of RX FIFO */
#define UART_RXFIFO_DEPTH       9   

/** Possible UART Errors */    
typedef enum
{
    // UART - No Error
    UART_ERROR_NONE = 0,
    // UART - Overrun error
    UART_ERROR_OVERRUN = 0x02,
    // UART - Framing error
    UART_ERROR_FRAMING = 0x04,
    // UART - Parity error
    UART_ERROR_PARITY  = 0x08

} UART_ERROR;

/**
*@brief This function initializes UART for read notifcation and thershold setting.
*
*@param None
*
*@retval None
*/
void UART_Init(void);
/**
*@brief This function reads the SERCOM UART for the given length.
*
*@param rb - read buffer
*       len - length to read
*       wait - wait timeout
*   
*@retval None
*/
uint32_t UART_Read(uint8_t *rb, const uint32_t len, const int32_t wait);
/**
*@brief This function writes the given buffer in SERCOM UART for the given length.
*
*@param wb - write buffer
*       len - length to write
*   
*@retval None
*/
void UART_Write(int8_t *wb, uint32_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

