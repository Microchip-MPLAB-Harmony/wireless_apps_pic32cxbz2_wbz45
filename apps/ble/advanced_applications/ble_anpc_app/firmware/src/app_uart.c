
/*******************************************************************************
  Application Uart Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_uart.c

  Summary:
    This file contains the Uart functions for this project.

  Description:
    This file contains the Uart functions for this project.
 *******************************************************************************/

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "peripheral/sercom/usart/plib_sercom0_usart.h"
#include "driver/usart/drv_usart.h"
#include <string.h>
#include "app_uart.h"

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
#define APP_UART_MAX_LEN                   512 

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Local Variables
// *****************************************************************************
// *****************************************************************************

static DRV_HANDLE s_usartHandle     = DRV_HANDLE_INVALID;
static DRV_USART_BUFFER_HANDLE s_bufferHandle    = DRV_USART_BUFFER_HANDLE_INVALID;
static bool s_transferStatus  = false;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

static void app_UartBufferEventHandler(DRV_USART_BUFFER_EVENT bufferEvent, DRV_USART_BUFFER_HANDLE bufferHandle, uintptr_t context)
{
    switch(bufferEvent)
    {
        case DRV_USART_BUFFER_EVENT_COMPLETE:
        {
            s_transferStatus = true;
        }
        break;

        case DRV_USART_BUFFER_EVENT_ERROR:
        {
        }
        break;

        default:
        break;
    }
}

void APP_UartInit(void)
{
    s_usartHandle = DRV_USART_Open(DRV_USART_INDEX_0, DRV_IO_INTENT_READWRITE);

    if (s_usartHandle != DRV_HANDLE_INVALID)
    {
        //successful
        DRV_USART_BufferEventHandlerSet(s_usartHandle, app_UartBufferEventHandler, 0);
    }
    else
    {
        //error
    }
}

void APP_UartMsg(void *p_buffer)
{
    if (s_usartHandle != DRV_HANDLE_INVALID)
    {
        s_transferStatus=false;
        DRV_USART_WriteBufferAdd(s_usartHandle, (void*)p_buffer, strlen(p_buffer), &s_bufferHandle);

        if (s_bufferHandle != DRV_USART_BUFFER_HANDLE_INVALID)
        {
            //successful
        }
        else
        {
            //error
        }
    }
}
