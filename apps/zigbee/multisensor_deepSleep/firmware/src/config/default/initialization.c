/*******************************************************************************
  System Initialization File

  File Name:
    initialization.c

  Summary:
    This file contains source code necessary to initialize the system.

  Description:
    This file contains source code necessary to initialize the system.  It
    implements the "SYS_Initialize" function, defines the configuration bits,
    and allocates any necessary global system resources,
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
#include "configuration.h"
#include "definitions.h"
#include "device.h"


// ****************************************************************************
// ****************************************************************************
// Section: Configuration Bits
// ****************************************************************************
// ****************************************************************************

/*** FUSERID ***/
#pragma config USER_ID =      0xffffU

/*** DEVCFG0 ***/
#pragma config TDOEN =      ON
#pragma config SWOEN =      ON
#pragma config TROEN =      OFF
#pragma config ADCPOVR =      HW
#pragma config ACCMP1_ALTEN =      PA1
#pragma config CPENFILT =      OFF
#pragma config RTCIN0_ALTEN =      PA3
#pragma config RTCOUT_ALTEN =      PA4
#pragma config VBCMODE =      DIRECT
#pragma config SMBUSEN0 =      OFF
#pragma config SMBUSEN1 =      OFF
#pragma config SMBUSEN2 =      OFF
#pragma config HPLUGDIS =      OFF
#pragma config SLRTEN0 =      OFF
#pragma config SLRTEN1 =      OFF
#pragma config SLRTEN2 =      OFF
#pragma config PCM =      SFR
#pragma config INT0E =      OFF
#pragma config INT0P =      NEG
#pragma config FECCCON =         OFF
#pragma config FRECCDIS =      OFF


/*** DEVCFG1 ***/
#pragma config ICESEL =      PGC1_PGD1
#pragma config TRCEN =      ON
#pragma config ZBTWKSYS =      OFF
#pragma config CMP0_OE =      OFF
#pragma config CMP1_OE =      OFF
#pragma config CLASSBDIS =      OFF
#pragma config SLRCTRL0 =      GPIO
#pragma config SLRCTRL1 =      GPIO
#pragma config SLRCTRL2 =      GPIO
#pragma config SMCLR =      NO_POR
#pragma config QSCHE_EN =      OFF
#pragma config QSPI_HSEN =      PPS
#pragma config SCOM0_HSEN =      DIRECT
#pragma config SCOM1_HSEN =      PPS
#pragma config SCOM2_HSEN =      PPS
#pragma config CCL_OE =      ON
#pragma config I2CDSEL0 =      OFF
#pragma config I2CDSEL1 =      OFF
#pragma config I2CDSEL2 =      OFF
#pragma config WDTPSS =      PSS1048576
#pragma config QSPIDDRM =      OFF
#pragma config CLKZBREF =      OFF

/*** DEVCFG2 ***/
#pragma config ACMP_CYCLE =      _32US
#pragma config DMTINTV =      WIN_127_128
#pragma config PMUTEST_VDD_EN =      VDD_DIV_2
#pragma config POSCMOD =      HS
#pragma config WDTRMCS =      LPRC
#pragma config SOSCSEL =      XTAL
#pragma config WAKE2SPD =      ON
#pragma config CKSWEN =      ON
#pragma config FSCMEN =      ON
#pragma config WDTPSR =      PS1048576
#pragma config WDTSPGM =      STOPS
#pragma config WINDIS =      NORMAL
#pragma config WDTEN =      OFF
#pragma config WDTWINSZ =    WINSZ_25
#pragma config DMTCNT =      DMT31
#pragma config DMTEN =      OFF


/*** DEVCFG4 ***/
#pragma config SOSCCFG =    0x0U
#pragma config RTCEVENT_SEL =      ONE_SEC
#pragma config RTCEVENT_EN =      OFF
#pragma config VBKP_1KCSEL =      _32K
#pragma config VBKP_32KCSEL =      SOSC
#pragma config VBKP_DIVSEL =      DIV_31_25
#pragma config LPCLK_MOD =      DIV_1_024
#pragma config RTCEVTYPE =      OUT
#pragma config CPEN_DLY =      _8_LPRC
#pragma config DSZPBOREN =      OFF
#pragma config DSWDTPS =      DSPS5
#pragma config DSWDTOSC =    LPRC
#pragma config DSWDTEN =      OFF
#pragma config DSEN =    OFF
#pragma config UVREGROVR =      CONTROLLED
#pragma config RTCNTM_CSEL =      RAW

/*** FBCFG0 ***/
#pragma config BINFOVALID =      VALID
#pragma config PCSCMODE =      DUAL

/*** FCPN0 ***/
#pragma config CP =      DISABLED





// *****************************************************************************
// *****************************************************************************
// Section: Driver Initialization Data
// *****************************************************************************
// *****************************************************************************
/* Following MISRA-C rules are deviated in the below code block */
/* MISRA C-2012 Rule 11.1 */
/* MISRA C-2012 Rule 11.3 */
/* MISRA C-2012 Rule 11.8 */



// *****************************************************************************
// *****************************************************************************
// Section: System Data
// *****************************************************************************
// *****************************************************************************
/* Structure to hold the object handles for the modules in the system. */
SYSTEM_OBJECTS sysObj;

// *****************************************************************************
// *****************************************************************************
// Section: Library/Stack Initialization Data
// *****************************************************************************
// *****************************************************************************
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

#define QUEUE_LENGTH_ZIGBEE (16)

#define QUEUE_ITEM_SIZE_ZIGBEE (sizeof(void *))

OSAL_QUEUE_HANDLE_TYPE zigbeeRequestQueueHandle;

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

OSAL_API_LIST_TYPE     osalAPIList;




// *****************************************************************************
// *****************************************************************************
// Section: System Initialization
// *****************************************************************************
// *****************************************************************************
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



/*******************************************************************************
  Function:
    void PCHE_Setup ( void )

  Summary:
    Configure the PCHE_CHECON register based on the configured system clock.

  Remarks:
    This ramfunc function requires XC32 compiler v3.00 or greater.

    The ramfunc attribute is required to allow sufficient time after clearing
    the ADRWS bit before any other bus activity occurs. Refer to Prefetch
    cache module section in PIC32CX-BZ2 Family Errata.
*/

__attribute__((ramfunc, long_call, section(".ramfunc"),unique_section)) void PCHE_Setup(void)
{

    // Set Flash Wait states and enable pre-fetch
    // clear PFMWS and ADRWS
    PCHE_REGS->PCHE_CHECON = (PCHE_REGS->PCHE_CHECON & (~(PCHE_CHECON_PFMWS_Msk | PCHE_CHECON_ADRWS_Msk | PCHE_CHECON_PREFEN_Msk))) 
                                    | (PCHE_CHECON_PFMWS(1) | PCHE_CHECON_PREFEN(1));
    // write completion delay
    for(int i=1; i<10; i++)
    {
        asm ("NOP");
    }

}
void _on_reset(void)
{
    //Need to clear register before configure any GPIO
    DEVICE_ClearDeepSleepReg();

    // Initialize the RF Clock Generator
    SYS_ClkGen_Config();

    /* Can't call a RAM function before __pic32c_data_initialization
       Must call a flash function, but in A0 HW version, RAM function is required to avoid HW issue.
       Thus, will not config PCHE before __pic32c_data_initialization in A0 version.
    */
    // Configure Prefetch, Wait States
    if (DSU_REGS->DSU_DID & DSU_DID_REVISION_Msk)   //A1 and later version
    {
        PCHE_REGS->PCHE_CHECON = (PCHE_REGS->PCHE_CHECON & (~(PCHE_CHECON_PFMWS_Msk | PCHE_CHECON_ADRWS_Msk | PCHE_CHECON_PREFEN_Msk))) 
                                        | (PCHE_CHECON_PFMWS(1) | PCHE_CHECON_PREFEN(1));
    }
}




// *****************************************************************************
// *****************************************************************************
// Section: Local initialization functions
// *****************************************************************************
// *****************************************************************************

/* MISRAC 2012 deviation block end */

/*******************************************************************************
  Function:
    void SYS_Initialize ( void *data )

  Summary:
    Initializes the board, services, drivers, application and other modules.

  Remarks:
 */

void SYS_Initialize ( void* data )
{

    /* MISRAC 2012 deviation block start */
    /* MISRA C-2012 Rule 2.2 deviated in this file.  Deviation record ID -  H3_MISRAC_2012_R_2_2_DR_1 */

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


  
    CLOCK_Initialize();
    /* Configure Prefetch, Wait States */
    PCHE_REGS->PCHE_CHECON = (PCHE_REGS->PCHE_CHECON & (~(PCHE_CHECON_PFMWS_Msk | PCHE_CHECON_ADRWS_Msk | PCHE_CHECON_PREFEN_Msk)))
                                    | (PCHE_CHECON_PFMWS(1) | PCHE_CHECON_PREFEN(1));


	uint8_t deepSleepWakeupSrc;
	
	DEVICE_DeepSleepWakeSrc_T wakeSrc;
	DEVICE_GetDeepSleepWakeUpSrc(&wakeSrc);

	deepSleepWakeupSrc = (uint8_t) wakeSrc;
	if ((wakeSrc == DEVICE_DEEP_SLEEP_WAKE_NONE) || (wakeSrc == DEVICE_DEEP_SLEEP_WAKE_MCLR))  
	{
		deepSleepWakeupSrc = (uint8_t) DEVICE_DEEP_SLEEP_WAKE_NONE;
		RTC_Initialize();
	}


	GPIO_Initialize();

    EVSYS_Initialize();

    TC0_TimerInitialize();

    NVM_Initialize();

	TRNG_Initialize();



    /* MISRAC 2012 deviation block start */
    /* Following MISRA-C rules deviated in this block  */
    /* MISRA C-2012 Rule 11.3 - Deviation record ID - H3_MISRAC_2012_R_11_3_DR_1 */
    /* MISRA C-2012 Rule 11.8 - Deviation record ID - H3_MISRAC_2012_R_11_8_DR_1 */

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
    // Initialize PDS- Persistent Data Server
    PDS_Init(MAX_PDS_ITEMS_COUNT, MAX_PDS_DIRECTORIES_COUNT);


    // Initialize RF System
    SYS_Load_Cal(WSS_ENABLE_ZB);
 
    // Set up OSAL for RF Stack Library usage
    osalAPIList.OSAL_CRIT_Enter      = OSAL_CRIT_Enter;
    osalAPIList.OSAL_CRIT_Leave      = OSAL_CRIT_Leave;

    osalAPIList.OSAL_SEM_Create      = OSAL_SEM_Create;
    osalAPIList.OSAL_SEM_Pend        = OSAL_SEM_Pend;
    osalAPIList.OSAL_SEM_Post        = OSAL_SEM_Post;
    osalAPIList.OSAL_SEM_PostISR     = OSAL_SEM_PostISR;
    osalAPIList.OSAL_SEM_GetCount    = OSAL_SEM_GetCount;

    osalAPIList.OSAL_QUEUE_Create    = OSAL_QUEUE_Create;
    osalAPIList.OSAL_QUEUE_Send      = OSAL_QUEUE_Send;
    osalAPIList.OSAL_QUEUE_SendISR   = OSAL_QUEUE_SendISR;
    osalAPIList.OSAL_QUEUE_Receive   = OSAL_QUEUE_Receive;
    osalAPIList.OSAL_QUEUE_IsFullISR = OSAL_QUEUE_IsFullISR;
    osalAPIList.OSAL_QUEUE_CreateSet = OSAL_QUEUE_CreateSet;
    osalAPIList.OSAL_QUEUE_AddToSet  = OSAL_QUEUE_AddToSet;
    osalAPIList.OSAL_QUEUE_SelectFromSet = OSAL_QUEUE_SelectFromSet;

    osalAPIList.OSAL_MemAlloc = OSAL_Malloc;
    osalAPIList.OSAL_MemFree = OSAL_Free;





	// Create ZIGBEE Stack Message QUEUE
    (void)OSAL_QUEUE_Create(&zigbeeRequestQueueHandle, QUEUE_LENGTH_ZIGBEE, QUEUE_ITEM_SIZE_ZIGBEE);

    // Retrieve Zigbee's data from Information Base
    ZB_CS_SYS_IBData_t zgbIBdata = {0};

    zgbIBdata.validityCheck.extMacDevAddrValid = IB_GetMACAddr(&zgbIBdata.extMacDevAddr[0]);
    zgbIBdata.validityCheck.antennaGainValid = IB_GetAntennaGain((uint8_t *)&zgbIBdata.antGain); // should be changed to 8bit

    // Initialize ZIGBEE Stack
    Zigbee_Init(&osalAPIList, &zigbeeRequestQueueHandle, NULL, &zgbIBdata);

    // pass the above read wakep src for the stack to use
	CS_WriteParameter(CS_DEVICE_DEEP_SLEEP_WAKEUP_SRC_ID, &deepSleepWakeupSrc);
    //uint8_t value = PMU_Set_Mode(PMU_MODE_BUCK_PWM); //Set PMU as PWM mode
    //PMU_Set_Mode(PMU_MODE_BUCK_PWM); //Set PMU as PWM mode // For Buck Mode : PMU_MODE_MLDO

    CRYPT_WCCB_Initialize();

    /* MISRAC 2012 deviation block end */
    APP_Initialize();


    NVIC_Initialize();


    /* MISRAC 2012 deviation block end */
}

/*******************************************************************************
 End of File
*/
