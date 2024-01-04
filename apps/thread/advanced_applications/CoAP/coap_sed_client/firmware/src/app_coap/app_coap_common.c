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
  Application CoAP Common Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_coap_common.c

  Summary:
    This file contains the Application CoAP Common for this project.

  Description:
    This file contains the Application CoAP Common for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include "openthread/thread.h"
#include "definitions.h"
#include "../app_thread/app_thread_common.h"
#include "app_coap_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_CoapStart(void)
{
    otError error = OT_ERROR_NONE;

    otInstance * pinstance = APP_ThreadGetInstance();
    error = otCoapStart(pinstance, APP_COAP_PORT);
    if(error == OT_ERROR_NONE)
    {     
      /* Add debug print if required.*/ 
    }   
}

void APP_CoapRequestSend(APP_COAP_Req_T* appCoapReq)
{
    otError            error = OT_ERROR_NONE;
    otMessage          *prequest;
    otMessageInfo      messageinfo;
    otInstance         *pinstance = APP_ThreadGetInstance();
    
    do
    {
        prequest = otCoapNewMessage(pinstance, NULL);
        if (prequest == NULL)
        {
            /* Add debug print if required.*/
            break;
        }
        
        otCoapMessageInit(prequest, appCoapReq->coapType, appCoapReq->coapCode);
        
        if(appCoapReq->coapUriPath != NULL)
        {
            error = otCoapMessageAppendUriPathOptions(prequest, (const char*)appCoapReq->coapUriPath);
            if(error != OT_ERROR_NONE)
            {
                /* Add debug print if required.*/
                break;
            }
        }
        
        if(appCoapReq->coapData != NULL)
        {
            //Set the payload delimiter in the message
            error = otCoapMessageSetPayloadMarker(prequest);
            if (error != OT_ERROR_NONE)
            { 
                break; 
            }
            
            //Append the payload to the message
            error = otMessageAppend(prequest, appCoapReq->coapData, appCoapReq->coapDataSize);
            if (error != OT_ERROR_NONE)
            {
                /* Add debug print if required.*/
                break; 
            }   
        }
        
        //Set the UDP-destination port of the CoAP-server
        memset(&messageinfo, 0, sizeof(messageinfo));
        messageinfo.mPeerPort = APP_COAP_PORT;
        
        memcpy(&messageinfo.mPeerAddr,otThreadGetRealmLocalAllThreadNodesMulticastAddress(pinstance),sizeof(otIp6Address));
        
        error = otCoapSendRequest(pinstance, prequest, &messageinfo, appCoapReq->coapResponseHandler, appCoapReq->coapContext);
        if (error != OT_ERROR_NONE)
        { 
            break; 
        }
    } while (false);
    
    if (error != OT_ERROR_NONE && prequest != NULL)
    {
        /* Add debug print if required.*/
        otMessageFree(prequest);
    } 
}

void APP_CoapResponseSend(APP_COAP_Resp_T* appCoapResp)
{
    otError      error = OT_ERROR_NONE;
    otMessage  * presponse = NULL;
    otInstance * pinstance = APP_ThreadGetInstance();
    
    do
    {
        presponse = otCoapNewMessage(pinstance, NULL);
        if (presponse == NULL)
        {
            break;
        }

        error = otCoapMessageInitResponse(presponse,
                                          appCoapResp->pmessage,
                                          appCoapResp->coapType,
                                          appCoapResp->coapCode);

        if (error != OT_ERROR_NONE)
        {
            break;
        }
        
        if(appCoapResp->coapData != NULL)
        {
            error = otCoapMessageSetPayloadMarker(presponse);
            if (error != OT_ERROR_NONE)
            {
                break;
            }
            
            error = otMessageAppend(presponse, appCoapResp->coapData, appCoapResp->coapDataSize);
            if (error != OT_ERROR_NONE)
            {
                break;
            }            
        }

        error = otCoapSendResponse(pinstance, presponse, appCoapResp->pmessageinfo);

    } while (false);

    if ((error != OT_ERROR_NONE) && (presponse != NULL))
    {
        otMessageFree(presponse);
    }
}

void APP_CoapAddResource(otCoapResource* coapResource)
{
    otInstance * pinstance = APP_ThreadGetInstance();
    if(coapResource->mContext == NULL)
    {
       coapResource->mContext =  pinstance;
    }
    
    otCoapAddResource(pinstance, coapResource);
}


/* *****************************************************************************
 End of File
 */
