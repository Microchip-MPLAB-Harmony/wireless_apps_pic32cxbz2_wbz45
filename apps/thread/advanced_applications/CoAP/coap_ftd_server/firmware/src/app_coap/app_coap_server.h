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
  Application CoAP Server Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_coap_server.h

  Summary:
    This file contains the Application CoAP Server header file for this project.

  Description:
    This file contains the Application CoAP Server header file for this project.
 *******************************************************************************/

#ifndef _APP_COAP_SERVER_H    /* Guard against multiple inclusion */
#define _APP_COAP_SERVER_H


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

typedef void (*APP_COAP_ServerGetDataToSend_T)(uint8_t resourceID, uint8_t** dataBuffer,uint16_t* dataSize);
typedef void (*APP_COAP_CoapServerGetByteToRead_T)(uint8_t resourceID, uint16_t* bytesToRead);

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
	
void APP_CoapServerDataReceiveHandler(uint8_t resourceID, uint8_t* dataBuff);
void APP_CoapServerAddResource(void);
void APP_CoapServerRegisterCallback(APP_COAP_ServerGetDataToSend_T coapServerGetDataToSend, 
									APP_COAP_CoapServerGetByteToRead_T coapServerGetByteToRead);


/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _APP_COAP_SERVER_H */

/* *****************************************************************************
 End of File
 */
