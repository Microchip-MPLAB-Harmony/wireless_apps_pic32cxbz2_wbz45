/*******************************************************************************
  Timer/Counter(TCC0) PLIB

  Company
    Microchip Technology Inc.

  File Name
    plib_tcc0.c

  Summary
    TCC0 PLIB Implementation File.

  Description
    This file defines the interface to the TCC peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

  Remarks:
    None.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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
/* This section lists the other files that are included in this file.
*/

#include "interrupts.h"
#include "plib_tcc0.h"







// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

volatile static TCC_CALLBACK_OBJECT TCC0_CallbackObject;

// *****************************************************************************
// *****************************************************************************
// Section: TCC0 Implementation
// *****************************************************************************
// *****************************************************************************

/* Initialize TCC module in Compare Mode */
void TCC0_CompareInitialize( void )
{
    /* Reset TCC */
    TCC0_REGS->TCC_CTRLA = TCC_CTRLA_SWRST_Msk;

    while((TCC0_REGS->TCC_SYNCBUSY & TCC_SYNCBUSY_SWRST_Msk) == TCC_SYNCBUSY_SWRST_Msk)
    {
        /* Wait for Write Synchronization */
    }

    /* Configure counter mode & prescaler */
    TCC0_REGS->TCC_CTRLA = TCC_CTRLA_PRESCALER_DIV1 | TCC_CTRLA_PRESCSYNC_PRESC ;
    /* Configure waveform generation mode */
    TCC0_REGS->TCC_WAVE = TCC_WAVE_WAVEGEN_NPWM;

    TCC0_REGS->TCC_WEXCTRL = TCC_WEXCTRL_OTMX(0UL);


    
    TCC0_REGS->TCC_PER = 65535U;
    
    TCC0_REGS->TCC_CC[0] = 24U;
    TCC0_REGS->TCC_CC[1] = 24U;
    TCC0_REGS->TCC_CC[2] = 24U;
    TCC0_REGS->TCC_CC[3] = 24U;
    TCC0_REGS->TCC_CC[4] = 24U;
    TCC0_REGS->TCC_CC[5] = 24U;

    /* Clear all interrupt flags */
    TCC0_REGS->TCC_INTFLAG = TCC_INTFLAG_Msk;

    /* Enable period Interrupt */
    TCC0_CallbackObject.callback_fn = NULL;
    TCC0_REGS->TCC_INTENSET =(TCC_INTENSET_MC0_Msk | TCC_INTENSET_MC1_Msk | TCC_INTENSET_MC2_Msk);

    TCC0_REGS->TCC_EVCTRL = (TCC_EVCTRL_MCEO0_Msk | TCC_EVCTRL_MCEO1_Msk | TCC_EVCTRL_MCEO2_Msk);
    while((TCC0_REGS->TCC_SYNCBUSY) != 0U)
    {
        /* Wait for Write Synchronization */
    }
}

/* Enable the counter */
void TCC0_CompareStart( void )
{
    TCC0_REGS->TCC_CTRLA |= TCC_CTRLA_ENABLE_Msk;
    while((TCC0_REGS->TCC_SYNCBUSY & TCC_SYNCBUSY_ENABLE_Msk) == TCC_SYNCBUSY_ENABLE_Msk)
    {
        /* Wait for Write Synchronization */
    }
}

/* Disable the counter */
void TCC0_CompareStop( void )
{
    TCC0_REGS->TCC_CTRLA &= ~TCC_CTRLA_ENABLE_Msk;
    while((TCC0_REGS->TCC_SYNCBUSY & TCC_SYNCBUSY_ENABLE_Msk) == TCC_SYNCBUSY_ENABLE_Msk)
    {
        /* Wait for Write Synchronization */
    }
}

uint32_t TCC0_CompareFrequencyGet( void )
{
    return (uint32_t)64000000;
}

void TCC0_CompareCommandSet(TCC_COMMAND command)
{
    TCC0_REGS->TCC_CTRLBSET = (uint8_t)((uint32_t)command << TCC_CTRLBSET_CMD_Pos);
    while((TCC0_REGS->TCC_SYNCBUSY) != 0U)
    {
        /* Wait for Write Synchronization */
    }    
}

/* Get the current counter value */
uint32_t TCC0_Compare24bitCounterGet( void )
{
    /* Write command to force COUNT register read synchronization */
    TCC0_REGS->TCC_CTRLBSET |= (uint8_t)TCC_CTRLBSET_CMD_READSYNC;

    while((TCC0_REGS->TCC_SYNCBUSY & TCC_SYNCBUSY_CTRLB_Msk) == TCC_SYNCBUSY_CTRLB_Msk)
    {
        /* Wait for Write Synchronization */
    }

    while((TCC0_REGS->TCC_CTRLBSET & TCC_CTRLBSET_CMD_Msk) != 0U)
    {
        /* Wait for CMD to become zero */
    }

    /* Read current count value */
    return TCC0_REGS->TCC_COUNT;
}

/* Configure counter value */
void TCC0_Compare24bitCounterSet( uint32_t countVal )
{
    TCC0_REGS->TCC_COUNT = countVal;

    while((TCC0_REGS->TCC_SYNCBUSY & TCC_SYNCBUSY_COUNT_Msk) == TCC_SYNCBUSY_COUNT_Msk)
    {
        /* Wait for Write Synchronization */
    }
}

/* Configure period value */
bool TCC0_Compare24bitPeriodSet( uint32_t period )
{
    bool status = false;
    if((TCC0_REGS->TCC_STATUS & TCC_STATUS_PERBUFV_Msk) == 0U)
    {
        /* Configure period value */
        TCC0_REGS->TCC_PERBUF = period & 0xFFFFFFU;
        status = true;
    }
    return status;
}

/* Read period value */
uint32_t TCC0_Compare24bitPeriodGet( void )
{
    /* Get period value */
    return TCC0_REGS->TCC_PER;
}

/* Configure duty cycle value */
bool TCC0_Compare24bitMatchSet(TCC0_CHANNEL_NUM channel, uint32_t compareValue )
{
    bool status = false;
    if ((TCC0_REGS->TCC_STATUS & (1UL << (TCC_STATUS_CCBUFV0_Pos + (uint32_t)channel))) == 0U)
    {
        /* Set new compare value for compare channel */
        TCC0_REGS->TCC_CCBUF[channel] = compareValue & 0xFFFFFFU;
        status = true;
    }
    return status;
}


/* Register callback function */
void TCC0_CompareCallbackRegister( TCC_CALLBACK callback, uintptr_t context )
{
    TCC0_CallbackObject.callback_fn = callback;

    TCC0_CallbackObject.context = context;
}

/* Compare match interrupt handler */
void __attribute__((used)) TCC0_InterruptHandler( void )
{
    uint32_t status;
    /* Additional local variable to prevent MISRA C violations (Rule 13.x) */
    uintptr_t context;
    context = TCC0_CallbackObject.context;    
    status = TCC0_REGS->TCC_INTFLAG;
    /* clear period interrupt */
    TCC0_REGS->TCC_INTFLAG = TCC_INTFLAG_Msk;
    (void)TCC0_REGS->TCC_INTFLAG;
    if(TCC0_CallbackObject.callback_fn != NULL)
    {
        TCC0_CallbackObject.callback_fn(status, context);
    }
}

  