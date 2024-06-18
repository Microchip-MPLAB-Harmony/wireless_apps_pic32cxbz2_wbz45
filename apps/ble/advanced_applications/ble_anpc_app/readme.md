# ANPC Principal Application


## Table of Contents
- 1. Introduction
- 2. Contents
  - 2.1 Working Flow
  - 2.2 Testing
    - 2.2.1 Environment Setup
    - 2.2.2 Test Introduction
    - 2.2.3 Test Cases


## 1. Introduction
This application, which implements the profile Alert Notification Client, functions in the peripheral role. The document consists of two parts. The first part, **2.1 Working Flow**, describes the operational flow of this application. The second part, **2.2 Testing**, introduces the test environment and presents test cases.

## 2. Contents
### 2.1 Working Flow
State machine:

![image1](images/ble_anpc_app_image_state_machine.png)

| Description                                | Variable              | Value      | 
| ----------------------------------         | ---------------       | ---------- | 
| The timeout for advertisement without bond | timeout_adv           | 60 seconds | 
| The timeout for advertisement with bond    | timeout_with_bond_adv | 30 seconds | 


This application demonstrates how to simulate an ANPC device on the curiosity board which is **Device** in the state machine. There are the following states and their introductions behind in the state machine:

State machine:
- **APP_ANPC_STATE_IDLE**:          The application remains in idle mode.
- **APP_ANPC_STATE_ADV**:           The application is waiting for a new connection within timeout_adv seconds.
- **APP_ANPC_STATE_WITH_BOND_ADV**: The application is waiting for a reconnection within timeout_with_bond_adv seconds.
- **APP_ANPC_STATE_CONN**:          The application is prepared to send commands/receive notifications.

This application relies on Button 1 as the input and LEDs/UART as the output to have interactions with users. MPLAB® X Integrated Development Environment (IDE) provides a UART tool which is in Tools/Embedded/Simple Serial Port Terminal.
- Button 1:
  - Type of Pressing:
    - Press: Press the button for less than 500 ms.
    - Long press: Press the button for more than 500 ms.
  - Functions:

| State                            | Action              | Behavior                                                                                 | 
| :----------------------------    | :----------         | :----------                                                                              | 
| **APP_ANPC_STATE_CONN**          | Long press Button 1 | Disconnect the exiting connection, enable advertisement and allow new device to be bonded. | 
| **APP_ANPC_STATE_WITH_BOND_ADV** | Long press Button 1 | Enable advertisement and allow new device to be bonded.                                    | 
| **APP_ANPC_STATE_CONN**          | Press Button 1      | Enable New Alert and Unread Alert notification if they are disabled.                     | 
| **APP_ANPC_STATE_CONN**          | Press Button 1      | Disable New Alert and Unread Alert notification if they are enabled.                     | 


- LEDs indicate the current state according to the following table:

| APP Connection State         | LED behavior                                                           |
| ---------------------------  | ---------------------------------------------------------------------- |
| APP_ANPC_STATE_IDLE          | All LEDs are turned off.                                               |
| APP_ANPC_STATE_ADV           | Blue LED flashes once every 3 seconds. (On: 50 ms, Off: 2950 ms)       |
| APP_ANPC_STATE_WITH_BOND_ADV | Blue LED flashes twice every 3 seconds. (On: 50 ms, Off: 50 ms)        |
| APP_ANPC_STATE_CONN          | Blue LED flashes twice every 1.5 seconds. (On: 50 ms, Off: 150 ms)     |

- UART Settings:
  - Baud Rate: 115200 bps
  - Data: 8-bits
  - Parity: None
  - Stop bits: 1 bit
  - Flow Control: None
  - Character encoding: UTF-8

### 2.2 Testing
#### 2.2.1 Environment Setup
- In the following section, ANPS device and ANPS device 2 represent WBZ series curiosity board with Alert Notification server role application (ble_anps_app).
- In the following section, ANPC device represents WBZ series curiosity board with Alert Notification client role application (ble_anpc_app).

The WBZ series ANPC device is connected via Uart tool to the computer using the USB cable.
#### 2.2.2 Test Introduction
There are tests which are described below, please do them in order.

**Test 1**: Basically, the idea is to demonstrate:
- How to establish a connection and pair with the curiosity board.
- How to receive notifications and perform actions after establishing a connection.
- How to reconnect to the curiosity board after disconnecting.

**Test 2**: Basically, the idea is to demonstrate:
- How to allow a new device to be bonded with the curiosity board.

The LEDs behavior according to the "APP Connection State."

#### 2.2.3 Test Cases
**Test 1**:

1. On the computer, use the UART tool with the UART settings mentioned above.
2. On the computer, compile and program the ANPC application to the ANPC device.
    - On the ANPC device, plug in the USB cable to enter APP_ANPC_STATE_ADV state and wait for a new connection within timeout_adv seconds.
3. On the computer, compile and program the ANPS application to the ANPS device.
    - On the ANPS device, plug in the USB cable to power on and establish a new connection with the ANPC device.
4. On the ANPC device, it enters the APP_ANPC_STATE_CONN state, indicating that a connection has been established.
5. On the computer, the following message is printed to the UART tool:
    - BLE_ANPC_EVT_WRITE_NEW_ALERT_NTFY_RSP_IND_ENABLE
    - BLE_ANPC_EVT_WRITE_UNREAD_ALERT_NTFY_RSP_IND_ENABLE
6. On the ANPS device, press Button 1 to send the New Alert Notification. (A notification will be sent as New or Unread in the order the button is pressed.)
7.  On the ANPC device, the New Alert notification is received.
    - On the computer, the following message is printed to the UART tool:
        - BLE_ANPC_EVT_NEW_ALERT_IND
        - connHandle: 0071
        - categoryId: 00
        - numOfNewAlert: 02
        - receivedLength: 000b
        - txtStr: Mark.Smith 
8. On the ANPS device, press Button 1 to send the Unread Alert Notification. (A notification will be sent as New or Unread in the order the button is pressed.)
9.  On the ANPC device, the Unread Alert notification is received.
    - On the computer, the following message is printed to the UART tool:
        - BLE_ANPC_EVT_UNREAD_ALERT_IND
        - connHandle: 0071
        - categoryId: 00
        - unreadCnt: 03
10.	On the ANPC device, press Button 1 to disable the CCCD (Client Characteristic Configuration Descriptor) for New/Unread Alert on the ANPS device.
11. On the computer, the following message is printed to the UART tool:
    - BLE_ANPC_EVT_WRITE_NEW_ALERT_NTFY_RSP_IND_DISABLE
    - BLE_ANPC_EVT_WRITE_UNREAD_ALERT_NTFY_RSP_IND_DISABLE
12.	On the ANPS device, press Button 1 to send the New/Unread Alert Notification. (A notification will be sent as New or Unread in the order the button is pressed.)
13.	On the ANPC device, the New/Unread Alert Notification are not received.
14. On the ANPC device, unplug the USB cable to turn off the power.
15. On the ANPC device, plug in the USB cable to turn on the power.
    - The ANPC device enters the APP_ANPC_STATE_WITH_BOND_ADV state and waits for a reconnection  within timeout_with_bond_adv seconds.
16. On the ANPC device, it enters the APP_ANPC_STATE_CONN state, indicating that a connection has been established.

**Test 2**:

17. On the ANPS device, unplug the USB cable to turn off the power.
18. On the computer, compile and program the ANPS application to the ANPS device 2.
    - On the ANPS device 2, plug in the USB cable to power on and establish a new connection with the ANPC device.
19. On the ANPC device, it enters the APP_ANPC_STATE_WITH_BOND_ADV state, it is waiting for reconnection with the ANPS device, thus it will not attempt to reconnect with the ANPS device 2.
20. On the ANPC device, long press Button 1 to enter the APP_ANPC_STATE_ADV state and wait for a new connection within timeout_adv seconds.
21. On the ANPC device, it enters the APP_ANPC_STATE_CONN state, indicating that a connection has been established with ANPS device 2.