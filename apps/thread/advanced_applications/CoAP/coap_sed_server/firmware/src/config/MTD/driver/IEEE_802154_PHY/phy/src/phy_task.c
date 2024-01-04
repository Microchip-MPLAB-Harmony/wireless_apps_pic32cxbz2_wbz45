/*******************************************************************************
  PHY Task

  File Name:
    phy_task.c

  Summary:
    This file contains PHY RTOS Task related fucntions

  Description:
    None
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
// DOM-IGNORE_END

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include "phy.h"
#include "definitions.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Externals                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

extern OSAL_SEM_HANDLE_TYPE semPhyInternalHandler;

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Prototypes                                                  */
/* ************************************************************************** */
/* ************************************************************************** */
void PHY_Tasks(void)
{  
    OSAL_RESULT result = OSAL_RESULT_TRUE;
    if (semPhyInternalHandler != NULL)    
    {
        result = OSAL_SEM_Pend(&semPhyInternalHandler, OSAL_WAIT_FOREVER);
        PHY_TaskHandler();
    }
    
    (void)result;
}

/* ************************************************************************** */
void PHY_PostTask(bool isISRContext)
{
    OSAL_RESULT result = OSAL_RESULT_TRUE;
    
    if(semPhyInternalHandler != NULL)
    {
        if(isISRContext)
        {
            result = OSAL_SEM_PostISR(&semPhyInternalHandler);
        }
        else
        {
            result = OSAL_SEM_Post(&semPhyInternalHandler);
        }
    }
    else
    {
        PHY_TaskHandler();
    }
    
    (void)result;
}

/* *****************************************************************************
 End of File
 */
