
/*******************************************************************************
  Sleep System Source File

  Company:
    Microchip Technology Inc.

  File Name:
    device_deep_sleep.c

  Summary:
    This file contains the Deep Sleep functions.

  Description:
    This file contains the Deep Sleep functions.
 *******************************************************************************/

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

#include "device.h"
#include "device_deep_sleep.h"
#include "definitions.h"


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
/* RTC clock frequency during deep sleep mode. */
#define DEVICE_RTC_CLOCK_FREQUENCY_1024HZ                 ( 1024U )
#define DEVICE_RTC_CLOCK_FREQUENCY_32768HZ                ( 32768U )


typedef enum DEVICE_ClkSrc_T
{
    DEVICE_SOURCE_SOSC,
    DEVICE_SOURCE_LPRC,
    DEVICE_SOURCE_END
} DEVICE_ClkSrc_T;

typedef enum DEVICE_RtcClkFreq_T
{
    DEVICE_RTC_CLK_FREQ_1024HZ,
    DEVICE_RTC_CLK_FREQ_32768HZ,

    DEVICE_RTC_CLK_FREQ_END
} DEVICE_RtcClkFreq_T;


// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
static uint32_t s_rtcClkFreq;
static DEVICE_DeepSleepWakeSrc_T __attribute__((section (".bss.s_wakeUpSrc"), noload)) s_wakeUpSrc;
static uint32_t __attribute__((section (".bss.s_rtcCounter"), noload)) s_rtcCounter;



// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************
static void devie_SysUnlock(void)
{
    /* Unlock system for clock configuration */
    CFG_REGS->CFG_SYSKEY = 0x00000000U;
    CFG_REGS->CFG_SYSKEY = 0xAA996655U;
    CFG_REGS->CFG_SYSKEY = 0x556699AAU;
}

static void devie_SetWfi(void)
{
    /* Set wait for interrupt (WFI) */
    __asm volatile( "dsb" ::: "memory" );
    __asm volatile( "wfi" );
    __asm volatile( "isb" );
}

static void devie_DisableInt(void)
{
    //Disable interrupt
    __asm volatile( "cpsid i" ::: "memory" );
    __asm volatile( "dsb" );
    __asm volatile( "isb" );
}

static void devie_DisableSysTick(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    
    if ((SCB->ICSR & SCB_ICSR_PENDSTSET_Msk) != 0U) 
    {
        SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
    }
}

static void device_DisableSercom(void)
{
    if ((SERCOM0_REGS->USART_INT.SERCOM_CTRLB & SERCOM_USART_INT_CTRLB_TXEN_Msk) != 0U)
    {
        SERCOM0_REGS->USART_INT.SERCOM_CTRLB &= ~SERCOM_USART_INT_CTRLB_TXEN_Msk;
    }

    if ((SERCOM1_REGS->USART_INT.SERCOM_CTRLB & SERCOM_USART_INT_CTRLB_TXEN_Msk) != 0U)
    {
        SERCOM1_REGS->USART_INT.SERCOM_CTRLB &= ~SERCOM_USART_INT_CTRLB_TXEN_Msk;
    }

    if ((SERCOM2_REGS->USART_INT.SERCOM_CTRLB & SERCOM_USART_INT_CTRLB_TXEN_Msk) != 0U)
    {
        SERCOM2_REGS->USART_INT.SERCOM_CTRLB &= ~SERCOM_USART_INT_CTRLB_TXEN_Msk;
    }

    if ((SERCOM3_REGS->USART_INT.SERCOM_CTRLB & SERCOM_USART_INT_CTRLB_TXEN_Msk) != 0U)
    {
        SERCOM3_REGS->USART_INT.SERCOM_CTRLB &= ~SERCOM_USART_INT_CTRLB_TXEN_Msk;
    }
}

static void device_ConfigLpClkSrc( DEVICE_ClkSrc_T clkSrc, DEVICE_RtcClkFreq_T freq )  // Configure Low Power Clock
{
    uint32_t tmpCfgcon4;
    
    tmpCfgcon4 = CFG_REGS->CFG_CFGCON4;
    tmpCfgcon4 &= ~((uint32_t)0x0000F800U);
    
    if (clkSrc == DEVICE_SOURCE_SOSC)
    {
        // SOSC as LPClk  
        if (freq == DEVICE_RTC_CLK_FREQ_1024HZ)
        {
            //CFG_REGS->CFG_CFGCON4SET = 0x0000A800U;    //bit 11 = 1 => RTC is 1024Hz
            tmpCfgcon4 |= (uint32_t)0x0000A800U;
            CFG_REGS->CFG_CFGCON4 = tmpCfgcon4;
            s_rtcClkFreq = DEVICE_RTC_CLOCK_FREQUENCY_1024HZ;
        }
        else if (freq == DEVICE_RTC_CLK_FREQ_32768HZ)
        {
            //CFG_REGS->CFG_CFGCON4SET = 0x0000A000U;    //bit 11 = 0 => RTC is 32768Hz(higher current consumption)
            tmpCfgcon4 |= (uint32_t)0x0000A000U;
            CFG_REGS->CFG_CFGCON4 = tmpCfgcon4;

            s_rtcClkFreq = DEVICE_RTC_CLOCK_FREQUENCY_32768HZ;
        }
        else
        {
            /* Do nothing */
        }
    }
    else if (clkSrc == DEVICE_SOURCE_LPRC )
    {
        // LPRC as LPClk  
        //bit 15 = 0 => LPCLK/DSWDT is 32768Hz; bit 15 = 1 => 32kHz
        if (freq == DEVICE_RTC_CLK_FREQ_1024HZ)
        {
            //CFG_REGS->CFG_CFGCON4SET = 0x0000F800;   //bit 11 = 1 => RTC is 1024Hz
            tmpCfgcon4 |= (uint32_t)0x0000F800U;
            CFG_REGS->CFG_CFGCON4 = tmpCfgcon4;

            s_rtcClkFreq = DEVICE_RTC_CLOCK_FREQUENCY_1024HZ;
        }
        else if (freq == DEVICE_RTC_CLK_FREQ_32768HZ)
        {
            //CFG_REGS->CFG_CFGCON4SET = 0x0000F000;   //bit 11 = 0 => RTC is 32768Hz
            tmpCfgcon4 |= (uint32_t)0x0000F000U;
            CFG_REGS->CFG_CFGCON4 = tmpCfgcon4;
            
            s_rtcClkFreq = DEVICE_RTC_CLOCK_FREQUENCY_32768HZ;
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
}

static void device_DisableEic(void)
{
    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_DisableIRQ(EIC_IRQn);

    // reset EIC
    EIC_REGS->EIC_CTRLA |= EIC_CTRLA_SWRST_Msk;

    while((EIC_REGS->EIC_SYNCBUSY & EIC_SYNCBUSY_SWRST_Msk) == EIC_SYNCBUSY_SWRST_Msk)
    {
        /* Wait for sync */
    }

    // turn off clocks
    CFG_REGS->CFG_CFGPCLKGEN1CLR = CFG_CFGPCLKGEN1_EICCSEL_Msk;
    CFG_REGS->CFG_CFGPCLKGEN1CLR = CFG_CFGPCLKGEN1_EICCD_Msk;
}

static void device_disablePmd(bool enableRtc)
{
    // Disable PMD lock
    CFG_REGS->CFG_CFGCON0 &= ~CFG_CFGCON0_PMDLOCK_Msk;

    if (enableRtc)
    {
        CFG_REGS->CFG_PMD1SET = 0xFFFEFFFFU; // Except RTCC (bit 16)
    }
    else
    {
        CFG_REGS->CFG_PMD1SET = 0xFFFFFFFFU;
    }

    CFG_REGS->CFG_PMD2SET = 0xFFFFFFFFU;
    CFG_REGS->CFG_PMD3SET = 0xFFFFU;
    CFG_REGS->CFG_CFGCON1CLR = 0x00100000U; // SAMD Macro CCL Pads (via PPS) Output Disable    
    CMCC_REGS->CMCC_CTRL = 0x0; // SAMD Macro CMCC Disable

    //SAMD Macro DMAC Disable
    DMAC_REGS->DMAC_CTRL = DMAC_CTRL_DMAENABLE(0) & DMAC_REGS->DMAC_CTRL;  //Come from validation team

    EIC_REGS->EIC_CTRLA = 0x0; // SAMD Macro EIC Disable
    FREQM_REGS->FREQM_CTRLA = 0x0; // SAMD Macro FREQM Disable 

    if (!enableRtc)   //Disable RTC
    {
        RTC_Timer32Stop();
    }
}

static void device_ConfigDsCtrlRtcc(bool enable)
{
    if (enable)
    {
        //RTCC Request enable
        DSCON_REGS->DSCON_DSCON &= ~DSCON_DSCON_RTCPWREQ_Msk;
        DSCON_REGS->DSCON_DSCON &= ~DSCON_DSCON_RTCPWREQ_Msk;

        //RTCC wake up enable
        DSCON_REGS->DSCON_DSCON &= ~DSCON_DSCON_RTCCWDIS_Msk;
        DSCON_REGS->DSCON_DSCON &= ~DSCON_DSCON_RTCCWDIS_Msk;

    }
    else
    {
        //RTCC Request disable
        DSCON_REGS->DSCON_DSCON |= DSCON_DSCON_RTCPWREQ_Msk;
        DSCON_REGS->DSCON_DSCON |= DSCON_DSCON_RTCPWREQ_Msk;

        //RTCC wake up disable
        DSCON_REGS->DSCON_DSCON |= DSCON_DSCON_RTCCWDIS_Msk;
        DSCON_REGS->DSCON_DSCON |= DSCON_DSCON_RTCCWDIS_Msk;
    }
}

static void device_ConfigDeepSleepEnableReg(void)
{
    if ((CFG_REGS->CFG_CFGCON0 & CFG_CFGCON0_CFGCLOCK_Msk) != 0U)
    {
       // Disable CFG lock
       CFG_REGS->CFG_CFGCON0 &= ~CFG_CFGCON0_CFGCLOCK_Msk;
    }

    CFG_REGS->CFG_CFGCON4SET = CFG_CFGCON4_DSEN_Msk;
}

static void device_ConfigDsCtrlReg(void)
{
    DSCON_REGS->DSCON_DSCON |= DSCON_DSCON_DSEN_Msk;
    DSCON_REGS->DSCON_DSCON |= DSCON_DSCON_DSEN_Msk;
}

static void device_ConfigDeepSleepReg(void)
{
    devie_SysUnlock();

    DEVICE_SLEEP_ConfigWssLowPowerMode();

    device_ConfigDeepSleepEnableReg();
    device_ConfigDsCtrlReg();

    //set sleep enable to 1, make CPU into sleep
    CRU_REGS->CRU_OSCCON |= CRU_OSCCON_SLPEN_Msk;
}

static void device_ConfigInt0(void)
{
    if ((CFG_REGS->CFG_CFGCON0 & CFG_CFGCON0_CFGCLOCK_Msk) != 0U)
    {
       // Disable CFG lock
       CFG_REGS->CFG_CFGCON0 &= ~CFG_CFGCON0_CFGCLOCK_Msk;
    }

    CFG_REGS->CFG_CFGCON0SET = CFG_CFGCON0_INT0P_Msk;    //Set INT0 Polarity as positive
    
    CFG_REGS->CFG_CFGCON0SET = CFG_CFGCON0_INT0E_Msk;    //ENABLE INT0
}


/* RTC callback event handler */
static void device_RtcHandler(RTC_TIMER32_INT_MASK intCause, uintptr_t context)
{
    if ((RTC_MODE0_INTENSET_CMP0_Msk & intCause) != 0U)
    {

    }
}


/* Register RTC callback function */
static void device_DeepSleepRegRtcCallback(void)
{
    RTC_Timer32CallbackRegister(device_RtcHandler, 0);
}

/* Set RTC timer */
static void device_setDsInterval(uint32_t interval)
{
    uint32_t compareValue = 0;
    uint32_t currentRtcCnt = 0;


    device_DeepSleepRegRtcCallback();

    /* Disable RTC interrupt to prevent from unexpected RTC interrupt triggered. */
    RTC_Timer32InterruptDisable(RTC_MODE0_INTENCLR_CMP0_Msk);

    /*
       1. Unit of interval: 1ms
       2. RTC Clock : RTC_Timer32FrequencyGet
       3. intercal (ms) * RTC clock (32 kHz) = compareValue value
    */
    compareValue = (uint32_t)(((uint64_t)interval * s_rtcClkFreq + (configTICK_RATE_HZ / 2U)) / configTICK_RATE_HZ);

    /* Get current RTC counter value after system wakes up */
    currentRtcCnt = RTC_Timer32CounterGet();

    if ((0xFFFFFFFFU - currentRtcCnt) < compareValue)
    {
        compareValue -= (0xFFFFFFFFU - currentRtcCnt);
    }
    else
    {
        compareValue += currentRtcCnt;
    }

    RTC_Timer32Compare0Set(compareValue);

    RTC_Timer32InterruptEnable(RTC_MODE0_INTENSET_CMP0_Msk);

    /* Check if RTC timer has been started or not */
    if ((RTC_REGS->MODE0.RTC_CTRLA & RTC_MODE0_CTRLA_ENABLE_Msk) == 0U)
    {
        RTC_Timer32Start();
    }
}


/* Configure the GPIO setting for deep sleep. 
   It's an example code for wbz curiosity board.
   The user application could have its own configuration based on the requirement. 
*/
static void Device_GpioConfig(void)
{
    /* Disable JTAG since at least one of its pins is configured for Non-JTAG function */
    CFG_REGS->CFG_CFGCON0CLR = CFG_CFGCON0_JTAGEN_Msk;

    GPIOA_REGS->GPIO_TRISSET = 0xFFFF; //Set all pins as input 
    GPIOA_REGS->GPIO_CNPUSET = 0x1FF5; //Except PA1 (QSPI Data), PA3, the others are pulled up
    GPIOA_REGS->GPIO_CNPDSET = 0x0008; //RPA3 is Pulled down: SERCOM 0-RTS

    GPIOB_REGS->GPIO_TRISSET = 0xFFFF; //Set all pins as input 
    GPIOB_REGS->GPIO_CNPUSET = 0xF886; //PB1, PB2, PB7(User LED), 11(QSPI_SCK), 12 (QSPI DATA1), 13(QSPI DATA0) are pulled high
    GPIOB_REGS->GPIO_CNPDSET = 0x0029; //pull down RB0,3,5 (LEDs))
}


/* Clear the deep sleep related regiser if the device is waken from deep sleep mode. */
bool DEVICE_ClearDeepSleepReg(void)
{
    if ((RCON_REGS->RCON_RCON & RCON_RCON_DPSLP_Msk) == RCON_RCON_DPSLP_Msk)
    {
        s_wakeUpSrc = DEVICE_DEEP_SLEEP_WAKE_OTHER;
        
        if ((DSCON_REGS->DSCON_DSWAKE & DSCON_DSWAKE_MCLR_Msk) != 0U)
        {
            s_wakeUpSrc = DEVICE_DEEP_SLEEP_WAKE_MCLR;
        }
        else if ((DSCON_REGS->DSCON_DSWAKE & DSCON_DSWAKE_INT0_Msk) != 0U)
        {
            s_wakeUpSrc = DEVICE_DEEP_SLEEP_WAKE_INT0;
        }
        else if ((DSCON_REGS->DSCON_DSWAKE & DSCON_DSWAKE_RTCC_Msk) != 0U)
        {
            s_wakeUpSrc = DEVICE_DEEP_SLEEP_WAKE_RTC;
        }
        else if ((DSCON_REGS->DSCON_DSWAKE & DSCON_DSWAKE_DSWDT_Msk) != 0U)
        {
            s_wakeUpSrc = DEVICE_DEEP_SLEEP_WAKE_DSWDT;
        }
        else 
        {
            /* Do nothing */
        }

        //Note: Before cleaning deep sleep related register,
        //the application might consider to restore the GPIO settings before entering deep sleep
        //to avoid reseting the I/O status to default.

        //Clear RCON reg
        RCON_REGS->RCON_RCONCLR = 0xFFFFFFFFU;

        //Clear DSCON reg
        DSCON_REGS->DSCON_DSCON = 0x0000;
        DSCON_REGS->DSCON_DSCON = 0x0000;
        
        DSCON_REGS->DSCON_DSWAKE = 0x0000;
        DSCON_REGS->DSCON_DSWAKE = 0x0000;

        s_rtcCounter = RTC_Timer32CounterGet();

        return true;
    }
    else
    {
        s_wakeUpSrc = DEVICE_DEEP_SLEEP_WAKE_NONE;
        s_rtcCounter = 0;

        return false;
    }
}

/* Perform the deep sleep interval calibration to exclude the HW preparation time of advertising. */
uint32_t DEVICE_DeepSleepIntervalCal(uint32_t expectedInt)
{
    uint32_t currRTcCnt = RTC_Timer32CounterGet();
    uint32_t diff;
    uint32_t ret;
    uint32_t temp;

    if ((s_wakeUpSrc != DEVICE_DEEP_SLEEP_WAKE_RTC) && (s_wakeUpSrc != DEVICE_DEEP_SLEEP_WAKE_INT0))
    {
        return expectedInt;
    }

    if (currRTcCnt >= s_rtcCounter)
    {
        diff = currRTcCnt - s_rtcCounter;
    }
    else
    {
        diff = 0xFFFFFFFFU - s_rtcCounter;
        diff += (currRTcCnt + 1U);
    }

    temp = (uint32_t)(((uint64_t)diff * configTICK_RATE_HZ) / RTC_Timer32FrequencyGet());

    ret = expectedInt -temp;
    return ret;
}

/* Get the wake up source of deep sleep mode.*/
void DEVICE_GetDeepSleepWakeUpSrc(DEVICE_DeepSleepWakeSrc_T *wakeUpSrc)
{
    *wakeUpSrc = s_wakeUpSrc;
}

/* Set the wake up source of deep sleep mode.*/
void DEVICE_SetDeepSleepWakeUpSrc(DEVICE_DeepSleepWakeSrc_T wakeUpSrc)
{
    s_wakeUpSrc = wakeUpSrc;
}

/* Perform the procedure of entering deep sleep mode.*/
void DEVICE_EnterDeepSleep(bool enableRetRam, uint32_t interval)
{
    devie_SysUnlock();

    //Disable SERCOM TX Enable
    device_DisableSercom();

    //Disable All Peripherals with PMD Disable Bits Except RTCC
    device_disablePmd(true);

    //Gpio set Port A & B as input mode and pulled up Except for Interrupt
    Device_GpioConfig();

    // Config LP clock source as SOSC or LPRC
    if ((CFG_REGS->CFG_CFGCON4 & CFG_CFGCON4_VBKP_32KCSEL_Msk) == 0x2000U)   //LP clock source is set as SOSC, no need to switch
    {
        s_rtcClkFreq = DEVICE_RTC_CLOCK_FREQUENCY_32768HZ;     //Just update the static variable
    }
    else  //LP clock source is not set as SOSC, need to switch to LPRC
    {
        device_ConfigLpClkSrc(DEVICE_SOURCE_LPRC, DEVICE_RTC_CLK_FREQ_32768HZ);
    }

    // WCM Memories Ret + NAP Mode
    DEVICE_SLEEP_ConfigRetRam(enableRetRam);

    // Disable system timer
    devie_DisableSysTick();

    // Clear interrupt state
    device_DisableEic();

    // Configure INT0 for DS, fixed @ RB4.
    device_ConfigInt0();

    device_ConfigDeepSleepReg();

    if (interval != 0U)
    {
        device_setDsInterval(interval);
    }
    else
    {
        //Disable RTC interrupt if the device is waken up by RTC and enters deep sleep again without enabling RTC timer
        RTC_Timer32Compare0Set (0);
        RTC_Timer32InterruptDisable(RTC_MODE0_INTENCLR_CMP0_Msk);
    }

    //Set wait for interrupt
    devie_SetWfi();
}

/* Perform the procedure of entering xtreme deep sleep mode.*/
void DEVICE_EnterExtremeDeepSleep(bool enableInt0)
{
    devie_SysUnlock();

    //Disable SERCOM TX Enable
    device_DisableSercom();

    //Disable All Peripherals with PMD Disable Bits
    device_disablePmd(false);

    //Gpio set Port A & B as input mode and pulled up Except for Interrupt
    Device_GpioConfig();

    // Config LP clock source as SOSC or LPRC
    if ((CFG_REGS->CFG_CFGCON4 & CFG_CFGCON4_VBKP_32KCSEL_Msk) == 0x2000U)   //LP clock source is set as SOSC, no need to switch
    {
        s_rtcClkFreq = DEVICE_RTC_CLOCK_FREQUENCY_32768HZ;     //Just update the static variable
    }
    else  //LP clock source is not set as SOSC, need to switch to LPRC
    {
        device_ConfigLpClkSrc(DEVICE_SOURCE_LPRC, DEVICE_RTC_CLK_FREQ_32768HZ);
    }

    //Disable retention ram
    DEVICE_SLEEP_ConfigRetRam(false);

    // Disable system timer
    devie_DisableSysTick();

    // Clear interrupt state
    device_DisableEic();

    //Disable RTCC wake up
    device_ConfigDsCtrlRtcc(false);

    //Disable DSWDT
    if ((CFG_REGS->CFG_CFGCON4 & CFG_CFGCON4_DSWDTEN_Msk) != 0U)
    {
        CFG_REGS->CFG_CFGCON4CLR = CFG_CFGCON4_DSWDTEN_Msk;
    }

    if (enableInt0)            //Determine if the device can be woken from XDS by INT0
    {
        device_ConfigInt0();
    }

    device_ConfigDeepSleepReg();

    //Disable interrupt
    devie_DisableInt();

    //Set wait for interrupt
    devie_SetWfi();
}
