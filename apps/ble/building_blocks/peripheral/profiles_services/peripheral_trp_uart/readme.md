# BLE Transparent UART

[Getting Started](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-17DABF04-E5D8-4201-A746-2FC244450A19)

[Getting Started with Peripheral Building Blocks](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-B3B46369-F5B4-401B-8405-658BE34988F4)

[BLE Connection](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-F9A0C390-C124-49A7-9F22-157D20BFBE5D) **--\>** [BLE Transparent UART](#GUID-07EC83B7-CC28-4C55-8634-4B7F56A9DB36)

## Introduction {#INTRODUCTION .section}

This tutorial will help users create a peripheral device and send/receive characters between 2 connected BLE devices over Microchip proprietary Transparent UART Profile. Peripheral device will be WBZ451 Device and Central device can either be a Smartphone with Light Blue App or another WBZ451 Device.The instructions mentioned below are applicable for a BLE Peripheral device.

Users of this document can choose to just run the precompiled Application Example hex file on the WBZ451 Curiosity board and experience the demo or can go through the steps involved in developing this Application from scratch.

These examples each build on top on one and other. We strongly recommend that you follow the examples in order, to learn the basics concepts before progressing to the more advanced topics.

## Recommended Reads {#RECOMMENDED-READS .section}

1.  [BLE Software Specification](https://onlinedocs.microchip.com/pr/GUID-C5EAF60E-9124-427C-A0F1-F2DBE662EA92-en-US-2/index.html?GUID-222749FE-01C5-43B6-A5C7-CD82B3FC7F5F)

2.  [BLE Connection\(peripheral\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-F9A0C390-C124-49A7-9F22-157D20BFBE5D)


## Hardware Required {#HARDWARE-REQUIRED .section}

|**Tool**|**Qty**|
|--------|-------|
|**WBZ451 Curiosity Board**|1|
|Micro USB cable|1|

## SDK Setup {#SDK-SETUP .section}

1.  [Getting Started with Software Development](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-2AD37FE2-1915-4E34-9A05-79E3810726D7)


## Software {#SOFTWARE .section}

1.  [TeraTerm](https://ttssh2.osdn.jp/index.html.en)


## Smartphone App {#SMARTPHONE-APP .section}

1.  Light Blue


## Programming the precompiled hex file or Application Example {#PROGRAMMING-THE-PRECOMPILED-HEX-FILE-OR-APPLICATION-EXAMPLE .section}

**Programming the hex file using MPLABX IPE**

1.  Precompiled Hex file is located in "<Harmony Content Path\>\\wireless\_apps\_pic32cxbz2\_wbz45\\apps\\ble\\building\_blocks\\peripheral\\profiles\_services\\peripheral\_trp\_uart\\hex" folder

2.  Follow the steps mentioned [here](https://microchipdeveloper.com/ipe:programming-device)


**Caution:** Users should choose the correct Device and Tool information

**Programming the Application using MPLABX IDE**

1.  Follow steps mentioned in of [Running a Precompiled Example](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-EA74172C-595E-4A34-B359-D42EE443F0EC) document

2.  Open and program the Application Example "peripheral\_trp\_uart.x" located in "<Harmony Content Path\>\\wireless\_apps\_pic32cxbz2\_wbz45\\apps\\ble\\building\_blocks\\peripheral\\profiles\_services\\peripheral\_trp\_uart\\firmware" using MPLABX IDE


<Harmony Content Path\> [how to find what is my Harmony Content Path](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-A55E9342-CE44-4A91-86BB-FEC6706FCD1C)

## Demo Description {#DEMO-DESCRIPTION .section}

Upon programming the demo application, WBZ451 will start Advertising \(connectable\), central device \(Smartphone or another WBZ451\) scanning for these advertisements will connect to the device. In case of using Light Blue App search for “Microchip” and connect. After a connection has been made data can be sent back and forth over UART between the two devices that are connected. Demo will print start of the advertisement “Advertising”, connection “BLE Scanning”, “Connected” and “Disconnected” state on a terminal emulator like TeraTerm @ \(Speed: 115200, Data: 8-bit, Parity: none, stop bits: 1 bit, Flow control: none\). Application Data to be sent to the connected central device \(Smartphone or another WBZ451\) should be entered in the terminal emulator.

## Testing {#TESTING .section}

This section assumes that user has programmed the Application Example on the WBZ451 Curiosity Board **Demo Experience when using a Smartphone \(Light Blue App\) as Central Device**

-   Reset the WBZ451 Curiosity board, Open Terminal emulator like Tera Term, select the right COM port@ \(Speed: 115200, Data: 8-bit, Parity: none, stop bits: 1 bit, Flow control: none\).

    ![](media/GUID-8E187215-BC27-44CE-80EB-EBE8EC8E46A0-low.png)

-   open Light Blue App on your smartphone, Search and select the advertisement with Device Name "Microchip"

    ![](media/GUID-60DC8E12-5567-4A74-B46E-A6021FB7FFAC-low.png)

-   To receive data from WBZ451 Curiosity Board \(peripheral\) device to central device, users need to select the UUID: 49535343-1E4D-4BD9-BA61-23C647249616 and select listen for notifications

    ![](media/GUID-BEC9A855-FFD9-4FDA-A24D-00E3DE5C5058-low.png)

-   Enter "test" on Teraterm and data should be displayed on Light Blue App

    ![](media/GUID-A719E983-89F4-48A1-A78F-39DBDDD78930-low.png)

-   To send data from central device to WBZ451 Curiosity Board \(peripheral\), users need to select the UUID: 49535343-8841-43F4-A8D4-ECBE34729BB3 and select write new value

-   Enter "trp uart" on App and data should be displayed on Terminal Window

    ![](media/GUID-194DB011-4CB0-45CC-86B3-D8265D7C85BA-low.png)


**Demo Experience when using another WBZ45x as Central device** Users can use another WBZ451 Curiosity Board configured as [BLE Transparent UART\(central\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-7A350957-2010-468F-AF8D-0635373EF79D) instead of using a Smartphone App as central device

## Developing this Application from scratch using MPLAB Code Configurator {#DEVELOPING-THIS-APPLICATION-FROM-SCRATCH-USING-MPLAB-CODE-CONFIGURATOR .section}

This section explains the steps required by a user to develop this application example from scratch using MPLABx Code Configurator

**Tip:** New users of MPLAB Code Configurator are recommended to go through the [overview](https://onlinedocs.microchip.com/pr/GUID-1F7007B8-9A46-4D03-AEED-650357BA760D-en-US-6/index.html?GUID-B5D058F5-1D0B-4720-8649-ACE5C0EEE2C0).

1.  Create a new MCC Harmony Project -- [link](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-B86E8493-D00D-46EF-8624-D412342147F0) for instructions

2.  Import component configuration --This step helps users setup the basic components and configuration required to develop this application. The imported file is of format .mc3 and is located in the path "<Harmony Content Path>\wireless_apps_pic32cxbz2_wbz45\apps\ble\building_blocks\peripheral\profiles_services\peripheral_trp_uart\firmware\peripheral_trp_uart.X". Users should follow the instructions mentioned [here](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-F8FE2886-8A2C-4FC0-9956-C094CE44D162) to import the component configuration.

3.  Accept Dependencies or satisfiers, select "Yes"

4.  Verify if the Project Graph window has all the expected configuration

    ![](media/GUID-F1235AA6-65E5-4085-BE15-19CCBAEFA131-low.png)


## Verify Advertisement,Connection and Transparent UART Profile Configuration {#VERIFY-ADVERTISEMENTCONNECTION-AND-TRANSPARENT-UART-PROFILE-CONFIGURATION .section}

1.  Select **BLE\_Stack** component in project graph

    ![](media/GUID-63653B1F-5E94-49F5-A60E-26EB3B0C56B3-low.png)

2.  Select **Transparent Profile** component in project graph

    ![](media/GUID-A4F1DC5F-4E2B-471C-9FFA-2871E5C1DB49-low.png)


## Generate Code {#GENERATE-CODE-LINK-FOR-INSTRUCTIONS .section}

Instructions on[how to Generate Code](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-9C28F407-4879-4174-9963-2CF34161398E)

## Files and Routines Automatically generated by the MCC {#FILES-AND-ROUTINES-AUTOMATICALLY-GENERATED-BY-THE-MCC .section}

After generating the program source from MCC interface by clicking Generate Code, the BLE configuration can be found in the following project directories

![](media/GUID-3DDA4F46-954F-4D03-87F2-E96FF3899E20-low.png)

The [OSAL](http://ww1.microchip.com/downloads/en/DeviceDoc/MPLAB%20Harmony%20OSAL%20Libraries%20Help%20v2.06.pdf), RF System, BLE System initialization routine executed during program initialization can be found in the project files. This initialization routine is automatically generated by the MCC

![](media/GUID-A8152D7D-0C8E-4C1D-89FB-7EFF9979AB30-low.png)

The BLE stack initialization routine excuted during Application Initialization can be found in project files. This intitialization routine is automatically generated by the MCC. This call initializes and configures the GAP, GATT, SMP, L2CAP and BLE middleware layers.

![](media/GUID-F39EFA99-5AC5-4EA4-964E-7B36EAD29112-low.png "")

Autogenerated, advertisement data format

![](media/GUID-6FD5F59B-0164-4C4B-A0D4-2909AB8E4D2A-low.png)

|**Source Files**|**Usage**|
|----------------|---------|
|app.c|Application State machine, includes calls for Initialization of all BLE stack \(GAP,GATT, SMP, L2CAP\) related component configurations|
|app\_ble\\app\_ble.c|Source Code for the BLE stack related component configurations, code related to function calls from app.c|
|app\_ble\\app\_ble\_handler.c|All GAP, GATT, SMP and L2CAP Event handlers|
|app\_ble\\app\_trsps\_handler.c|All Transparent UART Server related Event handlers|
|ble\_trsps.c|All Transparent Server Functions for user application|

> **Tip:** app.c is autogenerated and has a state machine based Application code sample, users can use this template to develop their application

**Header Files**

-   ble\_gap.h- This header file contains BLE GAP functions and is automatically included in the app.c file

-   ble\_trsps.h is the Header File associated with API’s and structures related to BLE Transparent Client functions for Application User


**Function Calls**

MCC generates and adds the code to initialize the BLE Stack GAP, GATT, L2CAP and SMP in *APP\_BleStackInit\(\)* function

-   APP\_BleStackInit\(\) is the API that will be called inside the Applications Initial State -- APP\_STATE\_INIT in app.c


## User Application Development {#USER-APPLICATION-DEVELOPMENT .section}

**Include**

-   "ble\_trsps.h" in app.c, BLE Transparent UART Server related API's are available here

-   "osal/osal\_freertos\_extend.h" in app\_trsps\_handler.c, OSAL related API's are available here

-   definitions.h in all the files where UART will be used to print debug information Tip: definitions.h is not specific to just UART peripheral, instead it should be included in all application source files where peripheral functionality will be exercised

-   user action is required as mentioned [here](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-99583057-8B1A-42F2-84E8-CFC7717FA5D1)


**Set PUBLIC Device Address**

-   BLE\_GAP\_SetDeviceAddr\(&devAddr\);


```
    BLE_GAP_Addr_T devAddr;
    devAddr.addrType = BLE_GAP_ADDR_TYPE_PUBLIC;
    devAddr.addr[0] = 0xA1;
    devAddr.addr[1] = 0xA2;
    devAddr.addr[2] = 0xA3;
    devAddr.addr[3] = 0xA4;
    devAddr.addr[4] = 0xA5;
    devAddr.addr[5] = 0xA6;

    // Configure device address
    BLE_GAP_SetDeviceAddr(&devAddr);
```

![](media/GUID-B8535FB6-AECD-4582-B154-936932E5CC0C-low.png)

**Start Advertisement**

-   BLE\_GAP\_SetAdvEnable\(0x01, 0\);


![](media/GUID-D3C30BCF-F2A2-4A49-88AB-75608A354E3D-low.png)

**Connected & Disconnected Events**

-   In app\_ble\_handler.c BLE\_GAP\_EVT\_CONNECTED event will be generated when a BLE connection is completed


**Connection Handler**

-   Connection handle associated with the peer peripheral device needs to be saved for data exchange after a BLE connection

-   p\_event-\>eventField.evtConnect.connHandle has this information

    ![](media/GUID-47448003-978B-45DF-A12C-281EE7716419-low.png)


**Transmit Data**

-   BLE\_TRSPS\_SendData\(conn\_hdl , 1, &data\); is the API to be used for sending data towards the central device **Note:** The precompiled application example uses a UART callback to initiate the data transmission upon receiving a character on UART


Example Implementation for Transmitting the received data over UART using the BLE\_TRSPS\_SendData API

```
uint16_t conn_hdl;// connection handle info captured @BLE_GAP_EVT_CONNECTED event
uint16_t ret;
uint8_t uart_data;
void uart_cb(SERCOM_USART_EVENT event, uintptr_t context)
{
  APP_Msg_T   appMsg;  
  // If RX data from UART reached threshold (previously set to 1)
  if( event == SERCOM_USART_EVENT_READ_THRESHOLD_REACHED )
  {
    // Read 1 byte data from UART
    SERCOM0_USART_Read(&uart_data, 1);
    appMsg.msgId = APP_MSG_UART_CB;
    OSAL_QUEUE_Send(&appData.appQueue, &appMsg, 0);     
  }
}

void APP_UartCBHandler()
{
    // Send the data from UART to connected device through Transparent service
    BLE_TRSPS_SendData(conn_hdl, 1, &uart_data);      
}
  // Register call back when data is available on UART for Peripheral Device to send
// Enable UART Read
SERCOM0_USART_ReadNotificationEnable(true, true);
// Set UART RX notification threshold to be 1
SERCOM0_USART_ReadThresholdSet(1);
// Register the UART RX callback function
SERCOM0_USART_ReadCallbackRegister(uart_cb, (uintptr_t)NULL);
```

![](media/GUID-9CB438E4-2601-4FE5-8A5D-9DB3AC81AC96-low.png "")

![](media/GUID-2CE7F5BD-CA7A-475E-817A-EA16688B80DE-low.png "")

```
else if(p_appMsg->msgId==APP_MSG_BLE_STACK_LOG)
{
   // Pass BLE LOG Event Message to User Application for handling
   APP_BleStackLogHandler((BT_SYS_LogEvent_T *)p_appMsg->msgData);
}
else if(p_appMsg->msgId==APP_MSG_UART_CB)
{
   // Pass BLE UART Data transmission target BLE UART Device handling
   APP_UartCBHandler();
}
```

![](media/GUID-275EE509-D2E9-490F-9785-330C2CEF0818-low.png "")

**Receive Data**

-   BLE\_TRSPS\_EVT\_RECEIVE\_DATA is the event generated when data is sent from central device

-   Users need to use the BLE\_TRSPS\_GetDataLength\(p\_event-\>eventField.onReceiveData.connHandle, &data\_len; API to extract the length of application data received

-   BLE\_TRSPS\_GetData\(p\_event-\>eventField.onReceiveData.connHandle, data\); API is used to retrieve the data


**Tip:** BLE\_TRSPC\_Event\_T p\_event structure stores the information about BLE transparent UART callback functions

Example Implementation for printing the received data from central device over UART

```
  /* TODO: implement your application code.*/
  uint16_t data_len;
  uint8_t *data;
  // Retrieve received data length
  BLE_TRSPS_GetDataLength(p_event->eventField.onReceiveData.connHandle, &data_len);
  // Allocate memory according to data length
  data = OSAL_Malloc(data_len);
  if(data == NULL)
  break;
  // Retrieve received data
  BLE_TRSPS_GetData(p_event->eventField.onReceiveData.connHandle, data);
  // Output received data to UART
  SERCOM0_USART_Write(data, data_len);
  // Free memory
  OSAL_Free(data);
```

![](media/GUID-2020B383-BE4A-4648-BB96-1E732A38243B-low.png)

Users can exercise various other BLE functionalities by using[BLE Stack API](https://onlinedocs.microchip.com/pr/GUID-C5EAF60E-9124-427C-A0F1-F2DBE662EA92-en-US-2/index.html)

## Where to go from here {#WHERE-TO-GO-FROM-HERE .section}

-   [Advanced Application \(BLE Sensor App\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-D3F1BF46-F275-409B-A344-4DB9EAABC5E9) - BLE Sensor App utlizes the Transparent UART building block


**Parent topic:**[Peripheral](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-B3B46369-F5B4-401B-8405-658BE34988F4)
