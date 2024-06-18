# ANPS Principal Application

## Table of Contents
- 1. Introduction
- 2. Contents
  - 2.1 Working Flow
  - 2.2 Testing
    - 2.2.1 Environment Setup
    - 2.2.2 Test Introduction
    - 2.2.3 Test Cases


## 1. Introduction
This application, which implements the profile Alert Notification Server, functions in the central role. The document consists of two parts. The first part, **2.1 Working Flow**, describes the operational flow of this application. The second part, **2.2 Testing**, introduces the test environment and presents test cases. In this document, the term "curiosity board" refers to the WBZ series curiosity board.

## 2. Contents
### 2.1 Working Flow
State machine:

![image1](images/ble_anps_app_image_state_machine.png)

This application demonstrates how to simulate an ANPS device on the curiosity board. There are the following states and their introductions behind in the state machine:


| Description                           | Variable               | Value      | 
| ----------------------------------    | ---------------        | ---------- | 
| The timeout for scanning without bond | timeout_scan           | 60 seconds | 
| The timeout for scanning with bond    | timeout_with_bond_scan | 30 seconds | 

State machine:
- **APP_ANPS_STATE_IDLE**:            The application remains in idle mode.
- **APP_ANPS_STATE_SCAN**:            The application is prepared to establish a new connection within timeout_scan seconds. 
- **APP_ANPS_STATE_WITH_BOND_SCAN**:  The application is prepared to reconnect within timeout_with_bond_scan seconds.
- **APP_ANPS_STATE_CONNECTING**:      The application is currently in the process of establishing a connection.
- **APP_ANPS_STATE_CONNECTED**:       The application is prepared to send commands notifications.

This application relies on Button 1 as the input and LEDs as the output to have interactions with users.
- Button 1:
  - Type of Pressing:
    - Press: Press the button for less than 500 ms.
    - Long press: Press the button for more than 500 ms.
  - Functions:

| State                         | Action              | Behavior                                                        | 
| :-------------------------    | :----------         | :----------                                                     | 
| **APP_ANPS_STATE_WITH_BOND_SCAN** | Long press Button 1 | Connect to a new device.                                        | 
| **APP_ANPS_STATE_CONNECTED**      | Long press Button 1 | Disconnect the existing connection and connect to a new device. | 
| **APP_ANPS_STATE_CONNECTED**      | Press Button 1      | Send the New Alert/Unread Alert notification. (A notification will be sent as New or Unread in the order the button is pressed.)| 


- LEDs indicate the current state according to the following table:

| APP Connection State          | LED behavior                                                           | 
| ---------------------------   | ---------------------------------------------------------------------- | 
| APP_ANPS_STATE_IDLE           | All LEDs are turned off.                                               | 
| APP_ANPS_STATE_SCAN           | Blue LED flashes once every 3 seconds. (On: 50 ms, Off: 2950 ms)       | 
| APP_ANPS_STATE_WITH_BOND_SCAN | Blue LED flashes twice every 3 seconds. (On: 50 ms, Off: 50 ms)        | 
| APP_ANPS_STATE_CONNECTED      | Blue LED flashes twice every 1.5 seconds. (On: 50 ms, Off: 150 ms)     | 

### 2.2 Testing
#### 2.2.1 Environment Setup
- In the following section, ANPS device and ANPS device 2 represent WBZ series curiosity board with Alert Notification server role application (ble_anps_app).
- In the following section, ANPC device represents WBZ series curiosity board with Alert Notification client role application (ble_anpc_app).
#### 2.2.2 Test Introduction
A test which is described below, please do them in order.

**Test 1**: Basically, the idea is to demonstrate:
- How to establish a connection and pair with the curiosity board.
- How to send notifications and perform actions after establishing a connection.
- How to reconnect the curiosity board after disconnecting.

**Test 2**: Basically, the idea is to demonstrate:
- How to allow a new device to be bonded with the curiosity board

The LEDs behavior according to the "APP Connection State."
#### 2.2.3 Test Cases
**Test 1**:

1.  On the computer, use the UART tool with the UART settings to the ANPC device.
    - UART Settings:
      - Baud Rate: 115200 bps
      - Data: 8-bits
      - Parity: None
      - Stop bits: 1 bit
      - Flow Control: None
      - Character encoding: UTF-8
2.  On the computer, compile and program the ANPC application to the ANPC device.
      - On the ANPC device, plug in the USB cable, and wait for a new connection.
3.  On the computer, compile and program the ANPS application to the ANPS device.
      - On the ANPS device, plug in the USB cable to enter APP_ANPS_STATE_SCAN state and establish a new connection with the ANPC device within timeout_scan seconds.
4.  On the ANPS device, it enters the APP_ANPS_STATE_CONNECTED state, indicating that a connection has been established.
5.  On the ANPS device, press Button 1 to send the New Alert notification. (A notification will be sent as New or Unread in the order the button is pressed.)
6.  On the ANPC device, the New Alert notification will be received if the Client Characteristic Configuration Descriptor (CCCD) is enabled for New/Unread Alert on the ANPS device.
    - On the computer, the following message is printed to the UART tool:
        - BLE_ANPC_EVT_NEW_ALERT_IND
        - connHandle: 0071
        - categoryId: 00
        - numOfNewAlert: 02
        - receivedLength: 000b
        - txtStr: Mark.Smith 
7.  On the ANPS device, press Button 1 to send the Unread Alert notification. (A notification will be sent as New or Unread in the order the button is pressed.)  
8.  On the ANPC device, the Unread Alert notification will be received if the Client Characteristic Configuration Descriptor (CCCD) is enabled for New/Unread Alert on the ANPS device.
    - On the computer, the following message is printed to the UART tool:
        - BLE_ANPC_EVT_UNREAD_ALERT_IND
        - connHandle: 0071
        - categoryId: 00
        - unreadCnt: 03
9. On the ANPS device, unplug the USB cable to turn off the power.
10. On the ANPC device, connection is disconnected, it will prepare to be reconnected.
11. On the ANPS device, plug in the USB cable to turn on the power.
    - The ANPS device enters the APP_ANPS_STATE_WITH_BOND_SCAN state and prepares to reconnect within timeout_with_bond_scan seconds.
12. On the ANPS device, it enters the APP_ANPS_STATE_CONNECTED state, indicating that a connection has been established.

**Test 2**:

13. On the ANPS device, unplug the USB cable to turn off the power.
14. On the computer, compile and program the ANPS application to the ANPS device 2.
    - On the ANPS device 2, plug in the USB cable to enter APP_ANPS_STATE_SCAN state and establish a new connection with the ANPC device within timeout_scan seconds.
15. On the ANPC device, it is awaiting reconnection with the ANPS device, thus it will not attempt to reconnect with the ANPS device 2.
16. On the ANPC device, long press Button 1 to wait for a new connection.
17. On the ANPS device 2, it enters the APP_ANPS_STATE_CONNECTED state, indicating that a connection has been established.