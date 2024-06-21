/*******************************************************************************
 System Idle Task File

  File Name:
    app_idle_task.c

  Summary:
    This file contains source code necessary for FreeRTOS idle task

  Description:

  Remarks:
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

#include "definitions.h"
/*-----------------------------------------------------------*/

/* Ensure the SysTick is clocked at the same frequency as the core. */
#define APP_IDLE_SYSTICK_CLOCK_HZ                   configCPU_CLOCK_HZ

/* The systick is a 24-bit counter. */
#define APP_IDLE_MAX_24_BIT_NUMBER                  ( 0xffffffUL )

/*
 * A factor to estimate the duration time between RTC timer and Application timer expired
 * while the SysTick counter is stopped during tickless idle calculations.
 * (SOSC is used as the low power clock.)
 */
#define APP_IDLE_RTC_TIMER_FACTOR_SOSC                   ( 22UL )   //2.2ms

/*
 * A factor to estimate the duration time between RTC timer and Application timer expired
 * while the SysTick counter is stopped during tickless idle calculations.
 * (POSC is used as the low power clock.)
 */
#define APP_IDLE_RTC_TIMER_FACTOR_POSC                   ( 12UL )   //1.2ms

#define MAX_SLEEP_ALLOWED   (60*60*1000L) //1 hour
/* The RTC is a 32-bit counter. */
#define APP_IDLE_MAX_32_BIT_NUMBER                  ( 0xffffffffUL )

/* RTC clock frequency . */
#define APP_IDLE_RTC_CLOCK_FREQUENCY_32K                 ( 32000U )

/* Constants required to manipulate the core.  Registers first... */
#define APP_IDLE_NVIC_SYSTICK_CTRL_REG           ( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define APP_IDLE_NVIC_SYSTICK_LOAD_REG           ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define APP_IDLE_NVIC_SYSTICK_CURRENT_VALUE_REG  ( * ( ( volatile uint32_t * ) 0xe000e018 ) )

/* ...then bits in the registers. */
#define APP_IDLE_NVIC_SYSTICK_ENABLE_BIT         ( 1UL << 0UL )
#define APP_IDLE_NVIC_SYSTICK_INT_BIT            ( 1UL << 1UL )
#define APP_IDLE_NVIC_SYSTICK_CLK_BIT            ( 1UL << 2UL )
#define APP_IDLE_NVIC_SYSTICK_COUNT_FLAG_BIT     ( 1UL << 16UL )
#define APP_IDLE_NVIC_PENDSVCLEAR_BIT            ( 1UL << 27UL )
#define APP_IDLE_NVIC_PEND_SYSTICK_CLEAR_BIT     ( 1UL << 25UL )


/*
 * The number of SysTick increments that make up one tick period.
 */
static uint32_t ulTimerCountsForOneTick = 0;

/*
 * The number of RTC count increments that make up one tick period.
 */
static uint32_t ulRtcCountsForOneTick = 0UL;

static bool s_rtcIntFlag;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 32 bit resolution of RTC.
 */
static uint32_t xMaximumPossibleSuppressedTicksRtc = 0UL;

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
static uint32_t s_ulStoppedTimerCompensationRtcP = 0UL;
static uint32_t s_ulStoppedTimerCompensationRtcS = 0UL;

static uint32_t s_rtcCntBeforeSleep = 0UL;
static bool s_chkRtcCnt;

extern bool deviceCanSleep;

void app_idle_task( void )
{
    uint8_t PDS_Items_Pending = PDS_GetPendingItemsCount();
    bool RF_Cal_Needed = RF_NeedCal(); // device_support library API
    uint8_t BT_RF_Suspended = 0;

    if (PDS_Items_Pending || RF_Cal_Needed)
    {
        OSAL_CRITSECT_DATA_TYPE IntState;
        IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
        BT_RF_Suspended = BT_SYS_RfSuspendReq(1);
        //once BT_RF_Suspended is true, BT internal RF_Suspend_Req_Flag will be set,
        //and BT is forbidden to prepare RF.
        OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, IntState);


        if (BT_RF_Suspended && MAC_ReadyToSleep())
        {
            if (PDS_Items_Pending)
            {
                PDS_StoreItemTaskHandler();
            }
            if ((RF_Cal_Needed) && (BT_RF_Suspended == BT_SYS_RF_SUSPENDED_NO_SLEEP))
            {
            
               PHY_TrxStatus_t trxStatus = PHY_GetTrxStatus();
               OSAL_CRITSECT_DATA_TYPE intStatus;
               if (trxStatus == PHY_TRX_SLEEP)
               {
                   PHY_TrxWakeup();
                   intStatus = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_LOW);
                   RF_Timer_Cal(WSS_ENABLE_BLE_ZB);
                   OSAL_CRIT_Leave(OSAL_CRIT_TYPE_LOW, intStatus);
                   PHY_TrxSleep(SLEEP_MODE_1);
               }
               else
               {
                   RF_Timer_Cal(WSS_ENABLE_BLE_ZB);
               }
            

            }
            BT_SYS_RfSuspendReq(0);
        }
    }
    /*
      Request BT to enter sleep mode, BLE stack will check if it can enter or not.
      Return true means that BT enters sleep mode.
    */
    BT_SYS_EnterSleepMode(RTC_Timer32FrequencyGet(), RTC_Timer32CounterGet());
}



/* 
   Record RTC counter value in each tick interrupt to ensure the real time RTC counter value be recorded
   during system is active. Then RTC tickless idle mode can use this value to calculate how much tim passed
   during system sleep.
*/
void app_idle_updateRtcCnt(uint32_t cnt)
{
    s_rtcCntBeforeSleep = cnt;
    s_chkRtcCnt = true;
}

/* RTC callback event handler */
static void app_idle_RtcHandler(RTC_TIMER32_INT_MASK intCause, uintptr_t context)
{
    if (RTC_MODE0_INTENSET_CMP0_Msk & intCause )
    {
        s_rtcIntFlag = true;
    }
}

/* Register RTC callback function */
static void app_idle_RtcInit(void)
{
    RTC_Timer32CallbackRegister(app_idle_RtcHandler, 0);
    s_rtcIntFlag = false;
}

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void vPortSetupTimerInterrupt( void )
{
    /* Calculate the constants required to configure the tick interrupt. */

    ulTimerCountsForOneTick = ( APP_IDLE_SYSTICK_CLOCK_HZ / configTICK_RATE_HZ );

    /*
       RTC count per one ms (CNT/ms) = RTC Clock / 1000 
       RTC count per one tick (CNT/tick) = RTC Clock / 1000 * ticks per ms
       ulRtcCountsForOneTick = (RTC_Timer32FrequencyGet() / configTICK_RATE_HZ) * portTICK_PERIOD_MS;
    */
    ulRtcCountsForOneTick = (RTC_Timer32FrequencyGet() + (configTICK_RATE_HZ / 2)) / configTICK_RATE_HZ;

    xMaximumPossibleSuppressedTicksRtc = APP_IDLE_MAX_32_BIT_NUMBER / ulRtcCountsForOneTick;

    s_ulStoppedTimerCompensationRtcP = 0UL;
    s_ulStoppedTimerCompensationRtcS = 0UL;

    /* 
       Calculate the compensation value for RTC timer based on the LPCLK source selection.
       No need to do compensation if RTC clock is set as 1kHz or 1024 Hz due to time scale is larger
       so that even a slight compensation will result in worse timer accuracy.
    */
    if (RTC_Timer32FrequencyGet() >= APP_IDLE_RTC_CLOCK_FREQUENCY_32K)
    {
        if ((CFG_REGS->CFG_CFGCON4 & CFG_CFGCON4_VBKP_32KCSEL_Msk) == 0x2000) //SOSC : XTAL_OFF
        {
            s_ulStoppedTimerCompensationRtcS = (RTC_Timer32FrequencyGet() * APP_IDLE_RTC_TIMER_FACTOR_SOSC) / (configTICK_RATE_HZ * 10);
        }
        else   //POSC
        {
            s_ulStoppedTimerCompensationRtcP = (RTC_Timer32FrequencyGet() * APP_IDLE_RTC_TIMER_FACTOR_POSC) / (configTICK_RATE_HZ * 10);
        }
    }

    s_chkRtcCnt = true;

    app_idle_RtcInit();

    /* Stop and clear the SysTick. */
    APP_IDLE_NVIC_SYSTICK_CTRL_REG = 0UL;
    APP_IDLE_NVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

    /* Configure SysTick to interrupt at the requested rate. */
    APP_IDLE_NVIC_SYSTICK_LOAD_REG = ( APP_IDLE_SYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
    APP_IDLE_NVIC_SYSTICK_CTRL_REG = ( APP_IDLE_NVIC_SYSTICK_CLK_BIT | APP_IDLE_NVIC_SYSTICK_INT_BIT | APP_IDLE_NVIC_SYSTICK_ENABLE_BIT );
}

/* Main implementation of RTC based Tickless Idle Mode */
void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    bool isSystemCanSleep = false;

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if( eTaskConfirmSleepModeStatus() == eAbortSleep )
    {
        return;
    }

    /* Check if BT allow system to enter sleep mode */
    if (BT_SYS_AllowSystemSleep(RTC_Timer32FrequencyGet(), RTC_Timer32CounterGet()))
    {
        isSystemCanSleep = true;
    }

    /* Allow system to enter sleep mode */
    if(isSystemCanSleep && MAC_ReadyToSleep() && deviceCanSleep && (xExpectedIdleTime > 512) && (xExpectedIdleTime < MAX_SLEEP_ALLOWED))
    {
        /* Stop the SysTick momentarily.  The time the SysTick is stopped for
        is accounted for as best it can be, but using the tickless mode will
        inevitably result in some tiny drift of the time maintained by the
        kernel with respect to calendar time. */
        APP_IDLE_NVIC_SYSTICK_CTRL_REG &= ~APP_IDLE_NVIC_SYSTICK_ENABLE_BIT;

        /* Enter a critical section but don't use the taskENTER_CRITICAL()
        method as that will mask interrupts that should exit sleep mode. */

        __asm volatile( "cpsid i" ::: "memory" );
        __asm volatile( "dsb" );
        __asm volatile( "isb" );

        /* Back up PMU mode */
        PMU_Mode_T pmuMode = PMU_Get_Mode();

        /* Set PMU as BUCK PSM mode if it's not in MLDO mode.
        If it's in MLDO mode, do not perform mode switch to PSM. */
        if (pmuMode != PMU_MODE_MLDO)
        {
            PMU_Set_Mode(PMU_MODE_BUCK_PSM);

            /* Disable current sensor to improve current consumption. */
            PMU_ConfigCurrentSensor(false);
        }

        /* Enter system sleep mode */
		DEVICE_EnterDeepSleep(true, xExpectedIdleTime);
    }
}

/*-----------------------------------------------------------*/
/*******************************************************************************
 End of File
 */
