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

/*******************************************************************************
  Application CoAP Client Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_coap_client.h

  Summary:
    This file contains the Application CoAP Client header file for this project.

  Description:
    This file contains the Application CoAP Client header file for this project.
 *******************************************************************************/

#ifndef _APP_COAP_CLIENT_H    /* Guard against multiple inclusion */
#define _APP_COAP_CLIENT_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app_coap_common.h"


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_CoapClientAckReceiveHandler(uint8_t resourceID);
void APP_CoapClientDataReceiveHandler(uint8_t resourceID, uint8_t* dataBuff);
void APP_CoapClientPutReq(APP_COAP_ResourceId_T resourceID, uint8_t* dataBuffer, uint16_t dataBuffSize);
void APP_CoapClientGetReq(APP_COAP_ResourceId_T resourceID, uint16_t byteToRead);


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _APP_COAP_CLIENT_H */

/* *****************************************************************************
 End of File
 */
