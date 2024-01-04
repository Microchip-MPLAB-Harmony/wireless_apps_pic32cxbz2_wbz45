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
  Application CoAP Server Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_coap_server.c

  Summary:
    This file contains the Application CoAP Server Source code for this project.

  Description:
    This file contains the Application CoAP server Source code for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app_coap_server.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

static APP_COAP_Resource_T appCoapResources[APP_RESOURCE_TOTAL] = 
{
    {APP_LED_RESOURCE,{"led",NULL,NULL,NULL}},
};

static APP_COAP_ServerGetDataToSend_T appCoapServerGetDataToSend;
static APP_COAP_CoapServerGetByteToRead_T appCoapServerGetByteToRead;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void app_CoapReqHandler(void * pcontext, otMessage * pmessage,
                               const otMessageInfo * pmessageinfo)
{
    APP_Msg_T   appMsg;
    APP_Msg_T   *p_appMsg;
    uint16_t byteToRead = 0;
    uint8_t resourceID = 0;
    
    do
    {
        resourceID = *((int*)pcontext);
        if (otCoapMessageGetCode(pmessage) == OT_COAP_CODE_GET)
        {
            if (otCoapMessageGetType(pmessage) == OT_COAP_TYPE_CONFIRMABLE) 
            {
              APP_COAP_Resp_T coapResp;
              coapResp.pmessage = pmessage;
              coapResp.pmessageinfo = pmessageinfo;
              coapResp.coapCode = OT_COAP_CODE_CONTENT;
              coapResp.coapType = OT_COAP_TYPE_ACKNOWLEDGMENT;
              
              if(appCoapServerGetDataToSend != NULL)
                appCoapServerGetDataToSend(resourceID,&coapResp.coapData,&coapResp.coapDataSize);
              
              APP_CoapResponseSend(&coapResp);
            }
        }

        else if (otCoapMessageGetCode(pmessage) == OT_COAP_CODE_PUT)
        {
            appMsg.msgData[0] = resourceID;
            if(appCoapServerGetDataToSend != NULL)
                appCoapServerGetByteToRead(resourceID,&byteToRead);
             if(otMessageRead(pmessage, otMessageGetOffset(pmessage), &appMsg.msgData[1], byteToRead) != byteToRead)
            {
               break; 
            }
            else
            {
                appMsg.msgId = APP_COAP_SERVER_DATA_RECEIVED;

                p_appMsg = &appMsg;
                OSAL_QUEUE_Send(&appData.appQueue, p_appMsg, 0); 
            }
            
            if (otCoapMessageGetType(pmessage) == OT_COAP_TYPE_CONFIRMABLE) 
            {
              APP_COAP_Resp_T coapResp;
              coapResp.pmessage = pmessage;
              coapResp.pmessageinfo = pmessageinfo;
              coapResp.coapCode = OT_COAP_CODE_CHANGED;
              coapResp.coapType = OT_COAP_TYPE_ACKNOWLEDGMENT;
              coapResp.coapData = NULL;
              APP_CoapResponseSend(&coapResp);
            }
        }
        else
        {
            break;
        }

    } while (false);
}


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_CoapServerRegisterCallback(APP_COAP_ServerGetDataToSend_T coapServerGetDataToSend, 
                                    APP_COAP_CoapServerGetByteToRead_T coapServerGetByteToRead)
{
   appCoapServerGetDataToSend =  coapServerGetDataToSend;
   appCoapServerGetByteToRead = coapServerGetByteToRead;
}

void APP_CoapServerAddResource(void)
{
    for(uint8_t i = 0; i < APP_RESOURCE_TOTAL; i++)
    {
        appCoapResources[i].resouce.mHandler = app_CoapReqHandler;
        appCoapResources[i].resouce.mContext = (void *)&appCoapResources[i].resourceID;
        appCoapResources[i].resouce.mNext    = NULL;
        APP_CoapAddResource(&appCoapResources[i].resouce);
    }
}


/* *****************************************************************************
 End of File
 */
