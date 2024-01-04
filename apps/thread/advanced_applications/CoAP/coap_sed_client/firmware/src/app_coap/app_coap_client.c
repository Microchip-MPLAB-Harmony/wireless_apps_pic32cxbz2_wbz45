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
  Application CoAP Client Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_coap_client.c

  Summary:
    This file contains the Application CoAP Client Source code for this project.

  Description:
    This file contains the Application CoAP Client Source code for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_coap_client.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
static uint16_t s_byteToRead = 0;

static APP_COAP_Resource_T appCoapResources[APP_RESOURCE_TOTAL] = 
{
    {APP_LED_RESOURCE,{"led",NULL,NULL,NULL}},
};


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void app_coapClientRespHandler(void *pcontext, otMessage * pmessage,
                                   const otMessageInfo *pmessageinfo, otError result)
{
    APP_Msg_T   appMsg;
    APP_Msg_T   *p_appMsg;
    
    do
    {
        if(otCoapMessageGetType(pmessage) == OT_COAP_TYPE_ACKNOWLEDGMENT)
        {
            otCoapCode coapcode = otCoapMessageGetCode(pmessage);
            
            appMsg.msgData[0] = *((int*)pcontext);
            
            if(coapcode  == OT_COAP_CODE_CHANGED)
            {
                    appMsg.msgId = APP_COAP_CLIENT_ACK_RECEIVED;
                    p_appMsg = &appMsg;
                    OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0); 
            }
            else if(coapcode  == OT_COAP_CODE_CONTENT)
            {
                if(otMessageRead(pmessage, otMessageGetOffset(pmessage), &appMsg.msgData[1], s_byteToRead) != s_byteToRead)
                {
                   break; 
                }
                else
                {
                    appMsg.msgId = APP_COAP_CLIENT_DATA_RECEIVED;

                    p_appMsg = &appMsg;
                    OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0);    
                }
            }
        }
    }while (false);
}

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_CoapClientPutReq(APP_COAP_ResourceId_T resourceID, uint8_t* dataBuffer, uint16_t dataBuffSize)
{
    APP_COAP_Req_T coapReq;
    coapReq.coapCode = OT_COAP_CODE_PUT;
    coapReq.coapType = OT_COAP_TYPE_CONFIRMABLE;
    coapReq.coapUriPath = (char*)appCoapResources[resourceID].resouce.mUriPath;
    coapReq.coapData = dataBuffer;
    coapReq.coapDataSize = dataBuffSize;
    coapReq.coapResponseHandler = app_coapClientRespHandler;
    coapReq.coapContext = (void *)&appCoapResources[resourceID].resourceID;
    APP_CoapRequestSend(&coapReq);
}

void APP_CoapClientGetReq(APP_COAP_ResourceId_T resourceID, uint16_t byteToRead)
{
    APP_COAP_Req_T coapReq;
    s_byteToRead = byteToRead;
    coapReq.coapCode = OT_COAP_CODE_GET;
    coapReq.coapType = OT_COAP_TYPE_CONFIRMABLE;
    coapReq.coapUriPath = (char*)appCoapResources[resourceID].resouce.mUriPath;
    coapReq.coapData = NULL;
    coapReq.coapResponseHandler = app_coapClientRespHandler;
    coapReq.coapContext = (void *)&appCoapResources[resourceID].resourceID;
    APP_CoapRequestSend(&coapReq);    
}

/* *****************************************************************************
 End of File
 */
