/*******************************************************************************
  Timer/Counter(TC3) PLIB

  Company
    Microchip Technology Inc.

  File Name
    plib_tc3.c

  Summary
    TC3 PLIB Implementation File.

  Description
    This file defines the interface to the TC peripheral library. This
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
#include "plib_tc3.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************

static TC_TIMER_CALLBACK_OBJ TC3_CallbackObject;

// *****************************************************************************
// *****************************************************************************
// Section: TC3 Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Initialize the TC module in Timer mode */
void TC3_TimerInitialize( void )
{
    /* Reset TC */
    TC3_REGS->COUNT16.TC_CTRLA = TC_CTRLA_SWRST_Msk;

    while((TC3_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_SWRST_Msk) == TC_SYNCBUSY_SWRST_Msk)
    {
        /* Wait for Write Synchronization */
    }

    /* Configure counter mode & prescaler */
    TC3_REGS->COUNT16.TC_CTRLA = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_PRESCSYNC_PRESC ;

    /* Configure in Match Frequency Mode */
    TC3_REGS->COUNT16.TC_WAVE = (uint8_t)TC_WAVE_WAVEGEN_MPWM;

    /* Configure timer period */
    TC3_REGS->COUNT16.TC_CC[0U] = 64000U;

    /* Clear all interrupt flags */
    TC3_REGS->COUNT16.TC_INTFLAG = (uint8_t)TC_INTFLAG_Msk;

    TC3_CallbackObject.callback = NULL;
    /* Enable interrupt*/
    TC3_REGS->COUNT16.TC_INTENSET = (uint8_t)(TC_INTENSET_MC1_Msk);


    while((TC3_REGS->COUNT16.TC_SYNCBUSY) != 0U)
    {
        /* Wait for Write Synchronization */
    }
}

/* Enable the TC counter */
void TC3_TimerStart( void )
{
    TC3_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE_Msk;
    while((TC3_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE_Msk) == TC_SYNCBUSY_ENABLE_Msk)
    {
        /* Wait for Write Synchronization */
    }
}

/* Disable the TC counter */
void TC3_TimerStop( void )
{
    TC3_REGS->COUNT16.TC_CTRLA &= ~TC_CTRLA_ENABLE_Msk;
    while((TC3_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE_Msk) == TC_SYNCBUSY_ENABLE_Msk)
    {
        /* Wait for Write Synchronization */
    }
}

uint32_t TC3_TimerFrequencyGet( void )
{
    return (uint32_t)(64000000U);
}

void TC3_TimerCommandSet(TC_COMMAND command)
{
    TC3_REGS->COUNT16.TC_CTRLBSET = (uint8_t)((uint32_t)command << TC_CTRLBSET_CMD_Pos);
    while((TC3_REGS->COUNT16.TC_SYNCBUSY) != 0U)
    {
        /* Wait for Write Synchronization */
    }    
}

/* Get the current timer counter value */
uint16_t TC3_Timer16bitCounterGet( void )
{
    /* Write command to force COUNT register read synchronization */
    TC3_REGS->COUNT16.TC_CTRLBSET |= (uint8_t)TC_CTRLBSET_CMD_READSYNC;

    while((TC3_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CTRLB_Msk) == TC_SYNCBUSY_CTRLB_Msk)
    {
        /* Wait for Write Synchronization */
    }

    while((TC3_REGS->COUNT16.TC_CTRLBSET & TC_CTRLBSET_CMD_Msk) != 0U)
    {
        /* Wait for CMD to become zero */
    }

    /* Read current count value */
    return (uint16_t)TC3_REGS->COUNT16.TC_COUNT;
}

/* Configure timer counter value */
void TC3_Timer16bitCounterSet( uint16_t count )
{
    TC3_REGS->COUNT16.TC_COUNT = count;

    while((TC3_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_COUNT_Msk) == TC_SYNCBUSY_COUNT_Msk)
    {
        /* Wait for Write Synchronization */
    }
}

/* Configure timer period */
void TC3_Timer16bitPeriodSet( uint16_t period )
{
    TC3_REGS->COUNT16.TC_CC[0] = period;
    while((TC3_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC0_Msk) == TC_SYNCBUSY_CC0_Msk)
    {
        /* Wait for Write Synchronization */
    }
}

/* Read the timer period value */
uint16_t TC3_Timer16bitPeriodGet( void )
{
    return (uint16_t)TC3_REGS->COUNT16.TC_CC[0];
}

void TC3_Timer16bitCompareSet( uint16_t compare )
{
    TC3_REGS->COUNT16.TC_CC[1] = compare;
    while((TC3_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC1_Msk) == TC_SYNCBUSY_CC1_Msk)
    {
        /* Wait for Write Synchronization */
    }
}


/* Register callback function */
void TC3_TimerCallbackRegister( TC_TIMER_CALLBACK callback, uintptr_t context )
{
    TC3_CallbackObject.callback = callback;

    TC3_CallbackObject.context = context;
}

/* Timer Interrupt handler */
void TC3_TimerInterruptHandler( void )
{
    if (TC3_REGS->COUNT16.TC_INTENSET != 0U)
    {
        TC_TIMER_STATUS status;
        status = (TC_TIMER_STATUS) TC3_REGS->COUNT16.TC_INTFLAG;
        /* Clear interrupt flags */
        TC3_REGS->COUNT16.TC_INTFLAG = (uint8_t)TC_INTFLAG_Msk;
        if((status != TC_TIMER_STATUS_NONE) && (TC3_CallbackObject.callback != NULL))
        {
            TC3_CallbackObject.callback(status, TC3_CallbackObject.context);
        }
    }
}

