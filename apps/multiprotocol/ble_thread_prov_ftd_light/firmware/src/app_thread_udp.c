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
#include "sensors/inc/rgb_led.h"


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

static void APP_Thread_ProcessUdpData(uint8_t* buffData)
{
    if((buffData[0] == 0) && (buffData[1] == 0) && (buffData[2] == 0))
    {
        RGB_LED_Off();
        SYS_CONSOLE_MESSAGE("[UDP] LED OFF\r\n");
        APP_TRPS_Sensor_BLE_light_onOff_Sync(false);
    }
    else
    {
        SYS_CONSOLE_MESSAGE("[UDP] COLOR SET\r\n");
        APP_TRPS_Sensor_HSV2XYL(buffData[0],buffData[1],buffData[2]);
        APP_TRPS_Sensor_BLE_light_Color_Sync(buffData[0],buffData[1]);
        APP_TRPS_Sensor_BLE_light_Brightness_Sync(buffData[2]);
        APP_TRPS_Sensor_BLE_light_onOff_Sync(true);
    }
}

static void APP_Thread_PrintTempData(uint8_t* buffData)
{
    uint16_t Temp    = (uint16_t)(buffData[0])<<8 | (uint16_t)(buffData[1]);
    float temperature = (float)Temp/10;
    SYS_CONSOLE_PRINT("[UDP] SED Temperature: %.2f DegC\n\r",temperature);
}

static void APP_Thread_ReceiveCb(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    uint16_t len = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);
    uint8_t output_buffer[len];
    char string[OT_IP6_ADDRESS_STRING_SIZE];
    otIp6AddressToString(&(aMessageInfo->mPeerAddr), string, OT_IP6_ADDRESS_STRING_SIZE);
    
    otMessageRead(aMessage,otMessageGetOffset(aMessage),output_buffer,len);
    SYS_CONSOLE_PRINT("[UDP] Received from %s\r\n", string);
    if(3 == len)
    {
        APP_Thread_ProcessUdpData(output_buffer);
    }
    else if(2 == len)
    {
        APP_Thread_PrintTempData(output_buffer);
    }
    else if(1 == len)
    {
       if(APP_LED_TOGGLE == output_buffer[0])
            APP_TRPS_Sensor_BLE_light_toggle(); 
    }
    else
    {
        /*Do Nothing*/
    }
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

static void APP_Thread_UdpBind(void) 
{
   otError err;
   otSockAddr addr;
   memset(&addr,0,sizeof(otSockAddr));
   addr.mPort = FTD_UDP_PORT_NO;
   otInstance * pinstance = APP_ThreadGetInstance();
   do
   {
        err = otUdpBind(pinstance, &aSocket, &addr, OT_NETIF_THREAD);
        if (err != OT_ERROR_NONE) {
            SYS_CONSOLE_PRINT("[UDP] Bind fail Err:%d\r\n",err);
            break;
        }
        SYS_CONSOLE_PRINT("[UDP] Listening on port %d\r\n",FTD_UDP_PORT_NO);
   }while(false);
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
    
    msgInfo.mPeerPort = MTD_UDP_PORT_NO;
    
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
    APP_Thread_UdpBind();
}


/* *****************************************************************************
 End of File
 */
