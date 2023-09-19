/*******************************************************************************
  ZCC0 Custom Cluster Header File

  Company:
    Microchip Technology Inc.

  File Name:
   customFlowratemonitorCluster.h

  Summary:
    This file contains the Custom Cluster interface.

  Description:
    This file contains the Custom Cluster interface.
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

/******************************************************************************
                    Includes section
******************************************************************************/

#include <zcl/include/zclFlowratemonitorCluster.h>

/******************************************************************************
                    Definition(s) section
******************************************************************************/
#define FLOWRATEMONITOR_MIN_REPORT_PERIOD 5
#define FLOWRATEMONITOR_MAX_REPORT_PERIOD 10

/*******************************************************************************
                    Prototypes section
*******************************************************************************/

void customFlowratemonitorInit(void);

/******************************************************************************
                    Global variables
******************************************************************************/

//Custom Cluster Attributes
extern ZCL_FlowratemonitorClusterServerAttributes_t flowratemonitorServerClusterAttributes;
//Custom Cluster Commands
extern PROGMEM_DECLARE (ZCL_FlowratemonitorClusterCommands_t flowratemonitorServerClusterCommands);



