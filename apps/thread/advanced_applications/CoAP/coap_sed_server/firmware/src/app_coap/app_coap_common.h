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
  Application CoAP Common Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_coap_common.h

  Summary:
    This file contains the Application CoAP Common header file for this project.

  Description:
    This file contains the Application CoAP Common header file for this project.
 *******************************************************************************/

#ifndef _APP_COAP_COMMON_H    /* Guard against multiple inclusion */
#define _APP_COAP_COMMON_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "openthread/coap.h"
#include "definitions.h"


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
    
#define APP_COAP_PORT 5683


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

typedef struct APP_COAP_Req_T
{
	otCoapCode coapCode;
	otCoapType coapType;
	char* coapUriPath;
	uint8_t* coapData;
	uint16_t coapDataSize;
	otCoapResponseHandler coapResponseHandler;
	void* coapContext;
} APP_COAP_Req_T;

typedef struct APP_COAP_Resp_T
{
	otMessage * pmessage;
	const otMessageInfo * pmessageinfo;
	otCoapCode coapCode;
	otCoapType coapType;
	uint8_t* coapData;
	uint16_t coapDataSize;
} APP_COAP_Resp_T;

/**@brief The definition of Resource ID. */
typedef enum APP_COAP_ResourceId_T
{
	APP_LED_RESOURCE,
	APP_RESOURCE_TOTAL
} APP_COAP_ResourceId_T;

typedef struct APP_COAP_Resource_T
{
   APP_COAP_ResourceId_T resourceID;
   otCoapResource resouce;
}APP_COAP_Resource_T;


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_CoapStart(void);
void APP_CoapRequestSend(APP_COAP_Req_T* appCoapReq);
void APP_CoapAddResource(otCoapResource* coapResource);
void APP_CoapResponseSend(APP_COAP_Resp_T* appCoapResp);
    


/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _APP_COAP_COMMON_H */

/* *****************************************************************************
 End of File
 */
