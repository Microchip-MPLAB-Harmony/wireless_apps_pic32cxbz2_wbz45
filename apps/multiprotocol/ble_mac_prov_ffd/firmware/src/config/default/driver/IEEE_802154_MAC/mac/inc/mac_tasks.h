/*******************************************************************************
  MAC Task Header

  File Name:
    mac_tasks.h

  Summary:
    This file contains MAC-RTOS task related functions

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
// DOM-IGNORE-END

/* Prevent double inclusion */
#ifndef MAC_TASKS_H
#define MAC_TASKS_H


// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    void WPAN_Tasks(void)

  Summary:
    RTOS task for the MAC Layer

  Description:
    This function calls the MAC layer task handler upon reception of 
	Semaphore signal
 
  Precondition:

  Parameters:
    None

  Returns:
    None

  Example:
    <code>
	xTaskCreate((TaskFunction_t) _WPAN_Tasks,
                "WPAN_Tasks",
                1024,
                NULL,
                1,
                &xWPAN_Tasks);
				
	TaskHandle_t xWPAN_Tasks;

	void _WPAN_Tasks(  void *pvParameters  )
	{     
		while(1)
		{
			WPAN_Tasks();
		}
	}
    
    </code>

  Remarks:
    None 
*/

void WPAN_Tasks(void);

// *****************************************************************************
/*
  Function:
    void WPAN_PostTask(void)

  Summary:
    This function is used to invoke the MAC RTOS task

  Description:
    This function is used to invoke the MAC RTOS task. This will be used 
	by the MAC layer for signaling the task done status
 
  Precondition:

  Parameters:

  Returns:
    None

  Example:
	None

  Remarks:
    None 
*/

void WPAN_PostTask(void);


#endif /* MAC_TASKS_H */

/* EOF */
