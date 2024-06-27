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

#ifndef DEVICE_DEEP_SLEEP_H
#define DEVICE_DEEP_SLEEP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************

/** Various available deep Sleep Wakeup Sources */
typedef enum DEVICE_DeepSleepWakeSrc_T
{
    DEVICE_DEEP_SLEEP_WAKE_NONE,   // The device is not waken from deep sleep.
    DEVICE_DEEP_SLEEP_WAKE_INT0,   // The device is waken from deep sleep by interrupt 0.
    DEVICE_DEEP_SLEEP_WAKE_RTC,    // The device is waken from deep sleep by RTC.
    DEVICE_DEEP_SLEEP_WAKE_DSWDT,  // The device is waken from deep sleep by Deep Sleep Watch Dog Timeout.
    DEVICE_DEEP_SLEEP_WAKE_MCLR,   // The device is waken from deep sleep by MCLR.
    DEVICE_DEEP_SLEEP_WAKE_OTHER,  // The device is waken from deep sleep by the other reason.

    DEVICE_DEEP_SLEEP_WAKE_END
} DEVICE_DeepSleepWakeSrc_T;

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/**
 *@brief The API is used to clear the deep sleep related register. If the device is waken from deep sleep mode,
 *  the related register will be cleared. If it's a normal Power-on Reset. no register will be cleared.
 *
 *@param None
 *
 *@retval A boolean value, True means the device is waken from deep sleep mode.
*/
bool DEVICE_ClearDeepSleepReg(void);

// *****************************************************************************
/**
 *@brief The API is used to get the wake up source of deep sleep mode.
 *
 *@param[out] wakeUpSrc   -                      Pointer to the wake up source. See @ref DEVICE_DeepSleepWakeSrc_T.
 *
 *@retval None.
*/
void DEVICE_GetDeepSleepWakeUpSrc(DEVICE_DeepSleepWakeSrc_T *wakeUpSrc);

// *****************************************************************************
/**
 *@brief The API is used to set the wake up source of deep sleep mode.
 *
 *@param[in] wakeUpSrc   -                     Tthe wake up source. See @ref DEVICE_DeepSleepWakeSrc_T.
 *
 *@retval None.
*/
void DEVICE_SetDeepSleepWakeUpSrc(DEVICE_DeepSleepWakeSrc_T wakeUpSrc);

// *****************************************************************************
/**
 *@brief The API is used to enter deep sleep mode.
 *
 *@param[in] enableRetRam -                      Enable/Disable retention ram.
 *@param[in] interval     -                      The interval of deep sleep mode (unit: ms). 
                                                 Set as 0 will keep the device in the deep sleep mode until the INT0 (PB4) is triggered.
 *@param[out] None
 *
 *@retval None
*/
void DEVICE_EnterDeepSleep(bool enableRetRam,  uint32_t interval);

// *****************************************************************************
/**
 *@brief The API is used to perform the deep sleep interval calibration to exclude the HW preparation time of advertising.
 *
 *@param[in] expectedInt   -                    The expected deep sleep interval.
 *
 *@retval The deep sleep interval after calibration.
*/
uint32_t DEVICE_DeepSleepIntervalCal(uint32_t expectedInt);

// *****************************************************************************
/**
 *@brief The API is used to enter extreme deep sleep mode.
 *
 *@param[in] enableInt0 -                      Enable/Disable if the device can be woken from extreme deep sleep mode by INT0.
 *@param[out] None
 *
 *@retval None
*/
void DEVICE_EnterExtremeDeepSleep(bool enableInt0);

#ifdef __cplusplus
}
#endif

#endif//DEVICE_DEEP_SLEEP_H
