/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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
  Application Thread Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_thread.c

  Summary:
    This file contains the Application Thread Source code for this project.

  Description:
    This file contains the Application Thread Source code for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include "definitions.h"
#include "configuration.h"

#include "openthread/udp.h"
#include "openthread/message.h"
#include "openthread/ip6.h"
#include "openthread/instance.h"
#include "openthread/error.h"
#include "openthread/thread.h"
#include "app_thread/app_thread_common.h"
#include "app_thread.h"
#include "app_thread_udp.h"
#include "app_ble_sensor.h"
#include "app_timer.h"


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
static otUdpSocket aSocket;


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void APP_Thread_ReceiveCb(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    uint16_t len = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
    uint8_t output_buffer[len];
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString(&(aMessageInfo->mPeerAddr), string, OT_IP6_ADDRESS_STRING_SIZE);
    
    otMessageRead(aMessage,otMessageGetOffset(aMessage),output_buffer,len);
    SYS_CONSOLE_PRINT("[UDP] Received from %s\r\n", string);
}

static void APP_Thread_UdpOpen(void) 
{
   otError err;
   otInstance * pinstance = APP_ThreadGetInstance();
   err = otUdpOpen(pinstance, &aSocket, APP_Thread_ReceiveCb, NULL);
   if (err != OT_ERROR_NONE)
   {
      SYS_CONSOLE_MESSAGE("[UDP] Open failed\r\n");
   }
}



// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
void APP_Thread_UdpSend(uint8_t* cmd,uint16_t cmdLen)
{
    otError err = OT_ERROR_NONE;
    otMessageInfo msgInfo;
    const otIp6Address *mPeerAddr;
    memset(&msgInfo,0,sizeof(msgInfo));
    otInstance * pinstance = APP_ThreadGetInstance();
//    otIp6AddressFromString("ff03::1",&msgInfo.mPeerAddr);
    
    mPeerAddr = otThreadGetRealmLocalAllThreadNodesMulticastAddress(pinstance);
    memcpy(&msgInfo.mPeerAddr, mPeerAddr, OT_IP6_ADDRESS_SIZE);
    
    msgInfo.mPeerPort = FTD_UDP_PORT_NO;
    
    do {
        otMessage *udp_msg = otUdpNewMessage(pinstance,NULL);
        err = otMessageAppend(udp_msg,cmd,cmdLen);
        if(err != OT_ERROR_NONE)
        {
            SYS_CONSOLE_MESSAGE("[UDP] Message Add fail\r\n");
            break;
        }
        
        err = otUdpSend(pinstance,&aSocket,udp_msg,&msgInfo);
        if(err != OT_ERROR_NONE)
        {
            SYS_CONSOLE_MESSAGE("[UDP] Send fail\r\n");
            break;
        }

    }while(false);
}

void APP_Thread_UdpInitData(void)
{
    APP_Thread_UdpOpen();
}


/* *****************************************************************************
 End of File
 */